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
  
  declareProperty("TimeSmearPattern",   m_timeSmear      ={0.,0.,0.,0.,0.,0.,0.,0.});
  declareProperty("ChargeSmearPattern", m_chargeSmear    ={0.,0.,0.,0.,0.,0.,0.,0.});
  declareProperty("EfficiencyPattern",  m_efficiency     ={1.,1.,1.,1.,1.,1.,1.,1.});

}


Muon::NSWCalibSmearingTool::~NSWCalibSmearingTool()
{ }


StatusCode Muon::NSWCalibSmearingTool::initialize()
{
  ATH_MSG_DEBUG("In initialize()");

  // initialize the MuonIdHelperTool and check the configuration
  ATH_CHECK(m_idHelperTool.retrieve());

  if ( !(m_idHelperTool->hasMM() && m_idHelperTool->hasSTgc() ) ) {
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


StatusCode Muon::NSWCalibSmearingTool::smearAndSelect(Identifier id, double& time, double& charge, bool& accepted)
{
  
  int gasGap = 0;

  if ( m_idHelperTool->isMM(id) ) {
    int multilayer = m_idHelperTool->mmIdHelper().multilayer(id);
    gasGap = (multilayer-1)*4+m_idHelperTool->mmIdHelper().gasGap(id);
  } 
  else if ( m_idHelperTool->issTgc(id) ) {
    int multilayer = m_idHelperTool->stgcIdHelper().multilayer(id);
    gasGap = (multilayer-1)*4+m_idHelperTool->stgcIdHelper().gasGap(id);
  } 
  else {
    ATH_MSG_ERROR("Wrong identifier: should be MM or STGC");
    return StatusCode::FAILURE;
  }

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

  return StatusCode::SUCCESS;
}

