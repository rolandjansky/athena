/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRIGEFFJPSITOOLS_H
#define ITRIGEFFJPSITOOLS_H
/**
 * @author    Hironori Kiyamura
 * @author    Takashi Matsushita
 * @date      $Date: 2013-11-11 01:34:19 +0100 (Mon, 11 Nov 2013) $
 * @version   $Revision: 569614 $
 */

/** @todo */
/** @warnings */


/*--------------------------------------------------------------------*
 * headers
 *--------------------------------------------------------------------*/
#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"

class INavigable4Momentum;
namespace Rec { class TrackParticle; }
namespace Trk { class Track; }
namespace Trk { class RIO_OnTrack; }
namespace Trk { class TrackStateOnSurface; }

static const InterfaceID IID_ITrigEffJpsiTools("ITrigEffJpsiTools", 1, 0);


/*--------------------------------------------------------------------*
 * classes
 *--------------------------------------------------------------------*/
class ITrigEffJpsiTools : virtual public IAlgTool
{
 public:
    enum TrackType { COMBINED=0, SPECTROMETER=1, EXTRAPOLATED=2, IDONLY=3, UNDEFINED=99 };
    enum TargetDetector { UNDEF, TGC, RPC };
    struct TrigObject {
      std::vector<std::string>  names;
      double                    eta;
      double                    phi;
      double                    pt;
      int                       q;
      int64_t                   id;
      int64_t                   parentId;
      int64_t                   grandparentId;
      std::vector<int64_t>      daughterIds;
      std::vector<int>          combIds;
      std::vector<int>          efObjIds;
      int                       type; // combined, spectrometer, extrapolated track
    };
    typedef std::map<int64_t,  TrigObject> TrigObjectMap;
    typedef std::pair<int64_t, TrigObject> TrigObjectPair;

    static const InterfaceID& interfaceID() { return IID_ITrigEffJpsiTools; };

    /** checks if track has passed given trigger chain
     *
     * @param track[in] Rec::TrackParticle to be tested
     * @param chainName[in] name of trigger chain
     * @return true if the given track has passed the trigger chain
     */
    virtual bool isTriggeredMuonEF(const Rec::TrackParticle* track,
                                   const std::string& chainName) const = 0;

    /** checks if track has passed given trigger chain
     *
     * @param track[in] Rec::TrackParticle to be tested
     * @param chainName[in] name of trigger chain
     * @return true if the given track has passed the trigger chain
     */
    virtual bool isTriggeredMuonL2(const Rec::TrackParticle* track,
                                   const std::string& chainName) const = 0;

    /** checks if track has passed given trigger chain
     *
     * @param track[in] Rec::TrackParticle to be tested
     * @param chainName[in] name of trigger chain
     * @return true if the given track has passed the trigger chain
     */
    virtual bool isTriggeredMuonL1(const Rec::TrackParticle* track,
                                   const std::string& chainName) const = 0;

    /** checks if track has matching muon RoI with the given threshold or not
     *
     * @param track[in] Rec::TrackParticle to be tested
     * @param thresholdNumber[in] logic number of the muon level1 threshold to be passed
     * @return true if the given track has corresponding muon RoI, false otherwise
     */
    virtual bool isTriggeredMuon(const Rec::TrackParticle* track,
                                 int thresholdNumber) const = 0;

    /** checks if a track pair is a J/psi candidate or not
     *
     * @param track1[in] Rec::TrackParticle to be tested
     * @param track2[in] Rec::TrackParticle to be tested
     * @return true if the given track pair is a J/psi candidate, false otherwise
     */
    virtual bool isJPsiCandidate(const Rec::TrackParticle* track1,
                                 const Rec::TrackParticle* track2) const = 0;

    /** checks if a 4-vector pair is a Jpsi candidate or not
     *
     * @param mom1[in] INavigable4Momentum to be tested
     * @param mom2[in] INavigable4Momentum to be tested
     */
    virtual bool isJPsiCandidate(const INavigable4Momentum* mom1,
                                 const INavigable4Momentum* mom2) const = 0;

    /** searches level1 muon RoI associating with the given track
     *
     * @param track[in] Rec::TrackParticle to be tested
     * @param barrel[out] returns true if the track passes barrel pivot plane
     * @param endcap[out] returns true if the track passes endcap pivot plane
     * @param eta[out] if not null, returns extrapolated position and error in eta (if the track passes both barrel/endcap pivot planes without matching roi, barrel information is stored)
     * @param phi[out] if not null, returns extrapolated position and error in phi (if the track passes both barrel/endcap pivot planes without matching roi, barrel information is stored)
     * @return index to muon_type (std::vector<Muon_ROI>), -1 when there is no association
     */
    virtual int getMatchedRoI(const Rec::TrackParticle* track,
                              bool* barrel = 0,
                              bool* endcap = 0,
                              Amg::Vector2D* eta = 0,
                              Amg::Vector2D* phi = 0) const = 0;

    /** calculates size of the givin RoI
     *
     * @param roiWord[in] roiWord to be examined
     * @param etaSize[out] roiSize in eta
     * @param phiSize[out] roiSize in phi
     * @return StatusCode
     */
    virtual StatusCode getRoiSize(const unsigned int roiWord,
                                  double* etaSize,
                                  double* phiSize) const = 0;

    /** get array of reconstruction input objects (rio) on track
     *
     * @param track[in] reference track
     * @param rots[in] array of rio on the reference track
     * @param pivotOnly[in] if true, get rio on pivot plane only
     * @return StatusCode
     */
    virtual StatusCode getRioOnTriggerChamber(const Trk::Track* track,
                                              std::vector<const Trk::RIO_OnTrack*>& rots,
                                              bool pivotOnly=true) const = 0;

    /** extrapolate an input track to given rio
     *
     * @param track[in] track to be extrapolated
     * @param rot[in] target rio
     * @param eta[out]  eta and its error of extrapolated position
     * @param phi[out]  phi and its error of extrapolated position
     * @param ptpz[out] pt and pz of the extrapolated track
     * @param getTsos[in] set true when you need tsos output
     * @param tsos[out] array of TrackStateOnSurface along extrapolation
     * @return true for successful extrapolation, false otherwise
     */
    virtual bool extrapolateToRio(const Rec::TrackParticle* track,
                                  const Trk::RIO_OnTrack* rot,
                                  Amg::Vector2D* eta,
                                  Amg::Vector2D* phi,
                                  Amg::Vector2D* ptpz,
                                  bool getTsos,
                                  const std::vector<const Trk::TrackStateOnSurface*>* &tsos) const = 0;

    /** extrapolate an input track to given point
     *
     * @param track[in] track to be extrapolated
     * @param pos[in] target position
     * @param detector[in] target detector
     * @param eta[out] eta and its error of extrapolated position
     * @param phi[out] phi and its error of extrapolated position
     * @param mom[out] momentum of the extrapolated track
     * @return true for successful extrapolation, false otherwise
     */
    virtual bool extrapolateToPoint(const Rec::TrackParticle* track,
                                    const Amg::Vector3D* pos,
                                    const int detector,
                                    Amg::Vector2D* eta,
                                    Amg::Vector2D* phi,
                                    Amg::Vector3D* mom) const = 0;

    /** get all trigger objects of a given chain and its parents
     *
     * @param chain[in] name of the trigger chain
     * @param muonsEF[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @param muonsL2[out] pointer to the TrigObjectMap to be filled by L2 trigger objects
     * @param muonsL1[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @return true for successful retrival, false otherwise
     */
    virtual bool getTriggerObjects(const std::string& chain,
                                   TrigObjectMap* muonsEF = NULL,
                                   TrigObjectMap* muonsL2 = NULL,
                                   TrigObjectMap* muonsL1 = NULL) const = 0;

    /** get L1 trigger objects of a given chain
     *
     * @param chain[in] name of the trigger chain
     * @param muonsL1[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @return true for successful retrival, false otherwise
     */
    virtual bool getL1TriggerObjects(const std::string& chain,
                                     TrigObjectMap* muonsL1 = NULL) const = 0;

    /** get L2 trigger objects of a given chain and its parents
     *
     * @param chain[in] name of the trigger chain
     * @param muonsL2[out] pointer to the TrigObjectMap to be filled by L2 trigger objects
     * @param muonsL1[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @return true for successful retrival, false otherwise
     */
    virtual bool getL2TriggerObjects(const std::string& chain,
                                     TrigObjectMap* muonsL2 = NULL,
                                     TrigObjectMap* muonsL1 = NULL) const = 0;

    /** get EF trigger objects of a given chain and its parents
     *
     * @param chain[in] name of the trigger chain
     * @param muonsEF[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @param muonsL2[out] pointer to the TrigObjectMap to be filled by L2 trigger objects
     * @param muonsL1[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @return true for successful retrival, false otherwise
     */
    virtual bool getEFTriggerObjects(const std::string& chain,
                                     TrigObjectMap* muonsEF = NULL,
                                     TrigObjectMap* muonsL2 = NULL,
                                     TrigObjectMap* muonsL1 = NULL) const = 0;

    /** get EF trigger objects of a given chain and its parents
     *
     * @param chain[in] name of the trigger chain
     * @param muonsEF[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @param muonsL2[out] pointer to the TrigObjectMap to be filled by L2 trigger objects
     * @param muonsL1[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @return true for successful retrival, false otherwise
     */
    virtual bool getEFAllTriggerObjects(const std::string& chain,
                                     TrigObjectMap* muonsEF = NULL,
                                     TrigObjectMap* muonsL2 = NULL,
                                     TrigObjectMap* muonsL1 = NULL) const = 0;

    /** get EF trigger objects of a given chain and its parents
     *
     * @param chain[in] name of the trigger chain
     * @param muonsEF[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @param muonsL2[out] pointer to the TrigObjectMap to be filled by L2 trigger objects
     * @param muonsL1[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @return true for successful retrival, false otherwise
     */
    virtual bool getEFIDTriggerObjects(const std::string& chain,
				       TrigObjectMap* muonsEF = NULL,
				       TrigObjectMap* muonsL2 = NULL,
				       TrigObjectMap* muonsL1 = NULL) const = 0;

    virtual bool getEFMSTriggerObjects(const std::string& chain,
				       TrigObjectMap* muonsEF = NULL,
				       TrigObjectMap* muonsL2 = NULL,
				       TrigObjectMap* muonsL1 = NULL) const = 0;


    /** get BPhysics L2 trigger objects of a given chain and its parents
     *
     * @param chain[in] name of the trigger chain
     * @param muonsL2[out] pointer to the TrigObjectMap to be filled by L2 trigger objects
     * @param muonsL1[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @return true for successful retrival, false otherwise
     */
    virtual bool getBPhysL2TriggerObjects(const std::string& chain,
                                          TrigObjectMap* muonsL2 = NULL,
                                          TrigObjectMap* muonsL1 = NULL) const = 0;

    /** get BPhysics EF trigger objects of a given chain and its parents
     *
     * @param chain[in] name of the trigger chain
     * @param muonsEF[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @param muonsL2[out] pointer to the TrigObjectMap to be filled by L2 trigger objects
     * @param muonsL1[out] pointer to the TrigObjectMap to be filled by EF trigger objects
     * @return true for successful retrival, false otherwise
     */
    virtual bool getBPhysEFTriggerObjects(const std::string& chain,
                                          TrigObjectMap* muonsEF = NULL,
                                          TrigObjectMap* muonsL2 = NULL,
                                          TrigObjectMap* muonsL1 = NULL) const = 0;

    /** get bunch crossing ID difference between 7-bit bcid and the event
     *
     * @param roiBcid[in] 7-bit bcid of muon RoI
     */
    virtual int muctpiBcidDiff(const int roiBcid) const = 0;

    /** get bunch crossing ID difference between given roi and the event
     *
     * @param roiWord[in] roiWord
     */
    virtual int getMuonRoiBcidDiff(const uint32_t roiWord) const = 0;


    /** checks if the event is triggered by EF_mu0_missingRoi or L1_2MU0 with TGC only trigger
     *
     * @param roiWord[in] roiWord
     */
    virtual bool isOffTimeTgcTrigger() const = 0;

    /** emulates EF_mu4_MSOnly trigger with EF_mu0_outOfTime1
     *
     */
    virtual bool EF_mu4_MSOnly_from_EF_mu0_outOfTime1() const = 0;
};
#endif
