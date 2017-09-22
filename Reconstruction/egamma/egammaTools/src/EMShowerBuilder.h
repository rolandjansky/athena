/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EMSHOWERBUILDER_H
#define EGAMMATOOLS_EMSHOWERBUILDER_H
/**
  @class EMShowerBuilder
  EMShower data object builder : 
  - Get pointing information associated to the cluster from the egammaqpoint tool
  - Get shower depth associated to the cluster from the egammaqgcld tool
  - Define a Calo Cell list corresponding to EM, HAD, FCAL Calo
  - egammaIso calculates information concerning isolation behind em clusters 
  in the hadronic calorimeter and around the em cluster
  for isolation around em clustr use cone of different sizes
  - EMTrackIsolation calculates isolation based on tracking information
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
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgTool.h"
#include "egammaBaseTool.h"
#include "GaudiKernel/ToolHandle.h" 
#include "StoreGate/ReadHandleKey.h"

#include "egammaInterfaces/IEMShowerBuilder.h"
#include "egammaInterfaces/IegammaShowerShape.h"
#include "egammaInterfaces/IegammaIso.h"

#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODTracking/VertexContainer.h" 

class StoreGateSvc;
class CaloCellContainer;
class IEMTrackIsolationTool;
class IChronoStatSvc;

class EMShowerBuilder : public egammaBaseTool, virtual public IEMShowerBuilder
{
 public:

  /** @brief Default constructor*/
  EMShowerBuilder(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  /** @brief Destructor*/
  ~EMShowerBuilder();
	
  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief standard execute method */
  virtual StatusCode execute(xAOD::Egamma*);
  /** @brief method to calculate shower shapes from a CaloCellContainer */
  virtual StatusCode recoExecute(xAOD::Egamma* eg, const CaloCellContainer* cellcoll);
  /** @brief finalize method*/
  StatusCode finalize();

 private:
  /** @brief method to find parent electron/photon of cluster */
  const xAOD::Egamma* matchdRParent();
  /** @brief method to retrieve ShowerBuilder tool */
  StatusCode RetrieveShowerShapeTool();
  /** @brief method to retrieve hadronic leakage calculation from CaloIso tool */
  StatusCode RetrieveHadronicLeakageTool();
 

  /** @brief method shared by the various execute method to retrieve the cell and cluster containers */
  StatusCode retrieveContainers();
  /** @brief calculate shower shapes*/
  StatusCode CalcShowerShape(xAOD::Egamma* eg);
  StatusCode CalcHadronicLeakage(xAOD::Egamma* eg);
  
  /** @brief fill shower detail from shower shape calculation*/
  StatusCode FillEMShowerShape(xAOD::Egamma* eg) const ;

  /** @brief Cell container*/
  SG::ReadHandleKey<CaloCellContainer> m_cellsKey {this,
      "CellsName", "AllCalo", "Names of containers which contain cells"};
     
  /** @brief vector of calo-id to treat*/
  Gaudi::Property<std::vector<int> > m_caloNums {this,
      "CaloNums", {}, "list of calo to treat"};     

  /** @brief Tool for shower shape calculation*/
  ToolHandle<IegammaShowerShape> m_ShowerShapeTool {this,
      "ShowerShapeTool", "egammaShowerShape/egammashowershape", 
      "Handle of instance of egammaShowerShape Tool to be run"};

  /** @brief Tool for hadronic leakage calculation*/
  ToolHandle<IegammaIso> m_HadronicLeakageTool {this,
      "HadronicLeakageTool", "egammaIso", 
      "Handle of the EMCaloIsolationTool for Hadronic leakage"};

  /** @brief the CaloCell container */
  const CaloCellContainer* m_cellcoll;
  /** @brief the CaloCluster container */
  const xAOD::CaloCluster* m_clus;

  /** @brief boolean to print results*/
  Gaudi::Property<bool> m_Print {this,
      "Print", false, "in case of extra prints"};

  /** @brief Boolean to call shower shape calculation and filling
      (NB: this could be important when redoing calculation from AODs) */
  Gaudi::Property<bool> m_UseShowerShapeTool {this,
      "UseShowerShapeTool", true, 
      "Boolean to call shower shape calculation and filling"};

  /** @brief Boolean to call calo isolation variables calculation and filling
      (NB: this could be important when redoing calculation from AODs) */
  Gaudi::Property<bool> m_UseCaloIsoTool {this,
      "UseCaloIsoTool", true, 
      "Boolean to call hadronic leakage calculation and filling"};

  /** @brief */
  bool m_caloSelection ;

  /** @brief boolean to know if we are looking at cosmic data */
  Gaudi::Property<bool> m_isCosmics {this,
      "isCosmics", false, "Boolean for use of cosmics"};
  
   // for timing
  Gaudi::Property<bool> m_timing {this, 
      "Timing", false, "do extra timing"};

  IChronoStatSvc* m_timingProfile;

};

#endif
