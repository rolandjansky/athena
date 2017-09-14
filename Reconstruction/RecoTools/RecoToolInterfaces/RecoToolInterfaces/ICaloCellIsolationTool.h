/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICaloCellIsolationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef CALOCELLISOLATIONTOOLS_ICALOCELLISOLATIONTOOL_H
#define CALOCELLISOLATIONTOOLS_ICALOCELLISOLATIONTOOL_H

// #include "GaudiKernel/IAlgTool.h"
#include "AsgTools/AsgTool.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODBase/IParticle.h"
#include "IsolationCommon.h"

class CaloCellContainer;

namespace xAOD {

  /** @class ICaloCellIsolationTool
      @brief interface for tools calculating track isolation
 
      @author Niels van Eldik, Sandrine Laplace
   */
  class ICaloCellIsolationTool : virtual public asg::IAsgTool {
    ASG_TOOL_INTERFACE( xAOD::ICaloCellIsolationTool )
  public:

    /**ICaloCellIsolationTool interface for cell isolation: 
       The tool expects the cones to be order in decreasing order (etcone40 -> etcone20)
       Internally it reorders the cones so the output isolation values are also in the same order. 
       @param[in] result    output object to be filled
       @param[in] tp        input iparticle
       @param[in] cones     vector of input cones to be used
       @param[in] corrections bitset specifying which corrections to apply to isolation
       @param[in] container cell contrainer (for trigger only)
       @return true if the calculation was successfull
    */    
    virtual bool caloCellIsolation( CaloIsolation& result, const IParticle& tp, 
				    const std::vector<Iso::IsolationType>& cones, 
				    CaloCorrection corrections, 
				    const CaloCellContainer* container = 0 ) const = 0; 

    /**ICaloCellIsolationTool interface for calculating the cell isolation variables and decorating the particle: 
       The tool expects the cones to be order in decreasing order (etcone40 -> etcone20)
       Internally it reorders the cones so the output isolation values are also in the same order. 
       @param[in] tp        input iparticle
       @param[in] cones     vector of input cones to be used
       @param[in] corrections bitset specifying which corrections to apply to isolation
       @param[in] container cell contrainer (for trigger only)
       @return true if the calculation and decoration was successfull
    */    
    virtual bool decorateParticle_caloCellIso( const IParticle& tp,
                             const std::vector<Iso::IsolationType>& cones,
                             CaloCorrection corrections,
                             const CaloCellContainer* Cells = 0) = 0;
  };

} // end of namespace

#endif 
