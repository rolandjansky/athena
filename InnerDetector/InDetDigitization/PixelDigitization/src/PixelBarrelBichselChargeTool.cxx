/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelBarrelBichselChargeTool.cxx
//   Implementation file for class PixelBarrelBichselChargeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef __BICHSEL_TIMER__
// #define __BICHSEL_TIMER__
#endif

#include "PixelBarrelBichselChargeTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetIdentifier/PixelID.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "SiPropertiesSvc/SiliconProperties.h"
//#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"

#include "BichselSimTool.h"
#include "TLorentzVector.h"

#include <chrono>

using namespace InDetDD;

// Constructor with parameters:
PixelBarrelBichselChargeTool::PixelBarrelBichselChargeTool(const std::string& type, const std::string& name,const IInterface* parent):
	SubChargesTool(type,name,parent),
	m_numberOfSteps(50),
	m_numberOfCharges(10),
	m_diffusionConstant(.007),
  m_doBichsel(false),
  //m_doBichselMomentumCut(1000.),     // need to change to beta-gamma cut
  m_doBichselBetaGammaCut(0.1),        // replace momentum cut
  m_doDeltaRay(false),                 // need validation
  m_doPU(true),                        
  m_BichselSimTool("BichselSimTool"),
  m_OutputFileName("EnergyDeposition.root"),
  m_doHITPlots(false)
{ 
	declareProperty("numberOfSteps",m_numberOfSteps,"Geant4:number of steps for PixelBarrel");
	declareProperty("numberOfCharges",m_numberOfCharges,"Geant4:number of charges for PixelBarrel");
	declareProperty("diffusionConstant",m_diffusionConstant,"Geant4:Diffusion Constant for PixelBarrel");
  declareProperty("doBichsel", m_doBichsel, "re-do charge deposition following Bichsel model");
  //declareProperty("doBichselMomentumCut", m_doBichselMomentumCut, "minimum MOMENTUM for particle to be re-simulated through Bichsel Model");
  declareProperty("doBichselBetaGammaCut", m_doBichselBetaGammaCut, "minimum beta-gamma for particle to be re-simulated through Bichsel Model");
  declareProperty("doDeltaRay", m_doDeltaRay, "whether we simulate delta-ray using Bichsel model");
  declareProperty("doPU", m_doPU, "Whether we apply Bichsel model on PU");
  declareProperty("BichselSimTool", m_BichselSimTool, "tool that implements Bichsel model");
  declareProperty("OutputFileName", m_OutputFileName, "name of output file for customized study during digitization");
  declareProperty("doHITPlots", m_doHITPlots, "whether we make hit plots");
}

class DetCondCFloat;

// Destructor:
PixelBarrelBichselChargeTool::~PixelBarrelBichselChargeTool()
{
}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode PixelBarrelBichselChargeTool::initialize() {
  StatusCode sc = SubChargesTool::initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelBarrelBichselChargeTool::initialize() failed");
    return sc ;
  }

  ATH_MSG_INFO("You are using PixelBarrelBichselChargeTool, not PixelBarrelChargeTool");

  if(m_doBichsel){
    ATH_MSG_INFO("Bichsel Digitization is turned ON in PixelBarrelBichselChargeTool!");

    sc = m_BichselSimTool.retrieve();
    if(sc.isFailure()){
      ATH_MSG_FATAL("Fail to retrieve BichselSimTool in PixelBarrelBichselChargeTool!");
      return sc;
    }
  }
  else{
    ATH_MSG_INFO("Bichsel Digitization is turned OFF in PixelBarrelBichselChargeTool!");
  }

  m_doDeltaRay = (m_doBichsel && m_doDeltaRay);    // if we don't do Bichsel model, no re-simulation on delta-ray at all!

  if(m_doHITPlots){
    f_output = new TFile(m_OutputFileName.data(), "RECREATE");

    ATH_MSG_INFO("Initializing diagnosing root file ...");

    h_Length = new TH1D("Length", "Length", 500, 0, 1000);
    h_hitTime = new TH1D("HitTime", "HitTime", 2000, -100, 100);
    h_EnergyDepositionNominal = new TH1D("EnergyDepositionNominal", "EnergyDepositionNominal", 500, 0, 1000);
    h_EnergyDepositionBichsel = new TH1D("EnergyDepositionBichsel", "EnergyDepositionBichsel", 500, 0, 1000);
    h_EnergyDepositionDeltaRay = new TH1D("EnergyDepositionDeltaRay", "EnergyDepositionDeltaRay", 500, 0, 1000);
    h_EnergyDepositionDeltaRay_Bichsel = new TH1D("EnergyDepositionDeltaRay_Bichsel", "EnergyDepositionDeltaRay_Bichsel", 500, 0, 1000);
    h_EnergyDepositionDeltaRay_Nominal = new TH1D("EnergyDepositionDeltaRay_Nominal", "EnergyDepositionDeltaRay_Nominal", 500, 0, 1000);

    h_largestEnergyDeposition = new TH1D("largestEnergyDeposition", "largestEnergyDeposition", 100, 0, 1);
    h_twolargestEnergyDeposition = new TH1D("twolargestEnergyDeposition", "twolargestEnergyDeposition", 100, 0, 1);

    h_isRealBichsel_DeltaRay = new TH1D("isRealBichsel_DeltaRay", "isRealBichsel_DeltaRay", 2, -0.5, 1.5);
    h_isRealBichsel_Primary = new TH1D("isRealBichsel_Primary", "isRealBichsel_Primary", 2, -0.5, 1.5);

    h_dEdxBetaGammaMap_Bichsel.clear();
    h_dEdxBetaGammaMap_Bichsel["proton"] = new TH2D("BetaGamma_dEdx_Bichsel_proton", "BetaGamma_dEdx_Bichsel_proton", 600, -1, 5, 600, 0, 3000);
    h_dEdxBetaGammaMap_Bichsel["pion"] = new TH2D("BetaGamma_dEdx_Bichsel_pion", "BetaGamma_dEdx_Bichsel_pion", 600, -1, 5, 600, 0, 3000);
    h_dEdxBetaGammaMap_Bichsel["electron"] = new TH2D("BetaGamma_dEdx_Bichsel_electron", "BetaGamma_dEdx_Bichsel_electron", 600, -1, 5, 600, 0, 3000);
    h_dEdxBetaGammaMap_Bichsel["kaon"] = new TH2D("BetaGamma_dEdx_Bichsel_kaon", "BetaGamma_dEdx_Bichsel_kaon", 600, -1, 5, 600, 0, 3000);
    h_dEdxBetaGammaMap_Bichsel["muon"] = new TH2D("BetaGamma_dEdx_Bichsel_muon", "BetaGamma_dEdx_Bichsel_muon", 600, -1, 5, 600, 0, 3000);

    h_dEdxBetaGammaMap_Nominal.clear();
    h_dEdxBetaGammaMap_Nominal["proton"] = new TH2D("BetaGamma_dEdx_Nominal_proton", "BetaGamma_dEdx_Nominal_proton", 600, -1, 5, 600, 0, 3000);
    h_dEdxBetaGammaMap_Nominal["pion"] = new TH2D("BetaGamma_dEdx_Nominal_pion", "BetaGamma_dEdx_Nominal_pion", 600, -1, 5, 600, 0, 3000);
    h_dEdxBetaGammaMap_Nominal["electron"] = new TH2D("BetaGamma_dEdx_Nominal_electron", "BetaGamma_dEdx_Nominal_electron", 600, -1, 5, 600, 0, 3000);
    h_dEdxBetaGammaMap_Nominal["kaon"] = new TH2D("BetaGamma_dEdx_Nominal_kaon", "BetaGamma_dEdx_Nominal_kaon", 600, -1, 5, 600, 0, 3000);
    h_dEdxBetaGammaMap_Nominal["muon"] = new TH2D("BetaGamma_dEdx_Nominal_muon", "BetaGamma_dEdx_Nominal_muon", 600, -1, 5, 600, 0, 3000);

    h_dEdxMomentumMap_Bichsel.clear();
    h_dEdxMomentumMap_Bichsel["proton"] = new TH2D("Momentum_dEdx_Bichsel_proton", "Momentum_dEdx_Bichsel_proton", 400, -1, 3, 600, 0, 3000);
    h_dEdxMomentumMap_Bichsel["pion"] = new TH2D("Momentum_dEdx_Bichsel_pion", "Momentum_dEdx_Bichsel_pion", 400, -1, 3, 600, 0, 3000);
    h_dEdxMomentumMap_Bichsel["electron"] = new TH2D("Momentum_dEdx_Bichsel_electron", "Momentum_dEdx_Bichsel_electron", 400, -1, 3, 600, 0, 3000);
    h_dEdxMomentumMap_Bichsel["kaon"] = new TH2D("Momentum_dEdx_Bichsel_kaon", "Momentum_dEdx_Bichsel_kaon", 400, -1, 3, 600, 0, 3000);
    h_dEdxMomentumMap_Bichsel["muon"] = new TH2D("Momentum_dEdx_Bichsel_muon", "Momentum_dEdx_Bichsel_muon", 400, -1, 3, 600, 0, 3000);

    h_dEdxMomentumMap_Nominal.clear();
    h_dEdxMomentumMap_Nominal["proton"] = new TH2D("Momentum_dEdx_Nominal_proton", "Momentum_dEdx_Nominal_proton", 400, -1, 3, 600, 0, 3000);
    h_dEdxMomentumMap_Nominal["pion"] = new TH2D("Momentum_dEdx_Nominal_pion", "Momentum_dEdx_Nominal_pion", 400, -1, 3, 600, 0, 3000);
    h_dEdxMomentumMap_Nominal["electron"] = new TH2D("Momentum_dEdx_Nominal_electron", "Momentum_dEdx_Nominal_electron", 400, -1, 3, 600, 0, 3000);
    h_dEdxMomentumMap_Nominal["kaon"] = new TH2D("Momentum_dEdx_Nominal_kaon", "Momentum_dEdx_Nominal_kaon", 400, -1, 3, 600, 0, 3000);
    h_dEdxMomentumMap_Nominal["muon"] = new TH2D("Momentum_dEdx_Nominal_muon", "Momentum_dEdx_Nominal_muon", 400, -1, 3, 600, 0, 3000);

    // timers
    h_timer_execute = new TH1D("timer_execute", "timer_execute", 20000, 0, 20000);
    h_timer_BichselSim = new TH1D("timer_BichselSim", "timer_BichselSim", 20000, 0, 20000);
    h_timer_DigiLayer = new TH1D("timer_DigiLayer", "timer_DigiLayer", 20000, 0, 20000);
    h_timer_diffuse = new TH1D("timer_diffuse", "timer_diffuse", 20000, 0, 20000);

    // hit categories
    std::vector<std::string> hitCategoryList = {
      "Total",
      "DoBichsel",
      "NoBichsel",
      "HasMCParticleLink",
      "UnknwonParticleType",
      "BelowBetaGammCut",
      "NoMCParticleLink",
      "HS-doBichsel",
      "InTimePU-doBichsel",
      "HS",
      "InTimePU",
    };
    int nbins = hitCategoryList.size();
    h_hitCategory = new TH1D("hit_category", "hit_category", nbins, 0, nbins);
    h_hitCategory->Sumw2();
    for(unsigned index = 0; index < hitCategoryList.size(); index++){
      h_hitCategory->GetXaxis()->SetBinLabel(index+1, hitCategoryList[index].data());
    }

  }

  ATH_MSG_DEBUG ( "PixelBarrelBichselChargeTool::initialize()");
  return sc ;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode PixelBarrelBichselChargeTool::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelBarrelBichselChargeTool::finalize() failed");
    return sc ;
  }

  if(m_doHITPlots){
    f_output->Write();
  }

  ATH_MSG_DEBUG ( "PixelBarrelBichselChargeTool::finalize()");
  return sc ;
}

//----------------------------------------------------------------------
// charge
//----------------------------------------------------------------------
StatusCode PixelBarrelBichselChargeTool::charge(const TimedHitPtr<SiHit> &phit,
		  SiChargedDiodeCollection& chargedDiodes,
		  const InDetDD::SiDetectorElement &Module)
{
#ifdef __BICHSEL_TIMER__
  auto timer_execute_start = std::chrono::high_resolution_clock::now();
#endif

  ATH_MSG_DEBUG("Applying PixelBarrel charge processor");
  const EBC_EVCOLL evColl = (m_needsMcEventCollHelper) ? getMcEventCollectionHMPLEnumFromTimedHitPtr(phit) : EBC_MAINEVCOLL;
  const bool isEventIndexIsPosition = (phit.eventId()==0);
  HepMcParticleLink McLink(phit->trackNumber(), phit.eventId(), evColl, isEventIndexIsPosition);
  const HepMC::GenParticle* genPart= McLink.cptr(); 
  bool delta_hit = true;
  if (genPart) delta_hit = false;
  double sensorThickness = Module.design().thickness();
  const InDet::SiliconProperties & siProperties = m_siPropertiesSvc->getSiProperties(Module.identifyHash());
  electronHolePairsPerEnergy = siProperties.electronHolePairsPerEnergy();

  double stepsize = sensorThickness/m_numberOfSteps;
  double tanLorentz = Module.getTanLorentzAnglePhi();
  const CLHEP::Hep3Vector pos=phit->localStartPosition();
  const CLHEP::Hep3Vector cs=phit->localEndPosition();
  
  double xEta=pos[SiHit::xEta];
  double xPhi=pos[SiHit::xPhi];
  const double xDep=pos[SiHit::xDep];
  
  double xEtaf = cs[SiHit::xEta];
  double xPhif = cs[SiHit::xPhi];
  const double xDepf = cs[SiHit::xDep];

  if (!m_disableDistortions && !delta_hit) simulateBow(&Module,xPhi,xEta,xDep,xPhif,xEtaf,xDepf);
  
  double cEta=xEtaf-xEta;
  double cPhi=xPhif-xPhi;
  const double cDep=xDepf-xDep;

  double length=sqrt(cEta*cEta+cPhi*cPhi+cDep*cDep);
  const int nsteps=int(length/stepsize)+1; 
  const int ncharges=this->m_numberOfCharges*this->m_numberOfSteps/nsteps+1; 
 
  //double stepEta = cEta / nsteps;
  //double stepPhi = cPhi / nsteps;
  //double stepDep = cDep / nsteps; 

  //double e1=phit->energyLoss()/static_cast<double>(nsteps*ncharges);

  double coLorentz=sqrt(1+pow(tanLorentz,2));

  //////////////////////////////////////////////////////
  // ***                For Bichsel               *** //
  //////////////////////////////////////////////////////

  double iTotalLength = length*1000.;   // mm -> micrometer

  // ultimate feed in to the diffusion (to surface) part
  std::vector<std::pair<double,double> > trfHitRecord; trfHitRecord.clear();

  // -1 ParticleType means we are unable to run Bichel simulation for this case
  int ParticleType = -1;
  if(m_doBichsel){

    ParticleType = delta_hit ? (m_doDeltaRay ? 4 : -1) : m_BichselSimTool->trfPDG(genPart->pdg_id()); 

    if(ParticleType != -1){ // this is a protection in case delta_hit == true (a delta ray)
      TLorentzVector genPart_4V;

      if(genPart){ // non-delta-ray
        genPart_4V.SetPtEtaPhiM(genPart->momentum().perp(), genPart->momentum().eta(), genPart->momentum().phi(), genPart->momentum().m());
        double iBetaGamma = genPart_4V.Beta() * genPart_4V.Gamma();
        //if(genPart_4V.P()/CLHEP::MeV < m_doBichselMomentumCut) ParticleType = -1;
        if(iBetaGamma < m_doBichselBetaGammaCut) ParticleType = -1;
      }
      else{ // delta-ray. 
        double k = phit->energyLoss()/CLHEP::MeV;     // unit of MeV
        double m = 0.511;                             // unit of MeV
        double iBetaGamma = TMath::Sqrt(k*(2*m+k))/m;

        if(iBetaGamma < m_doBichselBetaGammaCut) ParticleType = -1;
      }

      // In-time PU
      if(!m_doPU){
        if(phit.eventId() != 0) ParticleType = -1;
      }

      // Out-of-time PU
      // We don't cut on the out-of-time PU, since studies show that the fraction is too small

    }
  } 

  bool m_isRealBichsel = false;
#ifdef __BICHSEL_TIMER__
  auto timer_DigiLayer_start = std::chrono::high_resolution_clock::now();
#endif
  if(ParticleType != -1){ // yes, good to go with Bichsel
    // I don't know why genPart->momentum() goes crazy ... 
    TLorentzVector genPart_4V;
    double iBetaGamma;

    if(genPart){
      genPart_4V.SetPtEtaPhiM(genPart->momentum().perp(), genPart->momentum().eta(), genPart->momentum().phi(), genPart->momentum().m());
      iBetaGamma = genPart_4V.Beta() * genPart_4V.Gamma();
    }
    else{
      double k = phit->energyLoss()/CLHEP::MeV;     // unit of MeV
      double m = 0.511;                             // unit of MeV
      iBetaGamma = TMath::Sqrt(k*(2*m+k))/m;
    }

    int iParticleType = ParticleType;
    //double iTotalLength = length*1000.;   // mm -> micrometer

    // begin simulation
#ifdef __BICHSEL_TIMER__
    auto timer_BichselSim_start = std::chrono::high_resolution_clock::now();
#endif
    std::vector<std::pair<double,double> > rawHitRecord = m_BichselSimTool->BichselSim(iBetaGamma, iParticleType, iTotalLength, genPart ? (genPart->momentum().e()/CLHEP::MeV) : (phit->energyLoss()/CLHEP::MeV) );
#ifdef __BICHSEL_TIMER__
    auto timer_BichselSim_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> timer_BichselSim_elapse = timer_BichselSim_end - timer_BichselSim_start;
    if(m_doHITPlots){
     h_timer_BichselSim->Fill( timer_BichselSim_elapse.count() );
    }
#endif

    // check if returned simulation result makes sense
    if(rawHitRecord.size() == 0){ // deal with rawHitRecord==0 specifically -- no energy deposition
      std::pair<double,double> specialHit;
      specialHit.first = 0.; specialHit.second = 0.;
      trfHitRecord.push_back(specialHit);      
    }
    else if( (rawHitRecord.size() == 1) && (rawHitRecord[0].first == -1.) && (rawHitRecord[0].second == -1.) ){ // special flag returned from BichselSim meaning it FAILs
      for(int istep = 0; istep < nsteps; istep++){ // do the same thing as old digitization method
        std::pair<double,double> specialHit;
        specialHit.first = 1.0*iTotalLength/nsteps * (istep + 0.5); specialHit.second = phit->energyLoss()*1.E+6/nsteps;
        trfHitRecord.push_back(specialHit);
      }
    }
    else{ // cluster thousands hits to ~20 groups
      trfHitRecord = m_BichselSimTool->ClusterHits(rawHitRecord, nsteps);
      m_isRealBichsel = true;
    }
  }
  else{  // same as old digitization method
    //double iTotalLength = length*1000.;   // mm -> micrometer
    for(int istep = 0; istep < nsteps; istep++){ // do the same thing as old digitization method
        std::pair<double,double> specialHit;
        specialHit.first = 1.0*iTotalLength/nsteps * (istep + 0.5); specialHit.second = phit->energyLoss()*1.E+6/nsteps;
        trfHitRecord.push_back(specialHit);
    }
  }

#ifdef __BICHSEL_TIMER__
  auto timer_DigiLayer_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::micro> timer_DigiLayer_elapse = timer_DigiLayer_end - timer_DigiLayer_start;
  if(m_doHITPlots){
    h_timer_DigiLayer->Fill( timer_DigiLayer_elapse.count() );
  }
#endif

  // *** Finsih Bichsel *** //

  // *** Fill histograms *** //

  if(m_doHITPlots){
    // hit category
    h_hitCategory->Fill("Total", 1);
    
    if(phit.eventId() == 0) h_hitCategory->Fill("HS", 1);
    else                    h_hitCategory->Fill("InTimePU", 1);

    if(ParticleType != -1){ // do bichsel model eventually
      h_hitCategory->Fill("DoBichsel", 1);

      if(phit.eventId() == 0) h_hitCategory->Fill("HS-doBichsel", 1);
      else                    h_hitCategory->Fill("InTimePU-doBichsel", 1);
    }
    else{
      h_hitCategory->Fill("NoBichsel", 1);
    }

    if(!delta_hit){ // not a delta-ray
      h_hitCategory->Fill("HasMCParticleLink", 1);

      if(m_BichselSimTool->trfPDG(genPart->pdg_id()) == -1) // unknown particle type
        h_hitCategory->Fill("UnknwonParticleType", 1);
      else if(ParticleType == -1) // below beta-gamma threshold
        h_hitCategory->Fill("BelowBetaGammCut", 1);
    }
    else{
      h_hitCategory->Fill("NoMCParticleLink", 1);
    }


    if( (ParticleType != -1) && (!delta_hit) ){  // only primary particles that goes through Bichsel Model
      TLorentzVector genPart_4V;
      genPart_4V.SetPtEtaPhiM(genPart->momentum().perp(), genPart->momentum().eta(), genPart->momentum().phi(), genPart->momentum().m());
      double iBetaGamma = genPart_4V.Beta() * genPart_4V.Gamma();

      float sumEnergyLoss = 0.;
      for(unsigned int index = 0; index < trfHitRecord.size(); index++){
        sumEnergyLoss += (trfHitRecord[index].second / 1000.);    // eV -> keV
      }

      h_Length->Fill(iTotalLength);    // micrometer
      h_hitTime->Fill(phit->meanTime());  // hit time around current event Bunch-Xing time in ns

      if(iTotalLength > 280.){
        double dEdx_Nominal = phit->energyLoss() * 1000. / (iTotalLength/1000.);
        h_EnergyDepositionNominal->Fill(dEdx_Nominal);   // keV per mm

        double dEdx_Bichsel = sumEnergyLoss / (iTotalLength/1000.);
        h_EnergyDepositionBichsel->Fill(dEdx_Bichsel);  // keV per mm

        h_isRealBichsel_Primary->Fill(m_isRealBichsel);

        // show the distribution
        TH1D* util = new TH1D("util", "util", 20, 0, 1);   
        for(unsigned int index = 0; index < trfHitRecord.size(); index++){
          util->Fill(trfHitRecord[index].first/iTotalLength, (trfHitRecord[index].second / 1000.)/sumEnergyLoss);
        }

        //h_largestEnergyDeposition->Fill(util->GetBinContent(util->GetMaximumBin()));

        std::vector<double> util_vector;
        for(int index = 1; index <= util->GetNbinsX(); index++){
          util_vector.push_back(util->GetBinContent(index));
        }

        std::sort(util_vector.begin(), util_vector.end());
        std::reverse(util_vector.begin(), util_vector.end());

        h_largestEnergyDeposition->Fill(util_vector[0]);
        h_twolargestEnergyDeposition->Fill(util_vector[0] + util_vector[1]);

        // std::cout << "-------------" << std::endl;
        // for(unsigned int index = 1; index <= util->GetNbinsX(); index++){
        //   std::cout << util->GetBinContent(index) << " ";
        // }
        // std::cout << std::endl;

        delete util;

        if(ParticleType == 1){
          h_dEdxBetaGammaMap_Bichsel["proton"]->Fill(TMath::Log10(iBetaGamma), dEdx_Bichsel);
          h_dEdxBetaGammaMap_Nominal["proton"]->Fill(TMath::Log10(iBetaGamma), dEdx_Nominal);
          h_dEdxMomentumMap_Bichsel["proton"]->Fill(TMath::Log10(genPart_4V.P()/1000.), dEdx_Bichsel);
          h_dEdxMomentumMap_Nominal["proton"]->Fill(TMath::Log10(genPart_4V.P()/1000.), dEdx_Nominal);
        }
        else if(ParticleType == 2){
          h_dEdxBetaGammaMap_Bichsel["pion"]->Fill(TMath::Log10(iBetaGamma), dEdx_Bichsel);
          h_dEdxBetaGammaMap_Nominal["pion"]->Fill(TMath::Log10(iBetaGamma), dEdx_Nominal);
          h_dEdxMomentumMap_Bichsel["pion"]->Fill(TMath::Log10(genPart_4V.P()/1000.), dEdx_Bichsel);
          h_dEdxMomentumMap_Nominal["pion"]->Fill(TMath::Log10(genPart_4V.P()/1000.), dEdx_Nominal);
        }
        else if(ParticleType == 4){
          h_dEdxBetaGammaMap_Bichsel["electron"]->Fill(TMath::Log10(iBetaGamma), dEdx_Bichsel);
          h_dEdxBetaGammaMap_Nominal["electron"]->Fill(TMath::Log10(iBetaGamma), dEdx_Nominal);
          h_dEdxMomentumMap_Bichsel["electron"]->Fill(TMath::Log10(genPart_4V.P()/1000.), dEdx_Bichsel);
          h_dEdxMomentumMap_Nominal["electron"]->Fill(TMath::Log10(genPart_4V.P()/1000.), dEdx_Nominal);
        }
        else if(ParticleType == 5){
          h_dEdxBetaGammaMap_Bichsel["kaon"]->Fill(TMath::Log10(iBetaGamma), dEdx_Bichsel);
          h_dEdxBetaGammaMap_Nominal["kaon"]->Fill(TMath::Log10(iBetaGamma), dEdx_Nominal);
          h_dEdxMomentumMap_Bichsel["kaon"]->Fill(TMath::Log10(genPart_4V.P()/1000.), dEdx_Bichsel);
          h_dEdxMomentumMap_Nominal["kaon"]->Fill(TMath::Log10(genPart_4V.P()/1000.), dEdx_Nominal);
        }
        else if(ParticleType == 6){
          h_dEdxBetaGammaMap_Bichsel["muon"]->Fill(TMath::Log10(iBetaGamma), dEdx_Bichsel);
          h_dEdxBetaGammaMap_Nominal["muon"]->Fill(TMath::Log10(iBetaGamma), dEdx_Nominal);
          h_dEdxMomentumMap_Bichsel["muon"]->Fill(TMath::Log10(genPart_4V.P()/1000.), dEdx_Bichsel);
          h_dEdxMomentumMap_Nominal["muon"]->Fill(TMath::Log10(genPart_4V.P()/1000.), dEdx_Nominal);
        }
      }
    }

    if(delta_hit){  // only secondary particles
      if(iTotalLength > 280.){
        h_EnergyDepositionDeltaRay->Fill(phit->energyLoss() * 1000);   // unit of keV

        if(ParticleType != -1){ // only secondary particles that go through Bichsel Model
          float sumEnergyLoss = 0.;
          for(unsigned int index = 0; index < trfHitRecord.size(); index++){
            sumEnergyLoss += (trfHitRecord[index].second / 1000.);    // eV -> keV
          }

          double dEdx_Nominal = phit->energyLoss() * 1000. / (iTotalLength/1000.);
          double dEdx_Bichsel = sumEnergyLoss / (iTotalLength/1000.);

          h_isRealBichsel_DeltaRay->Fill(m_isRealBichsel);

          h_EnergyDepositionDeltaRay_Nominal->Fill(dEdx_Nominal);
          h_EnergyDepositionDeltaRay_Bichsel->Fill(dEdx_Bichsel);
        }
      }
    }

  }

  // *** Now diffuse charges to surface *** //
#ifdef __BICHSEL_TIMER__
  auto timer_diffuse_start = std::chrono::high_resolution_clock::now();
#endif

  for(unsigned int istep = 0; istep < trfHitRecord.size(); istep++){
    std::pair<double,double> iHitRecord = trfHitRecord[istep];

    // double xEta1 = xEta +  stepEta * (istep + 0.5);
    // double xPhi1 = xPhi +  stepPhi * (istep + 0.5);
    // double depD  = xDep +  stepDep * (istep + 0.5);
    double xEta1 = xEta + 1.0*iHitRecord.first/iTotalLength*cEta;
    double xPhi1 = xPhi + 1.0*iHitRecord.first/iTotalLength*cPhi;
    double depD  = xDep + 1.0*iHitRecord.first/iTotalLength*cDep;

    // Distance between charge and readout side.  p_design->readoutSide() is
    // +1 if readout side is in +ve depth axis direction and visa-versa.
    double spess = 0.5 * sensorThickness - Module.design().readoutSide() * depD;
    if (spess<0) spess=0;
      
    for(int i=0 ; i<ncharges ; i++) {
  
      // diffusion sigma
      double rdif=this->m_diffusionConstant*sqrt(spess*coLorentz/0.3);
      
      // position at the surface
      double xPhiD=xPhi1+spess*tanLorentz+rdif*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
      double xEtaD=xEta1+rdif*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

      // Get the charge position in Reconstruction local coordinates.
      SiLocalPosition chargePos = Module.hitLocalToLocal(xEtaD, xPhiD);
      
      // The parametrization of the sensor efficiency (if needed)
      // double ed=e1*this->electronHolePairsPerEnergy;
      double ed=(1.0*iHitRecord.second/1.E+6/ncharges)*this->electronHolePairsPerEnergy;
      
      //The following lines are adapted from SiDigitization's Inserter class
      SiSurfaceCharge scharge(chargePos,SiCharge(ed,hitTime(phit),SiCharge::track,McLink));
    
      SiCellId diode = Module.cellIdOfPosition(scharge.position());
       
	    SiCharge charge = scharge.charge();

	    if (diode.isValid()) {
        chargedDiodes.add(diode,charge);
      }
    }									
  }

#ifdef __BICHSEL_TIMER__
  auto timer_diffuse_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::micro> timer_diffuse_elapse = timer_diffuse_end - timer_diffuse_start;
  if(m_doHITPlots){
    h_timer_diffuse->Fill( timer_diffuse_elapse.count() );
  }
#endif

#ifdef __BICHSEL_TIMER__
  auto timer_execute_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::micro> timer_execute_elapse = timer_execute_end - timer_execute_start;
  if(m_doHITPlots){
    h_timer_execute->Fill( timer_execute_elapse.count() );
  }
#endif

	return StatusCode::SUCCESS;
}

void PixelBarrelBichselChargeTool::simulateBow(const InDetDD::SiDetectorElement * element,
                                        double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const {

  // The corrections are assumed to be in the reconstruction local frame, so
  // we must convertfrom the hit local frame to the  reconstruction local frame.
  // In fact the frames are the same for the pixel barrel so these gymnastics are not
  // really needed but its safer to do it properly.

  // If tool is NONE we apply no correction.
  if (m_pixDistoTool.empty()) return;
  Amg::Vector3D dir(element->hitPhiDirection() * (xf - xi),
                    element->hitEtaDirection() * (yf - yi),
                    element->hitDepthDirection() * (zf - zi));

  Amg::Vector2D locposi = element->hitLocalToLocal(yi, xi);
  Amg::Vector2D locposf = element->hitLocalToLocal(yf, xf);
   
  Amg::Vector2D newLocposi = m_pixDistoTool->correctSimulation(element->identify(), locposi, dir);
  Amg::Vector2D newLocposf = m_pixDistoTool->correctSimulation(element->identify(), locposf, dir);
 
  // Extract new coordinates and convert back to hit frame.
  xi = newLocposi[Trk::x] * element->hitPhiDirection();
  yi = newLocposi[Trk::y] * element->hitEtaDirection();

  xf = newLocposf[Trk::x] * element->hitPhiDirection();
  yf = newLocposf[Trk::y] * element->hitEtaDirection();
 
}

