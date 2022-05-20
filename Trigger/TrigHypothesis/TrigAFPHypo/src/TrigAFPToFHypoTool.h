
/*
Copyright! (C) 2002-2012 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGAFPHYPO_TRIGAFPTOFHYPOTOOL_H
#define TRIGAFPHYPO_TRIGAFPTOFHYPOTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthCheckedComponent.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODForward/AFPVertexContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODForward/AFPVertex.h"
#include "xAODTracking/Vertex.h"
#include "Gaudi/Property.h"


class TrigAFPToFHypoTool : public AthCheckedComponent<AthAlgTool>
{
 public:
  TrigAFPToFHypoTool(const std::string &type, const std::string &name, const IInterface *parent);

  virtual StatusCode initialize() override;
  
  struct AFPToFHypoToolInfo // data structure to be passed to the hypo tool
  {  
   AFPToFHypoToolInfo(const TrigCompositeUtils::DecisionIDContainer a, const xAOD::AFPVertex* b, const xAOD::Vertex* c, TrigCompositeUtils::Decision *d) :
   inputPassingChains(a), afpVtx(b), prmVtx(c), outputDecision(d)
   {}
   const TrigCompositeUtils::DecisionIDContainer inputPassingChains;    
   const xAOD::AFPVertex* afpVtx;
   const xAOD::Vertex* prmVtx;
   TrigCompositeUtils::Decision* outputDecision;
  };

  StatusCode decide( std::vector<AFPToFHypoToolInfo>& inputs ) const;

 private:

  HLT::Identifier m_decisionId;
  Gaudi::Property<float> m_deltaZCut{this, "deltaZCut", 3.5, "Require the difference between the dijet and AFP vertex to be smaller than this value [mm]"};
  Gaudi::Property<bool> m_acceptAll{this, "AcceptAll", false, "Accept all events"};
};

#endif //> !TRIGAFPHYPO_TRIGAFPTOFHYPOTOOL_H
