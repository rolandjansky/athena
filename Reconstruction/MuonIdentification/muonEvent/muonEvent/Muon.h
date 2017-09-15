// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENT_MUON_H
#define MUONEVENT_MUON_H 1
/*****************************************************************************
Name    : Muon.h - Adapted from the previous CombinedMuon.h
Package : offline/Reconstruction/MuonIdentification/muonEvent
Author  : Ketevi A. Assamagan
*****************************************************************************/

#include "ParticleEvent/ParticleImpl.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "FourMom/Lib/P4ImplIPtCotThPhiM.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkSegment/SegmentCollection.h"
#include "muonEvent/MuonCaloEnergyContainer.h"
#include "muonEvent/MuonParamDefs.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"

#include "muonEvent/MuonQualityWord.h"

#include <inttypes.h>

namespace Analysis {
  typedef NavigableTerminalNode MuonNavigation;

  /** @class Analysis::Muon
      @brief The ATLAS Muon object - see doxygen, physics workbookd and
             the Muon Combined Performance WG's pages for full documentation

  */

class Muon : public ParticleImpl<
  ::Analysis::MuonNavigation,
  ::P4ImplIPtCotThPhiM
  >
{

  ///////////////////////////////////////////////////////////////////
  // Public typedefs:
  ///////////////////////////////////////////////////////////////////
 public:

  // for readability and lazy people
  typedef ParticleImpl< ::Analysis::MuonNavigation,
                        ::P4ImplIPtCotThPhiM
    > MuonImpl_t;
  typedef MuonImpl_t::navigable_type navigable_type;
  typedef MuonImpl_t::momentum_type  momentum_type;
  typedef MuonImpl_t::particle_type  particle_type;

 public:

  /** Default constructor */
  Muon( );

  /** Default constructor */
  Muon( MuonParameters::Author author );

  /** create the combined muon from 3 TrackParticles: InDet, Extrapolated and Combined TP,
      the combined TP defines the Fourmomentum, -
      isExtrapolatedToVertex indicates whether it is a Track Extrapolated to the perigee
      otherwise (if false) then it is a track extraplated from vertex out to the spectrometer */
  Muon( MuonParameters::Author author,
        const Rec::TrackParticle* inDetTP, const Rec::TrackParticle* extrapTP,
        const Rec::TrackParticle* combTP, const bool isExtrapolatedToVertex=true);


  /** if the combined failed, still create the combined muon from Inner Detector track
      and a MuonSegment. The trackparticle is associated as ID trackparticle and defines
      the Fourmomentum. */
  Muon( MuonParameters::Author author, const Rec::TrackParticle* inDetTP,
        const Trk::SegmentCollection* segmentContainer,
        const std::vector<const Trk::Segment*>& muonSegments);

  /** creates a muon for Standalone, ID-seeded (MuGirl) and Calo-tagged muons:
      for isExtrapolatedToVertex the trackparticle is interpreted as MS-standalone muon,
      for not isExtrapolatedToVertex the author is queried to distinguish MuGirl-refitted
      from calo tagged muons.
  */
  Muon(MuonParameters::Author author,
       const Rec::TrackParticle* extrapTP, const bool isExtrapolatedToVertex=false);

  /** Copy constructor */
  Muon( const Muon& rhs );

  /** Assignment operator */
  Muon& operator=( const Muon& rhs );

  /** Destructor */
  ~Muon();

  /** navigation method */
  virtual void fillToken( INavigationToken & thisToken ) const;
  /** navigation method */
  virtual void fillToken( INavigationToken & thisToken, const boost::any& ) const;

  /** return the primary author of this Muon - the algorithm that is most likely to
      reconstruct prompt muons with high purity */
  MuonParameters::Author author() const { return m_author; }

  /** get all the authors of this Muon
      for example during overlap checking, the same Muon may have been reconstructed by
      many different algorithms.
      the authors are 16-bit word, 1-bit reserved for each muon Algorithm
      the positions of the 1-bit are defined as follows:
      Muonboy | STACO | MuTag | Muid SA | Muid Co | MuGirl | MuGirlLowBeta | CaloMuon | CaloTag | CaloLikelihood | MuTagIMO | MuonCombinedRefit | ExtrapolateMuonToIP
       x         x        x      x          x        x          x              x          x            x             x               x           x
      each of these authors have 1 bit and the lower bits are Muonboy
      */
  uint16_t allAuthors() const { return m_allAuthors; }

  /** check if this author is among the list of authors of this Muon -
      obviously, the primary author MUST be in the list */
  bool isAuthor ( const unsigned int author ) const;

  /** check if this author is among the list of authors of this Muon -
        obviously, the primary author MUST be in the list */
  bool isAuthor ( const MuonParameters::Author author ) const;

  /** set the primary Author : the primary author is the algorithm that first created this Muon */
  void set_author (MuonParameters::Author author) { m_author = author;}

  /** set all the authors of this Muon
      the authors are 16-bit word, 1-bit reserved for each muon Algorithm
      the positions of the 1-bit are defined as follows:
      Muonboy | STACO | MuTag | Muid SA | Muid Co | MuGirl | MuGirlLowBeta | CaloMuon |
      CaloTag / CaloLikelihood | MuTagIMO | MuonCombinedRefit | ExtrapolateMuonToIP
      each of these authors have 1 bit and the lower bits are Muonboy
      */
  void set_allAuthors ( const uint16_t allAuthors ) { m_allAuthors = allAuthors; }

  /** the add_author method returns a bool:
      - if the author was already set, it will not reset it; it will return false
      - if the author does not exist in the author list; it will not all add it, instead it return false */
  bool add_author ( MuonParameters::Author author );

  /** other add_author method */
  void add_authors ( const uint16_t authors ) { m_allAuthors |= authors; }

  /** method for querying primary identification mechanism: muon identified
      by successfully combining a full ID and a full MS track */
  bool isCombinedMuon() const;

  /** method for querying primary identification mechanism: muon identified
      by spectrometer track (that is not associated to ID) as main author */
  bool isStandAloneMuon() const;

  /** Method for querying primary identification mechanism: muon identified
      by inner-detector seeded identification (segment tagging or ID+MS refit) */
  bool isSegmentTaggedMuon() const;

  /** Method for querying primary identification mechanism: muon identified
      solely by calorimeter based identification (so no signal in MS) */
  bool isCaloMuonId() const;

  /**  Method for querying primary identification mechanism: a combined 
  muon where a clean forward standalone muon has been associated to a ID 
  tracklet*/
  bool isSiliconAssociatedForwardMuon() const;

  /** is this muon also reconstructed by the lowPt algorithm? */
  bool alsoFoundByLowPt () const { return m_isAlsoFoundByLowPt; }

  /** is this muon also reconstructed by a CaloMuon ID algorithm? */
  bool alsoFoundByCaloMuonId () const { return m_isAlsoFoundByCaloMuonId; }

  /** is this Calo Muon also reconstructed by one of the standard muon reconstruction algorithm?
      Note that for calo muons, only Inner Detector and Calorimeter are used - the standard muon
      reconstruction used the muon spectrometer as well
      0 - not found by muon reconstruction
      1 - found by Muonboy/STACO/MuTag
      2 = found by MuID SA/MuID Combined/MuGirl
      3 - found by both 1 and 2 */
  unsigned short caloMuonAlsoFoundByMuonReco() const { return m_caloMuonAlsoFoundByMuonReco; }

  /** set is also seen by low Pt reconstruction */
  void set_isAlsoFoundByLowPt(const bool isAlsoSeenByLowPt) {
    (isAlsoSeenByLowPt) ? m_isAlsoFoundByLowPt=true : m_isAlsoFoundByLowPt=false;
  }

  /** set is also seen by calo muon Id reconstruction */
  void set_isAlsoFoundByCaloMuonId(const bool isAlsoSeenByCaloMuonId) {
    (isAlsoSeenByCaloMuonId) ? m_isAlsoFoundByCaloMuonId=true : m_isAlsoFoundByCaloMuonId=false; }

  /** set if this calo muon is also reconstructed by one of the standard muon recosntruction algorithms
      that require the muon spectrometer - not that for calo muons, Inner Detector and Calorimeter only
      are used
      0 - not found by muon reconstruction
      1 - found by Muonboy/STACO/MuTag
      2 = found by MuID SA/MuID Combined/MuGirl
      3 - found by both 1 and 2 */
  void set_caloMuonAlsoFoundByMuonReco(const unsigned short isAlsoFoundByMuonReco) {
    m_caloMuonAlsoFoundByMuonReco=isAlsoFoundByMuonReco;
  }

  /** return the primary track of this muon */
  const Rec::TrackParticle * track() const;

  /** get a parameter for this Muon - isolation energy in some cone for example */
  double parameter( MuonParameters::ParamDef x) const;
  /** vector of parameter pairs for this Muon - isolation energy in some cone for example */
  const std::vector< std::pair<MuonParameters::ParamDef,float> > & parameter() const { return m_parameters; }

  /** get the chi2 of the MS-ID track match at the perigee*/
  double matchChi2() const {return m_matchChi2;}

  /** get the chi2 over number of DoF of the track match at perigee */
  double matchChi2OverDoF() const {
     int dof = this->matchNumberDoF();
     if (dof > 0) return (this->matchChi2()/dof);
     else return 0;
  }

  /** get the number of DoF of the track match at perigee */
  int matchNumberDoF() const {return m_matchNumberDoF;}

  /** get the chi2 of the track fit */
  double fitChi2() const {
    const Rec::TrackParticle * theTrack = track();
    if ( theTrack ) return theTrack->fitQuality()->chiSquared();
    else return -1.0;
  }

  /** get the number of DoF of the track fit */
  int fitNumberDoF() const {
    const Rec::TrackParticle * theTrack = track();
    if ( theTrack ) return theTrack->fitQuality()->numberDoF();
    else return 0;
  }

  /** get the chi2 over number of DoF of the track fit */
  double fitChi2OverDoF() const {
    int dof = this->fitNumberDoF();
    if (dof > 0) return (fitChi2() / dof);
    else return 0.0;
  }

  /** returns track matching at MS entrance for combined muons and match at
      first segment for tagged muons. Pointer cached+owned by muon object. */
  const Trk::FitQuality* outerMatchChi2() const{
    if (m_outerMatchNumberDoF>0) return &m_transientOuterMatchChi2;
    else return NULL;
  }

  /** energy loss and its uncertainty in the calorimeter */
  std::pair<double,double> energyLoss() const;

  /** energy loss in the calorimeter - access to the full object */
  const CaloEnergy* caloEnergyLoss() const { return (m_caloEnergyLoss.isValid() ? *m_caloEnergyLoss : 0); }

  /** Is this combined Muon the best match?
      Given a combined track, this method returns true/false
      if this combined track is the best match to a muon spectrometer track.
      Indeed because of high track multiplicity in Inner Detector,
      there may be many combined tracks for one given muon
      spectrometer track. */
  bool bestMatch () const { return m_bestMatch; }

  /** is this Muon corrected or not in AOD-to-AOD correction */
  bool isCorrected () const { return m_isCorrected; }

  /** deprecated - please use primary identification, muon quality or MuonCP group
      guidelines to adopt a given muon quality definition */
//  bool isLowPtReconstructedMuon() const {
//    bool accept = false;
    /** this is the case of the DPD where the Track may have been dropped */
//    if ( m_hasInDetTrackParticle && !this->hasInDetTrackParticle() )
//       accept = this->isLowPt() && !this->bestMatch();
//    /** this is the case of the AOD */
//   else
//        accept = this->isLowPt() && this->hasInDetTrackParticle() && !this->bestMatch();
//    return accept;
//  }

  /** deprecated - please use hasCombinedMuonTrackParticle() instead */
  bool hasCombinedMuon() const                  {return hasCombinedMuonTrackParticle();}
  /** test if associated indet TrackParticle is available */
  bool hasInDetTrackParticle() const            {return m_inDetTrackParticle.isValid();}
  /** test if associated MS TrackParticle (extrapolated to IP) is available */
  bool hasMuonExtrapolatedTrackParticle() const {return m_muonExtrapolatedTrackParticle.isValid();}
  /** test if associated TrackParticle from ID-seeded full track fit is available */
  bool hasInnerExtrapolatedTrackParticle() const {return m_innerExtrapolatedTrackParticle.isValid();}
  /** test if associated combined-muon TrackParticle is available */
  bool hasCombinedMuonTrackParticle() const     {return m_combinedMuonTrackParticle.isValid();}
  /** test if (in addition to the refitted combined muon) a stat.combined TrackParticle is available.
      If no refit and combined muon only found by Staco, should be identica */
  bool hasStatCombinedMuonTrackParticle() const {return m_statCombinedMuonTrackParticle.isValid();}

  /** test for a valid pointer to a muon cluster */
  bool hasCluster() const { return m_cluster.isValid();}

  /** flag if the there was re-fit of ID and MS hits, currently muons from MuidCB, 
      CombinedMuonFit and MuGirl-refit */
  bool hasGlobalMuonTrackFit() const;

  /** deprecated - instead use primary identification, muon quality or MuonCP group
      guidelines to adopt a given muon quality definition */
//  bool isHighPt() const;

  /** deprecated - instead use primary identification, muon quality or MuonCP group
      guidelines to adopt a given muon quality definition */
 // bool isLowPt() const;

  /** Access to the TrackParticles */
  const Rec::TrackParticle* inDetTrackParticle() const {
    return ( (this->hasInDetTrackParticle()) ? *m_inDetTrackParticle : 0); }

  /** Access to the TrackParticles */
  const Rec::TrackParticle* muonSpectrometerTrackParticle() const {
    return ( (m_muonSpectrometerTrackParticle.isValid()) ? *m_muonSpectrometerTrackParticle : 0); }

  /** Access to the TrackParticles */
  const Rec::TrackParticle* muonExtrapolatedTrackParticle() const {
    return ( (this->hasMuonExtrapolatedTrackParticle()) ? *m_muonExtrapolatedTrackParticle : 0); }

  /** Access to the TrackParticles */
  const Rec::TrackParticle* innerExtrapolatedTrackParticle() const {
    return ( (this->hasInnerExtrapolatedTrackParticle()) ? *m_innerExtrapolatedTrackParticle : 0); }

  /** Access to the TrackParticles */
  const Rec::TrackParticle* combinedMuonTrackParticle() const {
    return ( (this->hasCombinedMuonTrackParticle()) ? *m_combinedMuonTrackParticle : 0); }

  /** Access to the TrackParticles: statistical combination.
      If no refitted muon exists and combined muon only found by Staco, should be identical to above */
  const Rec::TrackParticle* statCombinedMuonTrackParticle() const {
    return ( (this->hasStatCombinedMuonTrackParticle()) ? *m_statCombinedMuonTrackParticle : 0); }

  /** Access to vector smart pointers (ElementLink) to muon segments */
  const ElementLinkVector<Trk::SegmentCollection> muonSegmentLink() const { return m_muonSegments; }

  /** Access  to the smart pointer (ElementLink) to a given muon segment in the list */
  const ElementLink<Trk::SegmentCollection> muonSegmentLink(const unsigned int i, bool& validLink) const;

  /** Access to the smart pointer to inner detector track */
  const ElementLink<Rec::TrackParticleContainer> inDetTrackLink() const { return m_inDetTrackParticle; }

  /** Access to the smart pointer to the muon track extrapolated to the vertex */
  const ElementLink<Rec::TrackParticleContainer> muonExtrapTrackLink() const { return m_muonExtrapolatedTrackParticle; }

  /** Access to the smart pointer to the muon track extrapolated out from the vertex */
  const ElementLink<Rec::TrackParticleContainer> innerExtrapTrackLink() const { return m_innerExtrapolatedTrackParticle; }

  /** Access to the smart pointer to the muon combined track */
  const ElementLink<Rec::TrackParticleContainer> combinedTrackLink() const { return m_combinedMuonTrackParticle; }

  /** Access to the smart pointer to the muon combined track */
  const ElementLink<Rec::TrackParticleContainer> statCombinedTrackLink() const { return m_statCombinedMuonTrackParticle; }

  /** Access to the smart pointer to the muon spectrometer track */
  const ElementLink<Rec::TrackParticleContainer> muonSpectrometerTrackLink() const { return m_muonSpectrometerTrackParticle; }

  /*8 Access to the smart pointer to the calo  cluster associated to the muon */
  const ElementLink<CaloClusterContainer>& clusterLink() const { return m_cluster; }

  /** Access to the smart pointer to the calorimeter energy loss objeect associated to the muon */
  const ElementLink<MuonCaloEnergyContainer> caloEnergyLink() const { return m_caloEnergyLoss; }

  /** access to the calorimeter info */
  const CaloCluster* cluster() const { return ((this->hasCluster()) ? *m_cluster : 0); }

  /** Get Inner detector Hits information from ID trackparticle */
  int numberOfInnermostPixelLayerHits() const;
  /** Get number of Pixel hits from ID trackparticle */
  int numberOfPixelHits() const;
  /** Get number of Silicon SCT hits from ID trackparticle */
  int numberOfSCTHits() const;
  /** Get number of TRT hits from ID trackparticle */
  int numberOfTRTHits() const;
  /** Get number of TRT High Threshold hits from ID trackparticle */
  int numberOfTRTHighThresholdHits() const;
  /** Get number of shared hits or holes from ID trackparticle */
  int numberOfInnermostPixelLayerSharedHits() const;
  /** Get number of Pixel shared hits from ID trackparticle */
  int numberOfPixelSharedHits() const;
  /** Get number of Pixel holes from ID trackparticle */
  int numberOfPixelHoles() const;
  /** Get number of Silicon SCT shared hits from ID trackparticle */
  int numberOfSCTSharedHits() const;
  /** Get number of Silicon SCT holes from ID trackparticle */
  int numberOfSCTHoles() const;
  /** Get number of TRT Outliers from ID trackparticle */
  int numberOfTRTOutliers() const;
  /** Get number of TRT Outliers High Threshold from ID trackparticle */
  int numberOfTRTHighThresholdOutliers() const;

  /** Get Muon Spectrometer MDT Hits information for MS track or tagged segments */
  int numberOfMDTHits() const;
  /** Get Muon Spectrometer MDT holes information for MS track or tagged segments */
  int numberOfMDTHoles() const;
  /** Get Muon Spectrometer CSC Eta Hits information for MS track or tagged segments */
  int numberOfCSCEtaHits() const;
  /** Get Muon Spectrometer CSC Eta Holes information for MS track or tagged segments */
  int numberOfCSCEtaHoles() const;
  /** Get Muon Spectrometer CSC Phi Hits information for MS track or tagged segments */
  int numberOfCSCPhiHits() const;
  /** Get Muon Spectrometer CSC Phi Holes information for MS track or tagged segments */
  int numberOfCSCPhiHoles() const;
  /** Get Muon Spectrometer RPC Eta Hits information for MS track or tagged segments */
  int numberOfRPCEtaHits() const;
  /** Get Muon Spectrometer RPC Eta Holes information for MS track or tagged segments */
  int numberOfRPCEtaHoles() const;
  /** Get Muon Spectrometer RPC Phi Hits information for MS track or tagged segments */
  int numberOfRPCPhiHits() const;
  /** Get Muon Spectrometer RPC Phi Holes information for MS track or tagged segments */
  int numberOfRPCPhiHoles() const;
  /** Get Muon Spectrometer TGC Eta Hits information for MS track or tagged segments */
  int numberOfTGCEtaHits() const;
  /** Get Muon Spectrometer TGC Eta Holes information for MS track or tagged segments */
  int numberOfTGCEtaHoles() const;
  /** Get Muon Spectrometer TGC Phi Hits information for MS track or tagged segments */
  int numberOfTGCPhiHits() const;
  /** Get Muon Spectrometer TGC Phi Holes information for MS track or tagged segments */
  int numberOfTGCPhiHoles() const;

  /** number of Ganged Pixel for track from primary author */
  int numberOfGangedPixels() const;
  /** number of outliers for track from primary author */
  int numberOfOutliersOnTrack() const;
  /** number of summaryTypes for track from primary author */
  int numberOfTrackSummaryTypes(unsigned int type=0) const;

  // get d0 and z0 with respect to primary vertex
  // KC: These methods were taking out of the TrackParticle
  //  const double z0wrtPrimVtx() const;
  //  const double d0wrtPrimVtx() const;

  /** Access to the MS eta hits associated by segment-tagged muons */
  const std::vector<int>& associatedEtaDigits() const { return m_associatedEtaDigits; }
  /** Access to the MS phi hits associated by segment-tagged muons */
  const std::vector<int>& associatedPhiDigits() const { return m_associatedPhiDigits; }
  /** Access to the MS eta hits associated by segment-tagged muons */
  const std::vector<int>& segmentEtaDigits() const { return m_associatedEtaDigits; }
  /** Access to the MS phi hits associated by segment-tagged muons */
  const std::vector<int>& segmentPhiDigits() const { return m_associatedPhiDigits; }

  /** field integral seen by the track in the muon spectrometer - defined only for standalone,
      combined and MuGirl(refit) muons */
  float spectrometerFieldIntegral() const
  { return this->parameter(MuonParameters::spectrometerFieldIntegral); }

  /** scatteringAngleSignificance based on the curvature significance, from track fit in ID
      (thus not available for SA muons) */
  float scatteringCurvatureSignificance() const
  { return this->parameter(MuonParameters::scatteringCurvatureSignificance); }

  /** scatteringAngleSignificance based on hard scatterers over few near-by measurements,
      from track fit in inner detector (thus not available for SA muons) */
  float scatteringNeighbourSignificance() const
  { return this->parameter(MuonParameters::scatteringNeighbourSignificance); }

  /** momentumBalanceSignificance defined for fitted or combined muons in order to identify decays in flight */
  float momentumBalanceSignificance() const
  {return this->parameter(MuonParameters::momentumBalanceSignificance); }

  /** Access to some Delta(eta) from muon segment taggers */
  float segmentDeltaEta() const { return this->parameter(MuonParameters::segmentDeltaEta); }
  /** Access to some Detla(phi) from muon segment taggers */
  float segmentDeltaPhi() const { return this->parameter(MuonParameters::segmentDeltaPhi); }
  /** Access to a match chi2/ndf from muon segment taggers */
  float segmentChi2OverDoF() const { return this->parameter(MuonParameters::segmentChi2OverDoF); }
  /** Access to some neural-network output for muons found by MuGirl */
  float annBarrel () const { return this->parameter(MuonParameters::annBarrel); }
  /** Access to some neural-network output for muons found by MuGirl */
  float annEndCap () const { return this->parameter(MuonParameters::annEndCap); }
  /** Access to some angles for muons found by MuGirl */
  float innAngle () const { return this->parameter(MuonParameters::innAngle); }
  /** Access to some angles for muons found by MuGirl */
  float midAngle () const { return this->parameter(MuonParameters::midAngle); }
  /** Access to event phase for muons found by MuGirlLowBeta */
  float t0() const { return this->parameter(MuonParameters::t0); }
  /** Access to particle velocity fit parameter for muons found by MuGirlLowBeta */
  float beta() const { return this->parameter(MuonParameters::beta); }

  /** Access to the muon segments */
  const Trk::Segment * muonSegment(unsigned int i) const {
     if ( m_muonSegments.size() <= i ) return 0;
     else if ( m_muonSegments[i].isValid() ) return *(m_muonSegments[i]);
     else return 0; }

  /** Access to the number of muon segments */
  unsigned int numberOfSegments() const { return m_muonSegments.size(); }

  /** access to TrkTracks - InDetTrack*/
  const Trk::Track* inDetTrkTrack() const {
	return ((this->hasInDetTrackParticle()) ? (*m_inDetTrackParticle)->originalTrack() : 0); }
  /** access to TrkTracks - MuonSpectrometerTrack*/
  const Trk::Track* muonSpectrometerTrkTrack() const {
	return ((m_muonSpectrometerTrackParticle.isValid()) ? (*m_muonSpectrometerTrackParticle)->originalTrack() : 0); }
  /** access to TrkTracks - muonExtrapolatedTrack*/
  const Trk::Track* muonExtrapolatedTrkTrack() const {
	return ((this->hasMuonExtrapolatedTrackParticle()) ? (*m_muonExtrapolatedTrackParticle)->originalTrack() : 0); }
  /** access to TrkTracks - innerExtrapolatedTrack*/
  const Trk::Track* innerExtrapolatedTrkTrack() const {
	return ((this->hasInnerExtrapolatedTrackParticle()) ? (*m_innerExtrapolatedTrackParticle)->originalTrack() : 0); }
  /** access to TrkTracks - CombinedTrack*/
  const Trk::Track* combinedMuonTrkTrack() const {
	return ((this->hasCombinedMuonTrackParticle()) ? (*m_combinedMuonTrackParticle)->originalTrack() : 0); }

  /** access to muon quality information 
      the user must take care to delete the pointer
      the Muon does not own it */
  const MuonQualityWord & isMuon () const {
    return m_isMuonBits;
  }
 
  /** test for muon being categorised as "loose" */
  bool isLoose () const {
    return m_isMuonBits.isLoose();
  }  
 
  /** test for muon being categorised  as "medium" */
  bool isMedium () const {
    return m_isMuonBits.isMedium();
  }

  /** test for muon being categorised as "tight" */
  bool isTight () const {
    return m_isMuonBits.isTight();
  }

  /** return the muon likelihood - not yet used */ 
  float isMuonLikelihood() const { return m_isMuonLikelihood; }

  // set methods

  /** set a parameter in the MuonParamDefs.h */
  void set_parameter(MuonParameters::ParamDef index, double value, bool overwrite=false);

  /** set the energy loss in the calorimeter */
  void set_energyLoss ( const CaloEnergy* caloEnergy ) { m_caloEnergyLoss.setElement(const_cast<CaloEnergy*>(caloEnergy)); }
  /** set the energy loss in the calorimeter */
  void set_energyLossContainer ( const MuonCaloEnergyContainer* cont ) { m_caloEnergyLoss.setStorableObject(*cont); }
  /** set the energy loss in the calorimeter */
  void set_energyLoss ( const MuonCaloEnergyContainer* cont, const CaloEnergy* caloEnergy );

  /** set the chi2 parameters - matchChi2*/
  void set_matchChi2(float x)     {m_matchChi2 = x;}
  /** set the chi2 parameters - number of degrees of freedom*/
  void set_matchNumberDoF(int x)  {m_matchNumberDoF = x;}
  /** set the outer match chi2/ndof for combined and segment-tagged muons */
  void set_outerMatchChi2(double x, int ndof);

  /** set the best match */
  void set_bestMatch( bool best) { m_bestMatch = best; }

  /** is this Muon corrected in AOD-to-AOD correction or not? */
  void set_isCorrected ( bool corrected ) { m_isCorrected=corrected; }

  /** set the Calorimeter information */
  void set_cluster( const CaloClusterContainer* cont, const CaloCluster* cluster ) {
    m_cluster.toContainedElement( *cont, const_cast<CaloCluster*> (cluster));
    m_hasCluster = m_cluster.isValid();
  }
  /** set the Calorimeter information */
  void set_cluster( const CaloCluster* cluster) {
     m_cluster.setElement(const_cast<CaloCluster*>(cluster));
     m_hasCluster = m_cluster.isValid();
  }
  /** set the Calorimeter information, by link */
  void set_cluster( const ElementLink<CaloClusterContainer>& cluster) {
     m_cluster = cluster;
     m_hasCluster = m_cluster.isValid();
  }
  /** set the Calorimeter information */
  void set_clusterContainer(const CaloClusterContainer * clusterContainer) {
     m_cluster.setStorableObject(*clusterContainer);
     m_hasCluster = m_cluster.isValid();
  }

  /** Set the inner detector track particle. Does not change the Muon's 4-momentum. */
  void set_inDetTrackParticle(const Rec::TrackParticleContainer*, const Rec::TrackParticle* );
  /** Set the inner detector track particle. Does not change the Muon's 4-momentum. */
  void set_inDetTrackParticle(const Rec::TrackParticle* );

  /** set Muon Spectrometer TrackParticle */
  void set_muonSpectrometerTrackParticle(const Rec::TrackParticleContainer*, const Rec::TrackParticle*);

  /** set the muon spectrometer extrapolated track particle. Does not change the Muon's 4-momentum. */
  void set_muonExtrapolatedTrackParticle(const Rec::TrackParticleContainer*, const Rec::TrackParticle*);
  /** set the muon spectrometer extrapolated track particle. Does not change the Muon's 4-momentum. */
  void set_muonExtrapolatedTrackParticle(const Rec::TrackParticle* );

  /** set the extrapolated track particle from the inner detector. Does not change the Muon's 4-momentum. */
  void set_innerExtrapolatedTrackParticle(const Rec::TrackParticleContainer*, const Rec::TrackParticle*);
  /** set the extrapolated track particle from the inner detector. Does not change the Muon's 4-momentum. */
  void set_innerExtrapolatedTrackParticle(const Rec::TrackParticle* );

  /** set the combined muon track particle.  Does not change the Muon's 4-momentum. */
  void set_combinedMuonTrackParticle(const Rec::TrackParticleContainer*, const Rec::TrackParticle* );
  /** set the combined muon track particle. Does not change the Muon's 4-momentum. */
  void set_combinedMuonTrackParticle(const Rec::TrackParticle* );
  /** set the combined muon track particle for statistical combination. Does not change the Muon's 4-momentum. */
  void set_statCombinedMuonTrackParticle(const Rec::TrackParticleContainer*,
					 const Rec::TrackParticle* );

  /** set the combined muon track particle */
  void set_muonSegments(const Trk::SegmentCollection*, const std::vector<const Trk::Segment*>& );
  /** set the combined muon track particle */
  void addSegment ( const Trk::SegmentCollection*, const Trk::Segment* );

  /** set the TrackParticle containers */
  void set_inDetTrackParticleContainer(const Rec::TrackParticleContainer * inDetTPContainer) {
     m_inDetTrackParticle.setStorableObject(*inDetTPContainer);
     m_hasInDetTrackParticle = m_inDetTrackParticle.isValid();
  }
  /** set the TrackParticle containers */
  void set_muonExtrapolatedTrackParticleContainer(const Rec::TrackParticleContainer *
                                                  muonExtrapolatedTPContainer) {
     m_muonExtrapolatedTrackParticle.setStorableObject(*muonExtrapolatedTPContainer);
     m_hasMuonExtrapolatedTrackParticle = m_muonExtrapolatedTrackParticle.isValid();
  }
  /** set the TrackParticle containers */
  void set_innerExtrapolatedTrackParticleContainer(const Rec::TrackParticleContainer *
                                                   innerExtrapolatedTPContainer) {
     m_innerExtrapolatedTrackParticle.setStorableObject(*innerExtrapolatedTPContainer);
     m_hasInnerExtrapolatedTrackParticle = m_innerExtrapolatedTrackParticle.isValid();
  }
  /** set the TrackParticle containers */
  void set_combinedMuonTrackParticleContainer(const Rec::TrackParticleContainer * combinedMuonTPContainer) {
     m_combinedMuonTrackParticle.setStorableObject(*combinedMuonTPContainer);
     m_hasCombinedMuonTrackParticle = m_combinedMuonTrackParticle.isValid();
  }

  /** set the TrackParticle containers */
  void set_statCombinedMuonTrackParticleContainer(const Rec::TrackParticleContainer * combinedMuonTPContainer) {
     m_statCombinedMuonTrackParticle.setStorableObject(*combinedMuonTPContainer);
  }

  /** Set the LowPt stuff */
  /** Set the LowPt - Associated Eta Digits */
  void set_numberOfAssociatedEtaDigits(int nMDT, int nCSC, int nRPC, int nTGC);
  /** Set the LowPt - Associated Phi Digits */
  void set_numberOfAssociatedPhiDigits(int nMDT, int nCSC, int nRPC, int nTGC);
  /** Set the LowPt - Associated Eta Digits */
  void set_numberOfAssociatedEtaDigits(const std::vector<int>& associated_digits);
  /** Set the LowPt - Associated Phi Digits */
  void set_numberOfAssociatedPhiDigits(const std::vector<int>& associated_digits);

  /** Set Segment Eta Digits */
  void set_numberOfSegmentEtaDigits(int nMDT, int nCSC, int nRPC, int nTGC);
  /** Set Segment Phi Digits */
  void set_numberOfSegmentPhiDigits(int nMDT, int nCSC, int nRPC, int nTGC);
  /** Set Segment Eta Digits */
  void set_numberOfSegmentEtaDigits(const std::vector<int>& associated_digits);
  /** Set Segment Phi Digits */
  void set_numberOfSegmentPhiDigits(const std::vector<int>& associated_digits);

  /** set field integral seen by the track in the muon spectrometer - defined only for standalone,
      combined and MuGirl(refit) muons */
  void set_spectrometerFieldIntegral (const float field) 
  { this->set_parameter(MuonParameters::spectrometerFieldIntegral,field); }

  /** set scatteringAngleSignificance based on the curvature significance, from track fit in ID. */
  void set_scatteringCurvatureSignificance (const float significance) 
  { this->set_parameter(MuonParameters::scatteringCurvatureSignificance,significance); }

  /** set scatteringAngleSignificance based on hard scatterers over few near-by measurements,
      from track fit in inner detector */
  void set_scatteringNeighbourSignificance (const float significance) 
  { this->set_parameter(MuonParameters::scatteringNeighbourSignificance,significance); }

  /** set momentumBalanceSignificance for fitted or combined muons as discriminator against background muons */
  void set_momentumBalanceSignificance(const float significance)
  { this->set_parameter(MuonParameters::momentumBalanceSignificance,significance); }

  /** Set Segment Delta Eta */
  void set_segmentDeltaEta (const float  deltaEta) { this->set_parameter(MuonParameters::segmentDeltaEta,deltaEta); }
  /** Set Segment Delta Phi */
  void set_segmentDeltaPhi (const float  deltaPhi) {  this->set_parameter(MuonParameters::segmentDeltaPhi,deltaPhi); }
  /** Set Segment Chi2 over degrees of freeedom */
  void set_segmentChi2OverDoF (const float chi2) { this->set_parameter(MuonParameters::segmentChi2OverDoF,chi2); }

  /** Set annBarrel */
  void set_annBarrel (const float annB) {  this->set_parameter(MuonParameters::annBarrel,annB); }
  /** Set annEndCap */
  void set_annEndCap (const float annE) {  this->set_parameter(MuonParameters::annEndCap,annE); }
  /** Set innAngle */
  void set_innAngle  (const float innA) {  this->set_parameter(MuonParameters::innAngle,innA); }
  /** Set midAngle */
  void set_midAngle  (const float midA) {  this->set_parameter(MuonParameters::midAngle,midA); }

 //-----------------------------------------------------------------
 // 4-mom modifiers
 //-----------------------------------------------------------------

 /** set inverse pT data member  */
 void setIPt( const double theIPt )
 { return this->momentumBase().setIPt(theIPt); }

 /** set cot(theta) data member  */
 void setCotTh( const double theCotTh )
 { return this->momentumBase().setCotTh(theCotTh); }

 /** set phi data member  */
 void setPhi( const double thePhi )
 { return this->momentumBase().setPhi(thePhi); }

 /** set m data member  */
 void setM( const double theM )
 { return this->momentumBase().setM(theM); }

 /** function to copy muon segment element links into this muon */
 void copySegmentLinkVector( const ElementLinkVector<Trk::SegmentCollection>& segments );

 /** set muon quality parameters */
 void set_isMuonBits ( const uint16_t isMuonBits ) { m_isMuonBits.set(isMuonBits); }
 void set_isMuonBits ( const MuonQualityWord& qWord ) { m_isMuonBits = qWord; }
 void set_isMuonLikelihood ( const float muonLikelihood ) { m_isMuonLikelihood = muonLikelihood; }

 private:

  /** the author of this muon */
  MuonParameters::Author m_author;

  /** was there a successfully combined trach? */
  bool m_hasCombinedMuon;

  /** test for TrackParticles */
  bool m_hasInDetTrackParticle;
  bool m_hasMuonExtrapolatedTrackParticle;
  bool m_hasInnerExtrapolatedTrackParticle;
  bool m_hasCombinedMuonTrackParticle;

  /* Calorimeter cluster info */
  bool m_hasCluster;

  /* chi2 of the track matching */
  float m_matchChi2;
  int m_matchNumberDoF;
  int m_outerMatchNumberDoF;
  Trk::FitQuality m_transientOuterMatchChi2;
  bool m_bestMatch;

  /** Low Pt muon stuff */
  std::vector<int> m_associatedEtaDigits;
  std::vector<int> m_associatedPhiDigits;
  /** end LowPt stuff */

  /** the associated TrackParticles */
  ElementLink<Rec::TrackParticleContainer> m_inDetTrackParticle;
  ElementLinkVector<Trk::SegmentCollection> m_muonSegments;
  ElementLink<Rec::TrackParticleContainer> m_muonExtrapolatedTrackParticle;
  ElementLink<Rec::TrackParticleContainer> m_innerExtrapolatedTrackParticle;
  ElementLink<Rec::TrackParticleContainer> m_combinedMuonTrackParticle;
  ElementLink<Rec::TrackParticleContainer> m_statCombinedMuonTrackParticle;
  ElementLink<CaloClusterContainer> m_cluster;

  /** needed by MOORE */
  ElementLink<Rec::TrackParticleContainer> m_muonSpectrometerTrackParticle;

  /** Parameters as defined in MuonParamDef.h */
  std::vector<std::pair<MuonParameters::ParamDef, float> > m_parameters;

  /** this muon is also found by the lowPT reconstruction algorithm */
  bool m_isAlsoFoundByLowPt;

  /** this muon is also found by the Calo Muon ID reconstruction algorithm */
  bool m_isAlsoFoundByCaloMuonId;

  /** energy loss in calorimeter */
  ElementLink<MuonCaloEnergyContainer> m_caloEnergyLoss;

  /** flag the calo muon if it is also reconstruction by the standart muon algorithms */
  unsigned short m_caloMuonAlsoFoundByMuonReco;

  /** AOD to AOD correction */
  bool m_isCorrected;


  /** all authors - all the authors of this this Muon */
  uint16_t m_allAuthors;

  /** muon selection parameters */
  MuonQualityWord m_isMuonBits;
  float    m_isMuonLikelihood;

};

}

#endif // MUONEVENT_MUON_H





