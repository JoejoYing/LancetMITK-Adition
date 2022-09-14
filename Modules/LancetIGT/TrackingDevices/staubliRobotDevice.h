#ifndef STAUBLIROBOTDEVICE_H
#define STAUBLIROBOTDEVICE_H
//mitk
#include <mitkCommon.h>
#include <mitkTrackingDevice.h>


#include "MitkLancetIGTExports.h"


//qt
#include <QObject>
#include <QThread>
#include <QMetaType>
#include <QPointer>
#include <QString>

#include "mitkTrackingTool.h"

//STAUBLI ROBOT API
#include "robotapi_staubli.h"
#include "udpsocketrobotheartbeat.h" //udp

/** Documentation
  * \brief superclass for specific STAUBLI Robot Devices that use socket communication.
  *
  * implements the TrackingDevice interface for STAUBLI robot devices ()
  *
  * \ingroup Robot
 */
class MITKLANCETIGT_EXPORT  StaubliRobotDevice :public QObject, public  mitk::TrackingDevice
{
	Q_OBJECT
public:
	mitkClassMacro(StaubliRobotDevice, TrackingDevice);
	itkFactorylessNewMacro(Self);
	itkCloneMacro(Self);

	typedef std::vector<mitk::TrackingTool::Pointer> StaubliEndEffectorContainerType;//末端执行器容器类型

	itkGetMacro(IsConnected, bool);
	/**
	   * @brief Opens the connection to the device. This have to be done before the tracking is started.
	   */

	bool OpenConnection() override;
	/**
	   * @brief Closes the connection and clears all resources.
	   */
	bool CloseConnection() override;
	/**
	 * \brief Start the tracking.
	 *
	 * A new thread is created, which continuously reads the position and orientation information of each tool and stores them inside the tools.
	 * Call StopTracking() to stop the tracking thread.
	 */
	bool StartTracking() override;
	/**
	   * \param toolNumber The number of the tool which should be given back.
	   * \return Returns the tool which the number "toolNumber". Returns nullptr, if there is
	   * no tool with this number.
	   */
	mitk::TrackingTool* GetTool(unsigned toolNumber) const override;
	mitk::TrackingTool* GetToolByName(std::string name) const override;

	mitk::TrackingTool* GetInternalTool();//获取内部工具

	mitk::TrackingTool* AddTool(const char* toolName, const char* fileName);
	unsigned GetToolCount() const override;

	void TrackTools();

	std::array<double, 6> GetTrackingData();

	//robotDevice api, move to RobotDevice abstract class later
	bool RequestExecOperate(const QString& funname, const QStringList& param);

protected:
	StaubliRobotDevice();
	virtual ~StaubliRobotDevice() override;

	/**
	  * \brief Add a staubli end effector tool to the list of tracked tools and add tool tcp to robot. This method is used by AddTool
	  * @throw mitk::IGTHardwareException Throws an exception if there are errors while adding the tool.
	  * \warning adding tools is not possible in tracking mode, only in setup and ready.
	  */
	virtual bool InternalAddTool(mitk::TrackingTool* tool); //todo create new tcp in robot

private slots:
	void IsRobotConnected(bool isConnect);

private:
	static void heartbeatThreadWorker(StaubliRobotDevice* _this);
	void ThreadStartTracking();
private:
	bool m_IsConnected = false;

	QPointer<QThread> m_Heartbeat;
	std::array<double, 6> m_TrackingData = {};

	//track
	mutable std::mutex m_ToolsMutex; ///< mutex for coordinated access of tool container
	StaubliEndEffectorContainerType m_StaubliEndEffectors; ///< container for all tracking tools
	///< creates tracking thread that continuously polls serial interface for new tracking data 创建跟踪线程，连续轮询串行接口以获取新的跟踪数据
	std::thread m_Thread;                            ///< ID of tracking thread

	//Robot
	RobotApi m_RobotApi;
	//Device Configure
	std::string m_IpAddress{ "172.31.1.147" };
	QString m_Port{ "69999" };
	std::string m_RemoteIpAddress{ "172.31.1.147" };
	QString m_RemotePort{ "69999" };
	std::string m_DeviceName{ "Staubli" };

	//communication
	UdpSocketRobotHeartbeat m_udp;

};




#endif // STAUBLIROBOTDEVICE_H

