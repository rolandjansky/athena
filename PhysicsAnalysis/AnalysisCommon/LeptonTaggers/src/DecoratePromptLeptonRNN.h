// This is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECORATELEPTONTAGGERRNN_H
#define DECORATELEPTONTAGGERRNN_H

/**********************************************************************************
 * @Package: PhysicsAnpProd
 * @Class  : DecoratePromptLeptonRNN
 * @Author : Rustem Ospanov
 * @Author : Fudong He
 *
 * @Brief  : Algorithm to compute RNN score for electrons and muons
 * 
 **********************************************************************************/

// Local
#include "VarHolder.h"
#include "IRNNTool.h"

// ROOT
#include "TStopwatch.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

namespace Prompt
{
  class DecoratePromptLeptonRNN : public AthAlgorithm
  {
    /*
      Select the ID tracks near to the lepton, and prepare the RNN inputs variables of those ID tracks to the VarHolder objects. 
      Then pass the VarHolder objects to RNNTool class to get the RNN prediction.
      Decorate the lepton with the RNN predictions.

    */
  public:
     
    DecoratePromptLeptonRNN(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~DecoratePromptLeptonRNN() {}
    
    StatusCode initialize();
    StatusCode execute(); 
    StatusCode finalize();

  private:

    typedef SG::AuxElement::Decorator<float> decoratorFloat_t;
    typedef std::map<std::string, std::unique_ptr<decoratorFloat_t> > decoratorFloatMap_t;

  private:

    const xAOD::TrackParticle* findMuonTrack(const xAOD::Muon *muon);

    const xAOD::Jet* findClosestTrackJet(const xAOD::TrackParticle *particle, const xAOD::JetContainer &trackJets);

    bool compDummy(const xAOD::IParticle &particle, 
		   const std::string &prefix);
    
    bool prepTrackObject(Prompt::VarHolder         &p,
                         const xAOD::TrackParticle &track,
                         const xAOD::TrackParticle &lepton,
                         const xAOD::Jet           &trackJet,  
                         const xAOD::Vertex        *priVtx,
			 const xAOD::EventInfo     &event); 

    bool compScore(const xAOD::IParticle                &particle, 
                   const std::vector<Prompt::VarHolder> &tracks,
                   const std::string                    &prefix);

    bool passTrack(Prompt::VarHolder &p);

    StatusCode makeHist(TH1 *&h, const std::string &key, int nbin, double xmin, double xmax);

  private:
    
    // Properties:
    std::string                                       m_inputContainerLepton;
    std::string                                       m_inputContainerTrack;
    std::string                                       m_inputContainerTrackJet;
    std::string                                       m_inputContainerPrimaryVertices;

    std::string                                       m_outputStream;    
    std::string                                       m_outputVectorName;
    std::string                                       m_decorationPrefixRNN;

    std::vector<std::string>                          m_configCutsTrack;
    
    double                                            m_minTrackpT;       
    double                                            m_maxTrackEta;
    double                                            m_maxTrackZ0Sin;    
  
    double                                            m_minTrackLeptonDR;
    double                                            m_maxTrackLeptonDR; 
    double                                            m_maxLepTrackJetDR;
  
    double                                            m_maxTrackSharedSilHits;
    unsigned                                          m_minTrackSilHits;  
    unsigned                                          m_maxTrackSilHoles;
    unsigned                                          m_maxTrackPixHoles;

    bool                                              m_debug;
    bool                                              m_printTime;

    // Components and variables:
    ToolHandle<IRNNTool>                              m_toolRNN;

    ServiceHandle<ITHistSvc>                          m_histSvc;

    TStopwatch                                        m_timerEvent;
    int                                               m_countEvent;
    
    decoratorFloatMap_t                               m_decoratorMap;

    std::map<std::string, TH1*>                       m_hists;
    
    std::unique_ptr<SG::AuxElement::ConstAccessor<unsigned char> > m_accessQuality;
  };
}

#endif
