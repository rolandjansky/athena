/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "NSWCalibSmearingTool.h"

using namespace Muon;

Muon::NSWCalibSmearingTool::NSWCalibSmearingTool(const std::string& t,
						 const std::string& n, 
						 const IInterface* p ) :
  AthAlgTool(t,n,p)
{
  declareInterface<INSWCalibSmearingTool>(this);
  
  declareProperty("TimeSmear",   m_timeSmear   = {0.,0.,0.,0.,0.,0.,0.,0.});
  declareProperty("ChargeSmear", m_chargeSmear = {0.,0.,0.,0.,0.,0.,0.,0.});
  declareProperty("ChannelEfficiency",  m_channelEfficiency  = {1.,1.,1.,1.,1.,1.,1.,1.});
  declareProperty("ClusterEfficiency",  m_clusterEfficiency  = {1.,1.,1.,1.,1.,1.,1.,1.});

  declareProperty("GainFraction",  m_gainFraction  = {1.,1.,1.,1.,1.,1.,1.,1.});
  
  declareProperty("PhiSectors", m_phiSectors = {true,true,true,true,true,true,true,true} );
  // first two eta sectors are side-C, second two are side-A
  declareProperty("EtaSectors", m_etaSectors = {true,true,true,true} );
  
}

StatusCode Muon::NSWCalibSmearingTool::initialize()
{
  ATH_MSG_DEBUG("In initialize()");
  ATH_CHECK(m_idHelperSvc.retrieve());

  if ( !(m_idHelperSvc->hasMM() && m_idHelperSvc->hasSTgc() ) ) {
    ATH_MSG_ERROR("MM or STGC not part of initialized detector layout");
    return StatusCode::FAILURE;
  }
 
  m_random = TRandom3();

  return StatusCode::SUCCESS;
}

//
// check if a hit is acceppted
//
StatusCode Muon::NSWCalibSmearingTool::isAccepted(const Identifier id, bool& accepted)
{
  accepted = true;

  int etaSector = 0;
  int phiSector = 0;
  int gasGap = 0;

  if (!getIdFields(id,etaSector,phiSector,gasGap)) {
    ATH_MSG_WARNING("Invalid identifier");
    return StatusCode::SUCCESS;
  }

  /// check if a full hit can be accepted
  if ( m_random.Rndm() > m_clusterEfficiency.value()[gasGap-1] ) {
    accepted = false;
  }

  return StatusCode::SUCCESS;
}

//
// smear only the charge
//
StatusCode Muon::NSWCalibSmearingTool::smearCharge(Identifier id, float& charge, bool& accepted)
{

  ATH_MSG_DEBUG("Smearing the strips charge");

  int etaSector = 0;
  int phiSector = 0;
  int gasGap = 0;

  if (!getIdFields(id,etaSector,phiSector,gasGap)) {
    ATH_MSG_WARNING("Invalid identifier");
    return StatusCode::SUCCESS;
  }

  if ( m_phiSectors.value()[phiSector-1] && m_etaSectors.value()[etaSector-1] ) {
    // smear charge
    double chargeSmear = m_chargeSmear.value()[gasGap-1];
    charge = charge+m_random.Gaus(0.0,chargeSmear);
    
    // check if the single strip can be accepted
    accepted = true;
    if ( m_random.Rndm() > m_channelEfficiency.value()[gasGap-1] ) {
      accepted = false;
    }
  }

  return StatusCode::SUCCESS;
}

//
// smear time and charge
//
StatusCode Muon::NSWCalibSmearingTool::smearTimeAndCharge(Identifier id, float& time, float& charge, bool& accepted)
{
  
  if ( m_idHelperSvc->issTgc(id) ) {
    ATH_MSG_ERROR("Can't smear time for the STGC's");
    return StatusCode::FAILURE;
  } 

  int etaSector = 0;
  int phiSector = 0;
  int gasGap    = 0;

  if (!getIdFields(id,etaSector,phiSector,gasGap)) {
    ATH_MSG_WARNING("Invalid identifier");
    return StatusCode::SUCCESS;
  }

  if ( m_phiSectors.value()[phiSector-1] && m_etaSectors.value()[etaSector-1] ) {

    // smear time and charge
    double timeSmear   = m_timeSmear.value()[gasGap-1];
    double chargeSmear = m_chargeSmear.value()[gasGap-1];
    
    time = time+m_random.Gaus(0.0,timeSmear);
    charge = charge+m_random.Gaus(0.0,chargeSmear);

    // check if the RDO can be accepted
    accepted = true;
    if ( m_random.Rndm() > m_channelEfficiency.value()[gasGap-1] ) {
      accepted = false;
    }
  }

  return StatusCode::SUCCESS;
}

//
// get the fraction of the actual gain for a given gap
//
StatusCode Muon::NSWCalibSmearingTool::getGainFraction(Identifier id, float& gainFraction)
{
  int etaSector = 0;
  int phiSector = 0;
  int gasGap    = 0;

  if (!getIdFields(id,etaSector,phiSector,gasGap)) {
    ATH_MSG_WARNING("Invalid identifier");
    return StatusCode::SUCCESS;
  }

  gainFraction = 1.0;

  if ( m_phiSectors.value()[phiSector-1] && m_etaSectors.value()[etaSector-1] ) {
    gainFraction = m_gainFraction.value()[gasGap-1];
  }  

  return StatusCode::SUCCESS;
}


//
// get id fields for both STGC and MM
//
bool NSWCalibSmearingTool::getIdFields(const Identifier id, int& etaSector, int& phiSector, int& gasGap)
{
  if ( m_idHelperSvc->isMM(id) ) {
    int multilayer = m_idHelperSvc->mmIdHelper().multilayer(id);
    gasGap = (multilayer-1)*4+m_idHelperSvc->mmIdHelper().gasGap(id);
    etaSector = m_idHelperSvc->mmIdHelper().stationEta(id);
    phiSector = m_idHelperSvc->mmIdHelper().stationPhi(id);
  } 
  else if ( m_idHelperSvc->issTgc(id) ) {
    int multilayer = m_idHelperSvc->stgcIdHelper().multilayer(id);
    gasGap = (multilayer-1)*4+m_idHelperSvc->stgcIdHelper().gasGap(id);
    etaSector = m_idHelperSvc->stgcIdHelper().stationEta(id);
    phiSector = m_idHelperSvc->stgcIdHelper().stationPhi(id);
  } 
  else {
    ATH_MSG_WARNING("Wrong identifier: should be MM or STGC");
    return false;
  }

  // transform the eta sector
  etaSector < 0 ? etaSector = etaSector + 3 : etaSector = etaSector + 2;


  if ( phiSector < 1 || phiSector> (int) m_phiSectors.value().size() 
       || etaSector < (int) (-m_etaSectors.value().size()) || etaSector> (int) m_etaSectors.value().size() || etaSector==0
       || gasGap < 1 || gasGap> (int) m_timeSmear.value().size() || gasGap>(int) m_chargeSmear.value().size() ) {
    ATH_MSG_WARNING("Wrong phi, eta sector, or gasGap number: " << phiSector << " " 
		  << etaSector << " " << gasGap);
    return false;
  }

  return true;
}
