// This is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRIMARYVERTEXREFITTER_H
#define PRIMARYVERTEXREFITTER_H 

/**********************************************************************************
 * @Package: LeptonTaggers
 * @Class  : PrimaryVertexReFitter
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
#include "PromptUtils.h"
#include "IVertexFittingSvc.h"

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"

// xAOD
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODBTagging/SecVtxHelper.h"

// ROOT
#include "TStopwatch.h"
#include "TMath.h"

namespace Prompt
{
  class PrimaryVertexReFitter: public AthAlgorithm
  {
  public:

    PrimaryVertexReFitter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PrimaryVertexReFitter() {}
       
    StatusCode initialize();
    StatusCode execute(); 
    StatusCode finalize();
 
  private:
    
    bool decorateLepWithReFitPrimaryVertex(const FittingInput &input,
					   const xAOD::TrackParticle* tracklep,
                                           const xAOD::IParticle *lep,
                                           const std::vector<const xAOD::TrackParticle*> &tracks,
                                           xAOD::VertexContainer &refitVtxContainer);

  private:

    typedef SG::AuxElement::Decorator<float>                               decoratorFloat_t;
    typedef SG::AuxElement::Decorator<ElementLink<xAOD::VertexContainer> > decoratorElemVtx_t;

  private:

    //
    // Tools and services: 
    //
    ServiceHandle<Prompt::IVertexFittingSvc>               m_vertexFitterSvc;

    //
    // Properties:
    //
    bool                                                   m_printTime;
    bool                                                   m_debug;

    std::string                                            m_leptonContainerName;
    std::string                                            m_primaryVertexContainerName;
    std::string                                            m_reFitPrimaryVertexName;

    std::string                                            m_distToRefittedPriVtxName;
    std::string                                            m_normDistToRefittedPriVtxName;
    std::string                                            m_lepVtxLinkName;
    std::string                                            m_lepRefittedVtxWithoutLeptonLinkName;

    TStopwatch                                             m_timerAll;
    TStopwatch                                             m_timerExec;  

    //
    // Decorators
    //
    std::unique_ptr<decoratorFloat_t>                      m_distToRefittedPriVtx;
    std::unique_ptr<decoratorFloat_t>                      m_normdistToRefittedPriVtx;
    std::unique_ptr<decoratorElemVtx_t>                    m_lepRefittedRMVtxLinkDec;
  };
}

#endif // NONPROMPTLEPTONVERTEXINGALG_H
