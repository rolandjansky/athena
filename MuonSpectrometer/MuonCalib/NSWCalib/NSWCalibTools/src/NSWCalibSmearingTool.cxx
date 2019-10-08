#include "NSWCalibSmearingTool.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"

using namespace Muon;

Muon::NSWCalibSmearingTool::NSWCalibSmearingTool(const std::string& t,
						 const std::string& n, 
						 const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool")
{
  declareInterface<INSWCalibSmearingTool>(this);
  
  declareProperty("TimeSmearPattern",   m_timeSmear   = {0.,0.,0.,0.,0.,0.,0.,0.});
  declareProperty("ChargeSmearPattern", m_chargeSmear = {0.,0.,0.,0.,0.,0.,0.,0.});
  declareProperty("EfficiencyPattern",  m_efficiency  = {1.,1.,1.,1.,1.,1.,1.,1.});
  
  declareProperty("PhiSectors", m_phiSectors = {true,true,true,true,true,true,true,true} );
  declareProperty("EtaSectors", m_etaSectors = {true,true} );
  
}


Muon::NSWCalibSmearingTool::~NSWCalibSmearingTool()
{ }


StatusCode Muon::NSWCalibSmearingTool::initialize()
{
  ATH_MSG_DEBUG("In initialize()");

  // initialize the MuonIdHelperTool and check the configuration
  ATH_CHECK(m_idHelperTool.retrieve());

  if ( !(m_idHelperTool->HasMM() && m_idHelperTool->HasSTgc() ) ) {
    ATH_MSG_ERROR("MuonIdHelperTool not properly configured, missing MM or STGC");
    return StatusCode::FAILURE;
  }
 
  m_random = TRandom3();

  return StatusCode::SUCCESS;
}


StatusCode Muon::NSWCalibSmearingTool::finalize()
{
  ATH_MSG_DEBUG("In finalize()");

  return StatusCode::SUCCESS;
}

//
// smear only the charge
//
StatusCode Muon::NSWCalibSmearingTool::smearCharge(Identifier id, float& charge, bool& accepted)
{
  
  unsigned int gasGap = 0;
  unsigned int eta    = 0;
  unsigned int phi    = 0;

  if ( m_idHelperTool->isMM(id) ) {
    int multilayer = m_idHelperTool->mmIdHelper().multilayer(id);
    gasGap = (multilayer-1)*4+m_idHelperTool->mmIdHelper().gasGap(id);
    eta = m_idHelperTool->mmIdHelper().stationEta(id);
    phi = m_idHelperTool->mmIdHelper().stationPhi(id);
  } 
  else if ( m_idHelperTool->issTgc(id) ) {
    int multilayer = m_idHelperTool->stgcIdHelper().multilayer(id);
    gasGap = (multilayer-1)*4+m_idHelperTool->stgcIdHelper().gasGap(id);
    eta = m_idHelperTool->stgcIdHelper().stationEta(id);
    phi = m_idHelperTool->stgcIdHelper().stationPhi(id);
  } 
  else {
    ATH_MSG_ERROR("Wrong identifier: should be MM or STGC");
    return StatusCode::FAILURE;
  }

  if ( phi < 1 || phi>m_phiSectors.value().size() 
       || eta < 1 || eta>m_etaSectors.value().size() 
       || gasGap < 1 || gasGap> m_timeSmear.value().size() || gasGap>m_chargeSmear.value().size() ) {
    ATH_MSG_ERROR("Wrong phi, eta sector, or gasGap number: " << phi << " " << eta << " " << gasGap);
    return StatusCode::FAILURE;
  }

  if ( m_phiSectors.value()[phi-1] && m_etaSectors.value()[eta-1] ) {
    // smear charge
    double chargeSmear = m_chargeSmear.value()[gasGap-1];
    charge = charge+m_random.Gaus(0.0,chargeSmear);
    
    // check if the RDO can be accepted
    accepted = false;
    if ( m_random.Rndm() <= m_efficiency.value()[gasGap-1] ) {
      accepted = true;
    }
  }

  return StatusCode::SUCCESS;
}

//
// smear time and charge
//
StatusCode Muon::NSWCalibSmearingTool::smearTimeAndCharge(Identifier id, float& time, float& charge, bool& accepted)
{
  
  unsigned int gasGap = 0;
  unsigned int eta    = 0;
  unsigned int phi    = 0;

  if ( m_idHelperTool->isMM(id) ) {
    int multilayer = m_idHelperTool->mmIdHelper().multilayer(id);
    gasGap = (multilayer-1)*4+m_idHelperTool->mmIdHelper().gasGap(id);
    eta = m_idHelperTool->mmIdHelper().stationEta(id);
    phi = m_idHelperTool->mmIdHelper().stationPhi(id);
  } 
  else if ( m_idHelperTool->issTgc(id) ) {
    ATH_MSG_ERROR("Can't smear time for the STGC's");
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_ERROR("Wrong identifier: should be MM or STGC");
    return StatusCode::FAILURE;
  }

  if ( phi < 1 || phi>m_phiSectors.value().size() 
       || eta < 1 || eta>m_etaSectors.value().size() 
       || gasGap < 1 || gasGap> m_timeSmear.value().size() || gasGap>m_chargeSmear.value().size() ) {
    ATH_MSG_ERROR("Wrong phi, eta sector, or gasGap number: " << phi << " " << eta << " " << gasGap);
    return StatusCode::FAILURE;
  }

  if ( m_phiSectors.value()[phi-1] && m_etaSectors.value()[eta-1] ) {

    // smear time and charge
    double timeSmear   = m_timeSmear.value()[gasGap-1];
    double chargeSmear = m_chargeSmear.value()[gasGap-1];
    
    time = time+m_random.Gaus(0.0,timeSmear);
    charge = charge+m_random.Gaus(0.0,chargeSmear);

    // check if the RDO can be accepted
    accepted = false;
    if ( m_random.Rndm() <= m_efficiency.value()[gasGap-1] ) {
      accepted = true;
    }
  }

  return StatusCode::SUCCESS;
}

