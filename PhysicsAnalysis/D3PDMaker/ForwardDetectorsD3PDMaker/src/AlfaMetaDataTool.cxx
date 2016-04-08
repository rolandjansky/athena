/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"
#include "SGTools/crc64.h"


// D3PDMaker include(s):
#include "D3PDMakerInterfaces/ID3PD.h"

// Local include(s):
#include "AlfaMetaDataTool.h"

namespace D3PD
{
	AlfaMetaDataTool::AlfaMetaDataTool( const std::string& type,const std::string& name, const IInterface* parent )
		: AthAlgTool( type, name, parent ), m_COOLDir( "" ), m_d3pdSvc( "D3PDSvc", name ), m_iovSvc( "IOVDbSvc", name )
	{
		declareProperty( "MetaDataDir", m_COOLDir = "alfaMeta" );
		declareProperty( "D3PDSvc", m_d3pdSvc );

		//DCS BLM
		m_pullBLMId=NULL;
		m_pvecBLM=NULL;

		//DCS HVCHANNEL
		m_pullHVChannelId=NULL;
		m_pvecActualVMeas=NULL;
		m_pvecActualIMeas=NULL;

		//DCS LOCALMONITORING
		m_pullLocalMonitoringId=NULL;
		m_pvecTempSensor1=NULL;
		m_pvecTempSensor2=NULL;
		m_pvecTempSensor3=NULL;
		m_pvecTempSensor4=NULL;
		m_pvecTempSensor5=NULL;

		//DCS MOVEMENT
		m_pullMovementId=NULL;
		m_pvecPosLVDT=NULL;
		m_pvecPosMotor=NULL;

		//DCS RADMON
		m_pullRadMonId=NULL;
		m_pvecDose=NULL;
		m_pvecFluence=NULL;
		m_pvecTemp=NULL;

		//DCS TRIGGERRATES
		m_pullTriggerRatesId=NULL;
		m_pvecTriggerRates=NULL;

		//DCS FECONFIGURATION
		m_pullFEConfigurationId=NULL;
		m_pvecThreshold=NULL;
		m_pvecGain=NULL;
		m_pvecMDGainMode=NULL;

		//DCS TRIGGERSETTINGS
		m_pullTriggerSettingsId=NULL;
		m_pvecTriggerSet=NULL;
		m_pvecLatency=NULL;

                m_DCSTreeBLM = 0;
                m_DCSTreeFEConfiguration = 0;
                m_DCSTreeHVChannel = 0;
                m_DCSTreeLocalMonitoring = 0;
                m_DCSTreeMovement = 0;
                m_DCSTreeRadMon = 0;
                m_DCSTreeTriggerRates = 0;
                m_DCSTreeTriggerSettings = 0;
	}

	StatusCode AlfaMetaDataTool::initialize()
	{
		StatusCode sc;

		ATH_MSG_INFO( "Initializing. Package version: " << PACKAGE_VERSION );

		CHECK(m_d3pdSvc.retrieve());
		CHECK(m_iovSvc.retrieve());

		//DCS BLM
		CHECK(m_d3pdSvc->make(m_COOLDir+"/alfa_BlmTree",m_DCSTreeBLM));
		CHECK(m_DCSTreeBLM->addVariable("ID",m_pullBLMId,"BLM ID"));
		CHECK(m_DCSTreeBLM->addVariable("BLM",m_pvecBLM,"BLM values"));

		//DCS HVCHANNEL
		CHECK(m_d3pdSvc->make(m_COOLDir+"/alfa_HVChannelTree",m_DCSTreeHVChannel));
		CHECK(m_DCSTreeHVChannel->addVariable("ID",m_pullHVChannelId,"HVCHANNEL ID"));
		CHECK(m_DCSTreeHVChannel->addVariable("Voltage",m_pvecActualVMeas,"Voltage values"));
		CHECK(m_DCSTreeHVChannel->addVariable("Current",m_pvecActualIMeas,"Current values"));

		//DCS LOCALMONITORING
		CHECK(m_d3pdSvc->make(m_COOLDir+"/alfa_LocalMonitoringTree",m_DCSTreeLocalMonitoring));
		CHECK(m_DCSTreeLocalMonitoring->addVariable("ID",m_pullLocalMonitoringId,"LOCALMONITORING ID"));
		CHECK(m_DCSTreeLocalMonitoring->addVariable("TempSensor1",m_pvecTempSensor1,"Temperature sensor no. 1"));
		CHECK(m_DCSTreeLocalMonitoring->addVariable("TempSensor2",m_pvecTempSensor2,"Temperature sensor no. 2"));
		CHECK(m_DCSTreeLocalMonitoring->addVariable("TempSensor3",m_pvecTempSensor3,"Temperature sensor no. 3"));
		CHECK(m_DCSTreeLocalMonitoring->addVariable("TempSensor4",m_pvecTempSensor4,"Temperature sensor no. 4"));
		CHECK(m_DCSTreeLocalMonitoring->addVariable("TempSensor5",m_pvecTempSensor5,"Temperature sensor no. 5"));

		//DCS MOVEMENT
		CHECK(m_d3pdSvc->make(m_COOLDir+"/alfa_MovementTree",m_DCSTreeMovement));
		CHECK(m_DCSTreeMovement->addVariable("ID",m_pullMovementId,"MOVEMENT ID"));
		CHECK(m_DCSTreeMovement->addVariable("LVDT",m_pvecPosLVDT,"LVDT value"));
		CHECK(m_DCSTreeMovement->addVariable("Motor",m_pvecPosMotor,"Position of motor"));

		//DCS RADMON
		CHECK(m_d3pdSvc->make(m_COOLDir+"/alfa_RadMonTree",m_DCSTreeRadMon));
		CHECK(m_DCSTreeRadMon->addVariable("ID",m_pullRadMonId,"RADMON ID"));
		CHECK(m_DCSTreeRadMon->addVariable("Dose",m_pvecDose,"Dose"));
		CHECK(m_DCSTreeRadMon->addVariable("Fluence",m_pvecFluence,"Fluence"));
		CHECK(m_DCSTreeRadMon->addVariable("Temperature",m_pvecTemp,"Temperature"));

		//DCS TRIGGERRATES
		CHECK(m_d3pdSvc->make(m_COOLDir+"/alfa_TriggerRatesTree",m_DCSTreeTriggerRates));
		CHECK(m_DCSTreeTriggerRates->addVariable("ID",m_pullTriggerRatesId,"TRIGGERRATES ID"));
		CHECK(m_DCSTreeTriggerRates->addVariable("TriggerRates",m_pvecTriggerRates,"Trigger rates"));

		//DCS FECONFIGURATION
		CHECK(m_d3pdSvc->make(m_COOLDir+"/alfa_FEConfigurationTree",m_DCSTreeFEConfiguration));
		CHECK(m_DCSTreeFEConfiguration->addVariable("ID",m_pullFEConfigurationId,"FECONFIGURATION ID"));
		CHECK(m_DCSTreeFEConfiguration->addVariable("Threshold",m_pvecThreshold,"Threshold"));
		CHECK(m_DCSTreeFEConfiguration->addVariable("Gain",m_pvecGain,"Gain"));
		CHECK(m_DCSTreeFEConfiguration->addVariable("MDGainMode",m_pvecMDGainMode,"Gain mode of the Main detector"));

		//DCS TRIGGERSETTINGS
		CHECK(m_d3pdSvc->make(m_COOLDir+"/alfa_TriggerSettingsTree",m_DCSTreeTriggerSettings));
		CHECK(m_DCSTreeTriggerSettings->addVariable("ID",m_pullTriggerSettingsId,"TRIGGERSETTINGS ID"));
		CHECK(m_DCSTreeTriggerSettings->addVariable("TriggerSettings",m_pvecTriggerSet,"Trigger settings"));
		CHECK(m_DCSTreeTriggerSettings->addVariable("Latency",m_pvecLatency,"Trigger latency"));

		//register IOV/COOL callbacks
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_BLM));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_HVCHANNEL));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_LOCALMONITORING));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_MOVEMENT));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_RADMON));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_TRIGGERRATES));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_FECONFIGURATION));
		CHECK(AddCOOLFolderCallback(DCSCOLLNAME_TRIGGERSETTINGS));

		return sc;
   }

	StatusCode AlfaMetaDataTool::queryInterface( const InterfaceID& riid, void** ppvIf )
	{
		if(riid==IMetadataTool::interfaceID()){
			*ppvIf = static_cast< IMetadataTool* >( this );
			addRef();
			return StatusCode::SUCCESS;
		}
		/*else if( riid == IIncidentListener::interfaceID() ) {
			*ppvIf = static_cast< IIncidentListener* >( this );
			addRef();
			return StatusCode::SUCCESS;
		}*/

		return AthAlgTool::queryInterface( riid, ppvIf );
   }

   StatusCode AlfaMetaDataTool::writeMetadata( ID3PD* )
   {
		ATH_MSG_INFO( "DCS information available in the D3PD in directory: \""<< m_COOLDir << "\"" );
		return StatusCode::SUCCESS;
   }

   StatusCode AlfaMetaDataTool::AddCOOLFolderCallback(const string& Folder)
   {
	   StatusCode sc=StatusCode::FAILURE;

	   const DataHandle<CondAttrListCollection> DataPtr;
	   sc=detStore()->regFcn(&AlfaMetaDataTool::COOLUpdate, this, DataPtr, Folder, true);
	   if(sc!=StatusCode::SUCCESS){
		   msg(MSG::ERROR) << "Cannot register COOL callback for folder '"<<Folder<<"'" << endreq;
	   }

	   return sc;
   }

   //StatusCode AlfaMetaDataTool::COOLUpdate(IOVSVC_CALLBACK_ARGS_P(I,keys)) //-- old declaration (<=17.6.0)
   StatusCode AlfaMetaDataTool::COOLUpdate(IOVSVC_CALLBACK_ARGS_K(keys))
   {
	   int nChannel;
	   StatusCode sc=StatusCode::SUCCESS;
	   list<string>::const_iterator iter;
	   const CondAttrListCollection* listAttrColl;
	   CondAttrListCollection::const_iterator iterAttr;

	   for(iter=keys.begin();iter!=keys.end();iter++){
		   if((*iter)==DCSCOLLNAME_BLM){
			   msg(MSG::INFO) << " IOV/COOL Notification '"<<DCSCOLLNAME_BLM<<"'" << endreq;
			   ClearDCSData(EDCSI_BLM);

			   if(detStore()->retrieve(listAttrColl,DCSCOLLNAME_BLM)==StatusCode::SUCCESS){
				   *m_pullBLMId=CalcDCSId(EDCSI_BLM);

				   for(iterAttr=listAttrColl->begin();iterAttr!=listAttrColl->end();iterAttr++){
					   nChannel=iterAttr->first;
					   (*m_pvecBLM)[nChannel-1]=((iterAttr->second)[0]).data<float>();
				   }

				   if((sc=m_DCSTreeBLM->capture()).isFailure()){
					   msg(MSG::ERROR) << " Couldn't save '"<<DCSCOLLNAME_BLM<<"' tree" << endreq;
					   sc=StatusCode::FAILURE;
					   break;
				   }
			   }
			   else{
				   msg(MSG::ERROR) << "DCS Folder '"<<DCSCOLLNAME_BLM<<"' not found" << endreq;
				   sc=StatusCode::FAILURE;
				   break;
			   }
		   }
		   else if((*iter)==DCSCOLLNAME_HVCHANNEL){
			   msg(MSG::INFO) << " IOV/COOL Notification '"<<DCSCOLLNAME_HVCHANNEL<<"'" << endreq;
			   ClearDCSData(EDCSI_HVCHANNEL);

			   if(detStore()->retrieve(listAttrColl,DCSCOLLNAME_HVCHANNEL)==StatusCode::SUCCESS){
				   *m_pullHVChannelId=CalcDCSId(EDCSI_HVCHANNEL);

				   for(iterAttr=listAttrColl->begin();iterAttr!=listAttrColl->end();iterAttr++){
						nChannel=iterAttr->first;
						(*m_pvecActualVMeas)[nChannel-1]=((iterAttr->second)[0]).data<float>();
						(*m_pvecActualIMeas)[nChannel-1]=((iterAttr->second)[1]).data<float>();
				   }

				   if((sc=m_DCSTreeHVChannel->capture()).isFailure()){
					   msg(MSG::ERROR) << " Couldn't save '"<<DCSCOLLNAME_HVCHANNEL<<"' tree" << endreq;
					   sc=StatusCode::FAILURE;
					   break;
				   }
			   }
			   else{
				   msg(MSG::ERROR) << "DCS Folder '"<<DCSCOLLNAME_HVCHANNEL<<"' not found" << endreq;
				   sc=StatusCode::FAILURE;
				   break;
			   }
		   }
		   else if((*iter)==DCSCOLLNAME_LOCALMONITORING){
			   msg(MSG::INFO) << " IOV/COOL Notification '"<<DCSCOLLNAME_LOCALMONITORING<<"'" << endreq;
			   ClearDCSData(EDCSI_LOCALMONITORING);

			   if(detStore()->retrieve(listAttrColl,DCSCOLLNAME_LOCALMONITORING)==StatusCode::SUCCESS){
				   *m_pullLocalMonitoringId=CalcDCSId(EDCSI_LOCALMONITORING);

				   for(iterAttr=listAttrColl->begin();iterAttr!=listAttrColl->end();iterAttr++){
						nChannel=iterAttr->first; //RPot ID
						(*m_pvecTempSensor1)[nChannel-1]=((iterAttr->second)[0]).data<float>();
						(*m_pvecTempSensor2)[nChannel-1]=((iterAttr->second)[1]).data<float>();
						(*m_pvecTempSensor3)[nChannel-1]=((iterAttr->second)[2]).data<float>();
						(*m_pvecTempSensor4)[nChannel-1]=((iterAttr->second)[3]).data<float>();
						(*m_pvecTempSensor5)[nChannel-1]=((iterAttr->second)[4]).data<float>();
				   }

				   if((sc=m_DCSTreeLocalMonitoring->capture()).isFailure()){
					   msg(MSG::ERROR) << " Couldn't save '"<<DCSCOLLNAME_LOCALMONITORING<<"' tree" << endreq;
					   sc=StatusCode::FAILURE;
					   break;
				   }
			   }
			   else{
				   msg(MSG::ERROR) << "DCS Folder '"<<DCSCOLLNAME_LOCALMONITORING<<"' not found" << endreq;
				   sc=StatusCode::FAILURE;
				   break;
			   }
		   }
		   else if((*iter)==DCSCOLLNAME_MOVEMENT){
			   msg(MSG::INFO) << " IOV/COOL Notification '"<<DCSCOLLNAME_MOVEMENT<<"'" << endreq;
			   ClearDCSData(EDCSI_MOVEMENT);

			   if(detStore()->retrieve(listAttrColl,DCSCOLLNAME_MOVEMENT)==StatusCode::SUCCESS){
				   *m_pullMovementId=CalcDCSId(EDCSI_MOVEMENT);

				   for(iterAttr=listAttrColl->begin();iterAttr!=listAttrColl->end();iterAttr++){
						nChannel=iterAttr->first; //RPot ID
						(*m_pvecPosLVDT)[nChannel-1]=((iterAttr->second)[0]).data<float>();
						(*m_pvecPosMotor)[nChannel-1]=((iterAttr->second)[1]).data<float>();
				   }

				   if((sc=m_DCSTreeMovement->capture()).isFailure()){
					   msg(MSG::ERROR) << " Couldn't save '"<<DCSCOLLNAME_MOVEMENT<<"' tree" << endreq;
					   sc=StatusCode::FAILURE;
					   break;
				   }
			   }
			   else{
				   msg(MSG::ERROR) << "DCS Folder '"<<DCSCOLLNAME_MOVEMENT<<"' not found" << endreq;
				   sc=StatusCode::FAILURE;
				   break;
			   }
		   }
		   else if((*iter)==DCSCOLLNAME_RADMON){
			   msg(MSG::INFO) << " IOV/COOL Notification '"<<DCSCOLLNAME_RADMON<<"'" << endreq;
			   ClearDCSData(EDCSI_RADMON);

			   if(detStore()->retrieve(listAttrColl,DCSCOLLNAME_RADMON)==StatusCode::SUCCESS){
				   *m_pullRadMonId=CalcDCSId(EDCSI_RADMON);

				   for(iterAttr=listAttrColl->begin();iterAttr!=listAttrColl->end();iterAttr++){
						nChannel=iterAttr->first;
						(*m_pvecDose)[nChannel-1]=((iterAttr->second)[0]).data<float>();
						(*m_pvecFluence)[nChannel-1]=((iterAttr->second)[1]).data<float>();
						(*m_pvecTemp)[nChannel-1]=((iterAttr->second)[2]).data<float>();
				   }

				   if((sc=m_DCSTreeRadMon->capture()).isFailure()){
					   msg(MSG::ERROR) << " Couldn't save '"<<DCSCOLLNAME_RADMON<<"' tree" << endreq;
					   sc=StatusCode::FAILURE;
					   break;
				   }
			   }
			   else{
				   msg(MSG::ERROR) << "DCS Folder '"<<DCSCOLLNAME_RADMON<<"' not found" << endreq;
				   sc=StatusCode::FAILURE;
				   break;
			   }
		   }
		   else if((*iter)==DCSCOLLNAME_TRIGGERRATES){
			   msg(MSG::INFO) << " IOV/COOL Notification '"<<DCSCOLLNAME_TRIGGERRATES<<"'" << endreq;
			   ClearDCSData(EDCSI_TRIGGERRATES);

			   if(detStore()->retrieve(listAttrColl,DCSCOLLNAME_TRIGGERRATES)==StatusCode::SUCCESS){
				   *m_pullTriggerRatesId=CalcDCSId(EDCSI_TRIGGERRATES);

				   for(iterAttr=listAttrColl->begin();iterAttr!=listAttrColl->end();iterAttr++){
						nChannel=iterAttr->first; //RPot ID
						(*m_pvecTriggerRates)[nChannel-1]=(int)(((iterAttr->second)[0]).data<unsigned int>());
				   }

				   if((sc=m_DCSTreeTriggerRates->capture()).isFailure()){
					   msg(MSG::ERROR) << " Couldn't save '"<<DCSCOLLNAME_TRIGGERRATES<<"' tree" << endreq;
					   sc=StatusCode::FAILURE;
					   break;
				   }
			   }
			   else{
				   msg(MSG::ERROR) << "DCS Folder '"<<DCSCOLLNAME_TRIGGERRATES<<"' not found" << endreq;
				   sc=StatusCode::FAILURE;
				   break;
			   }
		   }
		   else if((*iter)==DCSCOLLNAME_FECONFIGURATION){
			   msg(MSG::INFO) << " IOV/COOL Notification '"<<DCSCOLLNAME_FECONFIGURATION<<"'" << endreq;
			   ClearDCSData(EDCSI_FECONFIGURATION);

			   if(detStore()->retrieve(listAttrColl,DCSCOLLNAME_FECONFIGURATION)==StatusCode::SUCCESS){
				   *m_pullFEConfigurationId=CalcDCSId(EDCSI_FECONFIGURATION);

				   for(iterAttr=listAttrColl->begin();iterAttr!=listAttrColl->end();iterAttr++){
						nChannel=iterAttr->first; //RPot ID
						(*m_pvecThreshold)[(nChannel-1)*FECNFTHRESHLOLDCNT+0]=((iterAttr->second)[0]).data<int>();
						(*m_pvecThreshold)[(nChannel-1)*FECNFTHRESHLOLDCNT+1]=((iterAttr->second)[5]).data<int>();
						(*m_pvecThreshold)[(nChannel-1)*FECNFTHRESHLOLDCNT+2]=((iterAttr->second)[8]).data<int>();

						(*m_pvecGain)[(nChannel-1)*FECNFGAINCNT+0]=((iterAttr->second)[1]).data<int>();
						(*m_pvecGain)[(nChannel-1)*FECNFGAINCNT+1]=((iterAttr->second)[3]).data<int>();
						(*m_pvecGain)[(nChannel-1)*FECNFGAINCNT+2]=((iterAttr->second)[4]).data<int>();
						(*m_pvecGain)[(nChannel-1)*FECNFGAINCNT+3]=((iterAttr->second)[6]).data<int>();
						(*m_pvecGain)[(nChannel-1)*FECNFGAINCNT+4]=((iterAttr->second)[7]).data<int>();

						m_pvecMDGainMode->push_back(((iterAttr->second)[2]).data<string>());
				   }

				   if((sc=m_DCSTreeFEConfiguration->capture()).isFailure()){
					   msg(MSG::ERROR) << " Couldn't save '"<<DCSCOLLNAME_FECONFIGURATION<<"' tree" << endreq;
					   sc=StatusCode::FAILURE;
					   break;
				   }
			   }
			   else{
				   msg(MSG::ERROR) << "DCS Folder '"<<DCSCOLLNAME_FECONFIGURATION<<"' not found" << endreq;
				   sc=StatusCode::FAILURE;
				   break;
			   }
		   }
		   else if((*iter)==DCSCOLLNAME_TRIGGERSETTINGS){
			   msg(MSG::INFO) << " IOV/COOL Notification '"<<DCSCOLLNAME_TRIGGERSETTINGS<<"'" << endreq;
			   ClearDCSData(EDCSI_TRIGGERSETTINGS);

			   if(detStore()->retrieve(listAttrColl,DCSCOLLNAME_TRIGGERSETTINGS)==StatusCode::SUCCESS){
				   *m_pullTriggerSettingsId=CalcDCSId(EDCSI_TRIGGERSETTINGS);

				   for(iterAttr=listAttrColl->begin();iterAttr!=listAttrColl->end();iterAttr++){
						nChannel=iterAttr->first; //RPot ID
						(*m_pvecTriggerSet)[(nChannel-1)*TRIGSETCNT+0]=(int)(((iterAttr->second)[0]).data<bool>());
						(*m_pvecTriggerSet)[(nChannel-1)*TRIGSETCNT+1]=(int)(((iterAttr->second)[1]).data<bool>());
						(*m_pvecTriggerSet)[(nChannel-1)*TRIGSETCNT+2]=(int)(((iterAttr->second)[2]).data<bool>());
						(*m_pvecTriggerSet)[(nChannel-1)*TRIGSETCNT+3]=(int)(((iterAttr->second)[3]).data<bool>());
						(*m_pvecTriggerSet)[(nChannel-1)*TRIGSETCNT+4]=(int)(((iterAttr->second)[4]).data<bool>());
						(*m_pvecTriggerSet)[(nChannel-1)*TRIGSETCNT+5]=(int)(((iterAttr->second)[5]).data<bool>());

						(*m_pvecLatency)[(nChannel-1)*TRIGSETLATENCYCNT+0]=(int)(((iterAttr->second)[6]).data<unsigned int>());
						(*m_pvecLatency)[(nChannel-1)*TRIGSETLATENCYCNT+1]=(int)(((iterAttr->second)[7]).data<unsigned int>());
						(*m_pvecLatency)[(nChannel-1)*TRIGSETLATENCYCNT+2]=(int)(((iterAttr->second)[8]).data<unsigned int>());
				   }

				   if((sc=m_DCSTreeTriggerSettings->capture()).isFailure()){
					   msg(MSG::ERROR) << " Couldn't save '"<<DCSCOLLNAME_TRIGGERSETTINGS<<"' tree" << endreq;
					   sc=StatusCode::FAILURE;
					   break;
				   }
			   }
			   else{
				   msg(MSG::ERROR) << "DCS Folder '"<<DCSCOLLNAME_TRIGGERSETTINGS<<"' not found" << endreq;
				   sc=StatusCode::FAILURE;
				   break;
			   }
		   }
	   }

	   return StatusCode::SUCCESS;
   }

   unsigned long long AlfaMetaDataTool::CalcDCSId(eDCSItem eItem)
   {
	   unsigned long long ullID;
	   string Folder, Foldername, Tag;
	   IOVRange Range;
	   bool bRetrieved;
	   unsigned long long ullBytesRead;
	   float fReadTime;

	   switch(eItem)
	   {
	   case EDCSI_BLM:
		   Folder=DCSCOLLNAME_BLM;
		   break;
	   case EDCSI_HVCHANNEL:
		   Folder=DCSCOLLNAME_HVCHANNEL;
		   break;
	   case EDCSI_LOCALMONITORING:
		   Folder=DCSCOLLNAME_LOCALMONITORING;
		   break;
	   case EDCSI_MOVEMENT:
		   Folder=DCSCOLLNAME_MOVEMENT;
		   break;
	   case EDCSI_RADMON:
		   Folder=DCSCOLLNAME_RADMON;
		   break;
	   case EDCSI_TRIGGERRATES:
		   Folder=DCSCOLLNAME_TRIGGERRATES;
		   break;
	   case EDCSI_FECONFIGURATION:
		   Folder=DCSCOLLNAME_FECONFIGURATION;
		   break;
	   case EDCSI_TRIGGERSETTINGS:
		   Folder=DCSCOLLNAME_TRIGGERSETTINGS;
		   break;
	   default:
		   break;
	   }

	   if(!m_iovSvc->getKeyInfo(Folder,Foldername,Tag,Range,bRetrieved,ullBytesRead,fReadTime)) {
				msg(MSG::ERROR)<<"Couldn't get IOV data about folder: "<<Folder<<endreq;
				return 0;
	   }

	   // Construct the ID:
	   IOVTime time=Range.start();
	   if(time.isRunEvent()){
		   ullID=static_cast<unsigned long long>(((time.run()&0xffff)<<16)|(time.event()&0xffff));
	   }
	   else if(time.isTimestamp()){
		   //ullID=static_cast<unsigned long long>(time.timestamp()&0xffffffff);
		   ullID=static_cast<unsigned long long>(time.timestamp());
	   }
	   else{
		   ullID=0;
	   }

	   return ullID;
   }

   void AlfaMetaDataTool::ClearDCSData(eDCSItem eItem)
   {
		switch(eItem){
		case EDCSI_BLM:
		   if(m_pullBLMId!=NULL) *m_pullBLMId=0;
		   if(m_pvecBLM!=NULL){
			   m_pvecBLM->resize(BLMCNT);
			   fill_n(m_pvecBLM->begin(),m_pvecBLM->size(),-9999.0);
		   }
		   break;
		case EDCSI_HVCHANNEL:
		   if(m_pullHVChannelId!=NULL) *m_pullHVChannelId=0;
		   if(m_pvecActualVMeas!=NULL){
			   m_pvecActualVMeas->resize(HVCHANNELCNT);
			   fill_n(m_pvecActualVMeas->begin(),m_pvecActualVMeas->size(),-9999.0);
		   }
		   if(m_pvecActualIMeas!=NULL){
			   m_pvecActualIMeas->resize(HVCHANNELCNT);
			   fill_n(m_pvecActualIMeas->begin(),m_pvecActualIMeas->size(),-9999.0);
		   }
		   break;
		case EDCSI_LOCALMONITORING:
		   if(m_pullLocalMonitoringId!=NULL) *m_pullLocalMonitoringId=0;
		   if(m_pvecTempSensor1!=NULL){
			   m_pvecTempSensor1->resize(RPOTSCNT);
			   fill_n(m_pvecTempSensor1->begin(),m_pvecTempSensor1->size(),-9999.0);
		   }
		   if(m_pvecTempSensor2!=NULL){
			   m_pvecTempSensor2->resize(RPOTSCNT);
			   fill_n(m_pvecTempSensor2->begin(),m_pvecTempSensor2->size(),-9999.0);
		   }
		   if(m_pvecTempSensor3!=NULL){
			   m_pvecTempSensor3->resize(RPOTSCNT);
			   fill_n(m_pvecTempSensor3->begin(),m_pvecTempSensor3->size(),-9999.0);
		   }
		   if(m_pvecTempSensor4!=NULL){
			   m_pvecTempSensor4->resize(RPOTSCNT);
			   fill_n(m_pvecTempSensor4->begin(),m_pvecTempSensor4->size(),-9999.0);
		   }
		   if(m_pvecTempSensor5!=NULL){
			   m_pvecTempSensor5->resize(RPOTSCNT);
			   fill_n(m_pvecTempSensor5->begin(),m_pvecTempSensor5->size(),-9999.0);
		   }
		   break;
		case EDCSI_MOVEMENT:
		   if(m_pullMovementId!=NULL) *m_pullMovementId=0;
		   if(m_pvecPosLVDT!=NULL){
			   m_pvecPosLVDT->resize(RPOTSCNT);
			   fill_n(m_pvecPosLVDT->begin(),m_pvecPosLVDT->size(),-9999.0);
		   }
		   if(m_pvecPosMotor!=NULL){
			   m_pvecPosMotor->resize(RPOTSCNT);
			   fill_n(m_pvecPosMotor->begin(),m_pvecPosMotor->size(),-9999.0);
		   }
		   break;
		case EDCSI_RADMON:
		   if(m_pullRadMonId!=NULL) *m_pullRadMonId=0;
		   if(m_pvecDose!=NULL){
			   m_pvecDose->resize(RADMONCNT);
			   fill_n(m_pvecDose->begin(),m_pvecDose->size(),-9999.0);
		   }
		   if(m_pvecFluence!=NULL){
			   m_pvecFluence->resize(RADMONCNT);
			   fill_n(m_pvecFluence->begin(),m_pvecFluence->size(),-9999.0);
		   }
		   if(m_pvecTemp!=NULL){
			   m_pvecTemp->resize(RADMONCNT);
			   fill_n(m_pvecTemp->begin(),m_pvecTemp->size(),-9999.0);
		   }
		   break;
		case EDCSI_TRIGGERRATES:
		   if(m_pullTriggerRatesId!=NULL) *m_pullTriggerRatesId=0;
		   if(m_pvecTriggerRates!=NULL){
			   m_pvecTriggerRates->resize(RPOTSCNT);
			   fill_n(m_pvecTriggerRates->begin(),m_pvecTriggerRates->size(),-1);
		   }
		   break;
		case EDCSI_FECONFIGURATION:
		   if(m_pullFEConfigurationId!=NULL) *m_pullFEConfigurationId=0;
		   if(m_pvecThreshold!=NULL){
			   m_pvecThreshold->resize(RPOTSCNT*FECNFTHRESHLOLDCNT);
			   fill_n(m_pvecThreshold->begin(),m_pvecThreshold->size(),-1);
		   }
		   if(m_pvecGain!=NULL){
			   m_pvecGain->resize(RPOTSCNT*FECNFGAINCNT);
			   fill_n(m_pvecGain->begin(),m_pvecGain->size(),-1);
		   }
		   if(m_pvecMDGainMode!=NULL){
			   m_pvecMDGainMode->clear();
		   }
		   break;
		case EDCSI_TRIGGERSETTINGS:
		   if(m_pullTriggerSettingsId!=NULL) *m_pullTriggerSettingsId=0;
		   if(m_pvecTriggerSet!=NULL){
			   m_pvecTriggerSet->resize(RPOTSCNT*TRIGSETCNT);
			   fill_n(m_pvecTriggerSet->begin(),m_pvecTriggerSet->size(),-1);
		   }
		   if(m_pvecLatency!=NULL){
			   m_pvecLatency->resize(RPOTSCNT*TRIGSETLATENCYCNT);
			   fill_n(m_pvecLatency->begin(),m_pvecLatency->size(),-1);
		   }
		   break;
		default:
		   break;
		}
   }

} // namespace D3PD
