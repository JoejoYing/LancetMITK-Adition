/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QmitkLancetKukaWidget.h"

#include "kukaRobotDevice.h"
#include "lancetKukaTrackingDeviceTypeInformation.h"

#include <QScrollBar>
#include <QSettings>

const std::string QmitkLancetKukaWidget::VIEW_ID = "org.mitk.views.LancetKukaWidget";

QmitkLancetKukaWidget::QmitkLancetKukaWidget(QWidget* parent, Qt::WindowFlags f)
  : QmitkNDIAbstractDeviceWidget(parent, f)
  , m_Controls(nullptr)
{
}

void QmitkLancetKukaWidget::Initialize()
{
  InitializeNDIWidget();
  CreateQtPartControl(this);
  CreateConnections();
}

QmitkLancetKukaWidget::~QmitkLancetKukaWidget()
{
  delete m_Controls;
}

void QmitkLancetKukaWidget::CreateQtPartControl(QWidget *parent)
{
  if (!m_Controls)
  {
    // create GUI widgets
    m_Controls = new Ui::QmitkLancetKukaWidget;
    m_Controls->setupUi(parent);
  }
}

void QmitkLancetKukaWidget::SelfCheck()
{
  auto device = dynamic_cast<KukaRobotDevice*>( GetTrackingDevice().GetPointer());
  if (device!=nullptr && device->GetIsConnected())
  {
    device->RequestExecOperate(/*"Robot",*/ "setio", { "20", "20" });
  }
  else
  {
    MITK_ERROR << "robot not connect";
  }
}

void QmitkLancetKukaWidget::CreateConnections()
{
  if (m_Controls)
  {
    connect((QObject*)(m_Controls->m_testConnectionPolaris), SIGNAL(clicked()), this, SLOT(TestConnection()));
    // connect((QObject*)(m_Controls->m_AutoScanPolaris), SIGNAL(clicked()), this, SLOT(AutoScanPorts()));
    //connect(m_Controls->m_frameRateComboBoxPolaris, SIGNAL(currentIndexChanged(int)), this, SLOT(SetTrackingFrequency(int)));
    //set a few UI components depending on Windows / Linux
#ifdef WIN32
    //m_Controls->portTypeLabelPolaris->setVisible(false);
    //m_Controls->portTypePolaris->setVisible(false);

#else
    m_Controls->m_comPortLabelPolaris->setText("Port Nr:");
    m_Controls->m_portSpinBoxPolaris->setPrefix("");
#endif
  }
  }

void QmitkLancetKukaWidget::ResetOutput()
{
  m_Controls->m_outputTextPolaris->setHtml("<body style=\" font-family:\'MS Shell Dlg 2\'; font-size:7pt; font-weight:400; font-style:normal;\" bgcolor=black><span style=\"color:#ffffff;\"><u>output:</u>");
}

void QmitkLancetKukaWidget::AddOutput(std::string s)
{
  m_Controls->m_outputTextPolaris->setHtml(QString(s.c_str()));
  m_Controls->m_outputTextPolaris->verticalScrollBar()->setValue(m_Controls->m_outputTextPolaris->verticalScrollBar()->maximum());
}

mitk::TrackingDevice::Pointer QmitkLancetKukaWidget::GetTrackingDevice()
{
  KukaRobotDevice::Pointer tempTrackingDevice = KukaRobotDevice::New();

  //get port
  int port = 0;
  //port = m_Controls->m_portSpinBoxPolaris->value();

  //build prefix (depends on linux/win)
  QString prefix = "";
#ifdef WIN32
  prefix = "COM";
  //tempTrackingDevice->SetPortNumber(static_cast<mitk::SerialCommunication::PortNumber>(port)); //also set the com port for compatibility
  //tempTrackingDevice->SetIlluminationActivationRate(GetPolarisFrameRate());
#else
  prefix = m_Controls->portTypePolaris->currentText();
  tempTrackingDevice->SetIlluminationActivationRate(GetPolarisFrameRate());
#endif

  //build port name string
  QString portName = prefix + QString::number(port);
  //tempTrackingDevice->SetTrackingFrequency(GetVegaTrackingRate());
  //tempTrackingDevice->SetDeviceName(portName.toStdString()); //set the port name
  //tempTrackingDevice->SetBaudRate(mitk::SerialCommunication::BaudRate115200);//set baud rate
  tempTrackingDevice->SetType(lancet::KukaRobotTypeInformation::GetTrackingDeviceName());
  return static_cast<mitk::TrackingDevice::Pointer>(tempTrackingDevice);
}

void QmitkLancetKukaWidget::StoreUISettings()
{
  std::string id = "org.mitk.modules.igt.ui.trackingdeviceconfigurationwidget";
  if (this->GetPersistenceService()) // now save the settings using the persistence service
  {
    mitk::PropertyList::Pointer propList = this->GetPersistenceService()->GetPropertyList(id);
    //propList->Set("PolarisPortWin", m_Controls->m_portSpinBoxPolaris->value());
    //propList->Set("PortTypePolaris", m_Controls->portTypePolaris->currentIndex());
    propList->Set("PolarisFrameRate", GetVegaTrackingRate());
  }
  else // QSettings as a fallback if the persistence service is not available
  {
    QSettings settings;
    settings.beginGroup(QString::fromStdString(id));
    //settings.setValue("portSpinBoxPolaris", QVariant(m_Controls->m_portSpinBoxPolaris->value()));
    //settings.setValue("portTypePolaris", QVariant(m_Controls->portTypePolaris->currentIndex()));
    settings.setValue("PolarisFrameRate", QVariant(GetVegaTrackingRate()));
    settings.endGroup();
  }
}

void QmitkLancetKukaWidget::LoadUISettings()
{
  // std::string id = "org.mitk.modules.igt.ui.trackingdeviceconfigurationwidget";
  // if (this->GetPersistenceService())
  // {
  //   int port = 0;
  //   int portType = 0;
  //   int polarisFrameRate = 0;
  //
  //   mitk::PropertyList::Pointer propList = this->GetPersistenceService()->GetPropertyList(id);
  //   if (propList.IsNull())
  //   {
  //     MITK_ERROR << "Property list for this UI (" << id << ") is not available, could not load UI settings!"; return;
  //   }
  //
  //   propList->Get("PolarisPortWin", port);
  //   propList->Get("PortTypePolaris", portType);
  //   propList->Get("PolarisFrameRate", polarisFrameRate);
  //   this->SetPortTypeToGUI(portType);
  //   this->SetPortValueToGUI(port);
  //   m_Controls->m_frameRateComboBoxPolaris->setCurrentIndex(polarisFrameRate);
  // }
  // else
  // {
  //   // QSettings as a fallback if the persistence service is not available
  //   QSettings settings;
  //   settings.beginGroup(QString::fromStdString(id));
  //
  //   //m_Controls->m_portSpinBoxPolaris->setValue(settings.value("portSpinBoxPolaris", 0).toInt());
  //   //m_Controls->portTypePolaris->setCurrentIndex(settings.value("portTypePolaris", 0).toInt());
  //   //framerates 20,30,60 --> divided by 30 = 0,1,2 --> index of combobox
  //   m_Controls->m_frameRateComboBoxPolaris->setCurrentIndex((int)(settings.value("PolarisFrameRate", 0).toInt() / 30));
  //
  //   settings.endGroup();
  // }
}

int QmitkLancetKukaWidget::GetVegaTrackingRate()
{
  uint index =/* m_Controls->m_frameRateComboBoxPolaris->currentIndex()*/20;
  return index;
}

void QmitkLancetKukaWidget::SetPortValueToGUI(int portValue){
  //m_Controls->m_portSpinBoxPolaris->setValue(portValue);
}
void QmitkLancetKukaWidget::SetPortTypeToGUI(int portType){
  //m_Controls->portTypePolaris->setCurrentIndex(portType);
}

QmitkLancetKukaWidget* QmitkLancetKukaWidget::Clone(QWidget* parent) const
{
  QmitkLancetKukaWidget* clonedWidget = new QmitkLancetKukaWidget(parent);
  clonedWidget->Initialize();

  //clonedWidget->SetPortTypeToGUI(m_Controls->portTypePolaris->currentIndex());
  //clonedWidget->SetPortValueToGUI(m_Controls->m_portSpinBoxPolaris->value());
  //clonedWidget->m_Controls->m_frameRateComboBoxPolaris->setCurrentIndex(m_Controls->m_frameRateComboBoxPolaris->currentIndex());
  return clonedWidget;
}
