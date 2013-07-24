/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#include "LArCalibUtils/LArRampAdHocPatchingAlg.h"
#include "StoreGate/StoreGateSvc.h"

LArRampAdHocPatchingAlg::LArRampAdHocPatchingAlg (const std::string& name, ISvcLocator* pSvcLocator) 
 : Algorithm(name,pSvcLocator),
   m_detStore(0),
   m_log(NULL),
   m_contIn(0),
   m_contOut(0)
{
  declareProperty("ContainerKey",   m_containerKey="");
  declareProperty("NewContainerKey",m_newContainerKey="");

  declareProperty("ChannelsToBePatchedHG",m_channelsToBePatchedHG);
  declareProperty("ChannelsToBePatchedMG",m_channelsToBePatchedMG);
  declareProperty("ChannelsToBePatchedLG",m_channelsToBePatchedLG);

  declareProperty("PatchesToBeAppliedHG",m_patchesToBeAppliedHG);
  declareProperty("PatchesToBeAppliedMG",m_patchesToBeAppliedMG);
  declareProperty("PatchesToBeAppliedLG",m_patchesToBeAppliedLG);

  declareProperty("UseCorrChannels",m_useCorrChannel,
		  "True: Use separate correction COOL channel, False: Correction + data in the same channel");
}


LArRampAdHocPatchingAlg::~LArRampAdHocPatchingAlg() 
{
  delete m_log;
}


StatusCode LArRampAdHocPatchingAlg::initialize() 
{
  m_log=new MsgStream(msgSvc(),name());
  (*m_log) << MSG::INFO << "Initialing LArRampAdHocPatchingAlg..." << endreq;
  (*m_log) << MSG::INFO << "HG: "<<m_channelsToBePatchedHG.size()<<" "<<m_patchesToBeAppliedHG.size()<<endreq;
  (*m_log) << MSG::INFO << "MG: "<<m_channelsToBePatchedMG.size()<<" "<<m_patchesToBeAppliedMG.size()<<endreq;
  (*m_log) << MSG::INFO << "LG: "<<m_channelsToBePatchedLG.size()<<" "<<m_patchesToBeAppliedLG.size()<<endreq;

  if ( m_channelsToBePatchedHG.size() && ( m_channelsToBePatchedHG.size() !=  m_patchesToBeAppliedHG.size() ) ) {
    (*m_log) << MSG::ERROR << "Wrong size of HIGH gain input vectors!" << endreq;
    return StatusCode::FAILURE;
  } else {
    (*m_log) << MSG::INFO << m_channelsToBePatchedHG.size() << " ad-hoc patches to be applied in HIGH gain" << endreq;
  } 

  if ( m_channelsToBePatchedMG.size() && ( m_channelsToBePatchedMG.size() !=  m_patchesToBeAppliedMG.size() ) ) {
    (*m_log) << MSG::ERROR << "Wrong size of MEDIUM gain input vectors!" << endreq;
    return StatusCode::FAILURE;
  } else {
    (*m_log) << MSG::INFO << m_channelsToBePatchedMG.size() << " ad-hoc patches to be applied in MEDIUM gain" << endreq;
  } 

  if ( m_channelsToBePatchedLG.size() && ( m_channelsToBePatchedLG.size() !=  m_patchesToBeAppliedLG.size() ) ) {
    (*m_log) << MSG::ERROR << "Wrong size of LOW gain input vectors!" << endreq;
    return StatusCode::FAILURE;
  } else {
    (*m_log) << MSG::INFO << m_channelsToBePatchedLG.size() << " ad-hoc patches to be applied in LOW gain" << endreq;
  } 

  return StatusCode::SUCCESS;
}


StatusCode LArRampAdHocPatchingAlg::stop() 
{
  (*m_log) << MSG::INFO << "Entering LArRampAdHocPatchingAlg" << endreq;

  StatusCode sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << " Cannot locate DetectorStore " << endreq;
    return StatusCode::FAILURE;
  } 
  
  if (m_newContainerKey.size()) { //New container key give -> different containers for reading and writing
    sc=m_detStore->retrieve(m_contIn,m_containerKey); //const-retrieve
    if (sc.isFailure()) {
      (*m_log) << MSG::ERROR << "Failed to retrieve const object with key " << m_containerKey 
	       << " from DetectorStore." << endreq;
      return StatusCode::FAILURE;
    }    
    m_contOut=new LArRampComplete();
    m_contOut->setGroupingType((LArConditionsContainerBase::GroupingType)m_contIn->groupingType());
    sc=m_contOut->initialize();
    if (sc.isFailure()) {
      (*m_log) << MSG::ERROR << "Failed to inizialize output container" << endreq;
      return StatusCode::FAILURE;
    }
    sc=m_detStore->record(m_contOut,m_newContainerKey);
    if (sc.isFailure()) {
      (*m_log) << MSG::ERROR << "Failed to record object with key " << m_containerKey 
	       << " to DetectorStore." << endreq;
      return StatusCode::FAILURE;
    }
    sc=m_detStore->symLink(m_contOut,(ILArRamp*)m_contOut);
    if (sc.isFailure()) {
      (*m_log) << MSG::ERROR << "Failed to symlink LArRampComplete to ILArRamp" << endreq;
      return StatusCode::FAILURE;
    }
    (*m_log) << MSG::INFO << "Loaded input container " << m_containerKey 
	     << ", write to new container " << m_newContainerKey << endreq;
  }
  else { //Same container for reading and writing (must not be locked)
    sc=m_detStore->retrieve(m_contOut,m_containerKey); //non-const retrieve
    if (sc.isFailure()) {
      (*m_log) << MSG::ERROR << "Failed to retrieve non-const object with key " << m_containerKey 
	       << " from DetectorStore." << endreq;
      return StatusCode::FAILURE;
    }
    m_contIn=const_cast<const LArRampComplete*>(m_contOut);
    (*m_log) << MSG::INFO << "Work on container '" <<  m_containerKey  << "'" << endreq;
  }

  if (m_channelsToBePatchedHG.size()) {
      (*m_log) << MSG::INFO << "Going to apply ad-hoc patches to HIGH gain ramps." << endreq;
    sc = ApplyAdHocPatches(m_channelsToBePatchedHG,m_patchesToBeAppliedHG,0);
    if (sc.isFailure()) {
      (*m_log) << MSG::ERROR << "Failed to apply ad-hoc patches to HIGH gain ramps." << endreq;
      return StatusCode::FAILURE;
    }
  }

  if (m_channelsToBePatchedMG.size()) {
      (*m_log) << MSG::INFO << "Going to apply ad-hoc patches to MEDIUM gain ramps." << endreq;
    sc = ApplyAdHocPatches(m_channelsToBePatchedMG,m_patchesToBeAppliedMG,1);
    if (sc.isFailure()) {
      (*m_log) << MSG::ERROR << "Failed to apply ad-hoc patches to MEDIUM gain ramps." << endreq;
      return StatusCode::FAILURE;
    }
  }

  if (m_channelsToBePatchedLG.size()) {
      (*m_log) << MSG::INFO << "Going to apply ad-hoc patches to LOW gain ramps." << endreq;
    sc = ApplyAdHocPatches(m_channelsToBePatchedLG,m_patchesToBeAppliedLG,2);
    if (sc.isFailure()) {
      (*m_log) << MSG::ERROR << "Failed to apply ad-hoc patches to MEDIUM gain ramps." << endreq;
      return StatusCode::FAILURE;
    }
  }

  (*m_log) << MSG::INFO << "Done with LArRampAdHocPatchingAlg" << endreq;

  return StatusCode::SUCCESS;
}



StatusCode LArRampAdHocPatchingAlg::ApplyAdHocPatches(std::vector<unsigned>& channelsToBePatched , std::vector<unsigned>& patchesToBeApplied, unsigned gain)
{ 
  if (channelsToBePatched.size() != patchesToBeApplied.size())
    return StatusCode::FAILURE;
  for (unsigned ich=0;ich<channelsToBePatched.size();++ich ) {    
    const HWIdentifier chid    = (HWIdentifier)channelsToBePatched[ich];
    // check if the channel to be patched is there...
    if (m_contIn->get(chid,gain).isEmpty()) continue;
    // ... if so, proceed with ad-hoc patching(s)
    const unsigned patchMethod = patchesToBeApplied[ich];
    //bool status = false;    
    switch (patchMethod) {
    case ZeroIntercept:
      /*status =*/ ZeroTheIntercept(chid,gain);
    }
    /*
    if (status) {
      (*m_log) << MSG::INFO << "Sucessfully patched  channel 0x" << MSG::hex << chid.get_compact() << MSG::dec <<" Gain:" << gain << endreq;
    } else {
      (*m_log) << MSG::WARNING << "Failed to patch channel 0x" << MSG::hex << chid.get_compact() << MSG::dec <<" Gain:" << gain << endreq;
    }
    */
  }
  return StatusCode::SUCCESS;
}


bool LArRampAdHocPatchingAlg::ZeroTheIntercept(HWIdentifier chid, unsigned gain)
{
  unsigned s = m_contIn->get(chid,gain).m_vRamp.size();
  LArRampObj patch;
  patch.m_vRamp.clear();
  patch.m_vRamp.resize(s);
  (*m_log) << MSG::INFO << "Size of the patched object: "<<s<<endreq;
  if(s==0) return false;
  patch.m_vRamp[0] = 0.; // zeroes the intercept
  for (unsigned i=1;i<s;++i){ // copy the other coefficients
    patch.m_vRamp[i] = m_contIn->get(chid,gain).m_vRamp[i];
  }
  StatusCode sc=m_contOut->insertCorrection(chid,patch,gain,m_useCorrChannel);
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Failed to zero the ramp intercept channel 0x" 
	     << MSG::hex << chid.get_compact()  << MSG::dec << ", gain " << gain << "." << endreq;
    return false;
  } else {
    (*m_log) << MSG::INFO << "Successfully zeroed the ramp intercept channel 0x" 
	     << MSG::hex << chid.get_compact()  << MSG::dec << ", gain " << gain << "." << endreq;
    return true;
  }
}
