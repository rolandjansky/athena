/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICaloClustersInConeTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef PARTICLESINCONETOOLS_ICALOCLUSTERSINCONETOOL_H
#define PARTICLESINCONETOOLS_ICALOCLUSTERSINCONETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include <vector>

namespace xAOD {

  static const InterfaceID IID_ICaloClustersInConeTool("xAOD::ICaloClustersInConeTool", 1, 0);

  /** @class ICaloClustersInConeTool
      @brief interface for collecting truth particles inside a cone
 
      @author Niels van Eldik
   */
  class ICaloClustersInConeTool : virtual public IAlgTool {
  public:

    static const InterfaceID& interfaceID( ) ;

    /**ICaloClustersInConeTool interface: 
       @param[in] eta       eta for matching
       @param[in] phi       phi for matching
       @param[in] dr        cone size
       @param[in] output    output vector to be filled
       @return true if the calculation was successfull
     */    
    virtual bool particlesInCone( float eta, float phi, float dr, std::vector< const CaloCluster*>& output ) = 0;

    /**ICaloClustersInConeTool interface: 
       @param[in] eta       eta for matching
       @param[in] phi       phi for matching
       @param[in] dr        cone size
       @param[in] output    output vector to be filled
       @return true if the calculation was successfull
     */    
    virtual bool particlesInCone( float eta, float phi, float dr, std::vector< ElementLink<CaloClusterContainer> >& output ) = 0;

  };

  inline const InterfaceID& ICaloClustersInConeTool::interfaceID() { 
    return IID_ICaloClustersInConeTool; 
  }

} // end of namespace

#endif 
