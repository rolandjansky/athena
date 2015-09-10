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

}

