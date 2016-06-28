/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <memory>
#include <cstdlib>
#include <iomanip>
#include <string>
#include "CmdParser.h"
#include "Messaging.h"


// ROOT include(s):
#include <TFile.h>
#include <TString.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/Egamma.h"
#include "xAODCore/ShallowCopy.h"
#include <xAODCaloEvent/CaloCluster.h>
#include <xAODCaloEvent/CaloClusterContainer.h>
#include <xAODCaloEvent/CaloClusterAuxContainer.h>
#include <xAODEgamma/ElectronContainer.h>
#include <xAODEgamma/ElectronAuxContainer.h>
#include <xAODTracking/TrackParticleContainer.h>
#include <xAODTracking/TrackParticleAuxContainer.h>	

//Asg includes
#include "AsgTools/AsgMessaging.h"
#include "PATInterfaces/SystematicsUtil.h"
#include "PATInterfaces/MakeSystematicsVector.h"
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"


#define CHECK( ARG )						\
  do {								\
    const bool result = ARG;					\
    if( ! result ) {						\
      MSG_ERROR( APP_NAME << "FAILED  to execute" <<#ARG);	\
      return EXIT_FAILURE;					\
    }								\
  } while( false )

#define printUsage(ARG) MSG_ERROR( "Usage:  [--file/-f /path/to/input/file] [--runno/-r run number] [--corr/c correlationModel] " \
				   << "[--type/-t FullSim/AtlFast2] [--useDefaults/-d]" << ARG ); 


xAOD::CaloCluster* create_cluster(float eta, float phi,float e);

int main( int argc, char* argv[] ) {

  // The application's name:
  const char* APP_NAME = argv[ 0 ];
  //Set the message level
  MSG::Level mylevel=MSG::ERROR;
//  MSG::Level mylevelToy=MSG::DEBUG;//MSG::FATAL;

  MSGHELPERS::getMsgStream().msg().setLevel(mylevel); 
  MSGHELPERS::getMsgStream().msg().setName(APP_NAME); 

  bool useCompactDisplay = true;
  
  std::string fileName="";
std::string corrmodel="";
  int runno=-1;
  float eta=-999;
  float pt = -1;
  TString type;

  CmdParser p;
  p.addOption("file","f");
  p.addOption("runno","r");
  p.addOption("eta","e");
  p.addOption("pt","p");
  p.addOption("type","t");
  p.addOption("corr","c");
  p.Init(argc,argv);

  TString tmp = p.getArg("f");

  if (tmp != "")
    fileName = tmp;
  else
    {
      MSG_INFO("No file name given");
      printUsage(argv[0]);
      return 0;
    }
  tmp = p.getArg("r");
  if (tmp != "")
    runno = atoi(tmp.Data());
  else
    {
      MSG_INFO("No run number given");
      printUsage(argv[0]);
      return 0;
    }
  corrmodel = p.getArg("c");
  if (!(corrmodel=="COMBMCTOYS"
	||corrmodel=="MCTOYS"
	||corrmodel=="FULL"
	||corrmodel=="SIMPLIFIED"
	||corrmodel=="TOTAL"
	||corrmodel=="SYST"
      ) )   {
      MSG_INFO("No valid correlation model given (COMBMCTOYS/MCTOYS/FULL/SIMPLIFIED/TOTAL/SYST)");
      printUsage(argv[0]);
      return 0;
    }

type = p.getArg("t");
  if (!(type=="FullSim"||type=="AtlFast2"))
    {
      MSG_INFO("No valid type given (FullSim or AtlFast2)");
      printUsage(argv[0]);
      return 0;
    }
   
  PATCore::ParticleDataType::DataType SimType;
  if (type=="FullSim")
    SimType =  PATCore::ParticleDataType::Full;
  else if (type=="AtlFast2")
    SimType =  PATCore::ParticleDataType::Fast;
  else {
    MSG_INFO("Cannot parse SimType, use either Full or AtlFast2!");
    return 0;
  }

  //  Electron Distributions ////////////////////////////////////////
std::vector<float> middleBins;
middleBins.push_back(-2.47);
middleBins.push_back(-2.37);
middleBins.push_back(-2.01);
middleBins.push_back(-1.81);
middleBins.push_back(-1.52);
middleBins.push_back(-1.37);
 middleBins.push_back(-1.15);
 middleBins.push_back(-0.8);
middleBins.push_back(-0.6);
middleBins.push_back(-0.1);
middleBins.push_back(0.0);
middleBins.push_back(0.1);
middleBins.push_back(0.6);
middleBins.push_back(0.8);
middleBins.push_back(1.15);
middleBins.push_back(1.37);
middleBins.push_back(1.52);
middleBins.push_back(1.81);
middleBins.push_back(2.01);
middleBins.push_back(2.37);
middleBins.push_back(2.47);

 std::vector<float> etBins;
  etBins.push_back(10000);
  etBins.push_back(15000);
  etBins.push_back(20000);
  etBins.push_back(25000);
  etBins.push_back(30000);
  etBins.push_back(35000);
  etBins.push_back(40000);
  etBins.push_back(45000);
 etBins.push_back(50000);
 etBins.push_back(60000);
 etBins.push_back(80000);
 etBins.push_back(100000);
 
 TH2F * elecs = new TH2F("elecs","elecs", etBins.size()-1,&(etBins.at(0)) ,middleBins.size()-1,&(middleBins.at(0))   );
 //  Electron Distributions ////////////////////////////////////////

 /// uncertainty histograms ///////////////////////
 TH2D * sigma_2D = (TH2D *)elecs->Clone("sigma_2D"); sigma_2D->Reset();
 TH1D * sigma_1D_pt = (TH1D *)elecs->ProjectionX("sigma_1D_pt",1,1); sigma_1D_pt->Reset();
 TH1D * sigma_1D_eta = (TH1D *)elecs->ProjectionY("sigma_1D_eta",1,1); sigma_1D_eta->Reset();
 TH1D * sigma_1D_tot= new TH1D("Total uncertainty","Total uncertainty",1,0,1);
 /// uncertainty histograms ///////////////////////
 
 // maps of uncertainties?
 std::map<std::string,TH1D*> syst_uncertainties_1D_pt;
 std::map<std::string,TH1D*> syst_uncertainties_1D_eta;
 std::map<std::string,TH1D*> syst_uncertainties_1D_tot;

  // Initialise the application:
  CHECK( xAOD::Init( APP_NAME ) );

  // Create a TEvent object:
  //    //xAOD::TEvent event( xAOD::TEvent::kBranchAccess );
  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  xAOD::EventInfo* ei= new xAOD::EventInfo();
  ei->makePrivateStore();
  ei->setRunNumber(runno);
  ei->setEventNumber(363636);
  
  static SG::AuxElement::Decorator<unsigned int> randomrunnumber("RandomRunNumber") ;
  randomrunnumber(*ei)= runno;

xAOD::TStore store;
  store.record(ei, "EventInfo");

  if (!useCompactDisplay){
    MSG_INFO("Creating new EECTool");
  }
  //Likelihood
  AsgElectronEfficiencyCorrectionTool myEgCorrections ("myEgCorrections");
  
 myEgCorrections.msg().setLevel(MSG::DEBUG);


 if (!useCompactDisplay){
   MSG_INFO("Adding File: "<<fileName);
 }
 std::vector<std::string> inputFiles{fileName} ;


 CHECK( myEgCorrections.setProperty("CorrectionFileNameList",inputFiles) );
 CHECK( myEgCorrections.setProperty("ForceDataType",(int)SimType) );
 CHECK( myEgCorrections.setProperty("CorrelationModel", corrmodel.c_str() ));
 myEgCorrections.msg().setLevel(mylevel);

 if (!useCompactDisplay){
   MSG_INFO("Initializing EECTools");
 }
 CHECK( myEgCorrections.initialize() );

 double SF = 0;
 std::vector<double> unc;

 // Get a list of systematics                                                                                                                    
 CP::SystematicSet recSysts = myEgCorrections.recommendedSystematics();

 // Convert into a simple list                                                                                                                   
 std::vector<CP::SystematicSet> sysList = CP::make_systematics_vector(recSysts);


 /// loop over electrons                                                                                                                         
 // for (int binx=1;binx<elecs->GetXaxis()->GetNbins();binx++) {
 //  for (int biny=1;biny<elecs->GetYaxis()->GetNbins();biny++) {

 // pt =   elecs->GetYaxis()->GetBinLowEdge(biny)+0.01 ;
 //eta =  elecs->GetXaxis()->GetBinLowEdge(binx)+0.005 ;


 /// loop over electrons                                                                                                                                                   
 for (int binx=1;binx<elecs->GetXaxis()->GetNbins();binx++) {
   for (int biny=1;biny<elecs->GetYaxis()->GetNbins();biny++) {


 //  if (!useCompactDisplay){
 // MSG_INFO("Adding File: "<<fileName);
 // }
 // std::vector<std::string> inputFiles{fileName} ;

  //"Creating calo cluster container");
  xAOD::CaloClusterContainer *m_clusters = new xAOD::CaloClusterContainer();
  xAOD::CaloClusterAuxContainer *m_clAux = new xAOD::CaloClusterAuxContainer();
  m_clusters->setStore(m_clAux);

  xAOD::TrackParticleContainer *m_tracks = new xAOD::TrackParticleContainer();
  xAOD::TrackParticleAuxContainer  *m_tracksAux = new xAOD::TrackParticleAuxContainer();
  m_tracks->setStore(m_tracksAux);

  xAOD::ElectronContainer *m_electrons = new xAOD::ElectronContainer();
  xAOD::ElectronAuxContainer *m_electronsAux = new xAOD::ElectronAuxContainer();
  m_electrons->setStore(m_electronsAux);	

  std::vector< ElementLink< xAOD::CaloClusterContainer > > links_clusters;



  /// loop over electrons                                                                                                                                      
  //  for (int binx=1;binx<elecs->GetXaxis()->GetNbins();binx++) {
  //  for (int biny=1;biny<elecs->GetYaxis()->GetNbins();biny++) {

      eta =   elecs->GetYaxis()->GetBinLowEdge(biny)+0.01 ;
      std::cout <<"hello  "<< eta << std::endl;
      pt =  elecs->GetXaxis()->GetBinLowEdge(binx)+0.005 ;
      std::cout <<"hello  "<< pt<<"  "<< eta << std::endl;



  double  e  =(pt * cosh(eta));
  //   double e = pt/TMath::Sin(eta);
  xAOD::Electron* el = new xAOD::Electron();
  el->makePrivateStore();
  m_electrons->push_back(el);

  //  m_clusters->clear(); 
  xAOD::CaloCluster* cluster = create_cluster(eta, 0.0, e);
  m_clusters->push_back(cluster);
  store.record( m_clusters, Form("MyClusters%d%d", biny,binx) );
  store.record( m_clAux, Form("MyClustersAux%d%d", biny,binx) );

  
  links_clusters.push_back(ElementLink< xAOD::CaloClusterContainer >( cluster, *m_clusters ));
  el->setCaloClusterLinks(links_clusters);
  xAOD::TrackParticle* track = new xAOD::TrackParticle();
  track->makePrivateStore();
  track->setDefiningParameters(0., 0., 0.0, 2 * atan(exp(-eta)), 1.);
  m_tracks->push_back(track);
  std::vector< ElementLink< xAOD::TrackParticleContainer > > links_tracks;
  store.record( m_tracks, Form("MyTrackParticles%d%d",biny,binx) );
  store.record( m_tracksAux, Form("MyTrackParticlesAux%d%d",biny,binx) );

  el->setTrackParticleLinks(links_tracks);
  el->setEta(eta);
  el->setPhi(0.0);
  el->setM(0);
  el->setPt(e / cosh(eta));
  store.record( m_electrons, Form("MyElectron%d%d", biny,binx) );
  //"MyElectrons" );
  store.record( m_electronsAux,  Form("MyElectronAux%d%d", biny,binx) );//"MyElectronsAux." );
  /*
//std::string corrmodel = "TOTAL";
  CHECK( myEgCorrections.setProperty("CorrectionFileNameList",inputFiles) );
  CHECK( myEgCorrections.setProperty("ForceDataType",(int)SimType) );
  CHECK( myEgCorrections.setProperty("CorrelationModel", corrmodel.c_str() ));
  myEgCorrections.msg().setLevel(mylevel);
  
  if (!useCompactDisplay){
    MSG_INFO("Initializing EECTools");
  }
  CHECK( myEgCorrections.initialize() );
  
  if(!useCompactDisplay) {
    MSG_INFO(el->pt());
  }

  double SF = 0; 
  std::vector<double> unc;

  // Get a list of systematics
  CP::SystematicSet recSysts = myEgCorrections.recommendedSystematics();
   
  // Convert into a simple list
  std::vector<CP::SystematicSet> sysList = CP::make_systematics_vector(recSysts);
*/  
  // //std::vector<CP::SystematicSet> sysListToys;
  // CP::MakeSystematicsVector sysListToys;
  // sysListToys.addGroup("toys");
  // sysListToys.setToys(  myEgCorrectionsToys.getNumberOfToys( )   );
   
  // sysListToys.calc(recSystsToys);
  // std::vector<CP::SystematicSet> sysListToys2=sysListToys.result("toys");
  
  std::cout<< el->e() << "  "<< e <<"  " << eta << "  "<< el->caloCluster()->etaBE(2)<<std::endl;
  //el->caloCluster()->etaBE(2)


   /// loop over electrons
  //  for (int binx=1;binx<elecs->GetXaxis()->GetNbins()+1;binx++) {
  //  for (int biny=1;biny<elecs->GetYaxis()->GetNbins()+1;biny++) {

  //  el->setEta(  elecs->GetYaxis()->GetBinLowEdge(biny)+0.01 );
  //  el->setPt( elecs->GetXaxis()->GetBinLowEdge(binx)+0.01 );

      if(myEgCorrections.getEfficiencyScaleFactor(*el,SF) != CP::CorrectionCode::Ok){
	MSG_ERROR( APP_NAME << "Problem in getEfficiencyScaleFactor");
	return EXIT_FAILURE;
      }
     
      /// check elec eta/pt
      MSG_ERROR(APP_NAME<<" " <<elecs->GetYaxis()->GetBinLowEdge(biny)+0.005 << "  get eta/pt:  " <<el->pt() << "  " << el->eta() );
 
sigma_1D_pt->Fill( el->pt()  , SF );
sigma_1D_eta->Fill( el->eta()  ,SF );
sigma_1D_tot->Fill( 0.5  , SF );

      ////////////////////////////////////// SYSTEMATICS LOOPING

      // Loop over systematics
      for(const auto& sys : sysList){
	double systematic = 0; 
    
	if(!useCompactDisplay)  {
	  MSG_INFO(APP_NAME<<" " << " Processing syst: " << sys.name().c_str());
	}
    
	// Configure the tool for this systematic
	CHECK( myEgCorrections.applySystematicVariation(sys) );

	// if(!useCompactDisplay)     {   
//	MSG_ERROR(APP_NAME<<" " << "Applied syst (FULL):  " <<myEgCorrections.appliedSystematics().name().c_str());
	//  }
  
    if(myEgCorrections.getEfficiencyScaleFactor(*el,systematic) != CP::CorrectionCode::Ok){
      MSG_ERROR( APP_NAME << "Problem in getEfficiencyScaleFactor");
      return EXIT_FAILURE;
    }
std::cout << myEgCorrections.appliedSystematics().name().c_str()<<"   "<<systematic << std::endl;  

 if (!syst_uncertainties_1D_pt[myEgCorrections.appliedSystematics().name()])  {syst_uncertainties_1D_pt[myEgCorrections.appliedSystematics().name()] = (TH1D*)sigma_1D_pt ->Clone(Form("%s_1D_pt" ,myEgCorrections.appliedSystematics().name().c_str()) );syst_uncertainties_1D_pt[myEgCorrections.appliedSystematics().name()] ->Reset();}
	else syst_uncertainties_1D_pt[myEgCorrections.appliedSystematics().name()]->Fill( el->pt()  , systematic );
 
 if (!syst_uncertainties_1D_eta[myEgCorrections.appliedSystematics().name()])  {syst_uncertainties_1D_eta[myEgCorrections.appliedSystematics().name()] = (TH1D*)sigma_1D_eta ->Clone(Form("%s_1D_eta" ,myEgCorrections.appliedSystematics().name().c_str()) );
   syst_uncertainties_1D_eta[myEgCorrections.appliedSystematics().name()] ->Reset();
 } 	else syst_uncertainties_1D_eta[myEgCorrections.appliedSystematics().name()]->Fill( el->eta()  , systematic );

 if (!syst_uncertainties_1D_tot[myEgCorrections.appliedSystematics().name()])  {syst_uncertainties_1D_tot[myEgCorrections.appliedSystematics().name()] = (TH1D*)sigma_1D_tot ->Clone(Form("%s_1D_tot" ,myEgCorrections.appliedSystematics().name().c_str()) );
   syst_uncertainties_1D_tot[myEgCorrections.appliedSystematics().name()]->Reset();
 }
 else syst_uncertainties_1D_tot[myEgCorrections.appliedSystematics().name()]->Fill( 0.5 , systematic );
 
}}  
}
  // Loop over systematics
  	 
      TFile * file = new TFile(Form("SysChecks_%s.root", corrmodel.c_str()),"RECREATE");
      file->cd();


sigma_1D_pt->Write();
sigma_1D_eta->Write();
sigma_1D_tot->Write();

      //// xxxxxxxx calculate uncertainty
 std::map<std::string,TH1D*>::const_iterator itr  = syst_uncertainties_1D_tot.begin();
 std::map<std::string,TH1D*>::const_iterator itrE  = syst_uncertainties_1D_tot.end();
 std::map<std::string,TH1D*>::const_iterator itrEta  = syst_uncertainties_1D_eta.begin();
 std::map<std::string,TH1D*>::const_iterator itrPt  = syst_uncertainties_1D_pt.begin();

  for ( ; itr!=itrE; itr++ ) {
    itr->second->Write();
    itrEta->second->Write();
    itrPt->second->Write();
  // for (int binx=1;binx<elecs->GetXaxis()->GetNbins()+1;binx++) {
  //   for (int biny=1;biny<elecs->GetYaxis()->GetNbins()+1;biny++) {
  //   }}

    itrEta++;
    itrPt++;
  }


  CHECK( myEgCorrections.finalize() );
  // Return gracefully:
  return 0;
}

xAOD::CaloCluster* create_cluster(float eta, float phi, float e){
  // create cluster
  xAOD::CaloCluster* cluster = new xAOD::CaloCluster();
  cluster->makePrivateStore();

  // set eta, phi for all the layers (barrel / endcap)
  const std::set<CaloSampling::CaloSample> samplings {
    CaloSampling::PreSamplerB, CaloSampling::EMB1, CaloSampling::EMB2, CaloSampling::EMB3,
      CaloSampling::PreSamplerE, CaloSampling::EME1, CaloSampling::EME2, CaloSampling::EME3 };

  unsigned sampling_pattern = 0;
  for (auto sample : samplings) { sampling_pattern |= 0x1U << sample; }
  cluster->setSamplingPattern(sampling_pattern);
  MSG_DEBUG("nsamples  = " << cluster->nSamples());
  for (auto sample : samplings) {
    cluster->setEta(sample, eta);
    cluster->setPhi(sample, phi);
  }
  
  if (std::abs(eta) < 1.45) {
    cluster->setEnergy(CaloSampling::PreSamplerB, e/4);
    cluster->setEnergy(CaloSampling::EMB1, e/4);
    cluster->setEnergy(CaloSampling::EMB2, e/4);
    cluster->setEnergy(CaloSampling::EMB3, e/4);
  }
  else {
    cluster->setEnergy(CaloSampling::PreSamplerE, e/4);
    cluster->setEnergy(CaloSampling::EME1, e/4);
    cluster->setEnergy(CaloSampling::EME2, e/4);
    cluster->setEnergy(CaloSampling::EME3, e/4);
  }
  
  
  cluster->setE(e);
  cluster->setEta(eta);
  cluster->setPhi(phi);
  cluster->auxdata<float>("etaCalo") = eta;
  cluster->auxdata<float>("phiCalo") = phi;
  cluster->insertMoment(xAOD::CaloCluster::ETACALOFRAME, eta);
  cluster->insertMoment(xAOD::CaloCluster::PHICALOFRAME, phi);
  std::cout << "create cluster " << eta << std::endl;
  return cluster;
}
