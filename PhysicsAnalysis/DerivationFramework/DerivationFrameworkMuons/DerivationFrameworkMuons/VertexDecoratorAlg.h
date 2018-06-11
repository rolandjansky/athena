/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MuonTPVertexDecoratorAlg__H
#define MuonTPVertexDecoratorAlg__H

#include <AthenaBaseComps/AthAlgorithm.h>
#include <GaudiKernel/ToolHandle.h>
#include <AthContainers/AuxElement.h>
#include <memory>



namespace InDet{
    class IInDetTrackSelectionTool;
}

class VertexDecoratorAlg : public AthAlgorithm { 

public:
  VertexDecoratorAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  ~VertexDecoratorAlg(); 

  /// Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

private: 
    std::string m_VertexContainer;
    std::string m_decorationPrefix;
    
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trkselTool;
    bool m_filterTracks;
    
    std::unique_ptr< SG::AuxElement::Decorator<float> > m_dec_sum_pt;
    std::unique_ptr< SG::AuxElement::Decorator<unsigned int > > m_dec_n_trks;
    
};


#endif //MuonTPJpsiVertexFittingAlg__H
