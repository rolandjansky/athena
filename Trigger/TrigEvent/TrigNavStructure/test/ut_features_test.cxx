/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <stdexcept>
#include <stdint.h>

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/EventContext.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "SGTools/TestStore.h"
#include "TestTools/initGaudi.h"

#include "TrigNavStructure/StandaloneNavigation.h"
#include "TrigNavStructure/TypelessHolder.h"
#include "testutils.h"

using namespace std;
using namespace HLT;

struct TestTNS : public StandaloneNavigation {
  BaseHolder* addHolder(class_id_type clid, sub_index_type sub, const std::string& label) {
    auto th = std::make_shared<TypelessHolder>(clid, label, sub);
    MSG("DEBUG","added holder for clid/label/sub: " << clid << "/" << label << "/" << sub);
    TrigHolderStructure& holderstorage = getHolderStorage();
    holderstorage.registerHolder(th);
    return th.get();      
  }
  
  bool addFeature(TriggerElement* te, class_id_type clid, const std::string& label, index_type begin, index_type end) {
    sub_index_type sub = invalid_sub_index;
    TrigHolderStructure& holderstorage = getHolderStorage();
    for ( auto holder: holderstorage.getAllHolders() ) {
      if ( clid == holder->typeClid() and label == holder->label() ) {
	sub = holder->subTypeIndex();
      }
    }

    if ( sub == invalid_sub_index ) 
      return false;    
    te->addFeature(clid, TriggerElement::ObjectIndex(sub, begin, end));    
    return true;
  }

  void dumpHolders() {
    MSG("DEBUG", "dumpHolders");
    TrigHolderStructure& holderstorage = getHolderStorage();
    for (auto holder: holderstorage.getAllHolders()) {
      MSG("DEBUG", "Holder CLID:" << holder->typeClid() << " label: " << holder->label() << " sub: " << holder->subTypeIndex() );
    }
  }


};

static const class_id_type roi_clid = 6171771;
static const class_id_type cluster_clid = 19188792;
static const class_id_type track_clid = 472619874;
static const class_id_type electron_clid = 788776;

bool build(TestTNS& tns) {
  tns.reset();
  tns.getInitialNode();
  TriggerElement* roi1  = tns.addRoINode(tns.getInitialNode());
  TriggerElement* th1  = tns.addNode(roi1, 10);
  TriggerElement* clu1 = tns.addNode(th1, 11);
  TriggerElement* trk1 = tns.addNode(clu1, 12);
  __attribute__((__unused__))
  TriggerElement* el1  = tns.addNode(trk1, 13);



  TriggerElement* roi2 = tns.addRoINode(tns.getInitialNode());  
  TriggerElement* th2  = tns.addNode(roi2, 20);
  TriggerElement* clu2 = tns.addNode(th2, 21);
  TriggerElement* trk2 = tns.addNode(clu2, 22);
  __attribute__((__unused__))
  TriggerElement* el2  = tns.addNode(trk2, 23); 
  el2=0;

  tns.addHolder(roi_clid, 0, "initial");
  tns.addHolder(roi_clid, 1, "secondary");

  tns.addHolder(cluster_clid, 3, "L2ElectronClusters"); // we assume some of other clusters were slimmed out therefore we give it sub==3
  tns.addHolder(track_clid, 0, "L2ElectronTracks"); 
  tns.addHolder(electron_clid, 0, "L2Electrons"); 

  return true;
}

TriggerElement * getById(TestTNS& tns, te_id_type id) {
  std::vector<TriggerElement*> tes;
  tns.getAllOfType(id, tes);
  if ( tes.size() != 1 ) {
    MSG("ERROR", "Number of TEs is not 1 but " << tes.size());
    throw std::runtime_error("assumption of the single TE of given ID failed");
  }
  return tes[0];
}


bool attach(TestTNS& tns) {

  auto roi1 = TrigNavStructure::getDirectSuccessors(tns.getInitialNode())[0];
  if( tns.addFeature(roi1, roi_clid, "initial", 0, 1) == false ) 
    REPORT_AND_STOP("attach to RoI node failed");

  auto roi2 = TrigNavStructure::getDirectSuccessors(tns.getInitialNode())[0];
  if( tns.addFeature(roi2, roi_clid, "initial", 1, 2) == false ) 
    REPORT_AND_STOP("attach to RoI node failed");

  // we used for the test TEs ID they have convention that the for digit labels RoI second labels step of processing
  if ( tns.addFeature(getById(tns, 11), cluster_clid, "L2ElectronClusters", 0, 1) == false )
    REPORT_AND_STOP("Cluster attach failed");

  if ( tns.addFeature(getById(tns, 21), cluster_clid, "L2ElectronClusters", 1, 2) == false )
    REPORT_AND_STOP("Cluster attach failed");

  if ( tns.addFeature(getById(tns, 12), track_clid, "L2ElectronTracks", 0, 10) == false )
    REPORT_AND_STOP("Tracks attach failed");

  if ( tns.addFeature(getById(tns, 22), track_clid, "L2ElectronTracks", 27, 32) == false )
    REPORT_AND_STOP("Tracks attach failed");

  if ( tns.addFeature(getById(tns, 13), electron_clid, "L2Electrons", 0, 10) == false )
    REPORT_AND_STOP("Electrons attach failed");

  if ( tns.addFeature(getById(tns, 23), electron_clid, "L2Electrons", 20, 25) == false )
    REPORT_AND_STOP("Electrons attach failed");

  return true;
}


bool isGoodFeature(const HLT::TriggerElement::FeatureAccessHelper& fea, 
	       class_id_type clid, sub_index_type sub, 
	       index_type begin, index_type end) {
  if ( not fea.valid() )
    REPORT_AND_STOP("Invalid state of Feature");

  if ( clid != invalid_class_id and fea.getCLID() != clid ) 
    REPORT_AND_STOP("Wrong CLID");
  
  if ( sub != invalid_sub_index and fea.getIndex().subTypeIndex() != sub ) 
    REPORT_AND_STOP("Wrong SubTypeIndex");
  
  if ( begin != invalid_index and fea.getIndex().objectsBegin() != begin ) 
    REPORT_AND_STOP("Wrong objectsBegin");
  
  if ( end != invalid_index and fea.getIndex().objectsEnd() != end ) 
    REPORT_AND_STOP("Wrong objectsEnd");
  
  return true;
}



bool getFromExplicitTE(TestTNS& tns) {
  BEGIN_TEST;
  // get from valid TE
  auto fea = tns.getFeature(getById(tns, 11), cluster_clid, 3);
  if ( not isGoodFeature(fea, cluster_clid, 3, 0, 1) )
    REPORT_AND_STOP("Valid request to get clusters fails");
  PROGRESS;

  auto holder = tns.getHolder(fea);
  if ( holder == nullptr ) 
    REPORT_AND_STOP("Could not get the holder for features: " << fea);
  PROGRESS;

  if ( holder->label() != "L2ElectronClusters" ) 
    REPORT_AND_STOP("Holder has wrong label");  
  PROGRESS;

  // get from valid TE neglecting the sub index
  fea = tns.getFeature(getById(tns, 12), track_clid, invalid_sub_index);
  if ( not isGoodFeature(fea, track_clid, 0, 0, 10) )
    REPORT_AND_STOP("Valid request (neglecting sub index) to get tracks fails");
  PROGRESS;

  // requests below shuld be failing
  // wrong subType
  fea = tns.getFeature(getById(tns, 10), cluster_clid, 1);
  if ( fea.valid() )
    REPORT_AND_STOP("Got feature while should obtain nothing using this sub type ID");
  PROGRESS;

  // earlier TE
  fea = tns.getFeature(getById(tns, 10), cluster_clid, 3);
  if ( fea.valid() )
    REPORT_AND_STOP("Got feature while should obtain nothing from this TE");
  PROGRESS;

  // latter TE
  fea = tns.getFeature(getById(tns, 12), cluster_clid, 3);
  if ( fea.valid() )
    REPORT_AND_STOP("Got feature while should obtain nothing from this TE");
  PROGRESS;
  return true;
}



bool getRecursivelyTEbyCLID(TestTNS& tns) {
  BEGIN_TEST;

  const TriggerElement* source {nullptr};

  // check if the recursion goes in correct direction
  auto fea = tns.getFeatureRecursively(getById(tns, 20), cluster_clid,"", source);
  if ( fea.valid() ) 
    REPORT_AND_STOP("Recursion goes in the wrong direction, we should not be able to get cluster from RoI");
  PROGRESS;

  // check if the recursion starts well (i.e. the TE where objects is available should be covered as well)
  fea = tns.getFeatureRecursively(getById(tns, 21), cluster_clid, "", source);
  if ( not isGoodFeature(fea, cluster_clid, 3, 1, 2) ) 
    REPORT_AND_STOP("Recursion does not start properly");
  if ( source != getById(tns, 21) ) 
    REPORT_AND_STOP("In no-recursion case, the source is wrong");  
  PROGRESS;

  // and finally check if the recursion works well (one step down)
  fea = tns.getFeatureRecursively(getById(tns, 23), track_clid, "", source);
  if ( not isGoodFeature(fea, track_clid, 0, 27, 32) ) 
    REPORT_AND_STOP("Recursion does not descend properly");

  if ( source != getById(tns, 22) ) 
    REPORT_AND_STOP("In one step recursion case, the source is wrong");
  PROGRESS;

  // more steps down
  fea = tns.getFeatureRecursively(getById(tns, 23), cluster_clid, "", source);
  if ( not isGoodFeature(fea, cluster_clid, 3, 1, 2) ) 
    REPORT_AND_STOP("Recursion does not descend properly");

  if ( source != getById(tns, 21) ) 
    REPORT_AND_STOP("In more step recursion case, the source is wrong");
  PROGRESS;
  return true;
}


bool getRecursivelyTEbyLabel(TestTNS& tns) {
  BEGIN_TEST;

  const TriggerElement* source {nullptr};
  // first let's try invalid label
  auto fea = tns.getFeatureRecursively(getById(tns, 23), cluster_clid, "invalid", source);
  if ( fea.valid() )
    REPORT_AND_STOP("Feature found irrespectively of incorrect(1) label");
  PROGRESS;

  // some other lable used by another feature
  fea = tns.getFeatureRecursively(getById(tns, 23), cluster_clid, "L2Electrons", source);
  if ( fea.valid() )
    REPORT_AND_STOP("Feature found irrespectively of incorrect(2) label");
  PROGRESS;

  // and finally good case
  fea = tns.getFeatureRecursively(getById(tns, 23), cluster_clid, "L2ElectronClusters", source);
  if ( not isGoodFeature(fea, cluster_clid, 3, 1, 2) )
    REPORT_AND_STOP("Feature not found using label");
  PROGRESS;

  return true;
}



bool ser(TestTNS& tns) {

  std::vector<TriggerElement*> tes;
  tns.getAll(tes, false);  
  MSG("INFO", "Number of TEs before re-serialization " << tes.size() );
  const size_t nTEs = tes.size();
  
  std::vector<uint32_t> data;
  if ( not tns.serialize(data) ) {
    REPORT_AND_STOP("Serialization fails");
  }
  MSG("INFO", "size of serialized navigation is " << data.size());
  MSG("INFO", "elements: " << data[0] << " " << data[1] << " " << data[2] << " " <<data[3]);
  

  tns.reset();

  if ( not tns.deserialize(data) )
    REPORT_AND_STOP("De-serialization fails");

  { // check number of all TEs
    std::vector<TriggerElement*> tes;
    tns.getAll(tes, false);
    MSG("INFO", "Number of TEs after deserialization " << tes.size() );
    if ( tes.size() != nTEs )
      REPORT_AND_STOP("Different number of TEs after re-serialization");
  }
  return true;
}



int main() {

  // initialize Gaudi, SG
  ISvcLocator* pSvcLoc{nullptr};
  assert( Athena_test::initGaudi(pSvcLoc) );
  StoreGateSvc* pSG(nullptr);
  assert( pSvcLoc->service("StoreGateSvc", pSG, true).isSuccess() );

  // Create a context
  IProxyDict* xdict = &*pSG;
  xdict = pSG->hiveProxyDict();
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext(xdict) );
  Gaudi::Hive::setCurrentContext (ctx);

  TestTNS tns;

  if ( build(tns) == false ) 
    ABORT("not strictly a test but pre-conditions, nonetheless fails");

  if ( attach(tns) == false ) 
    ABORT("not strictly a test but pre-conditions, nonetheless fails");
  
  MSG("INFO", "build navigation. test.");

  if ( getFromExplicitTE(tns) == false ) 
    ABORT("basic get failed");

  if ( getRecursivelyTEbyCLID(tns) == false ) 
    ABORT("Get by CLID failed"); 

  if ( getRecursivelyTEbyLabel(tns) == false ) 
    ABORT("Get by label failed"); 

  // Now is the trick, we serialize the navigation and scratch the old content. 
  // Then we deserialize into it the old content and see if the tests still work.
  if ( ser(tns) == false ) 
    ABORT("Issue with serialziation->reset->deserialization cycle");

  MSG("INFO", "tests after serialization, deserialization");
  tns.dumpHolders();
  if ( getFromExplicitTE(tns) == false ) 
    ABORT("basic get failed");

  if ( getRecursivelyTEbyCLID(tns) == false ) 
    ABORT("Get by CLID failed"); 

  if ( getRecursivelyTEbyLabel(tns) == false ) 
    ABORT("Get by label failed"); 

  MSG("OK", "test passed");
  return 0;
}
