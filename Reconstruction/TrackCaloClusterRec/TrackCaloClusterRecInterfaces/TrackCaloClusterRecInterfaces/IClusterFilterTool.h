///////////////////////////////////////////////////////////////////
// IClusterFilterTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECINTERFACES_ICLUSTERFILTERTOOL_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECINTERFACES_ICLUSTERFILTERTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

static const InterfaceID IID_IClusterFilterTool("IClusterFilterTool", 1, 0);

/** @brief Interface for extending Particles with calo intersections */
class IClusterFilterTool : virtual public IAlgTool {
  public:
    
    /** Method to check if a given cluster has to be rejected or not
     *  based on primary vertices requirements
     *  @param CaloCluster   reference to the cluster
     *  @return true if the cluster has been rejected
     * 
     */
    
    virtual bool rejectCluster(const xAOD::CaloCluster& cluster) = 0;
    
    static const InterfaceID& interfaceID( ) ;
};

inline const InterfaceID& IClusterFilterTool::interfaceID() {
  return IID_IClusterFilterTool; 
}

#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECINTERFACES_ICLUSTERFILTERTOOL_H