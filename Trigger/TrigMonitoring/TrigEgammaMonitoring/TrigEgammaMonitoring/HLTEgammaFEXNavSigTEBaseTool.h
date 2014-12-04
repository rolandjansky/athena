/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTEGAMMAFEXNAVSIGTEBASETOOL_H
#define HLTEGAMMAFEXNAVSIGTEBASETOOL_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "xAODTrigger/EmTauRoI.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"

#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMClusterContainer.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"
#include "egammaEvent/EMShower.h"

#include "TH1F.h"

#include "TrigHLTMonitoring/IHLTMonTool.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/Electron.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/Egamma.h"

class TrigInDetTrackCollection;
class TrigInDetTrack;

namespace Rec { class TrackParticle; }
namespace Reco { class ITrackToVertex; }

using std::vector;
using std::string;


/**
 *  class HLTEgammaFEXNavSigTEBaseTool
 *  
 *  - defines concrete histgram booking and filling functions
 *  - inherits from IHLTMonTool and uses its histogram manager (MonGroups)
 *    
 *    in there histogram groups
 *
 *  - higher level filling (object level) is performed by HLTEgammaNavSigTEMonTool which derives from this
 *  - object fillers do not apply cuts, they merely check that all information is present
 *  -- cuts must be applied elsewhere
 *  
 *
 *
 */
class HLTEgammaFEXNavSigTEBaseTool : public IHLTMonTool {

  public:

    ///Constructor
    HLTEgammaFEXNavSigTEBaseTool(const string & type, const string & name, const IInterface* parent);
    
    ///Destructor
    ~HLTEgammaFEXNavSigTEBaseTool();

  protected:

    //booking functions
    
    /**
     * Main per-signature histogram booking function called from book().
     * - only calls specialised per-signature booking functions
     */
    StatusCode book_per_signature(const string signature);

    /**
     * Booking histograms for a given signature for selection variables
     */
    StatusCode book_per_signature_sel(const string signature);   

    /**
     * Booking histograms for a given signature for resolution variables
     */
    StatusCode book_per_signature_res(const string signature);  

    /**
     * Booking histograms for a given signature for rate variables
     **/ 
    //StatusCode book_per_signature_rate(const string signature, string SubPath); 

    /**
     * Booking histograms for a given signature for rejection efficiency
     *
     * - will contain distribution ratios of Et, Eta, Phi between some given stage
     *   (e.g. L2ID) and the previous one (e.g. L2Calo)
     * - note: output histograms need to be booked to a MonGroup with special merge algorithm
     *         see comment for efficiency booking function
     */
    StatusCode book_per_signature_reject(const string signature); 

    /**
     * Booking histograms for a given signature for efficiency wrt Offline Electrons and Photons
     *
     * - note: it is important that a special merge algorithm is assigned to the MonGroups
     *         corresponding to the output histograms. the one chosen also requires that efficiencies
     *         are calculated in [%].
     */  
    //TODO add link to available algorithm implementations 
    StatusCode book_per_signature_eff(const string signature); 
    
    /**
     * Booking histograms for a given signature for luminosity and rate information
     * - e.g. number of objects passing a given TE divided by luminosity in a given LB
     */   
    StatusCode book_per_signature_lumi(const string signature); 

    /**
     * Booking histograms for Offline Electrons and Photons
     *
     * - this is used both for all offline egamma objects in the event, as well as
     *   objects matched to a given trigger object
     * - supplemented by wrapper for offline-match booking that has the same argument
     *   as the other per-signature booking functions
     */  
    StatusCode book_offline(bool isPhoton, string signature = "");   
 
    //Function for booking the offline matches
    StatusCode book_match(bool isPhtoton, string signature = "");

    ///Wrapper for offline-match histogram booking
    // - note: apparently the default of the "wrapee" isn't propagated
    //         to the child class, so we need to use this
    inline StatusCode book_offline(string signature){
      return(book_match(isPhotonSignature(signature), signature));
    }

    /**
     * Booking histograms for Luminosity information, e.g luminosity per LB
     */  
    StatusCode book_lumi(void);
    
    
    //processing

    /**
     * Post-processing per signature
     *
     *  - build offline- and rejection efficiency histograms
     */
    StatusCode proc_per_signature(const string signature);



    //object fillers
    
    /**
     * Fills histograms corresponding to L1 quantities
     */
    void fillL1(const EmTau_ROI &itEMTau, const string &grp);
    void fillL1(const xAOD::EmTauRoI &itEMTau, const string &grp);
    
    /**
     * Fills resolution L1<->Offline
     * - ratios L1/offline for et, eta, and phi 
     */
    void fillL1OffRes(const EmTau_ROI &itEMTau, const egamma* matchedEgamma, const string &grp);
    void fillL1OffRes(const xAOD::EmTauRoI &itEMTau, const xAOD::Egamma* matchedEgamma, const string &grp);
    
    /**
     * Fills L2 calo observables
     */
    void fillL2Calo(const TrigEMCluster *matchedL2, const string &grp);  
    void fillL2Calo(const xAOD::TrigEMCluster *matchedL2, const string &grp);  
    
    
    /**
     *  Fills resolution L2Calo<-> Offline
     */
    void fillL2CaloOffRes(const TrigEMCluster *matchedL2, const egamma* matchedEgamma, const string &grp);
    void fillL2CaloOffRes(const xAOD::TrigEMCluster *matchedL2, const xAOD::Egamma* matchedEgamma, const string &grp);
   
    /**
     * Fills L2 track observable histograms
     */
    void fillL2ID(const TrigInDetTrack* track,  const TrigEMCluster* cluster, const string &grp);
    void fillL2ID(const xAOD::TrackParticle* track,  const xAOD::TrigEMCluster* cluster, const string &grp);
    
    /**
     * Fills L2 track resolution histograms
     */
    void fillL2IDOffRes(const TrigInDetTrack* track, const egamma* matchedEgamma, const string &grp);
    void fillL2IDOffRes(const xAOD::TrackParticle* track, const xAOD::Electron* matchedEgamma, const string &grp);

    /**
     * Fills EF calo observables
     */
    void fillEFCalo(const CaloCluster* cluster, const TrigEMCluster* l2Cluster, const string &grp);
    void fillEFCalo(const xAOD::CaloCluster* cluster, const xAOD::TrigEMCluster* l2Cluster, const string &grp);

    /**
     * Fills EF calo shower observables
     */
    void fillEFCaloShower(const egamma* EFeg, const string &grp);
    void fillEFCaloShower(const xAOD::Egamma* EFeg, const string &grp);

    /**
     * Fills EF Calo resolution histograms
     */
    void fillEFCaloOffRes(const CaloCluster* cluster, const egamma* offline, const string &grp);
    void fillEFCaloOffRes(const xAOD::CaloCluster* cluster, const xAOD::Egamma* offline, const string &grp);
   
    /**
     * Fills event filter/offline track information
     */
    void fillEFID(const egamma *matchedEFTrk, const string &grp);
    void fillEFID(const xAOD::Electron *matchedEFTrk, const string &grp);
    
    /**
     * Fills EF track resolutions
     */
    void fillEFIDOffRes(const egamma *matchedEFTrk, const egamma *matchedEgamma, const string &grp);
    void fillEFIDOffRes(const xAOD::Electron *matchedEFTrk, const xAOD::Electron *matchedEgamma, const string &grp);

    /**
     * Fills event filter/offline egamma information
     */
    void fillEFeg(const egamma* matchedEFeg, const string &grp, const double pt_cut);
    void fillEFeg(const xAOD::Egamma* matchedEFeg, const string &grp, const double pt_cut);
    
    /**
     * Fills event match offline egamma information for efficiencies
     */
    void fillEFMatch(const egamma* matchedEFeg, const string &grp, const double pt_cut);
    void fillEFMatch(const xAOD::Egamma* matchedEFeg, const string &grp, const double pt_cut);

    /**
     * Fills EF egamma resolutions
     */
    void fillEFegOffRes(const egamma* matchedEFeg, const egamma* matchedEgamma, const string &grp);
    void fillEFegOffRes(const xAOD::Egamma* matchedEFeg, const xAOD::Egamma* matchedEgamma, const string &grp);
    
    /**
     *  Fills offline egamma histograms
     *  - calls event filter egamma filler (and track filler for electrons)
     */
    void fillOfflineEgamma(const egamma* eg, bool isPhoton, const string path, const double elec_cut, const double phot_cut);
    void fillOfflineEgamma(const xAOD::Electron* eg, bool isPhoton, const string path, const double elec_cut, const double phot_cut);
    
    /**
     *  Fills offline egamma histograms
     *  - calls event match efficiency egamma filler
     */
    void fillOfflineMatches(const egamma* eg, bool isPhoton, const string path, const double elec_cut, const double phot_cut);
    void fillOfflineMatches(const xAOD::Egamma* eg, bool isPhoton, const string path, const double elec_cut, const double phot_cut);
    

    /**
     * Fills rejection histograms
     * - input kinematic variables always correspond to an L1 ROI matched to a trigger object
     *   at a particular stage
     * - note: this is the only time actual object-variables are passed to a filler, for more
     *         robustness we could pass an EmTau_ROI instead.
     * TODO: rename?
     */
    void FillRates(float Pt, float Eta, float Phi, const string &level, const string &grp);
    

    //lumi fillers
    
    //TODO document and tidy
    void FillLumi(const double lumi, const string &grp);
    
    void FillLumiHist2(const long unsigned int lb,const double lumi, const string &grp);
    
    void FillLumiProfile(const long unsigned int lb, const double lumi, const string &grp);
    
    void FillRateLumi(const long unsigned int lb, const long unsigned int rate, const string &hname, const string &grp);
    
    void FillRateLumiHist2(const long unsigned int lb, const double lumi, const long unsigned int rate, const string &hname, const string &grp); 
    
    void FillRateLumiProfile(const long unsigned int lb, const double lumi, const long unsigned int rate, const string &hname, const string &grp);
    


    ///Print table of trigger counts
    void PrintTable();
   

    //helpers
    
    ///Decides whether signature is a photon trigger
    bool isPhotonSignature(string signature);


    //helpers for calculation of trigger variables

    ///Protected ratio calculation (catches division by zero and returns zero instead)
    inline float calcRatio(float num, float den){ return( den!=0 ? num/den : 0.); }

    ///Eta calculation from theta
    // - note: for some reason compiler claims Log() and Tan() are not part of TMath (use std instead)
    inline float calcEta(float theta){ return(-std::log(std::tan(theta/2.))); }

    ///Et calculation from E and Eta
    inline float calcEt(float e, float eta){ return(calcRatio(e, cosh(eta))); }

    ///Rstrips (=Eratio) from emaxs1 and e2tsts1
    inline float calcRStrips(float emaxs1, float e2tsts1){ return(calcRatio(emaxs1-e2tsts1, emaxs1+e2tsts1)); }

    ///DeltaPhi = |phi1 - phi2| (correcting for crossing the discontinuity at +/-pi)
    float calcDeltaPhi(float phi1, float phi2);

    ///DeltaEta = |eta1 - eta2|
    inline float calcDeltaEta(float eta1, float eta2){ return(fabsf(eta1 - eta2)); }

    ///DeltaR = sqrt(deltaEta^2 + deltaPhi^2) using safe delta phi
    inline float calcDeltaR(float eta1, float eta2, float phi1, float phi2){
      return(sqrt(pow(calcDeltaEta(eta1,eta2),2) + pow(calcDeltaPhi(phi1,phi2),2)));
    }



    //histogram manipulation helpers
    
    ///Scale histogram error
    // - note: doesn't seem to be used at present
    virtual void scaleError(float factor, TH1 *h);

    /**
     * Divide histograms assuming binomial errors
     * - if an additional flag is provided, we also scale the histogram with a factor of 100
     */
    void histDivide(TH1 *num, TH1 *den, TH1 *quo, bool scale = false);



    //physics helpers

    /**
     * Extrapolate TrigInDetTrack eta/phi to calorimeter
     * - wraps around TrigInDetTrackHelper to provide eta/phi at calorimeter
     * - directly returns a boolean indicating whether there were any problems
     * - eta/phi are written to member variables m_extrapolatedEta (Phi) to be used after checking the boolean
     */
    bool extrapolateTrackToCalo(const TrigInDetTrack* trk);

    //toolhandle for the track-to-vertex tool (needed for impact parameter)
    //ToolHandles are inherited from IHLTMonTool
    ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
    

    //path building helpers

  /**
   * Find combinations of strings
   * 
   * - fills a vector with all concatenations of strings supplied in the two input vectors
   * - will always put strings in first input vector on the left
   * - does not currently allow to place the output in one of the input vectors
   */
  void combineStrings(vector<string>& parts1, vector<string>& parts2, vector<string>& output);
  
  ///Prepend all strings in a vector with a string
  void combineStrings(string part1, vector<string>& parts2);

  ///Append string to all strings in a vector
  void combineStrings(vector<string>& parts1, string part2);

  ///Collect all pass-states to concider based on flags set in job options
  void fetchPassStates(vector<string>& output);

  ///Collect all stages to consider based on electron/photon boolean
  void fetchStages(bool isPhoton, vector<string>& output);
  


  /**********************
   *      MEMBERS
   **********************/


  IToolSvc* m_toolSvc; //!< Central tool service
  bool m_doExtrapol; //!< Switch for track->calo extrapolation

  //storegate keys
  string m_EMClusterKey;
  string m_egDetailKey;

  //members setting bits of path
  //TODO not employed in all places a path is set
  //     maybe insiting on there being no explicit path-words in the code
  //     is a bit over the top anyway
  //TODO find out whether not hard-coding as much of the path as possible
  //     reduces run-time performance (probably not. monitoring is huge anyway.)
  string m_histoBasePath;
  string m_activeChainPath;
  string m_activeTePath;
  string m_allTePath;

  string m_offElePath;
  string m_offPhoPath;
  string m_matchedOffElePath;
  string m_matchedOffPhoPath;
  string m_effElePath;
  string m_effPhoPath;

  string m_rejectionPath;
  string m_preRejectionPath;

  string m_lumiPath;

  //legacy booking members
  //TODO replace these by more graceful ones
  string m_BasicPath;
  string m_BasicPathNoCut;
  string m_BasicPathActiveTEs;
  string m_BasicPathActiveChain;

  //TODO check that at least one of these is true at init()
  bool m_doActiveChain;
  bool m_doActiveTe;
  bool m_doAllTe;
  bool m_skipActiveTe;

  //toggle lumi calculation
  bool m_doLumiCalc;
  
  //members for keeping track of lumi info
  int m_nLB;  
  int m_nLBUp;
  int m_nLBLo;
  int m_nLumi;
  double m_nLumiLo;
  double m_nLumiUp;
  
  //L2 cuts
  //TODO who uses them?
  float m_L2Rcore_cut;
  float m_L2Rstrip_cut;
  float m_L2Et_cut;
  float m_L2Ehad_cut;

  //L1 energies
  //TODO who uses them?
  float m_L1EmClus;
  float m_L1EmIsol;
  float m_L1HadCore;
  float m_L1HadIsol;

  //TODO hwat is this
  bool m_passThroughL1;
  
  //L2 tracking info
  //TODO who uses them?
  float m_L2IDPt;
  float m_L2IDEP;
  float m_L2IDDeta;
  float m_L2IDDphi;

  //lumi?
  float m_L;

  //members set during post processing, used by PrintTable()
  //TODO find out if anything else uses them and pass them one
  //     (perhaps in vectorised form) to print table directly
  //     if they are updated per-event, we'll have to keep them
  //     as member variables
  float m_nTotal;
  float m_nL1;
  float m_nL2Calo;
  float m_nL2ID;
  float m_nEFCalo;
  float m_nEFeg;
  float m_nEFID;

  float m_EFCaloEt;
  float m_EFCaloDeta;
  float m_EFCaloDphi;

  float m_EFTrackeCalibFactor;
  float m_EFTrackDeta;
  float m_EFTrackDphi;
  float m_EFTrackEtaCut;
  float m_EFTrackLowEoPHighEta;
  float m_EFTrackHighEoPHighEta;
  float m_EFTrackLowEoPLowEta;
  float m_EFTrackHighEoPLowEta;

  float m_lowEtCut;
  float m_maxEtaCut;
    
  ///Temporary storage of track eta extrapolated to calo (don't use without performing extrapolation first)
  float m_extrapolatedEta;
  ///Temporary storage of track phi extrapolated to calo (don't use without performing extrapolation first) 
  float m_extrapolatedPhi;
  

};

#endif
