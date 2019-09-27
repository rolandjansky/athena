// This is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PROMPT_VERTEXFITTINGSVC_H
#define PROMPT_VERTEXFITTINGSVC_H

/**********************************************************************************
 * @Package: LeptonTaggers
 * @Class  : VertexFittingSvc
 * @Author : Fudong He
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Merge the input vertices and output merged vertices.
 * 
 **********************************************************************************/

// Local
#include "IVertexFittingSvc.h"
#include "VarHolder.h"
#include "PromptUtils.h"

// Reconstruction
#include "TrkVertexFitterInterfaces/IVertexFitter.h"

// Athena
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

// xAOD
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

// ROOT
#include "TStopwatch.h"

namespace Prompt
{
  //
  // Vertex Merging Tool
  //
  class VertexFittingSvc: public extends<AthService, Prompt::IVertexFittingSvc>
  {
  public:

    VertexFittingSvc(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~VertexFittingSvc() {}

    virtual StatusCode initialize();

    virtual StatusCode finalize();

    virtual std::pair<xAOD::Vertex*, int> fitVertexWithPrimarySeed(const FittingInput &input,
								   const std::vector<const xAOD::TrackParticle* > &tracks,
								   VtxType vtx);

    virtual std::pair<xAOD::Vertex*, int> fitVertexWithSeed(const FittingInput &input, 
							    const std::vector<const xAOD::TrackParticle* > &tracks,
							    const Amg::Vector3D& seed,
							    VtxType vtxType);

    virtual bool moveVertexPointerToContainer(xAOD::Vertex *vtx, xAOD::VertexContainer &container);

    virtual void resetEvent();

    virtual bool isValidVertex(const xAOD::Vertex *vtx) const;

  private: 

    void removeDoubleEntries(std::vector<const xAOD::TrackParticle*> &tracks);

    bool decorateNewSecondaryVertex(const FittingInput &input, xAOD::Vertex *secVtx);

    xAOD::Vertex* getSecondaryVertexWithSeed(const std::vector<const xAOD::TrackParticle*> &tracks,
					     const xAOD::TrackParticleContainer *inDetTracks,
					     const Amg::Vector3D& seed);
    
  private:

    typedef std::unique_ptr<xAOD::Vertex> vtxPtr_t;

  private:

    //
    // Properties:
    //
    ToolHandle<Trk::IVertexFitter>                         m_vertexFitter;
    ToolHandle<Trk::IVertexFitter>                         m_seedVertexFitter;

    bool                                                   m_debug;
    bool                                                   m_doSeedVertexFit;

    double                                                 m_merge_probcut;
    double                                                 m_merge_chi2cut;
    double                                                 m_merge_distcut;

    std::string                                            m_distToPriVtxName;
    std::string                                            m_normDistToPriVtxName;
    std::string                                            m_distToRefittedPriVtxName;
    std::string                                            m_normDistToRefittedPriVtxName;
    std::string                                            m_distToRefittedRmLepPriVtxName;
    std::string                                            m_normDistToRefittedRmLepPriVtxName;

    //
    // Variables
    //
    TStopwatch                                             m_timer;
    int                                                    m_countNumberOfFits;
    int                                                    m_countNumberOfFitsFailed;
    int                                                    m_countNumberOfFitsInvalid;

    int                                                    m_secondaryVertexIndex;

    std::vector<vtxPtr_t>                                  m_storedVtxPtrs;

    std::unique_ptr<SG::AuxElement::Decorator<float> >     m_distToPriVtx;
    std::unique_ptr<SG::AuxElement::Decorator<float> >     m_normDistToPriVtx;
    std::unique_ptr<SG::AuxElement::Decorator<float> >     m_distToRefittedPriVtx;
    std::unique_ptr<SG::AuxElement::Decorator<float> >     m_normDistToRefittedPriVtx;
    std::unique_ptr<SG::AuxElement::Decorator<float> >     m_distToRefittedRmLepPriVtx;
    std::unique_ptr<SG::AuxElement::Decorator<float> >     m_normDistToRefittedRmLepPriVtx;
    
    std::unique_ptr<SG::AuxElement::Decorator<int> >       m_indexDec;
    std::unique_ptr<SG::AuxElement::Decorator<int> >       m_decoratorType;
  };
}

#endif 
