/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPAUprimaryVertexFinder.h, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSISUTILS_ITOPOCLUSTERTOOL_H
#define PHOTONANALYSISUTILS_ITOPOCLUSTERTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include <vector>

class CaloCluster;
class CaloClusterContainer;


static const InterfaceID IID_IPAUtopoClusterTool("IPAUtopoClusterTool", 1, 0);
        
class IPAUtopoClusterTool : virtual public IAlgTool {
  
 public:
  /** Virtual destructor */
  virtual ~IPAUtopoClusterTool(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_IPAUtopoClusterTool; };

  virtual const CaloCluster* getCluster(const CaloCluster* clus, const CaloClusterContainer* clusContainer, const std::vector<float>& dRcone, 
					std::vector<float>& EtCone) =0;
  
};

#endif // PHOTONANALYSISUTILS_ITOPOCLUSTER_H
