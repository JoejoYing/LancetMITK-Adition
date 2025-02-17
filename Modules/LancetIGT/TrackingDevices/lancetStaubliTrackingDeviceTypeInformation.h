
// mitk
#ifndef LANCETSTAUBLITRACKINGDEVICETYPEINFORMATION_H
#define LANCETSTAUBLITRACKINGDEVICETYPEINFORMATION_H

#include <mitkTrackingDeviceTypeInformation.h>
// The following header file is generated by CMake and thus it's located in
// the build directory. It provides an export macro for classes and functions
// that you want to be part of the public interface of your module.
#include <MitkLancetIGTExports.h>
namespace lancet
{

	class MITKLANCETIGT_EXPORT StaubliRobotTypeInformation : public mitk::TrackingDeviceTypeInformation
	{
	public:
		StaubliRobotTypeInformation();
		mitk::TrackingDeviceSource::Pointer CreateTrackingDeviceSource(mitk::TrackingDevice::Pointer trackingDevice,
			mitk::NavigationToolStorage::Pointer navigationTools,
			std::string* errorMessage,
			std::vector<int>* toolCorrespondencesInToolStorage) override;
		static std::string GetTrackingDeviceName();
		static mitk::TrackingDeviceData GetDeviceDataLancetStaubliTrackingDevice();
	};
}
#endif // LANCETSTAUBLITRACKINGDEVICETYPEINFORMATION_H
