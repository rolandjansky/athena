/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CPAnalysisExamples includes
#include "CPAnalysisExamples/xAthExample.h"

#include "CPAnalysisExamples/xExampleEDM.h"
#include "CPAnalysisExamples/xExampleUtils.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/AlgTool.h"

#include <ctime>

xAthExample::xAthExample( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ) {

}



StatusCode xAthExample::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

///this was me just playing around with stuff .. ignore it
//    IMETRebuilder* tmpTool = dynamic_cast<IMETRebuilder*>(AlgTool::Factory::create("met::METRebuilder", "met::METRebuilder", "myTool", this));
//    //tmpTool->setProperty("JetColl","oo");
//    ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc",name());
//    CHECK( joSvc->addPropertyToCatalogue("myTool",StringProperty("JetColl","Blah")) );
//    CHECK( joSvc->setMyProperties( tmpTool->name(), dynamic_cast<IProperty*>(tmpTool) ) );
/// end of stuff

     // pt bins
  float xxLim[17] = {9.9999,
                     20.00, 30.00, 40.00, 60.00, 80.00, 120.0,
                     160.0, 240.0, 320.0, 480.0, 640.0, 960.0,
                     1280., 1920., 2560.,
                     10000.1};

  std::string xxStr[17] = {"0000",
                           "0020","0030","0040","0060","0080","0120",
                           "0160","0240","0320","0480","0640","0960",
                           "1280","1920","2560",
                           "0000"};

  for(unsigned int i=0; i<17; ++i){
    ptLim[i] = xxLim[i];
    ptStr[i] = xxStr[i];
  }

  // Primary vertex

  m_TH1["intactual"] = new TH1F("intactual","intactual",50,0.,50.);
  m_TH1["intaverage"] = new TH1F("intaverage","intaverage",50,0.,50.);
  m_TH1["intdiff"] = new TH1F("intdiff","intdiff",100,-50.,50.);

  m_TH1["privtxidx"] = new TH1F("privtxidx","privtxidx",50,0.,50.);
  m_TH1["privtxz0"] = new TH1F("privtxz0","privtxz0",100,-200.,200.);
  m_TH1["privtxn2"] = new TH1F("privtxn2","privtxn2",50,0.,50.);
  m_TH1["privtxn5"] = new TH1F("privtxn5","privtxn5",50,0.,50.);
  m_TH1["privtxntrk"] = new TH1F("privtxntrk","privtxntrk",100,0.,200.);

  // Muons

  m_TH1["mupt"] = new TH1F("mupt", "mupt", 100, 0., 500. );
  m_TH1["mueta"] = new TH1F("mueta", "mueta", 60, -3., 3. );
  m_TH1["muphi"] = new TH1F("muphi", "muphi", 100, -1., 1.);
  m_TH1["mutype"] = new TH1F("mutype","mutype",5,0.,5.); 
  m_TH1["muquality"] = new TH1F("muquality","muquality",5,0.,5.); 
  m_TH1["mugoodpt"] = new TH1F("mugoodpt", "mugoodpt", 100, 0., 500. );
  m_TH1["mugoodeta"] = new TH1F("mugoodeta", "mugoodeta", 60, -3., 3. );
  m_TH1["muidrat"] = new TH1F("muidrat","muidrat",100,0.,2.);
  m_TH1["muptc30"] = new TH1F("muptc30","muptc30",100,0.,20.);

  // Electrons

  m_TH1["elpt"] = new TH1F("elpt","elpt",100, 0., 500.);
  m_TH1["eleta"] = new TH1F("eleta","eleta", 60, -3., 3.);
  m_TH1["elphi"] = new TH1F("elphi","elphi", 100, -1., 1.);
  m_TH1["elauthor"] = new TH1F("elauthor","elauthor",16,0,16.);
  m_TH1["elntrk"] = new TH1F("elntrk","elntrk",20,0,20.);
  m_TH1["eleoverp"] = new TH1F("eleoverp","eleoverp",100,0,5.);
  m_TH1["eletc30"] = new TH1F("eletc30","eletc30",100,0.,20.);
  m_TH1["elgood"] = new TH1F("elgood","elgood",2,0.,2.);
  m_TH1["elgoodpt"] = new TH1F("elgoodpt","elgoodpt",100, 0., 500.);
  m_TH1["elgoodeta"] = new TH1F("elgoodeta","elgoodeta", 60, -3., 3.);
  m_TH1["eldrtrue"] = new TH1F("eldrtrue","eldrtrue",100,0.,1.);
  m_TH1["elptrat"] = new TH1F("elptrat","elptrat",100,0.,2.);

  // AntiKt4LCTopo jets

  static const float ptjmax = 2000.;
  static const float ptjlomax = 200.;

  m_TH1["akt4pt"] = new TH1F("akt4pt","akt4pt",100,0.,ptjmax);
  m_TH1["akt4ptlo"] = new TH1F("akt4ptlo","akt4ptlo",100,0.,ptjlomax);
  m_TH1["akt4eta"] = new TH1F("akt4eta","akt4eta",60,-3.,3.);
  m_TH1["akt4phi"] = new TH1F("akt4phi","akt4phi",100,-1.,1.);
  m_TH1["akt4rlep"] = new TH1F("akt4rlep","akt4rlep",100,0.,2.);
  m_TH1["akt4calibrlep"] = new TH1F("akt4calibrlep","akt4calibrlep",100,0.,2.);
  m_TH1["akt4ptbad"] = new TH1F("akt4ptbad","akt4ptbad",100,0.,ptjmax);
  m_TH1["akt4etabad"] = new TH1F("akt4etabad","akt4etabad",60,-3.,3.);
  m_TH2["akt4drelvpt"] = new TH2F("akt4drelvpt","akt4drelvpt",50,0.,200.,
                                  50,0.,2.);

  for(unsigned int ib=1; ib<16; ++ib){
    std::string str = "akt4calib" + ptStr[ib];
    m_TH1[str.c_str()] = new TH1F(str.c_str(),str.c_str(),100,0.,2.);
  }

  m_TH1["akt4jvf"] = new TH1F("akt4jvf","akt4jvf",110,-1.,1.19999);
  m_TH1["akt4jvfx"] = new TH1F("akt4jvfx","akt4jvfx",110,-1.,1.19999);
  m_TH1["akt4jvftot"] = new TH1F("akt4jvftot","akt4jvftot",110,0.,1.09999);
  for(unsigned int ib=1; ib<16; ++ib){
    std::string str = "akt4jvf" + ptStr[ib];
    m_TH1[str.c_str()] = new TH1F(str.c_str(),str.c_str(),110,-1.,1.19999);
  }

  m_TH1["akt4trueid"] = new TH1F("akt4trueid","akt4trueid",18,-2.,16.);
  m_TH1["akt4ptb"] = new TH1F("akt4ptb","akt4ptb",100,0.,ptjmax);
  m_TH1["akt4etab"] = new TH1F("akt4etab","akt4etab",60,-3.,3.);
  m_TH1["akt4mv1"] = new TH1F("akt4mv1","akt4mv1",100,0.,2.);
  m_TH1["akt4sv1b"] = new TH1F("akt4sv1b","akt4sv1b",100,0.,1.);
  m_TH1["akt4sv1u"] = new TH1F("akt4sv1u","akt4sv1u",100,0.,1.);
  m_TH2["akt4sv1uvb"] = new TH2F("akt4sv1uvb","akt4sv1uvb",80,0.,0.4,50,0.,1.);

  m_TH1["akt4ntrk"] = new TH1F("akt4ntrk","akt4ntrk",50,0.,50.);
  m_TH1["akt4rtrk"] = new TH1F("akt4rtrk","akt4rtrk",100,0.,1.);
  m_TH1["akt4nsitrk"] = new TH1F("akt4nsitrk","akt4nsitrk",20,0.,20.);
  m_TH1["akt4z0trk"] = new TH1F("akt4z0trk","akt4z0trk",100,-200.,200.);
  m_TH1["akt4dz0trk"] = new TH1F("akt4dz0trk","akt4dz0trk",200,-5.,5.);
  m_TH2["akt4z0vz0"] = new TH2F("akt4z0vz0","akt4z0vz0",25,-50.,50.,25,-50.,50.);

  for(unsigned int ib=1; ib<16; ++ib){
    std::string str = "akt4rtrk" + ptStr[ib];
    m_TH1[str.c_str()] = new TH1F(str.c_str(),str.c_str(),100,0.,2.);
  }

  m_TH1["akt4drtrue"] = new TH1F("akt4drtrue","akt4drtrue",100,0.,2.0);
  m_TH1["akt4drtruecalib"] = new TH1F("akt4drtruecalib","akt4drtruecalib",100,0.,2.0);
  for(unsigned int ib=1; ib<16; ++ib){
    std::string str = "akt4jes" + ptStr[ib];
    m_TH1[str.c_str()] = new TH1F(str.c_str(),str.c_str(),100,0.,2.);
    std::string str2 = "akt4jescalib" + ptStr[ib];
    m_TH1[str2.c_str()] = new TH1F(str2.c_str(),str2.c_str(),100,0.,2.);
  }
  m_TH2["akt4jesvpt"] = new TH2F("akt4jesvpt","akt4jesvpt",50,0.,500,50,0.,2.);

  // AntiKt4Truth jets

  m_TH1["aktT4pt"] = new TH1F("aktT4pt","aktT4pt",100,0.,ptjmax);
  m_TH1["aktT4ptlo"] = new TH1F("aktT4ptlo","aktT4ptlo",100,0.,ptjlomax);
  m_TH1["aktT4eta"] = new TH1F("aktT4eta","aktT4eta",60,-3.,3.);
  m_TH1["aktT4phi"] = new TH1F("aktT4phi","aktT4phi",100,-1.,1.);

  // AntiKt4EMTopo jets

  m_TH1["aktem4n100"] = new TH1F("aktem4n100","aktem4n100",20,0.,20.);
  m_TH1["aktem4ncalib100"] = new TH1F("aktem4ncalib100","aktem4ncalib100",20,0.,20.);

  // TauJets

  m_TH1["taupt"] = new TH1F("taupt", "taupt", 100, 0., 500. );
  m_TH1["taueta"] = new TH1F("taueta", "taueta", 60, -3., 3. );
  m_TH1["tauphi"] = new TH1F("tauphi", "tauphi", 100, -1., 1.);
  m_TH1["tauid"] = new TH1F("tauid","tauid",5,0.,5.); 
  m_TH1["taubdt"] = new TH1F("taubdt","taubdt",100,0.,1.);
  m_TH2["taudrelvpt"] = new TH2F("taudrelvpt","taudrelvpt",50,0.,200.,
                                 50,0.,2.);
  m_TH1["tautruept"] = new TH1F("tautruept","tautruept",100,0.,500.);
  m_TH1["tautrueeta"] = new TH1F("tautrueeta","tautrueeta",60,-3.,3.);
  m_TH1["taugoodpt"] = new TH1F("taugoodpt", "taugoodpt", 100, 0., 500. );
  m_TH1["taugoodeta"] = new TH1F("taugoodeta", "taugoodeta", 60, -3., 3. );
  m_TH1["taugoodbdt"] = new TH1F("taugoodbdt","taugoodbdt",100,0.,1.);
  m_TH1["tauntrk"] = new TH1F("tauntrk","tauntrk",10,0.,10.);
  m_TH1["tauptrtrk"] = new TH1F("tauptrtrk","tauptrtrk",120,0.,1.2);
  m_TH1["taudrtrue"] = new TH1F("taudrtrue","taudrtrue",100,0.,1.);
  m_TH1["tauptrat"] = new TH1F("tauptrat","tauptrat",100,0.,2.);

  // Met

  m_TH1["metet"] = new TH1F("metet","metet",100,0.,1000.);
  m_TH1["metetlo"] = new TH1F("metetlo","metetlo",100,0.,200.);
  m_TH1["metsum"] = new TH1F("metsum","metsum",100,0.,2000.);
  m_TH1["mettrueet"] = new TH1F("mettrueet","mettrueet",100,0.,1000.);
  m_TH1["mettrueetlo"] = new TH1F("mettrueetlo","mettrueetlo",100,0.,200.);
  m_TH1["mettruesum"] = new TH1F("mettruesum","mettruesum",100,0.,1000.);
  m_TH1["metcalibet"] = new TH1F("metcalibet","metcalibet",100,0.,1000.);
  m_TH1["metcalibetlo"] = new TH1F("metcalibetlo","metcalibetlo",100,0.,100.);
  m_TH1["metcalibsum"] = new TH1F("metcalibsum","metcalibsum",100,0.,2000.);

  // Truth particles

  m_TH1["tppt"] = new TH1F("tppt","tppt",100,0.,500.);
  m_TH1["tpptlo"] = new TH1F("tpptlo","tpptlo",100,0.,2.);
  m_TH1["tpeta"] = new TH1F("tpeta","tpeta",70,-7.,7.);
  m_TH1["tpnum"] = new TH1F("tpnum","tpnum",100,0.,1000.);
  m_TH1["tpsum"] = new TH1F("tpsum","tpsum",100,-100.,100.);
  m_TH1["tpsumlo"] = new TH1F("tpsumlo","tpsumlo",100,-1.,1.);
  m_TH1["tpbpt"] = new TH1F("tpbpt","tpbpt",100,0.,500.);
  m_TH1["tpbeta"] = new TH1F("tpbeta","tpbeta",100,-5.,5.);
  m_TH1["tpwkid"] = new TH1F("tpwkid","tpwkid",50,-25.,25.);

  // Interactions from Geant
  m_TH1["tpvxr"] = new TH1F("tpvxr","tpvxr",100,0.,500.);
  m_TH1["tpvxeta"] = new TH1F("tpvxeta","tpvxeta",60,-3.,3.);
  m_TH1["tpvxidin"] = new TH1F("tpvxidin","tpvxidin",100,0.,100.);


  //////////////////////////////////////
  // Add histograms to job and set Sumw2
  //////////////////////////////////////

  ServiceHandle<ITHistSvc> histSvc("THistSvc",name());

  std::map<std::string,TH1*>::iterator h1Itr = m_TH1.begin();
  std::map<std::string,TH1*>::iterator h1ItrE = m_TH1.end();
  for(; h1Itr != h1ItrE; ++h1Itr){
    TH1* h1 = (*h1Itr).second;
    h1->Sumw2();
    CHECK( histSvc->regHist(std::string("/MYSTREAM/")+h1->GetName(),h1) );
  }

  std::map<std::string,TH2*>::iterator h2Itr = m_TH2.begin();
  std::map<std::string,TH2*>::iterator h2ItrE = m_TH2.end();
  for(; h2Itr != h2ItrE; ++h2Itr){
    TH2* h2 = (*h2Itr).second;
    h2->Sumw2();
    CHECK( histSvc->regHist(std::string("/MYSTREAM/")+h2->GetName(),h2) );
  }


   
  // Wanted to restructure this more like real analysis:
  // (1) Get objects and apply CP tools for each
  // (2) Remove overlaps
  // (3) Make plots
  // But many tools missing, and intermediate plots useful. Have moved
  // collections to xExampleEDM.h to facilitate this later.


  m_eventCounter = 0;
  m_warnCounter = 0;
  m_warnLimit = 100;

   //initialize all the tools (configured in the job options)
   m_jetCleaningTool.setTypeAndName("JetCleaningTool/JetCleaning");
   CHECK( m_jetCleaningTool.retrieve() );

   m_akt4LCTopoCalibrationTool.setTypeAndName("JetCalibrationTool/JetCalibLCTopo");
   CHECK( m_akt4LCTopoCalibrationTool.retrieve() );

   m_akt4EMTopoCalibrationTool.setTypeAndName("JetCalibrationTool/JetCalibEMTopo");
   CHECK( m_akt4EMTopoCalibrationTool.retrieve() );

   m_metRebuilder.setTypeAndName("met::METRebuilder/metRebuilder");
   CHECK( m_metRebuilder.retrieve() );


  // Initialize new object containers
  // Will clear them after each execute()

  // Deep copy for Muons
  m_newMuons = new xAOD::MuonContainer();
  m_newMuonsAux = new xAOD::MuonAuxContainer();
  m_newMuons->setStore(m_newMuonsAux);

  // Deep copy for Electrons
  m_newElectrons = new xAOD::ElectronContainer();
  m_newElectronsAux = new xAOD::ElectronAuxContainer();
  m_newElectrons->setStore(m_newElectronsAux);

  // Deep copy for TauJets
  m_newTauJets = new xAOD::TauJetContainer();
  m_newTauJetsAux = new xAOD::TauJetAuxContainer();
  m_newTauJets->setStore(m_newTauJetsAux);

  // Shallow copy used for AntiKt4LCTopo jets. 
  // m_akt4CopyJets in xExampleEDM.h includes all jets.
  // View container provides access to selected jets
  m_akt4ViewJets = new xAOD::JetContainer(SG::VIEW_ELEMENTS);

  // Deep copy used for AntiKt4EMTopo jets
  m_aktem4NewJets = new xAOD::JetContainer();
  m_aktem4NewJetsAux = new xAOD::JetAuxContainer();
  m_aktem4NewJets->setStore(m_aktem4NewJetsAux);

  // Do NOT initialize containers for rebuilt MET
  // This is done in METRebuilder


  return StatusCode::SUCCESS;
}

StatusCode xAthExample::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

    ATH_MSG_INFO("Finished processing " << m_eventCounter << " events");
   ATH_MSG_INFO("Produced " << m_warnCounter << " retrieve warnings" );

  

  return StatusCode::SUCCESS;
}

StatusCode xAthExample::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

   if(m_eventCounter==0) m_startTime = time(NULL); //time in seconds

   if( (m_eventCounter % 100) ==0 ){
    Info("execute()", "Event number = %i", m_eventCounter );
  }
  m_eventCounter++;


  ////////////
  // EventInfo
  ////////////

  const xAOD::EventInfo* eventInfo = 0;
  if( !evtStore()->retrieve( eventInfo, "EventInfo").isSuccess() ){
    ATH_MSG_ERROR("Failed to retrieve EventInfo collection. Exiting." );
    return StatusCode::FAILURE;
  }

  bool isMC = false;
  if( eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION) ) isMC = true;
  
  // Get relative event weights, necessary for JZxW jets, etc.
  // Overall weight is sigma*efficiency/Nevent, where sigma and filter
  // efficiency are gotten from AMI

  float wt = 1;
  const std::vector< float > weights = eventInfo->mcEventWeights();
  if( weights.size() > 0 ) wt = weights[0];


  // Pileup
  m_TH1["intactual"]->Fill(eventInfo->actualInteractionsPerCrossing(), wt);
  m_TH1["intaverage"]->Fill(eventInfo->averageInteractionsPerCrossing(), wt);
  m_TH1["intdiff"]->Fill(eventInfo->actualInteractionsPerCrossing() - 
                         eventInfo->averageInteractionsPerCrossing(), wt);


  ///////////////////
  // Primary vertices
  ///////////////////

  // Get primary vertices. First vertex has \max \sum p_T^2?
  // Should cache \sum pt^2 -- xAOD::Vertex only has multiplicity

  const xAOD::VertexContainer* pVertices = 0;
  if( !evtStore()->retrieve(pVertices,"PrimaryVertices") ){
    ATH_MSG_ERROR("Failed to retrieve PrimaryVertices. Exiting." );
    return StatusCode::FAILURE;
  }

  // Assume first vertex is correct one
  const xAOD::Vertex* pVertex = 0;
  if( pVertices->size() > 0 ){
    pVertex = (*pVertices)[0];
  } else {
    ATH_MSG_ERROR("Failed to find primary vertex. Exiting" );
    return StatusCode::FAILURE;
  }

  // Check \sum pt^2 for vertices. Will be incorrect if tracks have been
  // thinned, so do not replace primary vertex choice.
  // Need to check validity of *all* ElementLinks. Otherwise code may die
  // when run on derived xAOD samples.

  // Note nv2 and nv5 will be *wrong* if tracks are thinned.

  int iVertex = -1;
  float pt2PV = 0;
  int nv2 = 0;
  int nv5 = 0;

  xAOD::VertexContainer::const_iterator vtxItr = pVertices->begin();
  xAOD::VertexContainer::const_iterator vtxItrE = pVertices->end();

  int ivtx = -1;
  for(; vtxItr!=vtxItrE; ++vtxItr){
    ++ivtx;
    int ntrkv = (*vtxItr)->nTrackParticles();
    if( ntrkv == 0 ) continue;
    if( ntrkv > 1 ) ++nv2;
    if( ntrkv > 4 ) ++nv5;
    // Find primary vertex - FIXME.
    const std::vector< ElementLink<xAOD::TrackParticleContainer> > vxTracks = 
      (*vtxItr)->trackParticleLinks();
    float ptsum = 0;
    for(unsigned int it=0; it<vxTracks.size(); ++it){
      if( (vxTracks[it]).isValid() ){
       // Dereference an ElementLink to get a pointer to the object
        const xAOD::TrackParticle* trkit = *(vxTracks[it]);
        if( trkit ) ptsum += trkit->p4().Perp2();
      }
    }
    if( ptsum > pt2PV ){
      pt2PV = ptsum;
      iVertex = ivtx;
    }
  }

  float z0PV = pVertex->z();
  int ntrkVertex = pVertex->nTrackParticles();

  // Fill histograms
  m_TH1["privtxntrk"]->Fill(ntrkVertex, wt);
  m_TH1["privtxidx"]->Fill(std::min(iVertex,49), wt);
  m_TH1["privtxz0"]->Fill(z0PV, wt);
  m_TH1["privtxn2"]->Fill(nv2, wt);
  m_TH1["privtxn5"]->Fill(nv5, wt);


  //////////////////
  // Truth particles
  //////////////////

  // Truth information in xAOD has linked TruthParticle and TruthVertex
  // containers. Particles have parent and decay vertices.
  // Must follow sequence of decays to find all descendants.

  const xAOD::TruthParticleContainer* truths = 0;

  // Following mainly relevant to ttbar events
  std::vector< const xAOD::TruthParticle* > bHadrons;
  std::vector< TLorentzVector > wLeptons;
  std::vector< TLorentzVector > wElectrons;
  std::vector< TLorentzVector > wMuons;
  std::vector< TLorentzVector > wTauJets;
  std::vector< const xAOD::TruthParticle* > wkids;

  if( isMC ) {
    double eTruth = 0;
    int nTruth = 0;

    // Note TruthParticle, NOT TruthParticles
    // Need contains() test to avoid warning message for each event.
    if( evtStore()->contains<xAOD::TruthParticleContainer>("TruthParticle") ){
      if( !evtStore()->retrieve( truths, "TruthParticle" ) ){
        ATH_MSG_ERROR("Failed to retrieve TruthParticle. Exiting." );
        return StatusCode::FAILURE;
      }
    } else {
      if( m_warnCounter < m_warnLimit ){
        ATH_MSG_WARNING("Did not find TruthParticle.");
      }
      ++m_warnCounter;
    }

    // MC samples may not contain truth
    if( truths ){
      xAOD::TruthParticleContainer::const_iterator tpItr = truths->begin();
      xAOD::TruthParticleContainer::const_iterator tpItrE = truths->end();
    
      // Stable particles 
      // Not for PowhegPythia ttbar!
      // Powheg has detached W's with status==3 children and no
      // stable descendants. Remove status!=1 and no decayVtx().
    
      for( ; tpItr != tpItrE; ++tpItr ) {
  
        // (*tpItr)->m() gives ZERO - FIXME.
  
        // Generator stable particles have  status==1 && barcode<100000
        // Other status==1 particles are from pileup or Geant
  
        bool isGS = (*tpItr)->status()==1 && (*tpItr)->barcode()<100000;
  
        if( isGS ){
          m_TH1["tppt"]->Fill( (*tpItr)->pt()/1000., wt);
          m_TH1["tpptlo"]->Fill( (*tpItr)->pt()/1000., wt);
          if( (*tpItr)->pt() > 0 ) m_TH1["tpeta"]->Fill( (*tpItr)->eta(), wt);
          ++nTruth;
          eTruth += (*tpItr)->e();
        } //end isGS
  
        // Stable particles with decay vertex give Geant secondaries
        // Plot vertex location and particle type to see what is being kept
  
        if( isGS && (*tpItr)->hasDecayVtx() ){
          const xAOD::TruthVertex* dvtx = (*tpItr)->decayVtx();
          if( dvtx ){
            m_TH1["tpvxr"]->Fill( dvtx->perp(), wt);
            if( dvtx->perp() > 0 ) m_TH1["tpvxeta"]->Fill( dvtx->eta(), wt);
            int idabs = std::abs( (*tpItr)->pdgId() );
            // Plot all hadrons as id=99
            m_TH1["tpvxidin"]->Fill( std::min(idabs,99) );
          }
        }
  
        // B hadrons - plot and save TruthParticle*
        if( isBHadron(*tpItr) ){
          m_TH1["tpbpt"]->Fill( (*tpItr)->pt()/1000., wt);
          m_TH1["tpbeta"]->Fill( (*tpItr)->eta() , wt);
          bHadrons.push_back(*tpItr);
        }
  
        // W leptonic decays - e/mu/tau among first generation children
        // Generator specific: used mainly with PowhegPythia6 ttbar sample
        // Remove possible virtual W in B decays 
  
        if( abs((*tpItr)->pdgId()) == 24 && (*tpItr)->p4().M() > 5000 ){
          bool badW = false;
          bool hadW = true;
          const xAOD::TruthVertex* dvtx = 0;
          if( (*tpItr)->hasDecayVtx() ) (*tpItr)->decayVtx();
  
          // Has valid vertex
          if( dvtx ){
            const std::vector< ElementLink<xAOD::TruthParticleContainer> >& 
            outPart = dvtx->outgoingParticleLinks();
            int lepIda = 0;
            for(unsigned int k=0; k<outPart.size(); ++k){
              if( ! (outPart[k]).isValid() ) continue;
              const xAOD::TruthParticle* kid = *(outPart[k]);
              if( ! kid ) continue;
              int kidId = kid->pdgId();
              int kidIda = std::abs(kidId);
  
              // Remove self-decays, W -> W
              if( kidId == (*tpItr)->pdgId() ) badW = true;
              // PowhegPythia6 has W's with no real descendants
              if( kid->status() != 1 && !(kid->decayVtx()) ) badW = true;
              if( !badW ) m_TH1["tpwkid"]->Fill( kid->pdgId() , wt);
  
              // Find leptonic W decays
              if( kidIda==11 || kidIda==13 || kidIda==15 ){
                hadW = false;
                lepIda = std::abs(kidId);
                wLeptons.push_back( kid->p4() );
              }
            }
  
            // Sum non-neutrino stable descendants, including leptons,
            // photons, and hadrons from taus. 
            // getDescendants replaces HepMC descendants iterator
  
            if( !hadW && !badW ){
              getDescendants( (*tpItr), wkids );
              TLorentzVector pwvis;
              bool nuemu = false;
              for(unsigned int kk=0; kk<wkids.size(); ++kk){
                const xAOD::TruthParticle* kid = wkids[kk];
                int ida = std::abs( kid->pdgId() );
                if( ida==12 || ida==14 ) nuemu = true;
                if( kid->status()==1 && kid->barcode()<100000 && 
                ida!=12 && ida!=14 && ida!=16 ){
                  pwvis += kid->p4();
                }
              }
              
              if( lepIda==11 ) wElectrons.push_back( pwvis );
              if( lepIda==13 )  wMuons.push_back( pwvis );
              if( lepIda==15 && !nuemu ) wTauJets.push_back( pwvis );
            }
          }
        }//end id==24
      }//end tpItr
  
      // Total multiplicity and energy (Ecm hard coded)
      // Expect eTruth = Ecm, but not true for PowhegPythia6 ttbar sample
      float Ecm = 8000.;
      m_TH1["tpnum"]->Fill(nTruth, wt);
      m_TH1["tpsum"]->Fill(eTruth/1000. - Ecm, wt);
      m_TH1["tpsumlo"]->Fill(eTruth/1000. - Ecm, wt);

    }//end truths
  }//end isMC
  

  ////////
  // Muons
  ////////

  // Get the Muons from the event:
  m_muons = 0;
  if( evtStore()->contains<xAOD::MuonContainer>("Muons") ){
    if( !evtStore()->retrieve(m_muons, "Muons") ){
      ATH_MSG_ERROR("Failed to retrieve Muons. Exiting.");
      return StatusCode::FAILURE;
    }
  } else {
    if( m_warnCounter < m_warnLimit ){
      ATH_MSG_WARNING("Did not find Muons.");
    }
    ++m_warnCounter;
  }

  if( m_muons ){
    xAOD::MuonContainer::const_iterator muItr = m_muons->begin();
    xAOD::MuonContainer::const_iterator muItrE = m_muons->end();
  
    for( ; muItr != muItrE; ++muItr ) {
      m_TH1["mupt"]->Fill( (*muItr)->pt()/1000., wt);
      m_TH1["mueta"]->Fill( (*muItr)->eta() , wt);
      m_TH1["muphi"]->Fill( (*muItr)->phi()/M_PI , wt);
  
      int mutype = (*muItr)->muonType();
      m_TH1["mutype"]->Fill(mutype, wt);
      int muquality = (*muItr)->quality();
      m_TH1["muquality"]->Fill(muquality, wt);
      if( mutype!=xAOD::Muon::Combined || muquality!=xAOD::Muon::Tight) continue;
  
      // Do not have Muon CP tools yet, so just copy'
      // Would be done internally by CP correctedCopy() method.
      xAOD::Muon* tmpmu = new xAOD::Muon();
      tmpmu->makePrivateStore(**muItr);
      m_newMuons->push_back(tmpmu);
    }//end muItr
  
    // Make remaining plots from m_newMuons
  
    muItr = m_newMuons->begin();
    muItrE = m_newMuons->end();
  
    for(; muItr!=muItrE; ++muItr){
      const xAOD::Muon* amuon = (*muItr);
      if( amuon->pt() < 20000 ) continue;
  
      // Kinematics
      m_TH1["mugoodpt"]->Fill( amuon->pt()/1000., wt);
      m_TH1["mugoodeta"]->Fill( amuon->eta() , wt);
  
      // Isolation variables are track ptcone20,ptcone30,ptcone40 and
      // calorimeter etcone20,etcone30,etcone40. Do just one:
      float ptc30 = -666.;
      amuon->isolation(ptc30, xAOD::Iso::ptcone30);
      m_TH1["muptc30"]->Fill(ptc30/1000., wt);
  
      // Dereference an ElementLink to get a pointer to the object
      // Compare pt of inner detector track and muon
      const ElementLink< xAOD::TrackParticleContainer >& idlink =
        amuon->inDetTrackParticleLink();
      if( idlink.isValid() ) {
        const xAOD::TrackParticle* idtrack = *idlink;
        if( idtrack ) {
          m_TH1["muidrat"]->Fill( idtrack->pt()/amuon->pt() , wt);
        }
      }
    }//end muItr
  }//end m_muons
  
  
  ////////////
  // Electrons
  ////////////

  m_electrons = 0;
  if( evtStore()->contains<xAOD::ElectronContainer>("ElectronCollection") ){
    if( !evtStore()->retrieve(m_electrons, "ElectronCollection") ){
      ATH_MSG_ERROR("Failed to retrieve ElectronCollection. Exiting.");
      return StatusCode::FAILURE;
    }
  } else {
    if( m_warnCounter < m_warnLimit ){
      ATH_MSG_WARNING("Did not find ElectronCollection.");
    }
    ++m_warnCounter;
  }

  if( m_electrons ){
    // Electron inherits from Egamma, which contains most stuff
    // isolation for Muon but isolationValue for Electron
    // Iso for Muon but EgammaParameters for Electron

    xAOD::ElectronContainer::const_iterator eItr = m_electrons->begin();
    xAOD::ElectronContainer::const_iterator eItrE = m_electrons->end();
  
    for(; eItr != eItrE; ++eItr){
      TLorentzVector pel = (*eItr)->p4();
      m_TH1["elpt"]->Fill( (*eItr)->pt()/1000., wt);
      m_TH1["eleta"]->Fill( (*eItr)->eta() , wt);
      m_TH1["elphi"]->Fill( (*eItr)->phi()/M_PI , wt);
      m_TH1["elauthor"]->Fill( (*eItr)->author() , wt);
  
      // Select good electrons - no CP tools yet
      bool egood = false;
      if( (*eItr)->auxdata<char>("Tight") ) egood = true;
      m_TH1["elgood"]->Fill(egood, wt);
      if( !egood ) continue;
  
      // Do not have Electron CP tools yet, so just copy.
      // Would be done internally by CP correctedCopy() method.
      xAOD::Electron* tmpel = new xAOD::Electron();
      tmpel->makePrivateStore(**eItr);
      m_newElectrons->push_back(tmpel);
    }
  
    // Make remaining plots from m_newElectrons
  
    eItr = m_newElectrons->begin();
    eItrE = m_newElectrons->end();
  
    for(; eItr!=eItrE; ++eItr){
      const xAOD::Electron* anelectron = (*eItr);
      if( anelectron->pt() < 20000 ) continue;
      TLorentzVector pel = anelectron->p4();
  
      m_TH1["elgoodpt"]->Fill( anelectron->pt()/1000., wt);
      m_TH1["elgoodeta"]->Fill( anelectron->eta() , wt);
  
      m_TH1["elntrk"]->Fill( anelectron->nTrackParticles() , wt);
      float pttrk = 0;
      if( anelectron->nTrackParticles() > 0 ){
        pttrk = fabs( anelectron->trackParticle(0)->pt() );
      }
      if( pttrk>0 ) pttrk = anelectron->pt()/pttrk;
      m_TH1["eleoverp"]->Fill( pttrk , wt);
  
      // Same isolation variables as for muon but different syntax:
      // isolation for Muon but isolationValue for Electron
      // Iso for Muon but EgammaParameters for Electron
  
      float etc30 = -666.;
      anelectron->isolationValue(etc30, xAOD::Iso::etcone30);
      m_TH1["eletc30"]->Fill(etc30, wt);
  
      // Match to truth electrons from W
      // Truth 4-vector is sum of e + gammas from W descendants
      float rtrue = 0.999;
      float pttrue = -1;
      for(unsigned int kk=0; kk<wElectrons.size(); ++kk){
        float drkk = pel.DeltaR( wElectrons[kk] );
        if( drkk < rtrue ) {
          rtrue = drkk;
          pttrue = wElectrons[kk].Perp();
        }
      }
      m_TH1["eldrtrue"]->Fill(rtrue,wt);
      if( rtrue < 0.1 ) m_TH1["elptrat"]->Fill(anelectron->pt()/pttrue, wt);

    } //end for eItr
  }//end m_electrons


  //////////
  // TauJets
  //////////

  // Get the TauJets from the event:
  m_tauJets = 0;
  if( evtStore()->contains<xAOD::TauJetContainer>("TauRecContainer") ){
    if( !evtStore()->retrieve(m_tauJets, "TauRecContainer") ){
      ATH_MSG_ERROR("Failed to retrieve TauRecContainer. Exiting.");
      return StatusCode::FAILURE;
    }
  } else {
    if( m_warnCounter < m_warnLimit ){
      ATH_MSG_WARNING("Did not find TauRecContainer.");
    }
    ++m_warnCounter;
  }

  if( m_tauJets ){
    xAOD::TauJetContainer::const_iterator tauItr = m_tauJets->begin();
    xAOD::TauJetContainer::const_iterator tauItrE = m_tauJets->end();
  
    for( ; tauItr != tauItrE; ++tauItr ) {
      TLorentzVector ptau = (*tauItr)->p4();
      m_TH1["taupt"]->Fill( (*tauItr)->pt()/1000., wt);
      if( (*tauItr)->pt() < 20000 ) continue;
      m_TH1["taueta"]->Fill( (*tauItr)->eta() , wt);
      m_TH1["tauphi"]->Fill( (*tauItr)->phi()/M_PI , wt);
  
      // TauID flags
      bool tauLoose = 
        (*tauItr)->isTau( xAOD::TauJetParameters::JetBDTSigLoose );
      bool tauMedium = 
        (*tauItr)->isTau( xAOD::TauJetParameters::JetBDTSigMedium );
      bool tauTight = 
        (*tauItr)->isTau( xAOD::TauJetParameters::JetBDTSigTight );
      if( tauLoose )  m_TH1["tauid"]->Fill(1, wt);
      if( tauMedium )  m_TH1["tauid"]->Fill(2, wt);
      if( tauTight )  m_TH1["tauid"]->Fill(3, wt);
      float bdt = (*tauItr)->discriminant( xAOD::TauJetParameters::BDTJetScore );
      m_TH1["taubdt"]->Fill( bdt );
      if( !tauTight ) continue;
  
      // Remove tight Electron overlaps, DeltaR < 0.2
      float rtaue = 1.999;
      for(unsigned int ie=0; ie<m_newElectrons->size(); ++ie){
        const xAOD::Electron* elie = (*m_newElectrons)[ie];
        float dr = ptau.DeltaR(elie->p4());
        if( dr < rtaue ) rtaue = dr;
      }
      m_TH2["taudrelvpt"]->Fill(ptau.Perp()/1000.,rtaue,wt);
      if( rtaue < 0.2 ) continue;
  
      // Do not have TauJet CP tools yet, so just copy.
      // Would be done internally by CP correctedCopy() method.
      xAOD::TauJet* tmptau = new xAOD::TauJet();
      tmptau->makePrivateStore(**tauItr);
      m_newTauJets->push_back(tmptau);
  
    }//end for tauItr

    // Make remaining plots from m_newTauJets
  
    tauItr = m_newTauJets->begin();
    tauItrE = m_newTauJets->end();
  
    for(; tauItr!=tauItrE; ++tauItr){
      if( (*tauItr)->pt() < 40000 ) continue;
      TLorentzVector ptau = (*tauItr)->p4();
  
      m_TH1["taugoodpt"]->Fill( (*tauItr)->pt()/1000., wt);
      m_TH1["taugoodeta"]->Fill( (*tauItr)->eta() , wt);
      float bdt = (*tauItr)->discriminant( xAOD::TauJetParameters::BDTJetScore );
      m_TH1["taugoodbdt"]->Fill( bdt );
  
      // Exercise tracks
      // tau->nTracks() gives segfault. FIXME.
      // Still true in _r5591_r5625
      /***********
      m_TH1["taugoodntrk"]->Fill( (*tauItr)->nTracks() );
      std::vector< ElementLink<xAOD::TrackParticleContainer> > tautrks =
        (*tauItr)->trackLinks();
      for(unsigned int it=0; it<tautrks.size(); ++it){
        if( !(tautrks[it]).isValid() ) continue;
        const xAOD::TrackParticle* trk = *(tautrks[it]);
        if( trk ) m_TH1["taugoodptrtrk"]->Fill( trk->pt()/(*tauItr)->pt(), wt);
      }
      ***********/
  
      // Match to W -> hadtau nu
      float rtrue = 0.999;
      float pttrue = -1;
      for(unsigned int kk=0; kk<wTauJets.size(); ++kk){
        if( wTauJets[kk].Perp()<=0 ) continue;
        float drkk = ptau.DeltaR( wTauJets[kk] );
        if( drkk < rtrue ) {
          rtrue = drkk;
          pttrue = wTauJets[kk].Perp();
        }
      }
  
      // Match to truth W -> tau nu, !tau -> e,mu
      // Sum all isGenStable decay products other than neutrinos
      m_TH1["taudrtrue"]->Fill(rtrue, wt);
      if( rtrue<0.1 ) m_TH1["tauptrat"]->Fill( (*tauItr)->pt()/pttrue );
  
    }//end for tauItr
  
    for(unsigned int i=0; i<wTauJets.size(); ++i){
      m_TH1["tautruept"]->Fill( wTauJets[i].Perp()/1000. );
      if( wTauJets[i].Perp()<= 0 ) continue;
      m_TH1["tautrueeta"]->Fill( wTauJets[i].PseudoRapidity() );
    }
  }//end m_tauJets
  

  ////////////////////
  // AntiKt4Truth Jets
  ////////////////////

  m_akt4TruthJets = 0;
  if( isMC ){
    if( evtStore()->contains<xAOD::JetContainer>("AntiKt4TruthJets") ){
      if( !evtStore()->retrieve(m_akt4TruthJets, "AntiKt4TruthJets") ){
        ATH_MSG_ERROR("Failed to retrieve AntiKt4TruthJets. Exiting.");
        return StatusCode::FAILURE;
      }
    } else {
      if( m_warnCounter < m_warnLimit ){
        ATH_MSG_WARNING("Did not find AntiKt4TruthJets.");
      }
      ++m_warnCounter;
    }

    if( m_akt4TruthJets ){
      xAOD::JetContainer::const_iterator aktT4Itr = m_akt4TruthJets->begin();
      xAOD::JetContainer::const_iterator aktT4ItrE = m_akt4TruthJets->end();

      for(; aktT4Itr != aktT4ItrE; ++aktT4Itr){
        TLorentzVector pjet = (*aktT4Itr)->p4();

        // Remove leptonic W decays from jets
        float rlep = 1.999;
        for(unsigned int kk=0; kk<wLeptons.size(); ++kk){
          float drkk = pjet.DeltaR( wLeptons[kk] );
          rlep = TMath::Min(rlep,drkk);
        }
        if( rlep < 0.4 ) continue;

        // Kinematics
        float ptj = (*aktT4Itr)->pt()/1000.;
        m_TH1["aktT4pt"]->Fill(ptj, wt);
        m_TH1["aktT4ptlo"]->Fill(ptj, wt);
        m_TH1["aktT4eta"]->Fill((*aktT4Itr)->eta(), wt);
        m_TH1["aktT4phi"]->Fill((*aktT4Itr)->phi()/M_PI, wt);
      }
    }//end m_akt4TruthJets
  }//end isMC


  ////////////////////////////////
  // AntiKt4LCTopo Jets and B jets
  ////////////////////////////////

  if( evtStore()->contains<xAOD::JetContainer>("AntiKt4LCTopoJets") ){
    if( !evtStore()->retrieve(m_akt4Jets, "AntiKt4LCTopoJets") ){
      ATH_MSG_ERROR("Failed to retrieve AntiKt4LCTopoJets. Exiting.");
      return StatusCode::FAILURE;
    }
  } else {
    if( m_warnCounter < m_warnLimit ){
      ATH_MSG_WARNING("Did not find AntiKt4LCTopoJets.");
    }
    ++m_warnCounter;
  }

  // Reconstructed jets
  // Set up shallow copy using variables in xExampleEDM.h

  m_badJetPtMax = 0;

  if( m_akt4Jets ){
    m_akt4CopyJets = xAOD::shallowCopyContainer( *m_akt4Jets );
    xAOD::JetContainer::iterator akt4Itr =  (m_akt4CopyJets.first)->begin();
    xAOD::JetContainer::iterator akt4ItrE = (m_akt4CopyJets.first)->end();
  
    for(; akt4Itr != akt4ItrE; ++akt4Itr){
      bool isGood = true;
      TLorentzVector pjet = (*akt4Itr)->p4();
      xAOD::JetFourMom_t p4aod = (*akt4Itr)->jetP4();
      float ptj = (*akt4Itr)->pt()/1000.;
  
      // Apply cleaning tool
      if( !m_jetCleaningTool->keep(**akt4Itr) ){
        m_TH1["akt4ptbad"]->Fill(ptj, wt);
        m_TH1["akt4etabad"]->Fill((*akt4Itr)->eta(), wt);
        isGood = false;
        // Appropriate scale for bad jet?
        m_badJetPtMax = std::max(m_badJetPtMax, ptj);
      }
  
      // Apply JetCalibration tool. 
      // Calibration already applied in valid1 samples; applying it again
      // makes JES worse. Should apply to old scale. FIXME.
      // JetArea pileup subtraction applied in mc14 samples, so omit it
      // in xExample::initialize().
      // jetP4(), setJetP4() use JetFourMom_t, which is a Math::LorentzVector,
      // not a TLorentzVector.
  
      // Shallow copy
      if( !m_akt4LCTopoCalibrationTool->applyCalibration(**akt4Itr) ){
        Error("execute()","Cannot apply shallow-copy calibration.");
        continue;
      }
      (*akt4Itr)->setJetP4("xaodscale",p4aod);

      // Link to original jets needed for METRebuilder. See setObjLink in
      // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/METUtilities
      // Must do this for EVERY jet 
      const ElementLink< xAOD::IParticleContainer > link(*m_akt4Jets,(*akt4Itr)->index());
      m_objLinkAcc(**akt4Itr) = link;

      // Calibration factors
      for(unsigned int ib=1; ib<16; ++ib){
        if( ptj>ptLim[ib] && ptj<ptLim[ib+1] ){
           std::string str = "akt4calib" + ptStr[ib];
           float calib = (*akt4Itr)->pt()/p4aod.pt();
           m_TH1[str.c_str()]->Fill(calib, wt);
        }
      }
  
      // Remove electron overlaps after calibration
      // Just omit jet from view container
      float rjete = 1.999;
      for(unsigned int ie=0; ie<m_newElectrons->size(); ++ie){
        const xAOD::Electron* elie = (*m_newElectrons)[ie];
        float dr = pjet.DeltaR(elie->p4());
        if( dr < rjete ) rjete = dr;
      }
      m_TH2["akt4drelvpt"]->Fill(pjet.Perp()/1000., rjete, wt);

      // Save jet if not removed by cleaning and does not match electron
      if( rjete > 0.2 && isGood ) m_akt4ViewJets->push_back((*akt4Itr));
  
    } //end for akt4Itr

    // Save calibrated jets in TStore for METRebuilder in evtStore()
    // Hence do NOT delete them at end

    if( !evtStore()->record(m_akt4CopyJets.first, "AntiKt4LCCalibJets") ||
        !evtStore()->record(m_akt4CopyJets.second, "AntiKt4LCCalibJetsAux.") ){
      ATH_MSG_ERROR("Failed to record AntiKt4LCCalibJets. Exiting.");
      return StatusCode::FAILURE;
    }


    // Make calibration plots with recalibrated jets
  
    xAOD::JetContainer::const_iterator akt4Itr2=(m_akt4ViewJets)->begin();
    xAOD::JetContainer::const_iterator akt4Itr2E=(m_akt4ViewJets)->end();
  
    for(; akt4Itr2!=akt4Itr2E; ++akt4Itr2){
      const xAOD::Jet* ajet = (*akt4Itr2);
      TLorentzVector pjet = ajet->p4();
      xAOD::JetFourMom_t p4aod = ajet->jetP4("xaodscale");
  
      // Match calibrated to truth jets
      // Bin in pttrue, so do before pt cut
      if( m_akt4TruthJets ){
        float drtrue = 666.;
        int itrue = -666;
        for(unsigned int it=0; it<m_akt4TruthJets->size(); ++it){
          const xAOD::Jet* jit = (*m_akt4TruthJets)[it];
          float drit = pjet.DeltaR(jit->p4());
          if( drit < drtrue ){
            drtrue = drit;
            itrue = it;
          }
        }
        if( itrue > -1 ){
          m_TH1["akt4drtruecalib"]->Fill(drtrue, wt);
          float ptjT = ((*m_akt4TruthJets)[itrue])->pt();
          float jescalib = ajet->pt()/ptjT;
          float jesaod = p4aod.pt()/ptjT;
          ptjT = ptjT/1000.;
          if( drtrue < 0.1 ){
            m_TH2["akt4jesvpt"]->Fill(ptjT,jesaod, wt);
            for(unsigned int ib=1; ib<16; ++ib){
              if(ptjT>ptLim[ib] && ptjT<ptLim[ib+1]){
                std::string str = "akt4jescalib" + ptStr[ib];
                m_TH1[str.c_str()]->Fill(jescalib, wt);
                std::string str2 = "akt4jes" + ptStr[ib];
                m_TH1[str2.c_str()]->Fill(jesaod, wt);
              }
            }
          }
        }
      }//end m_akt4TruthJets
    } //end akt4Itr2
  

    // valid1 samples are produced with calibration; applying it again
    // made JES worse. Hence akt4Itr3 with various options.
    // Default is for MC14
  
    // Make remaining plots with original jets
    //xAOD::JetContainer::const_iterator akt4Itr3 = m_akt4Jets->begin();
    //xAOD::JetContainer::const_iterator akt4Itr3E = m_akt4Jets->end();
  
    // Make remaining plots with recalibrated jets
    xAOD::JetContainer::const_iterator akt4Itr3=(m_akt4ViewJets)->begin();
    xAOD::JetContainer::const_iterator akt4Itr3E=(m_akt4ViewJets)->end();
  
    for(; akt4Itr3!=akt4Itr3E; ++akt4Itr3){
      const xAOD::Jet* ajet = (*akt4Itr3);
      TLorentzVector pjet = ajet->p4();
  
      // Remove leptonic W decays from jets
      float rlep = 1.999;
      for(unsigned int kk=0; kk<wLeptons.size(); ++kk){
        float drkk = pjet.DeltaR( wLeptons[kk] );
        rlep = std::min(rlep,drkk);
      }
      m_TH1["akt4rlep"]->Fill(rlep, wt);
      if( rlep < 0.4 ) continue;
  
      // Kinematics
      float ptj = (*akt4Itr3)->pt()/1000.;
      m_TH1["akt4pt"]->Fill((*akt4Itr3)->pt()/1000., wt);
      m_TH1["akt4ptlo"]->Fill((*akt4Itr3)->pt()/1000., wt);
      m_TH1["akt4eta"]->Fill((*akt4Itr3)->eta(), wt);
      m_TH1["akt4phi"]->Fill((*akt4Itr3)->phi()/M_PI, wt);
  
      // Apply jet cuts for remaining plots
      if( (*akt4Itr3)->pt()<20000. || fabs((*akt4Itr3)->eta())>2.5 ) continue;
  
      // Jet moment keys listed in xAODJet/JetAttributes.h
  
      // Plot JVF for all jets
      std::vector<float> jvfvec = 
        (*akt4Itr3)->getAttribute< std::vector<float> >(xAOD::JetAttribute::JVF);
  
      float jvf0 = -666;
      if( (int)jvfvec.size() > iVertex ){
         jvf0 = jvfvec[iVertex];
      } else {
        ATH_MSG_WARNING( "Invalid JVF index " << iVertex << " " << jvfvec.size());
      }
  
      // Plot primary vertex JVF inclusive and binned in ptj
      m_TH1["akt4jvf"]->Fill(jvf0, wt);
      for(unsigned int ib=1; ib<16; ++ib){
        if( ptj>ptLim[ib] && ptj<ptLim[ib+1] ){
           std::string str = "akt4jvf" + ptStr[ib];
           m_TH1[str.c_str()]->Fill(jvf0, wt);
        }
      }
  
      float jvfmax = -666;
      float jvfsum = 0;
      //Dummy vertex at end?
      for(unsigned int ii=0; ii<jvfvec.size(); ++ii){
        jvfsum += jvfvec[ii];
        if( jvfvec[ii] > jvfmax ) jvfmax = jvfvec[ii];
      }
  
      // Last primary vertex is dummy
      m_TH1["akt4jvftot"]->Fill(jvfsum, wt);
      for(unsigned int ii=0; ii<jvfvec.size()-1; ++ii ){
        if( (int)ii != iVertex ) m_TH1["akt4jvfx"]->Fill(jvfvec[ii] , wt);
      }
  
      // Truth flavor
      int trueid = (*akt4Itr3)->getAttribute<int>("TruthLabelID");
      m_TH1["akt4trueid"]->Fill(trueid, wt);
      if( trueid == 5 ){
        m_TH1["akt4ptb"]->Fill((*akt4Itr3)->pt()/1000., wt);
        m_TH1["akt4etab"]->Fill((*akt4Itr3)->eta(), wt);
      }
  
      // Flavor tagging
      // "Probabilities" for SV1 and IP3D make no sense - FIXME.
      // MV1 is always zero - FIXME.
      const xAOD::BTagging* btag =(*akt4Itr3)->btagging();
      double mv1 = btag->MV1_discriminant();
      m_TH1["akt4mv1"]->Fill(mv1, wt);
      m_TH1["akt4sv1b"]->Fill(btag->SV1_pb(), wt);
      m_TH1["akt4sv1u"]->Fill(btag->SV1_pu(), wt);
      m_TH2["akt4sv1uvb"]->Fill(btag->SV1_pb(),btag->SV1_pu(), wt);
  
      // Associated tracks
      // Use only tracks with at least 1pixel/6SCT hits
      // With pileup need Delta z0 cut
      // Does not work with thinned tracks - FIXME.

      std::vector<const xAOD::IParticle*> jtracks;
      jtracks = (*akt4Itr3)->getAssociatedObjects<xAOD::IParticle>( xAOD::JetAttribute::GhostTrack );
      int ntracks = 0;
      TLorentzVector psum(0,0,0,0);
      for(unsigned int jj=0; jj<jtracks.size(); ++jj){
        const xAOD::TrackParticle* trkjj = dynamic_cast<const xAOD::TrackParticle*>(jtracks[jj]);
        if( trkjj->pt() < 1000 ) continue;
        uint8_t npix = 0;
        uint8_t nsct = 0;
        if( !trkjj->summaryValue(npix,xAOD::numberOfPixelHits)) continue;
        if( !trkjj->summaryValue(nsct,xAOD::numberOfSCTHits)) continue;
        m_TH1["akt4nsitrk"]->Fill(npix+nsct, wt);
        if( npix<1 || nsct<6 ) continue;
        // z for track and primary vertex in different frames 
        float dz0 = trkjj->z0()+trkjj->vz() - z0PV;
        m_TH1["akt4dz0trk"]->Fill( dz0, wt);
        if( std::abs( dz0*sin(trkjj->theta())) > 1.5 ) continue;
        ++ntracks;
        TLorentzVector ptrk = trkjj->p4();
        psum += ptrk;
        m_TH1["akt4rtrk"]->Fill( pjet.DeltaR(ptrk) , wt);
      }
  
      // Protect against occasional track pt >> 1TeV
      double rat = psum.Pt() / (*akt4Itr3)->pt();
      rat = std::min(rat,3.0);
      for(unsigned int ib=1; ib<16; ++ib){
        if( ptj>ptLim[ib] && ptj<ptLim[ib+1] ){
           std::string str = "akt4rtrk" + ptStr[ib];
           m_TH1[str.c_str()]->Fill(rat, wt);
        }
      }
      m_TH1["akt4ntrk"]->Fill(ntracks , wt);
  
    } //end for akt4Itr3
  }//end m_akt4Jets


  /////////////////////
  // AntiKt4EMTopo Jets
  /////////////////////

  m_aktem4Jets = 0;
  if( evtStore()->contains<xAOD::JetContainer>("AntiKt4EMTopoJets") ){
    if( !evtStore()->retrieve(m_aktem4Jets, "AntiKt4EMTopoJets") ){
      ATH_MSG_ERROR("Failed to retrieve AntiKt4EMTopoJets. Exiting.");
      return StatusCode::FAILURE;
    }
  } else {
    if( m_warnCounter < m_warnLimit ){
      ATH_MSG_WARNING("Did not find AntiKt4EMTopoJets.");
    }
    ++m_warnCounter;
  }

  if( m_aktem4Jets ){
    xAOD::JetContainer::const_iterator aktem4Itr = m_aktem4Jets->begin();
    xAOD::JetContainer::const_iterator aktem4ItrE = m_aktem4Jets->end();
  
    // This is a simple example utilizing a deep copy for jets to count
    // uncalibrated and calibrated jets with pt > 100GeV.
    // NOT an efficient way to make this trivial plot.
  
    int njet100 = 0;
    for(; aktem4Itr != aktem4ItrE; ++aktem4Itr){
      TLorentzVector pjet = (*aktem4Itr)->p4();
      xAOD::JetFourMom_t p4aod = (*aktem4Itr)->jetP4();
      if( (*aktem4Itr)->pt() > 100000 ) ++njet100;
  
      // Make the deep copy. MUST initialize pointer to 0.
      xAOD::Jet* calibJet = 0;
      m_akt4EMTopoCalibrationTool->calibratedCopy(**aktem4Itr, calibJet);
      calibJet->setJetP4("xaodscale",p4aod);
  
      // ALWAYS give ownership to a DataVector, which can delete properly.
      // "delete calibJet" does NOT work.
      m_aktem4NewJets->push_back(calibJet);
  
      // Delete unwanted jet. STL methods for removing items from vectors
      // are overloaded to work properly.
      if( calibJet->pt() < 100000 ) m_aktem4NewJets->pop_back();
    }
  
    m_TH1["aktem4n100"]->Fill(njet100, wt);
    m_TH1["aktem4ncalib100"]->Fill(m_aktem4NewJets->size(), wt);
  }//end m_aktem4Jets


  /////////////
  // Missing ET
  /////////////

  // First just plot (uncalibrated) MET from xAOD

  const xAOD::MissingETContainer* METFinals = 0;
  if( evtStore()->contains<xAOD::MissingETContainer>("MET_RefFinal") ){
    if( !evtStore()->retrieve(METFinals, "MET_RefFinal") ){
      ATH_MSG_ERROR("Failed to retrieve MET_RefFinal. Exiting.");
      return StatusCode::FAILURE;
    }
  } else {
    if( m_warnCounter < m_warnLimit ){
      ATH_MSG_WARNING("Did not find MET_RefFinal.");
    }
    ++m_warnCounter;
  }

  const xAOD::MissingETContainer* METTruths = 0;
  if( evtStore()->contains<xAOD::MissingETContainer>("MET_Truth") ){
    if( !evtStore()->retrieve(METTruths, "MET_Truth") ){
      ATH_MSG_ERROR("Failed to retrieve MET_Truth. Exiting.");
      return StatusCode::FAILURE;
    }
  } else {
    if( m_warnCounter < m_warnLimit ){
      ATH_MSG_WARNING("Did not find MET_Truth.");
    }
    ++m_warnCounter;
  }

  // Each MissingETContainer has multiple versions labeled by strings
  // See METReaderAlg.cxx. Examples include
  // (*METFinals)["Final"]              (*METTruths)["NonInt"]
  // (*METFinals)["RefEle"]             (*METTruths)["Int"]
  // (*METFinals)["RefGamma"]           (*METTruths)["IntOut"]
  // (*METFinals)["RefTau"]             (*METTruths)["IntMuons"]
  // (*METFinals)["RefJet"]
  // (*METFinals)["Muons"]
  // (*METFinals)["SoftClus"]
  // (*METFinals)["SoftClus"]

  const xAOD::MissingET* metRefFinal = 0;
  if( METFinals ) metRefFinal = (*METFinals)["Final"];
  const xAOD::MissingET* metTruth = 0;
  if( METTruths ) metTruth = (*METTruths)["NonInt"];

  if( metRefFinal ){
    m_TH1["metet"]->Fill( metRefFinal->met()/1000., wt);
    m_TH1["metetlo"]->Fill( metRefFinal->met()/1000., wt);
    m_TH1["metsum"]->Fill( metRefFinal->sumet()/1000., wt);
  }

  if( metTruth ){
    m_TH1["mettrueet"]->Fill(metTruth->met()/1000., wt);
    m_TH1["mettrueetlo"]->Fill(metTruth->met()/1000., wt);
    m_TH1["mettruesum"]->Fill(metTruth->sumet()/1000., wt);
  }

  // Recalculate MET with calibrated jets
  // Remaining inputs are default - see initialize()

  if( !m_metRebuilder->execute() ){
    ATH_MSG_ERROR("Failed to execute METRebuilder. Exiting.");
    return StatusCode::FAILURE;
  }

  // Retrieve new MET - name set in initialize().

  m_metCalibs = 0;
  if( !evtStore()->retrieve(m_metCalibs, "MET_CalibFinal") ){
    ATH_MSG_ERROR("Failed to retrieve MET_CalibFinal. Exiting.");
    return StatusCode::FAILURE;
  }
  const xAOD::MissingET* metCalibFinal = 0;
  if( m_metCalibs ) metCalibFinal = (*m_metCalibs)["Final"];

  // Histogram new MET
  if( metCalibFinal ){
    m_TH1["metcalibet"]->Fill( metCalibFinal->met()/1000., wt);
    m_TH1["metcalibetlo"]->Fill( metCalibFinal->met()/1000., wt);
    m_TH1["metcalibsum"]->Fill( metCalibFinal->sumet()/1000., wt);
  }


  ////////////////////////
  // Clear copy containers
  ////////////////////////

  // Deep copies. Clearing containers deletes contents including AuxStore.
  m_newMuons->clear();
  m_newElectrons->clear();
  m_newTauJets->clear();

  // For AntiKt4LCTopo shallow copy
  // Clearing view container does not delete contents
  // m_akt4CopyJets saved in TStore so NOT deleted.
  m_akt4ViewJets->clear();

  // For AntiKt4EMTopo deep copy
  // Clearing container deletes contents
  m_aktem4NewJets->clear();



  // Close with a message:
  if( ((m_eventCounter-1) % 100) == 0 ){
    Info( "execute()",
          "===>>>  done processing event #%i, "
          "run #%i %i events processed so far  <<<===",
          static_cast< int >( eventInfo->eventNumber() ),
          static_cast< int >( eventInfo->runNumber() ),
          static_cast< int >( m_eventCounter ) );
    Info("execute()","Processing rate = %f Hz", float(m_eventCounter)/(time(NULL)-m_startTime));
  }


  return StatusCode::SUCCESS;
}


