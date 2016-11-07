/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CPAnalysisExamples includes
#include "Test__EgammaCalibrationAndSmearingTool.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODCore/ShallowCopy.h"

#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/tools/IOStats.h"

#include "xAODBase/IParticleHelpers.h"

namespace Test {

EgammaCalibrationAndSmearingTool::EgammaCalibrationAndSmearingTool( const std::string& name, ISvcLocator* pSvcLocator ) : 
   AthAnalysisAlgorithm( name, pSvcLocator ),
   myTool("CP::EgammaCalibrationAndSmearingTool/myTool") {

}


EgammaCalibrationAndSmearingTool::~EgammaCalibrationAndSmearingTool() {}


StatusCode EgammaCalibrationAndSmearingTool::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");
  //
  //This is called once, before the start of the event loop
  //Retrieves of tools you have configured in the joboptions go here
  //

   CHECK( AthAnalysisHelper::setProperty( myTool, "ESModel", "es2015PRE" ) );
   CHECK( myTool.retrieve() );

   m_el_pt = new TH1D("el_pt","Electron Calibrated Pt [GeV]",100,0,100);
   m_el_pt_new_vs_old = new TH2D("el_pt_new_vs_old","Electron Calibrated Vs Uncalibrated Pt [GeV]",100,0,100,100,0,100);
   ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
   CHECK( histSvc->regHist("/MYSTREAM/el_pt_new_vs_old",m_el_pt_new_vs_old) );
   CHECK( histSvc->regHist("/MYSTREAM/el_pt",m_el_pt) );

  return StatusCode::SUCCESS;
}

StatusCode EgammaCalibrationAndSmearingTool::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //

  //print the smart slimming xAOD statistics 
  //this will only be useful when running with the 'ReadAthenaxAOD' or 'ReadAthenaxAODHybrid' modes
  xAOD::ReadStats& stats = xAOD::IOStats::instance().stats();
  stats.Print("SmartSlimming");



  return StatusCode::SUCCESS;
}

StatusCode EgammaCalibrationAndSmearingTool::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed

   i++;


  //
  //Your main analysis code goes here
  //If you will use this algorithm to perform event skimming, you
  //should ensure the setFilterPassed method is called
  //If never called, the algorithm is assumed to have 'passed' by default
  //

   //get the electrons
   const xAOD::ElectronContainer* xaod_els = 0;
   if( evtStore()->retrieve( xaod_els , "Electrons" ).isFailure() ) {
      ATH_MSG_ERROR("Could not retrieve electrons"); return StatusCode::FAILURE;
   }

   //create a shallow copy and calibrate 
   auto calibratedElectrons = xAOD::shallowCopyContainer( *xaod_els );
   int j=0;
   for(auto el : *calibratedElectrons.first) {
     myTool->applyCorrection(*el).setChecked();
     m_el_pt_new_vs_old->Fill(xaod_els->at(j++)->pt()/1000.,el->pt()/1000.); //record to histogram
     m_el_pt->Fill(el->pt()/1000.);
   }

   //for first 10 events we will print out the calibrated and uncalibrated Pt for comparison
   if(i<10) {
      for(uint j=0;j<xaod_els->size();j++) {
         ATH_MSG_INFO("Uncalibrated Pt = " << xaod_els->at(j)->pt() << " , Calibrated Pt = " << calibratedElectrons.first->at(j)->pt());
      }
   }

   //cleanup the shallow copy 
   delete calibratedElectrons.first;
   delete calibratedElectrons.second; //the auxstore 

  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode EgammaCalibrationAndSmearingTool::beginInputFile() { 
  //
  //This method is called at the start of each input file, even if
  //the input file contains no events. Accumulate metadata information here
  //

  //example of retrieval of CutBookkeepers: (remember you will need to include the necessary header files and use statements in requirements file)
  // const xAOD::CutBookkeeperContainer* bks = 0;
  // CHECK( inputMetaStore()->retrieve(bks, "CutBookkeepers") );

  //example of IOVMetaData retrieval (see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_access_file_metadata_in_C)
  //float beamEnergy(0); CHECK( retrieveMetadata("/TagInfo","beam_energy",beamEnergy) );
  //std::vector<float> bunchPattern; CHECK( retrieveMetadata("/Digitiation/Parameters","BeamIntensityPattern",bunchPattern) );



  return StatusCode::SUCCESS;
}

} //> end namespace Test
