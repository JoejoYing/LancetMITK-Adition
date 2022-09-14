set(H_FILES
  DataManagement/lancetNavigationObject.h
  DataManagement/lancetTrackingDeviceSourceConfigurator.h
  
  IO/lancetNavigationObjectWriter.h

  Algorithms/lancetNavigationDataInReferenceCoordFilter.h
  Rendering/lancetNavigationObjectVisualizationFilter.h

  TrackingDevices/lancetRobotTrackingTool.h
  TrackingDevices/lancetKukaTrackingDeviceTypeInformation.h
  TrackingDevices/lancetStaubliTrackingDeviceTypeInformation.h
  TrackingDevices/kukaRobotDevice.h
  TrackingDevices/staubliRobotDevice.h

  UI/QmitkLancetKukaWidget.cpp
  UI/QmitkLancetStaubliWidget.h
)

set(CPP_FILES
  DataManagement/lancetNavigationObject.cpp
  DataManagement/lancetTrackingDeviceSourceConfigurator.cpp
  
  IO/lancetNavigationObjectWriter.cpp
  
  Algorithms/lancetNavigationDataInReferenceCoordFilter.cpp
  Rendering/lancetNavigationObjectVisualizationFilter.cpp

  TrackingDevices/lancetRobotTrackingTool.cpp
  TrackingDevices/lancetKukaTrackingDeviceTypeInformation.cpp
  TrackingDevices/lancetStaubliTrackingDeviceTypeInformation.cpp

  TrackingDevices/kukaRobotDevice.cpp
  TrackingDevices/staubliRobotDevice.cpp

  UI/QmitkLancetKukaWidget.cpp
  UI/QmitkLancetStaubliWidget.cpp
)

set(UI_FILES
  UI/QmitkLancetKukaWidget.ui
  UI/QmitkLancetStaubliWidget.ui
)

set(MOC_H_FILES
  UI/QmitkLancetKukaWidget.h
  TrackingDevices/kukaRobotDevice.h
  UI/QmitkLancetStaubliWidget.h
  TrackingDevices/staubliRobotDevice.h
)

set(RESOURCE_FILES
)
