/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBJETHYPO_TRIGBJETETHYPOALGMT_H
#define TRIGBJETHYPO_TRIGBJETETHYPOALGMT_H 1

#include "TrigBjetHypoAlgBaseMT.h"

/**
 * @class Implements b-jet selection for the new HLT framework
 * @brief 
 **/

class TrigBjetEtHypoAlgMT : public TrigBjetHypoAlgBaseMT {
 public: 

  TrigBjetEtHypoAlgMT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize();
  virtual StatusCode  execute( const EventContext& context ) const;

 protected:
  TrigBjetEtHypoAlgMT();

  virtual StatusCode attachLinksToDecision( const EventContext&,
                                            TrigCompositeUtils::Decision&,
					    int index,
					    int indexPrmVertex = 0 ) const;
  
 protected:
  ToolHandleArray< TrigBjetEtHypoTool > m_hypoTools {this,"HypoTools",{},"Hypo Tools"};
  
  Gaudi::Property< std::string > m_roiLink {this,"RoILink","Undefined","RoI link to attach to the output decision"};
  Gaudi::Property< std::string > m_prmVtxLink {this,"PrmVtxLink","Undefined","PrmVtx link to attach to the output decision"};

  Gaudi::Property< bool > m_retrieveVertexFromEventView {this,"RetrieveVertexFromEventView",false,"State where the prim vertex has to be retrieved from the Event or from the View context"};
  
  SG::ReadHandleKey< xAOD::JetContainer > m_inputJetsKey {this,"Jets","Undefined","Input Jet Container Key"};
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_inputRoIKey {this,"RoIs","Undefined","Input RoIs that will be linked to the output decision"};
  SG::ReadHandleKey< xAOD::VertexContainer > m_inputPrmVtx {this,"PrmVtx","Undefined","Primary vertex to be linked to the output decision"};
}; 


#endif 
