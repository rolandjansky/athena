/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetTagD3PDMaker/src/JetTagTwoTrackVertexCandidateHolder.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief holder class to add JetFitter two track vertices info to storegate
*/

#ifndef JetTagD3PDMaker_JetTagTwoTrackVertexCandidateHolder_H
#define JetTagD3PDMaker_JetTagTwoTrackVertexCandidateHolder_H


#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Trk{
  class VxCandidate;
  class TrackParticleBase;
}

namespace D3PD{

  class JetTagJetFitterDataToSGAlg;

class JetTagTwoTrackVertexCandidateHolder{ /// all inline

 public:

  ~JetTagTwoTrackVertexCandidateHolder(){};

  void setVxCandidate(const Trk::VxCandidate* a){m_vxCandidate=a;}
  void setNeutralTrack(const Trk::TrackParticleBase* a){m_neutralTrack=a;}

  const Trk::VxCandidate* getVxCandidate() const {return m_vxCandidate;}
  const Trk::TrackParticleBase* getNeutralTrack() const {return m_neutralTrack;}

 private:

  friend class D3PD::JetTagJetFitterDataToSGAlg;

  JetTagTwoTrackVertexCandidateHolder(){m_vxCandidate=0; m_neutralTrack=0;};

  const Trk::VxCandidate* m_vxCandidate;
  const Trk::TrackParticleBase* m_neutralTrack;

};


}/// namespace

typedef DataVector<D3PD::JetTagTwoTrackVertexCandidateHolder> JetTagTwoTrackVertexDataHolder;

CLASS_DEF( JetTagTwoTrackVertexDataHolder , 206721314 , 1 )
//CLASS_DEF( DataVector<D3PD::JetTagTwoTrackVertexCandidateHolder> , 70477947 , 1 )

#endif //JetTagD3PDMaker_JetTagTwoTrackVertexCandidateHolder_H
