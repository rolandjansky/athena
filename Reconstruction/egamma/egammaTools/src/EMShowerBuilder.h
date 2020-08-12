/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EMSHOWERBUILDER_H
#define EGAMMATOOLS_EMSHOWERBUILDER_H
/**
   @class EMShowerBuilder
   EMShower data object builder :
   - Get pointing information associated to the cluster from the egammaqpoint
   tool
   - Get shower depth associated to the cluster from the egammaqgcld tool
   - Define a Calo Cell list corresponding to EM, HAD, FCAL Calo
   - egammaIso calculates information concerning isolation behind em clusters
   in the hadronic calorimeter and around the em cluster
   for isolation around em clustr use cone of different sizes
   - Calculate shower shapes in all samplings from egammaShowerShape tool
   - When running on AOD data(defined as samples which do not contain AllCalo
   CaloCellContainer) as there is not enough cells, the showers are not
   recalculated. On the other hand we try to find the original
   electron/photon object which matches the cluster and
   keep this information in the new EMShower object

   @author H. Ma
   @author F. Derue
*/

// INCLUDE HEADER FILES:
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "egammaBaseTool.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "egammaInterfaces/IEMShowerBuilder.h"
#include "egammaInterfaces/IegammaIso.h"
#include "egammaInterfaces/IegammaShowerShape.h"

#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

class StoreGateSvc;
class CaloCellContainer;
class IChronoStatSvc;
class CaloDetDescrManager;

class EMShowerBuilder
  : public AthAlgTool
  , virtual public IEMShowerBuilder
{
public:
  /** @brief Default constructor*/
  EMShowerBuilder(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  /** @brief Destructor*/
  ~EMShowerBuilder() = default;

  /** @brief initialize method*/
  virtual StatusCode initialize() override final;
  /** @brief standard execute method */
  virtual StatusCode execute(const EventContext& ctx,
                             const CaloDetDescrManager& cmgr,
                             xAOD::Egamma*) const override final;

  virtual StatusCode executeWithCells(const CaloCellContainer* cellcoll,
                                      const CaloDetDescrManager& cmgr,
                                      xAOD::Egamma*) const override final;
  /** @brief finalize method*/
  virtual StatusCode finalize() override;

private:
  /** @brief method to retrieve ShowerBuilder tool */
  StatusCode RetrieveShowerShapeTool();
  /** @brief method to retrieve hadronic leakage calculation from CaloIso tool
   */
  StatusCode RetrieveHadronicLeakageTool();
  /** @brief calculate shower shapes*/
  StatusCode CalcShowerShape(xAOD::Egamma* eg,
                             const CaloDetDescrManager& cmgr,
                             const CaloCellContainer* cellcoll) const;
  /** @brief calculate Hadronic leakage*/
  StatusCode CalcHadronicLeakage(xAOD::Egamma* eg,
                                 const CaloDetDescrManager& cmgr,
                                 const xAOD::CaloCluster* clus,
                                 const CaloCellContainer* cellcoll) const;
  /** @brief fill shower detail from shower shape calculation*/
  StatusCode FillEMShowerShape(xAOD::Egamma* eg,
                               const IegammaShowerShape::Info& info) const;

  /** @brief Cell container*/
  SG::ReadHandleKey<CaloCellContainer> m_cellsKey{
    this,
    "CellsName",
    "AllCalo",
    "Names of containers which contain cells"
  };

  /** @brief Obsolete enum on the layers to use for the HadLeakage variables*/
  Gaudi::Property<std::vector<int>> m_caloNums{ this,
                                                "CaloNums",
                                                {},
                                                "list of calo to treat" };

  /** @brief Tool for shower shape calculation*/
  ToolHandle<IegammaShowerShape> m_ShowerShapeTool{
    this,
    "ShowerShapeTool",
    "egammaShowerShape/egammashowershape",
    "Handle of instance of egammaShowerShape Tool to be run"
  };

  /** @brief Tool for hadronic leakage calculation*/
  ToolHandle<IegammaIso> m_HadronicLeakageTool{
    this,
    "HadronicLeakageTool",
    "egammaIso",
    "Handle of the EMCaloIsolationTool for Hadronic leakage"
  };

  /** @brief boolean to print results*/
  Gaudi::Property<bool> m_Print{ this,
                                 "Print",
                                 false,
                                 "in case of extra prints" };

  /** @brief Boolean to call shower shape calculation and filling
      (NB: this could be important when redoing calculation from AODs) */
  Gaudi::Property<bool> m_UseShowerShapeTool{
    this,
    "UseShowerShapeTool",
    true,
    "Boolean to call shower shape calculation and filling"
  };

  /** @brief Boolean to call calo isolation variables calculation and filling
      (NB: this could be important when redoing calculation from AODs) */
  Gaudi::Property<bool> m_UseCaloIsoTool{
    this,
    "UseCaloIsoTool",
    true,
    "Boolean to call hadronic leakage calculation and filling"
  };

  /** @brief boolean to know if we are looking at cosmic data */
  Gaudi::Property<bool> m_isCosmics{ this,
                                     "isCosmics",
                                     false,
                                     "Boolean for use of cosmics" };
};

#endif
