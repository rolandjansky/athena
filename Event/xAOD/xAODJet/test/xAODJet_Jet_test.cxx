/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG

#include "JetFactory.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

#include "xAODCore/ShallowCopy.h"
#include "CxxUtils/checker_macros.h"

#include <vector>
#include <iostream>
#include <cassert>

////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////

bool is_equal (double x1, double x2)
{
  if (std::abs (x1-x2) / (std::abs(x1+x2)+1e-5) < 1e-5)
    return true;
  std::cout << "Match failure: " << x1 << " " << x2 << "\n";
  return false;
}

#define TEST_MSG(msg) std::cout << msg << std::endl

#define TESTMACRO(failcond, errmsg)                                            \
  do {                                                                         \
    if (!(failcond)) {                                                         \
      TEST_MSG("TEST FAILED : " << errmsg);                                    \
      return 1;                                                                \
    } else {                                                                   \
      TEST_MSG("TEST SUCCEEDED : " << errmsg);                                 \
    }                                                                          \
  } while (0)



////////////////////////////////////////////////////////////////
// Test functions
////////////////////////////////////////////////////////////////



int testJetCreation(){

  TEST_MSG("\n ---------------- testJetCreation  ");
  xAOD::Jet jet;
  jet.makePrivateStore();
  jet.setJetP4(xAOD::JetFourMom_t(1,1,0,3));
  TESTMACRO( is_equal(jet.pt(),1), " created jet pt==1"); // crash or exception if the 2 lines above are ommitted !!

  jet.setAttribute("Width", 3.14);
  TESTMACRO( is_equal(jet.getAttribute<float>(xAOD::JetAttribute::Width), 3.14) , " Moment retrieval by enum");

  jet.setAttribute(xAOD::JetAttribute::Width, 0.123);
  TESTMACRO( is_equal(jet.getAttribute<float>("Width"), 0.123), " Moment retrieval by string");


  ElementLink<xAOD::IParticleContainer> el;
  jet.addConstituent(el);
  const auto & elv = jet.constituentLinks();
  TESTMACRO((elv.size()==1), " constituents size ==1  ->  "<<elv.size() );

  return 0;
}



int testJetCopy ATLAS_NOT_THREAD_SAFE (xAOD::JetContainer& jetCont){

  TEST_MSG("\n ---------------- testJetCopy  ");
  xAOD::Jet* jet0 = jetCont[0];

  //xAOD::Jet copy0(*jet0);
  TEST_MSG( " --> testing assignment operator "<< jet0->getAttribute<float>("Width")  );
  xAOD::Jet copy0; copy0 = *jet0;

  TESTMACRO( is_equal(jet0->pt() ,copy0.pt()) , "copy0 jet pt identical" );
  TESTMACRO( is_equal(jet0->eta() , copy0.eta()), "copy0 jet eta identical" );
  TESTMACRO( is_equal(jet0->e() ,copy0.e()), "copy0 jet e identical" );
  TESTMACRO( is_equal(jet0->getAttribute<float>("Width") , copy0.getAttribute<float>("Width") ), "copy0 jet Width identical" );

  TEST_MSG( " --> testing copy operator" );
  xAOD::Jet copy1(*jet0);
  TESTMACRO( is_equal( jet0->pt() , copy1.pt()), "copy1 jet pt identical" );
  TESTMACRO( is_equal( jet0->e() , copy1.e()), "copy1 jet e identical" );
  TESTMACRO( is_equal( jet0->getAttribute<float>("Width") , copy1.getAttribute<float>("Width")), "copy0 jet Width identical" );


  TEST_MSG( " --> test copy into a container" );
  // here we test that assigning to a jet part of a container works as exepected
  xAOD::JetContainer cont;
  xAOD::JetAuxContainer* aux = new xAOD::JetAuxContainer();
  cont.setStore(aux);
  jet0 = new xAOD::Jet(); cont.push_back(jet0);
  //*jet0 = copy1;
  *jet0 = *(jetCont[0]);
  TESTMACRO( is_equal( jet0->pt() , copy1.pt()), "copy in container : jet pt identical" );
  TESTMACRO( is_equal( jet0->e() , copy1.e()), "copy in container : jet e identical" );
  TESTMACRO( is_equal( jet0->getAttribute<float>("Width") , copy1.getAttribute<float>("Width")), "copy in container : jet Width identical" );


  return 0;

}


int testAttributes ATLAS_NOT_THREAD_SAFE (xAOD::JetContainer& jetCont){

  TEST_MSG("\n ---------------- testJetCopy ");
  const xAOD::Jet* jet = jetCont[0];

  float phi = jet->getAttribute<float>("Width");
  // Test direct call by enum vs string
  TESTMACRO( is_equal(phi , jet->getAttribute<float>(xAOD::JetAttribute::Width) ), "attribute retieval str vs enum" );

  // test retrieve as double
  TESTMACRO( is_equal(phi , jet->getAttribute<double>(xAOD::JetAttribute::Width) ), "attribute as double" );


  // Test get by reference call by enum vs string
  float w1=0,w2=0;
  bool ok1= jet->getAttribute<float>("Width", w1);
  bool ok2= jet->getAttribute<float>(xAOD::JetAttribute::Width, w2);
  TESTMACRO( ok1 , "getAttribute(string) returns true");
  TESTMACRO( ok2 , "getAttribute(enu,) returns true");
  TESTMACRO( (w1==w2) && w1 , "attribute retrieval by reference");
  TESTMACRO( is_equal( w1, w2 ), "attribute retrieval by reference identical" );
  TESTMACRO( is_equal( phi, w1 ), "attribute retrieval by ref & direct retrieval identical " );

  // Test setting attributes
  xAOD::Jet* jet_nc = jetCont[0]  ;
  jet_nc->setAttribute(xAOD::JetAttribute::Width, 123.456) ;
  TESTMACRO( is_equal( 123.456 , jet->getAttribute<float>(xAOD::JetAttribute::Width) ), "set then retrieve attribute" );

  std::vector<float> v = {1. ,2. ,4. };
  jet_nc->setAttribute("TestVect", v);
  TESTMACRO( v == jet->getAttribute<std::vector<float> >("TestVect")  , "retrieved vector identical" );


  // retrieve scale as attribute
  TEST_MSG("  --> retrieve 4-momentum");
  xAOD::JetFourMom_t p4constit = jet->getAttribute< xAOD::JetFourMom_t >("JetConstitScaleMomentum");
  TESTMACRO( p4constit == jet->jetP4(xAOD::JetScale::JetConstitScaleMomentum), "Scale momentum retrieved as attribute identical to scale");
  jet_nc->setAttribute( "MyScale", p4constit*1.2);
  TESTMACRO( (p4constit*1.2) == jet->jetP4("MyScale"), "Scale momentum set as attribute identical when retrived as scale");


  return 0;
}

int testLink ATLAS_NOT_THREAD_SAFE (xAOD::JetContainer& jetCont){
  TEST_MSG("\n ---------------- testLink  ");

  xAOD::Jet* jet = jetCont[0];
  const xAOD::Jet* jetL = jetCont[1];

  jet->setAssociatedObject("MyJet", jetL);
  TESTMACRO( jetL == jet->getAssociatedObject<xAOD::Jet>("MyJet"), "jet retrieved from asso identical");
  TESTMACRO( jetL == jet->getAssociatedObject<xAOD::IParticle>("MyJet"), "jet retrieved  from as IParticle asso identical");
  const xAOD::Jet* jetL2 ;
  TESTMACRO( jet->getAssociatedObject("MyJet",jetL2), "getAssociatedObject(id,o) returned true");
  TESTMACRO( jetL == jetL2 , "getAssociatedObject(id,o) retrieved identical");

  ElementLink<xAOD::IParticleContainer> el = jet->getAttribute< ElementLink<xAOD::IParticleContainer> >("MyJet");
  TESTMACRO( *el == jetL ,"jet retrieved as EL identical");
  // test setting vector of objects :
  std::vector< const xAOD::Jet* > vecJets = {jetL, jetCont[2] };
  jet->setAssociatedObjects("VecJets", vecJets);
  std::vector< const xAOD::Jet* > retrievedJets;
  bool r = jet->getAssociatedObjects( "VecJets", retrievedJets);
  TESTMACRO( r, "retrieval of VecJets done" );
  TESTMACRO( vecJets == retrievedJets , "Retrieved vector jets identical");

  return 0;
}

int testKinematicChange(){
  TEST_MSG("\n ---------------- testKinematicChange  ");
  xAOD::Jet jet;
  jet.makePrivateStore();
  xAOD::JetFourMom_t v(1,1,2,3);
  jet.setJetP4(v);

  double px0 = jet.px();
  double e0 = jet.e();

  jet.setJetP4(v*1.2);

  TESTMACRO( is_equal(1.2*px0, jet.px()) , "px changed as expected" );
  TESTMACRO( is_equal(1.2*e0, jet.e()) , "e changed as expected" );
  TESTMACRO( is_equal(1.2, jet.pt()) , "pt changed as expected" );
  TESTMACRO( is_equal(1.2, jet.jetP4(xAOD::JetScale::JetAssignedScaleMomentum).Pt()) , "Pt from JetAssignedScaleMomentum as expected" );


  jet.setJetP4(xAOD::JetScale::JetConstitScaleMomentum, v*0.8);
  xAOD::JetFourMom_t vconstit = jet.jetP4("JetConstitScaleMomentum");
  TESTMACRO( is_equal(vconstit.Pt(), (v*0.8).Pt() ), "JetConstitScaleMomentum retrieve by string identical" );

  return 0;
}


int testConstituents ATLAS_NOT_THREAD_SAFE (xAOD::JetContainer& jetCont){
  TEST_MSG("\n ---------------- testConstituents  ");
  xAOD::Jet & jet = * jetCont[0];

  // use jets as fake constituents
  xAOD::Jet * c1 = jetCont[3];
  xAOD::Jet * c2 = jetCont[4];

  jet.addConstituent(c1);
  jet.addConstituent(c2,2.2);

  xAOD::JetConstituentVector vec = jet.getConstituents();
  TESTMACRO( (vec.size() == 2) , "size of JetConstituentVector : 2=="<< vec.size() );

  xAOD::JetConstituent jc = vec.at(0);
  TESTMACRO( ( jc.rawConstituent() == c1 ) , " identical constituent retrieved from JetConstituent object");

  TEST_MSG("   Pt    finding scale     uncalconstit scale" );
  TEST_MSG( jet.pt() << "     "<< jet.jetP4(xAOD::JetScale::JetConstitScaleMomentum).Pt() << "    "<< jet.jetP4(xAOD::JetScale::JetEMScaleMomentum).Pt() );

  std::vector<xAOD::JetConstituent> stlVec = vec.asSTLVector();

  TLorentzVector sum;
  int i=0;
  for( auto itr = vec.begin(); itr!= vec.end(); itr++){
    sum+= TLorentzVector( itr->Px(), itr->Py(), itr->Pz(), itr->E());
    TESTMACRO( jetCont[3+i] == (*itr)->rawConstituent() , "identical constituents from JetConstituentVector at "<< i );
    TESTMACRO( is_equal(stlVec[i].pt(), itr->pt() ), " stlVec and constituent Pt identical at  "<< i );
    TESTMACRO( (stlVec[i].rawConstituent() ==itr->rawConstituent() ), " stlVec and constituent raw constit ptr identical at  "<< i );
    i++;
  }
  TESTMACRO( is_equal(sum.E() , c1->e()+c2->e()), "identical constituent sum");
  TESTMACRO( (jet.rawConstituent(0)==c1), "identical 1st  constituent");
  TESTMACRO( (jet.rawConstituent(1)==c2), "identical 2nd  constituent");


  return 0;
}



int testClusterConstituents ATLAS_NOT_THREAD_SAFE (xAOD::JetContainer& jetCont, xAOD::CaloClusterContainer& clustCont){
  TEST_MSG("\n ---------------- testClusterConstituents  ");
  xAOD::Jet & jet1 = * jetCont[1];
  xAOD::Jet & jet2 = * jetCont[2];

  jet1.setConstituentsSignalState( xAOD::UncalibratedJetConstituent) ;
  jet2.setConstituentsSignalState( xAOD::CalibratedJetConstituent) ;

  for( xAOD::CaloCluster* cl : clustCont){
    jet1.addConstituent( cl );
    jet2.addConstituent( cl );
  }

  xAOD::JetConstituentVector vec1 = jet1.getConstituents();
  TEST_MSG( " contit energy  "<< vec1[0]->e() << "  cluster energy "<< clustCont[0]->rawE());
  TESTMACRO( is_equal( vec1[0]->e(), clustCont[0]->rawE() ), "EM cluster constituent at expected scale");
  TESTMACRO( is_equal( (*vec1.begin())->e(), clustCont[0]->rawE() ) , "EM cluster constituent at expected scale (from iterator)");

  xAOD::JetConstituentVector vec2 = jet2.getConstituents();
  TEST_MSG( " contit energy  "<< vec2[0]->e() << "  cluster energy "<< clustCont[0]->e());
  TESTMACRO( is_equal( vec2[0]->e(), clustCont[0]->e() ), "LC cluster constituent at expected scale");
  TESTMACRO( is_equal( (*vec2.begin())->e(), clustCont[0]->e() ) , "LC cluster constituent at expected scale (from iterator)");
  TESTMACRO( is_equal( (*vec2.begin(xAOD::UncalibratedJetConstituent))->e(), clustCont[0]->rawE() ) , "LC cluster constituent at expected scale (from iterator at EM scale)");

  return 0;
}

int testShallowCopy ATLAS_NOT_THREAD_SAFE (xAOD::JetContainer& jetCont){
  TEST_MSG("\n ---------------- testShallowCopy  ");

  xAOD::JetContainer & shallowcopy = *(xAOD:: shallowCopyContainer( jetCont ).first);
  xAOD::Jet * cjet = shallowcopy[0];
  xAOD::Jet * jet = jetCont[0];

  TESTMACRO( is_equal( cjet->pt(), jet->pt() ), " shallow copy pt identical");
  TESTMACRO( cjet->rawConstituent(0) == jet->rawConstituent(0), " first constit identical");

  delete &shallowcopy;
  return 0;

}

void standaloneInit(){


}




////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////
int main ATLAS_NOT_THREAD_SAFE () {
  TEST_MSG("start");
  xAOD::JetContainer& jetCont = xAOD::JetTests::createEmptyJetContainer("TestJetCont");

  assert( testJetCreation() == 0);
  assert( testKinematicChange() == 0 );


  xAOD::JetTests::fillStandardTestJets( jetCont );
  std::cout << " OK filled jets "<< jetCont.size() << std::endl;

  assert( testLink(jetCont) == 0) ;

  assert( testJetCopy(jetCont) == 0);
  assert( testConstituents(jetCont) == 0);
  assert( testAttributes(jetCont) == 0) ;

  xAOD::JetContainer& jetCont2 = xAOD::JetTests::createEmptyJetContainer("TestJetCont2");
  xAOD::JetTests::fillStandardTestJets( jetCont2 );
  std::cout << " OK filled jets2 "<< jetCont2.size() << std::endl;


  xAOD::CaloClusterContainer& clustCont = xAOD::JetTests::createEmptyCaloClusterContainer("TestClustForJets");
  xAOD::JetTests::fillStandardTestClusters(clustCont);

  assert( testClusterConstituents(jetCont, clustCont)== 0 );

  assert( testShallowCopy(jetCont)== 0 );

  xAOD::JetTests::shutDown();
  return 0;
}
