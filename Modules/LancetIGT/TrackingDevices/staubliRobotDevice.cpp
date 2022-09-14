#include "staubliRobotDevice.h"

#include "mitkIGTTimeStamp.h"

#include "lancetstaubliTrackingDeviceTypeInformation.h"

#define d2r 57.2957795130

bool StaubliRobotDevice::OpenConnection()
{
	m_RobotApi.connectrobot();
	if (GetState() == Ready)
	{
		MITK_WARN << "Ready after m_RobotApi.connectrobot();";
	}
	else {
		MITK_WARN << "Setup after m_RobotApi.connectrobot();";
	}

	//todo BUG:isRobotConnected will not turn ture immdiately
	SetState(TrackingDeviceState::Ready);
	m_Heartbeat = QThread::create(heartbeatThreadWorker, this);
	m_Heartbeat->start();
	return true;
}

bool StaubliRobotDevice::CloseConnection()
{
	m_RobotApi.disconnectrobot();
	SetState(TrackingDeviceState::Setup);
	return !m_IsConnected;
}

bool StaubliRobotDevice::StartTracking()
{
	if (this->GetState() != Ready)
		return false;

	this->SetState(Tracking);          // go to mode Tracking
	this->m_StopTrackingMutex.lock(); // update the local copy of m_StopTracking
	this->m_StopTracking = false;
	this->m_StopTrackingMutex.unlock();

	m_Thread = std::thread(&StaubliRobotDevice::ThreadStartTracking, this);//todo start tracking agine crash here 
	// start a new thread that executes the TrackTools() method
	mitk::IGTTimeStamp::GetInstance()->Start(this);
	MITK_INFO << "lancet staubli robot start tracking";
	return true;
}

mitk::TrackingTool* StaubliRobotDevice::GetTool(unsigned toolNumber) const
{
	std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
	if (toolNumber < m_StaubliEndEffectors.size())
		return m_StaubliEndEffectors.at(toolNumber);
	return nullptr;
}

mitk::TrackingTool* StaubliRobotDevice::GetToolByName(std::string name) const
{
	std::lock_guard<std::mutex> lock(m_ToolsMutex); // lock and unlock the mutex
	auto end = m_StaubliEndEffectors.end();
	for (auto iterator = m_StaubliEndEffectors.begin(); iterator != end; ++iterator)
		if (name.compare((*iterator)->GetToolName()) == 0)
			return *iterator;
	return nullptr;
}

mitk::TrackingTool* StaubliRobotDevice::GetInternalTool()
{
	return m_StaubliEndEffectors[0]; //only 1 end effector tracked;
}

mitk::TrackingTool* StaubliRobotDevice::AddTool(const char* toolName, const char* fileName)
{
	mitk::TrackingTool::Pointer t = mitk::TrackingTool::New();
	t->SetToolName(toolName);
	if (this->InternalAddTool(t) == false)
	{
		MITK_ERROR << "InternalAddTool failed";
		return nullptr;
	}

	return t.GetPointer();
}

unsigned StaubliRobotDevice::GetToolCount() const
{
	return static_cast<unsigned int>(this->m_StaubliEndEffectors.size());
}

void StaubliRobotDevice::TrackTools()
{
	m_RobotApi.requestrealtimedata();
	m_TrackingData[0] = m_RobotApi.realtime_data.pose.x;
	m_TrackingData[1] = m_RobotApi.realtime_data.pose.y;
	m_TrackingData[2] = m_RobotApi.realtime_data.pose.z;

	m_TrackingData[3] = m_RobotApi.realtime_data.pose.rx;
	m_TrackingData[4] = m_RobotApi.realtime_data.pose.ry;
	m_TrackingData[5] = m_RobotApi.realtime_data.pose.rz;
}

std::array<double, 6> StaubliRobotDevice::GetTrackingData()
{
	return m_TrackingData;
}

bool StaubliRobotDevice::RequestExecOperate(const QString& funname, const QStringList& param)
{
	MITK_INFO << "ExecOperateFunc " << funname.toStdString();
	for (auto string : param)
	{
		MITK_INFO << "ParamList " << string.toStdString();
	}
	if (funname.indexOf("movep") != -1 && param.size() == 6)
	{
		QThread::msleep(150);
		this->m_RobotApi.movep(param.at(0).toDouble(),
			param.at(1).toDouble(),
			param.at(2).toDouble(),
			param.at(3).toDouble(),
			param.at(4).toDouble(),
			param.at(5).toDouble());
		return true;
	}
	if (funname.indexOf("movej") != -1 && param.size() == 6)
	{
		QThread::msleep(150);
		this->m_RobotApi.movej(param.at(0).toDouble(),
			param.at(1).toDouble(),
			param.at(2).toDouble(),
			param.at(3).toDouble(),
			param.at(4).toDouble(),
			param.at(5).toDouble());
		return true;
	}
	else if (funname.indexOf("movel") != -1 && param.size() == 6)
	{
		QThread::msleep(150);
		this->m_RobotApi.movel(param.at(0).toDouble(),
			param.at(1).toDouble(),
			param.at(2).toDouble(),
			param.at(3).toDouble(),
			param.at(4).toDouble(),
			param.at(5).toDouble());
		return true;
	}
	//else if (funname.indexOf("setworkmode") != -1 && param.size() == 1)
	//{
	//	QThread::msleep(300);
	//	this->m_RobotApi.setworkmode(param.at(0).toInt());
	//	return true;
	//}
	//else if (funname.indexOf("setTcpNum") != -1 && param.size() == 2)
	//{
	//	QThread::msleep(300);
	//	this->m_RobotApi.setTcpNum(param.at(0).toInt(), param.at(1).toInt());
	//	return true;
	//}
	//else if (funname.indexOf("setio") != -1 && param.size() == 2)
	//{
	//	this->m_RobotApi.setio(param.at(0).toInt(), param.at(1).toInt());
	//	return true;
	//}
	//else if (funname.indexOf("update") != -1 && param.size() == 0)
	//{
	//	//emit this->(this->devicename(), this->realtimeData());
	//	MITK_ERROR << "not support";
	//	return false;
	//}
	//else if (funname.toLower().indexOf("applytcpvalue") != -1 && param.size() == 6)
	//{
	//	qInfo() << "call movel function " << this->RequestExecOperate("movel", param) << " param " << param;
	//	QThread::msleep(1000);
	//	qInfo() << "call setworkmode 1-11 function " << this->RequestExecOperate("setworkmode", { "11" });
	//	QThread::msleep(1000);
	//	qInfo() << "call setworkmode 1-5 function " << this->RequestExecOperate("setworkmode", { "5" });
	//	QThread::msleep(1000);
	//	return true;
	//}
	return false;
}

StaubliRobotDevice::StaubliRobotDevice()
	:TrackingDevice()
{
	m_Data = lancet::StaubliRobotTypeInformation::GetDeviceDataLancetStaubliTrackingDevice();
	m_StaubliEndEffectors.clear();
	//udp service
	m_udp.setRepetitiveHeartbeatInterval(500);
	m_udp.setRemoteHostPort(m_RemotePort.toUInt());
	m_udp.setRemoteHostAddress(QString::fromStdString(m_RemoteIpAddress));
	if (!m_udp.bind(QHostAddress(QString::fromStdString(m_IpAddress)), m_Port.toInt()))
	{
		MITK_ERROR << QString("bind to %1:%2 error!- %3").arg(QString::fromStdString(m_IpAddress)).arg(m_Port.toInt()).arg(m_udp.error()).toStdString();
	}
	MITK_INFO << QString("bind udp %1:%2 at fps:%3").arg(QString::fromStdString(m_IpAddress)).arg(m_Port.toInt()).arg(m_udp.repetitiveHeartbeatInterval()).toStdString();
	m_udp.startRepetitiveHeartbeat();

	//connect(&m_RobotApi, SIGNAL(signal_api_isRobotConnected(bool)),
	 // this, SLOT(IsRobotConnected(bool)));//todo BUGFIX
}

StaubliRobotDevice::~StaubliRobotDevice()
{
}

bool StaubliRobotDevice::InternalAddTool(mitk::TrackingTool* tool)
{
	MITK_INFO << "InternalAddTool Called!!!!";
	if (tool == nullptr)
		return false;
	mitk::TrackingTool::Pointer p = tool;
	//p->Enable();
	int returnValue;
	/* if the connection to the tracking device is already established, add the new tool to the device now */
	if (this->GetState() == Ready)
	{
		MITK_INFO << "State Ready";
		// Request a port handle to load a passive tool into
		//todo add tool tcp to robot
		/* now that the tool is added to the device, add it to list too */
		m_ToolsMutex.lock();
		this->m_StaubliEndEffectors.push_back(p);
		m_ToolsMutex.unlock();
		this->Modified();
		return true;
	}
	else if (this->GetState() == Setup)
	{
		MITK_INFO << "State Setup";
		/* In Setup mode, we only add it to the list, so that OpenConnection() can add it later */
		m_ToolsMutex.lock();
		this->m_StaubliEndEffectors.push_back(p);
		m_ToolsMutex.unlock();
		this->Modified();
		return true;
	}
	else // in Tracking mode, no tools can be added
		return false;
}

void StaubliRobotDevice::IsRobotConnected(bool isConnect)
{
	m_IsConnected = isConnect;
	if (isConnect)
	{
		SetState(Ready);

		m_Heartbeat = QThread::create(heartbeatThreadWorker, this);
		m_Heartbeat->start();
	}
	else
	{
		SetState(Setup);
	}
}

void StaubliRobotDevice::heartbeatThreadWorker(StaubliRobotDevice* _this)
{
	while (_this->m_IsConnected == true)
	{
		_this->m_RobotApi.setspeed(50);
		QThread::msleep(1000);
	}
}

void StaubliRobotDevice::ThreadStartTracking()
{
	MITK_INFO << "tracktools called";
	/* lock the TrackingFinishedMutex to signal that the execution rights are now transfered to the tracking thread */
	// keep lock until end of scope
	std::lock_guard<std::mutex> lock(m_TrackingFinishedMutex);
	if (this->GetState() != Tracking)
	{
		MITK_INFO << "TrackTools Return: state not tracking";
		return;
	}

	bool localStopTracking;
	// Because m_StopTracking is used by two threads, access has to be guarded by a mutex. To minimize thread locking, a local copy is used here
	this->m_StopTrackingMutex.lock(); // update the local copy of m_StopTracking
	localStopTracking = this->m_StopTracking;
	this->m_StopTrackingMutex.unlock();

	while ((this->GetState() == Tracking) && (localStopTracking == false))
	{
		//MITK_INFO << "tracking";
		m_RobotApi.requestrealtimedata();
		m_TrackingData[0] = m_RobotApi.realtime_data.pose.x;
		m_TrackingData[1] = m_RobotApi.realtime_data.pose.y;
		m_TrackingData[2] = m_RobotApi.realtime_data.pose.z;

		m_TrackingData[3] = m_RobotApi.realtime_data.pose.rx;
		m_TrackingData[4] = m_RobotApi.realtime_data.pose.ry;
		m_TrackingData[5] = m_RobotApi.realtime_data.pose.rz;

		Eigen::Quaterniond q;
		//kuka
		// Eigen::AngleAxisd rx(m_TrackingData[3] / d2r, Eigen::Vector3d::UnitZ());
		// Eigen::AngleAxisd ry(m_TrackingData[4] / d2r, Eigen::Vector3d::UnitY());
		// Eigen::AngleAxisd rz(m_TrackingData[5] / d2r, Eigen::Vector3d::UnitX());
		//return rz.matrix()*ry.matrix()*rx.matrix(); // kuka

		//Staubli
		Eigen::AngleAxisd rx(m_TrackingData[3] / d2r, Eigen::Vector3d::UnitX());
		Eigen::AngleAxisd ry(m_TrackingData[4] / d2r, Eigen::Vector3d::UnitY());
		Eigen::AngleAxisd rz(m_TrackingData[5] / d2r, Eigen::Vector3d::UnitZ());
		//return rx.matrix()*ry.matrix()*rz.matrix(); // staubli
		q = rx * ry * rz;

		mitk::TrackingTool::Pointer tool = GetInternalTool();
		mitk::Quaternion quaternion{ q.x(),q.y(),q.z(),q.w() };
		tool->SetOrientation(quaternion);
		mitk::Point3D position;
		position[0] = m_TrackingData[0];
		position[1] = m_TrackingData[1];
		position[2] = m_TrackingData[2];

		tool->SetPosition(position);
		tool->SetTrackingError(0);
		tool->SetErrorMessage("");
		//tool->SetFrameNumber(/*toolData.frameNumber*/); //todo add frameNumber in robot data for debug and frame-rate count ;
		tool->SetDataValid(true);


		/* Update the local copy of m_StopTracking */
		this->m_StopTrackingMutex.lock();
		localStopTracking = m_StopTracking;
		this->m_StopTrackingMutex.unlock();
	}
	/* StopTracking was called, thus the mode should be changed back to Ready now that the tracking loop has ended. */

	// returnvalue = m_capi.stopTracking();
	// if (warningOrError(returnvalue, "m_capi.stopTracking()"))
	// {
	//   MITK_INFO << "m_capi.stopTracking() err";
	//   return;
	// }
	// MITK_INFO << "m_capi stopTracking()";
	return;
	// returning from this function (and ThreadStartTracking()) this will end the thread and transfer control back to main thread by releasing trackingFinishedLockHolder
}