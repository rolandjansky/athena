/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetSubtractorToolBase.h"
#include "HIEventUtils/HIEventDefs.h"

HIJetSubtractorToolBase::HIJetSubtractorToolBase(const std::string& myname) : asg::AsgTool(myname)
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

bool HIJetSubtractorToolBase::inTowerBoundary(float eta0, float phi0, float eta, float phi) const
{
  if( 2.*std::abs(eta-eta0) > HI::TowerBins::getBinSizeEta() ) return false;
  if( 2.*std::abs(phi-phi0) > HI::TowerBins::getBinSizePhi() ) return false;
  return true;
}

void HIJetSubtractorToolBase::setSubtractedEtaPhi(float E, float& eta, float& phi, float eta0, float phi0, float sig) const
{
  if( E < MinEnergyForMoments() || ( (sig > 0) && sig < MinEnergySigForMoments() ) )
  {
    eta=eta0;
    phi=phi0;
  }
  else 
  {
    if( 2.*std::abs(eta-eta0) > HI::TowerBins::getBinSizeEta() ) eta=eta0;
    if( 2.*std::abs(phi-phi0) > HI::TowerBins::getBinSizePhi() ) phi=phi0;
  }
}
