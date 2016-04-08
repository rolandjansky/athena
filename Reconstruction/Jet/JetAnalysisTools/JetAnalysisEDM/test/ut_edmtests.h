// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <vector>

#include "JetAnalysisEDM/ParticleContainer.h"


#include "TFile.h"
#include "TChain.h"



namespace JetAnalysisEDM {

struct TestEDM {

  TestEDM(std::string n): m_chain(), m_ofile(NULL), m_otree(NULL), m_testname(n){}
  virtual ~TestEDM();
  virtual void init();
  virtual bool testEvent(){return true;};
  virtual bool fullTest();
  virtual bool i_fullTest(){return false;};

  virtual void setupOutTree();
  
  template<typename T>
  void WriteOut(T& o){ setupOutTree(); o.writeTo(m_otree); }

  void WriteOut(Particle& o, const std::string & s){ setupOutTree(); o.writeTo(m_otree, s); }

  TChain m_chain;
  
  TFile* m_ofile;
  TTree* m_otree;

  std::string m_testname;
  Long64_t m_master;
  int eventN;
};




#define DeclareTestClass2(TestName, other) struct TestName :public TestEDM { \
    TestName(std::string n= #TestName): TestEDM(n)  {};          \
  virtual bool i_fullTest(); \
  other } 
    

  DeclareTestClass2( TwoVarHandleTest, );

  DeclareTestClass2( TwoVarHandleWriteTest, );

  DeclareTestClass2( VecVecVarHandleTest, );

  DeclareTestClass2( ContainerVsVarHTest, );

  DeclareTestClass2( TwoEventInfoTest, ) ;

  DeclareTestClass2( StealVarTest, );

  DeclareTestClass2( ParticleGetVsVarHTest, );

  DeclareTestClass2( AccessorVsVarHTest, );

  DeclareTestClass2( ViewContainerTest, );

  DeclareTestClass2( ViewContainerCloneTest, );

  DeclareTestClass2( SinglePartTest, );

  DeclareTestClass2( ContainerModifTest, );

  DeclareTestClass2( ContainerIteratorTest, );

  DeclareTestClass2( DummyResultTest, );

  DeclareTestClass2( ParticleAttributeTest,  template<typename T> bool setTest(const ParticleContainer & jetCont, ParticleContainer& jetView);) ;

  DeclareTestClass2( ConstituentsTest, );

  DeclareTestClass2( KinematicVarTest, );

  DeclareTestClass2( SignalStateTest, );

  DeclareTestClass2( KinematicUtilTest, );

  DeclareTestClass2( SelectorTest, );

  DeclareTestClass2( PtEtaPhiMTest, );

  DeclareTestClass2( LoadEntryTest, );

  DeclareTestClass2( SetKinematicTest, );

  DeclareTestClass2( SwapTest, );

  DeclareTestClass2( ListAttributesTest, );

  DeclareTestClass2( RecreateInEventTest, bool process();  );

  DeclareTestClass2( EventManagerTest, );

}
