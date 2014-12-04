/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTEGAMMANAVSIGTETOOL_H
#define HLTEGAMMANAVSIGTETOOL_H

//Base classes
#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigEgammaMonitoring/HLTEgammaFEXNavSigTEBaseTool.h"

//Core components
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

//L1 objects
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

//L2 calo object
#include "TrigCaloEvent/TrigEMCluster.h"

//EF and Offline calo object/container
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

//Offline egamma object/container
#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"

//Specialised offline egamma objects/containers (TODO are these still needed?)
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/Photon.h"

#include "xAODEgamma/ElectronContainer.h"

//ROOT and STL
#include <map>
#include <set>
#include "TH1F.h"

//Lumi
#include "GaudiKernel/ServiceHandle.h"
#include "LumiBlockComps/ILumiCalcSvc.h"
#include "LumiCalc/CoolQuery.h"

//for convenience (TODO find out whether this is allowed in ATLAS software)
using std::string;
using std::vector;
using std::set;

//forward declare track-type classes
class TrigInDetTrackCollection;
namespace Rec { class TrackParticle; }


/**
 * HLTEgammaNavSigTEMonTool
 *
 * - inherits from HLTEgammaFEXNavSigTEBaseTool, which inherits from IHLTMonTool
 * - implements the basic four virtual functions defined in the monitoring interface
 * - inherits all histogram-related capabilities from base tool and never fills them directly
 * -- the only exception are rejection histograms where basic L1 kinematics are filled directly
 * - performs trigger object loading/checking/matching and passes objects on to fillers defined in base
 * - main workhorse function is fillCombination(...) which goes through the full trigger chain from
 *   L1 to EF and calls the appropriate fillers
 * - can be run on PassedChains, ActiveTEs or AllTEs (or several of them)
 * -- passed chain: requires all trigger elements in a chain to have passed
 * -- active TE: considers all individual trigger elements which have passed, regardless of whether they belong
 *               to a full passed chain.
 * -- all TE: we do not care whether a trigger element has passed. everything is filled.
 * -- the corresponding histograms are booked to different folders in the output tree
 * -- internally the distinction is made using a trigger condition (Physics or alsoDeactivateTEs) and
 *    a boolean indicating whether we only look at activeTEs
 * - fillCombination is called by fill_per_signature, which loads the appropriate combinations
 *   for each requested pass-state.
 */
class HLTEgammaNavSigTEMonTool : public HLTEgammaFEXNavSigTEBaseTool {

  public:
  
  // Constructor
  HLTEgammaNavSigTEMonTool (const string & type, const string & name, const IInterface* parent);
  ~HLTEgammaNavSigTEMonTool();

  StatusCode init();
  
  /** In this case, the book drives the book_per_signature for each
	of the valid requested signatures */
  StatusCode book(); // called by bookHistograms
  
  /** In this case, the fill drives the fill_per_signature */
  StatusCode fill(); // called by fillHistograms
  
  /** Filling histograms per signature, i.e. e10_loose, g20_loose etc.*/
  StatusCode fill_per_signature(const string signature); // called by fillHistograms
  
  /** Filling histograms per signature for chains that are not necessarily Active, 
      i.e. chains that may have some active TEs but not all TEs are active all the way to the EF */ 
  StatusCode fill_per_signature_alsoDeactivateChains(const string signature, bool onlyActiveTE);  
  
  /** Filling histograms per signature for Active chains, 
      i.e. chains that have all TEs,all the way to the EF, active */ 
  StatusCode fill_per_signature_physics(const string signature);
  
  /**
   * Filling histograms from offline egamma container
   * - loops over container supplied
   * - decides which object it is dealing with by applying author cut
   * - then applies appropriate pt cut (set in job options)
   * - survivors are filled using offline filler derived from base class
   */
  void fillOfflineEgammas(const egammaContainer* egCont);
  void fillOfflineEgammas(const xAOD::ElectronContainer* egCont);
  

  
  /**
   * PostProcessing
   * - one of the functions we have to implement
   * - currently only used at end of run
   * - fetches lumi for all LBs processed and calls lumi fillers
   * - calls per-signature post-processing defined in baseclass
   */
  StatusCode proc(); // called by procHistograms

  
  private:

  /** Print detailed information on active chains */
  StatusCode print_info_chains(const string signature);

  /**
   * Loops over L2 and EF information and fills L2 and EF histograms
   *
   * TODO this needs to be thoroughly documented
   */
  StatusCode fillCombination(Trig::Combination comb, const string signature,
      unsigned int condition, bool onlyActiveTE) ;
  
  
  /**
   * Match eta/phi coordinates to offline egamma object
   * - decides which offline egamma container to match in using isPhoton boolean
   * - can use offline calo eta/phi or track directions (falls back to calo, if the track cannot be gotten)
   * - applies author and minimum pt cuts (the latter are set in joboptions)
   * - returns best match
   */
  const xAOD::Electron* matchOffline(const float eta, const float phi, bool useCalo, bool isPhoton);
  

  /**
   * Match eta/phi coordinates to L1 ROI
   * - uses the L1 kinematics vectors filled during initial loop through ROIs
   * - returns a boolean corresponding to match success
   * - member match variables are set to those of closest matching ROI, valid until next call
   */
  bool matchL1(float eta, float phi);

  //timestamps (updated during fill())
  unsigned int m_firstTimeStamp; //!< Timestamp of first event 
  unsigned int m_lastTimeStamp; //!< Timestamp of last event

  //basic event info
  unsigned long int m_runnbr;
  unsigned long int m_lumiblocknbr;
  unsigned long int m_eventnbr;

  //vector holding lumiblocks already processed (to avoid double counting?)
  vector<unsigned long int> m_lumiblocknbr_vect;

  //signatures to run the monitoring off
  vector<string> m_signatures;
  
  //trigger conditions
  //TODO think about removing them
  //     all conditions needed for filling are specified by pass-state
  //     rates use hardcoded TrigDef::Physics (or should)
  unsigned int m_fill_condition;
  unsigned int m_rate_condition;
  
  //maps vectors of counters identified by signature name
  //filled with 
  std::map< string, vector<unsigned int> > m_counters;
  std::map< string, vector<unsigned int> > m_counters_perLB;
  std::map< string, vector<unsigned int> > m_counters_perLB_activeTE;

  //switch passthrough chain
  bool m_usePreConditionalPassThroughChain;
  string m_passThroughChainEF;
  string m_passThroughChainL2;  
  
  //Offline Matching
  bool m_doOfflineComparisons; 
  string m_electronContainerName; 
  string m_photonContainerName; 
  xAOD::ElectronContainer const* m_elecTES;
  xAOD::ElectronContainer const* m_photTES;
  float m_offEle_minptcut;
  float m_offPho_minptcut;
  float m_dR_off;
  
  //
  float m_roiMatch_et; //!< Temporary storage of matched 
  float m_roiMatch_eta;
  float m_roiMatch_phi;
  vector<float> m_roi_et;
  vector<float> m_roi_eta;
  vector<float> m_roi_phi;
  

  //---Lumi
  string m_database; 
  string m_payload; 
  string m_coolfolder;
  string m_tagname; 
  cool::ChannelId m_chanid;
  string m_triggerForLumi;  
  
  //OVERLAPTEST
  // - vectors to hold names of chains that sent event to ES
  vector<string> tmp_chainsToEs;
  //OVERLAPTEST
};
#endif

