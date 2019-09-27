// This is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NONPROMPTLEPTONVERTEXINGALG_H
#define NONPROMPTLEPTONVERTEXINGALG_H

/**********************************************************************************
 * @Package: LeptonTaggers
 * @Class  : NonPromptLeptonVertexingAlg
 * @Author : Fudong He
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Decorate leptons with secondary vertex algorithem output
 * 
 **********************************************************************************/

// Local
#include "VarHolder.h"
#include "VertexMergingTool.h"
#include "IVertexFittingSvc.h"

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/VertexContainer.h"

// ROOT
#include "TStopwatch.h"

// C/C++
#include <set>

namespace Prompt
{
  class NonPromptLeptonVertexingAlg: public AthAlgorithm
  {

  public:

    NonPromptLeptonVertexingAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~NonPromptLeptonVertexingAlg() {}
       
    virtual StatusCode initialize();
    virtual StatusCode execute(); 
    virtual StatusCode finalize();
 
  private:
    
    std::vector<const xAOD::TrackParticle*> findNearbyTracks(const xAOD::TrackParticle &tracklep, 
                                                             const xAOD::TrackParticleContainer &inDetTracks,
                                                             const xAOD::Vertex &priVtx);   
    
    bool passElecCand(const xAOD::Electron &elec);
    bool passMuonCand(const xAOD::Muon     &muon);

    std::vector<xAOD::Vertex*> prepLepWithTwoTrkSVVec(const FittingInput &input,
						      const xAOD::TrackParticle *tracklep,
                                                      const std::vector<const xAOD::TrackParticle* > &tracks);

    std::vector<xAOD::Vertex*> prepLepWithMergedSVVec(const FittingInput &input,
						      const xAOD::TrackParticle* tracklep,
						      const std::vector<xAOD::Vertex*> &twoTrk_vertices);

    void makeVertexCluster(std::vector<xAOD::Vertex *> &cluster_vtxs, std::vector<xAOD::Vertex *> &input_vtxs);

    void saveSecondaryVertices(std::vector<xAOD::Vertex *> &vtxs,
                               std::vector<int> &index_vector,
                               std::vector<ElementLink<xAOD::VertexContainer> > &sv_links,
                               xAOD::VertexContainer &SVContainer,
                               std::set< xAOD::Vertex* >& svSet);

  private:

    typedef SG::AuxElement::Decorator<std::vector<int> > decoratorVecInt_t;
    typedef SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::VertexContainer> > > decoratorVecElemVtx_t;

  private:

    //
    // Tools and services: 
    //
    ToolHandle<Prompt::IVertexMergingTool>                 m_vertexMerger;
    ServiceHandle<Prompt::IVertexFittingSvc>               m_vertexFitterSvc;

    //
    // Properties:
    //
    bool                                                   m_printTime;
    bool                                                   m_debug;
    bool                                                   m_debug_merge;
    bool                                                   m_selectTracks;

    double                                                 m_mergeMinVtxDist;
    double                                                 m_mergeChi2OverDoF;

    std::string                                            m_leptonContainerName;
    std::string                                            m_refittedPriVtxContainerName;
    std::string                                            m_svContainerName;

    std::string                                            m_decoratorNameSecVtxLinks;
    std::string                                            m_decoratorNameIndexVector;

    std::string                                            m_linkNameRefittedPriVtxWithoutLepton;
    std::string                                            m_refittedVertexTypeName;


    float                                                  m_minTrackpT;
    float                                                  m_maxTrackEta;
    float                                                  m_maxTrackZ0Sin;

    float                                                  m_minTrackLeptonDR;
    float                                                  m_maxTrackLeptonDR;

    unsigned                                               m_minTrackSilHits;
    float                                                  m_maxTrackSharedSilHits;
    unsigned                                               m_maxTrackSilHoles;
    unsigned                                               m_maxTrackPixHoles;

    //
    // Variables
    //
    TStopwatch                                             m_timerAll;
    TStopwatch                                             m_timerExec;  

    unsigned                                               m_countEvents;

    //
    // Decorators
    //
    std::unique_ptr<decoratorVecInt_t>                     m_indexVectorDec;
    std::unique_ptr<decoratorVecInt_t>                     m_indexVectorDecMerge;
    std::unique_ptr<decoratorVecInt_t>                     m_indexVectorDecDeepMerge;
    std::unique_ptr<decoratorVecElemVtx_t>                 m_lepSVElementLinksDec;
  };
}

#endif // NONPROMPTLEPTONVERTEXINGALG_H
