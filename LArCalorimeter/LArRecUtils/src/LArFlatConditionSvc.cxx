/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFlatConditionSvc.h"

#include "AthenaKernel/IOVRange.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "AthenaKernel/StorableConversions.h"
#include "SGTools/DataProxy.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "LArCOOLConditions/LAruA2MeVFlat.h"	 
#include "LArCOOLConditions/LArDAC2uAFlat.h"
#include "LArCOOLConditions/LArHVScaleCorrFlat.h"
#include "LArCOOLConditions/LArPedestalFlat.h"	 
#include "LArCOOLConditions/LArRampFlat.h"	 
#include "LArCOOLConditions/LArMphysOverMcalFlat.h"
#include "LArCOOLConditions/LArOFCFlat.h"
#include "LArCOOLConditions/LArShapeFlat.h"      

#include "LArCOOLConditions/LArAutoCorrSC.h"
#include "LArCOOLConditions/LAruA2MeVSC.h"	 
#include "LArCOOLConditions/LArDAC2uASC.h"
#include "LArCOOLConditions/LArfSamplSC.h"
#include "LArCOOLConditions/LArNoiseSC.h"	 
#include "LArCOOLConditions/LArPedestalSC.h"	 
#include "LArCOOLConditions/LArRampSC.h"
#include "LArCOOLConditions/LArShapeSC.h"
#include "LArCOOLConditions/LArMinBiasSC.h"

#include "CoralBase/Blob.h"
#include <boost/crc.hpp> 


LArFlatConditionSvc::LArFlatConditionSvc( const std::string& name, ISvcLocator* svc )
  : AthService( name, svc ),
    m_IOVSvc     ("IOVSvc", name),
    m_detStore   ("DetectorStore", name),
    m_clidSvc    ("ClassIDSvc", name),
    m_objInfo(17),
    m_attrListClid(),
    m_initializing(true),
    m_doRegularCells(true),
    m_doSuperCells(false)
{

  //Object names for regular cells
  m_objInfo[0].m_classname="LAruA2MeVFlat";	 
  m_objInfo[1].m_classname="LArDAC2uAFlat";	      
  m_objInfo[2].m_classname="LArHVScaleCorrFlat";
  m_objInfo[3].m_classname="LArPedestalFlat";	      
  m_objInfo[4].m_classname="LArRampFlat";	 
  m_objInfo[5].m_classname="LArMphysOverMcalFlat"; 
  m_objInfo[6].m_classname="LArOFCFlat";     
  m_objInfo[7].m_classname="LArShapeFlat";       
  //Interface names for regular cells
  m_objInfo[0].m_ifacename="ILAruA2MeV";
  m_objInfo[1].m_ifacename="ILArDAC2uA"; 
  m_objInfo[2].m_ifacename="ILArHVScaleCorr";
  m_objInfo[3].m_ifacename="ILArPedestal";	
  m_objInfo[4].m_ifacename="ILArRamp";
  m_objInfo[5].m_ifacename="ILArMphysOverMcal";
  m_objInfo[6].m_ifacename="ILArOFC";	
  m_objInfo[7].m_ifacename="ILArShape";

  //Object names for SuperCell conditions
  m_objInfo[8].m_classname="LArAutoCorrSC";
  m_objInfo[9].m_classname="LAruA2MeVSC";
  m_objInfo[10].m_classname="LArDAC2uASC"; 
  m_objInfo[11].m_classname="LArfSamplSC"; 
  m_objInfo[12].m_classname="LArNoiseSC"; 
  m_objInfo[13].m_classname="LArPedestalSC"; 
  m_objInfo[14].m_classname="LArRampSC"; 
  m_objInfo[15].m_classname="LArShapeSC"; 
  m_objInfo[16].m_classname="LArMinBiasSC"; 

  //Interface names for SuperCell conditions
  m_objInfo[8].m_ifacename="ILArAutoCorr";
  m_objInfo[9].m_ifacename="ILAruA2MeV";
  m_objInfo[10].m_ifacename="ILArDAC2uA"; 
  m_objInfo[11].m_ifacename="ILArfSampl"; 
  m_objInfo[12].m_ifacename="ILArNoise"; 
  m_objInfo[13].m_ifacename="ILArPedestal"; 
  m_objInfo[14].m_ifacename="ILArRamp"; 
  m_objInfo[15].m_ifacename="ILArShape"; 
  m_objInfo[16].m_ifacename="ILArMinBias"; 
  

  //StoreGate keys for conditions objects for regular cells
  declareProperty("uA2MeVOutput",       m_objInfo[0].m_outputKey="LAruA2MeV");
  declareProperty("DAC2uAVOutput",      m_objInfo[1].m_outputKey="LArDAC2uA");
  declareProperty("HVScaleCorrOutput",  m_objInfo[2].m_outputKey="LArHVScaleCorr");
  declareProperty("PedestalOutput",     m_objInfo[3].m_outputKey="LArPedestal");
  declareProperty("RampOutput",         m_objInfo[4].m_outputKey="LArRamp");
  declareProperty("MphysOverMcalOutput",m_objInfo[5].m_outputKey="LArMphysOverMcal");
  declareProperty("OFCOutput",          m_objInfo[6].m_outputKey="LArOFC");
  declareProperty("ShapeOutput",        m_objInfo[7].m_outputKey="LArShape");

  //StoreGate keys for conditions objects for SuperCells
  declareProperty("AutoCorrSCOutput",   m_objInfo[8].m_outputKey="LArAutoCorrSC");
  declareProperty("uA2MeVSCOutput",       m_objInfo[9].m_outputKey="LAruA2MeVSC");
  declareProperty("DAC2uAVSCOutput",    m_objInfo[10].m_outputKey="LArDAC2uASC");
  declareProperty("fSamplSCOutput",     m_objInfo[11].m_outputKey="LARfSamplSC");
  declareProperty("NoiseSCOutput",      m_objInfo[12].m_outputKey="LArNoiseSC");
  declareProperty("PedestalSCOutput",   m_objInfo[13].m_outputKey="LArPedestalSC");
  declareProperty("RampSCOutput",       m_objInfo[14].m_outputKey="LArRampSC");	    
  declareProperty("ShapeSCOutput",      m_objInfo[15].m_outputKey="LArShapeSC");	    
  declareProperty("MinBiasSCOutput",    m_objInfo[16].m_outputKey="LArMinBiasSC");	    


  //Folder names for flat conditions data for regular cells
  declareProperty("uA2MeVInput",        m_objInfo[0].m_inputKey);//="/LAR/ElecCalibFlat/uA2MeV");
  declareProperty("DAC2uAInput",       m_objInfo[1].m_inputKey);//="/LAR/ElecCalibFlat/DAC2uA");	  
  declareProperty("HVScaleCorrInput",   m_objInfo[2].m_inputKey);//="/LAR/ElecCalibFlat/HVScaleCorr");
  declareProperty("PedestalInput",      m_objInfo[3].m_inputKey);//="/LAR/ElecCalibFlat/Pedestal");
  declareProperty("RampInput",          m_objInfo[4].m_inputKey);//="/LAR/ElecCalibFlat/Ramp");
  declareProperty("MphysOverMcalInput", m_objInfo[5].m_inputKey);//="/LAR/ElecCalibFlat/MphysOverMcal");
  declareProperty("OFCInput",           m_objInfo[6].m_inputKey);//="/LAR/ElecCalibFlat/OFC");
  declareProperty("ShapeInput",         m_objInfo[7].m_inputKey);//="/LAR/ElecCalibFlat/Shape");

  //Folder names for flat conditions data for SuperCells
  declareProperty("AutoCorrSCInput",    m_objInfo[8].m_inputKey="/LAR/ElecCalibMCSC/AutoCorr");
  declareProperty("uA2MeVSCInput",      m_objInfo[9].m_inputKey="/LAR/ElecCalibMCSC/uA2MeV");    
  declareProperty("DAC2uAVSCInput",     m_objInfo[10].m_inputKey="/LAR/ElecCalibMCSC/DAC2uA");    
  declareProperty("fSamplSCInput",      m_objInfo[11].m_inputKey="/LAR/ElecCalibMCSC/fSampl");    
  declareProperty("NoiseSCInput",       m_objInfo[12].m_inputKey="/LAR/ElecCalibMCSC/Noise");     
  declareProperty("PedestalSCInput",    m_objInfo[13].m_inputKey="/LAR/ElecCalibMCSC/Pedestal");  
  declareProperty("RampSCInput",        m_objInfo[14].m_inputKey="/LAR/ElecCalibMCSC/Ramp");	     
  declareProperty("ShapeSCInput",       m_objInfo[15].m_inputKey="/LAR/ElecCalibMCSC/Shape");	     
  declareProperty("MinBiasSCInput",     m_objInfo[16].m_inputKey="/LAR/ElecCalibMCSC/MinBias");	     

  declareProperty("DoSuperCells", m_doSuperCells);
  declareProperty("DoRegularCells", m_doRegularCells);
  declareProperty("PrintChecksum",  m_printChecksum=true);
}
 
LArFlatConditionSvc::~LArFlatConditionSvc() {}
  


StatusCode LArFlatConditionSvc::initialize() {
  ATH_MSG_DEBUG("Initializing " << name());
  /*
  StatusCode sc=m_clidSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve " << m_clidSvc << endmsg;
    return sc;
  }
  
  
  sc=m_IOVSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve " << m_IOVSvc << endmsg;
    return sc;
  }
  */

  if (m_doSuperCells==false && m_doRegularCells==false) {
    msg(MSG::WARNING) << "Both DoSuperCells and DoRegularCells set to false. Do nothing ?!?" << endmsg;
  }

  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
  StatusCode sc=incSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to get the IncidentSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  incSvc->addListener( this, "BeginRun", 100 );

  const size_t nObj=m_objInfo.size();
  for (size_t i=0;i<nObj;++i) {
    CHECK(m_clidSvc->getIDOfTypeName(m_objInfo[i].m_classname,m_objInfo[i].m_clid));
    CHECK(m_clidSvc->getIDOfTypeName(m_objInfo[i].m_ifacename,m_objInfo[i].m_sclid));
  }

  CHECK(m_clidSvc->getIDOfTypeName("CondAttrListCollection",m_attrListClid));

  return StatusCode::SUCCESS;
}


StatusCode LArFlatConditionSvc::preLoadAddresses(StoreID::type storeID,tadList& tlist) {

  ATH_MSG_DEBUG("preLoadAddresses. Store:" << storeID);

  if (storeID!=StoreID::DETECTOR_STORE) return StatusCode::SUCCESS;

  size_t minIdx=0;
  size_t maxIdx=m_objInfo.size();
  if (!m_doSuperCells) maxIdx=8;
  if (!m_doRegularCells) minIdx=8;
  
  for (size_t i=minIdx;i<maxIdx;++i) {
    if (m_objInfo[i].m_outputKey.size()==0 || m_objInfo[i].m_inputKey.size()==0) {
      ATH_MSG_DEBUG("No input/output key set for obj " << m_objInfo[i].m_classname << ". Ignored.");
      continue;
    }
    SG::TransientAddress* tad=new SG::TransientAddress(m_objInfo[i].m_clid,m_objInfo[i].m_outputKey);
    tad->setTransientID(m_objInfo[i].m_sclid);
    StatusCode sc=m_IOVSvc->preLoadTAD(tad,"DetectorStore");
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "IOVSvc failed to preLoadTAD" << endmsg;
      return sc;
    }
   // Add TAD to Storegate
    ATH_MSG_DEBUG("Preloaded address of " << m_objInfo[i].m_classname);
    tlist.push_back(tad);
  }

 return StatusCode::SUCCESS;
}


StatusCode LArFlatConditionSvc::updateAddress(StoreID::type,
                                              SG::TransientAddress* tad,
                                              const EventContext& /*ctx*/) {
  // Provide TAD and associated range, actually reading the conditions data


  // first check if this clid is managed by this Service
  // return FAILURE if not - this allows other AddressProviders to be 
  // asked for the TAD

  const CLID clid=tad->clID();
  const std::string& key=tad->name();

  std::vector<objInfo_t>::const_iterator objIt=m_objInfo.begin();
  std::vector<objInfo_t>::const_iterator objIt_e=m_objInfo.end();
  
  for (;objIt!=objIt_e && objIt->m_clid!=clid;++objIt);
  if (objIt==objIt_e) {//Not found 
    //ATH_MSG_DEBUG("Ignore this clid/key");
    return(StatusCode::FAILURE);
  }
  ATH_MSG_DEBUG("Creating address for clid/key " << clid << " " << key);

  if (m_initializing) {
    ATH_MSG_DEBUG("updateAddress: Still in initialization phase. Do nothing");
    return StatusCode::SUCCESS;
  }
  

  const CondAttrListCollection* attrlist;
  StatusCode sc=m_detStore->retrieve(attrlist,objIt->m_inputKey);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve a CondAttrListCollection with key " << objIt->m_inputKey << endmsg;
    return StatusCode::FAILURE;
  }

  if(m_printChecksum) printCheckSums(*objIt,attrlist);

  IOVRange range;
  sc=m_IOVSvc->getRange(m_attrListClid,objIt->m_inputKey,range);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to get IOV range for CondAttrListCollection with key " << objIt->m_inputKey << " from IOVSvc" << endmsg;
    return sc;
  }

  const size_t index=objIt-m_objInfo.begin();
  switch (index) { 
  case 0: 
    {
      if (this->createFlatObj<LAruA2MeVFlat>(attrlist,tad)==0) {
      	msg(MSG::ERROR) << "Problem creating LAruA2MeVFlat object" << endmsg;
	return StatusCode::FAILURE; 
      }
    }
   break;
   
  case 1:
    {
      if (this->createFlatObj<LArDAC2uAFlat>(attrlist,tad)==0) {
	msg(MSG::ERROR) << "Problem creating LArDAC2uAFlat object" << endmsg;
	return StatusCode::FAILURE; 
      }
    }
    break;
  case 2:
    {
      if (this->createFlatObj<LArHVScaleCorrFlat>(attrlist,tad)==0) {
	msg(MSG::ERROR) << "Problem creating LArHVScaleCorrFlat object " << endmsg;
	return StatusCode::FAILURE; 
      }
    }
    break;
  case 3:
    {
      if (this->createFlatObj<LArPedestalFlat>(attrlist,tad)==0) {
	msg(MSG::ERROR) << "Problem creating LArPedestalFlat object" << endmsg;
	return StatusCode::FAILURE; 
      }
    }
    break;
  case 4:
    {
      if (this->createFlatObj<LArRampFlat>(attrlist,tad)==0) {
	msg(MSG::ERROR) << "Problem creating LArRampFlat object" << endmsg;
	return StatusCode::FAILURE; 
      }
    }
    break;
  case 5:
    {
      if (this->createFlatObj<LArMphysOverMcalFlat>(attrlist,tad)==0) {
	msg(MSG::ERROR) << "Problem creating LArMphysOverMcalFlat object" << endmsg;
	return StatusCode::FAILURE; 
      }
    }
    break;
  case 6:
    {
      if (this->createFlatObj<LArOFCFlat>(attrlist,tad)==0) {
	msg(MSG::ERROR) << "Problem creating LArOFCFlat object" << endmsg;
	return StatusCode::FAILURE; 
      }
    }
    break;
  case 7:      
    {
      if (this->createFlatObj<LArShapeFlat>(attrlist,tad)==0) {
	msg(MSG::ERROR) << "Problem creating LArShapeFlat object" << endmsg;
	return StatusCode::FAILURE; 
      }
    }
    break;


  case 8:      
    {
      if (this->createFlatObj<LArAutoCorrSC>(attrlist,tad)==0) {
	msg(MSG::ERROR) << "Problem creating LArAutoCorrSC object" << endmsg;
	return StatusCode::FAILURE; 
      }
    }
    break;
  case 9:      
    {
      if (this->createFlatObj<LAruA2MeVSC>(attrlist,tad)==0) {
	msg(MSG::ERROR) << "Problem creating LAruA2MeVSC object" << endmsg;
	return StatusCode::FAILURE; 
      }
    }
    break;
  case 10:      
    {
      if (this->createFlatObj<LArDAC2uASC>(attrlist,tad)==0) {
	msg(MSG::ERROR) << "Problem creating LArDAC2uASC object" << endmsg;
	return StatusCode::FAILURE; 
      }
    }
    break;
    
  case 11:      
      if (this->createFlatObj<LArfSamplSC>(attrlist,tad)==0) {
	msg(MSG::ERROR) << "Problem creating LArfSamplSC object" << endmsg;
	return StatusCode::FAILURE; 
      }
    break;

  case 12:      
    if (this->createFlatObj<LArNoiseSC>(attrlist,tad)==0) {
      msg(MSG::ERROR) << "Problem creating LArNoiseSC object" << endmsg;
      return StatusCode::FAILURE; 
    }
    break;

  case 13:      
    if (this->createFlatObj<LArPedestalSC>(attrlist,tad)==0) {
      msg(MSG::ERROR) << "Problem creating LArPedestal object" << endmsg;
      return StatusCode::FAILURE; 
    }
    break;
    
  case 14:      
    if (this->createFlatObj<LArRampSC>(attrlist,tad)==0) {
      msg(MSG::ERROR) << "Problem creating LArRampSC object" << endmsg;
      return StatusCode::FAILURE; 
    }
    break;

  case 15:      
    if (this->createFlatObj<LArShapeSC>(attrlist,tad)==0) {
      msg(MSG::ERROR) << "Problem creating LArShapeSC object" << endmsg;
      return StatusCode::FAILURE; 
    }
    break;

  case 16:      
    if (this->createFlatObj<LArMinBiasSC>(attrlist,tad)==0) {
      msg(MSG::ERROR) << "Problem creating LArMinBiasSC object" << endmsg;
      return StatusCode::FAILURE; 
    }
    break;
    

  default:
    msg(MSG::ERROR) << "Something screwed up at " << __FILE__ << " line " << __LINE__ << endmsg;
    return StatusCode::FAILURE;
  }

  sc=m_IOVSvc->setRange(objIt->m_clid,objIt->m_outputKey,range);//,"DetectorStore");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to set IOV range for object " << objIt->m_classname << " with key " << objIt->m_outputKey << endmsg;
    return sc;
  }
  

  return StatusCode::SUCCESS;
}

StatusCode LArFlatConditionSvc::finalize() {
  return StatusCode::SUCCESS;
}


void LArFlatConditionSvc::handle( const Incident& inc ) {
  m_initializing=false;
  ATH_MSG_DEBUG("entering handle(), incident type " << inc.type()
		<< " from " << inc.source());
  return;
}


template<class T> 
T* LArFlatConditionSvc::createFlatObj(const CondAttrListCollection* attr,  SG::TransientAddress* tad) {

  const CLID clid=tad->clID();
  const std::string& key=tad->name();

  T* flat=new T(attr);
  if (!flat->good()) {
    msg(MSG::ERROR) << "Failed to covert AthenaAttributeList to flat LAr conditions object with clid/key " 
		    << clid << "/" << key << endmsg;
    delete flat;
    return 0;
  }
  else {
    SG::DataProxy* proxy=m_detStore->proxy(clid,key);
    DataObject* dObj=SG::asStorable(flat);
    proxy->setObject(dObj);
   }
   return flat;
 }

void LArFlatConditionSvc::printCheckSums(const objInfo_t& objInfo, const CondAttrListCollection* attrListColl) {

  if (!msgLvl(MSG::INFO)) return;

  union {
    int sn;
    unsigned usn;
  } cnv;

  boost::crc_32_type crc32;

  msg(MSG::INFO) << "Loading " << objInfo.m_classname << " from folder " << objInfo.m_inputKey << endmsg;

  CondAttrListCollection::const_iterator it=attrListColl->begin();
  CondAttrListCollection::const_iterator it_e=attrListColl->end();
  for (;it!=it_e;++it) {
    const coral::AttributeList& attrList=it->second;
    const unsigned gain=it->first;
    msg(MSG::INFO) << "   Gain " << gain;
    for (const auto& attr : attrList) {
      const std::string& typeName=attr.specification().typeName();
      if (typeName.find("blob")==0) {
	//Got a blob:
	const coral::Blob& blob = attr.data<coral::Blob>();
	const std::string& blobName=attr.specification().name();
	crc32.process_bytes(blob.startingAddress(),blob.size());
	cnv.usn= crc32.checksum();
	msg(MSG::INFO) << ", " << blobName << " checksum=" << cnv.sn;
	crc32.reset();
      }//end if blob
    }//end loop over attributes
    msg(MSG::INFO) << endmsg;
  }//end loop over gains
  



}
