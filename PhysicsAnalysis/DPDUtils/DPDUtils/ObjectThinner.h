/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DPDUTILS_OBJECTTHINNER_H
#define DPDUTILS_OBJECTTHINNER_H
/*****************************************************************************
Name    : ObjectThinner.h
Package : offline/PhysicsAnalysis/DPDUtils
Author  : Ketevi A. Assamagan
Created : Septeomer 2009

DESCRIPTION:

Save TrackParticles and CaloClusters associated to Muon/Egamma/TauJet into the DPD

*****************************************************************************/

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ObjectVector.h"

#include "UserAnalysisUtils/UserAnalysisSelectionTool.h"

#include <vector>
#include <string>
#include <map>

class IThinningSvc;

class ObjectThinner : public AthAlgorithm {
 public:
  ObjectThinner(const std::string& name, ISvcLocator* pSvcLocator);
  ~ObjectThinner();
  
  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
  
 private:

  /** to thin the photon container */
  StatusCode photonContainerThinning ( std::vector<bool>& selectedInDetTracks,
                                       std::vector<bool>& selectedegClusters );

  /** to thin the electron container */
  StatusCode electronContainerThinning ( std::vector<bool>& selectedInDetTracks,
                                         std::vector<bool>& selectedegClusters,
                                         unsigned int& selectedElectrons );  

  /** to thin the muon container */
  StatusCode muonContainerThinning ( std::vector<bool>& selectedInDetTracks,
                                     unsigned int& selectedMuons );

  /** to thin the taujet container */
  StatusCode taujetContainerThinning ( std::vector<bool>& selectedInDetTracks );

  /** to thin the jet collection 
      here you may decide to save the TrackParticles associalted to jet and Caloclusters associated to jet */
  StatusCode jetContainerThinning ();

 private:
  
  /** The thinning service */
  typedef ServiceHandle<IThinningSvc> IThinningSvc_t;
  IThinningSvc_t m_thinningSvc;

  /** object selection tool */
  ToolHandle<UserAnalysisSelectionTool>       m_analysisSelectionTool;

  /** Inner Detector TrackParticleContainer name */
  std::string m_inDetTrackParticleContainerName;

  /** do the skimming of events */
  bool m_doSkimming;
  unsigned int m_numberOfPreSelectedMuons; 
  unsigned int m_numberOfPreSelectedElectrons;

  /** container names in to thin,
      photon, electorn, muon, taujet and jet in that order */
  std::vector<std::string> m_containerNames;
 
  /** what to thin - 0 means do not do any thinning
     and 1 means do the thinning. Again, in the order
     of photon, electron, muon, taujet and jet */
  std::vector<bool> m_doThinning;

  /** CaloCluster Container associated to photon, electron, muon and taujet
      in that order - the softe cluster container should also be in this list 
      the order should be [photon, electron, seofte, muon, taujet, jet] */
  std::vector<std::string> m_clusterContainerNames;

  /** the Muon Spectrometer TrackParticle containers associated to muons
      not the Inner Detector one - the order does not matter*/
  std::vector<std::string> m_muontpContainerNames;

};

#endif // DPDUTILS_OBJECTTHINNER
