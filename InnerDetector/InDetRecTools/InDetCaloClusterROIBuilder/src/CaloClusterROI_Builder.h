/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERROI_BUILDER_H
#define CALOCLUSTERROI_BUILDER_H

#include "InDetRecToolInterfaces/ICaloClusterROI_Builder.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODCaloEvent/CaloClusterFwd.h"

namespace Trk{
  class CaloClusterROI;
  class Surface;
  class LocalParameters;
}

class ICaloSurfaceBuilder;

namespace InDet{

class CaloClusterROI_Builder : public AthAlgTool, virtual public ICaloClusterROI_Builder
{
 public:
  /** Constructor with AlgTool parameters */
  CaloClusterROI_Builder(const std::string&, const std::string&, const IInterface*);

  CaloClusterROI_Builder();
  ~CaloClusterROI_Builder(); 

  // standard Athena methods
  virtual StatusCode initialize();
  virtual StatusCode finalize();  
  
  virtual Trk::CaloClusterROI* buildClusterROI( const xAOD::CaloCluster* cl ) const;


 private:
 
  const Trk::Surface*           getCaloSurface( const xAOD::CaloCluster* cluster ) const;
  const Trk::LocalParameters*   getClusterLocalParameters( const xAOD::CaloCluster* cluster, 
                                                           const Trk::Surface* surf) const;
                                                           
  
  bool   findPosition(const xAOD::CaloCluster* cluster) const;
  
  /** @brief Tool to build calorimeter layer surfaces */
  ToolHandle<ICaloSurfaceBuilder>  m_calosurf;
  
};

}

#endif //CALOCLUSTERROI_BUIDLER_H
