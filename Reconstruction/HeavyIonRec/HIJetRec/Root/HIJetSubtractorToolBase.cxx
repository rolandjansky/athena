/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetSubtractorToolBase.h"

HIJetSubtractorToolBase::HIJetSubtractorToolBase(const std::string& myname) : asg::AsgTool(myname)
{
#ifdef ASGTOOL_ATHENA
  //should not need this due to ASG_TOOL_CLASS macro since 
  //athena only calls 3 arg constructor which explicitly declares athena interface?
  declareInterface<IHISubtractorTool>(this); 
#endif

  declareProperty("MinimumEnergyForMoments",m_E_min_moment=50.,"> E, cluster given tower coordinates");
  declareProperty("UpdateClusters",m_update_clusters=false,"If true set cluster kinematics to reflect subtraction");
  declareProperty("HarmonicsList",m_harmonics_vec,"List of harmonics included in UE modulation");
}

StatusCode HIJetSubtractorToolBase::initialize()
{
  //Set cannot be passed as declare property
  //Make sure list is unique
  for(auto itr=m_harmonics_vec.begin(); itr!=m_harmonics_vec.end(); itr++) m_harmonics.insert(*itr);
    return StatusCode::SUCCESS;
}

