/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ICANDIDATESEARCH_H
#define ICANDIDATESEARCH_H
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "GaudiKernel/IAlgTool.h"
namespace Analysis {
static const InterfaceID IID_ICandidateSearch("ICandidateSearch", 1, 0);

class ICandidateSearch : virtual public IAlgTool {
public:
   virtual StatusCode performSearch(xAOD::VertexContainer*& , xAOD::VertexAuxContainer*& ) = 0;
   /** AlgTool interface methods */
   static const InterfaceID& interfaceID() { return IID_ICandidateSearch; }
};
}
#endif

