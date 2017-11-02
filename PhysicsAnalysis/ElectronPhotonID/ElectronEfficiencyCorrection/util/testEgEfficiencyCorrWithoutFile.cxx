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

#define printUsage(ARG) MSG_ERROR( "Usage:  [--file/-f /path/to/input/file] [--mapfile/-m /path/to/input/mapfile] [--runno/-r run number] [--eta/e eta] [--pt/-p pt in Mev]" \
				   << "[--keyreco/-k RecoKey] [--keyid/-d IDKey] [--keyiso/-i IsoKey] [--keytrigger/-t TriggerKey]" \
				   << "[--type/-t FullSim/AtlFast2] [--useDefaults/-d] [--compact/-c]" << ARG ); 


xAOD::CaloCluster* create_cluster(float eta, float phi,float e);

void dumbProperties(const AsgElectronEfficiencyCorrectionTool& tool){
  ///Get the properties
  MSG_INFO("=============" );  
  MSG_INFO( "Properties" );  
  auto propertyMap = tool.getPropertyMgr()->getProperties();
  for (auto  i : propertyMap){
    MSG_INFO("=============" );  
    MSG_INFO("Name : " <<i.first  <<  " Type : " <<i.second->typeName() );  
    int type= i.second->type();
    switch(type){    
    case Property::Type::BOOL :{
      bool value = *(tool.getProperty<bool>(i.first));
      MSG_INFO("Value : " <<value );        
      break;      
    }
    case Property::Type::INT:{
      int value = *(tool.getProperty<int>(i.first));
      MSG_INFO("Value : " <<value );        
      break;
    }
    case Property::Type::FLOAT :{
      float value = *(tool.getProperty<float>(i.first));
      MSG_INFO("Value : " <<value );        
      break;
    }
    case Property::Type::DOUBLE :{
      double value = *(tool.getProperty<double>(i.first));
      MSG_INFO("Value : " <<value );        
      break;
    }
    case Property::Type::STRING :{
      std::string value = *(tool.getProperty<std::string>(i.first));
      MSG_INFO("Value : " <<value );        
      break;
    }
    case Property::Type::STRINGVECTOR :{
      std::vector<std::string> value= *(tool.getProperty< std::vector<std::string> > (i.first));
      MSG_INFO("Values : ");        
      for (auto  j : value){
	MSG_INFO(j);        
      }
      break;
    }
    case Property::Type::FLOATVECTOR :{
      std::vector<float> value = *(tool.getProperty< std::vector<float> > (i.first));
      MSG_INFO("Values : ");        
      for (auto  j : value){
	MSG_INFO(j);        
      }
      break;
    }
    case Property::Type::INTVECTOR :{
      std::vector<int> value = *(tool.getProperty< std::vector<int> > (i.first));
      MSG_INFO("Values : ");        
      for (auto  j : value){
	MSG_INFO(j);        
      }
      break;
    }
    default :
      if(i.first == "DefaultRandomRunNumber" ){
	MSG_INFO("Value : " << *(tool.getProperty<unsigned int>(i.first)));
      }
      if(i.first == "OutputLevel" ){
	MSG_INFO("Value : " << static_cast<int> (tool.msg().level()));
      }
      break;
    }
  }
}



int main( int argc, char* argv[] ) {
  xAOD::TReturnCode::enableFailure();

  //=========================================================
  //Parse input
  // The application's name:
  const char* APP_NAME = argv[ 0 ];
  //Set the message level
  MSG::Level mylevel=MSG::INFO;

  MSGHELPERS::getMsgStream().msg().setLevel(mylevel); 
  MSGHELPERS::getMsgStream().msg().setName(APP_NAME); 
  //  MSGHELPERS::getMsgStream().msg().setName(TElectronEffi);

  bool useCompactDisplay = true;
  
  std::string fileName="";
  std::string mapfileName="";
  std::string idkey="";
  std::string recokey="";
  std::string isokey="";
  std::string triggerkey="";
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
  p.addOption("mapfile","m");
  p.addOption("keyreco","k");
  p.addOption("keyid","d");
  p.addOption("keyiso","i");
  p.addOption("keytrigger","t");


  p.Init(argc,argv);

  /// first check for mapfile
  TString tmp = p.getArg("m");
  if (tmp != "")
    mapfileName = tmp;
  else { 
    tmp = p.getArg("f");
    if (tmp != "")
      fileName = tmp;
    else {
      MSG_INFO("Neither map file name nor file name given");
      printUsage(argv[0]);
      return 0;
    }
  }

  if (fileName!="" && fileName.find(".root") == std::string::npos ){
    MSG_INFO("filename not a root file? "<< fileName );
    return 0;
  }

  if (mapfileName!="" && mapfileName.find(".txt")  == std::string::npos){
    MSG_INFO("mapfilename not a txt file? "<< mapfileName  );
    return 0;
  }

  tmp = p.getArg("t");
  if (tmp != "")
    triggerkey = tmp;

  tmp = p.getArg("i");
  if (tmp != "")
    isokey = tmp;

  tmp = p.getArg("k");
  if (tmp != "")
    recokey = tmp;

  tmp = p.getArg("d");
  if (tmp != "")
    idkey = tmp;

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
  //=========================================================

  // Initialise the application:
  CHECK( xAOD::Init( APP_NAME ) );
  
  //==========================================================
  // CREATE DUMMY INPUT
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
  CHECK(store.record(ei, "EventInfo"));


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
  CHECK(store.record( m_clusters, "MyClusters" ));
  CHECK(store.record( m_clAux, "MyClustersAux."));

  
  links_clusters.push_back(ElementLink< xAOD::CaloClusterContainer >( cluster, *m_clusters ));
  el->setCaloClusterLinks(links_clusters);
  xAOD::TrackParticle* track = new xAOD::TrackParticle();
  track->makePrivateStore();
  track->setDefiningParameters(0., 0., 0.0, 2 * atan(exp(-eta)), 1.);
  m_tracks->push_back(track);
  std::vector< ElementLink< xAOD::TrackParticleContainer > > links_tracks;
  CHECK(store.record( m_tracks, "MyTrackParticles" ));
  CHECK(store.record( m_tracksAux, "MyTrackParticlesAux." ));

  el->setTrackParticleLinks(links_tracks);
  el->setEta(eta);
  el->setPhi(0.0);
  el->setM(0);
  el->setPt(e / cosh(eta));
  CHECK(store.record( m_electrons, "MyElectrons" ));
  CHECK(store.record( m_electronsAux, "MyElectronsAux." ));
  //==========================================================

  //Test some models 
  if (!useCompactDisplay){
    MSG_INFO("Adding File: "<<fileName);
  }
  std::vector<std::string> inputFiles{fileName} ;
  MSG_INFO( "SimType  " << SimType );  
  MSG_INFO("el pt " <<el->pt());
  
  //==================================================================================
  //Test the SIMPLIFIED
  AsgElectronEfficiencyCorrectionTool myEgCorrections ("myEgCorrections");  
  MSG_INFO("using  :" << fileName);
  if (fileName!="")  
    CHECK( myEgCorrections.setProperty("CorrectionFileNameList",inputFiles) );

  MSG_INFO("using mapfile :" << recokey);

  if (mapfileName!="") CHECK( myEgCorrections.setProperty("MapFilePath", mapfileName));

  // set the keys of interest for correction files
  if ( recokey!="")  CHECK( myEgCorrections.setProperty("RecoKey", recokey));
  MSG_INFO("using mapfile :" << recokey);
  if ( idkey!="")   CHECK( myEgCorrections.setProperty("IdKey", idkey));
  if ( isokey!="")   CHECK( myEgCorrections.setProperty("IsoKey", isokey));
  if ( triggerkey!="")   CHECK( myEgCorrections.setProperty("TriggerKey", triggerkey));
  CHECK( myEgCorrections.setProperty("ForceDataType",(int)SimType) );
 // CHECK( myEgCorrections.setProperty("CorrelationModel", "TOTAL" ));
CHECK( myEgCorrections.setProperty("CorrelationModel", "FULL" ));
   
 myEgCorrections.msg().setLevel(mylevel);
  CHECK( myEgCorrections.initialize() );  
  dumbProperties(myEgCorrections);
  double SF = 0; 
  std::vector<double> unc;
  // Get a list of systematics
  CP::SystematicSet recSysts = myEgCorrections.recommendedSystematics();
  
  if(myEgCorrections.getEfficiencyScaleFactor(*el,SF) != CP::CorrectionCode::Ok){
    MSG_ERROR( APP_NAME << "Problem in getEfficiencyScaleFactor");
  }
  MSG_INFO("SF  "<< SF );

  // Loop over systematics
  double total2_up =0 ; 
  double total2_down =0 ;
  for(const auto& sys : recSysts){
    double systematic = 0; 
    
    // Configure the tool for this systematic
    CHECK( myEgCorrections.applySystematicVariation({sys}) );
 
    if(myEgCorrections.getEfficiencyScaleFactor(*el,systematic) == CP::CorrectionCode::Ok){
      MSG_INFO( myEgCorrections.appliedSystematics().name().c_str()<< " Result " << systematic<< " Systematic value  "<<systematic-SF );  
      unc.push_back(systematic);
    
      float param =sys.parameter() ;    
      if(param>0) {
	total2_up += (SF-systematic)*(SF-systematic) ;  
      }
      else{
	total2_down+= (SF-systematic)*(SF-systematic) ;  
      }
    }else{
      MSG_ERROR( APP_NAME << "Problem in getEfficiencyScaleFactor");
    }
  }
  double total_up = sqrt(total2_up);
  double total_down = -sqrt(total2_down); 

  MSG_INFO( "total up " << total_up  <<" total down " << total_down  );
  MSG_INFO( "Index " << myEgCorrections.systUncorrVariationIndex(*el) );

  /*
  //==================================================================================
  //Test the TOYS
  //TOYS
  MSG::Level mylevelToy=MSG::INFO;//MSG::FATAL;
  AsgElectronEfficiencyCorrectionTool myEgCorrectionsToys ("myEgCorrectionsToys");
  if (fileName!="")   CHECK( myEgCorrectionsToys.setProperty("CorrectionFileNameList",inputFiles) );
  if (mapfileName!="") CHECK( myEgCorrectionsToys.setProperty("MapFilePath", mapfileName));

  // set the keys of interest for correction files
  if ( recokey!="")  CHECK( myEgCorrectionsToys.setProperty("RecoKey", recokey));
  MSG_INFO("using mapfile :" << recokey);
  if ( idkey!="")   CHECK( myEgCorrectionsToys.setProperty("IdKey", idkey));
  if ( isokey!="")   CHECK( myEgCorrectionsToys.setProperty("IsoKey", isokey));
  if ( triggerkey!="")   CHECK( myEgCorrectionsToys.setProperty("TriggerKey", triggerkey));

  CHECK( myEgCorrectionsToys.setProperty("ForceDataType",(int)SimType) );
  CHECK( myEgCorrectionsToys.setProperty("CorrelationModel", "MCTOYS" ));
  myEgCorrectionsToys.msg().setLevel(mylevelToy);
  CHECK( myEgCorrectionsToys.initialize() );
  dumbProperties(myEgCorrectionsToys);
  double SFToys = 0; 
  if(myEgCorrectionsToys.getEfficiencyScaleFactor(*el,SFToys) == CP::CorrectionCode::Ok){
  MSG_INFO("SF  Toys central "<< SF );
  }
  std::vector<double> uncToys;
  CP::SystematicSet recSystsToys = myEgCorrectionsToys.recommendedSystematics();
  //std::vector<CP::SystematicSet> sysListToys;
  CP::MakeSystematicsVector sysListToys;
  sysListToys.addGroup("toys");
  sysListToys.setToys(  myEgCorrectionsToys.getNumberOfToys( )   );
  sysListToys.calc(recSystsToys);
  std::vector<CP::SystematicSet> sysListToys2=sysListToys.result("toys");

  /// DO TOY LOOP
  for(const auto& sysToys : sysListToys2){
  double systematicToys = 0; 
  CHECK( myEgCorrectionsToys.applySystematicVariation(sysToys) );    
  if(myEgCorrectionsToys.getEfficiencyScaleFactor(*el,systematicToys) == CP::CorrectionCode::Ok){
  MSG_INFO("SFToys value "<< systematicToys );
  }
  uncToys.push_back(systematicToys);
  }

  */
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

