/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetSubtractorToolBase.h"
#include "HIJetRec/HIJetRecDefs.h"
#include <TVector2.h>

HIJetSubtractorToolBase::HIJetSubtractorToolBase(const std::string& myname) : asg::AsgTool(myname),
									      m_useCells(true),
									      m_shape(nullptr),
									      m_index(nullptr),
									      m_modulator(nullptr)
{
#ifdef ASGTOOL_ATHENA
  //should not need this due to ASG_TOOL_CLASS macro since 
  //athena only calls 3 arg constructor which explicitly declares athena interface?
  declareInterface<IHISubtractorTool>(this); 
#endif

  declareProperty("MinimumEnergyForMoments",m_E_min_moment=50.,"> E, cluster given tower coordinates");
  declareProperty("MinimumSignificanceForMoments",m_E_sig_moment=0.1,"if E after subtr / E total < this cut, cluster given tower coordinates");
  declareProperty("UpdateClusters",m_update_clusters=false,"If true set cluster kinematics to reflect subtraction");

}


void HIJetSubtractorToolBase::setSubtractedEtaPhi(float E, float& eta, float& phi, float eta0, float phi0, float sig) const
{
  phi=TVector2::Phi_mpi_pi(phi);
  if(!HIJetRec::inTowerBoundary(eta0,phi0,eta,phi))
  {
    eta=eta0;
    phi=phi0;
  }
  else if( E < MinEnergyForMoments() || ( (sig > 0) && sig < MinEnergySigForMoments() ) )
  {
    eta=eta0;
    phi=phi0;
  }

}

StatusCode HIJetSubtractorToolBase::configureEvent(const xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const IHIUEModulatorTool* modulator)
{
  setShape(shape);
  setIndex(index);
  setModulator(modulator);
  return StatusCode::SUCCESS;

}
