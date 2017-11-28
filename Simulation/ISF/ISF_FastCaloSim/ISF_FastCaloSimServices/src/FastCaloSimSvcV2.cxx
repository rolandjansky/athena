/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FastCaloSimSvcV2.cxx, (c) ATLAS Detector software             //
///////////////////////////////////////////////////////////////////

// class header include
#include "FastCaloSimSvcV2.h"


// FastCaloSim includes
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSPCAEnergyParametrization.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"

//!
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "IdDictParser/IdDictParser.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/TileID.h"
//!

// StoreGate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"


#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "TCanvas.h" // Just for testing
#include <fstream>

using std::abs;
using std::atan2;

/** Constructor **/
ISF::FastCaloSimSvcV2::FastCaloSimSvcV2(const std::string& name, ISvcLocator* svc) :
  BaseSimulationSvc(name, svc),
  m_rndGenSvc("AtRndmGenSvc", name)
{
  declareProperty("ParamsInputFilename"            ,       m_paramsFilename);
  declareProperty("CaloCellsOutputName"            ,       m_caloCellsOutputName) ;
  declareProperty("CaloCellMakerTools_setup"       ,       m_caloCellMakerToolsSetup) ;
  declareProperty("CaloCellMakerTools_release"     ,       m_caloCellMakerToolsRelease) ;
  declareProperty("DoRandomFluctuations"           ,       m_doRandomFluctuations) ;
  declareProperty("RandomSvc"                      ,       m_rndGenSvc                );
  declareProperty("RandomStream"                   ,       m_randomEngineName         );
  declareProperty("Extrapolator"                   ,       m_extrapolator );
  declareProperty("FastCaloSimCaloExtrapolation"   ,       m_FastCaloSimCaloExtrapolation );
  declareProperty("useFastCaloSimCaloExtrapolation",       m_useFastCaloSimCaloExtrapolation );
  declareProperty("useOneDShapeParametrisation"    ,       m_useOneDShapeParametrisation );
  declareProperty("nHits"                          ,       m_nHits );
}

ISF::FastCaloSimSvcV2::~FastCaloSimSvcV2()
{}

/** framework methods */
StatusCode ISF::FastCaloSimSvcV2::initialize()
{

  ATH_MSG_INFO(m_screenOutputPrefix << "Initializing ...");
  
  double wiggleLayer1[]={0.0110626,0.0242509,0.0398173,0.055761,0.0736173,0.0938847,0.115154,0.13639,0.157644,0.178934,0.200182,0.221473,0.242745,0.264019,0.285264,0.306527,0.327811,0.349119,0.370387,0.391668,0.412922,0.434208,0.45546,0.476732,0.498023,0.51931,0.540527,0.561799,0.583079,0.604358,0.625614,0.646864,0.668112,0.689351,0.710629,0.731894,0.75318,0.774426,0.795695,0.81699,0.838258,0.859528,0.880783,0.90202,0.922515,0.941276,0.958477,0.975062,0.988922,1};
  double wiggleLayer2[]={0.0127507,0.0255775,0.0395137,0.0542644,0.0695555,0.0858206,0.102274,0.119653,0.137832,0.156777,0.176938,0.197727,0.217576,0.236615,0.256605,0.277766,0.2995,0.321951,0.344663,0.367903,0.392401,0.417473,0.443514,0.470867,0.498296,0.52573,0.553114,0.57921,0.604326,0.628822,0.652191,0.674853,0.697268,0.718983,0.739951,0.759866,0.778877,0.798762,0.819559,0.839789,0.858923,0.877327,0.894831,0.911693,0.92821,0.94391,0.959156,0.973593,0.986752,1};
  double wiggleLayer3[]={0.0217932,0.0438502,0.0670992,0.091085,0.11651,0.143038,0.169524,0.196205,0.222944,0.249703,0.276629,0.303559,0.33034,0.356842,0.383579,0.410385,0.437272,0.464214,0.49118,0.518202,0.545454,0.572667,0.600037,0.627544,0.655072,0.6826,0.709824,0.733071,0.754764,0.775672,0.793834,0.810904,0.828219,0.844119,0.858339,0.871248,0.882485,0.894889,0.907955,0.920289,0.931136,0.941039,0.949844,0.957641,0.965787,0.97392,0.981706,0.988892,0.994527,1};
  
  for(int i=0;i<50;i++)
  {
   m_wiggleLayer1[i]=wiggleLayer1[i];
   m_wiggleLayer2[i]=wiggleLayer2[i];
   m_wiggleLayer3[i]=wiggleLayer3[i];
  }
  
  cellcheck=0;
  
  if(cellcheck)
  {
   detID=new AtlasDetectorID();
   IdDictParser* parser = new IdDictParser;
   IdDictMgr& idd = parser->parse ("IdDictParser/ATLAS_IDS.xml");
   detID->initialize_from_dictionary(idd);
   
   larID=new LArEM_ID();
   larID->initialize_from_dictionary(idd);
   
   tileID=new TileID();
   tileID->set_do_neighbours(false);
   tileID->initialize_from_dictionary(idd);
   
   hecID=new LArHEC_ID();
   hecID->initialize_from_dictionary(idd);
   
   fcalID=new LArFCAL_ID();
   fcalID->set_do_neighbours(false);
   fcalID->initialize_from_dictionary(idd);
   
  }
  
  m_paramsFile = TFile::Open(m_paramsFilename.c_str());
  ATH_MSG_INFO("------> file = " << m_paramsFile);
  m_paramsFile_photons = TFile::Open("/afs/cern.ch/atlas/groups/Simulation/FastCaloSimV2/secondPCA_photons_july17.root");
  ATH_MSG_INFO("------> photon file = " << m_paramsFile_photons);
  m_photonFile = TFile::Open("/afs/cern.ch/atlas/groups/Simulation/FastCaloSimV2/photons_test_norm1.root");
  ATH_MSG_INFO("------> file = " << m_photonFile);
  m_elFile = TFile::Open("/afs/cern.ch/atlas/groups/Simulation/FastCaloSimV2/InputDistribution_el_50.000000GeV_eta_0.200000_0.250000.root");
  ATH_MSG_INFO("------> file = " << m_elFile);
  m_pionFile = TFile::Open("/afs/cern.ch/atlas/groups/Simulation/FastCaloSimV2/InputDistribution_pion_50.000000GeV_eta_0.200000_0.250000.root");
  ATH_MSG_INFO("------> file = " << m_pionFile);
  
  TFCSPCAEnergyParametrization* epara_pions    =new TFCSPCAEnergyParametrization("pions","pions");
  TFCSPCAEnergyParametrization* epara_photons  =new TFCSPCAEnergyParametrization("photons","photons");
  TFCSPCAEnergyParametrization* epara_electrons=new TFCSPCAEnergyParametrization("electrons","electrons");
  
  m_eparas.push_back(epara_pions);
  m_eparas.push_back(epara_photons);
  m_eparas.push_back(epara_electrons);
  
  ATH_MSG_INFO("loading inputs pions");
  m_eparas[0]->loadInputs(m_paramsFile,"EnergyParams/pdgid_211/EN_50000/eta_0_20");
  ATH_MSG_INFO("loading inputs photons");
  m_eparas[1]->loadInputs(m_paramsFile_photons,"");
  ATH_MSG_INFO("loading inputs electrons");
  m_eparas[2]->loadInputs(m_paramsFile,"EnergyParams/pdgid_11/EN_65000/eta_0_20");
  ATH_MSG_INFO("loading electrons done!");
  
  /** commenting out close and delete: accessing  the file in ::simulate */
  m_paramsFile->Close();
  delete m_paramsFile;
  
  ATH_CHECK(m_rndGenSvc.retrieve());
  m_randomEngine = m_rndGenSvc->GetEngine( m_randomEngineName);
  if(!m_randomEngine)
  {
   ATH_MSG_ERROR("Could not get random number engine from RandomNumberService. Abort.");
   return StatusCode::FAILURE;
  }
  
  const CaloDetDescrManager* calo_dd_man  = CaloDetDescrManager::instance();
  m_caloGeo = new CaloGeometryFromCaloDDM();
  m_caloGeo->LoadGeometryFromCaloDDM(calo_dd_man);
  TString path_to_fcal_geo_files = "/afs/cern.ch/atlas/groups/Simulation/FastCaloSimV2/";
  m_caloGeo->LoadFCalGeometryFromFiles(path_to_fcal_geo_files + "FCal1-electrodes.sorted.HV.09Nov2007.dat", path_to_fcal_geo_files + "FCal2-electrodes.sorted.HV.April2011.dat", path_to_fcal_geo_files + "FCal3-electrodes.sorted.HV.09Nov2007.dat");
   
  //m_caloGeo->PrintMapInfo(0,0); 
   
   
  for(unsigned int i=0;i<24;i++)
   m_rlayer.push_back(999);
  
  m_rlayer[0]=1455.98;
  m_rlayer[1]=1542.83;
  m_rlayer[2]=1757.11;
  m_rlayer[3]=1943.15;
  m_rlayer[4]=999;
  m_rlayer[5]=999;
  m_rlayer[6]=999;
  m_rlayer[7]=999;
  m_rlayer[8]=999;
  m_rlayer[9]=999;
  m_rlayer[10]=999;
  m_rlayer[11]=999;
  m_rlayer[12]=2450.0;
  m_rlayer[13]=3020.0;
  m_rlayer[14]=3630.0;
  m_rlayer[15]=999;
  m_rlayer[16]=999;
  m_rlayer[17]=999;
  m_rlayer[18]=999;
  m_rlayer[19]=999;
  m_rlayer[20]=999;
  m_rlayer[21]=999;
  m_rlayer[22]=999;
  m_rlayer[23]=999;

  // Get TimedExtrapolator 
  if(!m_extrapolator.empty() && m_extrapolator.retrieve().isFailure())
   return StatusCode::FAILURE;


  // Get FastCaloSimCaloExtrapolation
  if(m_FastCaloSimCaloExtrapolation.retrieve().isFailure())
  {
   ATH_MSG_ERROR("FastCaloSimCaloExtrapolation not found ");
   return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode ISF::FastCaloSimSvcV2::finalize()
{
  ATH_MSG_INFO(m_screenOutputPrefix << "Finalizing ...");
  return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimSvcV2::setupEvent()
{
  ATH_MSG_INFO(m_screenOutputPrefix << "setupEvent NEW EVENT!");
  
  m_theContainer = new CaloCellContainer(SG::VIEW_ELEMENTS);

  StatusCode sc = evtStore()->record(m_theContainer, m_caloCellsOutputName);
  if (sc.isFailure())
  {
    ATH_MSG_FATAL( m_screenOutputPrefix << "cannot record CaloCellContainer " << m_caloCellsOutputName );
    return StatusCode::FAILURE;
  }

  CHECK( m_caloCellMakerToolsSetup.retrieve() );
  ATH_MSG_DEBUG( "Successfully retrieve CaloCellMakerTools: " << m_caloCellMakerToolsSetup );
  ToolHandleArray<ICaloCellMakerTool>::iterator itrTool = m_caloCellMakerToolsSetup.begin();
  ToolHandleArray<ICaloCellMakerTool>::iterator endTool = m_caloCellMakerToolsSetup.end();
  for (; itrTool != endTool; ++itrTool)
  {
    StatusCode sc = (*itrTool)->process(m_theContainer);
    if (sc.isFailure())
    {
      ATH_MSG_ERROR( m_screenOutputPrefix << "Error executing tool " << itrTool->name() );
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimSvcV2::releaseEvent()
{
 
 ATH_MSG_INFO(m_screenOutputPrefix << "release Event");
 
 CHECK( m_caloCellMakerToolsRelease.retrieve() );
 
 //run release tools in a loop
 ToolHandleArray<ICaloCellMakerTool>::iterator itrTool = m_caloCellMakerToolsRelease.begin();
 ToolHandleArray<ICaloCellMakerTool>::iterator endTool = m_caloCellMakerToolsRelease.end();
 for (; itrTool != endTool; ++itrTool)
 {
  ATH_MSG_INFO( m_screenOutputPrefix << "Calling tool " << itrTool->name() );
  
  StatusCode sc = (*itrTool)->process(m_theContainer);
  
  if (sc.isFailure())
  {
   ATH_MSG_ERROR( m_screenOutputPrefix << "Error executing tool " << itrTool->name() );
  }
 }
 
 return StatusCode::SUCCESS;
 
}

/** Simulation Call */
StatusCode ISF::FastCaloSimSvcV2::simulate(const ISF::ISFParticle& isfp)
{
 
 ATH_MSG_INFO("NEW PARTICLE! FastCaloSimSvcV2 called with ISFParticle: " << isfp);
 
 int pdgid = fabs(isfp.pdgCode());
 int barcode=isfp.barcode();
 
 if(barcode!=10001)
 {
  ATH_MSG_INFO("ISF particle barcode is barcode "<<barcode<<". Go to next Particle.");
  return StatusCode::SUCCESS; 
 }
 
 particle_position =  isfp.position();
 eta = 0;
 phi = 0;
 eta_isfp = 0;
 phi_isfp = 0;
 
 eta_isfp = particle_position.eta();             // isfp eta and phi, in case we need them
 phi_isfp = particle_position.phi();  
 if(abs(eta_isfp) > 0.3 || abs(eta_isfp) < 0.15) //somewhat enlarged to not cut off too many particles
 {
  ATH_MSG_INFO("ISF particle is out of eta range: "<<eta_isfp<<". Go to next Particle.");
  return StatusCode::SUCCESS; 
 }
 
 if(!(pdgid==22 || pdgid==211 || pdgid==11))
 {
  ATH_MSG_INFO("Oh no! ISF particle has pdgid "<<pdgid<<" . That's not supported yet :(");
  return StatusCode::SUCCESS; 
 } 
 
 //Get extrapolation result
 
 //commenting out next line, because it fails for some events (where the truth is not saved, this apparanetly can happen)
 //HepMC::GenParticle* isfpTruth = isfp.getTruthBinding()->getTruthParticle();
 //will use the ISFP momentum instead:
 TFCSTruthState truth(isfp.momentum().x(),isfp.momentum().y(),isfp.momentum().z(),sqrt(pow(isfp.ekin(),2)+pow(isfp.mass(),2)),isfp.pdgCode());
 TFCSExtrapolationState result;
 m_FastCaloSimCaloExtrapolation->extrapolate(result,&truth);
 
 int index_epara=0;
 if(pdgid==22) index_epara=1;
 if(pdgid==11) index_epara=2;
 
 /** get the number of pca bins from the epara: **/
 int n_pcabins = m_eparas[index_epara]->n_pcabins();
 
 //determine the pca bin randomly:
 int pcabin = 1;
 float uniform=CLHEP::RandFlat::shoot(m_randomEngine);
 
 for (int n = 0; n < n_pcabins; n++)
 {
  if(uniform > n * 1.0 / (double)n_pcabins && uniform < (n + 1.) * 1.0 / (double)n_pcabins)
  pcabin = n + 1;
 }
 ATH_MSG_INFO("pca bin "<<pcabin);
 
 //-----ENERGY:-----
 simulstate.set_Ebin(pcabin);
 m_eparas[index_epara]->simulate(simulstate, nullptr, nullptr);
 
 ATH_MSG_INFO("Energy returned: " << simulstate.E());
 ATH_MSG_INFO("Energy fraction for layer: ");
 for (int s = 0; s < CaloCell_ID_FCS::MaxSample; s++)
 ATH_MSG_INFO(" Sampling " << s << " energy " << simulstate.E(s));
 
 //-----SHAPE:-----
 
 if(m_doRandomFluctuations == true && pdgid == 211)
 {
  ATH_MSG_INFO("\n\n*******************************************\n"
     << "RANDOM FLUCTUATIONS ARE ON"
     << "\n*******************************************\n\n");
 }
 else
 {
  ATH_MSG_INFO("\n\n*******************************************\n"
        << "RANDOM FLUCTUATIONS ARE OFF"
        << "\n*******************************************\n\n");
 }
 
 if (m_useOneDShapeParametrisation == true )
 {
  ATH_MSG_INFO("\n\n*******************************************\n"
        << "USING 1D SHAPE PARAMETRISATION\n"
        << "NHITS=" << m_nHits
        << "\n*******************************************\n\n");
 }
 else
 {
  ATH_MSG_INFO("\n\n*******************************************\n"
        << "USING 2D SHAPE PARAMETRISATION"
        << "\n*******************************************\n\n");
 }
 
 /** get the relevant calo layers */
 m_layers = m_eparas[index_epara]->get_layers();
 
 /** get the appropriate input histogram */
 std::string inputHistoName = "";
 
 esum=0.0;
 nCells=0;
 nCells_Tile=0;
 
 for ( ilayer = 0;  ilayer < m_layers->GetSize(); ilayer++)
 {  
  /** get the layer ID and associated energy */
  layer =  m_layers->GetAt(ilayer);
  layerE = simulstate.E(layer);
  
  // get eta and phi from extrapolator or isfp
  if(m_useFastCaloSimCaloExtrapolation)
  {
   ATH_MSG_INFO("Using FastCaloSimCaloExtrapolation result for eta, phi, r and z!");
   eta = result.eta(layer, 0);
   phi = result.phi(layer, 0);
   r_layer = result.r(layer,0);
   z_particle = result.z(layer,0);
  }
  if(!m_useFastCaloSimCaloExtrapolation)
  {
   ATH_MSG_INFO("Using isfp for eta, phi, r and z!");
   eta = eta_isfp;
   phi = phi_isfp;
   r_layer = m_rlayer[layer];
   z_particle = particle_position.z();
  }
  
  ATH_MSG_INFO("NOW RUNNING ON LAYER "<<layer);
  
  inputHistoName = "hEnergy_layer"+std::to_string(layer)+"_pca"+std::to_string(pcabin);
  
  if(pdgid==11)  //el
   m_histEnergyDensity2D = (TH2F*)m_elFile->Get(inputHistoName.c_str());
  if(pdgid==211) //pion
   m_histEnergyDensity2D = (TH2F*)m_pionFile->Get(inputHistoName.c_str());
  if(pdgid==22)  //photons
   m_histEnergyDensity2D = (TH2F*)m_photonFile->Get(inputHistoName.c_str());
  
  if(!m_photonFile)
  {
   ATH_MSG_FATAL("No photonFile!");
   return StatusCode::FAILURE;
  }
  if(!m_elFile)
  {
   ATH_MSG_FATAL("No elFile!");
   return StatusCode::FAILURE;
  }
  if(!m_pionFile)
  {
   ATH_MSG_FATAL("No pionFile!");
   return StatusCode::FAILURE;
  }
  if(!m_paramsFile)
  {
   ATH_MSG_FATAL("No paramFile!");
   return StatusCode::FAILURE;
  }
  if(!m_histEnergyDensity2D)
  { 
   ATH_MSG_FATAL("Problem. No histogram!! :-(");
   return StatusCode::FAILURE;
  }
  
  if(m_useOneDShapeParametrisation)
  {
   if (m_doRandomFluctuations == true && pdgid == 211) m_nHits = 5; //to be optimised
  }      
  if(!m_useOneDShapeParametrisation)
  {
   /** set number of hits due to random fluctuations if particle is a pion and flag is set */
   if (m_doRandomFluctuations == true && pdgid == 211) m_nHits = 10;
   /** otherwise get the number of hits to account for the resolution of the layer */
   else m_nHits = hitCalc(layerE, layer, pcabin, pdgid);
  }
  
  ATH_MSG_INFO("number of HITS = "<<m_nHits);
  
  R=sqrt(r_layer*r_layer + z_particle*z_particle);
  findEtaMMRange(R,eta,deltaEtaMMmin,deltaEtaMMmax);
  deltaEtaMMmin*=0.999;
  deltaEtaMMmax*=0.999;
  
  /*
  std::cout<<"Range for Delta eta[mm]: " << "[" << deltaEtaMMmin << "," << deltaEtaMMmax << "]" << std::endl;
  std::cout << "Particle position: R: " << R << " eta: " << eta << std::endl;
  std::cout<<"layer "<<layer<<" number of HITS = "<<m_nHits<<std::endl;
  */
  
  if(m_useOneDShapeParametrisation)
  {
   TH1D* proj = m_histEnergyDensity2D->ProjectionY("proj",0,-1,"e");
   
   for (int i = 1; i <= proj->GetNbinsX(); ++i)
   {      
    double mean, err, energyDensityInBin, energyInBin;      
    mean = proj->GetBinContent(i);
    err = proj->GetBinError(i);
    //std::cout << "Bin: " << i << " mean: " << mean << "  err: " << err << std::endl;
    
    energyDensityInBin = CLHEP::RandGauss::shoot(m_randomEngine,mean,err);

    energyInBin = energyDensityInBin * layerE;
    
    double minR = proj->GetBinLowEdge(i);
    double maxR = proj->GetBinLowEdge(i) + proj->GetBinWidth(i);
    
    //std::cout << "Energy in layer: " << layerE << " energyDensityInBin: " << energyDensityInBin << "  energyInBin: " << energyInBin << std::endl;
    LoopOverHits(energyInBin, minR, maxR);
    if(cellcheck) TestCell();
   }
  }
  if(!m_useOneDShapeParametrisation)
  {
   LoopOverHits(layerE);
   if(cellcheck) TestCell();
  }
    
 } //loop layer
 
 if(cellcheck) std::cout<<"ECHECK etot "<<esum<<" epara "<<simulstate.E()<<" nCells "<<nCells<<" nCells_Tile "<<nCells_Tile<<std::endl;
 
 delete m_histEnergyDensity2D;
 return StatusCode::SUCCESS;
	
}

void ISF::FastCaloSimSvcV2::LoopOverHits(double totalEnergy, double minR, double maxR)
{
 
 for (int hit = 0; hit < m_nHits; hit++)
 {
  double alpha, r;
  double delta_eta_mm;
  
  const CaloDetDescrElement* mcell = 0;
  do
  {
   if(m_useOneDShapeParametrisation)
   {
    r = CLHEP::RandFlat::shoot(m_randomEngine,minR,maxR);
    alpha = CLHEP::RandFlat::shoot(m_randomEngine,2*TMath::Pi());
   }
   if(!m_useOneDShapeParametrisation)
   {
    this->getRandom2(m_histEnergyDensity2D,alpha,r);
    //m_histEnergyDensity2D->GetRandom2(alpha, r);
   }
   delta_eta_mm=r * cos(alpha);
  }
  while(delta_eta_mm<deltaEtaMMmin || delta_eta_mm>deltaEtaMMmax);
  
  double delta_phi_mm = r * sin(alpha);
  //std::cout<<"got a hit positon from the histogram!"<<" r "<<r<<" alpha "<<alpha<<" r_layer "<<r_layer<<" z_particle "<<z_particle<<" eta "<<eta<<std::endl;
  //double r_layer=m_rlayers[ilayer*n_pcabins+pcabin-1];
  
  double hit_eta=findHitEta(delta_eta_mm,R,eta);
  //std::cout<<"hit_eta "<<hit_eta<<std::endl;
  
  double delta_phi=delta_phi_mm/r_layer;
  double hit_phi = phi+delta_phi;
  double hit_phi_shifted=hit_phi;     
  
  double wiggle = 0.0;
  if(layer < 21)
  {
   if(layer < 4 && layer > 0)
   {
    wiggle = doWiggle(layer);
    //std::cout << "wiggle is " << wiggle << std::endl;
   }
   hit_phi_shifted=hit_phi-wiggle;
   hit_phi_shifted=TVector2::Phi_mpi_pi(hit_phi_shifted);
   mcell = m_caloGeo->getDDE(layer, hit_eta, hit_phi_shifted);
  }
  if(layer >= 21) //FCAL
  {
   double theta=2.*atan(exp(-hit_eta));
   double rT=z_particle*tan(theta);
   double hit_x=rT*cos(hit_phi);
   double hit_y=rT*sin(hit_phi);
   mcell = m_caloGeo->getFCalDDE(layer, hit_x, hit_y, z_particle);      
  }
  
  if (!mcell) continue;
  
  CaloCell* theCell = (CaloCell*)m_theContainer->findCell(mcell->calo_hash());
  theCell->addEnergy(totalEnergy / m_nHits);
  
  /*
  if(hit<10)
  {
   ATH_MSG_INFO("Hit nr "<<i<<" eta: " << hit_eta << " phi: " << hit_phi << " Particle eta: " << eta << " phi: " << phi << " delta_eta: " << hit_eta - eta << " delta_phi: " << hit_phi - phi);
   ATH_MSG_INFO(" Cell from CaloGeometry: eta: " << mcell->eta() << " phi: " << mcell->phi() << " |CELL_eta - HIT_eta| " << abs(mcell->eta() - hit_eta)  << " |CELL_phi - HIT_phi| " << abs(mcell->phi() - hit_phi));
   ATH_MSG_INFO(" energy density in layer: "<<energyDensityInBin);
   ATH_MSG_INFO(" energy input into cell: "<<energyInBin / m_nHits);
  }
  */
  
  if(cellcheck){
   const CaloDetDescrElement* DDE = theCell->caloDDE();

   // Comparison CaloDetDescrElement vs CaloGeometry element
   if(abs(DDE->eta() - mcell->eta())>0.0001 || abs(DDE->phi() - mcell->phi())>0.0001 ){
     ATH_MSG_ERROR("Error in CaloGeometry: Cell with same ID has different position.");
     ATH_MSG_ERROR("CaloCell: " << "eta: " << DDE->eta() << " phi: " << DDE->phi() << " ID: " << DDE->identify());
     ATH_MSG_ERROR("CaloGeometry: " << "eta: " << mcell->eta() << " phi: " << mcell->phi() << " ID: " << mcell->identify());
     
   }

   // Hit-to-cell assignment check
   
   //double delta_phi_check=abs(std::fmod(DDE->phi() - hit_phi_shifted,TMath::Pi()*2.));
   double delta_phi_check=abs(TVector2::Phi_mpi_pi(DDE->phi() - hit_phi_shifted));
   //if(delta_phi_check>TMath::Pi())delta_phi_check=TMath::Pi()*2.-delta_phi_check;
   if(abs(DDE->eta() - hit_eta) > DDE->deta()/2*1.01 || abs(delta_phi_check) > DDE->dphi()/2*1.01) {
     ATH_MSG_ERROR("Error in hit-to-cell assignment: Hit is too far from the cell.");
     ATH_MSG_ERROR(" Cell from CaloGeometry: eta: " << DDE->eta() << " deta: " << DDE->deta() << " phi: " << DDE->phi() << " dphi: " << DDE->dphi() << " |CELL_eta - HIT_eta|/(0.5*deta) " << abs(DDE->eta() - hit_eta)*2./ DDE->deta() 
	  << " HIT_eta " << hit_eta << " |CELL_phi - HIT_phi|/(0.5*dphi) " << abs(delta_phi_check)*2./DDE->dphi() << " HIT_phi: " << hit_phi_shifted );		  
   }


   if(hit<10)
   {
    ATH_MSG_INFO("Hit nr "<<hit<<" eta: " << hit_eta << " phi: " << hit_phi << " Particle eta: " << eta << " phi: " << phi << " delta_eta: " << hit_eta - eta << " delta_phi: " << hit_phi - phi);
    ATH_MSG_INFO(" Cell from CaloGeometry: eta: " << mcell->eta() << " phi: " << mcell->phi() << " |CELL_eta - HIT_eta|/(0.5*CELL_deta) " << abs(mcell->eta() - hit_eta)*2/mcell->deta()  << " |CELL_phi - HIT_phi|/(0.5*CELL_dphi) " << abs(mcell->phi() - hit_phi_shifted)*2./mcell->dphi());
    ATH_MSG_INFO(" energy input into cell: "<<layerE / m_nHits);
   }
}  //if cellcheck
  
 } //for hit
 
} //LoopOverHits

void ISF::FastCaloSimSvcV2::TestCell()
{
 esum=0.0;
 nCells=0;
 nCells_Tile=0;	 
 
 double cut=0.1;
 
 CaloCellContainer::iterator it  =m_theContainer->begin();
 CaloCellContainer::iterator it_e=m_theContainer->end();
 
 for(;it!=it_e;++it)
 {
  CaloCell* theCell=(*it);
  //const unsigned int hash_id=theCell->caloDDE()->calo_hash();
  const Identifier cell_id=theCell->ID();
  esum+=theCell->energy();
  if(theCell->energy()>cut)
  {
   int isLAr=0; int isTile=0; int isHEC=0; int isFCAL=0;
   
   if(detID->is_lar_em(cell_id)==1)   isLAr=1;
   if(detID->is_lar_hec(cell_id)==1)  isHEC=1;
   if(detID->is_lar_fcal(cell_id)==1) isFCAL=1;
   if(detID->is_tile(cell_id)==1)     isTile=1;
   
   std::cout<<"cell_id "<<cell_id<<" energy "<<theCell->energy()<<" eta "<<theCell->eta()<<" phi "<<phi;
   if(isLAr)  { std::cout<<"  LArEM sampling "<<larID->sampling(cell_id); }
   if(isHEC)  { std::cout<<"  HEC sampling   "<<hecID->sampling(cell_id); }
   if(isFCAL) { std::cout<<"  FCAL module    "<<fcalID->module(cell_id); }
   if(isTile) { std::cout<<"  TILE sampling  "<<tileID->sampling(cell_id); nCells_Tile++; }
   
   std::cout<<" "<<std::endl;
   nCells++;
  }
 }
 
 double sum_epara=0.0;
 for(int a=0;a<=ilayer;a++) sum_epara+=simulstate.E(m_layers->GetAt(a));
 
 std::cout<<"ECHECK layer "<<layer<<" esum "<<esum<<" epara "<<sum_epara<<" (this layer: "<<simulstate.E(layer)<<") nCells "<<nCells<<" nCells_Tile "<<nCells_Tile<<std::endl;
}

//-----HITS:-----

int ISF::FastCaloSimSvcV2::hitCalc(double energy, int calolayer,int pcabin, int pdgid)
{
 
 int hits=1;

 std::vector<std::vector<int> > hitVectorPions(24);
 //for(int i=0;i<24;i++)hitVector[i].resize(5);
 hitVectorPions[0]={105,62,30,9,4};
 hitVectorPions[1]={569,283,117,31,7};
 hitVectorPions[2]={2320,1859,1189,380,35};
 hitVectorPions[3]={103,128,136,50,3};
 hitVectorPions[12]={18,28,37,49,36};
 hitVectorPions[13]={6,14,26,44,70};
 hitVectorPions[14]={0,0,4,0,8};
 
 std::vector<std::vector<int> > hitVectorPhotons(24);
 hitVectorPhotons[0]={8,10,5,5,8};
 hitVectorPhotons[1]={5,5,5,5,8};
 hitVectorPhotons[2]={25,55,60,150,150};
 hitVectorPhotons[3]={50,50,50,50,50};
 hitVectorPhotons[12]={30,40,50,25,30};
 hitVectorPhotons[13]={0,0,0,0,0};
 hitVectorPhotons[14]={0,0,0,0,0};
 /** Calculate estimates for cell energy fluctuations. Should be updated with better numbers */
 
 if(energy>0.0001)
 {
  
  double stochastic=0.1;
  double constant=0.0;
  
  if(pdgid == 211)
  {
   return hitVectorPions[calolayer][pcabin-1];
   
   /*
   if(calolayer >= 0 && calolayer<= 7)
   {
    stochastic=0.101;     //LAr      10.1%/sqrt(E)
    constant=0.002;
   }
   if(calolayer >= 8 && calolayer <= 11)
   {
    stochastic=0.706;     //HadEC  70.6%/sqrt(E) for pions
    constant=0.058;
   }
   if(calolayer >= 12 && calolayer <= 20)
   {
    stochastic=0.564;     //TileCal 56.4%/sqrt(E)
    constant=0.055;
   }
   if(calolayer >= 21 && calolayer <= 23)
   {
    stochastic=0.942;     //FCAL    94.2%/sqrt(E) for pions
    constant=0.075;
   }
   */
  }

  if(pdgid != 211)
  {
   if(calolayer >= 0 && calolayer<= 7)
   {
    stochastic=0.101;     //LAr      10.1%/sqrt(E)
    constant=0.002;
   }
   
   if(calolayer >= 8 && calolayer <= 11)
   {
    stochastic=0.214;     //HadEC 21.4%/sqrt(E)
    constant=0.0;
   }
   
   if(calolayer >= 12 && calolayer <= 20)
   {
    stochastic=0.564;     //TileCal 56.4%/sqrt(E)
    constant=0.055;
   }
   
   if(calolayer >= 21 && calolayer <= 23)
   {
    stochastic=0.285;     //FCAL    28.5%/sqrt(E)
    constant=0.035;
   }
  
  } //if pdgid!=211
   
  hits = 1.0 / ((stochastic/sqrt(energy/1000.0))*(stochastic/sqrt(energy/1000.0)) + constant*constant);

 } //if pass energy
 
 else
  ATH_MSG_INFO("Something is wrong. Layer "<<calolayer<<" has very little energy "<<energy);
  
 return hits;

}

double ISF::FastCaloSimSvcV2::findHitEta(const double delta_eta_mm,const double R,const double eta_particle)
{
  double x=eta_particle; // approximation of hit_eta, starting point for iterations
  
  //std::cout<<"in findHitEta: delta_eta_mm "<<delta_eta_mm<<" R "<<R<<" x "<<x<<std::endl;
  
  double a,b;
  double c,d;
  double delta;
  double sech,tanh;
  
  int count=0;
  const double epsilon=0.001;
  do
  {
    c=exp(x);
    d=1./c;
    sech=2./(c+d);
    tanh=(c-d)/2.*sech;
    delta=x-eta_particle;
    a = transformationEtaToMM(R,delta,sech) - delta_eta_mm;// transformation
    b = transformationEtaToMMFirstDerivative(R,delta,sech,tanh);// first derivative of the transformation
    x = x - a/b;
    count++;
    
    //std::cout<<"count "<<count<<" a "<<a<<" b "<<b<<" c "<<c<<" d "<<d<<" delta "<<delta<<" x "<<x<<std::endl;
  }
  while((abs(a) > epsilon) && count<1000);
  if(x!=x || count>=1000) ATH_MSG_ERROR("Error: Hit eta not found.");
  //std::cout << "hit_eta: " << x << " error: " << a/b << std::endl;
  return x;
}

double ISF::FastCaloSimSvcV2::findStartingPoint(const double R,const double eta_particle,bool forMaximum)
{
  double x=eta_particle;
  double delta,sech;
  double c,d;
  const double sign= forMaximum ? 1. : -1.;
  
  double y=0,max=0;
  
  const double epsilon=sign*0.2;
  //std::cout << "Serching for starting point" << std::endl;
  //std::cout << "Epsilon" << epsilon << std::endl;
  int count=0;
  for(;count<1000;count++)
  {
   x+=epsilon;
   delta=x-eta_particle;
   c=exp(x);
   d=1./c;
   sech=2./(c+d);
   y=transformationEtaToMM(R,delta,sech);
   //std::cout << "y " << y << " max " << max << " sign*(y - max) " << sign*(y - max) << std::endl;
   if(sign*(y - max)>0)max=y;
   else break;
  }
  x-=epsilon;
  
  return x;
}

void ISF::FastCaloSimSvcV2::findEtaMMRange(const double R,const double eta_particle,double & etaMin,double & etaMax)
{
  
  // const double epsilon1=0.1;
  double delta;
  double x=eta_particle;
  // double secondDerivative(0);
  double c,d,sech,tanh;
  
  double startingPointForMaximum=findStartingPoint(R,eta_particle,true);
  double startingPointForMinimum=findStartingPoint(R,eta_particle,false);
  
  
  c=exp(startingPointForMaximum);
  d=1./c;
  sech=2./(c+d);
  tanh=(c-d)/2.*sech;
  delta=startingPointForMaximum-eta_particle;
  //std::cout<< "Starting point for maximum: " << startingPointForMaximum << std::endl;
  //std::cout<< "Function evaluated in starting point for maximum: " << transformationEtaToMM(R,delta,sech) << std::endl;
  //std::cout<< "First derivative in starting point for maximum: " << transformationEtaToMMFirstDerivative(R,delta,sech,tanh) << std::endl;
  //std::cout<< "Second derivative in starting point for maximum: " << transformationEtaToMMSecondDerivative(R,delta,sech,tanh) << std::endl;
  
  
  c=exp(startingPointForMinimum);
  d=1./c;
  sech=2./(c+d);
  tanh=(c-d)/2.*sech;
  delta=startingPointForMinimum-eta_particle;
  //std::cout<< "Starting point for minimum: " << startingPointForMinimum << std::endl;
  //std::cout<< "Function evaluated in starting point for minimum: " << transformationEtaToMM(R,delta,sech) << std::endl;
  //std::cout<< "First derivative in starting point for minimum: " << transformationEtaToMMFirstDerivative(R,delta,sech,tanh) << std::endl;
  //std::cout<< "Second derivative in starting point for minimum: " << transformationEtaToMMSecondDerivative(R,delta,sech,tanh) << std::endl;
  
  x=startingPointForMaximum;
  const double epsilon=0.001;
  int count =0;
  double a,b;
  //double e;
  double gamma=1;
  
  //std::cout << "Searching for maximum" << std::endl;
  do
  {
    c=exp(x);
    d=1./c;
    sech=2./(c+d);
    tanh=(c-d)/2.*sech;
    delta=x-eta_particle;
    a = transformationEtaToMMFirstDerivative(R,delta,sech,tanh);// transformation
    b = transformationEtaToMMSecondDerivative(R,delta,sech,tanh);// first derivative of the transformation
    //e = transformationEtaToMM(R,delta,sech);
    x = x - gamma*a/b;
    count++;
    
    //std::cout<<"count "<<count<<" a "<<a<<" b "<<b<< " value " << e <<" c "<<c<<" d "<<d<<" delta "<<delta<<" x "<<x<<std::endl;
  } while((abs(a) > epsilon) && count<1000);
  if(x!=x || count>=1000) ATH_MSG_ERROR("Error: Maximum for delta eta[mm] range not found.");
  
  delta=x-eta_particle;
  c=exp(x);
  d=1./c;
  sech=2./(c+d);
  etaMax=transformationEtaToMM(R,delta,sech);
  //std::cout << " Position of maximum " << x << " Eta particle " << eta_particle << " Delta Eta max[mm] " << etaMax << std::endl;

  
  x=startingPointForMinimum;
  count =0;
  //std::cout << "Searching for minimum" << std::endl;
  do
  {
    c=exp(x);
    d=1./c;
    sech=2./(c+d);
    tanh=(c-d)/2.*sech;
    delta=x-eta_particle;
    a = transformationEtaToMMFirstDerivative(R,delta,sech,tanh);// transformation
    b = transformationEtaToMMSecondDerivative(R,delta,sech,tanh);// first derivative of the transformation
    //e = transformationEtaToMM(R,delta,sech);
    x = x - gamma*a/b;
    count++;
    
    //std::cout<<"count "<<count<<" a "<<a<<" b "<<b<< " value " << e <<" c "<<c<<" d "<<d<<" delta "<<delta<<" x "<<x<<std::endl;
  } while((abs(a) > epsilon) && count<1000);
  if(x!=x || count>=1000) ATH_MSG_ERROR("Error: Minimum for delta eta[mm] range not found.");
  delta=x-eta_particle;
  c=exp(x);
  d=1./c;
  sech=2./(c+d);
  etaMin=transformationEtaToMM(R,delta,sech);
  //std::cout << " Position of minimum " << x << " Eta particle " << eta_particle << " Delta Eta min[mm] " << etaMin << std::endl;
  
}

double ISF::FastCaloSimSvcV2::doWiggle(int layer)
{
 
 double wiggle = 0.0;
 double cell_dphi = 0.0;
 
 //Define cell size in phi  
 if(layer == 1)
  cell_dphi = 0.0981748;
 if(layer == 2 || layer == 3)
  cell_dphi = 0.0245437;
 if(layer==0 || layer>3)
 {
  ATH_MSG_ERROR("I am inside the wiggle calculation, but the layer is " << layer << "!");
  return 0.0;
 }
 
 //Set random numbers
 double searchRand = CLHEP::RandFlat::shoot(m_randomEngine);
  
 //Now for layer dependant approach
 if(layer == 1)
 {
  int chosenBin = (Int_t) TMath::BinarySearch(50, m_wiggleLayer1, searchRand);
  double x_wigg = ((-0.98)+(chosenBin+1)*0.04)/2;
  wiggle = x_wigg*cell_dphi/4; 
 }
 
 if(layer == 2)
 {
  int chosenBin = (Int_t) TMath::BinarySearch(50, m_wiggleLayer2, searchRand);
  double x_wigg = ((-0.98)+(chosenBin+1)*0.04)/2;
  wiggle = x_wigg*cell_dphi;
 }
 
 if(layer == 3)
 {
  int chosenBin = (Int_t) TMath::BinarySearch(50, m_wiggleLayer3, searchRand);
  double x_wigg = ((-0.98)+(chosenBin+1)*0.04)/2;
  wiggle = x_wigg*cell_dphi;
 }
 
 return wiggle;
  
}

void ISF::FastCaloSimSvcV2::getRandom2(TH2* h,double &x,double &y){
  //ATH_MSG_INFO("Running ISF::FastCaloSimSvcV2::getRandom2"); 
  TAxis* fXaxis = h->GetXaxis();
  TAxis* fYaxis = h->GetYaxis();
  const int nbinsx = h->GetNbinsX();
  const int nbinsy = h->GetNbinsY();
  const int nbins  = nbinsx*nbinsy;
  double integral;
  double *fIntegral = h->GetIntegral();
  // compute integral checking that all bins have positive content (see ROOT-5894)
  
  if (fIntegral[nbins+1] != h->GetEntries()) integral = h->ComputeIntegral(true);
  else integral = fIntegral[nbins];
 
  if (integral == 0 ) { x = 0; y = 0; return;}
  // case histogram has negative bins
  if (integral == TMath::QuietNaN() ) { x = TMath::QuietNaN(); y = TMath::QuietNaN(); return;}
  
  double r1 = CLHEP::RandFlat::shoot(m_randomEngine);
  int ibin = TMath::BinarySearch(nbins,fIntegral,(double) r1);
  int biny = ibin/nbinsx;
  int binx = ibin - nbinsx*biny;
  x = fXaxis->GetBinLowEdge(binx+1);
  if (r1 > fIntegral[ibin]) x +=
    fXaxis->GetBinWidth(binx+1)*(r1-fIntegral[ibin])/(fIntegral[ibin+1] - fIntegral[ibin]);
  y = fYaxis->GetBinLowEdge(biny+1) + fYaxis->GetBinWidth(biny+1)*CLHEP::RandFlat::shoot(m_randomEngine);
}

