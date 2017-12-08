/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"



#include "TestTools/initGaudi.h"

#include "TrigNavigation/TriggerElement.h"

#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/RoICacheHelper.h"

#include "TestUtils.h"

#include "TestTypes.h"
#include "TestUtils.h"

using namespace HLT;
using namespace TrigNavTest;
HLT::Navigation* hns;

template<class GET, class ATTACH>
class Alg : public RoICacheHelper {
public:
  Alg(const std::string& name, GET get, ATTACH attach) 
    : m_s (name), m_get(get), m_attach(attach) {}
  

  bool execute(TriggerElement* output ) {
    MsgStream log(Athena::getMessageSvc(), "RoICache_test:"+m_s);
    MsgStream *msglog = &log;
    BEGIN_TEST("One algorithm test "+m_s);
    REPORT_REGTEST("executing on " << output->getId());
    if (needsExecution(output)) {
      REPORT_REGTEST( "REAL execution needed on " << output->getId());;
      if ( !startCaching(output)  )
	REPORT_AND_STOP( "start caching failed "  << output->getId() );
     
      // imagine here hard work to work out A and B features
      
      if ( m_get(output).isFailure() ) 
	REPORT_AND_STOP( "get failed" );
      if ( m_attach(output).isFailure() )
	REPORT_AND_STOP( "attach failed" );      
      
      if ( !stopCaching(output) )
	REPORT_AND_STOP("stop Caching failed "  << output->getId() );
    } else {
      REPORT_REGTEST( "CACHED execution needed on "  << output->getId() );
      if ( !cache(output) ){
	REPORT_AND_STOP("The Caching failed "  << output->getId() );
      }
    }
    END_TEST;
  }
private:
  std::string m_s; 
  GET m_get;
  ATTACH m_attach;
};

template<class GET, class ATTACH>
Alg<GET, ATTACH> createAlgo(const std::string& name, GET get, ATTACH attach) {
  return Alg<GET, ATTACH>(name, get, attach);
}



int main() {
  using namespace std;

  MsgStream log(Athena::getMessageSvc(), "RoICache_test");

  MsgStream* msglog = &log;
  BEGIN_TEST("RoIcaching test");

  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("test2.txt",  pSvcLoc)) {
    REPORT_AND_STOP("Can not intit Gaudi");
  }
  assert(pSvcLoc);


  StoreGateSvc* pStore(0);


  if( pSvcLoc->service("StoreGateSvc", pStore, true).isSuccess() ) {
    REPORT_AND_CONTINUE("SG pointer: " << pStore );
  } else {
    REPORT_AND_STOP("SG not available");
  }

  IToolSvc* toolSvc;

  if( pSvcLoc->service("ToolSvc", toolSvc, true).isSuccess()  ) {
    REPORT_AND_CONTINUE("ToolSvc pointer obtained: " << toolSvc );
  } else {
    REPORT_AND_STOP( "ToolSvc not available");
  }


  IAlgTool* algTool;
  if ( toolSvc->retrieveTool("HLT::Navigation/Navigation", algTool).isSuccess() ) {
    REPORT_AND_CONTINUE("OK navigation tool retrieved" );
    hns = dynamic_cast< HLT::Navigation*>(algTool);
    if ( hns ) {
      REPORT_AND_CONTINUE("OK navigation casted" );
    } else {
      REPORT_AND_STOP("navigation not casted" );    
    }
  } else {
    REPORT_AND_STOP( "ERROR navigation tool NOT retrieved" );
  }

  hns->reset();

  
 
  TriggerElement* initial =  hns->getInitialNode();

  // we make an RoI with 3 branches 2 identical and one slightly modified (with different feature)
  // let's make objects first
  
  TriggerElement* roi0 = hns->addRoINode(initial);
  TriggerElement* roi1 = hns->addRoINode(initial);

  std::string key;
  TestA* a0 = new TestA(-34);
  hns->attachFeature(roi0, a0, Navigation::ObjectCreatedByNew, key, "initialRoI");

  TestA* a1 = new TestA(-34);
  hns->attachFeature(roi1, a1, Navigation::ObjectCreatedByNew, key, "initialRoI");

  TriggerElement* thresholdA = hns->addNode(roi0, 1);
  TriggerElement* thresholdB = hns->addNode(roi0, 2);
  TriggerElement* thresholdC = hns->addNode(roi0, 3);
  TriggerElement* thresholdD = hns->addNode(roi0, 4);

  TriggerElement* thresholdA1 = hns->addNode(roi1, 1);


  
  // make next layer
  TriggerElement* clusterA = hns->addNode(thresholdA, 11);
  TriggerElement* clusterB = hns->addNode(thresholdB, 12);
  TriggerElement* clusterC = hns->addNode(thresholdC, 13);
  TriggerElement* clusterD = hns->addNode(thresholdD, 14);

  TriggerElement* clusterA1 = hns->addNode(thresholdA1, 1200);

  REPORT_AND_CONTINUE("Navigation structure built");

  auto calo = createAlgo("calo", 
			 [](TriggerElement* te) -> StatusCode {   // get RoI
			   const TestA* back(0);
			   const TriggerElement* tsource;
			   std::string  lsource;
			   hns->getRecentFeature(te, back, "", tsource, lsource);
			   return StatusCode::SUCCESS;
			 }, 
			 [](TriggerElement* te) -> StatusCode { // produces xAOD like type TestBContainer
			   TestBContainer* a = new TestBContainer;
			   TestAuxB aux;
			   a->setStore(&aux);
			   std::string key;
			   hns->attachFeature(te, a, Navigation::ObjectCreatedByNew, key, "made_by_calo");	     	  
			   return StatusCode::SUCCESS;
			 });
  if( !calo.execute(clusterA) ) ABORT("");
  if( !calo.execute(clusterB) ) ABORT("");  
  if( !calo.execute(clusterC) ) ABORT("");
  if( !calo.execute(clusterD) ) ABORT("");

  if( !calo.execute(clusterA1) ) ABORT("");

  REPORT_AND_CONTINUE("Executed calo on clusters");
    
  // idea here is that only two clusters out of 4 obtain extra object
  // next algo will depend on this extra objects and there shoudl be apropriate caching kicking in only wto times
  auto caloaux = createAlgo("caux", 
			    [](TriggerElement* te) -> StatusCode { // require TestBContainer
			      const TestBContainer* back(0);
			      const TriggerElement* tsource;
			      std::string  lsource;
			      hns->getRecentFeature(te, back, "", tsource, lsource);
			      return StatusCode::SUCCESS; 
			    },
			    [](TriggerElement* te) -> StatusCode {
			      TestA* a = new TestA(-38); 
			      std::string key;
			      hns->attachFeature(te, a, Navigation::ObjectCreatedByNew, key, "refined_by_aux");	     	  
			      return StatusCode::SUCCESS; 
			    }
			    );
  if( !caloaux.execute(clusterC) ) ABORT("");
  if( !caloaux.execute(clusterD) ) ABORT("");



  //log << MSG::INFO << "\n" << *hns << endmsg;

  auto calo2 = createAlgo("calo2",
			  [&](TriggerElement* te) -> StatusCode {   // get RoI
			    const TestA* back(0);
			    const TriggerElement* tsource;
			    std::string  lsource;
			    hns->getRecentFeature(te, back, "", tsource, lsource);
			    REPORT_AND_CONTINUE("got back object " << back->value());
			    return StatusCode::SUCCESS;
			  }, 
			  [](TriggerElement* te) -> StatusCode { // produces xAOD like type TestBContainer
			    TestBContainer* a = new TestBContainer;
			    TestAuxB aux;
			    a->setStore(&aux);
			    std::string key;
			    hns->attachFeature(te, a, Navigation::ObjectCreatedByNew, key, "made_by_calo2");	     	  
			    return StatusCode::SUCCESS;
			  });
  if( !calo2.execute(clusterD) )    ABORT("");
  if( !calo2.execute(clusterC) )    ABORT("");
  if( !calo2.execute(clusterB) )    ABORT("");
  if( !calo2.execute(clusterA) )    ABORT("");

  if( !calo2.execute(clusterA1) )    ABORT("");

  REPORT_AND_CONTINUE("Executed c2 on clusters");



  //log << MSG::INFO << "\n" << *hns << endmsg;

  
  TriggerElement* trackA = hns->addNode(clusterA, 101);
  TriggerElement* trackB = hns->addNode(clusterB, 102);
  TriggerElement* trackC = hns->addNode(clusterC, 103);
  TriggerElement* trackD = hns->addNode(clusterD, 104);


  TriggerElement* trackA1 = hns->addNode(clusterA1, 10100);


  // we expect now high level of caching as we dpend on the common object
  auto trk = createAlgo("trk", 
			[&](TriggerElement* te) -> StatusCode {   // get RoI
			  const TestA* back(0);
			  const TriggerElement* tsource;
			  std::string  lsource;
			  hns->getRecentFeature(te, back, "initialRoI", tsource, lsource);
			  if ( back )  REPORT_AND_CONTINUE("got back object " << back->value());
			  return StatusCode::SUCCESS;
			}, 
			[](TriggerElement* te) -> StatusCode { // produces xAOD like type TestBContainer
			  TestC* a = new TestC(77);
			  std::string key;
			  hns->attachFeature(te, a, Navigation::ObjectCreatedByNew, key, "made_by_trk");	     	  
			  return StatusCode::SUCCESS;
			});
  if( !trk.execute(trackB) )  ABORT("");
  if( !trk.execute(trackD) )  ABORT("");
  if( !trk.execute(trackC) )  ABORT("");
  if( !trk.execute(trackA) )  ABORT("");
  if( !trk.execute(trackA1) ) ABORT("");



  REPORT_AND_CONTINUE("Executed trk");

  // now lets make the TrackA look different by attaching there an object of different type  
  {
    TestC* c = new TestC(55);
    std::string key;
    hns->attachFeature(trackA, c, Navigation::ObjectCreatedByNew, key, "made_by_tr2");	    
  }
  


  
  TriggerElement* eleA = hns->addNode(trackA, 1001);
  TriggerElement* eleB = hns->addNode(trackB, 1002);
  TriggerElement* eleC = hns->addNode(trackC, 1003);
  TriggerElement* eleD = hns->addNode(trackD, 1004);

  TriggerElement* eleA1 = hns->addNode(trackA1, 100100);


  auto matcher = createAlgo("matcher", 
			    [&](TriggerElement* te) -> StatusCode {   // get RoI
			      const TestC* back(0);
			      const TriggerElement* tsource;
			      std::string  lsource;
			      hns->getRecentFeature(te, back, "", tsource, lsource);
			      if ( back ) {
				REPORT_AND_CONTINUE("got back object " << back->value());
			      } else {
				REPORT_AND_CONTINUE("no object retrieved");
			      }
			      return StatusCode::SUCCESS;
			    }, 
			    [](TriggerElement* te) -> StatusCode { // produces xAOD like type TestBContainer
			      TestDContainer* a = new TestDContainer;
			      a->push_back(new TestD(98));
			      std::string key;
			      hns->attachFeature(te, a, Navigation::ObjectCreatedByNew, key, "made_by_ma");	     	  
			      return StatusCode::SUCCESS;
			});
  if( !matcher.execute(eleA) ) ABORT("");
  if( !matcher.execute(eleB) ) ABORT("");
  if( !matcher.execute(eleC) ) ABORT("");
  if( !matcher.execute(eleD) ) ABORT("");
  if( !matcher.execute(eleA1) ) ABORT("");
  REPORT_AND_CONTINUE("Executed ma on electron TEs");
  

  


  // now make another topological TEs and check caching on them
  // A bit of explanation.
  // We make 3 topological TEs.
  // In terms of RoIs used in them they are identical (i.e. both use roi0, roi1 pair)
  // Execution lines leading to eleC and eleD are identical. I.e. all algorithms cached.
  // While execution line leading to eleA is different. We should observe that 
  // there should be caching on ovCA1 and ovDA1 while ovAA1 should always be executed.
  //
  std::vector<TriggerElement*> v;
  v.push_back(eleC);
  v.push_back(eleA1);

  TriggerElement* ovCA1 = hns->addNode(v, 222);
  v[0] = eleD;
  TriggerElement* ovDA1 = hns->addNode(v, 223);
  v[0] = eleA;
  TriggerElement* ovAA1 = hns->addNode(v, 224);

  auto overlap = createAlgo ("overlap",
			     [&](TriggerElement* te) -> StatusCode {   // get obj made by ma
			       const TestDContainer* back(0);
			       const TriggerElement* tsource;
			       std::string  lsource;
			       hns->getRecentFeature(te, back, "", tsource, lsource);
			       return StatusCode::SUCCESS;
			     }, 
			     [](TriggerElement* ) -> StatusCode {
			       return StatusCode::SUCCESS;
			     });
  if ( !overlap.execute(ovCA1) ) ABORT("");
  if ( !overlap.execute(ovDA1) ) ABORT("");
  if ( !overlap.execute(ovAA1) ) ABORT("");


  REPORT_AND_CONTINUE("Executed on topo TEs");
  REPORT_AND_CONTINUE("ALLOK");


  //log << MSG::INFO << *hns << endmsg;
  return 0;
}
