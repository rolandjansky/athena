/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_ELECTRONREWEIGHT_H
#define DERIVATIONFRAMEWORK_ELECTRONREWEIGHT_H 

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/Electron.h"
#include "CaloEvent/CaloCell.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "egammaInterfaces/IEMShowerBuilder.h"
#include "egammaInterfaces/IegammaSwTool.h"

class CaloFillRectangularCluster;
class EMShowerBuilder;

namespace DerivationFramework {
class ElectronReweight: public AthAlgTool, public IAugmentationTool{ 
 public: 
  ElectronReweight( const std::string& t, const std::string& n, const IInterface* p );
  virtual ~ElectronReweight(); 

  StatusCode  initialize();
  virtual StatusCode  addBranches() const;
  StatusCode  finalize();
  void DecorateElec(const xAOD::Egamma *elec, xAOD::Egamma *c_elec) const;

 private: 
  ToolHandle<IEMShowerBuilder> m_EMShowerBuilderTool;
  ToolHandle<IegammaSwTool>   m_clusterCorrectionTool;
  std::string m_name_new, m_name_old,m_ClusterCorrectionToolName;
  std::string m_EMShowerBuilderToolName;
  std::string m_SGKey_electrons, m_SGKey_photons;
  bool isDecor;
  std::string name_electron, name_gamma, PreNm;
  };
}

#endif //> !DERIVATIONFRAMEWORK_ELECTRONREWEIGHT_H
