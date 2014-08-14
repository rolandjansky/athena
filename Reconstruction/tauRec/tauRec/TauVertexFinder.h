/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUVERTEXFINDER_H
#define TAUREC_TAUVERTEXFNIDER_H

#include "tauRec/TauToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/VertexContainer.h"
#include "JetEDM/TrackVertexAssociation.h"

/////////////////////////////////////////////////////////////////////////////

/** 
 * @brief Associate a vertex (origin) to the tau candidate.
 * 
 *  The vertex has to be consistent with the origin of tau jet seed.
 *  The vertex is determined in the following order: TJVA -> first PV in vertex container -> 0
 *  Attention: all downstream tau variables will be calculated to the vertex set here!
 * 
 * @author KG Tan <Kong.Guan.Tan@cern.ch>
 * @author Felix Friedrich <Felix.Friedrich@cern.ch>
 */

class TauVertexFinder : public TauToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor and Destructor
    //-------------------------------------------------------------
    TauVertexFinder(const std::string& type, const std::string& name, const IInterface* parent);
    ~TauVertexFinder();

    //-------------------------------------------------------------
    //! Algorithm functions
    //-------------------------------------------------------------
    virtual StatusCode initialize();
    virtual StatusCode eventInitialize(TauCandidateData *data);
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode eventFinalize(TauCandidateData *data);
    virtual StatusCode finalize();

    ElementLink<xAOD::VertexContainer> getPV_TJVA(const xAOD::TauJet* tauJet, const xAOD::VertexContainer* vertices);

private:
    float getJetVertexFraction(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*>& tracks, const jet::TrackVertexAssociation* tva) const;
        
    //-------------------------------------------------------------
    //! Convenience functions to handle storegate objects
    //-------------------------------------------------------------
    template <class T>
    bool openContainer(T* &container, std::string containerName, bool printFATAL=false);

    template <class T>
    bool retrieveTool(T &tool);

private:
    bool m_printMissingContainerINFO;
    float m_maxJVF;

    //-------------------------------------------------------------
    //! Configureables
    //-------------------------------------------------------------
    bool m_useTJVA;
    std::string m_inputPrimaryVertexContainerName;
    std::string m_assocTracksName;
    std::string m_trackVertexAssocName;    
};

#endif
