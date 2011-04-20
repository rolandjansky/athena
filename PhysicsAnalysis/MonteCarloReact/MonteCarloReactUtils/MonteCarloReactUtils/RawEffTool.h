/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************
Created by: Marc Hohlfeld
****************************************************/
#ifndef MONTECARLOREACT_RAWEFFTOOL_H
#define MONTECARLOREACT_RAWEFFTOOL_H


#include "MonteCarloReactUtils/EffTool.h"
#include "MonteCarloReactUtils/EffVal.h"

#include <vector>


class Efficiency;
class EffInfo;

namespace MonteCarloReact {

  class RawEffTool : public EffTool {

  public:

    /* Constructor */
    RawEffTool() : EffTool() { ; }
    RawEffTool( const EffInfo & request) : EffTool( request ) {;}

    /* Destructor */
    ~RawEffTool();

    /* Return Efficiency */
    EffVal getEff(std::vector<float> vec); 

  };
}
#endif 
