/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#include "LArCalibUtils/LArRampAdHocPatchingAlg.h"
#include "StoreGate/StoreGateSvc.h"

LArRampAdHocPatchingAlg::LArRampAdHocPatchingAlg (const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name,pSvcLocator),
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

  declareProperty("ValuesToBeAppliedHG",m_valuesToBeAppliedHG);
  declareProperty("ValuesToBeAppliedMG",m_valuesToBeAppliedMG);
  declareProperty("ValuesToBeAppliedLG",m_valuesToBeAppliedLG);

  declareProperty("UseCorrChannels",m_useCorrChannel,
		  "True: Use separate correction COOL channel, False: Correction + data in the same channel");
}


LArRampAdHocPatchingAlg::~LArRampAdHocPatchingAlg() 
{
}


StatusCode LArRampAdHocPatchingAlg::initialize() 
{
  ATH_MSG_INFO ( "Initialing LArRampAdHocPatchingAlg..." );
  ATH_MSG_INFO ( "HG: "<<m_channelsToBePatchedHG.size()<<" "<<m_patchesToBeAppliedHG.size());
  ATH_MSG_INFO ( "MG: "<<m_channelsToBePatchedMG.size()<<" "<<m_patchesToBeAppliedMG.size());
  ATH_MSG_INFO ( "LG: "<<m_channelsToBePatchedLG.size()<<" "<<m_patchesToBeAppliedLG.size());

  if ( m_channelsToBePatchedHG.size() && ( m_channelsToBePatchedHG.size() !=  m_patchesToBeAppliedHG.size() ) ) {
    ATH_MSG_ERROR ( "Wrong size of HIGH gain input vectors!" );
    return StatusCode::FAILURE;
  } else {
     unsigned count =  std::count_if (m_patchesToBeAppliedHG.begin(), m_patchesToBeAppliedHG.end(), std::bind2nd(std::equal_to<int>(),1) ); // 1 is to apply values
     if ( (count > 0) && (m_valuesToBeAppliedHG.size() != count)) {
         ATH_MSG_ERROR ( "Wrong size of HIGH gain values vector ! " << count << " " << m_valuesToBeAppliedHG.size());
         return StatusCode::FAILURE;
     } else {
        ATH_MSG_INFO ( m_channelsToBePatchedHG.size() << " ad-hoc patches to be applied in HIGH gain" );
     }
  } 

  if ( m_channelsToBePatchedMG.size() && ( m_channelsToBePatchedMG.size() !=  m_patchesToBeAppliedMG.size() ) ) {
    ATH_MSG_ERROR ( "Wrong size of MEDIUM gain input vectors!" );
    return StatusCode::FAILURE;
  } else {
     unsigned count =  std::count_if (m_patchesToBeAppliedMG.begin(), m_patchesToBeAppliedMG.end(), std::bind2nd(std::equal_to<int>(),1) ); // 1 is to apply values
     if ( (count > 0) && (m_valuesToBeAppliedMG.size() != count)) {
         ATH_MSG_ERROR ( "Wrong size of MEDIUM gain values vector !" );
         return StatusCode::FAILURE;
     } else {
        ATH_MSG_INFO ( m_channelsToBePatchedMG.size() << " ad-hoc patches to be applied in MEDIUM gain" );
     }
  } 

  if ( m_channelsToBePatchedLG.size() && ( m_channelsToBePatchedLG.size() !=  m_patchesToBeAppliedLG.size() ) ) {
    ATH_MSG_ERROR ( "Wrong size of LOW gain input vectors!" );
    return StatusCode::FAILURE;
  } else {
     unsigned count =  std::count_if (m_patchesToBeAppliedLG.begin(), m_patchesToBeAppliedLG.end(), std::bind2nd(std::equal_to<int>(),1) ); // 1 is to apply values
     if ( (count > 0) && (m_valuesToBeAppliedLG.size() != count)) {
         ATH_MSG_ERROR ( "Wrong size of LOW gain values vector !" );
         return StatusCode::FAILURE;
     } else {
       ATH_MSG_INFO ( m_channelsToBePatchedLG.size() << " ad-hoc patches to be applied in LOW gain" );
     }
  } 

  return StatusCode::SUCCESS;
}


StatusCode LArRampAdHocPatchingAlg::stop() 
{
  ATH_MSG_INFO ( "Entering LArRampAdHocPatchingAlg" );

  if (m_newContainerKey.size()) { //New container key give -> different containers for reading and writing
    ATH_CHECK( detStore()->retrieve(m_contIn,m_containerKey) ); //const-retrieve
    m_contOut=new LArRampComplete();
    m_contOut->setGroupingType((LArConditionsContainerBase::GroupingType)m_contIn->groupingType());
    ATH_CHECK( m_contOut->initialize() );
    ATH_CHECK( detStore()->record(m_contOut,m_newContainerKey) );
    ATH_CHECK( detStore()->symLink(m_contOut,(ILArRamp*)m_contOut) );
    ATH_MSG_INFO ( "Loaded input container " << m_containerKey 
                   << ", write to new container " << m_newContainerKey );
  }
  else { //Same container for reading and writing (must not be locked)
    ATH_CHECK( detStore()->retrieve(m_contOut,m_containerKey) ); //non-const retrieve
    m_contIn=const_cast<const LArRampComplete*>(m_contOut);
    ATH_MSG_INFO ( "Work on container '" <<  m_containerKey  << "'" );
  }

  if (m_channelsToBePatchedHG.size()) {
    ATH_MSG_INFO ( "Going to apply ad-hoc patches to HIGH gain ramps." );
    ATH_CHECK( ApplyAdHocPatches(m_channelsToBePatchedHG,m_patchesToBeAppliedHG,m_valuesToBeAppliedHG, 0) );
  }

  if (m_channelsToBePatchedMG.size()) {
    ATH_MSG_INFO ( "Going to apply ad-hoc patches to MEDIUM gain ramps." );
    ATH_CHECK( ApplyAdHocPatches(m_channelsToBePatchedMG,m_patchesToBeAppliedMG,m_valuesToBeAppliedMG,1) );
  }

  if (m_channelsToBePatchedLG.size()) {
    ATH_MSG_INFO ( "Going to apply ad-hoc patches to LOW gain ramps." );
    ATH_CHECK( ApplyAdHocPatches(m_channelsToBePatchedLG,m_patchesToBeAppliedLG,m_valuesToBeAppliedLG,2) );
  }

  ATH_MSG_INFO ( "Done with LArRampAdHocPatchingAlg" );
  return StatusCode::SUCCESS;
}



StatusCode LArRampAdHocPatchingAlg::ApplyAdHocPatches(std::vector<unsigned>& channelsToBePatched , std::vector<unsigned>& patchesToBeApplied, std::vector<std::vector<double> >& valuesToBeApplied, unsigned gain)
{ 
  if (channelsToBePatched.size() != patchesToBeApplied.size())
    return StatusCode::FAILURE;
  unsigned putcount=0;
  for (unsigned ich=0;ich<channelsToBePatched.size();++ich ) {    
    const HWIdentifier chid    = (HWIdentifier)channelsToBePatched[ich];
    // ... if so, proceed with ad-hoc patching(s)
    const unsigned patchMethod = patchesToBeApplied[ich];
    //bool status = false;    
    switch (patchMethod) {
    case ZeroIntercept: {
      // check if the channel to be patched is there...
      if (m_contIn->get(chid,gain).isEmpty()) continue;
      ZeroTheIntercept(chid,gain);
      break;
                        }
    case PutValues: {
         if ( valuesToBeApplied[putcount].size() == 0 ) return StatusCode::FAILURE;
         PutTheValues(chid,valuesToBeApplied[putcount],gain);
         ++putcount;
         break;
                    }
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
  ATH_MSG_INFO ( "Size of the patched object: "<<s);
  if(s==0) return false;
  patch.m_vRamp[0] = 0.; // zeroes the intercept
  for (unsigned i=1;i<s;++i){ // copy the other coefficients
    patch.m_vRamp[i] = m_contIn->get(chid,gain).m_vRamp[i];
  }
  StatusCode sc=m_contOut->insertCorrection(chid,patch,gain,m_useCorrChannel);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "Failed to zero the ramp intercept channel 0x" 
                    << MSG::hex << chid.get_compact()  << MSG::dec << ", gain " << gain << "." );
    return false;
  } else {
    ATH_MSG_INFO ( "Successfully zeroed the ramp intercept channel 0x" 
                   << MSG::hex << chid.get_compact()  << MSG::dec << ", gain " << gain << "." );
    return true;
  }
}

bool LArRampAdHocPatchingAlg::PutTheValues(HWIdentifier chid, std::vector<double>& rvalues, unsigned gain)
{
  unsigned s = rvalues.size();
  LArRampObj patch;
  patch.m_vRamp.clear();
  patch.m_vRamp.resize(s);
  ATH_MSG_INFO ( "Size of the patched object: "<<s);
  if(s==0) return false;
  for (unsigned i=0;i<s;++i){ // copy the values
    patch.m_vRamp[i] = rvalues[i];
  }
  StatusCode sc;
  try { sc=m_contOut->insertCorrection(chid,patch,gain,m_useCorrChannel);}
  catch(...) { ATH_MSG_INFO ("Skipping insert of corrections, maybe it's not about this partition");}
  if (sc.isFailure()) {
      ATH_MSG_ERROR ( "Failed to put the values for channel 0x" 
                    << MSG::hex << chid.get_compact()  << MSG::dec << ", gain " << gain << "." );
      return false;
  } else {
      ATH_MSG_INFO ( "Successfully put the values for channel 0x" 
                   << MSG::hex << chid.get_compact()  << MSG::dec << ", gain " << gain << "." );
      return true;
  }
}
