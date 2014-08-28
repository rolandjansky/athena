/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ///////////////////////////////////////////////////////////////////
// PAUprimaryVertexFinder.h, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////


#ifndef PHOTONANALYSISUTILS_TOPOCLUSTERTOOL_H
#define PHOTONANALYSISUTILS_TOPOCLUSTERTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

//Added Marc to check :
#include "AthenaBaseComps/AthAlgTool.h"

// GaudiKernel includes
#include "GaudiKernel/IAlgTool.h"

//===

#include "PhotonAnalysisUtils/IPAUtopoClusterTool.h"

class PAUtopoClusterTool : public AthAlgTool, virtual public IPAUtopoClusterTool {
 public:
  
  /** AlgTool like constructor */
  PAUtopoClusterTool(const std::string&,const std::string&,const IInterface*);
  
  /**Virtual destructor*/
  virtual ~PAUtopoClusterTool();
  
  /** AlgTool initailize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();
  
  virtual const CaloCluster* getCluster(const CaloCluster* clus, const CaloClusterContainer* clusContainer, const std::vector<float>& dRcone, 
					std::vector<float>& EtCone) ;

 private:

  float m_EtThreshold, m_DeltaRMatch, m_EtaMax ;
  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  
};


#endif // PHOTONANALYSISUTILS_TOPOCLUSTERTOOL_H
