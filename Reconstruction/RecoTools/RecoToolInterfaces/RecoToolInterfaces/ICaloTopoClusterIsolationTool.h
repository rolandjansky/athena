/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITopoClusterIsolationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef CALOTOPOCLUSTERISOLATIONTOOLS_ICALOTOPOCLUSTERISOLATIONTOOL_H
#define CALOTOPOCLUSTERISOLATIONTOOLS_ICALOTOPOCLUSTERISOLATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODBase/IParticle.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "IsolationCommon.h"

//class CaloClusterContainer;

namespace xAOD {

  static const InterfaceID IID_ICaloTopoClusterIsolationTool("xAOD::ICaloTopoClusterIsolationTool", 1, 0);

  /** @class ICaloTopoClusterIsolationTool
      @brief interface for tools calculating topo cluster isolation
 
      @author Niels van Eldik, Sandrine Laplace
   */

  class ICaloTopoClusterIsolationTool : virtual public IAlgTool {
  public:

    static const InterfaceID& interfaceID( ) ;


    /**ICaloTopoClusterIsolationTool interface for cluster isolation: 
       @param[in] result    output object to be filled
       @param[in] tp        input iparticle
       @param[in] cones     vector of input cones to be used
       @param[in] corrections bitset specifying which corrections to apply to isolation
       @param[in] container topo cluster contrainer (for trigger only)
       @return true if the calculation was successfull
    */    
    virtual bool caloTopoClusterIsolation(CaloIsolation& result, const IParticle& tp, 
					  const std::vector<Iso::IsolationType>& cones, 
					  CaloCorrection corrections, 
					  const CaloClusterContainer* container = 0 ) = 0;

    virtual bool decorateParticle_topoClusterIso(IParticle& tp,
                                                 const std::vector<Iso::IsolationType>& cones,
                                                 CaloCorrection corrections,
                                                 const CaloClusterContainer* TopClusters = 0) =0; 
  };
  
  inline const InterfaceID& ICaloTopoClusterIsolationTool::interfaceID() { 
    return IID_ICaloTopoClusterIsolationTool; 
  }

} // end of namespace

#endif 
