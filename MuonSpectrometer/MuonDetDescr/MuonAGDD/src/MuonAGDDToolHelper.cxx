/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDD/MuonAGDDToolHelper.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDModel/AGDDParameterStore.h"
#include "AGDDControl/AGDD2GeoModelBuilder.h"
#include "AGDD2GeoSvc/IAGDD2GeoSvc.h"
#include "AGDDKernel/AliasStore.h"
#include "AGDDKernel/AGDDDetector.h"
#include "AGDDKernel/AGDDDetectorStore.h"
#include "AGDD2GeoSvc/IAGDD2GeoSvc.h"

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonAGDDBase/micromegasHandler.h"
#include "MuonAGDDBase/mm_TechHandler.h"
#include "MuonAGDDBase/sTGCHandler.h"
#include "MuonAGDDBase/sTGC_readoutHandler.h"
#include "MuonAGDDBase/sTGC_TechHandler.h"
#include "MuonAGDDBase/mmSpacerHandler.h"
#include "MuonAGDDBase/mmSpacer_TechHandler.h"
#include "MuonAGDDBase/mm_readoutHandler.h"

#include <fstream>

using namespace MuonGM;

MuonAGDDToolHelper::MuonAGDDToolHelper()
{
	StatusCode result;
	
	result=Gaudi::svcLocator()->service("GeoModelSvc",p_GeoModelSvc);
  	if (result.isFailure())
    {
    	std::cout<<"unable to access GeoModelSvc "<<std::endl;
    }
    result=Gaudi::svcLocator()->service("RDBAccessSvc",p_RDBAccessSvc);
    if (result.isFailure())
    {
    	std::cout<<"unable to access RBDAccessSvc "<<std::endl;
    }
	m_tagInfoKey="";
	result=Gaudi::svcLocator()->service("TagInfoMgr",m_tagInfoMgr);
  	if (result.isFailure()) 
  	{
    	std::cout<<"Unable to retrieve TagInfoMgr!"<<std::endl;
  	}
  	else
  		m_tagInfoKey=m_tagInfoMgr->tagInfoKey();
}

std::vector<std::string>& MuonAGDDToolHelper::ReadAGDDFlags()
{
	static std::vector<std::string> structuresFromFlags;
	structuresFromFlags.clear();
   std::string agdd2geoVersion = p_RDBAccessSvc->getChildTag("AGDD2GeoSwitches",p_GeoModelSvc->muonVersion(),"MuonSpectrometer");

//   m_AGDD2GeoSwitches.clear();
   if(!agdd2geoVersion.empty()) 
   {
//     m_AGDD2GeoSwitchesStamp = m_AGDD2GeoSwitchesStamp + 1;
     std::string TheKEYNAME;
     int TheKEYVALUE;
     IRDBRecordset_ptr pIRDBRecordset = p_RDBAccessSvc->getRecordsetPtr("AGDD2GeoSwitches",p_GeoModelSvc->muonVersion(),"MuonSpectrometer");
     for(unsigned int i=0; i<pIRDBRecordset->size(); i++) 
     {
       const IRDBRecord* record = (*pIRDBRecordset)[i];
       TheKEYNAME = record->getString("KEYNAME");
       TheKEYVALUE = record->getInt("KEYVALUE");
       if ( TheKEYVALUE == 1 )
       {
//        std::cout<<"  Add to m_AGDD2GeoSwitches " << TheKEYNAME <<std::endl;
//		 if (aliases->IsAliased(TheKEYNAME))
         structuresFromFlags.push_back(TheKEYNAME);
       }
     }
   }
   else
   {
      std::cout<<"  agdd2geoVersion is empty " <<std::endl;
   }	
   return structuresFromFlags;
}


std::string MuonAGDDToolHelper::GetAGDD(bool dumpIt)
{
	
   const IGeoModelSvc * geoModel=p_GeoModelSvc;

   IRDBAccessSvc * accessSvc=p_RDBAccessSvc;

   std::string AtlasVersion = geoModel->atlasVersion();
   std::string MuonVersion  = geoModel->muonVersionOverride();
   
   std::string detectorKey  = MuonVersion.empty() ? AtlasVersion : MuonVersion;
   std::string detectorNode = MuonVersion.empty() ? "ATLAS" : "MuonSpectrometer";
   if ( MuonVersion == "CUSTOM"){
     detectorKey  = AtlasVersion ;
     detectorNode = "ATLAS"  ;
   } 


   IRDBRecordset_ptr recordsetAGDD = accessSvc->getRecordsetPtr("AGDD",detectorKey,detectorNode);

   const IRDBRecord *recordAGDD =  (*recordsetAGDD)[0];
   std::string AgddString = recordAGDD->getString("DATA");

   size_t pos=AgddString.find("AGDD.dtd");
   if (pos!=std::string::npos) AgddString.replace(pos-21,32,"-- Reference to AGDD.dtd automatically removed -->");
   std::ofstream  GeneratedFile;
   if (dumpIt) 
   {
	 	std::ofstream  GeneratedFile;
	 	GeneratedFile.open("generated_amdb_simrec_pool.txt");
		GeneratedFile<<AgddString<<std::endl;
		GeneratedFile.close();
   }
	 
   return AgddString;

}

StatusCode MuonAGDDToolHelper::SetTagInfo(std::string tag,std::string info) const
{
	StatusCode result;
	if (m_tagInfoMgr)
	{
		result=m_tagInfoMgr->addTag(tag,info);
		if (result.isFailure())
		{
//			ATH_MSG_ERROR("SetTagInfo: could not add tag "<<tag<<" value "<<
//				info<<" to TagInfo ");
		}
		return result;
	}
	else
	{
//		ATH_MSG_ERROR(" SetTagInfo: the TagInfoMgr is not set!");
		return StatusCode::FAILURE;
	}
}


bool MuonAGDDToolHelper::BuildMScomponents() const
{

//  ATH_MSG_INFO("BuildMScomponents - start");

  StoreGateSvc* pDetStore=0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc=svcLocator->service("DetectorStore",pDetStore);
  if (sc.isFailure()) 
    {
//      ATH_MSG_ERROR("AGDDController could not get at the detector store!");
    }
  MuonGM::MuonDetectorManager*	muonMgr;
  sc = pDetStore->retrieve( muonMgr );
  if ( sc.isFailure() ) {
//   ATH_MSG_ERROR(" Cannot retrieve MuonDetectorManager " );
   return false;
  }

  bool readoutGeoDone =  BuildReadoutGeometry(muonMgr/*, GetMSdetectors*/);
  if (!readoutGeoDone) {
//   ATH_MSG_INFO(" Problems met while building the ReadoutGeometry " );
   return false;
  }
//  else ATH_MSG_INFO( "ReadoutGeometry created" );
//  ATH_MSG_INFO("In BuildMScomponents - done");

  return true;
}

bool MuonAGDDToolHelper::BuildReadoutGeometry(MuonGM::MuonDetectorManager* mgr/*, std::map<GeoFullPhysVol*, std::string>* vec*/) const
{
  bool geoBuilt = true;

//  ATH_MSG_INFO("In BuildReadoutGeometry - start");

  //std::map<std::string, GeoFullPhysVol*>* myMap =  NULL;
  //myMap = GetMSdetectors();
  //log<<MSG::INFO<<"In AGDD2GeoSvc::BuildReadoutGeometry - size of the detector map = "<<myMap->size()<<endmsg;

  //std::map<std::string, GeoFullPhysVol*>::const_iterator it;
  
  detectorList& dList=AGDDDetectorStore::GetDetectorStore()->GetDetectorList();
  detectorList::const_iterator it;
  for (it=dList.begin(); it!=dList.end(); ++it)
  {
  	  std::vector<AGDDDetectorPositioner*>& dPos=((*it).second)->GetDetectorPositioners();;
	  for (unsigned int i=0;i<dPos.size();i++)
      {
      std::string chTag = dPos[i]->ID.detectorAddress;
      GeoFullPhysVol* vol = dPos[i]->theVolume;
//      ATH_MSG_INFO("Building RE for component named <"<<chTag<<">");
      
      std::string stName = chTag.substr(0,4);
      
      int etaIndex = 999;
      int phiIndex = 999;
      int mLayer   = 999;
      int iSide    = 0;
      int iLS      = atoi((chTag.substr(3,1)).c_str()); //sTG3 and sMD3 are small chambers for small sectors 
      if (iLS==3) iLS = 1; // small 
      else iLS = 0; // large 
      if (chTag.substr(13,1)=="A") iSide=1;
      else if (chTag.substr(13,1)=="C") iSide=-1;
      etaIndex = iSide*atoi((chTag.substr(5,1)).c_str());
      phiIndex = atoi((chTag.substr(12,1)).c_str());
      mLayer = atoi((chTag.substr(7,1)).c_str());
      
      //MuonReadoutElement* re = NULL;
      if (chTag.substr(0,3)=="sMD")
	  {
	  MMReadoutElement* re = new MMReadoutElement((GeoVFullPhysVol*)vol, stName, etaIndex, phiIndex, mLayer, false, mgr);
	  std::string myVolName = (chTag.substr(0,8)).c_str();
	  /* AGDDParameterBagMM* MMparaBag = dynamic_cast<AGDDParameterBagMM*> (AGDDParameterStore::GetParameterStore()->GetParameterBag(myVolName));
	  if(!MMparaBag) {
	    std::cout << " not possible to retrieve parameters for <" << myVolName << ": quitting! >" << std::endl;
		return false;
	  }
	  double pitch;
	  if(iLS==1) pitch = MMparaBag->TechParameters->pitchSS; //small
	  else pitch = MMparaBag->TechParameters->pitchLS; //large
	  re->initDesign(MMparaBag->largeX, MMparaBag->smallX, MMparaBag->lengthY, pitch, MMparaBag->TechParameters->thickness); */
	  re->initDesign(-999., -999., -999., -999., -999.);
	  re->fillCache();
	  mgr->addMMReadoutElement_withIdFields(re, iLS, etaIndex, phiIndex, mLayer);
	  }
      else if (chTag.substr(0,3)=="sTG")
	  {
	  sTgcReadoutElement* re = new sTgcReadoutElement((GeoVFullPhysVol*)vol, stName, etaIndex, phiIndex, mLayer, false, mgr);	  
	  std::string myVolName = (chTag.substr(0,8)).c_str();
	  /*
	  AGDDParameterBagsTGC* sTGCparaBag = dynamic_cast<AGDDParameterBagsTGC*> (AGDDParameterStore::GetParameterStore()->GetParameterBag(myVolName));
	  if(!sTGCparaBag) {
	    std::cout << " not possible to retrieve parameters for <" << myVolName << ": quitting! >" << std::endl;
		return false;
	  }
	  re->initDesign(sTGCparaBag->largeX, sTGCparaBag->smallX, sTGCparaBag->lengthY, sTGCparaBag->TechParameters->stripPitch,
			 sTGCparaBag->TechParameters->wirePitch, sTGCparaBag->TechParameters->stripWidth, sTGCparaBag->TechParameters->wireWidth,
			 sTGCparaBag->TechParameters->thickness);
	  
	  since the concept of Parameter Bag is dismissed things get hardcoded
	  initDesign for sTGCReadoutElement only uses stripPitch, strip Width and thickness - all other dimensions (here -999.) are (hard)coded there again
	  */
	  re->initDesign(-999., -999., -999., 3.2, -999., 2.7, -999., 2.6);
	  re->fillCache();
	  mgr->addsTgcReadoutElement_withIdFields(re, iLS, etaIndex, phiIndex, mLayer);
	  }
	  }

      //aggdContainer->push_back(*it);
  }
  //std::cout<<"Size of the NSWdetectors = <"<<vec->size()<<"> "<<aggdContainer->size()<<std::cout;
  //sc=pDetStore->record( aggdContainer ,"NSWdetectors");
  return geoBuilt;

}

void MuonAGDDToolHelper::SetNSWComponents()
{
	IAGDDtoGeoSvc* agddsvc;
	StatusCode result;
	result=Gaudi::svcLocator()->service("AGDDtoGeoSvc",agddsvc);
	if (result.isFailure())
    {
    	std::cout<<"could not initialize AGDDtoGeoSvc!!! "<<std::endl;
    }
	
	agddsvc->addHandler(new micromegasHandler("micromegas"));
	agddsvc->addHandler(new mm_TechHandler("mm_Tech"));
	agddsvc->addHandler(new sTGCHandler("sTGC"));
	agddsvc->addHandler(new sTGC_readoutHandler("sTGC_readout"));
	agddsvc->addHandler(new sTGC_TechHandler("sTGC_Tech"));
	agddsvc->addHandler(new mmSpacerHandler("mmSpacer"));
	agddsvc->addHandler(new mmSpacer_TechHandler("mmSpacer_Tech"));
	agddsvc->addHandler(new mm_readoutHandler("mm_readout"));
}	
