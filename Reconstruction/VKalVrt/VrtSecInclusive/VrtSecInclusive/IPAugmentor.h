/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// IPAugmentor.h
//
#ifndef _VrtSecInclusive_IPAugmentor_H
#define _VrtSecInclusive_IPAugmentor_H


#include "AthenaBaseComps/AthAlgorithm.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
//
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

// xAOD Classes
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"

#include <vector>
#include <functional>

namespace VKalVrtAthena {
  
  class IPAugmentor : public AthAlgorithm {
  public:
    /** Standard Athena-Algorithm Constructor */
    IPAugmentor(const std::string& name, ISvcLocator* pSvcLocator);
    
    /** Default Destructor */
    ~IPAugmentor();

    virtual StatusCode beginRun()   override;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize()   override;
    virtual StatusCode execute()    override;
   
  private:
    
    /////////////////////////////////////////////////////////
    //
    //  Member Variables
    //
    
    struct JobProperties {
      
      std::string secondaryVerticesContainerName;
      std::string augVerString;
      
      // Additional dressing option
      bool doAugmentDVimpactParametersToMuons;     // potentially useful for DV + muon search
      bool doAugmentDVimpactParametersToElectrons; // potentially useful for analyses involving electrons
      
    };
    
    struct JobProperties m_jp;
    
    /////////////////////////////////////////////////////////
    //
    //  Athena JobOption Properties
    //
    
    ToolHandle <Trk::ITrkVKalVrtFitter>        m_fitSvc;       // VKalVrtFitter tool
    
    using IPDecoratorType = SG::AuxElement::Decorator< std::vector< std::vector<float> > >;
    std::unique_ptr< IPDecoratorType > m_decor_d0_wrtSVs;
    std::unique_ptr< IPDecoratorType > m_decor_z0_wrtSVs;
    std::unique_ptr< IPDecoratorType > m_decor_pt_wrtSVs;
    std::unique_ptr< IPDecoratorType > m_decor_eta_wrtSVs;
    std::unique_ptr< IPDecoratorType > m_decor_phi_wrtSVs;
    std::unique_ptr< IPDecoratorType > m_decor_d0err_wrtSVs;
    std::unique_ptr< IPDecoratorType > m_decor_z0err_wrtSVs;
    
    // for event info to new ntuple (used to go by default in CollectionTree)
    void declareProperties();
    
    template<class LeptonFlavor>
    StatusCode augmentDVimpactParametersToLeptons( const std::string& containerName );
    
  };
  
} // end of namespace bracket


// This header file contains the definition of member templates
#include "details/IPAugmentor.icc"


#endif /* _VrtSecInclusive_IPAugmentor_H */
