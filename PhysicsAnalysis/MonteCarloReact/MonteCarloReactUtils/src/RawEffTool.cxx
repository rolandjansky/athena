/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MonteCarloReactUtils/RawEffTool.h"
#include "MonteCarloReactUtils/EffExceptions.h"
#include "MonteCarloReactUtils/EffInfo.h"
#include "MonteCarloReactUtils/Efficiency.h"
#include "MonteCarloReactUtils/EffVal.h"

#include <vector>

using namespace MonteCarloReact;

RawEffTool::~RawEffTool() {;}

EffVal RawEffTool::getEff(std::vector<float> vec) {
  EffVal retval;
  int ndim = vec.size();
  int mdim = m_eff->getInfo().getEffNVars();
  if(ndim < 0 || ndim > 2 || ndim != mdim){ // do not allow larger than 3 dimensions for now
    throw EffConfigException("RawEffTool::Eff dimension out of range");
  }

  switch(ndim) {
  case 1:
    retval = m_eff->getEff(vec[0]); break;
  case 2:
    retval = m_eff->getEff(vec[0],vec[1]); break;
    /*comment case for now as it is dead code due to the if above
      case 3:
    retval = m_eff->getEff(vec[0],vec[1],vec[2]); break;
    */
  default:
    throw EffConfigException("RawEffTool::Eff dimension really out of range!");
  } // end switch
  
  return retval;
  
} // end Eff


