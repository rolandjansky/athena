/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#include "LArCalibUtils/LArDuplicateConstants.h"

#include "LArRawConditions/LArPedestalComplete.h"
#include "LArRawConditions/LArRampComplete.h"
#include "LArRawConditions/LArMphysOverMcalComplete.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "LArRawConditions/LArShapeComplete.h"

typedef LArPedestalComplete::ConstConditionsMapIterator PedestalIt;
typedef LArRampComplete::ConstConditionsMapIterator RampIt;
typedef LArMphysOverMcalComplete::ConstConditionsMapIterator MpMcIt;
typedef LArOFCComplete::ConstConditionsMapIterator OFCIt;
typedef LArShapeComplete::ConstConditionsMapIterator ShapeIt;

LArDuplicateConstants::LArDuplicateConstants (const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name,pSvcLocator),
   m_onlineHelper(0)
{
  declareProperty("SourceGain"    , m_gainIN   = 1);
  declareProperty("TargetGain"    , m_gainOUT  = 2);
  declareProperty("KeyPedestal"   , m_keyPed   = "" ); //"LArPedestal");
  declareProperty("KeyRamp"       , m_keyRamp  = "" ); //"LArRamp");
  declareProperty("KeyMphysMcali" , m_keyMPMC  = "" ); //"LArMphysOverMcal");
  declareProperty("KeyOFC"        , m_keyOFC   = "" ); //"LArOFC");
  declareProperty("KeyShape"      , m_keyShape = "" ); //"LArShape");
}


LArDuplicateConstants::~LArDuplicateConstants() 
{
}


StatusCode LArDuplicateConstants::initialize() 
{
  ATH_MSG_INFO ( "Initialing " << name() );
  ATH_MSG_INFO ( "INPUT  gain = " << m_gainIN  );
  ATH_MSG_INFO ( "OUTPUT gain = " << m_gainOUT );
  return StatusCode::SUCCESS;
}


StatusCode LArDuplicateConstants::stop() 
{
  ATH_MSG_INFO ( "Entering LArDuplicateConstants" );

  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );

  // PEDESTAL

  if (!m_keyPed.empty()) {

    const LArPedestalComplete* PedestalIN;
    LArPedestalComplete* PedestalOUT;
    ATH_CHECK( detStore()->retrieve(PedestalIN,m_keyPed) );
    PedestalOUT=const_cast<LArPedestalComplete*>(PedestalIN); // cast to non-const the same container

    unsigned int n_tot_ped   = 0;
    unsigned int n_dup_ped   = 0;

    PedestalIt ped_it   = PedestalIN->begin(m_gainIN);
    PedestalIt ped_it_e = PedestalIN->end(m_gainIN);
    if ( ped_it == ped_it_e ) {
      ATH_MSG_WARNING ( "LArPedestalComplete (key = " << m_keyPed << ") has no pedestals with gain = " << m_gainIN );
    }  
    for ( ; ped_it!=ped_it_e; ped_it++) {
      const HWIdentifier chid = ped_it.channelId();
      if (m_onlineHelper->isEMBPS(chid)) { // duplicate constants for EMB PS only      
	n_tot_ped++;
	const LArPedestalP1& ped_t = PedestalOUT->get(chid,m_gainOUT);
	if (ped_t.isEmpty()) {
	  n_dup_ped++;
	  const LArPedestalP1& ped = PedestalIN->get(chid,m_gainIN);      
	  PedestalOUT->setPdata(chid,ped,m_gainOUT);
	  ATH_MSG_VERBOSE ( "Pedestal for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                            << " duplicated from gain " << m_gainIN << " to gain " << m_gainOUT );
	} else {
	  ATH_MSG_VERBOSE ( "Pedestal for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                            << " in gain " << m_gainOUT << " exists! Not duplicating..." );
	}
      }
    }
    ATH_MSG_INFO ( "LArPedestalComplete (key = " << m_keyPed << ") content duplicated from gain " 
                   << m_gainIN << " to gain " << m_gainOUT 
                   << " (" << n_dup_ped << "/" << n_tot_ped << " PS channels)" );
  }

  // RAMP

  if (!m_keyRamp.empty()) {

    const LArRampComplete* RampIN = nullptr;
    LArRampComplete* RampOUT = nullptr;
    ATH_CHECK( detStore()->retrieve(RampIN,m_keyRamp) );
    RampOUT=const_cast<LArRampComplete*>(RampIN); // cast to non-const the same container

    bool undoCorr=false;

    if (!RampOUT->correctionsApplied()) {
      ATH_CHECK( RampOUT->applyCorrections() );
      undoCorr=true;
      ATH_MSG_INFO ( "Applied corrections for Ramp" );
    }
    unsigned deltaGain = (m_gainOUT-m_gainIN); // Ex: 2-1  = 1
    float scaleRamp = pow(10.,deltaGain);      // Ex: 10^1 = 10
    ATH_MSG_INFO ( "Ramp scale factor = " << scaleRamp );

    unsigned int n_tot_ramp  = 0;
    unsigned int n_dup_ramp  = 0;
    unsigned nCorrChans=0;

    RampIt ramp_it   = RampIN->begin(m_gainIN);
    RampIt ramp_it_e = RampIN->end(m_gainIN);
    if ( ramp_it == ramp_it_e ) {
      ATH_MSG_WARNING ( "LArRampComplete (key = " << m_keyRamp << ") has no ramps with gain = " << m_gainIN );
    }
    for ( ; ramp_it!=ramp_it_e; ramp_it++) {
      const HWIdentifier chid = ramp_it.channelId();
      if (m_onlineHelper->isEMBPS(chid)) { // duplicate constants for EMB PS only
	n_tot_ramp++;
	const LArRampP1& ramp_t = RampOUT->get(chid,m_gainOUT);
	if (ramp_t.isEmpty()) {
	  n_dup_ramp++;
	  const LArRampP1& ramp = RampIN->get(chid,m_gainIN);      
	  // scale ramp according to gain difference
	  unsigned s = ramp.m_vRamp.size();
	  LArRampP1 newramp;
	  newramp.m_vRamp.clear();
	  newramp.m_vRamp.resize(s);
	  for (unsigned i=0;i<s;i++) {      
	    newramp.m_vRamp[i] = ramp.m_vRamp[i] * scaleRamp;      
	  }    
	  RampOUT->setPdata(chid,newramp,m_gainOUT);
	  ATH_MSG_VERBOSE ( "Ramp for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                            << " duplicated from gain " << m_gainIN << " to gain " << m_gainOUT );
	} else {
	  ATH_MSG_VERBOSE ( "Ramp for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                            << " in gain " << m_gainOUT << " exists! Not duplicating..." );
	}
      }
    }
    if (undoCorr) {
      ATH_CHECK( RampOUT->undoCorrections() );
      ATH_MSG_INFO ( "Undo corrections for Ramp" );
    }
    else {
      //Now work on correction channels
      LArRampComplete::ConstCorrectionIt cit=RampIN->undoCorrBegin(m_gainIN);
      LArRampComplete::ConstCorrectionIt cit_e=RampIN->undoCorrEnd(m_gainIN);
      for (;cit!=cit_e;cit++) {
	const HWIdentifier chid(cit->first);
	if (m_onlineHelper->isEMBPS(chid)) { // duplicate constants for EMB PS only
	  const LArRampP1& ramp=cit->second;
	  const unsigned s = ramp.m_vRamp.size();
	  LArRampP1 newramp;
	  newramp.m_vRamp.clear();
	  newramp.m_vRamp.resize(s);
	  for (unsigned i=0;i<s;i++) {      
	    newramp.m_vRamp[i] = ramp.m_vRamp[i] * scaleRamp;      
	  }    
	  ATH_CHECK( RampOUT->insertCorrection(chid,newramp,m_gainOUT,false) );
	  ++nCorrChans;
	}//end if isEMBPS
      } // end loop over correction channels
    }
    ATH_MSG_INFO ( "LArRampComplete (key = " << m_keyRamp << ") content duplicated from gain " 
                   << m_gainIN << " to gain " << m_gainOUT 
                   << " (" << n_dup_ramp << "/" << n_tot_ramp << " + " << nCorrChans << " PS channels)" );
  }
  
  // MPHYSOVERMCALI

  if (!m_keyMPMC.empty()) {

    const LArMphysOverMcalComplete* MphysMcaliIN = nullptr;
    LArMphysOverMcalComplete* MphysMcaliOUT = nullptr;
    ATH_CHECK( detStore()->retrieve(MphysMcaliIN,m_keyMPMC) );
    MphysMcaliOUT=const_cast<LArMphysOverMcalComplete*>(MphysMcaliIN); // cast to non-const the same container
    
    unsigned int n_tot_mpmc  = 0;
    unsigned int n_dup_mpmc  = 0;

    MpMcIt mpmc_it   = MphysMcaliIN->begin(m_gainIN);
    MpMcIt mpmc_it_e = MphysMcaliIN->end(m_gainIN);
    if ( mpmc_it == mpmc_it_e ) {
      ATH_MSG_WARNING ( "LArMphysOverMcaliComplete (key = " << m_keyMPMC << ") has no Mphys/Mcali with gain = " << m_gainIN );
    }  
    for ( ; mpmc_it!=mpmc_it_e; mpmc_it++) {    
      const HWIdentifier chid = mpmc_it.channelId();
      if (m_onlineHelper->isEMBPS(chid)) { // duplicate constants for EMB PS only
	n_tot_mpmc++;
	const LArSingleFloatP& mpmc_t = MphysMcaliOUT->get(chid,m_gainOUT);
	if (mpmc_t.isEmpty()) {
	  n_dup_mpmc++;
	  const LArSingleFloatP& mpmc = MphysMcaliIN->get(chid,m_gainIN);      
	  MphysMcaliOUT->setPdata(chid,mpmc,m_gainOUT);
	  ATH_MSG_VERBOSE ( "MpMc for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                            << " duplicated from gain " << m_gainIN << " to gain " << m_gainOUT );
	} else {
	  ATH_MSG_VERBOSE ( "MpMc for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                            << " in gain " << m_gainOUT << " exists! Not duplicating..." );
	}
      }
    }
    ATH_MSG_INFO ( "LArMphysOverMcaliComplete (key = " << m_keyMPMC << ") content duplicated from gain " 
                   << m_gainIN << " to gain " << m_gainOUT
                   << " (" << n_dup_mpmc << "/" << n_tot_mpmc << " PS channels)" );
  }
  
  // OFC

  if (!m_keyOFC.empty()) {

    const LArOFCComplete* OFCIN = nullptr;
    LArOFCComplete* OFCOUT = nullptr;
    ATH_CHECK( detStore()->retrieve(OFCIN,m_keyOFC) );
    OFCOUT=const_cast<LArOFCComplete*>(OFCIN); // cast to non-const the same container
    
    unsigned int n_tot_ofc   = 0;
    unsigned int n_dup_ofc   = 0;
    
    OFCIt ofc_it   = OFCIN->begin(m_gainIN);
    OFCIt ofc_it_e = OFCIN->end(m_gainIN);
    if ( ofc_it == ofc_it_e ) {
      ATH_MSG_WARNING ( "LArOFCComplete (key = " << m_keyOFC << ") has no OFCs with gain = " << m_gainIN );
    }  
    for ( ; ofc_it!=ofc_it_e; ofc_it++) {    
      const HWIdentifier chid = ofc_it.channelId();
      if (m_onlineHelper->isEMBPS(chid)) { // duplicate constants for EMB PS only
	n_tot_ofc++;
	
	//const LArOFCP1& ofc_t = OFCOUT->get(chid,m_gainOUT); // gives a segmentation fault from LAr2DWaveBase::LAr2DWaveBase(LArCompactSubsetChannelProxy const&) ()
	// LArOFCP1 ofc_t = OFCOUT->get(chid,m_gainOUT); // make a copy instead? Nope, still segfaulting.      
	//      if (ofc_t.isEmpty()) {  } // cannot use this :-(
      
	// directly get one data member :-(
	ILArOFC::OFCRef_t vOFCa_t = OFCOUT->OFC_a(chid,m_gainOUT);
	if (!vOFCa_t.size()) { // emulate isEmpty() behavior
	  
	  n_dup_ofc++;
	  const LArOFCP1& ofc = OFCIN->get(chid,m_gainIN); // why is this working here? Because it;s not empty?
	  OFCOUT->setPdata(chid,ofc,m_gainOUT);
	  ATH_MSG_VERBOSE ( "OFC for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                            << " duplicated from gain " << m_gainIN << " to gain " << m_gainOUT );
	} else {
	  ATH_MSG_VERBOSE ( "OFC for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                            << " in gain " << m_gainOUT << " exists! Not duplicating..." );
	  
	}
      }
    }
    ATH_MSG_INFO ( "LArOFCComplete (key = " << m_keyOFC << ") content duplicated from gain " 
                   << m_gainIN << " to gain " << m_gainOUT 
                   << " (" << n_dup_ofc << "/" << n_tot_ofc << " PS channels)" );
  }

  // SHAPE

  if (!m_keyShape.empty()) {

    const LArShapeComplete* ShapeIN = nullptr;
    LArShapeComplete* ShapeOUT = nullptr;
    ATH_CHECK( detStore()->retrieve(ShapeIN,m_keyShape) );
    ShapeOUT=const_cast<LArShapeComplete*>(ShapeIN); // cast to non-const the same container
    
    unsigned int n_tot_shape = 0;
    unsigned int n_dup_shape = 0;
    
    ShapeIt shape_it   = ShapeIN->begin(m_gainIN);
    ShapeIt shape_it_e = ShapeIN->end(m_gainIN);
    if ( shape_it == shape_it_e ) {
      ATH_MSG_WARNING ( "LArShapeComplete (key = " << m_keyShape << ") has no Shapes with gain = " << m_gainIN );
    }  
    for ( ; shape_it!=shape_it_e; shape_it++) {    
      const HWIdentifier chid = shape_it.channelId();
      if (m_onlineHelper->isEMBPS(chid)) { // duplicate constants for EMB PS only
	n_tot_shape++;
	
	//const LArShapeP2& shape_t = ShapeOUT->get(chid,m_gainOUT);      
	//if (shape_t.isEmpty()) {       
	
	ILArShape::ShapeRef_t vShape_t = ShapeOUT->Shape(chid,m_gainOUT);
	if (!vShape_t.size()) { 
	  
	  n_dup_shape++;
	  const LArShapeP2& shape = ShapeIN->get(chid,m_gainIN);
	  ShapeOUT->setPdata(chid,shape,m_gainOUT);
	  ATH_MSG_VERBOSE ( "Shape for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                            << " duplicated from gain " << m_gainIN << " to gain " << m_gainOUT );
	} else {
	  ATH_MSG_VERBOSE ( "Shape for channel 0x" << MSG::hex << chid.get_compact() << MSG::dec 
                            << " in gain " << m_gainOUT << " exists! Not duplicating..." );
	}
      }
    }
    ATH_MSG_INFO ( "LArShapeComplete (key = " << m_keyShape << ") content duplicated from gain " 
                   << m_gainIN << " to gain " << m_gainOUT 
                   << " (" << n_dup_shape << "/" << n_tot_shape << " PS channels)" );
    
  }

  ATH_MSG_INFO ( "Done with LArDuplicateConstants" );
  return StatusCode::SUCCESS;
}
