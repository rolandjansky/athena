/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
//#include "xAODTruth/TruthEvent.h"
//#include "xAODTruth/TruthEventContainer.h"


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::RandomRoISeedTool::RandomRoISeedTool
(const std::string& t,const std::string& n,const IInterface* p)
  : AthAlgTool(t,n,p),
    m_z0_window(1.0),
  m_atRndmSvc("AtRndmGenSvc", n),
  m_randomStreamName("MixingEventSelectorStream"),
  m_chooseRandGauss(nullptr)

{

  //
  declareInterface<IZWindowRoISeedTool>(this);

  //
  //declareProperty("InputTruthEventsCollection", m_input_truth_events );  
  declareProperty("TrackZ0Window", m_z0_window = 1.0);
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

  //TRandom3 *random = new TRandom3();
  //m_random = new TRandom3();

  //setup random stream
  CLHEP::HepRandomEngine* collEng(m_atRndmSvc->GetEngine(m_randomStreamName.value()));
  if(nullptr == collEng ) {
    ATH_MSG_ERROR ("can not get random stream " << m_randomStreamName.value());
    return StatusCode::FAILURE;
  }
  //flat distribution in [0,1] range
  m_chooseRandGauss = new CLHEP::RandGauss(*(collEng), 0.0, 35.0);


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

  //TRandom *random = new TRandom();
  //TRandom3 *random = new TRandom3();
  //m_random->SetSeed();

  float z_val;
  z_val = m_chooseRandGauss->fire();
  //z_val = m_random->Gaus(0,35);

  //while( std::abs( ZRef - z_val ) < 5 || std::abs(z_val) > 250.0 ){
  //z_val = m_random->Gaus(0,35);
  //}
  
  // prepare output
  std::vector<InDet::IZWindowRoISeedTool::ZWindow> listRoIs;  
  listRoIs.clear();
  
  InDet::IZWindowRoISeedTool::ZWindow RoI;
  RoI.z_reference = z_val;
  RoI.z_window[0] = RoI.z_reference - m_z0_window; 
  RoI.z_window[1] = RoI.z_reference + m_z0_window;
  listRoIs.push_back(RoI);
  ATH_MSG_DEBUG("Random RoI: " << RoI.z_reference << " [" << RoI.z_window[0] << ", " << RoI.z_window[1] << "]");
  
  return listRoIs;
  
}

