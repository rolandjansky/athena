/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
  : AthAlgTool(type, name, parent),
    m_IOVSvc ("IOVSvc", name)
{

  declareInterface< IMuonDetectorStatusDbTool >(this) ;

  
  m_tubeStatusDataLocation="TubeStatusKey";
  m_mdtIdHelper = 0;
  m_chamberStatusData = 0;
  
  declareProperty("TubeFolder",m_tubeFolder="/OFFLINE_FINAL/OFFLINE_FINAL_DEAD");
 

}

//StatusCode MuonDetectorStatusDbTool::updateAddress(SG::TransientAddress* tad)
StatusCode MuonDetectorStatusDbTool::updateAddress(StoreID::type /*storeID*/,
                                                   SG::TransientAddress* tad,
                                                   const EventContext& /*ctx*/)
{
    CLID clid        = tad->clID();
    std::string key  = tad->name();
    if ( 228145 == clid && m_tubeStatusDataLocation == key)
    {
        ATH_MSG_DEBUG( "OK Tube Status" );
        return StatusCode::SUCCESS;
    }
    return StatusCode::FAILURE;
}

StatusCode MuonDetectorStatusDbTool::initialize()
{ 
  ATH_MSG_INFO( "Initializing " );
  ATH_CHECK( m_IOVSvc.retrieve() );
 
  //**************************************************************************
  const DataHandle<CondAttrListCollection> tubeStatusData;
  ATH_CHECK( detStore()->regFcn(&IMuonDetectorStatusDbTool::loadTubeStatus,
                                dynamic_cast<IMuonDetectorStatusDbTool*>(this),
                                tubeStatusData, m_tubeFolder) );
  
  
  m_chamberStatusData =  new MdtDeadChamberStatus();
  ATH_CHECK( detStore()->record(m_chamberStatusData,m_tubeStatusDataLocation) );

  // Get the TransientAddress from DetectorStore and set "this" as the
  // AddressProvider
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<MdtDeadChamberStatus>::ID(), m_tubeStatusDataLocation);

 
  if (!proxy) {
    ATH_MSG_ERROR( "Unable to get the proxy for class TubeStatusContainer" );
    return StatusCode::FAILURE;
  }

  IAddressProvider* addp = this;
  proxy->setProvider(addp, StoreID::DETECTOR_STORE);
  //tad->setProvider(addp);
  ATH_MSG_DEBUG( "set address provider for TubeStatusContainer" );
  
  return StatusCode::SUCCESS;
}

StatusCode MuonDetectorStatusDbTool::loadParameterStatus(IOVSVC_CALLBACK_ARGS_P(I,keys)){
  ATH_MSG_INFO( "LoadParameters has been triggered for the following keys " );
  
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    msg(MSG::INFO) << *itr << " I="<<I<<" ";
     if(*itr==m_tubeFolder) {
       loadTubeStatus(I,keys);
       
     }

    
  }
  msg() << endmsg;
  
  return StatusCode::SUCCESS;
}





StatusCode MuonDetectorStatusDbTool::loadTubeStatus(IOVSVC_CALLBACK_ARGS_P(I,keys)) 
{
   ATH_MSG_INFO( "Load dead tubes  from DB" );
   
   // Print out callback information
   msg(MSG::DEBUG) << "Level " << I << " Keys: ";
   std::list<std::string>::const_iterator keyIt = keys.begin();
   for (; keyIt != keys.end(); ++ keyIt)
     msg(MSG::DEBUG) << *keyIt << " ";
   msg(MSG::DEBUG) << endmsg;

   StatusCode sc = detStore()->retrieve( m_chamberStatusData, m_tubeStatusDataLocation );
   if(sc.isSuccess())  {
     ATH_MSG_DEBUG( "ChamberStatus Map found " << m_chamberStatusData );
     detStore()->remove( m_chamberStatusData );
     ATH_MSG_DEBUG( "TubeStatuse Collection at " << m_chamberStatusData << " removed ");
   }
   
   const CondAttrListCollection * atrc;
   ATH_CHECK( detStore()->retrieve(atrc,m_tubeFolder) );
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

     ATH_MSG_DEBUG( "multilayer load is " << multilayer_list );
     ATH_MSG_DEBUG( "mezzanine load is " << mezzanine_list );
     ATH_MSG_DEBUG( "tube load is " << tube_list );
     
     std::string delimiter = ",";
     std::vector<std::string> info_tube,info_multilayer;
     
     MuonCalib::MdtStringUtils::tokenize(multilayer_list,info_multilayer,delimiter);
     MuonCalib::MdtStringUtils::tokenize(tube_list,info_tube,delimiter);
    
     ATH_MSG_DEBUG( " parsing of the tube_list" );
     
     unsigned int number, mlayerint, layerint, tubeint;
     // unsigned int status=1;
     char *ch_tmp;
     
     ATH_MSG_VERBOSE( "multilayer size is " << info_multilayer.size() );

     for(unsigned int i=0; i<info_multilayer.size();i++){
       ATH_MSG_VERBOSE( i << "..."<< info_multilayer[i]);
     }

     deadTubeVector.clear();  
     deadLayerTubeVector.clear(); 
     deadMultilayerData.clear();
     multilayer_old= layer_old= 0;
     int numbertube= atoi(const_cast<char*>(info_tube[1].c_str()));
     
     if (numbertube!=999){
       for(unsigned int i=2; i<info_tube.size();i++){
	
	 ATH_MSG_VERBOSE( i << "..."<< info_tube[i]);
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
	 ATH_MSG_VERBOSE( "ALL CHAMBER OFF!!!");
	 multilayer_allchamber=999;
       }
       if (info_multilayer.size()==2){
	 char * multi_ch= const_cast<char*>(info_multilayer[1].c_str()); 
	 multilayer_allchamber=atoi(multi_ch);
	 ATH_MSG_VERBOSE( "MULTILAYER "<<multilayer_allchamber  );
       }
       if (info_multilayer.size()==1){
	 ATH_MSG_VERBOSE( "ERROR");
       }
       deadMultilayerData.insert(make_pair(multilayer_allchamber,deadLayerTubeVector ));
       m_chamberStatusData->addChamberDead((std::string)info_tube[0],(MdtDeadChamberStatus::MdtMultilayerData)deadMultilayerData);
       
     }
   }
   
   ATH_MSG_VERBOSE( "Collection CondAttrListCollection CLID "
                    << atrc->clID() );
 
   IOVRange range;
   m_IOVSvc->getRange(1238547719, m_tubeFolder, range);
   
   ATH_MSG_DEBUG("CondAttrListCollection IOVRange "<<range);
   
   IOVRange range2;
   m_IOVSvc->setRange(228145, m_tubeStatusDataLocation, range);
   m_IOVSvc->getRange(228145, m_tubeStatusDataLocation, range2);
   ATH_MSG_DEBUG("TubeStatusContainer new IOVRange "<<range2);
   

   ATH_CHECK( detStore()->record( m_chamberStatusData, m_tubeStatusDataLocation ) );
   ATH_MSG_DEBUG( "Number  enries in new DeadChamberStatus Map " 
                  <<  m_chamberStatusData->nDeadChambers() );
   ATH_MSG_DEBUG( "\n----------\nPrintout DeadChamberStatus Map \n" 
                  <<  m_chamberStatusData->str() );


   SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<MdtDeadChamberStatus>::ID(), m_tubeStatusDataLocation);
   if (!proxy) {
     ATH_MSG_ERROR( "Unable to get the proxy for class MdtDeadChamberStatus" );
     return StatusCode::FAILURE;
   }
   
   IAddressProvider* addp = this;
   proxy->setProvider(addp, StoreID::DETECTOR_STORE);
   //tad->setProvider(addp);
   ATH_MSG_DEBUG( "set address provider for TubeStatusContainer" );
   
   return StatusCode::SUCCESS;
}
