#include "lancetStaubliTrackingDeviceTypeInformation.h"
#include "staubliRobotDevice.h"
#include "LancetVegaTrackingDevice.h"

namespace lancet
{
	std::string StaubliRobotTypeInformation::GetTrackingDeviceName()
	{
		return "Staubli";
	}

	mitk::TrackingDeviceData StaubliRobotTypeInformation::GetDeviceDataLancetStaubliTrackingDevice()
	{
		mitk::TrackingDeviceData data = { StaubliRobotTypeInformation::GetTrackingDeviceName(), "Staubli Robot", "cube", "X" };
		return data;
	}

	StaubliRobotTypeInformation::StaubliRobotTypeInformation()
	{
		this->m_DeviceName = GetTrackingDeviceName();
		this->m_TrackingDeviceData.push_back(GetDeviceDataLancetStaubliTrackingDevice());
	}

	mitk::TrackingDeviceSource::Pointer StaubliRobotTypeInformation::CreateTrackingDeviceSource(
		mitk::TrackingDevice::Pointer trackingDevice,
		mitk::NavigationToolStorage::Pointer navigationTools,
		std::string* errorMessage,
		std::vector<int>* toolCorrespondencesInToolStorage)
	{

		mitk::TrackingDeviceSource::Pointer returnValue = mitk::TrackingDeviceSource::New();
		StaubliRobotDevice::Pointer thisDevice = dynamic_cast<StaubliRobotDevice*>(trackingDevice.GetPointer());
		*toolCorrespondencesInToolStorage = std::vector<int>();
		//add the tools to the tracking device
		for (unsigned int i = 0; i < navigationTools->GetToolCount(); i++)
		{
			mitk::NavigationTool::Pointer thisNavigationTool = navigationTools->GetTool(i);
			toolCorrespondencesInToolStorage->push_back(i);
			bool toolAddSuccess = thisDevice->AddTool(thisNavigationTool->GetToolName().c_str(), thisNavigationTool->GetCalibrationFile().c_str());
			if (!toolAddSuccess)
			{
				//todo: error handling
				errorMessage->append("Can't add tool, is the SROM-file valid?");
				return nullptr;
			}
			thisDevice->GetTool(i)->SetToolTipPosition(thisNavigationTool->GetToolTipPosition(), thisNavigationTool->GetToolAxisOrientation());
		}
		returnValue->SetTrackingDevice(thisDevice);
		return returnValue;
	}
}
