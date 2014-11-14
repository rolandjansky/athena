// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TAUTRUTHTRACKMATCHINGTOOL_H
#define  TAUTRUTHTRACKMATCHINGTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Local include(s):
#include "TauAnalysisTools/ITauTruthTrackMatchingTool.h"


namespace TauAnalysisTools
{
enum TrackSpuriousType_t
{
  UnclassifiedTrack     = 0, // Track is unclassified
  ConversionTrack       = 1, // Track is from conversion
  PileupTrack           = 2, // Track is from pileup
  UnderlyingEventTrack  = 3, // Track is from underlying event
  FakeTrack             = 4  // Track is a fake
};


class TauTruthTrackMatchingTool
  : public TauAnalysisTools::ITauTruthTrackMatchingTool
  , public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauTruthTrackMatchingTool,
                  TauAnalysisTools::ITauTruthTrackMatchingTool )

public:

  TauTruthTrackMatchingTool( const std::string& name );

  virtual ~TauTruthTrackMatchingTool();

  virtual StatusCode initialize();

#ifndef __MAKECINT__
  // classify vector of tracks
  virtual StatusCode classifyTracks(std::vector<const xAOD::TrackParticle*>& vTracks);

  // classify a single track
  virtual StatusCode classifyTrack(const xAOD::TrackParticle& xTrackParticle);
#endif // not __MAKECINT__

private:

#ifndef __MAKECINT__
  void checkTrackSpuriousType(const xAOD::TrackParticle& xTrackParticle);
  void checkTrackIsTauInheritant(const xAOD::TrackParticle& xTrackParticle);
  bool checkTruthParent(const xAOD::TruthParticle& xTruthParticle, int& iDepth, std::string& sHistory);
  const xAOD::TruthParticle* getTruthParticle(const xAOD::TrackParticle& xTrackParticle);
#endif // not __MAKECINT__

}; // class TauTruthTrackMatchingTool

}

#ifdef DEVEL_TAUTRUTHTRACKMATCHINGTOOL

#include <TLorentzVector.h>
#include <vector>

using std::vector;

/** Forward declarations */

class Track;
class TruthParticle;
class Tau;

/*
* class D3PD_BLOCK

Interface for reading in branches from ATLAS D3PDs.
The branches are simply passed by the user. Retrieval
of the branches from the Tree and hence their ownership
is left to the user.

We have chosen NOT to pass mc_n, since this can be
problematic in PyROOT. Rather the mc_ block size is
retrived from mc_pdgId->size()

- Copied from:
  svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauSpinnerTool/

*/

class D3PD_BLOCK
{
public:

  /* mc_ branches for truth */
  vector<float> *mc_pt;
  vector<float> *mc_eta;
  vector<float> *mc_phi;
  vector<float> *mc_m;
  vector<float> *mc_charge;
  vector<int>   *mc_pdgId;
  vector<int>   *mc_status;
  vector<int>   *mc_barcode;
  vector<int>   *mc_vx_barcode;
  vector<vector<int> > *mc_child_index;
  vector<vector<int> > *mc_parent_index;

  /* trk_ branches */
  vector<float>   *trk_pt;
  vector<float>   *trk_eta;
  vector<float>   *trk_phi;
  vector<float>   *trk_mc_probability;
  vector<int>     *trk_mc_barcode;

  D3PD_BLOCK():
    mc_pt(NULL),
    mc_eta(NULL),
    mc_phi(NULL),
    mc_m(NULL),
    mc_charge(NULL),
    mc_pdgId(NULL),
    mc_status(NULL),
    mc_barcode(NULL),
    mc_vx_barcode(NULL),
    mc_child_index(NULL),
    mc_parent_index(NULL),
    trk_pt(NULL),
    trk_eta(NULL),
    trk_phi(NULL),
    trk_mc_probability(NULL),
    trk_mc_barcode(NULL)
  {}

  void set_addresses(
    vector<float> *_mc_pt,
    vector<float> *_mc_eta,
    vector<float> *_mc_phi,
    vector<float> *_mc_m,
    vector<float> *_mc_charge,
    vector<int>	*_mc_pdgId,
    vector<int>	*_mc_status,
    vector<int>	*_mc_barcode,
    vector<int>	*_mc_vx_barcode,
    vector<vector<int> > *_mc_child_index,
    vector<vector<int> > *_mc_parent_index,
    vector<float>   *_trk_pt,
    vector<float>   *_trk_eta,
    vector<float>   *_trk_phi,
    vector<float>   *_trk_mc_probability,
    vector<int>	  *_trk_mc_barcode)
  {
    mc_pt = _mc_pt;
    mc_eta = _mc_eta;
    mc_phi = _mc_phi;
    mc_m = _mc_m;
    mc_charge = _mc_charge;
    mc_pdgId = _mc_pdgId;
    mc_status = _mc_status;
    mc_barcode = _mc_barcode;
    mc_vx_barcode = _mc_vx_barcode;
    mc_child_index = _mc_child_index;
    mc_parent_index = _mc_parent_index;
    trk_pt = _trk_pt;
    trk_eta = _trk_eta;
    trk_phi = _trk_phi;
    trk_mc_probability = _trk_mc_probability;
    trk_mc_barcode = _trk_mc_barcode;
  }
};

/** class TruthTrackMatchingTool
This is where the magic happens!
*/
class TruthTrackMatchingTool
{

public:
  /** constructor */
  TruthTrackMatchingTool():
    m_myTauTrack(NULL),
    m_myTrack(NULL),
    m_myTau(NULL),
    m_use_delta_r_matching(false),
    m_delta_r_cut(999.)
  {};
  /** destructor */
  ~TruthTrackMatchingTool();

  enum TauMatchStatus_t
  {
    kTAUMATCHSTATUS_UNCL  	      = 0, // Default value
    kTAUMATCHSTATUS_TRUTH_MATCHED       = 1, // Tau is truth matched
    kTAUMATCHSTATUS_TRUTH_UNMATCHED     = 2, // Tau is not truth matched
  };
  enum TauType_t
  {
    kTAUTYPE_UNCL      = 0, // Default value
    kTAUTYPE_1PRONG    = 1, // Tau is truth-matched to 1 prong
    kTAUTYPE_3PRONG    = 3, // Tau is truth-matched to 3 prong
    kTAUTYPE_GT3PRONG  = 5, // Tau is truth-matched to > 3 prong
  };

  enum TrackMatchStatus_t
  {
    kTRACKMATCHSTATUS_UNCL	     = 0, // Track is unclassified
    kTRACKMATCHSTATUS_TRUTH_MATCHED    = 1, // Track is truth matched
    kTRACKMATCHSTATUS_TRUTH_UNMATCHED  = 2, // Track is spurious
    kTRACKMATCHSTATUS_MULTIMATCHED     = 3, // Track is truth matched to more than 1 particle
  };
  enum TrackSpuriousType_t
  {
    kTRACKSPURIOUSTYPE_UNCL	  = 0, // Track is unclassified
    kTRACKSPURIOUSTYPE_CONVERSION   = 1, // Track is from conversion
    kTRACKSPURIOUSTYPE_PILEUP	  = 2, // Track is from pileup
    kTRACKSPURIOUSTYPE_UE 	  = 3, // Track is from underlying event
    kTRACKSPURIOUSTYPE_FAKE	  = 4, // Track is a fake
    kTRACKNSPURIOUS_TYPES
  };

  D3PD_BLOCK m_myBlock;

  /* this method is called to set the D3PD branches used by the tool */
  void Initialize(vector<float> *,
                  vector<float> *,
                  vector<float> *,
                  vector<float> *,
                  vector<float> *,
                  vector<int> *,
                  vector<int> *,
                  vector<int> *,
                  vector<int> *,
                  vector<vector<int> > *,
                  vector<vector<int> > *,
                  vector<float> *,
                  vector<float> *,
                  vector<float> *,
                  vector<float> *,
                  vector<int> *);

  /* this runs the algorithm, by calling the private methods below */
  void Classify(TLorentzVector &);
  void Classify(TLorentzVector &, vector<TLorentzVector> &);

  void UseDeltaRMatching(bool , float );

  TauMatchStatus_t    getTauMatchStatus();
  TauType_t 	  getTauType();
  TrackMatchStatus_t  getTrackMatchStatus();
  TrackSpuriousType_t getTrackSpuriousType();
  int getMCParticleMatchIndex();
  float getDeltaRTruthPartner();
  void PrintOutput();

  static bool isGoodDaughter(int , int , int );
  static unsigned int nTracksType(TrackSpuriousType_t );
  static unsigned int s_counters_tracktype[kTRACKNSPURIOUS_TYPES];

private:
  /** private methods */

  void ClearTool();

  Track* MakeTauTrack(TLorentzVector &);			   /* returns a Track object for the input tau track  */
  Track* MakeTrk(Track *);  				   /* returns the Track object corresponding to input tau track, built from trk_ container (to have
        							   barcode & probability info) */

  void TruthTrackMatching(Track *); 			   /* finds barcode-matching truth track to input track */
  void TruthTrackMatching(vector<TruthParticle*> &, Track *);

  Tau* MakeTau(TLorentzVector &, vector<TLorentzVector> &); /* calls MakeTauTrack for each tau track, and fills vector<Track*> m_tautracks */

  void TruthTauMatching(Tau *);				/* Finds truth tau  corresponding to input tau, in order to fill vector<TruthParticle*>
        							   m_truthtracks for input tau */
  void TauTrkMatching(Tau *);				/* calls MakeTrk for each tau track of input tau, and fills vector<Track*> m_tracks for input tau */
  void TauTruthTrackMatching(Tau *);			/* calls TruthTrackMatching for each track of input tau's m_tracks */

  void FindChargedDaughters (TruthParticle *, vector<int> &);

  void SingleTrackOutput(Track *);

  /** private members */
  Track* m_myTauTrack;
  Track* m_myTrack;
  Tau*   m_myTau;

  bool   m_use_delta_r_matching;
  float  m_delta_r_cut;
};

/* **************** */
/*  object classes  */
/* **************** */

class Track : public TLorentzVector
{

public:

  Track() :
    m_truthpartner(NULL),
    m_track_match_status(TruthTrackMatchingTool::kTRACKMATCHSTATUS_UNCL),
    m_track_spurious_type(TruthTrackMatchingTool::kTRACKSPURIOUSTYPE_UNCL),
    m_barcode(-999),
    m_probability(-1.),
    m_tau_track_index(-1),
    m_truth_track_index(-1),
    m_charge(-999.),
    m_is_truth_matched(false),
    m_n_matching_truth(-1)
  {};
  Track (const Track& );
  ~Track();

  void SetBarcode(int barcode)
  {
    m_barcode = barcode;
  }
  void SetProbability(float probability)
  {
    m_probability = probability;
  }
  void SetTauTrackIndex(int match_index)
  {
    m_tau_track_index = match_index; /* sets the index of track to corresponding position in tau_track vector */
  }
  void SetTruthTrackIndex(int match_index)
  {
    m_truth_track_index = match_index; /* sets the index of corresponding mc_ truth trcak to this track */
  }
  void SetCharge(float charge)
  {
    m_charge = charge;
  }
  void SetIsTruthMatched(bool status)
  {
    m_is_truth_matched = status;
  }
  void SetHowManyMatchingTruthTracks(int n)
  {
    m_n_matching_truth = n;
  }
  void SetTrackMatchingStatus();

  int   Barcode()
  {
    return m_barcode;
  }
  float Probability()
  {
    return m_probability;
  }
  int   TauTrackIndex()
  {
    return m_tau_track_index;
  }
  int   TruthTrackIndex()
  {
    return m_truth_track_index;
  }
  float Charge()
  {
    return m_charge;
  }
  bool  isTruthMatched()
  {
    return m_is_truth_matched;
  }
  int   HowManyMatchingTruthTracks()
  {
    return m_n_matching_truth;
  }

  float DeltaRTruthPartner();

  void SetTrackMatchStatus(TruthTrackMatchingTool::TrackMatchStatus_t   status)
  {
    m_track_match_status = status;
  }
  void SetTrackSpuriousType(TruthTrackMatchingTool::TrackSpuriousType_t type)
  {
    m_track_spurious_type = type;
  }
  TruthTrackMatchingTool::TrackMatchStatus_t   WhichTrackMatchStatus()
  {
    return m_track_match_status;
  }
  TruthTrackMatchingTool::TrackSpuriousType_t  WhichSpuriousType()
  {
    return m_track_spurious_type;
  }

  /** public members */
  TruthParticle*          m_truthpartner;

private:
  /** private members */
  TruthTrackMatchingTool::TrackMatchStatus_t  m_track_match_status;
  TruthTrackMatchingTool::TrackSpuriousType_t m_track_spurious_type;
  int   m_barcode;
  float m_probability;
  int   m_tau_track_index;
  int   m_truth_track_index;
  float m_charge;
  bool  m_is_truth_matched;
  int   m_n_matching_truth; /* number of matching truth particles to THIS track */

};

class TruthParticle : public TLorentzVector
{

public:

  TruthParticle() :
    m_status(-999),
    m_barcode(-999),
    m_vertex_barcode(999),
    m_daughter_index(-1),
    m_charge(-999),
    m_pdgid(0)
  {};
  TruthParticle (const TruthParticle& );
  ~TruthParticle();

  void SetStatus(int status)
  {
    m_status = status;
  }
  void SetBarcode(int barcode)
  {
    m_barcode = barcode;
  }
  void SetVertexBarcode(int vertex_barcode)
  {
    m_vertex_barcode = vertex_barcode;
  }
  void SetDaughterIndex(int daughter_index)
  {
    m_daughter_index = daughter_index;
  }
  void SetCharge(float charge)
  {
    m_charge = charge;
  }
  void SetPdgId(int pdgid)
  {
    m_pdgid = pdgid;
  }

  int Status()
  {
    return m_status;
  }
  int Barcode()
  {
    return m_barcode;
  }
  int VertexBarcode()
  {
    return m_vertex_barcode;
  }
  int DaughterIndex()
  {
    return m_daughter_index;
  }
  float Charge()
  {
    return m_charge;
  }
  int PdgId()
  {
    return m_pdgid;
  }

  /*       void FindChargedDaughters (vector<int> &); */
  int nChargedDaughters()
  {
    return m_charged_daughters.size();
  }

  /** public members */
  vector<TruthParticle*> m_charged_daughters;

private:
  /** private members */
  int m_status;
  int m_barcode;
  int m_vertex_barcode;
  int m_daughter_index;
  float m_charge;
  int m_pdgid;
};


class Tau : public TLorentzVector
{

public:

  Tau() :
    m_truthpartner(NULL),
    m_tau_match_status(TruthTrackMatchingTool::kTAUMATCHSTATUS_TRUTH_UNMATCHED),
    m_tau_type(TruthTrackMatchingTool::kTAUTYPE_UNCL),
    m_charge(-999.),
    m_tau_index(-1),
    m_matched_truth_tau_index(-1),
    m_is_truth_matched(false)
  {};
  Tau (const Tau& );
  ~Tau();

  void  SetCharge(float charge)
  {
    m_charge=charge;
  }
  void  SetTauIndex(int index)
  {
    m_tau_index = index;
  }
  void  SetMatchedTruthTauIndex (int index)
  {
    m_matched_truth_tau_index = index;
  }
  void  SetIsTruthMatched(bool status)
  {
    m_is_truth_matched = status;
  }

  float Charge() const
  {
    return m_charge;
  }
  int   TauIndex()
  {
    return m_tau_index;
  }
  int   MatchedTruthTauIndex()
  {
    return m_matched_truth_tau_index;
  }
  bool  isTruthMatched()
  {
    return m_is_truth_matched;
  }

  void SetTauMatchStatus(TruthTrackMatchingTool::TauMatchStatus_t status)
  {
    m_tau_match_status = status;
  }
  void SetTauType(TruthTrackMatchingTool::TauType_t type)
  {
    m_tau_type = type;
  }
  TruthTrackMatchingTool::TauMatchStatus_t WhichTauMatchStatus()
  {
    return m_tau_match_status;
  }
  TruthTrackMatchingTool::TauType_t        WhichTauType()
  {
    return m_tau_type;
  }

  /** public members */
  vector<Track*>          m_tautracks, m_tracks;
  vector<TruthParticle*>  m_truthtracks;
  TruthParticle*          m_truthpartner;

private:
  /** private members */
  TruthTrackMatchingTool::TauMatchStatus_t m_tau_match_status;
  TruthTrackMatchingTool::TauType_t        m_tau_type;
  float m_charge;
  int   m_tau_index;
  int   m_matched_truth_tau_index;
  bool  m_is_truth_matched;
};


#endif //DEVEL_TAUTRUTHTRACKMATCHINGTOOL
#endif // TAUTRUTHTRACKMATCHINGTOOL_H

