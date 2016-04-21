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

#define printUsage(ARG) MSG_ERROR( "Usage:  [--file/-f /path/to/input/file] [--runno/-r run number] [--eta/e eta] [--pt/-p pt in Mev]" \
				   << "[--type/-t FullSim/AtlFast2] [--useDefaults/-d] [--compact/-c]" << ARG ); 


xAOD::CaloCluster* create_cluster(float eta, float phi,float e);

int main( int argc, char* argv[] ) {

  // The application's name:
  const char* APP_NAME = argv[ 0 ];
  //Set the message level
  MSG::Level mylevel=MSG::DEBUG;
  MSG::Level mylevelToy=MSG::DEBUG;//MSG::FATAL;

  MSGHELPERS::getMsgStream().msg().setLevel(mylevel); 
  MSGHELPERS::getMsgStream().msg().setName(APP_NAME); 

  bool useCompactDisplay = true;
  
  std::string fileName="";
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
  p.addOption("compact","c",false);
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
  tmp = p.getArg("e");
  if (tmp != "")
    eta = atof(tmp.Data());
  else
    {
      MSG_INFO("No eta value given");
      printUsage(argv[0]);
      return 0;
    }
  tmp = p.getArg("p");
  if (tmp != "")
    pt = atof(tmp.Data());
  else
    {
      MSG_INFO("No pt value given");
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
  bool test = p.getBool("c");
  useCompactDisplay = test;
   
  PATCore::ParticleDataType::DataType SimType;
  if (type=="FullSim")
    SimType =  PATCore::ParticleDataType::Full;
  else if (type=="AtlFast2")
    SimType =  PATCore::ParticleDataType::Fast;
  else {
    MSG_INFO("Cannot parse SimType, use either Full or AtlFast2!");
    return 0;
  }

  //    useCompactDisplay = true;

  // Initialise the application:
  CHECK( xAOD::Init( APP_NAME ) );

  // Create a TEvent object:
  //    //xAOD::TEvent event( xAOD::TEvent::kBranchAccess );
  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  xAOD::EventInfo* ei= new xAOD::EventInfo();
  ei->makePrivateStore();
  ei->setRunNumber(runno);
  ei->setEventNumber(363636);
  xAOD::TStore store;
  store.record(ei, "EventInfo");

  if (!useCompactDisplay){
    MSG_INFO("Creating new EECTool");
  }
  //Likelihood
  AsgElectronEfficiencyCorrectionTool myEgCorrections ("myEgCorrections");
  AsgElectronEfficiencyCorrectionTool myEgCorrectionsToys ("myEgCorrectionsToys");
  
 myEgCorrections.msg().setLevel(MSG::DEBUG);
 myEgCorrectionsToys.msg().setLevel(MSG::DEBUG);

  if (!useCompactDisplay){
    MSG_INFO("Adding File: "<<fileName);
  }
  std::vector<std::string> inputFiles{fileName} ;

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

  double  e  =(pt * cosh(eta));
  //   double e = pt/TMath::Sin(eta);
  xAOD::Electron* el = new xAOD::Electron();
  el->makePrivateStore();
  m_electrons->push_back(el);
 
  xAOD::CaloCluster* cluster = create_cluster(eta, 0.0, e);
  m_clusters->push_back(cluster);
  store.record( m_clusters, "MyClusters" );
  store.record( m_clAux, "MyClustersAux." );

  
  links_clusters.push_back(ElementLink< xAOD::CaloClusterContainer >( cluster, *m_clusters ));
  el->setCaloClusterLinks(links_clusters);
  xAOD::TrackParticle* track = new xAOD::TrackParticle();
  track->makePrivateStore();
  track->setDefiningParameters(0., 0., 0.0, 2 * atan(exp(-eta)), 1.);
  m_tracks->push_back(track);
  std::vector< ElementLink< xAOD::TrackParticleContainer > > links_tracks;
  store.record( m_tracks, "MyTrackParticles" );
  store.record( m_tracksAux, "MyTrackParticlesAux." );

  el->setTrackParticleLinks(links_tracks);
  el->setEta(eta);
  el->setPhi(0.0);
  el->setM(0);
  el->setPt(e / cosh(eta));
  store.record( m_electrons, "MyElectrons" );
  store.record( m_electronsAux, "MyElectronsAux." );


  CHECK( myEgCorrections.setProperty("CorrectionFileNameList",inputFiles) );
  CHECK( myEgCorrections.setProperty("ForceDataType",(int)SimType) );
//  CHECK( myEgCorrections.setProperty("CorrelationModel", "TOTAL" ));
  myEgCorrections.msg().setLevel(mylevel);

  CHECK( myEgCorrectionsToys.setProperty("CorrectionFileNameList",inputFiles) );
  CHECK( myEgCorrectionsToys.setProperty("ForceDataType",(int)SimType) );
 // CHECK( myEgCorrectionsToys.setProperty("CorrelationModel", "COMBMCTOYS" ));
  myEgCorrectionsToys.msg().setLevel(mylevelToy);

  
  if (!useCompactDisplay){
    MSG_INFO("Initializing EECTools");
  }
  CHECK( myEgCorrections.initialize() );
  CHECK( myEgCorrectionsToys.initialize() );
  
  if(!useCompactDisplay) {
    MSG_INFO(el->pt());
  }

  double SF = 0; 
  double SFToys = 0; 
  std::vector<double> uncToys;
  std::vector<double> unc;

  // Get a list of systematics
  CP::SystematicSet recSysts = myEgCorrections.recommendedSystematics();
  CP::SystematicSet recSystsToys = myEgCorrectionsToys.recommendedSystematics();
   
  // Convert into a simple list
  std::vector<CP::SystematicSet> sysList = CP::make_systematics_vector(recSysts);
  
  //std::vector<CP::SystematicSet> sysListToys;
  CP::MakeSystematicsVector sysListToys;
  sysListToys.addGroup("toys");
  sysListToys.setToys(  myEgCorrectionsToys.getNumberOfToys( )   );
   
  sysListToys.calc(recSystsToys);
  std::vector<CP::SystematicSet> sysListToys2=sysListToys.result("toys");
  
  if(myEgCorrections.getEfficiencyScaleFactor(*el,SF) != CP::CorrectionCode::Ok){
    MSG_ERROR( APP_NAME << "Problem in getEfficiencyScaleFactor");
    return EXIT_FAILURE;
  }
  if(myEgCorrectionsToys.getEfficiencyScaleFactor(*el,SFToys) != CP::CorrectionCode::Ok){
    MSG_ERROR( APP_NAME << "Problem in getEfficiencyScaleFactorToys");
    return EXIT_FAILURE;
  }
 
  std::cout << "hello1" << std::endl;

  // Loop over systematics
  for(const auto& sys : sysList){
    double systematic = 0; 
    
    if(!useCompactDisplay)  {
      MSG_INFO(APP_NAME<<" " << " Processing syst: " << sys.name().c_str());
    }
    
    // Configure the tool for this systematic
    CHECK( myEgCorrections.applySystematicVariation(sys) );

    // if(!useCompactDisplay)     {   
       MSG_ERROR(APP_NAME<<" " << "Applied syst (FULL):  " <<myEgCorrections.appliedSystematics().name().c_str());
   //  }
    
    if(myEgCorrections.getEfficiencyScaleFactor(*el,systematic) != CP::CorrectionCode::Ok){
      MSG_ERROR( APP_NAME << "Problem in getEfficiencyScaleFactor");
      return EXIT_FAILURE;
    }
std::cout << myEgCorrections.appliedSystematics().name().c_str()<<"   "<<systematic << std::endl;  
    unc.push_back(systematic);
  }
  // Loop over systematics
 
  /// DO TOY LOOP
  for(const auto& sysToys : sysListToys2){
    double systematicToys = 0; 
    
    if(!useCompactDisplay)  {
      // MSG_WARNING(APP_NAME<<" " << " Processing syst: " << sysToys.name().c_str());
    }

    // Configure the tool for this systematic
    CHECK( myEgCorrectionsToys.applySystematicVariation(sysToys) );

    if(!useCompactDisplay)     {   
      // MSG_WARNING(APP_NAME<<" " << "Applied syst:  "<< toyIndex << "  "<< toy_scale);   
  //       MSG_WARNING(APP_NAME<<" " << "Applied syst:  " <<myEgCorrectionsToys.appliedSystematics().name().c_str());
    }
    
    if(myEgCorrectionsToys.getEfficiencyScaleFactor(*el,systematicToys) != CP::CorrectionCode::Ok){
      //    MSG_ERROR( APP_NAME << "Problem in getEfficiencyScaleFactor");
      return EXIT_FAILURE;
    }
    
    if(!useCompactDisplay) {
      //    MSG_INFO(APP_NAME<<"itoys values " << systematicToys);
    }
    uncToys.push_back(systematicToys);
  }
  	 
  if (unc.size()==5) {
    std::cout <<" "  <<  SF 
	      <<" $\\pm$  " << unc.at(0) 
	      <<" $\\pm$  " << unc.at(1) <<" $\\pm$  " <<  unc.at(2) 
	      <<" $\\pm$  " <<  unc.at(3) <<" $\\pm$  " <<  unc.at(4) <<std::endl;
  }

  if (unc.size()==3 ) {
    std::cout <<" "  <<  unc.at(0)
	      << " $\\pm$  " << unc.at(0)-unc.at(1)  
	      <<" $\\pm$  " << unc.at(2)-unc.at(0) << std::endl;
  }
  //  if (useCompactDisplay)
  //      std::cout<<sf2.getScaleFactor()<< " $\\pm$ "<<sf2.getTotalUncertainty()<<" / $\\Delta$="<<sf2.getTotalUncertainty()-sqrt(val)<<std::endl;
  //
  //// calculate total uncertainty from uncorr and corr
  // double val = uncorr*uncorr;

  // for (Int_t i=test2->getFirstCorrSysPosition(); i<=(test2->getLastCorrSysPosition()); i++)
  // {
  //   if (!useCompactDisplay)
  //     std::cout<<"corr "<<i<<": "<<sf2.getResult(i)<<std::endl;
  //   val+= sf2.getResult(i)*sf2.getResult(i);
  // }
  // for (Int_t i=test2->getFirstToyMCPosition(); i<=(test2->getLastToyMCPosition()); i++)
  // {
  //   if (!useCompactDisplay)
  //     std::cout<<"toy "<<i<<": "<<sf2.getResult(i)<<std::endl;
  // }


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
  return cluster;
}
