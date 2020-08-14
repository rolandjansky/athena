/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSEnergyAndHitGAN.h"
#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitBase.h"
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"
#include "ISF_FastCaloSimEvent/TFCSCenterPositionCalculation.h"

#include "TFile.h"

#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"

#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"

#if defined(__FastCaloSimStandAlone__)
#include "CLHEP/Random/TRandomEngine.h"
#else
#include <CLHEP/Random/RanluxEngine.h>
#endif

#include <iostream>
#include <fstream>

//LWTNN
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/parse_json.hh"

// XML reader
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

//=============================================
//======= TFCSEnergyAndHitGAN =========
//=============================================

TFCSEnergyAndHitGAN::TFCSEnergyAndHitGAN(const char* name, const char* title):TFCSParametrizationBinnedChain(name,title)
{
  set_GANfreemem();
}

TFCSEnergyAndHitGAN::~TFCSEnergyAndHitGAN()
{
  if(m_input!=nullptr) {
    delete m_input;
  }
  if(m_graph!=nullptr) {
    delete m_graph;
  }
}

unsigned int TFCSEnergyAndHitGAN::get_nr_of_init(unsigned int bin) const
{
  if(bin>=m_bin_ninit.size()) return 0;
  return m_bin_ninit[bin];
}

void TFCSEnergyAndHitGAN::set_nr_of_init(unsigned int bin,unsigned int ninit)
{
  if(bin>=m_bin_ninit.size()) {
    m_bin_ninit.resize(bin+1,0);
    m_bin_ninit.shrink_to_fit();
  }
  m_bin_ninit[bin]=ninit; 
}

// initialize lwtnn network 
bool TFCSEnergyAndHitGAN::initializeNetwork(int pid,int etaMin,std::string FastCaloGANInputFolderName)
{

  // initialize all necessary constants
  // FIXME eventually all these could be stored in the .json file

  ATH_MSG_INFO("Using FastCaloGANInputFolderName: " << FastCaloGANInputFolderName );
  // get neural net JSON file as an std::istream object    
  int etaMax = etaMin + 5;
  
  reset_match_all_pdgid();
  set_pdgid(pid);
  if(pid==11) add_pdgid(-pid);
  if(pid==211) add_pdgid(-pid);
  set_eta_min(etaMin/100.0);
  set_eta_max(etaMax/100.0);
  set_eta_nominal((etaMin+etaMax)/200.0);

  std::string inputFile = FastCaloGANInputFolderName + "/neural_net_" + std::to_string(pid) +"_eta_" + std::to_string(etaMin) +"_" + std::to_string(etaMax) +".json";
  if (inputFile.empty()){
    ATH_MSG_ERROR("Could not find json file " << inputFile );
    return false;
  } else {
    ATH_MSG_INFO("For pid: " << pid <<" and eta " << etaMin <<"-" << etaMax <<", loading json file " << inputFile );
    std::ifstream input(inputFile);
    std::stringstream sin;
    sin << input.rdbuf();
    input.close();
    // build the graph
    auto config=lwt::parse_json_graph(sin);
    m_graph=new lwt::LightweightGraph(config);
    if (m_graph==nullptr){
      ATH_MSG_ERROR("Could not create LightWeightGraph from  " << inputFile );
      return false;
    }
    if(m_input!=nullptr) {
      delete m_input;
    }
    m_input = new std::string(sin.str());
  }                
  m_GANLatentSize = 50; 
    
  //Get all Binning histograms to store in memory
  GetBinning(pid,(etaMin+etaMax)/2,FastCaloGANInputFolderName);    

  if (m_GANLatentSize==0){
    ATH_MSG_ERROR("m_GANLatentSize uninitialized!");
    return false;
  }

  return true;
}

void TFCSEnergyAndHitGAN::GetBinning(int pid,int etaMid,std::string FastCaloGANInputFolderName){ 
   std::string xmlFullFileName = FastCaloGANInputFolderName + "/binning.xml";
   ATH_MSG_DEBUG("Opening XML file in "<< xmlFullFileName);
   
   std::vector<Binning> AllBinning;
   std::vector<int> EtaMaxList;
   
   xmlDocPtr doc = xmlParseFile( xmlFullFileName.c_str() );
   for( xmlNodePtr nodeRoot = doc->children; nodeRoot != NULL; nodeRoot = nodeRoot->next) {
      if (xmlStrEqual( nodeRoot->name, BAD_CAST "Bins" )) {
         for( xmlNodePtr nodeBin = nodeRoot->children; nodeBin != NULL; nodeBin = nodeBin->next ) {
            if (xmlStrEqual( nodeBin->name, BAD_CAST "Bin" )) {
               int nodePid = atof( (const char*) xmlGetProp( nodeBin, BAD_CAST "pid" ) );
               //int nodeEtaMin = atof( (const char*) xmlGetProp( nodeBin, BAD_CAST "etaMin" ) );
               int nodeEtaMax = atof( (const char*) xmlGetProp( nodeBin, BAD_CAST "etaMax" ) );
               
               Binning binsInLayer;
               bool correctentry=true;
	             if(nodePid!=pid) correctentry=false;
               
               for( xmlNodePtr nodeLayer = nodeBin->children; nodeLayer != NULL; nodeLayer = nodeLayer->next ) {
                  if( xmlStrEqual( nodeLayer->name, BAD_CAST "Layer" ) ) {
                     std::vector<double> edges; 
                     std::string s( (const char*)xmlGetProp( nodeLayer, BAD_CAST "r_edges" ) );
                    
                     std::istringstream ss(s);
                     std::string token;
 
                     while(std::getline(ss, token, ',')) {
                        edges.push_back(atof( token.c_str() ));
                     }
                     
                     int binsInAlpha = atof( (const char*) xmlGetProp( nodeLayer, BAD_CAST "n_bin_alpha" ) );
                     int layer = atof( (const char*) xmlGetProp( nodeLayer, BAD_CAST "id" ) );
                     
                     if(correctentry) ATH_MSG_DEBUG("nodepid="<<nodePid<<" nodeEtaMax="<<nodeEtaMax<<" Layer: "<<layer<<" binsInAlpha: "<<binsInAlpha<<" edges: "<< s);
                     
                     std::string name = "hist_pid_" + std::to_string(nodePid) + "_etaSliceNumber_" + std::to_string(EtaMaxList.size()) + "_layer_" + std::to_string(layer);
                     int xBins = edges.size()-1;
                     if (xBins == 0) {
                       xBins = 1; //remove warning
                       edges.push_back(0);
                       edges.push_back(1);
                     }  
                     binsInLayer[layer] = TH2D(name.c_str(), name.c_str(), xBins, &edges[0], binsInAlpha, -TMath::Pi(), TMath::Pi());
                     binsInLayer[layer].SetDirectory(0);
                  }
               }
               
	             if(!correctentry) continue;
               AllBinning.push_back(binsInLayer);
               EtaMaxList.push_back(nodeEtaMax);
            }         
         }
      }
   }
   
   int index = 0;
   for (int etaMax : EtaMaxList){
     if (etaMid < etaMax) {
       m_Binning=AllBinning[index];
       break;
     }
     index++;
   }
   
   ATH_MSG_DEBUG("Done XML file");
}   


const std::string TFCSEnergyAndHitGAN::get_variable_text(TFCSSimulationState& simulstate,const TFCSTruthState*, const TFCSExtrapolationState*) const
{
  return std::string(Form("layer=%d",simulstate.getAuxInfo<int>("GANlayer"_FCShash)));
}

bool TFCSEnergyAndHitGAN::fillFastCaloGanNetworkInputs(TFCSSimulationState& simulstate,const TFCSTruthState* truth, NetworkInputs & inputs,double & trueEnergy) const
{
  // fill randomize latent space
  //FIXME: should this really be momentum
  trueEnergy = truth->P();
  double randUniformZ = 0.;
  //FIXME: make dependency on input particle eta, pdgid and energy
  for (int i = 0; i< m_GANLatentSize; i ++)
  {
    randUniformZ = CLHEP::RandFlat::shoot(simulstate.randomEngine(), -1., 1.);
    inputs["node_0"].insert ( std::pair<std::string,double>(std::to_string(i), randUniformZ) );
  }
  
  //std::cout << "Check label: " <<trueEnergy <<" "<<std::pow(2,22)<<" "<<trueEnergy/std::pow(2,22)<<std::endl;
  inputs["node_1"].insert ( std::pair<std::string,double>("0", trueEnergy/(std::pow(2,22))) );

  return true;
}

bool TFCSEnergyAndHitGAN::fillEnergy(TFCSSimulationState& simulstate, const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol, NetworkInputs inputs) const
{
  const int     pdgId    = truth->pdgid();
  const double  charge   = HepPDT::ParticleID(pdgId).charge();
  const double EKin = truth->Ekin();

  ATH_MSG_VERBOSE("Momentum " << truth->P() <<" pdgId " << truth->pdgid());
  
  // compute the network output values
  //ATH_MSG_VERBOSE("neural network input = "<<inputs);
  ATH_MSG_VERBOSE("input size " << inputs["node_0"].size());
  NetworkOutputs outputs = m_graph->compute(inputs);
  //ATH_MSG_VERBOSE("neural network output = "<<outputs);

  ATH_MSG_VERBOSE("neural network output size = "<<outputs.size());

  const Binning& binsInLayers = m_Binning;
  ATH_MSG_VERBOSE("Get binning");
   
  int vox = 0; 
  for (auto element : binsInLayers){
    int layer = element.first;
    simulstate.setAuxInfo<int>("GANlayer"_FCShash,layer);
    TFCSLateralShapeParametrizationHitBase::Hit hit;

    if(get_number_of_bins()>0) {
      const int bin=get_bin(simulstate,truth,extrapol);
      if(bin>=0 && bin<(int)get_number_of_bins()) {
        for(unsigned int ichain=m_bin_start[bin];ichain<TMath::Min(m_bin_start[bin]+get_nr_of_init(bin),m_bin_start[bin+1]);++ichain) {
          ATH_MSG_DEBUG("for "<<get_variable_text(simulstate,truth,extrapol)<<" run init "<<get_bin_text(bin)<<": "<<chain()[ichain]->GetName());
          if(chain()[ichain]->InheritsFrom(TFCSLateralShapeParametrizationHitBase::Class())) {
            TFCSLateralShapeParametrizationHitBase* sim=(TFCSLateralShapeParametrizationHitBase*)(chain()[ichain]);
            if(sim->simulate_hit(hit,simulstate,truth,extrapol)!=FCSSuccess) {
              ATH_MSG_ERROR("error for "<<get_variable_text(simulstate,truth,extrapol)<<" run init "<<get_bin_text(bin)<<": "<<chain()[ichain]->GetName());
              return false;
            }
          } else {
            ATH_MSG_ERROR("for "<<get_variable_text(simulstate,truth,extrapol)<<" run init "<<get_bin_text(bin)<<": "<<chain()[ichain]->GetName()<<" does not inherit from TFCSLateralShapeParametrizationHitBase");
            return false;
          }
        }
      } else {
        ATH_MSG_WARNING("nothing to init for "<<get_variable_text(simulstate,truth,extrapol)<<": "<<get_bin_text(bin));
      }
    }  

    int binResolution = 5;
    if (layer == 1 || layer == 5){
      binResolution = 1;
    }

    const double center_eta = hit.center_eta(); 
    const double center_phi = hit.center_phi();
    const double center_r   = hit.center_r();
    const double center_z   = hit.center_z();
    
    ATH_MSG_VERBOSE(" Layer "<< layer
         << " Extrap eta " << center_eta 
         << " phi " << center_phi 
         << " R " << center_r);

    const float dist000    = TMath::Sqrt(center_r * center_r + center_z * center_z);
    const float eta_jakobi = TMath::Abs(2.0 * TMath::Exp(-center_eta) / (1.0 + TMath::Exp(-2 * center_eta)));

    int nHitsAlpha;
    int nHitsR;

    TH2D* h = &element.second;
    int xBinNum = h->GetNbinsX();
    //If only one bin in r means layer is empty, no value should be added
    if (xBinNum == 1) {
        ATH_MSG_VERBOSE(" Layer "<< layer
         << " has only one bin in r, this means is it not used, skipping (this is needed to keep correct syncronisation of voxel and layers)");
        //delete h;
        continue;
    }
    int yBinNum = h->GetNbinsY();
    for (int iy = 1; iy <= yBinNum; ++iy){
      for (int ix = 1; ix <= xBinNum; ++ix){
        double energyInVoxel  = outputs["out_" + std::to_string(vox)];
        ATH_MSG_VERBOSE(" Vox "<< vox
        << " energy " << energyInVoxel 
        << " binx " << ix 
        << " biny " << iy);

        if (energyInVoxel == 0){
          vox++;
          continue;
        }
        
        simulstate.add_E(layer,EKin*energyInVoxel);
        
        TAxis* x = (TAxis*)h->GetXaxis();
        nHitsR = x->GetBinUpEdge(ix) - x->GetBinLowEdge(ix);
        if (yBinNum == 1){
          //nbins in alpha depend on circumference lenght
          double r = x->GetBinUpEdge(ix);
          nHitsAlpha = ceil(2 * TMath::Pi() * r / binResolution);
        }
        else{
          //d = 2•r•sin (a/2r) this distance at the upper r must be 1mm for layer 1 or 5, 5mm otherwise. 
          TAxis* y = (TAxis*)h->GetYaxis();
          double angle = y->GetBinUpEdge(iy) - y->GetBinLowEdge(iy);
          double r = x->GetBinUpEdge(ix);
          double d = 2 * r * sin(angle/2*r);
          nHitsAlpha = ceil(d/binResolution);
        }
        
        nHitsAlpha = std::min(10,std::max(1,nHitsAlpha));
        nHitsR = std::min(10,std::max(1,nHitsR));
        
        for (int ir = 0; ir < nHitsR; ++ir){
          TAxis* x = (TAxis*)h->GetXaxis();
          double r = x->GetBinLowEdge(ix) + x->GetBinWidth(ix)/(nHitsR+1)*ir;
          
          
          for (int ialpha = 0; ialpha < nHitsAlpha; ++ialpha){
            double alpha;
            if (yBinNum == 1){
              alpha = CLHEP::RandFlat::shoot(simulstate.randomEngine(), -M_PI , M_PI);
            }
            else {
               TAxis* y = (TAxis*)h->GetYaxis();
               alpha = y->GetBinLowEdge(iy) + y->GetBinWidth(iy)/(nHitsAlpha+1)*ialpha;
            }

            hit.reset();
            hit.E()=EKin*energyInVoxel/(nHitsAlpha*nHitsR);

            if (layer <=20){
               float delta_eta_mm = r * cos(alpha);
               float delta_phi_mm = r * sin(alpha);
               
               ATH_MSG_VERBOSE("delta_eta_mm "<< delta_eta_mm << " delta_phi_mm "<< delta_phi_mm);

               // Particles with negative eta are expected to have the same shape as those with positive eta after transformation: delta_eta --> -delta_eta
               if(center_eta<0.)delta_eta_mm = -delta_eta_mm;
               // Particle with negative charge are expected to have the same shape as positively charged particles after transformation: delta_phi --> -delta_phi
               if(charge < 0.)  delta_phi_mm = -delta_phi_mm;
              
               const float delta_eta = delta_eta_mm / eta_jakobi / dist000;
               const float delta_phi = delta_phi_mm / center_r;
              
               hit.eta() = center_eta + delta_eta;
               hit.phi() = center_phi + delta_phi;
               
               ATH_MSG_VERBOSE(" Hit eta " << hit.eta() 
                    << " phi " << hit.phi() 
                    << " layer " << layer );
            }
            else { //FCAL is in (x,y,z)
              const float hit_r = r*cos(alpha) + center_r;
              float delta_phi = r*sin(alpha)/center_r;

              // Particle with negative charge are expected to have the same shape as positively charged particles after transformation: delta_phi --> -delta_phi
              if(charge < 0.) delta_phi = -delta_phi;
              const float hit_phi= delta_phi + center_phi;
              hit.x() = hit_r*cos(hit_phi);
              hit.y() = hit_r*sin(hit_phi);
              hit.z() = center_z;
              ATH_MSG_VERBOSE(" Hit x " << hit.x() 
                    << " y " << hit.y() 
                    << " layer " << layer );
            }

            if(get_number_of_bins()>0) {
              const int bin=get_bin(simulstate,truth,extrapol);
              if(bin>=0 && bin<(int)get_number_of_bins()) {
                for(unsigned int ichain=m_bin_start[bin]+get_nr_of_init(bin);ichain<m_bin_start[bin+1];++ichain) {
                  ATH_MSG_DEBUG("for "<<get_variable_text(simulstate,truth,extrapol)<<" run "<<get_bin_text(bin)<<": "<<chain()[ichain]->GetName());
                  if(chain()[ichain]->InheritsFrom(TFCSLateralShapeParametrizationHitBase::Class())) {
                    TFCSLateralShapeParametrizationHitBase* sim=(TFCSLateralShapeParametrizationHitBase*)(chain()[ichain]);
                    if(sim->simulate_hit(hit,simulstate,truth,extrapol)!=FCSSuccess) {
                      ATH_MSG_ERROR("error for "<<get_variable_text(simulstate,truth,extrapol)<<" run init "<<get_bin_text(bin)<<": "<<chain()[ichain]->GetName());
                      return false;
                    }
                  } else {
                    ATH_MSG_ERROR("for "<<get_variable_text(simulstate,truth,extrapol)<<" run init "<<get_bin_text(bin)<<": "<<chain()[ichain]->GetName()<<" does not inherit from TFCSLateralShapeParametrizationHitBase");
                    return false;
                  }
                }
              } else {
                ATH_MSG_WARNING("nothing to do for "<<get_variable_text(simulstate,truth,extrapol)<<": "<<get_bin_text(bin));
              }
            } else {
              ATH_MSG_WARNING("no bins defined, is this intended?");
            }  
         }
        }
        vox++;
      }
    }
    
    ATH_MSG_VERBOSE("Number of voxels " << vox);
    
    //delete h;
    ATH_MSG_VERBOSE("Done layer "<<layer);
  }
  for(int ilayer=0;ilayer<CaloCell_ID_FCS::MaxSample;++ilayer) {
    simulstate.set_Efrac(ilayer,simulstate.E(ilayer)/simulstate.E());
  }

  ATH_MSG_VERBOSE("Done particle");

  return true;
}



FCSReturnCode TFCSEnergyAndHitGAN::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  for(unsigned int ichain=0;ichain<m_bin_start[0];++ichain) {
    ATH_MSG_DEBUG("now run for all bins: "<<chain()[ichain]->GetName());
    if (simulate_and_retry(chain()[ichain], simulstate, truth, extrapol) != FCSSuccess) {
      return FCSFatal;
    }
  }

  //Compute all inputs to the network
  NetworkInputs inputs;
  double trueEnergy;
  ATH_MSG_VERBOSE("Get Inputs");
  if (!fillFastCaloGanNetworkInputs(simulstate,truth,inputs,trueEnergy)) {
    ATH_MSG_WARNING("Could not initialize network ");
    // bail out but do not stop the job
    return FCSSuccess;
  }

  ATH_MSG_VERBOSE("Fill Energies");
  if (!fillEnergy(simulstate,truth,extrapol,inputs)) {
    ATH_MSG_WARNING("Could not fill energies ");
    // bail out but do not stop the job
    return FCSSuccess;
  }


  for(unsigned int ichain=m_bin_start.back();ichain<size();++ichain) {
    ATH_MSG_DEBUG("now run for all bins: "<<chain()[ichain]->GetName());
    if (simulate_and_retry(chain()[ichain], simulstate, truth, extrapol) != FCSSuccess) {
      return FCSFatal;
    }
  }

  return FCSSuccess;
}

void TFCSEnergyAndHitGAN::Print(Option_t *option) const
{
  TFCSParametrization::Print(option);
  TString opt(option);
  bool shortprint=opt.Index("short")>=0;
  bool longprint=msgLvl(MSG::DEBUG) || (msgLvl(MSG::INFO) && !shortprint);
  TString optprint=opt;optprint.ReplaceAll("short","");

  if(longprint) {
    ATH_MSG_INFO(optprint<<"  "<<"Graph="<<m_graph<<"; json input"<<m_input<<"; free mem="<<GANfreemem()<<"; latent space="<<m_GANLatentSize<<"; Binning size="<<m_Binning.size());
    for(auto& l : m_Binning) if(l.second.GetNbinsX()>1) {
      ATH_MSG_INFO(optprint<<"    "<<"layer="<<l.first<<" nR="<<l.second.GetNbinsX()<<" nalpha="<<l.second.GetNbinsY());
    }
  }  

  TString prefix="- ";
  for(unsigned int ichain=0;ichain<size();++ichain) {
    if(ichain==0 && ichain!=m_bin_start.front()) {
      prefix="> ";
      if(longprint) ATH_MSG_INFO(optprint<<prefix<<"Run for all bins");
    }  
    for(unsigned int ibin=0;ibin<get_number_of_bins();++ibin) {
      if(ichain==m_bin_start[ibin]) {
        if(ibin<get_number_of_bins()-1) if(ichain==m_bin_start[ibin+1]) continue;
        prefix=Form("%-2d",ibin);
        if(longprint) ATH_MSG_INFO(optprint<<prefix<<"Run for "<<get_bin_text(ibin));
      }  
    }  
    if(ichain==m_bin_start.back()) {
      prefix="< ";
      if(longprint) ATH_MSG_INFO(optprint<<prefix<<"Run for all bins");
    }  
    chain()[ichain]->Print(opt+prefix);
  }
}

void TFCSEnergyAndHitGAN::Streamer(TBuffer &R__b)
{
   // Stream an object of class TFCSEnergyAndHitGAN

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TFCSEnergyAndHitGAN::Class(),this);
      if(m_graph!=nullptr) {
        delete m_graph;
        m_graph=nullptr;
      }
      if(m_input) {
        std::stringstream sin;
        sin.str(*m_input);
        auto config=lwt::parse_json_graph(sin);
        m_graph=new lwt::LightweightGraph(config);
      }  
#ifndef __FastCaloSimStandAlone__ 
      //When running inside Athena, delete config to free the memory     
      if(GANfreemem()) {
        delete m_input;
        m_input=nullptr;
      }  
#endif      
   } else {
      R__b.WriteClassBuffer(TFCSEnergyAndHitGAN::Class(),this);
   }
}

void TFCSEnergyAndHitGAN::unit_test(TFCSSimulationState* simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol)
{
  if(!simulstate) {
    simulstate=new TFCSSimulationState();
#if defined(__FastCaloSimStandAlone__)
    simulstate->setRandomEngine(new CLHEP::TRandomEngine());
#else
    simulstate->setRandomEngine(new CLHEP::RanluxEngine());
#endif    
  }  
  if(!truth) {
    TFCSTruthState* t=new TFCSTruthState();
    t->SetPtEtaPhiM(20000,0.225,0,130);
    t->set_pdgid(211);
    truth=t;
  }  
  if(!extrapol) {
    TFCSExtrapolationState* e=new TFCSExtrapolationState();
    e->set_CaloSurface_eta(truth->Eta());
    e->set_IDCaloBoundary_eta(truth->Eta());
    for(int i=0;i<24;++i) {
      e->set_eta(i, TFCSExtrapolationState::SUBPOS_ENT, truth->Eta());
      e->set_eta(i, TFCSExtrapolationState::SUBPOS_EXT, truth->Eta());
      e->set_eta(i, TFCSExtrapolationState::SUBPOS_MID, truth->Eta());
      e->set_phi(i, TFCSExtrapolationState::SUBPOS_ENT, 0);
      e->set_phi(i, TFCSExtrapolationState::SUBPOS_EXT, 0);
      e->set_phi(i, TFCSExtrapolationState::SUBPOS_MID, 0);
      e->set_r(i, TFCSExtrapolationState::SUBPOS_ENT, 1500+i*10);
      e->set_r(i, TFCSExtrapolationState::SUBPOS_EXT, 1510+i*10);
      e->set_r(i, TFCSExtrapolationState::SUBPOS_MID, 1505+i*10);
      e->set_z(i, TFCSExtrapolationState::SUBPOS_ENT, 3500+i*10);
      e->set_z(i, TFCSExtrapolationState::SUBPOS_EXT, 3510+i*10);
      e->set_z(i, TFCSExtrapolationState::SUBPOS_MID, 3505+i*10);
    }
    extrapol=e;
  }  

  TFCSEnergyAndHitGAN GAN("GAN","GAN");
  GAN.setLevel(MSG::VERBOSE);
  int pid=211;
  int etaMin=20;
  int etaMax = etaMin + 5;
  GAN.initializeNetwork(pid,etaMin,"/eos/atlas/atlascerngroupdisk/proj-simul/VoxalisationOutputs/nominal/GAN_michele_normE_MaxE/input_for_service_new");
  for(int i=0;i<24;++i) {
    TFCSCenterPositionCalculation* c=new TFCSCenterPositionCalculation(Form("center%d",i),Form("center layer %d",i));
    c->set_calosample(i);
    c->setExtrapWeight(0.5);
    c->setLevel(MSG::VERBOSE);
    c->set_pdgid(pid);
    if(pid==11) c->add_pdgid(-pid);
    if(pid==211) c->add_pdgid(-pid);
    c->set_eta_min(etaMin/100.0);
    c->set_eta_max(etaMax/100.0);
    c->set_eta_nominal((etaMin+etaMax)/200.0);
    
    GAN.push_back_in_bin(c,i);
    GAN.set_nr_of_init(i,1);
  }  
  
  GAN.Print();
  
  TFile* fGAN=TFile::Open("FCSGANtest.root","recreate");
  GAN.Write();
  fGAN->ls();
  fGAN->Close();

  fGAN=TFile::Open("FCSGANtest.root");
  TFCSEnergyAndHitGAN* GAN2=(TFCSEnergyAndHitGAN*)(fGAN->Get("GAN"));
  if(GAN2) {
    GAN2->Print();
  }
  
  GAN2->setLevel(MSG::DEBUG);
  GAN2->simulate(*simulstate,truth,extrapol);
  simulstate->Print();
}

