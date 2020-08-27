/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)

#include "vector"

#include "TrigNavigation/NavigationInit.h"

//#include "TestTools/initGaudi.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/ClassID.h"


#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "AnalysisTriggerEvent/JetET_ROI.h"
#include "AnalysisTriggerEvent/EnergySum_ROI.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"



#include "TrigDecisionTool/TrigDecisionTool.h"
//#include "TrigDecisionTool/DecisionAccess.h"

using namespace std;

class TrigInDetTrack {
public:
  TrigInDetTrack(int arg=0){a = arg;}
  ~TrigInDetTrack() { std::cerr << " deleting Track: "<< a << std::endl; }
  int a;
};

CLASS_DEF(TrigInDetTrack, 6421, 1)

class TrigInDetTrackCollection : public DataVector<TrigInDetTrack>{};
CLASS_DEF(TrigInDetTrackCollection, 64210, 1)

class TrigCaloCluster {
public:
  TrigCaloCluster(int arg=0){a = arg;}
  ~TrigCaloCluster() { std::cerr << " deleting Cluster: "<< a << std::endl; }
  TrigCaloCluster& operator=(const TrigCaloCluster&) = default;
  int a;
};

class TrigCaloClusterContainer : public DataVector<TrigCaloCluster>{};

CLASS_DEF(TrigCaloCluster, 6422, 1)
CLASS_DEF(TrigCaloClusterContainer, 64220, 1)


void  nevercalled() {
  //HLT::RegisterType<TrigCaloCluster,TrigCaloClusterContainer>::instan();
  //HLT::RegisterType<TrigInDetTrack,TrigInDetTrackCollection>::instan();
}


using namespace Trig;
#if 0
void testFeatureGroup() {
  TrigDecisionTool* tdt(0);
  FeatureContainer f = tdt->features("EF_mu6");

  vector< Feature<TrigInDetTrack> >   tracks = f.get<TrigInDetTrack>();
  vector< Feature<TrigCaloCluster> > clusters = f.get<TrigCaloCluster>();
  
  std::vector<Trig::Combination>::const_iterator fIt;

  for ( fIt = f.getCombinations().begin(); fIt != f.getCombinations().end(); ++fIt ) {
    vector< Feature<TrigCaloCluster> > cCont = fIt->get<TrigCaloCluster>();
    [[maybe_unused]]
    const TrigCaloCluster* cluster = 0;
    if (!cCont.empty())
      cluster = cCont[0];
    
    vector< Feature<TrigInDetTrack> > tCont = fIt->get<TrigInDetTrack>();
    [[maybe_unused]]
    const TrigInDetTrack* track = 0;
    if ( !tCont.empty() ) 
      track = tCont[0];
    
    Feature<TrigCaloCluster> matchingCl = tdt->ancestor<TrigCaloCluster>(tCont[0].te());
    if (!matchingCl.empty()) {}

    tdt->ancestor<Muon_ROI>(tCont[0].te());
    tdt->ancestor<Jet_ROI>(tCont[0].te());
    tdt->ancestor<EmTau_ROI>(tCont[0].te());

    Feature<TrigCaloCluster> a;
    Feature<TrigCaloCluster> b;
    sameObject(a, b);
  }
}
#endif

#if 0
void testFeatureLink() {
  TrigDecisionTool* tdt(0);
  TrigFeatureLink fl;
  tdt->featureLink2Object<TrigRoiDescriptor>(fl);
  tdt->featureLink2ElementLink<TrigRoiDescriptor>(fl);

  tdt->featureLink2Object<MuonFeature>(fl);
  tdt->featureLink2ElementLink<MuonFeature>(fl);
  tdt->featureLink2Object<TrigMuonEF>(fl);
  tdt->featureLink2ElementLink<TrigMuonEF>(fl);

}

#endif

/**/
//////////////////////////////////////////////////////////////////
#endif



int main() {  
  //  FeatureGroupUTest fg_test;
  //  fg_test.testIterate();
  static_assert(std::is_nothrow_move_constructible<Feature<TrigInDetTrack>>::value);
  std::cout << "done" << std::endl;
  return 0;
}

