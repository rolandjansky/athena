/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
Summary.h  -  description
-------------------
begin                : Tue Feb 17 2004
email                : edward.moyse@cern.ch
***************************************************************************/

#ifndef TRKTRACKSUMMARY_H
#define TRKTRACKSUMMARY_H

#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include <atomic>
#include <bitset>
#include <iostream>
#include <memory>
#include <vector>

class MsgStream;
class TrackSummaryCnv_p1;
class TrackSummaryCnv_p2;

namespace InDet {
class InDetTrackSummaryHelperTool;
}

namespace Muon {
class MuonTrackSummaryHelperTool;
}

namespace Trk {

// forward declarations
class Track;
class TrackSummaryTool;

/** enumerates the different types of information stored in Summary. Use in
get(const SummaryType type), for for example, summary.get(numOfPixelHits) When
adding a new transient information type, please make sure to increase
numberOfTrackSummaryTypes.*/
enum SummaryType
{
  // --- Inner Detector
  //!< number of contributing layers of the pixel detector

  numberOfContribPixelLayers = 29,
  //!< these are the hits in the first pixel layer, i.e. b-layer
  numberOfBLayerHits = 0,
  //!< number of blayer outliers
  numberOfBLayerOutliers = 31,
  //!< number of Pixel b-layer hits shared by several tracks.
  numberOfBLayerSharedHits = 16,
  //!< number of Pixel b-layer hits split by cluster splitting
  numberOfBLayerSplitHits = 43,
  //!< Do we expect a b-layer hit for this track?
  expectBLayerHit = 42,
  //!< Do we expect a 0th-layer hit for this track?
  expectInnermostPixelLayerHit = 52,
  //!< these are the hits in the 0th pixel layer?
  numberOfInnermostPixelLayerHits = 53,
  //!< number of 0th layer outliers
  numberOfInnermostPixelLayerOutliers = 54,
  //!< number of Pixel 0th layer hits shared by several tracks.
  numberOfInnermostPixelLayerSharedHits = 55,
  //!< number of Pixel 0th layer hits split by cluster splitting
  numberOfInnermostLayerSplitHits = 56,
  //!< Do we expect a 1st-layer hit for this track?
  expectNextToInnermostPixelLayerHit = 57,
  //!< these are the hits in the 1st pixel layer
  numberOfNextToInnermostPixelLayerHits = 58,
  //!< number of 1st pixel layer outliers
  numberOfNextToInnermostPixelLayerOutliers = 59,
  //!< number of Pixel 1st layer hits shared by several tracks.
  numberOfNextToInnermostPixelLayerSharedHits = 60,
  //!< number of Pixel 1st layer hits split by cluster splitting
  numberOfNextToInnermostLayerSplitHits = 61,
  //!< these are the pixel hits, including the b-layer
  numberOfPixelHits = 2,
  //!< these are the pixel outliers, including the b-layer
  numberOfPixelOutliers = 41,
  //!< number of pixel layers on track with absence of hits
  numberOfPixelHoles = 1,
  //!< number of Pixel all-layer hits shared by several tracks.
  numberOfPixelSharedHits = 17,
  //!< number of Pixel all-layer hits split by cluster splitting
  numberOfPixelSplitHits = 44,
  //!< number of pixels which have a ganged ambiguity.
  numberOfGangedPixels = 14,
  //!< number of Ganged Pixels flagged as fakes
  numberOfGangedFlaggedFakes = 32,
  //!< number of dead pixel sensors crossed
  numberOfPixelDeadSensors = 33,
  //!< number of pixel hits with broad errors (width/sqrt(12))
  numberOfPixelSpoiltHits = 35,
  //!< number of DBM Hits
  numberOfDBMHits = 63,
  //!< number of hits in SCT
  numberOfSCTHits = 3,
  //!< number of SCT outliers
  numberOfSCTOutliers = 39,
  //!< number of SCT holes
  numberOfSCTHoles = 4,
  //!< number of Holes in both sides of a SCT module
  numberOfSCTDoubleHoles = 28,
  //!< number of SCT hits shared by several tracks.
  numberOfSCTSharedHits = 18,
  numberOfSCTDeadSensors = 34,
  //!< number of TRT hits
  numberOfSCTSpoiltHits = 36,
  numberOfTRTHits = 5,
  //!< number of TRT outliers
  numberOfTRTOutliers = 19,
  //!< number of TRT holes
  numberOfTRTHoles = 40,
  //!< number of TRT hits which pass the high
  //!< threshold (only xenon counted)
  //!< total number of TRT hits which pass the high threshold
  numberOfTRTHighThresholdHits = 6,
  //!< total number of TRT hits which pass the high threshold
  numberOfTRTHighThresholdHitsTotal = 64,
  //!< number of TRT hits used for dE/dx computation
  numberOfTRTHitsUsedFordEdx = 65,
  //!< number of TRT high threshold outliers (only xenon counted)
  numberOfTRTHighThresholdOutliers = 20,
  //!< number of dead TRT straws crossed
  numberOfTRTDeadStraws = 37,
  //!< number of TRT tube hits
  numberOfTRTTubeHits = 38,
  //!< number of TRT hits on track in straws with xenon
  numberOfTRTXenonHits = 46,
  //!< number of TRT hits used by more than one track
  numberOfTRTSharedHits = 62,

  // --- Muon Spectrometer
  //!< number of mdt hits
  numberOfMdtHits = 7,
  //!< tgc, rpc and csc measure both phi and eta coordinate
  numberOfTgcPhiHits = 8,
  numberOfTgcEtaHits = 9,
  numberOfCscPhiHits = 10,
  numberOfCscEtaHits = 11,
  numberOfRpcPhiHits = 12,
  numberOfRpcEtaHits = 13,
  //! number of CSC Eta measurements missing from the track
  numberOfCscEtaHoles = 21,
  //! number of CSC Phi measurements missing from the track
  numberOfCscPhiHoles = 22,
  //! number of RPC Eta measurements missing from the track
  numberOfRpcEtaHoles = 23,
  //! number of RPC Phi measurements missing from the track
  numberOfRpcPhiHoles = 24,
  //! number of MDT measurements missing from the track
  numberOfMdtHoles = 25,
  //! number of TGC Eta measurements missing from the track
  numberOfTgcEtaHoles = 26,
  //! number of TGC Phi measurements missing from the track
  numberOfTgcPhiHoles = 27,

  // New Small Wheel
  //! number of TGC Eta measurements missing from the track
  numberOfStgcEtaHits = 67,
  //! number of TGC Phi measurements missing from the track
  numberOfStgcPhiHits = 68,
  //! number of TGC Eta measurements missing from the track
  numberOfMmHits = 69,
  //! number of TGC Eta measurements missing from the track
  numberOfStgcEtaHoles = 70,
  //! number of TGC Phi measurements missing from the track
  numberOfStgcPhiHoles = 71,
  //! number of TGC Eta measurements missing from the track
  numberOfMmHoles = 72,
  //! number of unspoilt CSC eta measurements (all CSC phi measurements
  //! are by definition spoilt). See Muon::CscClusterStatus for
  //! definitions of 'spoiled' hits.
  numberOfCscUnspoiltEtaHits = 45,
  //! number of non-deweighted MDT hits.  Only here as
  //! a placeholder, will be filled only on xAOD::Muon
  // --- all
  numberOfGoodMdtHits = 66,
  //!< number of measurements flaged as outliers in TSOS
  numberOfOutliersOnTrack = 15,
  //!< 100 times the standard deviation of the chi2 from the surfaces
  standardDeviationOfChi2OS = 30,

  // reserved: added to keep synchronisation with xAOD::TrackSummary in
  // anticipation of the two being merged

  //!< Electron probability from combining the below
  //!< probabilities [float].
  eProbabilityComb_res = 47,
  //!< Electron probability from  High Threshold (HT) information [float].
  eProbabilityHT_res = 48,
  //!< Electron probability from Time-Over-Threshold
  //!< (ToT) information [float].
  eProbabilityToT_res = 49,
  //!< Electron probability from Brem fitting (DNA) [float].
  eProbabilityBrem_res = 50,
  //!< the dE/dx estimate, calculated using the pixel clusters [?]
  pixeldEdx_res = 51,
  //!< Electron probability from NN [float]
  eProbabilityNN_res = 73,
  //!< TRT track occupancy.
  TRTTrackOccupancy_res = 74,
  //!< dEdx from TRT ToT measurement.
  TRTdEdx_res = 75,

  // -- numbers...
  numberOfTrackSummaryTypes = 76
};

// summary types that are stored as float values
static const std::vector<unsigned int> floatSummaryTypes = {
  eProbabilityComb_res,  eProbabilityHT_res, eProbabilityToT_res,
  eProbabilityBrem_res,  pixeldEdx_res,      eProbabilityNN_res,
  TRTTrackOccupancy_res, TRTdEdx_res
};

// Troels.Petersen@cern.ch:
enum eProbabilityType
{
  eProbabilityComb =
    0, //!< Electron probability from combining the below probabilities.
  eProbabilityHT =
    1, //!< Electron probability from High Threshold (HT) information.
  eProbabilityToT =
    2, //!< Electron probability from Time-Over-Threshold (ToT) information.
  eProbabilityBrem = 3,  //!< Electron probability from Brem fitting (DNA).
  eProbabilityNN = 4,    //!< Electron probability from NN.
  TRTTrackOccupancy = 5, //!< TRT track occupancy.
  TRTdEdx = 6,           //!< dEdx from TRT ToT measurement.
  eProbabilityNumberOfTRTHitsUsedFordEdx =
    7, //!< Number of TRT hits used for dEdx measurement.
  numberOfeProbabilityTypes = 8
};

static const std::vector<float> eProbabilityDefault(numberOfeProbabilityTypes,
                                                    0.5);

/** enumerates the various detector types currently accessible from the isHit()
method. \todo work out how to add muons to this*/
enum DetectorType
{
  pixelBarrel0 = 0, //!< there are three or four pixel barrel layers (R1/R2)
  pixelBarrel1 = 1,
  pixelBarrel2 = 2,
  pixelBarrel3 = 3,

  pixelEndCap0 = 4, //!< three pixel discs (on each side)
  pixelEndCap1 = 5,
  pixelEndCap2 = 6,

  sctBarrel0 = 7, //!< four sct barrel layers
  sctBarrel1 = 8,
  sctBarrel2 = 9,
  sctBarrel3 = 10,

  sctEndCap0 = 11, //!< and 9 sct discs (on each side)
  sctEndCap1 = 12,
  sctEndCap2 = 13,
  sctEndCap3 = 14,
  sctEndCap4 = 15,
  sctEndCap5 = 16,
  sctEndCap6 = 17,
  sctEndCap7 = 18,
  sctEndCap8 = 19,

  trtBarrel = 20,
  trtEndCap = 21,

  DBM0 = 22,
  DBM1 = 23,
  DBM2 = 24,

  numberOfDetectorTypes = 25

}; // not complete yet

/**A summary of the information contained by a track.

If there is information missing, or if I could add useful methods then let me
know. I have deliberately tried to keep this as simple as possible, whilst
allowing for future expansion.

A recent addition is the isHit() method. This is analogous to the old 'hit
patterns' in that you can use it to check if a specific sub-detector element has
been hit by the track.

For example, you could do:
if (summary.isHit(Trk::pixelBarrel0) ) callSomething();

Please note that this class does NO error checking - i.e. it would be possible
for someone to fill this object in an inconsistent manner and it would not be
caught.

@author Edward.Moyse@cern.ch
*/
class TrackSummary
{
public:
  friend class InDet::InDetTrackSummaryHelperTool;
  friend class Muon::MuonTrackSummaryHelperTool;
  friend class Trk::TrackSummaryTool;

  /** Value set in the cxx file to -1. If any of the values returned by
  get(const SummaryType& type)==SummaryTypeNotSet then this means that the
  information has not been filled. i.e. ID information cannot be filled if the
  TrackSummaryTool does not have access to the InDetTrackSummaryHelperTool*/
  static const int SummaryTypeNotSet;

  /**needed by POOL*/
  TrackSummary();

  /**ctor should probably only be called by TrkSummaryTool.
  @param information The vector passed should be complete
  i.e. it should be the size required by m_numSummaryTypes, and any undefined
  values should be set to -1. The vector should be filled using SummaryType to
  select the position of the information.
  @param hitPattern this bitset should be filled using the DetectorType enum.
  True indicates that a sub-detector was hit*/
  TrackSummary(const std::vector<int>& information,
               const std::vector<float>& eProbability,
               std::bitset<numberOfDetectorTypes>& hitPattern,
               float dedx = -1,
               int nhitsuseddedx = -1,
               int nhitsoverflowdedx = -1);

  /** copy ctor*/
  TrackSummary(const TrackSummary& rhs);
  /** assignment operator */
  TrackSummary& operator=(const TrackSummary&);
  /** Move ctor*/
  TrackSummary(TrackSummary&& rhs) noexcept = default;
  /** Move assignment operator */
  TrackSummary& operator=(TrackSummary&&) noexcept = default;

  virtual ~TrackSummary();

  /** adds the values of the passed TrackSummary to this TrackSummary. Mainly
   intended for adding Muon and ID TrackSummaries, when making a combined Track
   for example WARNING: may not make much sense for e.g.
   Trk::numberOfContribPixelLayers*/
  TrackSummary& operator+=(const TrackSummary& ts);

  /** returns the summary information for the passed SummaryType.
  @param type Use the provided enums to access it, i.e. by
  summary.get(numberOfPixelHits)
  @return returns -1 if the enum is undefined (i.e. the information was not
  available in the Track, or (more likely) Trk::TrkTrackSummaryTool is not
  filling it yet)*/
  int get(const SummaryType& type) const;

  /** returns the probability information for the passed ProbabilityType.
  @param type Use the provided enums to access it, i.e. by
  summary.getPID(eProbabilityComb)
  @return returns -1 if the enum is undefined (i.e. the information was not
  available in the Track, or (more likely) Trk::TrkTrackSummaryTool is not
  filling it yet)*/
  // Troels.Petersen@cern.ch:
  float getPID(const eProbabilityType& PIDtype) const;

  /** returns the dE/dx estimate, calculated using the pixel clusters */
  float getPixeldEdx() const;

  int numberOfUsedHitsdEdx() const;

  int numberOfOverflowHitsdEdx() const;

  unsigned long getHitPattern() const;

  /** returns true if the detector type 'type' is hit.
  @param type Please use the DetectorType enum to access the information  (the
  internal positions may change!)
  @return true if sub-detector 'type' is hit*/
  bool isHit(const DetectorType& type) const;

  /** returns a const pointer to the InDetTrackSummary if available */
  const InDetTrackSummary* indetTrackSummary() const;

  /** returns a pointer to a modifiable (non-const)  InDetTrackSummary if
   * available */
  InDetTrackSummary* indetTrackSummary();

  /** returns a pointer to the MuonTrackSummary if available */
  const MuonTrackSummary* muonTrackSummary() const;

  /** returns  pointer to a modifiable (non-const) MuonTrackSummary if available
   */
  MuonTrackSummary* muonTrackSummary();

  /**return number of parameters currently created*/
  static unsigned int numberOfInstantiations();

  /** Update unset summary information.
   * @param type the type of the summary information to be updated.
   * @param new_value the value to be set for the given type.
   * @return false in case the information was set alread.
   */
  bool update(Trk::SummaryType type, int new_value);

private: // data members
  friend class ::TrackSummaryCnv_p1;
  friend class ::TrackSummaryCnv_p2;

  /** vector containing the persistent summary information. */
  std::vector<int> m_information;

  /** vector containing the persistent summary information. */
  // Troels.Petersen@cern.ch:
  std::vector<float> m_eProbability;

  /** contains the dE/dx information, calculated using the pixel clusters */
  float m_dedx;

  int m_nhitsdedx;

  int m_nhitsoverflowdedx;

  /**contains the 'hit pattern'*/
  unsigned long m_idHitPattern;

  /** number of objects of this type in memory */
  static std::atomic<unsigned int> s_numberOfInstantiations;

  /** pointer to the InDetTrackSummary */
  std::unique_ptr<InDetTrackSummary> m_indetTrackSummary;

  /** pointer to the MuonTrackSummary */
  std::unique_ptr<MuonTrackSummary> m_muonTrackSummary;
};

/**output. This dumps the values of each of the possible summary enums*/
MsgStream&
operator<<(MsgStream& out, const TrackSummary& trackSum);

/**output. This dumps the values of each of the possible summary enums*/
std::ostream&
operator<<(std::ostream& out, const TrackSummary& trackSum);
}
#include "TrkTrackSummary/TrackSummary.icc"

#endif

