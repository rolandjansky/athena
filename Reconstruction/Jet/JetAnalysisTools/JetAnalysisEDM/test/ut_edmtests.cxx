/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <typeinfo>

#define private public
#define protected public
#include "ut_edmtests.h"

#include  "JetAnalysisEDM/EventObject.h"
#include  "JetAnalysisEDM/KinematicUtils.h"
#include  "JetAnalysisEDM/EventManager.h"

#include  "JetAnalysisEDM/ConstituentIterator.h"

using std::cout;
using std::endl;


// #define EVENTLOOP      int eventN;                         
//   for( eventN = 0; eventN < m_chain.GetEntries(); ++eventN ) 

#define STARTEVENTLOOP      for( eventN = 0; eventN < m_chain.GetEntries(); ++eventN ) { \
  m_master = m_chain.LoadTree(eventN);

#define ENDEVENTLOOP       if(m_otree) m_otree->Fill() ;    }


#define CHECK( Test, errorMsg)       if( ! Test ) {  \
  cout<< "Test "<< m_testname << " Failed at event "<< eventN << " / "<< m_master << " | Error message = "<< errorMsg <<endl; \
  return false; } 
 

//const char* c_inputFile = "/home/delsart/analysis/topAnalisis/files/topBoosted_only5.root";
const char* c_inputFile = "../../TestFiles/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.merge.NTUP_COMMON.e1513_s1499_s1504_r3658_r3549_p1562_only5.root";

namespace JetAnalysisEDM {

  bool  floatEqual(float v1, float v2){ return fabs(v1-v2)<0.01;}
  bool  floatEqualGeV(float v1, float v2){ return fabs(v1-v2)<0.5;}

  void TestEDM::init(){
    
    m_chain.SetName( "physics");
    m_chain.AddFile(c_inputFile);
    m_chain.AddFile(c_inputFile);

  
  }

  void TestEDM::setupOutTree(){
    if(m_ofile) return;
    m_ofile = TFile::Open( (m_testname+"_out.root").c_str(), "RECREATE" );
    m_ofile->cd();
    // Create an output tree:
    m_otree = new TTree( "bla", "Selected MuctpiD3PD tree" );
    m_otree->SetAutoSave( 100000000 );
    TTree::SetBranchStyle( 1 );
    m_otree->SetDirectory( m_ofile );
  }


  bool TestEDM::fullTest(){
    TestEDM::init();
    bool ok = i_fullTest();
    if(ok)     cout << m_testname << " ok "<< endl;
    return ok;
  }

  TestEDM::~TestEDM(){
    if(m_otree) {m_otree->Write(); delete m_otree;}
    if(m_ofile) m_ofile->Close();
    if(m_ofile) delete m_ofile;
  }

  // *********************************************************
  // *********************************************************

  


  bool TwoEventInfoTest::i_fullTest(){    
    
    EventObject event(m_master, &m_chain);
    EventObject event2(m_master, &m_chain);

    event.declareOutputVar<float>("actualIntPerXing");
    event.declareOutputVar<int>("cl_n");
    WriteOut(event);

    STARTEVENTLOOP {

      CHECK ( (event.RunNumber() == event2.RunNumber()) &&
              (event.EventNumber() == event2.EventNumber()) , " runnumber1="<< event.RunNumber() << "  runnumber2=" << event2.RunNumber());      
      CHECK( (event2.RunNumber() == event.RunNumber() ), " runnumber = "<< event.RunNumber() << "  event.runnumber ="<< event.RunNumber() );
      CHECK( (event.get<int>("el_n") > 0 ) , " el_n == 0 ");

      CHECK( (event.get< vector<float> & >("el_pt").size() > 0 ) , " el_pt.size() == 0 ");

      event.readAllActive();
      ENDEVENTLOOP;
    }
    return true;
  }
  // *********************************************************

  bool TwoVarHandleTest::i_fullTest(){
    VarHandle< std::vector<float> > jet_pt("jet_AntiKt10LCTopo_pt",&m_master, &m_chain);
    VarHandle< std::vector<float> > jet_pt2; 
    VarHandle< std::vector<float> > jet_phi; 

    //jet_pt.setParameters("jet_AntiKt10LCTopo_pt",&m_master, &m_chain);
    jet_pt2.setParameters("jet_AntiKt10LCTopo_pt",&m_master, &m_chain);
    jet_phi.setParameters("jet_AntiKt10LCTopo_phi",&m_master, &m_chain);
    float lastPt=-1;
    STARTEVENTLOOP {

      size_t n= jet_pt().size();
      size_t n2= jet_pt2().size();
      vector<float>& pt =  jet_pt();
      vector<float>& pt2 =  jet_pt2();
      CHECK( (n == n2) , " jet_pt sizes "<< n << " / "<< n2);
      for(size_t i= 0; i<n;i++){
        CHECK( pt[i] == pt2[i] , "At"<< i<< " pt differs "<< pt[i] << " / "<< pt2[i] ) ;
      }
      if( n>0 ) {CHECK( (pt[0] != lastPt) , " same pt on different events "<< lastPt); lastPt = pt[0]; }
      
      ENDEVENTLOOP;
    }
    return true;
  }
  // *********************************************************


  bool TwoVarHandleWriteTest::i_fullTest(){
    VarHandle< std::vector<float> > jet_pt; 
    VarHandle< std::vector<float> > jet_pt2; 

    jet_pt.setParameters("jet_AntiKt10LCTopo_pt",&m_master, &m_chain);
    jet_pt2.setParameters("myjet_pt",&m_master);
    jet_pt.setOutputVar();     WriteOut(jet_pt);
    jet_pt2.setOutputVar();    WriteOut(jet_pt2);

    STARTEVENTLOOP {

      vector<float>& pt =  jet_pt(); 
      vector<float>& pt2 =  jet_pt2();
      pt2 = pt;

      ENDEVENTLOOP;
    }
    return true;
  }
  // *********************************************************


  bool VecVecVarHandleTest::i_fullTest(){
    VarHandle< std::vector<float> > jet_pt("jet_AntiKt10LCTopo_pt",&m_master, &m_chain);
    VarHandle< std::vector< std::vector<int> > > jet_const_index; 
    jet_const_index.setParameters("jet_AntiKt10LCTopo_constit_index",&m_master, &m_chain);

    WriteOut(jet_pt);
    jet_const_index.setOutputVar(); WriteOut( jet_const_index);



    STARTEVENTLOOP {
      vector<float>& pt =  jet_pt(); 
      vector<vector<int> >&  index =  jet_const_index();
      size_t n= pt.size();
      size_t n2= index.size();
      CHECK( (n == n2) , " jet_pt sizes "<< n << " / "<< n2);
      for(size_t i= 0; i<n;i++){
        CHECK( ( !index[i].empty()) , "At"<< i<< " constit vector empty" ) ;
    }

      ENDEVENTLOOP;
    }
    return true;
  }
  // *********************************************************


  bool ContainerVsVarHTest::i_fullTest(){
    VarHandle< std::vector<float> > jet_pt; 
    jet_pt.setParameters("jet_AntiKt10LCTopo_pt",&m_master, &m_chain);
    VarHandle< vector<vector<float> > > jet_elec_X;
    jet_elec_X.setParameters("jet_AntiKt10LCTopo_Electron_X",&m_master, &m_chain);

    ParticleContainer jetCont("jet_AntiKt10LCTopo_", m_master);
    jetCont.readFrom( &m_chain );

    jetCont.setOutputVar(true, "^c.*");    
    WriteOut(jetCont);
    
    STARTEVENTLOOP {
      size_t n= jet_pt().size();
      size_t n2= jetCont.size();
      vector<float>& pt =  jet_pt();
      vector<vector<float> > & elec_X = jet_elec_X();
            
      CHECK( (n == n2) , " jet_pt sizes "<< n << " / "<< n2);
      for(size_t i= 0; i<n;i++){
        const vector<float> &thisj_elex_X  = jetCont[i]->get< vector<float>& >("Electron_X");        
        CHECK( pt[i] == jetCont[i]->pt() , "At"<< i<< " pt differs "<< pt[i] << " / "<< jetCont[i]->pt() ) ;
        CHECK( (thisj_elex_X.size() == elec_X[i].size()) , " At "<< i << "  elec_X size differs : "<< elec_X[i].size() << thisj_elex_X.size() );
        if( !thisj_elex_X.empty()) {
          CHECK( ( thisj_elex_X[0] == elec_X[i][0] ) , " At "<< i << "  elec_X[0] differs : "<< elec_X[i][0] << thisj_elex_X[0] );
        }
      }
      jetCont.readAllActive();

      ENDEVENTLOOP;
    }
    return true;
  }
  // *********************************************************


  bool ParticleGetVsVarHTest::i_fullTest(){
    VarHandle< std::vector<float> > jet_emscale_pt("jet_AntiKt10LCTopo_emscale_pt",&m_master, &m_chain);
    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);
    jetCont.readFrom(&m_chain);


    STARTEVENTLOOP {
      size_t n= jetCont.size();
      vector<float>& emscale_pt =  jet_emscale_pt();

      for(size_t i= 0; i<n;i++){
        CHECK( (emscale_pt[i] == jetCont[i]->get<float>("emscale_pt")) , "At "<< i<< " emscale_pt var differs from get "<< emscale_pt[i]<< " / "<< jetCont[i]->get<float>("emscale_pt")  );
      }
      ENDEVENTLOOP ;
    }
        return true;
  }
  // *********************************************************


  bool AccessorVsVarHTest::i_fullTest(){
        //VarHandle< std::vector<float> > jet_emscale_pt("jet_AntiKt10LCTopo_emscale_pt",&m_master, &m_chain);
    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);
    jetCont.readFrom(&m_chain);

    ParticleAccessor<float> acc_emscale_pt = jetCont.accessor<float>("emscale_pt");
    

    STARTEVENTLOOP {
            size_t n= jetCont.size();
      //vector<float>& emscale_pt =  jet_emscale_pt();

      for(size_t i= 0; i<n;i++){
        const Particle * jet = jetCont[i];
        //acc_emscale_pt[jet] = 3.4; SHOULD NOT compile
        CHECK( (acc_emscale_pt[jet] == jet->get<float>("emscale_pt")) , "At "<< i<< " emscale_pt var differs from acc "<< acc_emscale_pt[jet]<< " / "<< jet->get<float>("emscale_pt")  );
      }
      ENDEVENTLOOP ;
    }
        return true;
  }
  // *********************************************************


  bool ViewContainerTest::i_fullTest(){
    //VarHandle< std::vector<float> > jet_emscale_pt("jet_AntiKt10LCTopo_emscale_pt",&m_master, &m_chain);
    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);
    jetCont.readFrom(&m_chain);

    ParticleContainer jetView("myjet_", m_master);
    jetView.setSubsetOfInputContainer( &jetCont);
    jetView.declareOutputVar<float>("emscale_pt");
    jetView.declareOutputVarAsIn(&jetCont, "^constscale.*");
    jetView.declareOutputVar<vector<float> >("vect3");
    jetView.declareOutputVar<float >("Tau2");
    
    ParticleAccessor<float> jetView_tau1 = jetView.accessor<float>("Tau1");
    ParticleAccessor<float> jetView_constscale_pt = jetView.accessor<float>("constscale_pt");
    
    WriteOut(jetView);


    STARTEVENTLOOP {
      CHECK( ( jetView_tau1.m_accessType == &ParticleAccessor<float>::fromPartSourceIndex), " jetView_tau1 not accessor from fromPartSourceIndex");
      CHECK( (jetView_constscale_pt.m_accessType == &ParticleAccessor<float>::fromPartIndex), " jetView_constscale_pt not accessor from fromPartIndex");
      
      size_t n= jetCont.size();
      n = n > 3 ? 3:n; 

      for(size_t i= 0; 2*i<n;i++){
        const Particle * jet = jetCont[2*i];
        cout << jetView.addNewFromSource( jet ) << " __ "<<endl;
      }
      for(size_t i= 0; i<jetView.size();i++){
        const Particle * jet = jetCont[2*i];
        Particle * jetv = jetView[i];

        cout << jetv<< endl;
        //CHECK( (jet->pt() == jetv->pt()) , "At "<< i<< " pt var differs from jet "<< jet->pt() << " / view= "<< jetv->pt()  );
        CHECK( floatEqual(jet->pt() ,jetv->pt()) , "At "<< i<< " pt var differs from jet "<< jet->pt() << " / view= "<< jetv->pt()  );
        CHECK( (jet->get<float>("Tau1") == jetv->get<float>("Tau1") ) , "At "<< i<< " Tau1 var differs from jet "<< jet->get<float>("Tau1") << " / view= "<< jetv->get<float>("Tau1")  );        
        CHECK( (jetView_tau1[jetv] == jet->get<float>("Tau1") ) , "At "<< i<< " Tau1 var differs from jet "<< jet->get<float>("Tau1") << " / accessor view= "<< jetView_tau1[jetv]  );        
        //cout<< jet->get<float>("constscale_pt") << " part index "<< jetv->GetIndex()<< endl;
        //cout<< jetView_constscale_pt[jetv] <<endl;

        CHECK( (jetView_constscale_pt[jetv] == jet->get<float>("constscale_pt") ) , "At "<< i<< " constscale_pt var differs from jet "<< jet->get<float>("constscale_pt") << " / accessor view= "<< jetView_constscale_pt[jetv]  );        


        jetv->set<TVector3>("vect3", TVector3(jetv->get<float>("Tau2"),0,0));
        jetv->set<LorentzVector_t>("vect4", LorentzVector_t(jetv->get<float>("Tau2"),0,0,3.));

      }
      ENDEVENTLOOP ;
    }
        return true;
  }
  // *********************************************************


  bool ViewContainerCloneTest::i_fullTest(){
        //VarHandle< std::vector<float> > jet_emscale_pt("jet_AntiKt10LCTopo_emscale_pt",&m_master, &m_chain);
    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);
    jetCont.readFrom(&m_chain);

    ParticleContainer jetView("myjet_", m_master);
    jetView.setSubsetOfInputContainer( &jetCont);
    jetView.declareOutputVar<float>("constscale_pt");

    
    ParticleAccessor<float> jetView_tau1 = jetView.accessor<float>("Tau1");
    ParticleAccessor<float> jetView_constscale_pt = jetView.accessor<float>("constscale_pt");

    ParticleContainer jetContSA( "myjet2_", m_master);
    jetContSA.declareOutputVar<float>("constscale_pt");
    ParticleAccessor<float> jetContSA_tau1 = jetContSA.accessor<float>("Tau1");
    ParticleAccessor<float> jetContSA_constscale_pt = jetContSA.accessor<float>("constscale_pt");

    
    WriteOut(jetView);
    //WriteOut(jetContSA);

    STARTEVENTLOOP {

      CHECK( ( jetContSA_tau1.m_accessType == &ParticleAccessor<float>::fromPartGet), " jetContSA_tau1 not accessor from fromPartGet");
      CHECK( (jetContSA_constscale_pt.m_accessType == &ParticleAccessor<float>::fromPartIndex), " jetContSA_constscale_pt not accessor from fromPartIndex");

      size_t n= jetCont.size();

      for(size_t i= 0; i<n;i++){
        const Particle * jet = jetCont[i];
        Particle* j1 = jet->clone();
        jetView.Add( j1 );
        Particle* j2 = jet->clone();
        jetContSA.Add( j2 );        
      }
      for(size_t i= 0; i<n;i++){
        const Particle * jet = jetCont[i];
        Particle * jetv = jetView[i];
        Particle * jetsa = jetContSA[i];

        CHECK( (jet->pt() == jetv->pt()) , "At "<< i<< " pt var differs from jet "<< jet->pt() << " / view= "<< jetv->pt()  );
        CHECK( (jet->pt() == jetsa->pt()) , "At "<< i<< " pt var differs from jet "<< jet->pt() << " / sa= "<< jetv->pt()  );
        CHECK( (jet->get<float>("Tau1") == jetv->get<float>("Tau1") ) , "At "<< i<< " Tau1 var differs from jet "<< jet->get<float>("Tau1") << " / view= "<< jetv->get<float>("Tau1")  );        
        CHECK( (jetView_tau1[jetv] == jet->get<float>("Tau1") ) , "At "<< i<< " Tau1 var differs from jet "<< jet->get<float>("Tau1") << " / accessor view= "<< jetView_tau1[jetv]  );        
        CHECK( (jetView_constscale_pt[jetv] == jet->get<float>("constscale_pt") ) , "At "<< i<< " constscale_pt var differs from jet "<< jet->get<float>("constscale_pt") << " / accessor view= "<< jetView_constscale_pt[jetv]  );        


                 
        CHECK( (jet->get<float>("Tau1") == jetsa->get<float>("Tau1") ) , "At "<< i<< " Tau1 var differs from jet "<< jet->get<float>("Tau1") << " / sa= "<< jetsa->get<float>("Tau1")  );        
        CHECK( (jetContSA_tau1[jetsa] == jet->get<float>("Tau1") ) , "At "<< i<< " Tau1 var differs from jet "<< jet->get<float>("Tau1") << " / accessor sa = "<< jetView_tau1[jetsa]  );        
        CHECK( (jetContSA_constscale_pt[jetsa] == jet->get<float>("constscale_pt") ) , "At "<< i<< " constscale_pt var differs from jet "<< jet->get<float>("constscale_pt") << " / accessor sa= "<< jetView_constscale_pt[jetsa]  );        

      }

      ENDEVENTLOOP ;
    }
        return true;
  }
  // *********************************************************



  bool SinglePartTest::i_fullTest(){
    
    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);
    jetCont.readFrom(&m_chain);

    Particle firstJet;

    WriteOut(firstJet, "singlejet_");

    STARTEVENTLOOP {
      
      const Particle* jet = jetCont[0];
      firstJet = *jet; // copy 4 momentum and link
      //cout << jet->pt() << "   fj="<< firstJet.pt() << "  "<< firstJet.get<float>("pt")<< "  "<< firstJet.m_varMap->getHandle<float>("pt")->outputVar()<<endl;
      CHECK( (firstJet.pt() == jet->pt() ), " Particle pt="<< firstJet.pt()<< " != source pt="<< jet->pt() );
      CHECK( (firstJet.get<float>("Tau1") == jet->get<float>("Tau1") ), " Particle tau1="<< firstJet.get<float>("Tau1")<< " != source tau1="<< jet->get<float>("Tau1") );

      firstJet.set<float>("Tau1", 22.);
      CHECK( (firstJet.get<float>("Tau1") != jet->get<float>("Tau1") ), " After setting, Particle tau1="<< firstJet.get<float>("Tau1")<< " == source tau1="<< jet->get<float>("Tau1") );
      CHECK( (firstJet.get<float>("Tau1") == 22. ), " After setting, Particle tau1="<< firstJet.get<float>("Tau1")<< " != 22 " );

      firstJet = (*jetCont[0] + *jetCont[1]) ;
      CHECK( (firstJet.E() == (jet->e()+jetCont[1]->e()) ), " operator + failed " );
      
      firstJet = *jet ; // Reset so ntuple can be compared.

      ENDEVENTLOOP ;
    }
        return true;
  }
  // *********************************************************
    


  bool ContainerModifTest::i_fullTest(){
        //VarHandle< std::vector<float> > jet_emscale_pt("jet_AntiKt10LCTopo_emscale_pt",&m_master, &m_chain);
    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);
    jetCont.readFrom(&m_chain);
    jetCont.declareOutputVar<float>("OutputV");
    WriteOut(jetCont);


    ParticleContainer jetView("myjet_", m_master);
    jetView.setSubsetOfInputContainer( &jetCont);
    jetView.declareOutputVar<float>("constscale_pt");
    jetView.declareOutputVar<float>("OutputV");
    WriteOut(jetView);


    STARTEVENTLOOP {
      size_t n= jetCont.size();
      std::cout<< n << std::endl;
      for(size_t i= 0; i<n;i++){
        const Particle * jet = jetCont[i];
        Particle & j1 = *jetView.addNewFromSource( jet );
        j1 *= 1.3;
        j1.set<float>("constscale_pt", jet->get<float>("constscale_pt")*0.5);
        j1.set<float>("OutputV", 43.4);
        jet->set<float>("OutputV", 23.4);
        jet->set<float>("OutputVTrans", 23.4);
      }

      try{
        jetCont[0]->get<float>("dasdsa");
        CHECK( false , "get() non existing var worked ! ")
      } catch (const VarNotFound& v){        
      } catch (...){
        CHECK( false , "get() non existing thrown unknown exception ")

      }

      for(size_t i= 0; i<n;i++){
        const Particle * jet = jetCont[i];
        Particle * jetv = jetView[i];

        CHECK( ( fabs(jetv->pt() - 1.3* jet->pt()) <0.1) , " Unexpected pt "<< jetv->pt() <<"  != 1.3x "<< jet->pt() );
        float vconst_pt = jetv->get<float>("constscale_pt");float const_pt = jet->get<float>("constscale_pt");
        CHECK( (fabs(vconst_pt - 0.5*const_pt) < 0.1) , " Unexpected constscale_pt "<< vconst_pt <<"  != 0.5 x "<< const_pt );
        CHECK( (jet->get<float>("OutputVTrans")==jet->get<float>("OutputV")) , " OutputV in jetCont differ");
      }

      /// Make sure basic creation/filling works
      ParticleContainer testCont;
      testCont.push_back( new Particle);
      testCont[0]->set< float > ("vsd",34.5);
      ENDEVENTLOOP ;
    }
        return true;

  }
  // *********************************************************



  bool ContainerIteratorTest::i_fullTest(){
    //VarHandle< std::vector<float> > jet_emscale_pt("jet_AntiKt10LCTopo_emscale_pt",&m_master, &m_chain);
    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);
    jetCont.readFrom(&m_chain);


    STARTEVENTLOOP {
      
      ParticleContainer::const_iterator it = jetCont.begin();
      ParticleContainer::const_iterator itE = jetCont.end();
      size_t i=0;
      for( ; it != itE; ++it){
        const Particle * jet = *it;
        CHECK( (jet == jetCont[i]) , "Jet from iterator differ at index "<< i << " "<< jet << "  "<< jetCont[i] );
        ++i;
      }
      CHECK( (i == jetCont.size() ) , " Num iteration differs from size "<< i << "  "<< jetCont.size());

      ENDEVENTLOOP ;
    }
        return true;

  }
  // *********************************************************



  bool StealVarTest::i_fullTest(){

    //VarHandle< std::vector<float> > jet_emscale_pt("jet_AntiKt10LCTopo_emscale_pt",&m_master, &m_chain);
    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);
    jetCont.readFrom(&m_chain);
    std::vector< float >* stealer_jet_pt = 0;


    STARTEVENTLOOP {
      if(m_master==1){
        TBranch* bb = 0;
        m_chain.SetBranchAddress("jet_AntiKt10LCTopo_pt", &stealer_jet_pt, &bb);
        // cout << "event 1 bb="<< (&bb) << " "<< bb<< endl;
        //  TChainElement* element = (TChainElement*) m_chain.GetStatus()->FindObject("jet_AntiKt10LCTopo_pt");
        //  cout << "         = "<< element->GetBranchPtr() << "  stealer_jet_pt "<< stealer_jet_pt<< "  baddress "<< element->GetBaddress()<< "/ "<< m_chain.GetBranch("jet_AntiKt10LCTopo_pt") <<endl;
 
      }
      if(m_master>0){
        m_chain.GetBranch("jet_AntiKt10LCTopo_pt")->GetEntry(m_master);

        CHECK( ( stealer_jet_pt->at(0) == jetCont[0]->pt() ) , " jet0.pt != stealer_pt[0] ");
        if(jetCont.size()>0) CHECK( ( stealer_jet_pt->at(1) == jetCont[1]->pt() ) , " jet1.pt != stealer_pt[1] ");
      }
      
      
      ENDEVENTLOOP ;
    }
    return true;       
  }
  // *********************************************************

  bool DummyResultTest::i_fullTest(){

    //VarHandle< std::vector<float> > jet_emscale_pt("jet_AntiKt10LCTopo_emscale_pt",&m_master, &m_chain);
    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);
    jetCont.readFrom(&m_chain);

    STARTEVENTLOOP {
      bool valid;
      const float &a = jetCont[0]->get<float>("noway",valid);
      //cout << &a << "  "<< &VarHelperDummy<float>::get()<<endl;
      CHECK( ( a==0) , " float dummy do not default to 0");
      CHECK( ( valid == false) , " Not expected valid for get(noway)");
      const_cast<float&>(a) =4; // force modification of dummy.
      const std::vector< int > & vv = jetCont[0]->get<std::vector<int>& >("noway",valid);
      CHECK( vv.empty() , " vector<int> dummy not empty");
      const_cast<std::vector<int>&>(vv).resize(3);
      ENDEVENTLOOP ;
    }
    return true;       
  }
  // *********************************************************


  template<>
  struct VarHelperDummy<vector<Particle*> > {
    static vector<Particle*> & get(){ cout << " VarHelperDummy "<< endl; dummyObj.clear(); return dummyObj;}
    static vector<Particle*> dummyObj;
  };
  vector<Particle*> VarHelperDummy< vector<Particle*> >::dummyObj;
  template<>
  struct VarHelperDummy<Particle* > {
    static Particle* & get(){  dummyObj=0; return dummyObj;}
    static Particle* dummyObj;
  };
  Particle* VarHelperDummy< Particle* >::dummyObj;

  template<typename T> struct is_bool { static const bool value=false ;};
  template<>           struct is_bool<bool> { static const bool value=true ;};

  template<>
  struct VarHelperDummy< TVector3 > {
    static TVector3 & get(){ dummyObj.SetXYZ(0,0,0); return dummyObj;}
    static TVector3 dummyObj;
  };
  TVector3 VarHelperDummy< TVector3 >::dummyObj;
  template<>
  struct VarHelperDummy< LorentzVector_t > {
    static LorentzVector_t & get(){ dummyObj.SetCoordinates(0,0,0,0); return dummyObj;}
    static LorentzVector_t dummyObj;
  };
  LorentzVector_t VarHelperDummy< LorentzVector_t >::dummyObj;

  template <typename TT> TT builder(int n){return TT(n);}
  template <> LorentzVector_t builder<LorentzVector_t>(int n){return LorentzVector_t(n,0,0,n);}

  template<typename TT>
  bool ParticleAttributeTest::setTest(const ParticleContainer & jetCont, ParticleContainer& jetView){

    const char* tname = typeid(TT).name();
    const Particle * inputP = jetCont[0];
    bool valid;
    TT  p = inputP->get<TT>("noway",valid);
    
    CHECK( ( p==VarHelperDummy<TT>::get())  , " dummy do not default to 0. typeid="<<tname);
    Particle pp; pp = (*inputP);
    TT v = builder<TT>(4);
    //cout << " setting orphan part "<< tname << endl; 
    pp.set<TT>("link", v );
    
    CHECK( ( pp.get<TT>("link") == v ) , " Value retrieved differs from the one set previously. Orphan Part.  typeid="<<tname );
        
    inputP->set<TT>("link",v);
    CHECK( ( inputP->get<TT>("link") == v ) , " Value retrieved differs from the one set previously. Input Part. typeid="<<tname);

    Particle *pclone = inputP->clone();
    int n= 3;

    if( is_bool<TT>::value ) n = 0;// so v2 differs from v
    TT v2 = builder<TT>(n);
    pclone->set<TT>("link", v2);

    CHECK( ( pclone->get<TT>("link") == v2 ) , " Value retrieved differs from the one set previously. Clone Part.  typeid="<<tname );
    CHECK( ( inputP->get<TT>("link") != v2 ) , " Value from source ==  the one set from clone. Clone Part.  typeid="<<tname );

    Particle *pInView = jetView.addNewFromSource(inputP); 
    if( is_bool<TT>::value ) n = 0;// so v3 differs from v   
    TT v3 = builder<TT>(n);
    pInView->set<TT>("link", v3);
    CHECK( ( pInView->get<TT>("link") == v3 ) , " Value retrieved differs from the one set previously. View Part.  typeid="<<tname );
    CHECK( ( inputP->get<TT>("link") != v3 ) , " Value from source ==  the one set from viewPart. View Part.  typeid="<<tname );
    CHECK( (pInView->m_varMap == 0 )  , " view particle should not hold attribute map typeid="<<tname);
    delete pclone;
    return true;    
  }
  
  bool ParticleAttributeTest::i_fullTest(){

    //VarHandle< std::vector<float> > jet_emscale_pt("jet_AntiKt10LCTopo_emscale_pt",&m_master, &m_chain);
    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);
    jetCont.readFrom(&m_chain);
    jetCont.declareOutputVar<Particle*>("link");
    jetCont.declareOutputVar<float>("link",false);
    jetCont.declareOutputVar<bool>("link",false);
    jetCont.declareOutputVar< std::vector<float> >("link",false);

    ParticleContainer jetView("myjet_", m_master);
    jetView.declareOutputVar<Particle*>("link");
    jetView.declareOutputVar<float>("link",false);
    jetView.declareOutputVar<bool>("link",false);
    jetView.declareOutputVar< std::vector<float> >("link",false);
    jetView.setSubsetOfInputContainer( &jetCont);


    STARTEVENTLOOP {
      if(jetCont.size()==0) continue;
      cout << " event __ size "<< eventN << " --> "<< jetCont.size() << " viewsize = "<< jetView.size() << endl;
      if( !setTest<float>(jetCont, jetView) ) return false;
      if( !setTest<bool>(jetCont, jetView) ) return false;
      if( !setTest<vector<float> >(jetCont, jetView) ) return false;
      if( !setTest< TVector3 >(jetCont, jetView) ) return false;
      //cout << " event __ size "<< jetCont.size() << " viewsize = "<< jetView.size() << endl;
      if( !setTest< LorentzVector_t >(jetCont, jetView) ) return false;
      const Particle * inputP = jetCont[0];
      bool valid;
      const Particle * p = inputP->get<Particle*>("noway",valid);

      CHECK( ( p==0) , " float dummy do not default to 0");

      //TVector3 v = inputP->get<TVector3>("bla");
      
      Particle pp; pp = (*inputP);
      pp.set<Particle*>("link", inputP );

      CHECK( ( pp.get<Particle*>("link") == inputP ) , " Particle* retrieved from the one set previously. Orphan Part.  "<< pp.get<Particle*>("link") );

      pp.set<ParticleContainer*>("linkCont", &jetView);
      CHECK( ( pp.get<ParticleContainer*>("linkCont") == &jetView ) , " ParticleContainer* retrieved from the one set previously. Orphan Part");

      inputP->set<Particle*>("link",&pp);
      CHECK( ( inputP->get<Particle*>("link") == &pp ) , " Particle* retrieved from the one set previously. Input Part");

      Particle *pclone = inputP->clone();
      pclone->set<Particle*>("link", inputP);

      CHECK( ( pclone->get<Particle*>("link") == inputP ) , " Particle* retrieved differs from the one set previously. Clone Part.  " );
      CHECK( ( inputP->get<Particle*>("link") != inputP ) , " Particle* from source ==  the one set from clone. Clone Part.  " );


      Particle *pInView = jetView.addNewFromSource(inputP); 
      pInView->set<Particle*>("link", inputP);

      CHECK( ( pInView->get<Particle*>("link") == inputP ) , " Particle* retrieved differs from the one set previously. View Part.  " );
      CHECK( ( inputP->get<Particle*>("link") != inputP ) , " Particle* from source ==  the one set from clone. View Part.  " );

      Particle ptest( *jetCont[0] + *jetCont[1]);
      ptest.set<Particle*>("p0", jetCont[0]);
      ptest.set<Particle*>("p1", jetCont[1]);

      ENDEVENTLOOP ;
    }
    return true;       
  }
  // *********************************************************


  bool KinematicVarTest::i_fullTest(){

    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);    
    jetCont.readFrom(&m_chain);
    jetCont.declareKinematicVar("constscale_pt","constscale_eta","constscale_phi","constscale_E");
    
    WriteOut(jetCont);
    
    STARTEVENTLOOP {
      const Particle *j = jetCont[0];
      cout << j->pt() << endl;
      ENDEVENTLOOP ;
    }
    return true;       
  }
  // *********************************************************


  bool ConstituentsTest::i_fullTest(){

    //VarHandle< std::vector<float> > jet_emscale_pt("jet_AntiKt10LCTopo_emscale_pt",&m_master, &m_chain);
    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);
    jetCont.readFrom(&m_chain);
    
    const ParticleContainer clust("cl_lc", m_master);
    clust.declareKinematicVar("pt", "eta", "phi", "E_em");
    clust.forceInputMass(0.);
    clust.readFrom(&m_chain);

    const ParticleContainer trk("trk_", m_master);
    trk.declareKinematicVar("pt", "eta", "phi_wrtPV", "E_em");
    trk.forceInputMass(0.);
    trk.readFrom(&m_chain);
    
    jetCont.declareConstituents(&clust, "constit_index");

    VarHandle< std::vector< std::vector<int> > > jet_const_index; 
    VarHandle< std::vector<float> > clust_eta; 

    jet_const_index.setParameters("jet_AntiKt10LCTopo_constit_index",&m_master, &m_chain);
    clust_eta.setParameters("cl_lc_eta",&m_master, &m_chain);

    STARTEVENTLOOP {

      ParticleContainer::const_iterator it = jetCont.begin();
      ParticleContainer::const_iterator itE = jetCont.end();
      size_t i=0;     
      for( ; it != itE; ++it){
        const Particle * jet = *it;
        ParticleContainer constit ; 
        jet->fillConstituents(constit);
        // check with TTree : physics->Scan("jet_AntiKt10LCTopo_constit_n:jet_AntiKt10LCTopo_constit_index[][2]:cl_eta[jet_AntiKt10LCTopo_constit_index[][2]]","Iteration$<3") 
        
        CHECK( (constit.size() == jet_const_index()[i].size() ) , " constituent size incorrect");
        CHECK( (clust_eta()[jet_const_index()[i][0]] == constit[0]->eta()  ) , " constituent 0 eta differs from value in cl_eta");
        //cout << i<< "   "<< constit.size() << "  "<< constit[0]->eta()<<endl;
        i++;
        if(i>2) break;

        /// ConstituentIterator checks.
        if(i==0){
          ConstituentIterator cit = ConstituentIterator::first(jet);
          ConstituentIterator citE = ConstituentIterator::last(jet);
          size_t count = 0;
          for( ; cit != citE; ++cit){
            CHECK( (cit.get_real_constit() == constit[count]), "constit from fillConstituents and ConstituentIterator differs at " << count );
            CHECK( (cit.e() == constit[count]->e() ) , " constit e() from ConstituentIterator differs from pointer->e()");
            count++;
          }
          CHECK( (count== constit.size()) , "Num constit from ConstituentIterator differs from fillConstituents() ");
        }

      }
      CHECK( floatEqual( clust[0]->m(), 0.) ," cluster mass non null " );
      //cout<< " clus masses "<< clust[0]->m() << "  "<< clust[0]->e()  << endl;//clust[0]->m() 

      
      

      size_t ntrk = trk.size();
      CHECK( (ntrk != 0), "0 tracks");
      std::cout << " _trk0 "<< trk[0]->pt() << "  "<< trk[0]->m() << std::endl;
      ENDEVENTLOOP ;
    }
    return true;       
  }
  // *********************************************************


  bool SignalStateTest::i_fullTest(){

    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);    
    jetCont.readFrom(&m_chain);

    STARTEVENTLOOP {
      const Particle * p = jetCont[0];

      CHECK( (p->pt(P4SignalState::JETEMSCALE) == p->get<float>("emscale_pt") ) ," E(JEtEMSCALE) ="<< p->E(P4SignalState::JETEMSCALE) <<" differs from get "<< p->get<float>("emscale_pt") );
      CHECK( (p->pt(P4SignalState::JETCONSTITUENTSCALE) == p->get<float>("constscale_pt") ) ," E(CONSTSCALE) ="<< p->E(P4SignalState::JETCONSTITUENTSCALE) <<" differs from get "<< p->get<float>("constscale_pt") );
      
      Particle constscale_p = p->hlv(P4SignalState::JETCONSTITUENTSCALE) ; 
      CHECK( (constscale_p.pt() == p->get<float>("constscale_pt")) , " constscale_p.pt()="<< constscale_p.pt() << " differs from get<> "<<p->get<float>("constscale_pt") );
      ENDEVENTLOOP ;
    }
    return true;       
  }
  // *********************************************************

  bool KinematicUtilTest::i_fullTest(){

    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);    
    jetCont.readFrom(&m_chain);

    STARTEVENTLOOP {
      const Particle * p0 = jetCont[0];
      const Particle * p1 = jetCont[1];
      
      CHECK( ( P4Helpers::deltaR(*p0, *p0 )==0) ," deltar(p,p) != 0  : "<< P4Helpers::deltaR(*p0, *p0 ) );
      CHECK( ( P4Helpers::deltaR(*p0, *p1 ) <20) ," deltar(p0,p1) huge  : "<< P4Helpers::deltaR(*p0, *p1 ) );
      
      ENDEVENTLOOP ;
    }
    return true;       
  }
  // *********************************************************


  bool SelectorTest::i_fullTest(){

    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);    
    jetCont.readFrom(&m_chain);

    ParticleContainer filtered("myjet_", m_master);
    ParticleContainer filteredEM("myjet2_", m_master);

    float ptcut = 15000;
    float etacut = 1.;

    ParticleSelector sel;
    sel.addAtomicSel(SelectPt< Above >(ptcut) );
    sel.addAtomicSel(SelectEta< Below >(etacut) );

    float emPTcut = 12000;
    ParticleSelector selEM;
    selEM.addAtomicSel(SelectAttribute<float , Above >("emscale_pt", emPTcut) );

    STARTEVENTLOOP {
      filtered.clear();

      sel.filter( jetCont, filtered);

      size_t sel_i = 0;
      for(size_t i=0 ; i< jetCont.size();i++){
        const Particle* j = jetCont[i];
        if( (j->pt() > ptcut) && (j->eta()<etacut)) {
          //cout << j
          CHECK( (filtered[sel_i]->getSource() == j ) , " eta pt Selector differs hand-written cut sel_i="<<sel_i << " i="<< i);
          sel_i++;
        }        
      }

      selEM.filter( jetCont, filteredEM);

      sel_i = 0;
      for(size_t i=0 ; i< jetCont.size();i++){
        const Particle* j = jetCont[i];
        if( j->get<float>("emscale_pt")>emPTcut) {
          cout << i<< " "<< sel_i << " /  "<< filteredEM.size()<< " | "<<  j->get<float>("emscale_pt") << endl;
          CHECK( (filteredEM[sel_i]->getSource() == j ) , " emscale_pt Selector differs hand-written cut sel_i="<<sel_i << " i="<< i);
          sel_i++;
        }        
      }


      ENDEVENTLOOP ;
    }
    sel.addAtomicSel(SelectAbsEta< InRange >(etacut) ); // just test compilation

    return true;       
  }
  // *********************************************************

  bool PtEtaPhiMTest::i_fullTest(){

    const ParticleContainer mcCont("mc_",m_master);    
    mcCont.readFrom(&m_chain);
    mcCont.usePtEtaPhiM(true);
    mcCont.declareKinematicVar("pt", "eta", "phi", "m");
    WriteOut(mcCont);

    //cout <<"   prefix = "<< mcCont.getPrefix() << " name="<< mcCont.getName() << endl; 
    STARTEVENTLOOP {
      cout << " mc size "<< mcCont.size()<<endl;
      const Particle* p = mcCont[0];
      cout << " p e="<< p->e() << "  m "<< p->M() << endl;
      p = mcCont[234];
      cout << " p e="<< p->e() << "  m "<< p->M() << endl;
      
      ENDEVENTLOOP ;
    }
    return true;       
  }
  // *********************************************************

  bool LoadEntryTest::i_fullTest(){


    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);    
    jetCont.readFrom(&m_chain);

    ParticleContainer jetCont2("jet_AntiKt10LCTopo_");    
    jetCont2.readFrom(&m_chain);


    STARTEVENTLOOP {

      if( (m_master % 2) ==0 ){
        jetCont2.loadEntry(m_master);
      }

      const Particle * p1 = jetCont[0];
      const Particle * p2 = jetCont2[0];
      //std::cout << " m_master "<< m_master << " "<< m_master %2 << std::endl; //  "  "<< p1->get<float>("Tau2") <<" / "<< p2->get<float>("Tau2") << std::endl;
      if( (m_master % 2) ==0 ){
        CHECK( (p1->e() == p2->e() ) , "particle e differs ");
        CHECK( (p1->get<float>("Tau2") == p2->get<float>("Tau2") ) , "particle tau2 differs ");
      }else{
        CHECK( (p1->e() != p2->e() ) , "particle e identical (should not) ");
        // std::cout<< "   __p2  " << p2->get<float>("Tau2") << std::endl;
        // std::cout<< "   __p1  " << p1->get<float>("Tau2") << std::endl;
        // std::cout<< "   __p2  " << p2->get<float>("Tau2") << std::endl; // <-- This is wrong, because jetCont1 and jetCont share the same variables from the TTree.
        // kinematics are ok because they are not shared !
        // The line below will fail:
        //CHECK( (p1->get<float>("Tau2") != p2->get<float>("Tau2") ) , "particle tau2 identical (should not) ");
      }
      //CHECK( ( P4Helpers::deltaR(*p0, *p0 )==0) ," deltar(p,p) != 0  : "<< P4Helpers::deltaR(*p0, *p0 ) );
      //CHECK( ( P4Helpers::deltaR(*p0, *p1 ) <20) ," deltar(p0,p1) huge  : "<< P4Helpers::deltaR(*p0, *p1 ) );
      
      ENDEVENTLOOP ;
    }
    return true;       


  }

  // *********************************************************

  bool SetKinematicTest::i_fullTest(){


    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);    
    jetCont.readFrom(&m_chain);

    STARTEVENTLOOP {

      Particle * p = const_cast<Particle*>(jetCont[0]);
      float e = p->e();
      float eta = p->eta();
      p->SetE(e*2);
      CHECK( ( 2*e == p->e() ) ," E not doubled as expected " );
      
      p->setPtEtaPhiE( p->pt(), eta+0.1, p->phi(), e*2);
      CHECK( ( 2*e == p->e() ) ," E not doubled as expected after PtEtaPhiE" );
      CHECK( floatEqual(eta+0.1 ,p->eta() )   ," Eta not  as expected after PtEtaPhiE "<< eta+0.1<< "  _ "<< p->eta() );

      float pz = p->pz();
      p->setPxPyPzE( p->px(), p->py(),pz*1.1 ,p->e());
      cout.precision(10);
      CHECK( floatEqualGeV( 1.1*pz , p->pz() ) ," pz not  as expected after PtEtaPhiE "<< 1.1*pz << "  "<< p->pz() );

      float m = p->mass();
      p->setPtEtaPhiM( p->pt(), eta, p->phi(), m*1.2);
      CHECK( floatEqualGeV( 1.2*m , p->m() ) ," pz not  as expected after PtEtaPhiM "<< p->mass() );
      

      //CHECK( ( P4Helpers::deltaR(*p0, *p1 ) <20) ," deltar(p0,p1) huge  : "<< P4Helpers::deltaR(*p0, *p1 ) );
      
      ENDEVENTLOOP ;
    }
    return true;       


  }

  // *********************************************************

  bool SwapTest::i_fullTest(){

    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);    
    jetCont.readFrom(&m_chain);

    std::vector<int> vind(3); vind[0]=0;vind[1]=1;vind[2]=2;
    STARTEVENTLOOP {
      ParticleContainer conts1;
      ParticleContainer conts2;
    
      Particle * p = const_cast<Particle*>(jetCont[0]);
      vind[2] = jetCont.size() - 1;
      conts1.copyFrom(jetCont, vind);

      CHECK( (conts1.size() == 3) , " CopyFrom : num jet unexpected");
      CHECK( (conts1[0]->pt() == p->pt()) , " CopyFrom : jet0 pt unexpected");
      CHECK( (conts1[2]->pt() == jetCont[vind[2]]->pt()) , " CopyFrom : jet2 pt unexpected");
      Particle *p1 = conts1[1];
      conts1.swap(conts2);
      CHECK( (conts2.size() == 3) , " swap : num jet unexpected");
      CHECK( (conts2[1]== p1) , " swap : jet1 differs");
      
      ENDEVENTLOOP ;
    }
    return true;       


  }


  // *********************************************************

  void dumpVecString(vector<string_t> & keys){
    cout<< keys.size()<< "   :";
    for(size_t i=0;i<keys.size(); i++) cout<< " , "<< keys[i];
    cout<< endl;
  }

  bool ListAttributesTest::i_fullTest(){

    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);    
    jetCont.readFrom(&m_chain);


    STARTEVENTLOOP {
      const Particle * p = jetCont[0];
      
      std::vector<string_t> keys =  p->getAttributeKeys<float>();
      CHECK( (keys.size() == 58 ) , " unexpected number of attributes. Got "<< keys.size()); // depends on input file !
      //dumpVecString(  keys );
      keys = p->getAttributeKeys<int>();
      //dumpVecString(  keys );


      Particle p2;
      keys =  p2.getAttributeKeys<float>(); 
      CHECK( (keys.size() == 0 ) , " unexpected number of attributes p2 non null"); 
      p2.set<float>("asdf", 24.1);
      p2.set<float>("qwerty", 4.1);
      keys =  p2.getAttributeKeys<float>(); 
      dumpVecString(keys);
      CHECK( (keys.size() == 2 ) , " unexpected number of attributes p2 not 2"); 
      ENDEVENTLOOP ;
    }
    return true;       


  }

  // *********************************************************
  bool RecreateInEventTest::process(){
    const ParticleContainer jetCont("jet_AntiKt10LCTopo_",m_master);    
    jetCont.readFrom(&m_chain);

    const ParticleContainer jetCont2("jet_AntiKt10LCTopo_",m_master);    
    jetCont2.readFrom(&m_chain);

    EventObject event(m_master, &m_chain);
    //EventObject event2(m_master, &m_chain);

    
    CHECK( (jetCont2.size() == jetCont.size() ) , " same cont size differs "); 
    CHECK( (jetCont2[0]->pt() == jetCont[0]->pt() ) , " same cont : pt0 differs "); 
    CHECK( (jetCont2[0]->get<float>("Tau1") == jetCont[0]->get<float>("Tau1") ) , " same cont : Tau1_0 differs "); 

    CHECK( (event.RunNumber() >0 ), " wrong event number" );
    // CHECK( (event.get<int>("el_n") > 0 ) , " el_n == 0 ");
    
    // CHECK( (event.get< vector<float> & >("el_pt").size() > 0 ) , " el_pt.size() == 0 ");



    return true;
  }
  
  bool RecreateInEventTest::i_fullTest(){
    
    STARTEVENTLOOP {
      if(!process()) return false;
      ENDEVENTLOOP;
    }
    return true;
  }




  // *********************************************************
  bool EventManagerTest::i_fullTest(){
    
    setupOutTree();


    EventManager event(&m_chain);


    event.declareOutputTree(m_otree);

    const ParticleContainer * jetContI = event.retrieve("jet_AntiKt10LCTopo");
    event.declareOutputContainer("jet_AntiKt10LCTopo");

    ParticleContainer myjetCont("myjet");
    event.declareOutputContainer(&myjetCont);
    event.declareOutputContainerVar<float>("myjet", "myVar");
    event.declareOutputContainerVarAsIn("myjet", "^constscale.*", "jet_AntiKt10LCTopo");
    //myjetCont.declareOutputVarAsIn(jetContI, "^constscale.*");


    while( event.nextEntry() ){
      eventN = event.currentEntry();
      const ParticleContainer * jetCont = event.retrieve("jet_AntiKt10LCTopo");
      //cout << event.m_inputTree->GetReadEntry() << " EventNumber "<< event.EventNumber() << "  njet="<< jetCont->size()  << "  "<< jetCont << " / "<< jetContI << endl;
      
      CHECK( (jetContI == jetCont ) , "Jet containers are different");
      CHECK( (jetCont->size()>0) , "void jet container");
      
      
      Particle* newj = myjetCont.addNewFromSource((*jetCont)[0]);
      newj->set<float>("myVar",22.1);      
      newj->set<float>("constscale_eta", 10);
      newj = myjetCont.addNewFromSource((*jetCont)[1]);
      newj->set<float>("myVar",22.1);      

      newj->set<float>("constscale_eta", 10);


      const ParticleContainer* clus = event.retrieve("cl");
      CHECK( (clus->size() >0), "void cluster container");
      
      event.writeEvent();
    }

    return true;
  }

}





using namespace JetAnalysisEDM;
#define RUNTEST( N, TestClass) if((doAll)|| isInArgs(#N) ){ count++; TestClass t; res = t.fullTest(); \
                                                            if(!res)  {return count;} else cout << "---------------" << endl;}


int gargc;
char** gargv;

bool isInArgs(const char* t){
  int i=1;
  std::string s = t;
  while( i< gargc ) {if( s==gargv[i]) return true; i++;}
  return false;
}

bool testInputFile( ){
  TFile * f = TFile::Open(c_inputFile);
  return (f != 0 );
  
}

int main(int argc, char* argv[]){
  gargv = argv; gargc =argc;
  bool res = true;
  int count = 0;
  
  if(! testInputFile() ) {return 0;}

  bool doAll = (argc==1);  

  RUNTEST( 1, TwoEventInfoTest );
  RUNTEST( 2, TwoVarHandleTest );
  RUNTEST( 3, TwoVarHandleWriteTest );
  RUNTEST( 4, VecVecVarHandleTest );
  RUNTEST( 5, ContainerVsVarHTest );
  RUNTEST( 6,  ParticleGetVsVarHTest);
  RUNTEST( 7,  AccessorVsVarHTest);
  RUNTEST( 8,  ViewContainerTest);
  RUNTEST( 9,  ViewContainerCloneTest);
  RUNTEST( 10,  SinglePartTest);
  RUNTEST( 11,  ContainerModifTest);
  RUNTEST( 12,  ContainerIteratorTest);
  RUNTEST( 13,  StealVarTest);
  RUNTEST( 14,  DummyResultTest);
  RUNTEST( 15,  ParticleAttributeTest);
  RUNTEST( 16,  ConstituentsTest );
  RUNTEST( 17,  KinematicVarTest );
  RUNTEST( 18,  SignalStateTest );
  RUNTEST( 19,  KinematicUtilTest );
  RUNTEST( 20,  SelectorTest );
  RUNTEST( 21,  PtEtaPhiMTest );
  RUNTEST( 22,  LoadEntryTest );
  RUNTEST( 23,  SetKinematicTest );
  RUNTEST( 24,  SwapTest );
  RUNTEST( 25,  ListAttributesTest );
  RUNTEST( 26,  RecreateInEventTest );
  RUNTEST( 27,  EventManagerTest );

  return 0;

}
