/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagVxOnJetAxisCandidateHolder.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief holder class to add JetFitter VxOnJetAxis info to storegate
*/

#ifndef JetTagD3PDMaker_JetTagVxOnJetAxisCandidateHolder_H
#define JetTagD3PDMaker_JetTagVxOnJetAxisCandidateHolder_H


#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Trk{
  class VxJetCandidate;
  class VxVertexOnJetAxis;
}

namespace D3PD{

  class JetTagJetFitterDataToSGAlg;

 class JetTagVxOnJetAxisCandidateHolder{ /// all inline

 public:

  ~JetTagVxOnJetAxisCandidateHolder(){};

  void setVxJetCandidate(const Trk::VxJetCandidate* a){m_vxJetCandidate=a;}
  void setVxVertexOnJetAxis(const Trk::VxVertexOnJetAxis* a){m_vxVertexOnJetAxis=a;}

  const Trk::VxJetCandidate* getVxJetCandidate() const {return m_vxJetCandidate;}
  const Trk::VxVertexOnJetAxis* getVxVertexOnJetAxis() const {return m_vxVertexOnJetAxis;}

 private:

  friend class D3PD::JetTagJetFitterDataToSGAlg;

  JetTagVxOnJetAxisCandidateHolder(){m_vxJetCandidate=0; m_vxVertexOnJetAxis=0;};

  const Trk::VxJetCandidate* m_vxJetCandidate;
  const Trk::VxVertexOnJetAxis* m_vxVertexOnJetAxis;

};


}/// namespace

typedef DataVector<D3PD::JetTagVxOnJetAxisCandidateHolder> JetTagVxOnJetAxisDataHolder;

CLASS_DEF( JetTagVxOnJetAxisDataHolder , 204329421 , 1 )
//CLASS_DEF( DataVector<D3PD::JetTagVxOnJetAxisCandidateHolder> , 70477947 , 1 )

#endif //JetTagD3PDMaker_JetTagVxOnJetAxisCandidateHolder_H
