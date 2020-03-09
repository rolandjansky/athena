#include "NSWCalibTool.h"
#include "GaudiKernel/SystemOfUnits.h"

namespace {
  static constexpr double const& toRad = M_PI/180;
}

Muon::NSWCalibTool::NSWCalibTool(const std::string& t,
				  const std::string& n, 
				  const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
  m_magFieldSvc("AtlasFieldSvc",n)
{
  declareInterface<INSWCalibTool>(this);

  declareProperty("MagFieldSvc",   m_magFieldSvc, "Magnetic Field Service");
  declareProperty("DriftVelocity", m_vDrift = 47., "Drift Velocity");
  declareProperty("TimeResolution", m_timeRes = 25., "Time Resolution");

}


StatusCode Muon::NSWCalibTool::initialize()
{

  ATH_MSG_DEBUG("In initialize()");
  ATH_CHECK(m_magFieldSvc.retrieve());
  // initialize the MuonIdHelperTool and check the configuration
  ATH_CHECK(m_idHelperTool.retrieve());

  if ( !(m_idHelperTool->hasMM() && m_idHelperTool->hasSTgc() ) ) {
    ATH_MSG_ERROR("MuonIdHelperTool not properly configured, missing MM or STGC");
    return StatusCode::FAILURE;
  }
   m_lorentzAngleFunction = new TF1("lorentzAngleFunction","[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x",0,2);
   m_lorentzAngleFunction->SetParameters(0,58.87, -2.983, -10.62, 2.818);

  return StatusCode::SUCCESS;

}

StatusCode Muon::NSWCalibTool::calibrate( const Muon::MM_RawData* mmRawData, const Amg::Vector3D& globalPos, double& dist_drift, double& distRes_drift, double& calib_charge)
{
  double charge, time, vDriftCorrected;
  charge = mmRawData->charge();
  time = mmRawData->time();

  Amg::Vector3D magneticField;
  calib_charge = charge;

  m_magFieldSvc->getField(&globalPos,&magneticField);
  m_mmBFieldX = magneticField.x();
  m_mmBFieldY = magneticField.y();
  m_mmBFieldZ = magneticField.z();
  
  double lorentzAngle = m_lorentzAngleFunction->Eval((magneticField.y() > 0. ? 1. : -1.) * std::fabs (magneticField.y()) ) * toRad; // in radians;

  vDriftCorrected = m_vDrift * std::cos(lorentzAngle);
  dist_drift = vDriftCorrected * time;
  dist_drift =  dist_drift * Gaudi::Units::perThousand;
  distRes_drift = vDriftCorrected * m_timeRes;
  distRes_drift = distRes_drift * Gaudi::Units::perThousand;

  return StatusCode::SUCCESS;  
}


StatusCode Muon::NSWCalibTool::finalize()
{
  ATH_MSG_DEBUG("In finalize()");
  m_lorentzAngleFunction->Delete();
  return StatusCode::SUCCESS;
}
