/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamHaloGenerator/BeamHaloGeneratorAlg.h"
#include "BeamHaloGenerator/BeamHaloGenerator.h"
#include "BeamHaloGenerator/MarsHaloGenerator.h"
#include "BeamHaloGenerator/FlukaHaloGenerator.h"

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/WeightContainer.h"
#include <cmath>

//--------------------------------------------------------------------------

BeamHaloGeneratorAlg::BeamHaloGeneratorAlg(const std::string& name, ISvcLocator* svcLocator)
  : GenModule(name,svcLocator),
    m_inputTypeStr("MARS-NM"),  
    m_inputFile("bgi-b2l1.1"),
    m_interfacePlane(20850.0),
    m_enableFlip(false),
    m_flipProbability(0.),
    m_enableSampling(false),
    m_bufferFileName("buffer.bin"),
    m_generatorSettings(),
    m_doMonitoringPlots(false),
    m_tHistSvc(0),
    m_randomStream("BeamHalo"),
    m_beamHaloGenerator(0),
    m_evt() {
  declareProperty("inputType", m_inputTypeStr = "MARS-NM");
  declareProperty("inputFile",  m_inputFile = "bgi-b2l1.1");
  declareProperty("interfacePlane", m_interfacePlane = 20850.0); // (mm)
  declareProperty("enableFlip", m_enableFlip = false);
  declareProperty("flipProbability", m_flipProbability = 0.0);
  declareProperty("enableSampling", m_enableSampling = false);
  declareProperty("bufferFileName", m_bufferFileName = "BinaryBuffer.bin");
  declareProperty("generatorSettings", m_generatorSettings, "A set of cuts to be applied to generated particles.");
  declareProperty("doMonitoringPlots", m_doMonitoringPlots = false);
  declareProperty("randomStream", m_randomStream = "BeamHalo");

  for(int i=0;i<NPLOTS;i++) 
    m_validationPlots[i] = 0;

}

//--------------------------------------------------------------------------

BeamHaloGeneratorAlg::~BeamHaloGeneratorAlg() {
}

//--------------------------------------------------------------------------

StatusCode BeamHaloGeneratorAlg::genInitialize() {
  ATH_MSG_INFO("Initialising this BeamHaloGeneratorAlg: " << name());

  ATH_MSG_INFO("================ Generator Settings =================");
  ATH_MSG_INFO(" input type ------------------  " << m_inputTypeStr);
  ATH_MSG_INFO(" input file ------------------  " << m_inputFile);
  ATH_MSG_INFO(" interface plane -------------  " << m_interfacePlane << " (mm)");
  ATH_MSG_INFO(" enable flip about z=0 -------  " << (m_enableFlip ? "True" : "False"));
  ATH_MSG_INFO(" flip probability  -----------  " << m_flipProbability);
  ATH_MSG_INFO(" enable sampling -------------  " << (m_enableSampling ? "True" : "False"));
  ATH_MSG_INFO(" binary buffer file ----------  " << m_bufferFileName);
  std::vector<std::string>::iterator itr = m_generatorSettings.begin();
  std::vector<std::string>::iterator itr_end = m_generatorSettings.end();
  for(;itr!=itr_end;++itr) ATH_MSG_INFO((*itr));
  ATH_MSG_INFO(" Produce monitoring plots ----  " << (m_doMonitoringPlots ? "True" : "False"));
  ATH_MSG_INFO(" Random stream name ----------  " << m_randomStream);
  ATH_MSG_INFO("=====================================================");

  // Check the flip probability
  if(m_flipProbability <= 0) {
    ATH_MSG_INFO("Event flipping will be disabled.");
    m_enableFlip = false;
  }
  else if(m_flipProbability > 1) {
    ATH_MSG_WARNING("Flip probability " << m_flipProbability << " is out of range.  Event flipping will be disabled.");
    m_enableFlip = false;
  }
  else {
    m_enableFlip = true;
  }

  // Retrieve pointer to THistSvc if monitoring plots are requested
  /// @todo Use ServiceHandle system instead
  if(m_doMonitoringPlots) {
    StatusCode sc = service("THistSvc", m_tHistSvc);
    if (sc.isFailure() || !m_tHistSvc) {
      ATH_MSG_FATAL("Unable to locate Service THistSvc");
      return sc;
    }

    // Create log10 bins for x-axis of E plots.
    const Int_t nbins_E = 60;
    double xmin = 1e-2;
    double xmax = 3.5e3;
    double logxmin = std::log10(xmin);
    double logxmax = std::log10(xmax);
    double binwidth = (logxmax-logxmin)/nbins_E;
    Double_t xbins[nbins_E+1];
    xbins[0] = xmin;
    for (Int_t i=1;i<=nbins_E;i++) {
      xbins[i] = xmin + std::pow(10,logxmin+i*binwidth);
    }

    // Create the monitoring plots
    m_validationPlots[PRI_R] = new TH1F("primaryR",";Radius   [m];Events/[m]",100,0., 0.2);
    m_validationPlots[PRI_Z] = new TH1F("primaryZ",";z   [m];Events/[m]",1100,0., 550.); // Beam gas and TCT
    m_validationPlots[PRI_Z_TCT] = new TH1F("primaryZ_TCT",";z   [m];Events/[m]",100,144., 149.); // TCT region
    m_validationPlots[SP_R_ALL] = new TH1F("scoringPlaneR_all",";Radius   [m];Particles/[m]",240,0., 24.);
    m_validationPlots[SP_E_ALL] = new TH1F("scoringPlaneE_all",";Energy   [GeV];Particles/[GeV]",nbins_E,xbins);
    m_validationPlots[SP_PZ_ALL] = new TH1F("scoringPlanePz_all",";p_{z}   [GeV];Particles/[GeV]",350,0., 3500.);
    m_validationPlots[SP_PT_ALL] = new TH1F("scoringPlanePt_all",";p_{T}   [GeV];Particles/[GeV]",500,0., 50.);
    m_validationPlots[SP_R_PROTONS] = new TH1F("scoringPlaneR_protons",";Radius   [m];Particles/[m]",240,0., 24.);
    m_validationPlots[SP_E_PROTONS] = new TH1F("scoringPlaneE_protons",";Energy   [GeV];Particles/[GeV]",nbins_E,xbins);
    m_validationPlots[SP_PZ_PROTONS] = new TH1F("scoringPlanePz_protons",";p_{z}   [GeV];Particles/[GeV]",350,0., 3500.);
    m_validationPlots[SP_PT_PROTONS] = new TH1F("scoringPlanePt_protons",";p_{T}   [GeV];Particles/[GeV]",500,0., 50.);
    m_validationPlots[SP_R_MUONS] = new TH1F("scoringPlaneR_muons",";Radius   [m];Particles/[m]",240,0., 24.);
    m_validationPlots[SP_E_MUONS] = new TH1F("scoringPlaneE_muons",";Energy   [GeV];Particles/[GeV]",nbins_E,xbins);
    m_validationPlots[SP_PZ_MUONS] = new TH1F("scoringPlanePz_muons",";p_{z}   [GeV];Particles/[GeV]",350,0., 3500.);
    m_validationPlots[SP_PT_MUONS] = new TH1F("scoringPlanePt_muons",";p_{T}   [GeV];Particles/[GeV]",500,0., 50.);  

    if((sc = m_tHistSvc->regHist("/BeamHalo/primaryR", m_validationPlots[PRI_R])) == StatusCode::FAILURE) return sc;
    if((sc = m_tHistSvc->regHist("/BeamHalo/primaryZ", m_validationPlots[PRI_Z])) == StatusCode::FAILURE) return sc;
    if((sc = m_tHistSvc->regHist("/BeamHalo/primaryZ_TCT", m_validationPlots[PRI_Z_TCT])) == StatusCode::FAILURE) return sc;
    if((sc = m_tHistSvc->regHist("/BeamHalo/scoringPlaneR_all", m_validationPlots[SP_R_ALL])) == StatusCode::FAILURE) return sc;
    if((sc = m_tHistSvc->regHist("/BeamHalo/scoringPlaneE_all", m_validationPlots[SP_E_ALL])) == StatusCode::FAILURE) return sc; 
    if((sc = m_tHistSvc->regHist("/BeamHalo/scoringPlanePz_all", m_validationPlots[SP_PZ_ALL])) == StatusCode::FAILURE) return sc;
    if((sc = m_tHistSvc->regHist("/BeamHalo/scoringPlanePt_all", m_validationPlots[SP_PT_ALL])) == StatusCode::FAILURE) return sc; 
    if((sc = m_tHistSvc->regHist("/BeamHalo/scoringPlaneR_protons", m_validationPlots[SP_R_PROTONS])) == StatusCode::FAILURE) return sc;
    if((sc = m_tHistSvc->regHist("/BeamHalo/scoringPlaneE_protons", m_validationPlots[SP_E_PROTONS])) == StatusCode::FAILURE) return sc;     
    if((sc = m_tHistSvc->regHist("/BeamHalo/scoringPlanePz_protons", m_validationPlots[SP_PZ_PROTONS])) == StatusCode::FAILURE) return sc;
    if((sc = m_tHistSvc->regHist("/BeamHalo/scoringPlanePt_protons", m_validationPlots[SP_PT_PROTONS])) == StatusCode::FAILURE) return sc;
    if((sc = m_tHistSvc->regHist("/BeamHalo/scoringPlaneR_muons", m_validationPlots[SP_R_MUONS])) == StatusCode::FAILURE) return sc;
    if((sc = m_tHistSvc->regHist("/BeamHalo/scoringPlaneE_muons", m_validationPlots[SP_E_MUONS])) == StatusCode::FAILURE) return sc;     
    if((sc = m_tHistSvc->regHist("/BeamHalo/scoringPlanePz_muons", m_validationPlots[SP_PZ_MUONS])) == StatusCode::FAILURE) return sc;
    if((sc = m_tHistSvc->regHist("/BeamHalo/scoringPlanePt_muons", m_validationPlots[SP_PT_MUONS])) == StatusCode::FAILURE) return sc;
  }

  // Check the input type string
  if (m_inputTypeStr == "MARS-NM") {
    m_beamHaloGenerator = new MarsHaloGenerator(&particleTable(),
						&randomEngine(), 
						m_inputFile,
						m_generatorSettings);
  }
  else if (m_inputTypeStr == "FLUKA-VT") {
    m_beamHaloGenerator = new FlukaHaloGenerator(1, &particleTable(),
						 &randomEngine(), 
						 m_inputFile,
						 m_generatorSettings);
  }
  else if (m_inputTypeStr == "FLUKA-RB") {
    m_beamHaloGenerator = new FlukaHaloGenerator(0, &particleTable(),
                                                 &randomEngine(),
                                                 m_inputFile,
                                                 m_generatorSettings);
  }
  else {
    m_beamHaloGenerator = 0;
    ATH_MSG_FATAL("Input type " << m_inputTypeStr << " is not known.  Available types are: MARS-NM or FLUKA-VT");
    return StatusCode::FAILURE;
  }
  
  // Set the options using those specified to this algorithm
  m_beamHaloGenerator->setInterfacePlane(m_interfacePlane);
  m_beamHaloGenerator->setEnableFlip(m_enableFlip);
  m_beamHaloGenerator->setFlipProbability(m_flipProbability);
  m_beamHaloGenerator->setEnableSampling(m_enableSampling);
  m_beamHaloGenerator->setBufferFileName(m_bufferFileName);
  m_beamHaloGenerator->setDebugEnable(msgLvl(MSG::DEBUG));

  // Initialise the generator.
  if(m_beamHaloGenerator->genInitialize() != 0) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode BeamHaloGeneratorAlg::callGenerator() {

  // Clear the event ready for it to be filled with the next beam halo event.
  m_evt.clear();

  // Fill an event with particles from the converted ASCII input file.
  // (If the end of file has already been reached this will return a
  // non-zero value.)
  if(m_beamHaloGenerator->fillEvt(&m_evt) != 0) return StatusCode::FAILURE;

  // Fill monitoring plots if requested
  if(m_doMonitoringPlots) {
    auto weightContainer = m_evt.weights();
    if(weightContainer.size() != 5) {
      ATH_MSG_WARNING("The number of weights for this event is not equal to 5.");
      return StatusCode::SUCCESS;
    }
    double weight = weightContainer[0];
    HepMC::FourVector primaryPosition(weightContainer[1],weightContainer[2],weightContainer[3],weightContainer[4]);
    m_validationPlots[PRI_R]->Fill(primaryPosition.perp()/1000.0,weight);
    m_validationPlots[PRI_Z]->Fill(weightContainer[3]/1000.0,weight);
    m_validationPlots[PRI_Z_TCT]->Fill(std::fabs(weightContainer[3]/1000.0),weight);

    double values[4];
    int pdgId;
    for (const auto& hepmc_part: m_evt) {
      auto prodVertex = hepmc_part->production_vertex();
      if(!prodVertex) continue;
      
      // Store the values for use in the if conditions that follow
      values[0] = prodVertex->position().perp()/1000.; 
      values[1] = hepmc_part->momentum().e()/1000.;
      values[2] = hepmc_part->momentum().pz()/1000.;
      values[3] = hepmc_part->momentum().perp()/1000.;

      pdgId = hepmc_part->pdg_id(); if(pdgId<0) pdgId = -pdgId; 
      m_validationPlots[SP_R_ALL]->Fill(values[0],weight);
      m_validationPlots[SP_E_ALL]->Fill(values[1],weight);
      m_validationPlots[SP_PZ_ALL]->Fill(values[2],weight);
      m_validationPlots[SP_PT_ALL]->Fill(values[3],weight); 
      if(pdgId == 2212) {
        m_validationPlots[SP_R_PROTONS]->Fill(values[0],weight);
        m_validationPlots[SP_E_PROTONS]->Fill(values[1],weight);
        m_validationPlots[SP_PZ_PROTONS]->Fill(values[2],weight);
        m_validationPlots[SP_PT_PROTONS]->Fill(values[3],weight);
      }
      else if(pdgId == 13) {
        m_validationPlots[SP_R_MUONS]->Fill(values[0],weight);
        m_validationPlots[SP_E_MUONS]->Fill(values[1],weight);
        m_validationPlots[SP_PZ_MUONS]->Fill(values[2],weight);
        m_validationPlots[SP_PT_MUONS]->Fill(values[3],weight); 
      } 
    }
  } 

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode BeamHaloGeneratorAlg::fillEvt(HepMC::GenEvent* event) {
  *event = m_evt;
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode BeamHaloGeneratorAlg::genFinalize() {
  if(m_beamHaloGenerator->genFinalize() != 0) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
