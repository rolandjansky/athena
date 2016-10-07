/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EGAMMAOQFLAGSBUILDER_H
#define EGAMMATOOLS_EGAMMAOQFLAGSBUILDER_H
/**
  @class egammaOQFlagsBuilder
  egamma Object Quality flags data object builder : 
    - This tool checks if any cell of the cluster associated to the egamma object is affected by a detector problem: non nominal or dead high voltage, readout problems, missing FEBs, high quality factor, timing, etc.... If this is the case, then a bit corresponding to a specific problem is filled ( see egammaEvent/egammaEvent/egammaPIDdefs.h for bits definition).
Most of the informations are given separately for each layer of the EM calorimeter. They are also separately stored for the core (3x3 central cells in the middle layer of the EM calorimeter) and the edge (all the other cells) of the cluster.
  @author Frederic Derue derue@lpnhe.in2p3.fr
  @author Francesco Polci polci@lpsc.in2p3.fr
  @author Quentin Buat quentin.buat@lpsc.in2p3.fr
*/

// INCLUDE HEADER FILES: 
#include "egammaBaseTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRecConditions/ILArBadChanTool.h"
#include "LArCabling/LArCablingService.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"
#include "CaloInterface/ICaloAffectedTool.h"
#include "CaloUtils/CaloCellList.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

class IToolSvc;
class CaloCellContainer;
class ILArBadChanTool;
class HWIdentifier;
class LArEM_ID;
class CaloCell_ID;

class egammaOQFlagsBuilder : public egammaBaseTool
{
 public:

  /** @brief Default constructor*/
  egammaOQFlagsBuilder(const std::string& type,
		       const std::string& name,
		       const IInterface* parent);
   
  /** @brief Destructor*/
  ~egammaOQFlagsBuilder();
  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief standard execute method */
  virtual StatusCode execute(xAOD::Egamma*);
  /** @brief finalize method*/
  StatusCode finalize();

 private:
  /** Handle to bad-channel tools */
  ToolHandle<ILArBadChanTool> m_badChannelTool;
  ToolHandle<LArCablingService> m_larCablingSvc;
  ToolHandle<ICaloAffectedTool> m_affectedTool;
  const LArEM_ID* m_emHelper;
  const CaloCell_ID* m_calocellId;
  const CaloAffectedRegionInfoVec* m_affRegVec;
  const CaloCellContainer* m_cellcoll;
  std::string m_cellsName;

  // IToolSvc* m_toolSvc;
  bool isCore(Identifier Id, const std::vector<IdentifierHash>& neighbourList) const;
  std::vector<IdentifierHash> findNeighbours( Identifier cellCentrId) const; 
  bool isbadtilecell (CaloCellList& ccl, float clusterEta, float clusterPhi, 
		      double sizeEta, double sizePhi, CaloSampling::CaloSample sample) const ;   

  Identifier m_cellCentrId;
  bool findCentralCell(const xAOD::CaloCluster* cluster);
  StoreGateSvc* m_detStore;
  double m_QCellCut;
  double m_QCellHECCut;
  double m_QCellSporCut;
  double m_LArQCut;
  double m_TCut;
  double m_TCutVsE;
  double m_RcellCut;
};

#endif









