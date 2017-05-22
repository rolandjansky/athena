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


// StoreGate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"


#include "CLHEP/Random/RandFlat.h"

#include "CaloEvent/CaloCellContainer.h"
#include "ISF_FastCaloSimParametrization/CaloGeometryFromFile.h"
#include "ISF_FastCaloSimParametrization/CaloDetDescrElement.h"

#include "TCanvas.h" // Just for testing
#include <fstream>

using std::abs;
using std::atan2;

/** Constructor **/
ISF::FastCaloSimSvcV2::FastCaloSimSvcV2(const std::string& name, ISvcLocator* svc) :
  BaseSimulationSvc(name, svc),
  m_rndGenSvc("AtRndmGenSvc", name)
{
  declareProperty("ParamsInputFilename"        ,       m_paramsFilename);
  declareProperty("CaloCellsOutputName"        ,       m_caloCellsOutputName) ;
  declareProperty("CaloCellMakerTools_setup"   ,       m_caloCellMakerToolsSetup) ;
  declareProperty("CaloCellMakerTools_release" ,       m_caloCellMakerToolsRelease) ;
  declareProperty("DoRandomFluctuations"       ,       m_doRandomFluctuations) ;
  declareProperty("RandomSvc"                  ,       m_rndGenSvc                );
  declareProperty("RandomStream"               ,       m_randomEngineName         );
}

ISF::FastCaloSimSvcV2::~FastCaloSimSvcV2()
{}

/** framework methods */
StatusCode ISF::FastCaloSimSvcV2::initialize()
{
  ATH_MSG_DEBUG(m_screenOutputPrefix << "Initializing ...");

  m_paramsFile = TFile::Open(m_paramsFilename.c_str());
  ATH_MSG_DEBUG("------> file = " << m_paramsFile);

  TFCSPCAEnergyParametrization* epara_pions=new TFCSPCAEnergyParametrization("pions","pions");
  TFCSPCAEnergyParametrization* epara_photons=new TFCSPCAEnergyParametrization("photons","photons");
  TFCSPCAEnergyParametrization* epara_electrons=new TFCSPCAEnergyParametrization("electrons","electrons");

  m_eparas.push_back(epara_pions);
  m_eparas.push_back(epara_photons);
  m_eparas.push_back(epara_electrons);

  ATH_MSG_DEBUG("loading inputs pions");
  m_eparas[0]->loadInputs(m_paramsFile,"EnergyParams/pdgid_211/EN_50000/eta_0_20");
  ATH_MSG_DEBUG("loading inputs photons");
  m_eparas[1]->loadInputs(m_paramsFile,"EnergyParams/pdgid_22/EN_50000/eta_0_20");
  ATH_MSG_DEBUG("loading inputs electrons");
  m_eparas[2]->loadInputs(m_paramsFile,"EnergyParams/pdgid_11/EN_65000/eta_0_20");
  ATH_MSG_DEBUG("loading electrons done!");

  /** commenting out close and delete: accessing  the file in ::simulate */
  // file->Close();
  //delete file;

  ATH_CHECK(m_rndGenSvc.retrieve());
  m_randomEngine = m_rndGenSvc->GetEngine( m_randomEngineName);
  if(!m_randomEngine)
    {
      ATH_MSG_ERROR("Could not get random number engine from RandomNumberService. Abort.");
      return StatusCode::FAILURE;
    }

  m_caloGeo = new CaloGeometryFromFile();
  TString path_to_fcal_geo_files = "/afs/cern.ch/atlas/groups/Simulation/FastCaloSimV2/";
  m_caloGeo->LoadGeometryFromFile("/afs/cern.ch/atlas/groups/Simulation/FastCaloSim/ATLAS-GEO-20-00-01.root", "ATLAS-GEO-20-00-01");
  m_caloGeo->LoadFCalGeometryFromFiles(path_to_fcal_geo_files + "FCal1-electrodes.sorted.HV.09Nov2007.dat", path_to_fcal_geo_files + "FCal2-electrodes.sorted.HV.April2011.dat", path_to_fcal_geo_files + "FCal3-electrodes.sorted.HV.09Nov2007.dat");

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

  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode ISF::FastCaloSimSvcV2::finalize()
{
  ATH_MSG_DEBUG(m_screenOutputPrefix << "Finalizing ...");
  return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimSvcV2::setupEvent()
{
  ATH_MSG_DEBUG(m_screenOutputPrefix << "setupEvent NEW EVENT!");

  m_theContainer = new CaloCellContainer(SG::VIEW_ELEMENTS);

  // Only record if the container has not already been recorded
  if (!evtStore()->contains<CaloCellContainer>(m_caloCellsOutputName)) {
    if (evtStore()->record(m_theContainer, m_caloCellsOutputName).isFailure()) {
      ATH_MSG_FATAL( m_screenOutputPrefix << "cannot record CaloCellContainer " << m_caloCellsOutputName );
      return StatusCode::FAILURE;
    }
  }

  ATH_CHECK( m_caloCellMakerToolsSetup.retrieve() );
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

  ATH_MSG_DEBUG(m_screenOutputPrefix << "release Event");

  ATH_CHECK( m_caloCellMakerToolsRelease.retrieve() );
  //run release tools in a loop
  ToolHandleArray<ICaloCellMakerTool>::iterator itrTool = m_caloCellMakerToolsRelease.begin();
  ToolHandleArray<ICaloCellMakerTool>::iterator endTool = m_caloCellMakerToolsRelease.end();
  for (; itrTool != endTool; ++itrTool)
    {
      ATH_MSG_DEBUG( m_screenOutputPrefix << "Calling tool " << itrTool->name() );

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
  ATH_MSG_DEBUG("NEW PARTICLE! FastCaloSimSvcV2 called with ISFParticle: " << isfp);

  int pdgid = fabs(isfp.pdgCode());
  int barcode=isfp.barcode();

  if(barcode!=10001)
    {
      ATH_MSG_DEBUG("ISF particle barcode is barcode "<<barcode<<". Go to next Particle.");
      return StatusCode::SUCCESS;
    }

  Amg::Vector3D particle_position =  isfp.position();
  double eta(0.), phi(0.);
  eta = particle_position.eta();
  phi = particle_position.phi();
  if(abs(eta) > 0.3 || abs(eta) < 0.15) //somewhat enlarged to not cut off too many particles
    {
      ATH_MSG_DEBUG("ISF particle is out of eta range: "<<eta<<". Go to next Particle.");
      return StatusCode::SUCCESS;
    }
  if(pdgid==22 || pdgid==211 || pdgid==11)
    {

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
          if (uniform > n * 1.0 / (double)n_pcabins && uniform < (n + 1.) * 1.0 / (double)n_pcabins)
            pcabin = n + 1;
        }

      ATH_MSG_DEBUG("pca bin "<<pcabin);

      //-----ENERGY:-----

      TFCSSimulationState simulstate;
      simulstate.set_Ebin(pcabin);

      m_eparas[index_epara]->simulate(simulstate, nullptr, nullptr);

      ATH_MSG_DEBUG("Energy returned: " << simulstate.E());
      ATH_MSG_DEBUG("Energy fraction for layer: ");
      for (int s = 0; s < CaloCell_ID_FCS::MaxSample; s++)
        ATH_MSG_VERBOSE(" Sampling " << s << " energy " << simulstate.E(s));

      //-----SHAPE:-----

      if (m_doRandomFluctuations == true && pdgid == 211)
        {
          ATH_MSG_DEBUG("\n\n*******************************************\n"
                       << "RANDOM FLUCTUATIONS ARE ON"
                       << "\n*******************************************\n\n");
        }

      else {

        ATH_MSG_DEBUG("\n\n*******************************************\n"
                     << "RANDOM FLUCTUATIONS ARE OFF"
                     << "\n*******************************************\n\n");
      }

      /** get the relevant calo layers */
      IntArray *m_layers = m_eparas[index_epara]->get_layers();

      /** get the appropriate input histogram */
      std::string inputHisto = "";

      for ( int ilayer = 0;  ilayer < m_layers->GetSize(); ilayer++)
        {

          /** get the layer ID and associated energy */
          int layer =  m_layers->GetAt(ilayer);
          double layerE = simulstate.E(layer);
          int nHits=-1;

          ATH_MSG_DEBUG("NOW RUNNING ON LAYER "<<layer);

          inputHisto = "ShapeParams/pdgid_" + std::to_string(pdgid) + "/EN_50000/eta_0_20/hEnergyDensity_layer" + std::to_string(layer) + "_pca" + std::to_string(pcabin);
          m_histEnergyDensity2D = (TH2F*)m_paramsFile->Get(inputHisto.c_str());

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

          /** set number of hits due to random fluctuations if particle is a pion and flag is set */
          if (m_doRandomFluctuations == true && pdgid == 211) nHits = 10;

          /** otherwise get the number of hits to account for the resolution of the layer */
          else nHits = hitCalc(layerE, layer, pdgid);

          ATH_MSG_DEBUG("number of HITS = "<<nHits);

          /** simulate random hits from the input histogram */
          for (int i = 0; i < nHits; i++)
            {
              //std::cout<<"Hit nr "<<i<<std::endl;

              double r, alpha;
              double delta_phi,delta_phi_mm;

              const CaloGeoDetDescrElement* mcell = 0;
              m_histEnergyDensity2D->GetRandom2(alpha, r);

              //std::cout<<"got a hit positon from the histogram!"<<" r "<<r<<" alpha "<<alpha<<std::endl;

              //double r_layer=m_rlayers[ilayer*n_pcabins+pcabin-1];
              double r_layer=m_rlayer[ilayer];
              double hit_eta=findHitEta(alpha,r,r_layer,particle_position.z(),eta);

              delta_phi_mm = r * sin(alpha);
              delta_phi=delta_phi_mm/r_layer;
              double hit_phi = phi+delta_phi;

              if (layer < 21){
                double wiggle = 0.0;
                if (layer < 4 && layer > 0){
                  wiggle = doWiggle(layer);
                  //std::cout << "wiggle is " << wiggle << std::endl;
                }
                mcell = m_caloGeo->getDDE(layer, hit_eta, hit_phi-wiggle);
              }

              else
                {

                  double theta=2.*atan(exp(-hit_eta));
                  double rT=particle_position.z()*tan(theta);
                  double hit_x=rT*cos(hit_phi);
                  double hit_y=rT*sin(hit_phi);

                  mcell = m_caloGeo->getFCalDDE(layer, hit_x, hit_y, particle_position.z());
                }

              if (!mcell) continue;


              CaloCell* theCell = (CaloCell*)m_theContainer->findCell(mcell->calo_hash());

              if(i<10)
                {
                  ATH_MSG_DEBUG("Hit nr "<<i<<" eta: " << hit_eta << " phi: " << hit_phi << " Particle eta: " << eta << " phi: " << phi << " delta_eta: " << hit_eta - eta << " delta_phi: " << hit_phi - phi);
                  ATH_MSG_DEBUG(" Cell from CaloGeometry: eta: " << mcell->eta() << " phi: " << mcell->phi() << " |CELL_eta - HIT_eta| " << abs(mcell->eta() - hit_eta)  << " |CELL_phi - HIT_phi| " << abs(mcell->phi() - hit_phi));
                  ATH_MSG_DEBUG(" energy input into cell: "<<layerE / nHits);
                }

              theCell->addEnergy(layerE / nHits);

            }
        }

    } //pdgid must be 211 or 22 or 11
  else
    {
      ATH_MSG_DEBUG("Oh no! ISF particle has pdgid "<<pdgid<<" . That's not supported yet :(");
    }

  return StatusCode::SUCCESS;

}



//-----HITS:-----

int ISF::FastCaloSimSvcV2::hitCalc(double energy, int calolayer, int pdgid){

  int hits=1;

  /** Calculate estimates for cell energy fluctuations. Should be updated with better numbers */

  if(energy>0.0001)
    {

      double stochastic=0.1;
      double constant=0.0;

      if(pdgid == 211){

        if(calolayer >= 0 && calolayer<= 7){
          stochastic=0.101;     //LAr      10.1%/sqrt(E)
          constant=0.002;
        }

        else if(calolayer >= 8 && calolayer <= 11){
          stochastic=0.706;     //HadEC  70.6%/sqrt(E) for pions
          constant=0.058;
        }

        else if(calolayer >= 12 && calolayer <= 20){
          stochastic=0.564;     //TileCal 56.4%/sqrt(E)
          constant=0.055;
        }

        else if(calolayer >= 21 && calolayer <= 23){
          stochastic=0.942;     //FCAL    94.2%/sqrt(E) for pions
          constant=0.075;
        }
      }

      else {
        if(calolayer >= 0 && calolayer<= 7){
          stochastic=0.101;     //LAr      10.1%/sqrt(E)
          constant=0.002;
        }

        else if(calolayer >= 8 && calolayer <= 11){
          stochastic=0.214;     //HadEC 21.4%/sqrt(E)
          constant=0.0;
        }

        else if(calolayer >= 12 && calolayer <= 20){
          stochastic=0.564;     //TileCal 56.4%/sqrt(E)
          constant=0.055;
        }

        else if(calolayer >= 21 && calolayer <= 23){
          stochastic=0.285;     //FCAL    28.5%/sqrt(E)
          constant=0.035;
        }
      }

      hits = 1.0 / ((stochastic/sqrt(energy/1000.0))*(stochastic/sqrt(energy/1000.0)) + constant*constant);

    }
  else
    ATH_MSG_DEBUG("Something is wrong. Layer "<<calolayer<<" has very little energy "<<energy);

  return hits;

}
double ISF::FastCaloSimSvcV2::findHitEta(const double alpha,const double r, const double r_layer,const double z_particle,const double eta_particle){
  const double delta_eta_mm = r * cos(alpha);
  const double R=sqrt(r_layer*r_layer + z_particle*z_particle);
  double x=eta_particle; // approximation of hit_eta, starting point for iterations

  double a,b;
  double c,d;
  double delta;
  double e;

  const double epsilon=0.0001;
  do{
    c=exp(-x);
    d=1./(1.+(c*c));
    e=(2.*R)*(c*d);
    delta=x-eta_particle;
    a = e*delta - delta_eta_mm; // transformation
    b = e*(1. - (delta*d)); // first derivative of the transformation
    x = x - a/b;
  } while(abs(a) > epsilon);
  if(x!=x) ATH_MSG_ERROR("Error: Hit eta not defined.");
  //std::cout << "hit_eta: " << x << " error: " << a/b << std::endl;
  return x;
}

double ISF::FastCaloSimSvcV2::doWiggle(int layer)
{

  double wiggle = 0.0;
  double cell_dphi = 0.0;

  //Define cell size in phi
  if (layer == 1)
    cell_dphi = 0.0981748;
  else if (layer == 2 || layer == 3)
    cell_dphi = 0.0245437;
  else{
    ATH_MSG_ERROR("I am inside the wiggle calculation, but the layer is " << layer << "!");
    return 0.0;
  }

  //Set random numbers
  double searchRand = CLHEP::RandFlat::shoot(m_randomEngine);

  //Now for layer dependant approach
  if (layer == 1){
    double m_wiggleLayer1[]={0.0110626,0.0242509,0.0398173,0.055761,0.0736173,0.0938847,0.115154,0.13639,0.157644,0.178934,0.200182,0.221473,0.242745,0.264019,0.285264,0.306527,0.327811,0.349119,0.370387,0.391668,0.412922,0.434208,0.45546,0.476732,0.498023,0.51931,0.540527,0.561799,0.583079,0.604358,0.625614,0.646864,0.668112,0.689351,0.710629,0.731894,0.75318,0.774426,0.795695,0.81699,0.838258,0.859528,0.880783,0.90202,0.922515,0.941276,0.958477,0.975062,0.988922,1};

    int chosenBin = (Int_t) TMath::BinarySearch(50, m_wiggleLayer1, searchRand);
    double x_wigg = ((-0.98)+(chosenBin+1)*0.04)/2;
    wiggle = x_wigg*cell_dphi/4;
  }

  if (layer == 2){
    double m_wiggleLayer2[]={0.0127507,0.0255775,0.0395137,0.0542644,0.0695555,0.0858206,0.102274,0.119653,0.137832,0.156777,0.176938,0.197727,0.217576,0.236615,0.256605,0.277766,0.2995,0.321951,0.344663,0.367903,0.392401,0.417473,0.443514,0.470867,0.498296,0.52573,0.553114,0.57921,0.604326,0.628822,0.652191,0.674853,0.697268,0.718983,0.739951,0.759866,0.778877,0.798762,0.819559,0.839789,0.858923,0.877327,0.894831,0.911693,0.92821,0.94391,0.959156,0.973593,0.986752,1};

    int chosenBin = (Int_t) TMath::BinarySearch(50, m_wiggleLayer2, searchRand);
    double x_wigg = ((-0.98)+(chosenBin+1)*0.04)/2;
    wiggle = x_wigg*cell_dphi;
  }

  if (layer == 3){
    double m_wiggleLayer3[]={0.0217932,0.0438502,0.0670992,0.091085,0.11651,0.143038,0.169524,0.196205,0.222944,0.249703,0.276629,0.303559,0.33034,0.356842,0.383579,0.410385,0.437272,0.464214,0.49118,0.518202,0.545454,0.572667,0.600037,0.627544,0.655072,0.6826,0.709824,0.733071,0.754764,0.775672,0.793834,0.810904,0.828219,0.844119,0.858339,0.871248,0.882485,0.894889,0.907955,0.920289,0.931136,0.941039,0.949844,0.957641,0.965787,0.97392,0.981706,0.988892,0.994527,1};

    int chosenBin = (Int_t) TMath::BinarySearch(50, m_wiggleLayer3, searchRand);
    double x_wigg = ((-0.98)+(chosenBin+1)*0.04)/2;
    wiggle = x_wigg*cell_dphi;
  }

  return wiggle;

}
