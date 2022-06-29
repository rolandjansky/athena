// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef TRACKFITTER_H
#define TRACKFITTER_H

/**
 * @author Unknown
 * @date Unknown
 *
 * Major update March 3, 2020 - Riley Xu riley.xu@cern.ch
 *
 * This class fits tracks from roads.
 */

#include "AthenaBaseComps/AthMessaging.h"
#include "TrigHTTBanks/HTTFitConstantBank.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTFunctions.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTMaps/HTTPlaneMap.h"

#include <list>
#include <vector>

class TrackFitter
: public AthMessaging
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // Main interface functions

        int fitTracks(const std::vector<HTTRoad*>& roads, std::vector<HTTTrack>& tracks);
  
        std::vector<HTTTrack> getMissingHitsCheckTracks() {return m_tracks_missinghits_track;}

        ///////////////////////////////////////////////////////////////////////
        // Constructor and Initialization

        TrackFitter(const HTTFitConstantBank *nominalbank, const std::vector<const HTTFitConstantBank*>& droppedLayerbanks, bool guessingHits);

        ///////////////////////////////////////////////////////////////////////
        // Simple getters and setters

        void setIdentifyBadHit(bool flag)   { m_identify_badhit = flag; }
        bool getIdentifyBadHit() const      { return m_identify_badhit; }

        void setPlaneMap(HTTPlaneMap const * map)   { m_pmap = map; }
        HTTPlaneMap const * getPlaneMap() const     { return m_pmap; }

        void setNorecoveryNhits(int v)  { m_norecovery_nhits = v; }
        int getNorecoveryNhits() const  { return m_norecovery_nhits; }

        void setRequireFirst(bool v)    { m_require_first = v; }
        bool getRequireFirst() const    { return m_require_first; }

        void setDoMajority(unsigned int v)  { m_do_majority = v; }
        unsigned getDoMajority() const      { return m_do_majority; }

        void setDoMissingHitsCheck(bool v)  { m_setDoMissingHitsCheck = v; }
        bool getDoMissingHitsCheck() const  { return m_setDoMissingHitsCheck; }

        void setChi2DofRecoveryMin(float v) { m_Chi2Dof_recovery_min = v; }
        float getChi2DofRecoveryMin() const { return m_Chi2Dof_recovery_min; }

        void setChi2DofRecoveryMax(float v) { m_Chi2Dof_recovery_max = v; }
        float getChi2DofRecoveryMax() const { return m_Chi2Dof_recovery_max; }

        void setDoSecondStage(bool v)   { m_do2ndStage = v; }
        bool getDoSecondStage() const   { return m_do2ndStage; }

	void setIdealCoordFitType(TrackCorrType type) {m_IdealCoordFitType = type;}
	TrackCorrType getIdealCoordFitType() const { return m_IdealCoordFitType; }

	void setDoDeltaGPhis(bool v) { m_doDeltaGPhis = v; }
	bool getDoDeltaGPhis() const { return m_doDeltaGPhis; }

        ///////////////////////////////////////////////////////////////////////
        // Getters of data flow quantities

        int getNFits()              { return m_nfits; }
        int getNFitsMajority()      { return m_nfits_maj; }
        int getNFitsMajoritySCI()   { return m_nfits_maj_SCT; }
        int getNFitsMajorityPix()   { return m_nfits_maj_pix; }
        int getNFitsRecovery()      { return m_nfits_rec; }


    private:
        ///////////////////////////////////////////////////////////////////////
        // Configuration
        bool m_do2ndStage = false;
        bool m_guessinghits = true;
	TrackCorrType m_IdealCoordFitType = TrackCorrType::None;
	bool m_doDeltaGPhis = false;

        // Hit Combation
        const int m_max_ncomb = 10000;  // maximum number of combinations to process per road
        unsigned m_do_majority = 1;     // Do majority fits. If >1, we only do majority fits if ALL full fits fail the chi2 cut
        bool m_require_first = true;    // Require a hit in the first layer when doing majority recovery
        int m_norecovery_nhits = -1;    // maximum number of hits per superstrip beyond which majority is not attempted

        bool m_setDoMissingHitsCheck = false; // do missing hits check

        // Chi2
        float m_Chi2Dof_recovery_min = 40.; //Minimum chi2 cut to do recovery fits
        float m_Chi2Dof_recovery_max = 1e30; //maximum chi2 cut to do recovery fits

        // Output
        bool m_identify_badhit = false; // enables the identification of the bad hits for the recovery


        ///////////////////////////////////////////////////////////////////////
        // Storage

        HTTPlaneMap const *m_pmap = nullptr; // Convenience pointer

        // This bank should always exist. If we are guessing hits there is only one bank and it is this one
        // and if we are not guessing hits it is for 8/8
        // It is just a pointer from BankSvc->FitConstantBank1stStage() but we copy it;
	const HTTFitConstantBank *m_nominalBank;

        // These are only used if we are not guessing hits, one for each plane missing
        // Also just a set of pointers, but we copied them
        std::vector<const HTTFitConstantBank*> m_droppedLayerBanks;

        // These are the tracks we store if we are doing the missing hits cross-check
        std::vector<HTTTrack> m_tracks_missinghits_track;

        // Counters
        int m_idbase = 0;           // offset for new track ids
        int m_nfits = 0;            // number of fits tryied in a road
        int m_nfits_maj = 0;        // number of majority fits
        int m_nfits_maj_SCT = 0;    // number of majority fits with missing in SCT layer        
        int m_nfits_maj_pix = 0;    // number of majority fits with missing in PIX layer
        int m_nfits_rec = 0;        // number of recoverd fits
        int m_nfits_addrec = 0;     // number of recoverd fits


        ///////////////////////////////////////////////////////////////////////
        // Helper Functions

        void resetCounters();
        int fitTracks(HTTRoad *road, std::vector<HTTTrack> &tracks);
        void getMissingInfo(const HTTRoad & road, int & nMissing, bool & missPixel, bool & missStrip,
                            layer_bitmask_t & missing_mask, layer_bitmask_t & norecovery_mask);
        void makeTrackCandidates(const HTTRoad & road, const HTTTrack & temp, std::vector<HTTTrack> &track_cands);
        HTTTrack recoverTrack(HTTTrack const & t, sector_t sector, layer_bitmask_t norecovery_mask, double qoverpt);
        void compute_truth(HTTTrack & newtrk) const;
};

#endif // TRACKFITTER_H
