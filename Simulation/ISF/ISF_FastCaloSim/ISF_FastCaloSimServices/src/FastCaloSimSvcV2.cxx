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




#include "CaloEvent/CaloCellContainer.h"
#include "ISF_FastCaloSimParametrization/CaloGeometryFromFile.h"
#include "ISF_FastCaloSimParametrization/CaloDetDescrElement.h"

#include "TCanvas.h" // Just for testing
#include <fstream>

using std::abs;
using std::atan2;

/** Constructor **/
ISF::FastCaloSimSvcV2::FastCaloSimSvcV2(const std::string& name, ISvcLocator* svc) :
  BaseSimulationSvc(name, svc)
  //m_epara("etest", "etest")
{
  declareProperty("ParamsInputFilename"        ,       m_paramsFilename);
  declareProperty("CaloCellsOutputName"        ,       m_caloCellsOutputName) ;
  declareProperty("CaloCellMakerTools_setup"   ,       m_caloCellMakerToolsSetup) ;
  declareProperty("CaloCellMakerTools_release" ,       m_caloCellMakerToolsRelease) ;
  declareProperty("DoRandomFluctuations"       ,       m_doRandomFluctuations) ;
}

ISF::FastCaloSimSvcV2::~FastCaloSimSvcV2()
{}

/** framework methods */
StatusCode ISF::FastCaloSimSvcV2::initialize()
{
  ATH_MSG_INFO(m_screenOutputPrefix << "Initializing ...");

  m_paramsFile = TFile::Open(m_paramsFilename.c_str());
  ATH_MSG_INFO("------> file = " << m_paramsFile);
  
  
  TFCSPCAEnergyParametrization* epara_pions=new TFCSPCAEnergyParametrization("pions","pions");
  TFCSPCAEnergyParametrization* epara_photons=new TFCSPCAEnergyParametrization("photons","photons");
  TFCSPCAEnergyParametrization* epara_electrons=new TFCSPCAEnergyParametrization("electrons","electrons");
  
  m_eparas.push_back(epara_pions);
  m_eparas.push_back(epara_photons);
  m_eparas.push_back(epara_electrons);
  
  //TFile* eparaFile = TFile::Open("/afs/cern.ch/user/s/schaarsc/public/fastcalo/epara_storage/photons/secondPCA.root");
  //m_epara.loadInputs(eparaFile);
  
  std::cout<<"loading inputs pions"<<std::endl;
  m_eparas[0]->loadInputs(m_paramsFile,"EnergyParams/pdgid_211/EN_50000/eta_0_20");
  std::cout<<"loading inputs photons"<<std::endl;
  m_eparas[1]->loadInputs(m_paramsFile,"EnergyParams/pdgid_22/EN_50000/eta_0_20");
  std::cout<<"loading inputs electrons"<<std::endl;
  m_eparas[2]->loadInputs(m_paramsFile,"EnergyParams/pdgid_11/EN_65000/eta_0_20");
  std::cout<<"loading electrons done!"<<std::endl;
  
  /** commenting out close and delete: accessing  the file in ::simulate */
  // file->Close();
  //delete file;

  m_random = new TRandom3();
  m_random->SetSeed(0);
  
  //std::cout << "Loading CaloGeometry from file:" << std::endl;
  m_caloGeo = new CaloGeometryFromFile();
  TString path_to_fcal_geo_files = "/afs/cern.ch/atlas/groups/Simulation/FastCaloSimV2/";
  m_caloGeo->LoadGeometryFromFile("/afs/cern.ch/atlas/groups/Simulation/FastCaloSim/ATLAS-GEO-20-00-01.root", "ATLAS-GEO-20-00-01");
  m_caloGeo->LoadFCalGeometryFromFiles(path_to_fcal_geo_files + "FCal1-electrodes.sorted.HV.09Nov2007.dat", path_to_fcal_geo_files + "FCal2-electrodes.sorted.HV.April2011.dat", path_to_fcal_geo_files + "FCal3-electrodes.sorted.HV.09Nov2007.dat");
  //std::cout << "Done." << std::endl;
  
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
  ATH_MSG_INFO(m_screenOutputPrefix << "Finalizing ...");
  return StatusCode::SUCCESS;
}

StatusCode ISF::FastCaloSimSvcV2::setupEvent()
{
  ATH_MSG_INFO(m_screenOutputPrefix << "setupEvent");

  //std::cout << "FastCaloSimSvcV2 creates the cell Container " << std::endl;
  m_theContainer = new CaloCellContainer(SG::VIEW_ELEMENTS);

  //std::cout << "FastCaloSimSvcV2 writes cell container to storegate" << std::endl;

  StatusCode sc;
  
  // Only record if the container has not already been recorded
  if (!evtStore()->contains<CaloCellContainer>(m_caloCellsOutputName)) {
    sc = evtStore()->record(m_theContainer, m_caloCellsOutputName);
  }
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
    //std::cout << "FastCaloSimSvcV2 setup tool " << itrTool->name() << std::endl;

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
  //std::cout << "release Event" << std::endl;

  CHECK( m_caloCellMakerToolsRelease.retrieve() );
  std::cout << "Successfully retrieve CaloCellMakerTools: " << m_caloCellMakerToolsRelease << std::endl;
  //run release tools in a loop
  ToolHandleArray<ICaloCellMakerTool>::iterator itrTool = m_caloCellMakerToolsRelease.begin();
  ToolHandleArray<ICaloCellMakerTool>::iterator endTool = m_caloCellMakerToolsRelease.end();
  for (; itrTool != endTool; ++itrTool)
  {
    //ATH_MSG_INFO( m_screenOutputPrefix << "Calling tool " << itrTool->name() );
    std::cout << "Calling tool " << itrTool->name() << std::endl;

    StatusCode sc = (*itrTool)->process(m_theContainer);

    std::cout << "done with tool " << itrTool->name() << std::endl;

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
   ATH_MSG_INFO("FastCaloSimSvcV2 called with ISFParticle: " << isfp);
  //std::cout<<"FastCaloSimSvcV2 called with ISFParticle: " << isfp<<std::endl;

  int pdgid = fabs(isfp.pdgCode());
  
  if(pdgid==22 || pdgid==211 || pdgid==11)
  {
	  
	  int index_epara=0;
	  if(pdgid==22) index_epara=1;
	  if(pdgid==11) index_epara=2;
	  
	  /** get the number of pca bins from the epara: **/
	
	  int n_pcabins = m_eparas[index_epara]->n_pcabins();
	  //determine the pca bin randomly:
	  int pcabin = 1;
	  double uniform = m_random->Uniform(1);
	  for (int n = 0; n < n_pcabins; n++)
	  {
	    if (uniform > n * 1.0 / (double)n_pcabins && uniform < (n + 1.) * 1.0 / (double)n_pcabins)
	      pcabin = n + 1;
	  }
	
	  //std::cout<<"pca bin "<<pcabin<<std::endl;
	
	  //-----ENERGY:-----
	
	  TFCSSimulationState simulstate;
	  simulstate.set_Ebin(pcabin);
	
	  m_eparas[index_epara]->simulate(simulstate, nullptr, nullptr);
	
	  ATH_MSG_INFO("Energy returned: " << simulstate.E());
	  ATH_MSG_INFO("Energy fraction for layer: ");
//	  for (int s = 0; s < CaloCell_ID_FCS::MaxSample; s++)
//	    ATH_MSG_INFO(" Sampling " << s << " energy " << simulstate.E(s));
	
	  //-----SHAPE:-----
		double eta(0.), phi(0.);
	  //Amg::Vector3D particle_momentum =  isfp.momentum();
	  Amg::Vector3D particle_position =  isfp.position();
	  eta = particle_position.eta();
	  phi = particle_position.phi();  
	  
	  if(abs(eta) > 0.25 || abs(eta) < 0.2)
	   return StatusCode::SUCCESS; 
	  
	  if (m_doRandomFluctuations == true && pdgid == 211) {

	    ATH_MSG_INFO("\n\n*******************************************\n"
		      << "RANDOM FLUCTUATIONS ARE ON"
		      << "\n*******************************************\n\n");
	  }

	  else {

	    ATH_MSG_INFO("\n\n*******************************************\n"
		      << "RANDOM FLUCTUATIONS ARE OFF"
		      << "\n*******************************************\n\n");
	  }
	 
	  /** get the relevant calo layers */
	  IntArray *m_layers = m_eparas[index_epara]->get_layers();
	  
	  //std::cout<<"CHECK layers->GetSize() "<<m_layers->GetSize()<<std::endl;
	  //for(int i=0;i<m_layers->GetSize();i++)
	   //std::cout<<"check: LAYER " <<m_layers->GetAt(i)<<std::endl;
	  
	  
	  /** get the appropriate input histogram */
	  std::string inputHisto = "";
	  
	  for ( int ilayer = 0;  ilayer < m_layers->GetSize(); ilayer++)
	  {
	
	    /** get the layer ID and associated energy */
	    int layer =  m_layers->GetAt(ilayer);
	    double layerE = simulstate.E(layer);
	    int nHits=-1;
	
//	    ATH_MSG_INFO("NOW RUNNING ON LAYER "<<layer);
	
	    inputHisto = "ShapeParams/pdgid_" + std::to_string(pdgid) + "/EN_50000/eta_0_20/hEnergyDensity_layer" + std::to_string(layer) + "_pca" + std::to_string(pcabin);
	
	    //std::cout << " histogram_name = " << inputHisto << std::endl;
	    m_histEnergyDensity2D = (TH2F*)m_paramsFile->Get(inputHisto.c_str());
	    
	    if(!m_paramsFile) {
        std::cout<<"Problem. No file!! :-("<<std::endl;
        return StatusCode::FAILURE;
      }
	    if(!m_histEnergyDensity2D) { 
        std::cout<<"Problem. No histogram!! :-("<<std::endl;
        return StatusCode::FAILURE;
      }
	    

	    /** set number of hits due to random fluctuations if particle is a pion and flag is set */
	    if (m_doRandomFluctuations == true && pdgid == 211) nHits = 10;

	    /** otherwise get the number of hits to account for the resolution of the layer */
	    else nHits = hitCalc(layerE, layer, pdgid);
      
	    //std::cout<<"number of HITS = "<<nHits<<std::endl;
	    
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
	      
	      if (layer < 21) mcell = m_caloGeo->getDDE(layer, hit_eta, hit_phi);
	      else {
					
					double theta=2.*atan(exp(-hit_eta));
					double rT=particle_position.z()*tan(theta);
					double hit_x=rT*cos(hit_phi);
					double hit_y=rT*sin(hit_phi);
					
					mcell = m_caloGeo->getFCalDDE(layer, hit_x, hit_y, particle_position.z());      
				}
	      
	      if (!mcell) continue;
	
	      
	      CaloCell* theCell = (CaloCell*)m_theContainer->findCell(mcell->calo_hash());
	      	      
	      //ATH_MSG_INFO("Hit eta: " << hit_eta << " phi: " << hit_phi << " Particle eta: " << eta << " phi: " << phi << " delta_eta: " << hit_eta - eta << " delta_phi: " << hit_phi - phi);
	      //ATH_MSG_INFO("Cell from CaloGeometry: eta: " << mcell->eta() << " phi: " << mcell->phi() << " |CELL_eta - HIT_eta| " << abs(mcell->eta() - hit_eta)  << " |CELL_phi - HIT_phi| " << abs(mcell->phi() - hit_phi));
	      
	      //std::cout << "Hit eta: " << hit_eta << " phi: " << hit_phi << " Particle eta: " << eta << " phi: " << phi << " delta_eta: " << hit_eta - eta << " delta_phi: " << hit_phi - phi << std::endl;
	      //std::cout << "Cell from CaloGeometry: eta: " << mcell->eta() << " phi: " << mcell->phi() << " |CELL_eta - HIT_eta| " << abs(mcell->eta() - hit_eta)  << " |CELL_phi - HIT_phi| " << abs(mcell->phi() - hit_phi) << std::endl;
	      
	      theCell->addEnergy(layerE / nHits);
	      
	    }
	  }
	
  } //pdgid must be 211 or 22 or 11
  else
  {
   ATH_MSG_INFO("Oh no! ISF particle has pdgid "<<pdgid<<" . That's not supported yet :(");
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
   std::cout<<"Something is wrong. Layer "<<calolayer<<" has very little energy "<<energy<<std::endl;
  
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
	if(x!=x) std::cout << "Error: Hit eta not defined."<< std::endl; 
	//std::cout << "hit_eta: " << x << " error: " << a/b << std::endl;
	return x;
}
