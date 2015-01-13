/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUTRACKFINDER_H
#define TAUREC_TAUTRACKFINDER_H

#include "tauRec/TauToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "ITrackToVertex/ITrackToVertex.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace Trk {
  class ITrackSelectorTool;
  class IParticleCaloExtensionTool;
}

/////////////////////////////////////////////////////////////////////////////

/** 
 * @brief Associate tracks to the tau candidate.
 * 
 *  The tracks have to pass dedicated quality criteria and have to 
 *  match to a primary vertex consistent with the tau origin.
 * 
 * @author KG Tan <Kong.Guan.Tan@cern.ch>
 * @author Felix Friedrich
 */

class TauTrackFinder : public TauToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor and Destructor
    //-------------------------------------------------------------
    TauTrackFinder(const std::string& type, const std::string& name, const IInterface* parent);
    ~TauTrackFinder();

    //-------------------------------------------------------------
    //! Enumerator defining type of tau track
    //-------------------------------------------------------------
    enum TauTrackType
    {
        TauTrackCore  = 0,
        TauTrackWide  = 1,
        TauTrackOther = 2,
        NotTauTrack   = 3
    };

    //-------------------------------------------------------------
    //! Algorithm functions
    //-------------------------------------------------------------
    virtual StatusCode initialize();
    virtual StatusCode eventInitialize(TauCandidateData *data);
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode eventFinalize(TauCandidateData *data);
    virtual StatusCode finalize();
    
    //-------------------------------------------------------------
    //! Extrapolate track eta and phi to the calorimeter middle surface
    //-------------------------------------------------------------
    StatusCode extrapolateToCaloSurface(TauCandidateData *data);

    TauTrackType tauTrackType( const xAOD::TauJet* tauJet,
    		const xAOD::TrackParticle* trackParticle,
    		const xAOD::Vertex* primaryVertex);

    void getTauTracksFromPV( const xAOD::TauJet* tauJet,
    		const xAOD::TrackParticleContainer* trackParticleCont,
    		const xAOD::Vertex* primaryVertex,
    		std::vector<const xAOD::TrackParticle*> &tauTracks,
    		std::vector<const xAOD::TrackParticle*> &wideTracks,
    		std::vector<const xAOD::TrackParticle*> &otherTracks);

    // old style AOD version
    void removeOffsideTracksWrtLeadTrk(std::vector<const Rec::TrackParticle*> &tauTracks,
                                           std::vector<const Rec::TrackParticle*> &wideTracks,
                                           std::vector<const Rec::TrackParticle*> &otherTracks,
                                           const Trk::RecVertex* tauOrigin,
                                           double maxDeltaZ0);

    // new xAOD version
    void removeOffsideTracksWrtLeadTrk(std::vector<const xAOD::TrackParticle*> &tauTracks,
                                           std::vector<const xAOD::TrackParticle*> &wideTracks,
                                           std::vector<const xAOD::TrackParticle*> &otherTracks,
                                           const xAOD::Vertex* tauOrigin,
                                           double maxDeltaZ0);

    void  getDeltaZ0Values(std::vector<float>& vDeltaZ0coreTrks, std::vector<float>& vDeltaZ0wideTrks);
    void  resetDeltaZ0Cache();

private:
    //-------------------------------------------------------------
    //! Some internally used functions
    //-------------------------------------------------------------
    float getZ0(const Rec::TrackParticle* track, const Trk::RecVertex* vertex);  //AOD version
    float getZ0(const xAOD::TrackParticle* track, const xAOD::Vertex* vertex);   //xAOD version

private:
    //-------------------------------------------------------------
    //! Storegate names of input containers and output containers
    //-------------------------------------------------------------
    std::string m_inputTauJetContainerName;
    std::string m_inputTrackParticleContainerName;
    std::string m_inputPrimaryVertexContainerName;

    //-------------------------------------------------------------
    //! tools
    //-------------------------------------------------------------
    ToolHandle< Trk::IParticleCaloExtensionTool >  m_caloExtensionTool;
    ToolHandle<Trk::ITrackSelectorTool> m_trackSelectorTool_tau;
    ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
    
    //-------------------------------------------------------------
    //! Input parameters for algorithm
    //-------------------------------------------------------------
    double  m_maxJetDr_tau;
    double  m_maxJetDr_wide;
    
    //-------------------------------------------------------------
    // z0 cuts
    //-------------------------------------------------------------
    float m_z0maxDelta;
    bool m_applyZ0cut;
    bool m_storeInOtherTrks;
    std::vector<float> m_vDeltaZ0coreTrks;
    std::vector<float> m_vDeltaZ0wideTrks;

    //-------------------------------------------------------------
    // Bypass TrackSelectorTool
    //-------------------------------------------------------------

    bool m_bypassSelector;

    //-------------------------------------------------------------
    //! Convenience functions to handle storegate objects
    //-------------------------------------------------------------
    template <class T>
    bool openContainer(T* &container, std::string containerName, bool printFATAL=false);

    template <class T>
    bool retrieveTool(T &tool);

};

#endif
