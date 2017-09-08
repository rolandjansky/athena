/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICaloIsolationDecoratorTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef XAOD_ICALOISOLATIONDECORATORTOOL_H
#define XAOD_ICALOISOLATIONDECORATORTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODBase/IParticle.h"
#include "IsolationCommon.h"

class CaloCellContainer;
class CaloClusterContainer;

namespace xAOD {

  static const InterfaceID IID_ICaloIsolationDecoratorTool("xAOD::ICaloIsolationDecoratorTool", 1, 0);

  /** @class ICaloIsolationDecoratorTool
      @brief interface for tools calculating track isolation
 
      @author Niels van Eldik, Sandrine Laplace
   */

  class ICaloIsolationDecoratorTool : virtual public IAlgTool {
  public:

    static const InterfaceID& interfaceID( ) ;

    /**ICaloIsolationDecoratorTool interface for cell isolation: 
       @param[in] tp        input iparticle
       @param[in] cones     vector of input cones to be used
       @param[in] corrections bitset specifying which corrections to apply to isolation
       @param[in] Cells cell contrainer (for trigger only)
       @param[in] TopClusters topoCluster contrainer (for trigger only)
       @return true if the decoration was successfull
    */    
    virtual bool decorateParticle( const IParticle& tp,
				   const std::vector<Iso::IsolationType>& cones, 
				   CaloCorrection corrections, 
				   const CaloCellContainer* Cells = 0,
                                   const CaloClusterContainer* TopClusters = 0) = 0; 
    
  };
  
  inline const InterfaceID& ICaloIsolationDecoratorTool::interfaceID() { 
    return IID_ICaloIsolationDecoratorTool; 
  }

} // end of namespace

#endif 
