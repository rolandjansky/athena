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

#include "egammaInterfaces/IEMShowerBuilder.h"
#include "egammaInterfaces/IegammaShowerShape.h"
#include "egammaInterfaces/IegammaIso.h"
#include "egammaInterfaces/IegammaIsoPtCorrection.h"
#include "egammaInterfaces/IegammaTopoIso.h"

#include "xAODEgamma/EgammaEnums.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODTracking/VertexContainer.h" 

#include "xAODEventShape/EventShape.h"

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
  virtual StatusCode caloExecute(xAOD::Egamma* eg, const CaloCellContainer* cellcoll);
  /** @brief method to calculate shower shapes from a CaloCellContainer */
  virtual StatusCode recoExecute(xAOD::Egamma* eg, const CaloCellContainer* cellcoll,const xAOD::TrackParticleContainer* aTrackParticleContainer = 0);
  /** @brief finalize method*/
  StatusCode finalize();

 private:
  /** @brief method to find parent electron/photon of cluster */
  const xAOD::Egamma* matchdRParent();
  /** @brief method to retrieve ShowerBuilder tool */
  StatusCode RetrieveShowerShapeTool();
  /** @brief method to retrieve Calo isolation tool */
  StatusCode RetrieveEMCaloIsolationTool();
  /** @brief method to retrieve Track isolation tool */
  StatusCode RetrieveEMTrackIsolationTool();
  /** @brief method to retrieve pt correction for Calo isolation tool */
  StatusCode RetrieveEMCaloIsoPtCorrectionTool();
  /** @brief method to retrieve Topo Calo isolation tool */
  StatusCode RetrieveEMTopoCaloIsolationTool();


  /** @brief method shared by the various execute method to retrieve the cell and cluster containers */
  StatusCode retrieveContainers();
  /** @brief calculate shower shapes*/
  void CalcShowerShape(xAOD::Egamma* eg);
  /** @brief method to calculate isolation from tracker */
  void CalcFromTracker(xAOD::Egamma* eg,const xAOD::TrackParticleContainer* aTrackParticleContainer = 0);
  /** @brief calculate cluster isolation variables */
  void CalcCaloIsolation(xAOD::Egamma* eg);
  /** @brief calculate pt corrected cluster isolation variables */
  void CalcCaloIsolationPtCorrection(xAOD::Egamma* eg);

  /** @brief fill shower detail from shower shape calculation*/
  void FillEMShowerShape(xAOD::Egamma* eg);

  /** @brief fill local variables with information from calorimeter isolation calculation with default/dummy values */
  void FillEMShowerIsoVarDefault();
  /** @brief retrieve information from calorimeter isolation calculation */
  void FillEMShowerIso(xAOD::Egamma* eg);

  /** @brief method to return the isoSpecifier given parameter */
  IegammaIso::IsoSpecifier getSpecifier(xAOD::EgammaParameters::IsolationType par) const;

  std::string m_topoCaloClusterInputName;

  /** @brief Cell container*/
  std::string m_cellsName;     
  /** @brief vector of calo-id to treat*/
  std::vector<int> m_caloNums ;     

  /** @brief Tool for shower shape calculation*/
  ToolHandle<IegammaShowerShape> m_ShowerShapeTool;
  /** @brief Tool for isolation calculation*/
  ToolHandle<IegammaIso> m_emCaloIsolationTool;
  /** @brief Pointer to the EMTrackIsolationTool*/
  ToolHandle<IEMTrackIsolationTool> m_emTrackIsolationTool;
  /** @brief Tool for pt-corrected isolation calculation*/
  ToolHandle<IegammaIsoPtCorrection> m_emCaloIsoPtCorrectionTool;
  /** @brief Tool for topo calo isolation calculation*/
  ToolHandle<IegammaTopoIso> m_emTopoCaloIsolationTool;

  /** @brief the CaloCell container */
  const CaloCellContainer* m_cellcoll;
  /** @brief the CaloCluster container */
  const xAOD::CaloCluster* m_clus;

  /** @brief the eventShape container */
  const xAOD::EventShape* m_evtShape;


  /** @brief boolean to print results*/
  bool m_Print;
  /** @brief Boolean to call shower shape calculation and filling
      (NB: this could be important when redoing calculation from AODs) */
  bool m_UseShowerShapeTool;
  /** @brief Boolean to call calo isolation variables calculation and filling
      (NB: this could be important when redoing calculation from AODs) */
  bool m_UseCaloIsoTool;
  /** @brief Boolean to call isolation variables calculation and filling */
  bool m_UseTrackIsoTool;
  /** @brief */
  bool m_caloSelection ;

  /** @brief Flag to do isolation for Topo Seeded Photons */
  bool m_doIsolForTopoSeeded;

  /** @brief boolean to know if we are looking at cosmic data */
  bool m_isCosmics;
  
  /** @brief The isolation cones to do; really vector of enum type egammaParameter::ParamDef */
  std::vector<int> m_isoTypes;

  /** @brief The isolation cones to do; really vector of enum type egammaParameter::ParamDef */
  std::vector<int> m_topoIsoTypes;

  // Those below are just variables not configurables 

  // /** @brief Turn on noise isolation calculation */
  bool m_doNoiseCalc;

  /** @brief the cone sizes for the isolation (used when no noise)*/
  std::vector<double> m_Rs;

  /** @brief the cone sizes for the isolation (used when at least one noised iso)*/
  std::vector<IegammaIso::IsoSpecifier> m_specs;

  /** @brief do topo isolation is it is requested */
  bool m_doTopoIso;

  /** @brief calculate topoetcone40_ptcorrected if topoetcone40 is in the topo isoType list */
  bool m_dotopoptcor;
  
 /** @brief Correct isolation variables based on energy density estimations */
 bool m_doEnergyDensityCorrection;


  // for timing
  bool m_timing;
  IChronoStatSvc* m_timingProfile;

};

#endif
