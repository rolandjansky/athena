// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TestEDM_h
#define ViewAlgs_TestEDM_h


#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"


typedef xAOD::TrigComposite TestCluster;
typedef xAOD::TrigCompositeContainer TestClusterContainer;
typedef xAOD::TrigCompositeAuxContainer TestClusterAuxContainer;
namespace TestEDM {
  void setClusterEt(TestCluster*, float et);
  float getClusterEt(const TestCluster*);
  
  void setClusterEta(TestCluster*, float et);
  float getClusterEta(const TestCluster*);
  
  void setClusterPhi(TestCluster*, float et);
  float getClusterPhi(const TestCluster*);

  void copyCluster(const TestCluster* from , TestCluster* to);

}


typedef xAOD::TrigComposite TestTrack;
typedef xAOD::TrigCompositeContainer TestTrackContainer;
typedef xAOD::TrigCompositeAuxContainer TestTrackAuxContainer;
namespace TestEDM {
  void setTrackPt(TestTrack*, float pt);
  float getTrackPt(const TestTrack*);
  
  void setTrackEta(TestTrack*, float eta);
  float getTrackEta(const TestTrack*);
  
  void setTrackPhi(TestTrack*, float phi);
  float getTrackPhi(const TestTrack*);

  void copyTrack(const TestTrack* from , TestTrack* to);
}




template<class T>
class DecoratedStatusCode {
public:
  DecoratedStatusCode(const StatusCode sc, const T& c)
    : m_sc(sc), m_content(c) {}
  StatusCode sc() const { return m_sc; }
  const T& content() const { return m_content; }
private:
  StatusCode m_sc;
  T m_content;
};


#endif


