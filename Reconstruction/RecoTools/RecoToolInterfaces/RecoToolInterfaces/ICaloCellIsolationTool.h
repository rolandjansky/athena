/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICaloCellIsolationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef CALOCELLISOLATIONTOOLS_ICALOCELLISOLATIONTOOL_H
#define CALOCELLISOLATIONTOOLS_ICALOCELLISOLATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODBase/IParticle.h"
#include "IsolationCommon.h"

class CaloCellContainer;

namespace xAOD {

  static const InterfaceID IID_ICaloCellIsolationTool("xAOD::ICaloCellIsolationTool", 1, 0);

  /** @class ICaloCellIsolationTool
      @brief interface for tools calculating track isolation
 
      @author Niels van Eldik, Sandrine Laplace
   */

  class ICaloCellIsolationTool : virtual public IAlgTool {
  public:

    static const InterfaceID& interfaceID( ) ;

    /**ICaloCellIsolationTool interface for cell isolation: 
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
				    const CaloCellContainer* container = 0 ) = 0; 
    
    virtual bool decorateParticle_caloCellIso( IParticle& tp,
                             const std::vector<Iso::IsolationType>& cones,
                             CaloCorrection corrections,
                             const CaloCellContainer* Cells = 0) = 0;
  };

  inline const InterfaceID& ICaloCellIsolationTool::interfaceID() { 
    return IID_ICaloCellIsolationTool; 
  }

} // end of namespace

#endif 
