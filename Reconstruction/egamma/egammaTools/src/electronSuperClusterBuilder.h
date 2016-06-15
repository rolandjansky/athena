/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_ELECTRONSUPERCLUSTERBUILDER_H
#define EGAMMATOOLS_ELECTRONSUPERCLUSTERBUILDER_H

// INCLUDE HEADER FILES:
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "egammaInterfaces/IelectronSuperClusterBuilder.h"

//Fwd declarations
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "CaloUtils/CaloCellDetPos.h"

class CaloCellContainer;
class CaloCell;
class IEMExtrapolationTools;
class IegammaSwTool;
class IegammaMVATool;

class electronSuperClusterBuilder : public AthAlgTool,  virtual public IelectronSuperClusterBuilder {

 public:

  //Constructor/destructor.
  electronSuperClusterBuilder(const std::string& type,
			      const std::string& name,
			      const IInterface* parent);

  electronSuperClusterBuilder();
  ~electronSuperClusterBuilder();
  //Tool standard routines.
  StatusCode initialize();
  StatusCode finalize();
  //New StoreGate-based execute routine. Will use previously-established
  //track-cluster matches and make a supercluster egammaRec
  //container in StoreGate.
  StatusCode execute();

 private:
  //Superclustering routines.
  xAOD::CaloCluster* AddTopoClusters (const std::vector<const xAOD::CaloCluster*>&);

  void AddEMCellsToCluster(xAOD::CaloCluster* self,
			   const xAOD::CaloCluster* ref);

  //For  brem points in superclusters.
  StatusCode GetBremExtrapolations(const egammaRec*);

  bool OverlapsABremPoint(const xAOD::CaloCluster*);

  bool MatchSameTrack(const egammaRec *seed,
		      const egammaRec *sec) const;

  const std::vector<std::size_t> SearchForSecondaryClusters(const size_t i,
							    const EgammaRecContainer*,
							    std::vector<bool>& isUsed);
  bool  MatchesInWindow(const xAOD::CaloCluster *ref,
			const xAOD::CaloCluster *clus) const;

  StatusCode CalibrateCluster(xAOD::CaloCluster* newCluster,
			      const egammaRec* egRec);

  StatusCode fillPositionsInCalo(xAOD::CaloCluster* cluster);

  /////////////////////////////////////////////////////////////////////
  //internal variables
  std::vector<const CaloCell*> m_cellsin3x5;  
  std::vector<double> m_bpRetainedEnFrac;
  std::vector<double> m_bpExtrapEta; 
  std::vector<double> m_bpExtrapPhi;
  bool  m_useBremFinder;
  /** @brief Size of window in eta */
  int   m_delEtaCells;
  /** @brief Size of window in phi */
  int   m_delPhiCells;
  //Keep track of # of 3x5 and brem point
  //clusters added to seed clusters.
  int m_nWindowClusters, m_nExtraClusters,m_nBremPointClusters,m_nSameTrackClusters;
  float m_delEta; //!< half of window size, converted to units of eta
  float m_delPhi; //!< half of window size, converted to units of phi
  float m_EtThresholdCut;
  float m_secThresholdCut;
  float m_emFracCut;
  float m_trackOverlapDelEta;
  float m_trackOverlapDelPhi;
  float m_secondaryEmFracCut;
  float m_numberOfSiHits;
  bool  m_sumRemainingCellsInWindow;
  //std::string m_trackParticleContainerName;
  std::string m_inputEgammaRecContainerName;
  //std::string m_inputClusterContainerName;
  std::string m_electronSuperRecCollectionName;
  std::string m_outputElectronSuperClusters;

  /** @brief Tool for extrapolation */
  ToolHandle<IEMExtrapolationTools> m_extrapolationTool;

  bool m_correctClusters;  //!< Whether to run cluster correction
  bool m_calibrateClusters;  //!< Whether to run cluster calibration

  /** @breif Handle to the MVA calibration Tool **/
  ToolHandle<IegammaMVATool>  m_MVACalibTool;  
  /** @brief Tool to handle cluster corrections */
  ToolHandle<IegammaSwTool>   m_clusterCorrectionTool;
  /** @brief Position in Calo frame**/  
  CaloCellDetPos m_caloCellDetPos;

};

#endif
