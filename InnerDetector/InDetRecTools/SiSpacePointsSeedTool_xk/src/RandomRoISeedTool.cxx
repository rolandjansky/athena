/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class RandomRoISeedTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#include "SiSpacePointsSeedTool_xk/RandomRoISeedTool.h"
#include "TVector2.h"
#include <map>
#include "TRandom3.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandGauss.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::RandomRoISeedTool::RandomRoISeedTool
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_z0Window(1.0),
  m_atRndmSvc("AtRndmGenSvc", n),
  m_randomStreamName("MixingEventSelectorStream"),
  m_chooseRandGauss(nullptr)

{

  //
  declareInterface<IZWindowRoISeedTool>(this);

  //
  declareProperty("TrackZ0Window", m_z0Window = 1.0);
  declareProperty("RndmGenSvc", m_atRndmSvc, "IAtRndmGenSvc controlling the order with which events are takes from streams");
  declareProperty("RndmStreamName", m_randomStreamName, "IAtRndmGenSvc stream used as engine for our random distributions");   

}

///////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////

InDet::RandomRoISeedTool::~RandomRoISeedTool()
{
}

///////////////////////////////////////////////////////////////////
// Initialization
///////////////////////////////////////////////////////////////////

StatusCode InDet::RandomRoISeedTool::initialize()
{
  StatusCode sc = AlgTool::initialize();   

  //setup random stream
  CLHEP::HepRandomEngine* collEng(m_atRndmSvc->GetEngine(m_randomStreamName.value()));
  if(nullptr == collEng ) {
    ATH_MSG_ERROR ("can not get random stream " << m_randomStreamName.value());
    return StatusCode::FAILURE;
  }

  m_chooseRandGauss = new CLHEP::RandGauss(*(collEng), 0.0, 1.0); //Want to sample Gaussian with mean of 0 and sigma of 1


  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::RandomRoISeedTool::finalize()
{
   StatusCode sc = AlgTool::finalize(); 
   return sc;
}

/////////////////////////////////////////////////////////////////////
// Compute RoI
/////////////////////////////////////////////////////////////////////

std::vector<InDet::IZWindowRoISeedTool::ZWindow> InDet::RandomRoISeedTool::getRoIs()
{

  // -----------------------------------
  // Retrieve beamspot information
  // -----------------------------------

  float bsSigZ = 0.0;

  IBeamCondSvc* iBeamCondSvc; 
  StatusCode scBCS = service("BeamCondSvc", iBeamCondSvc);

  if (scBCS.isFailure() || iBeamCondSvc == 0) {
    iBeamCondSvc = 0;
    ATH_MSG_ERROR ("Could not retrieve Beam Conditions Service.");
  } else{
    bsSigZ = iBeamCondSvc->beamSigma(2);
  }

  float zVal;
  zVal = m_chooseRandGauss->fire() * bsSigZ; //This effectively samples from a beamspot with the correct beamspot sigma_z
  
  // prepare output
  std::vector<InDet::IZWindowRoISeedTool::ZWindow> listRoIs;  
  listRoIs.clear();
  
  InDet::IZWindowRoISeedTool::ZWindow RoI;
  RoI.zReference = zVal;
  RoI.zWindow[0] = RoI.zReference - m_z0Window; 
  RoI.zWindow[1] = RoI.zReference + m_z0Window;
  listRoIs.push_back(RoI);
  ATH_MSG_DEBUG("Random RoI: " << RoI.zReference << " [" << RoI.zWindow[0] << ", " << RoI.zWindow[1] << "]");
  
  return listRoIs;
  
}

