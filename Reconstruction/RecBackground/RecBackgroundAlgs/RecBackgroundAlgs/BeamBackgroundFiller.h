/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamBackgroundFiller.h 693118 2015-09-04 07:27:46Z salekd $
#ifndef RECBACKGROUNDALGS_BEAMBACKGROUNDFILLER
#define RECBACKGROUNDALGS_BEAMBACKGROUNDFILLER

#include "RecBackgroundEvent/BeamBackgroundData.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODJet/JetContainer.h"
#include "AthLinks/ElementLinkVector.h"

#include <string>
#include <vector>

/**
 * @Short Implementation of the Beam Background Identification Method
 *
 *        This implementation defines the selection criteria for identifying beam background muons,
 *        and looks for them based on several methods.
 *        The result are stored in BeamBackgroundData.
 *
 * @author David Salek <David.Salek@cern.ch>
 *
 * $Revision: 693118 $
 * $Date: 2015-09-04 09:27:46 +0200 (Fri, 04 Sep 2015) $
 */
class BeamBackgroundFiller : public AthAlgorithm
{
public:
  BeamBackgroundFiller(const std::string& name, ISvcLocator* pSvcLocator);
  ~BeamBackgroundFiller();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  // Function matching calorimeter clusters with muon segments
  void FillMatchMatrix();

  // Beam background identification methods
  void SegmentMethod();
  void OneSidedMethod();
  void TwoSidedMethod();
  void ClusterShapeMethod();

  // Function calculating time for mboy CSC segments
  // (needed until release 17.1.0, does not work on AOD)
  double GetSegmentTime(const Muon::MuonSegment* pMuonSegment);

  // Function to identify fake jets
  void FindFakeJets();
  // Function to store the results in BeamBackgroundData
  void FillBeamBackgroundData();

  void PrintMatchMatrix() {};

  // containers
  std::string m_cscSegmentContainerKey;
  std::string m_mdtSegmentContainerKey;
  std::string m_caloClusterContainerKey;
  std::string m_jetContainerKey;
  std::string m_beamBackgroundDataKey;

  const xAOD::CaloClusterContainer* p_clusContainer;
  const Trk::SegmentCollection* p_cscSegmentContainer;
  const Trk::SegmentCollection* p_mdtSegmentContainer;
  const xAOD::JetContainer* p_jetContainer;


  // switch to turn on/off the time reconstruction for mboy CSC segments (needed until release 17.1.0)
  bool m_doMuonBoyCSCTiming;


  // cuts used in the Beam Background Identification Method
  double m_cutThetaCsc;
  double m_cutThetaMdtI;
  double m_cutPhiSeg;
  double m_cutPhiCsc;
  double m_cutPhiMdtI;
  double m_cutRadiusCsc;
  double m_cutRadiusMdtI;
  double m_cutEnergy;
  double m_cutRadiusLow;
  double m_cutRadiusHigh;
  double m_cutMuonTime;
  double m_cutClusTime;
  double m_cutTimeDiffAC;
  double m_cutDrdz;


  BeamBackgroundData* p_beamBackgroundData;  // stores the results


  int m_numSegment;         // number of segments with the direction parallel to the beam pipe
  int m_numSegmentEarly;    // number of early segments
  int m_numSegmentACNoTime; // number of matched pairs of segments on side A and side C
  int m_numSegmentAC;       // number of matched pairs of segments with corresponding time difference
  int m_numMatched;         // number of clusters matched with the segments
  int m_numNoTimeLoose;     // number of clusters identified by the "No-Time Method"
  int m_numNoTimeMedium;
  int m_numNoTimeTight;
  int m_numOneSidedLoose;   // number of clusters identified by the "One-Sided Method"
  int m_numOneSidedMedium;
  int m_numOneSidedTight;
  int m_numTwoSidedNoTime;  // number of clusters identified by the "Two-Sided No-Time Method"
  int m_numTwoSided;        // number of clusters identified by the "Two-Sided Method"
  int m_numClusterShape;    // number of clusters identified by the "Cluster-Shape Method"
  int m_numJet;             // number of fake jets
  int m_direction;  // direction of beam halo from the "Two-Sided Method" (positive for A->C, negative for C->A)

  ElementLinkVector<xAOD::CaloClusterContainer> m_indexClus;  // link to the calorimeter cluster
  ElementLinkVector<Trk::SegmentCollection> m_indexSeg;  // line to the muon segment
  std::vector<std::vector<int> > m_matchMatrix;  // matching matrix (stores results for each muon segment and calorimeter cluster)
  std::vector<int> m_resultSeg;  // summary of the results for each muon segment
  std::vector<int> m_resultClus;  // summary of the results for each cluster segment
  std::vector<float> m_drdzClus;  // shape of the cluster

  ElementLinkVector<xAOD::JetContainer> m_indexJet;  // link to the jet
  std::vector<int> m_resultJet;  // summary of the results for each jet


  // tools
  ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;
  ToolHandle<Muon::MuonIdHelperTool>  m_idHelperTool;
  ToolHandle<MuonCalib::IIdToFixedIdTool> m_idToFixedIdTool;
};

#endif
