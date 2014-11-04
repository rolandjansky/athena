/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSerializeCnvSvc/TrigSerializeConvHelper.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ClassID.h"
#include "AthenaKernel/getMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"


#include "TrigSerializeCnvSvc/TrigStreamAddress.h"
#include "TrigSerializeResult/ITrigSerializerToolBase.h"
#include "TrigSerializeTP/TrigSerTPTool.h"
#include "TrigSerializeCnvSvc/ITrigSerGuidHelper.h"

//
#include "TROOT.h"
#include "TClass.h"


TrigSerializeConvHelper::TrigSerializeConvHelper(const std::string& toolname, const std::string& type, const IInterface* parent) :
  AthAlgTool(toolname, type, parent),
  m_serializerTool("TrigTSerializer/TrigTSerializer"),
  m_TPTool("TrigSerTPTool/TrigSerTPTool"),
  m_guidTool("TrigSerializeGuidHelper/TrigSerializeGuidHelper"),
  m_doTP(true)
{
  declareInterface<ITrigSerializeConvHelper>( this );
  declareProperty("doTP", m_doTP, "allows to serialize/deserialize transient classes");
}

TrigSerializeConvHelper::~TrigSerializeConvHelper(){
}

StatusCode TrigSerializeConvHelper::initialize(){

  StatusCode sc = m_serializerTool.retrieve();

  if (!sc.isSuccess()){
    msg(MSG::DEBUG) << "m_serializer not retrieved" << endreq;
    return sc;
  } else {
    msg(MSG::DEBUG) << "m_serializer retrieved" << endreq;
    /*
      if (m_serializerTool->initialize().isSuccess()){
      msg(MSG::DEBUG) << "serializer initialized" << endreq;
      }
    */
  }
  
  StatusCode sctp = m_TPTool.retrieve();
  if (!sctp.isSuccess()){
    msg(MSG::DEBUG) << "m_TPTool not retrieved" << endreq;
    return sctp;
  } else {
    msg(MSG::DEBUG) << "m_TPTool retrieved" << endreq;
    /*
      if (m_TPTool->initialize().isSuccess())
      msg(MSG::DEBUG) << "m_TPTool initialized" << endreq;
      */
  }

  StatusCode scg = m_guidTool.retrieve();
  if (!scg.isSuccess()){
    msg(MSG::DEBUG) << m_guidTool << " not retrieved" << endreq;
    return scg;
  } else {
    msg(MSG::DEBUG) << m_guidTool << "retrieved" << endreq;
  }
  
  m_oldEDMmap[ "TrigRoiDescriptor" ]         = "TrigRoiDescriptorCollection_tlp1";
  m_oldEDMmap[ "TrigOperationalInfo" ]       = "TrigOperationalInfoCollection_tlp1";
  m_oldEDMmap[ "TrigInDetTrackCollection" ]  = "TrigInDetTrackCollection_tlp1";
  m_oldEDMmap[ "MuonFeature" ]               = "MuonFeatureContainer_tlp1";
  m_oldEDMmap[ "IsoMuonFeature" ]            = "IsoMuonFeatureContainer_tlp1";
  m_oldEDMmap[ "CombinedMuonFeature" ]       = "CombinedMuonFeatureContainer_tlp1";
  m_oldEDMmap[ "TileMuFeature" ]             = "TileMuFeatureContainer_tlp1";
  m_oldEDMmap[ "TileTrackMuFeature" ]        = "TileTrackMuFeatureContainer_tlp1";
  m_oldEDMmap[ "TrigEMCluster" ]             = "TrigEMClusterContainer_tlp1";
  //m_oldEDMmap[ "TrigEMCluster" ]             = "TrigEMClusterContainer_tlp2";
  m_oldEDMmap[ "RingerRings" ]               = "RingerRingsContainer_tlp1";
  m_oldEDMmap[ "TrigTauCluster" ]            = "TrigTauClusterContainer_tlp1";
  m_oldEDMmap[ "TrigTauClusterDetails" ]     = "TrigTauClusterDetailsContainer_tlp1";
  m_oldEDMmap[ "TrigTauTracksInfo" ]         = "TrigTauTracksInfoCollection_tlp1";
  m_oldEDMmap[ "TrigT2Jet" ]                 = "TrigT2JetContainer_tlp1";
  m_oldEDMmap[ "TrigElectronContainer" ]     = "TrigElectronContainer_tlp2";
  m_oldEDMmap[ "TrigPhotonContainer" ]       = "TrigPhotonContainer_tlp2";
  m_oldEDMmap[ "TrigTau" ]                   = "TrigTauContainer_tlp1";
  m_oldEDMmap[ "TrigL2BjetContainer" ]       = "TrigL2BjetContainer_tlp2";
  m_oldEDMmap[ "TrigMissingET" ]             = "TrigMissingETContainer_tlp1";
  m_oldEDMmap[ "TrigT2MbtsBits" ]            = "TrigT2MbtsBitsContainer_tlp1";
  m_oldEDMmap[ "TrigSpacePointCounts" ]      = "TrigSpacePointCountsCollection_tlp1";
  m_oldEDMmap[ "TrigTrtHitCounts" ]          = "TrigTrtHitCountsCollection_tlp1";
  m_oldEDMmap[ "CosmicMuonCollection" ]      = "CosmicMuonCollection_tlp1";
  m_oldEDMmap[ "MdtTrackSegmentCollection" ] = "MdtTrackSegmentCollection_tlp1";
  m_oldEDMmap[ "TrigVertexCollection" ]      = "TrigVertexCollection_tlp1";
  m_oldEDMmap[ "TrigL2BphysContainer" ]      = "TrigL2BphysContainer_tlp1";
  m_oldEDMmap[ "TrigEFBphysContainer" ]      = "TrigEFBphysContainer_tlp1";
  m_oldEDMmap[ "TrigEFBjetContainer" ]       = "TrigEFBjetContainer_tlp2";
  m_oldEDMmap[ "JetCollection" ]             = "JetCollection_tlp2";
  m_oldEDMmap[ "CaloClusterContainer" ]      = "CaloClusterContainer_p4";
  m_oldEDMmap[ "CaloShowerContainer" ]       = "CaloShowerContainer_p2";
  //m_oldEDMmap[ "CaloTowerContainer" ]        = "CaloTowerContainer_p1";
  m_oldEDMmap[ "TrigMuonEFContainer" ]       = "TrigMuonEFContainer_tlp1";
  m_oldEDMmap[ "TrigMuonEFInfoContainer" ]   = "TrigMuonEFInfoContainer_tlp1";
  m_oldEDMmap[ "egDetailContainer" ]         = "egDetailContainer_p1";
  m_oldEDMmap[ "egammaContainer" ]           = "egammaContainer_p1";
  m_oldEDMmap[ "Analysis::TauJetContainer" ] = "TauJetContainer_p3";
  m_oldEDMmap[ "TrigTrackCounts" ]           = "TrigTrackCountsCollection_tlp1";
  m_oldEDMmap[ "TrackCollection" ]           = "Trk::TrackCollection_tlp2";
  m_oldEDMmap[ "Rec::TrackParticleContainer" ]   = "Rec::TrackParticleContainer_tlp1";
  m_oldEDMmap[ "Analysis::TauDetailsContainer" ] ="TauDetailsContainer_tlp1";
  m_oldEDMmap[ "VxContainer" ]               = "";
  m_oldEDMmap[ "CaloCellContainer" ]         = "CaloCompactCellContainer";

  
  return StatusCode::SUCCESS;
}

StatusCode TrigSerializeConvHelper::createObj(const std::string &clname, IOpaqueAddress* iAddr, void *&ptr, bool isxAOD){
    
  ptr = 0;
  msg(MSG::DEBUG) << "in TrigSerializeConvHelper::createObj for clname" << clname << " is xAOD? " << (isxAOD?"yes":"no") << endreq;
  
  //could alse get DATA (perhaps as boost::any) from the IOA
  TrigStreamAddress *addr = dynamic_cast<TrigStreamAddress*>(iAddr);
  if (!addr) {
    msg(MSG::WARNING) << "createObj cast failed" << endreq;
    return StatusCode::FAILURE;
  }
  
  ITrigSerializerToolBase* serializer = m_serializerTool.operator->();
  serializer->reset();
  
  std::vector<uint32_t> v = addr->get();
  
  std::string cl = clname;
  if (m_doTP and !isxAOD)
    cl = m_TPTool->persClassName(clname);

  msg(MSG::DEBUG) << "cl (persistent classname) is " << cl << endreq;

  
  uint32_t guid[4];

  if (cl!=""){
    StatusCode scid;
    scid = serializer->peekCLID(v, guid);
    if (scid.isFailure()){
      //BS has no hint on the pers class - use the original table
      if (msgLvl(MSG::DEBUG))
	msg(MSG::DEBUG) << "BS does not hint on payload object " << cl << endreq;
      if (m_oldEDMmap.find(clname)!=m_oldEDMmap.end()){
	if (cl!=m_oldEDMmap[clname]){
	  msg(MSG::DEBUG) << "Using backward compatibility map with " <<  m_oldEDMmap[clname]
			  << " instead of " << cl << endreq;
	  cl = m_oldEDMmap[clname];
	}
      }
    } else {
      //get the pers version from the BS
      std::string nclass;
      StatusCode ai = m_guidTool->IntsToClassName(guid, nclass);
      if (ai.isFailure()){
	//better do not decode
	return StatusCode::FAILURE;
      }
      if (cl != nclass){
	//cl =  nclass;
	if (msgLvl(MSG::DEBUG))
	  msg(MSG::DEBUG) << "Got hint of " << cl
			  << " different persistent class from the BS payload. Name from GUID: " << nclass << endreq;
      }
    }
    
    ptr = serializer->deserialize(cl, v);
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << cl << " deserialized to " << ptr << endreq;
  }
   
  // T/P separation
  if (m_doTP and !isxAOD){
    std::string transclass;
									   
    void *persObj = m_TPTool->convertPT(cl,ptr, transclass);



    //persistent object not needed anymore
    TClass *persClObj = gROOT->GetClass(cl.c_str());
    if (persClObj)
      persClObj->Destructor(ptr);

    ptr = persObj;
  }

  return StatusCode::SUCCESS;
}



StatusCode TrigSerializeConvHelper::createRep(const std::string &clname,
					      void *ptr, std::vector<uint32_t> &out, bool isxAOD){
  
  StatusCode sc(StatusCode::SUCCESS);

  msg(MSG::DEBUG) << "in TrigSerializeConvHelper::createRep for clname" << clname << " is xAOD? " << (isxAOD?"yes":"no") << endreq;

  std::string cl = clname;
  void *pObj = ptr;

  // T/P separation
  if (m_doTP and !isxAOD){
    std::string persclass("");
    pObj = m_TPTool->convertTP(clname,ptr, persclass);
    cl = persclass;
  }

  msg(MSG::DEBUG) << "convertTP: " << pObj << " of "
	 << cl << endreq;
  
  //void *serptr(0);
  //
  ITrigSerializerToolBase* serializer = m_serializerTool.operator->();
  serializer->reset();

  msg(MSG::VERBOSE) << "About to get GUID for " << cl << endreq;
  
  //opposite from string to class
  uint32_t irep[4];
  StatusCode ai = m_guidTool->ClassNameToInts(cl, irep);
  if (ai.isFailure()){
    msg(MSG::WARNING) << "Cannot store class identification for "
		      << cl << " to BS" << endreq;
  }

  msg(MSG::VERBOSE) << "got GUID: " << irep[0] << "-" << irep[1] << "-" << irep[2] << "-" << irep[3] << endreq;
  
  if (cl != "" && pObj){
    serializer->setCLID(irep);
    serializer->serialize(cl, pObj, out);
  } else {
    msg(MSG::WARNING) << "did not serialize " << ptr << " of " << clname << endreq;
  }

  if (m_doTP and !isxAOD){
    //we don't need the persistent object anymore
    TClass *persClObj = gROOT->GetClass(cl.c_str());
    if (persClObj)
      persClObj->Destructor(pObj);
  }

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "pObj: " << pObj << " of " << cl
	 << " payload: " << out.size() << endreq;

  return sc;
}
