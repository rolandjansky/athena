/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/IdentifierHash.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "PathResolver/PathResolver.h"
#include <fstream>
#include <string>
#include "MuonCondData/MdtDeadTubeStatusContainer.h"
#include "MuonCondData/MdtDeadTubeStatus.h"
#include "MuonCondInterface/IMuonDetectorStatusDbTool.h" 
#include "MuonCondTool/MuonDetectorStatusDbTool.h"
//#include "MuonCondSvc/MuonDetectorStatusDbSvc.h"
#include "MuonCondSvc/MdtStringUtils.h"

MuonDetectorStatusDbTool::MuonDetectorStatusDbTool (const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : AthAlgTool(type, name, parent)  
{

  declareInterface< IMuonDetectorStatusDbTool >(this) ;

  
  m_tubeStatusDataLocation="TubeStatusKey";
  m_mdtIdHelper = 0;
  
  declareProperty("TubeFolder",m_tubeFolder="/OFFLINE_FINAL/OFFLINE_FINAL_DEAD");
 

}

//StatusCode MuonDetectorStatusDbTool::updateAddress(SG::TransientAddress* tad)
StatusCode MuonDetectorStatusDbTool::updateAddress(StoreID::type /*storeID*/, SG::TransientAddress* tad)
{
    MsgStream log(msgSvc(), name());
    CLID clid        = tad->clID();
    std::string key  = tad->name();
    if ( 228145 == clid && m_tubeStatusDataLocation == key)
    {
        log << MSG::DEBUG << "OK Tube Status" << endreq;
        return StatusCode::SUCCESS;
    }
    return StatusCode::FAILURE;
}

StatusCode MuonDetectorStatusDbTool::initialize()
{ 
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initializing " << endreq;
 
  StatusCode sc = serviceLocator()->service("DetectorStore", m_detStore);
  if ( sc.isSuccess() ) {
    log << MSG::DEBUG << "Retrieved DetectorStore" << endreq;
  }else{
    log << MSG::ERROR << "Failed to retrieve DetectorStore" << endreq;
    return sc;
  }




  // Get interface to IOVSvc
  m_IOVSvc = 0;
  bool CREATEIF(true);
  sc = service( "IOVSvc", m_IOVSvc, CREATEIF );
  if ( sc.isFailure() )
  {
       log << MSG::ERROR << "Unable to get the IOVSvc" << endreq;
       return StatusCode::FAILURE;
  }

    
  //**************************************************************************
  const DataHandle<CondAttrListCollection> tubeStatusData;
  sc=m_detStore->regFcn(&IMuonDetectorStatusDbTool::loadTubeStatus,
                        dynamic_cast<IMuonDetectorStatusDbTool*>(this),
                        tubeStatusData, m_tubeFolder);
  
  if(sc.isFailure()) return StatusCode::FAILURE;

 
  
  
  m_chamberStatusData =  new MdtDeadChamberStatus();
  sc = m_detStore->record(m_chamberStatusData,m_tubeStatusDataLocation);
  if (sc == StatusCode::FAILURE) {
    log << MSG::ERROR << "Cannot record Dead Chamber map in the detector store"
        << endreq;
    return sc;
  }



  // Get the TransientAddress from DetectorStore and set "this" as the
  // AddressProvider
  SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<MdtDeadChamberStatus>::ID(), m_tubeStatusDataLocation);

 
  if (!proxy) {
    log << MSG::ERROR << "Unable to get the proxy for class TubeStatusContainer" << endreq;
    return StatusCode::FAILURE;
  }

  SG::TransientAddress* tad =  proxy->transientAddress();
  if (!tad) {
    log << MSG::ERROR << "Unable to get the tad" << endreq;
    return StatusCode::FAILURE;
  }

  IAddressProvider* addp = this;
  tad->setProvider(addp, StoreID::DETECTOR_STORE);
  //tad->setProvider(addp);
  log << MSG::DEBUG << "set address provider for TubeStatusContainer" << endreq;

                                                                           
  
  return sc;
}

StatusCode MuonDetectorStatusDbTool::loadParameterStatus(IOVSVC_CALLBACK_ARGS_P(I,keys)){
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "LoadParameters has been triggered for the following keys " << endreq;
  
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    log << MSG::INFO << *itr << " I="<<I<<" ";
     if(*itr==m_tubeFolder) {
       loadTubeStatus(I,keys);
       
     }

    
  }
  log << MSG::INFO << endreq;
  
 
  
  return StatusCode::SUCCESS;


}





StatusCode MuonDetectorStatusDbTool::loadTubeStatus(IOVSVC_CALLBACK_ARGS_P(I,keys)) 
{
  
  MsgStream log(msgSvc(), name());
   StatusCode sc=StatusCode::SUCCESS;
   log << MSG::INFO << "Load dead tubes  from DB" << endreq;
   
   // Print out callback information
   log << MSG::DEBUG << "Level " << I << " Keys: ";
   std::list<std::string>::const_iterator keyIt = keys.begin();
   for (; keyIt != keys.end(); ++ keyIt) log << MSG::DEBUG << *keyIt << " ";
   log << MSG::DEBUG << endreq;

   sc = m_detStore->retrieve( m_chamberStatusData, m_tubeStatusDataLocation );
   if(sc.isSuccess())  {
     log << MSG::DEBUG << "ChamberStatus Map found " << m_chamberStatusData << endreq;
     m_detStore->remove( m_chamberStatusData );
     log << MSG::DEBUG << "TubeStatuse Collection at " << m_chamberStatusData << " removed "<<endreq;
   }
   
  

   
  const CondAttrListCollection * atrc;
  sc=m_detStore->retrieve(atrc,m_tubeFolder);
  if(sc.isFailure())  {
    log << MSG::ERROR 
	<< "could not retreive the CondAttrListCollection from DB folder " 
	<< m_tubeFolder << endreq;
    return sc;
  }
  m_chamberStatusData =  new MdtDeadChamberStatus();
   
  
  MdtDeadChamberStatus::MdtMultilayerData deadMultilayerData;
   std::vector<std::vector<int> > deadLayerTubeVector;
   std::vector<int>  deadTubeVector;
   m_chamberStatusData->clear();
   unsigned int multilayer_old=0, layer_old=0;
   

   CondAttrListCollection::const_iterator itr;
   for (itr = atrc->begin(); itr != atrc->end(); ++itr) {
     const coral::AttributeList& atr=itr->second;
     std::string multilayer_list, mezzanine_list, tube_list;

     multilayer_list=*(static_cast<const std::string*>((atr["Dead_multilayer"]).addressOfData()));
     mezzanine_list=*(static_cast<const std::string*>((atr["Dead_mezzanine"]).addressOfData()));
     tube_list=*(static_cast<const std::string*>((atr["Dead_tube"]).addressOfData()));

     log << MSG::DEBUG << "multilayer load is " << multilayer_list << endreq;
     log << MSG::DEBUG << "mezzanine load is " << mezzanine_list << endreq;
     log << MSG::DEBUG << "tube load is " << tube_list << endreq;
     
     std::string delimiter = ",";
     std::vector<std::string> info_tube,info_multilayer;
     
     MuonCalib::MdtStringUtils::tokenize(multilayer_list,info_multilayer,delimiter);
     MuonCalib::MdtStringUtils::tokenize(tube_list,info_tube,delimiter);
    
     log << MSG::DEBUG << " parsing of the tube_list" << endreq;
     
     unsigned int number, mlayerint, layerint, tubeint;
     // unsigned int status=1;
     char *ch_tmp;
     
     log << MSG::VERBOSE << "multilayer size is " << info_multilayer.size() << endreq;

     for(unsigned int i=0; i<info_multilayer.size();i++){
       log<< MSG::VERBOSE << i << "..."<< info_multilayer[i]<< endreq;  

     }

     deadTubeVector.clear();  
     deadLayerTubeVector.clear(); 
     deadMultilayerData.clear();
     multilayer_old= layer_old= 0;
     int numbertube= atoi(const_cast<char*>(info_tube[1].c_str()));
     
     if (numbertube!=999){
       for(unsigned int i=2; i<info_tube.size();i++){
	
	 log<< MSG::VERBOSE << i << "..."<< info_tube[i]<< endreq;
	 ch_tmp= const_cast<char*>(info_tube[i].c_str());
	 number= atoi(ch_tmp);
	 
	 mlayerint=  number/1000;
	 layerint= (number- mlayerint*1000)/100;
	 tubeint= number- mlayerint*1000 - layerint*100;
	 if(layer_old!=layerint ){
	   if(layer_old!=0){
	     deadLayerTubeVector.push_back(deadTubeVector);
	     deadTubeVector.clear();
	   }  
	   layer_old= layerint;
	 }
	 if(multilayer_old!=mlayerint ){
	   if(multilayer_old!=0){
	     deadMultilayerData.insert(make_pair(multilayer_old,deadLayerTubeVector ));
	     deadLayerTubeVector.clear();
	   }
	   multilayer_old= mlayerint;  
	 }
	 
	 deadTubeVector.push_back(tubeint);
       }
     
       deadLayerTubeVector.push_back(deadTubeVector);  
       deadMultilayerData.insert(make_pair(multilayer_old,deadLayerTubeVector ));
       m_chamberStatusData->addChamberDead((std::string)info_tube[0],(MdtDeadChamberStatus::MdtMultilayerData)deadMultilayerData);
       
     }
     
     if (numbertube == 999){
       
       
       int multilayer_allchamber(0); 
     
       deadTubeVector.push_back(999);
       deadLayerTubeVector.push_back(deadTubeVector); 
       if (info_multilayer.size()==3){
	 log<< MSG::VERBOSE << "ALL CHAMBER OFF!!!"<< endreq;
	 multilayer_allchamber=999;
       }
       if (info_multilayer.size()==2){
	 char * multi_ch= const_cast<char*>(info_multilayer[1].c_str()); 
	 multilayer_allchamber=atoi(multi_ch);
	 log<< MSG::VERBOSE << "MULTILAYER "<<multilayer_allchamber  << endreq;
	 
       }
      if (info_multilayer.size()==1){
	 log<< MSG::VERBOSE << "ERROR"<< endreq;
       }
       deadMultilayerData.insert(make_pair(multilayer_allchamber,deadLayerTubeVector ));
       m_chamberStatusData->addChamberDead((std::string)info_tube[0],(MdtDeadChamberStatus::MdtMultilayerData)deadMultilayerData);
       
     }
     

   }
   
   
   log << MSG::VERBOSE << "Collection CondAttrListCollection CLID "
       << atrc->clID() << endreq;
 
   IOVRange range;
   m_IOVSvc->getRange(1238547719, m_tubeFolder, range);
   
   log << MSG::DEBUG <<"CondAttrListCollection IOVRange "<<range<<endreq;
   
   IOVRange range2;
   m_IOVSvc->setRange(228145, m_tubeStatusDataLocation, range);
   m_IOVSvc->getRange(228145, m_tubeStatusDataLocation, range2);
   log << MSG::DEBUG <<"TubeStatusContainer new IOVRange "<<range2<<endreq;
   

   m_detStore->record( m_chamberStatusData, m_tubeStatusDataLocation );
   log << MSG::DEBUG << "Number  enries in new DeadChamberStatus Map " 
       <<  m_chamberStatusData->nDeadChambers() << endreq;
   log << MSG::DEBUG << "\n----------\nPrintout DeadChamberStatus Map \n" 
       <<  m_chamberStatusData->str() << endreq;


 
   SG::DataProxy* proxy = m_detStore->proxy(ClassID_traits<MdtDeadChamberStatus>::ID(), m_tubeStatusDataLocation);
   if (!proxy) {
     log << MSG::ERROR << "Unable to get the proxy for class MdtDeadChamberStatus" << endreq;
     return StatusCode::FAILURE;
   }


   
   SG::TransientAddress* tad =  proxy->transientAddress();
   if (!tad) {
     log << MSG::ERROR << "Unable to get the tad" << endreq;
     return StatusCode::FAILURE;
   }
   
   IAddressProvider* addp = this;
   tad->setProvider(addp, StoreID::DETECTOR_STORE);
   //tad->setProvider(addp);
   log << MSG::DEBUG<< "set address provider for TubeStatusContainer" << endreq;
   
   return  sc; 
   
   
   
}




