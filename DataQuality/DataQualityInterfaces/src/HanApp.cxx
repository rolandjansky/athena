/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: HanApp.cxx,v 1.13 2008-12-07 03:25:37 ponyisi Exp $
// **********************************************************************

#include "DataQualityInterfaces/HanApp.h"

#include <TFile.h>
#include <TROOT.h>

#include "dqm_core/LibraryManager.h"
#include "dqm_core/InputRootFile.h"
#include "DataQualityInterfaces/HanConfig.h"
#include "DataQualityInterfaces/HanOutput.h"
#include "DataQualityInterfaces/HanInputRootFile.h"
#include "DataQualityInterfaces/HanUtils.h"

#include <TCanvas.h>

//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqi::HanApp)
///\endcond

  /*
void HanApp_dummyMethod()
{
  TCanvas* dummyCanvas = new TCanvas("dummyCanvas","dummyCanvas", 450, 450 );
  dummyCanvas->cd();
}
  */

namespace {

  void RecursivelyDeleteCollection(TCollection* seq) {
    TIter it(seq);
    while (TObject* obj = it()) {
      TCollection* coll = dynamic_cast<TCollection*>(obj);
      if (coll) {
	RecursivelyDeleteCollection(coll);
      }
    }
    seq->Delete();
  }

} // end unnamed namespace


namespace dqi {

// *********************************************************************
// Public Methods
// *********************************************************************

HanApp::
HanApp()
  : m_outputName("")
{
}


HanApp::
HanApp( std::string configName_, std::string inputName_, std::string outputName_, std::string path_ )
{
  Analyze( configName_, inputName_, outputName_, path_ );
}


HanApp::
~HanApp()
{
}


int
HanApp::
Analyze( std::string configName_, std::string inputName_, std::string outputName_, std::string path_ )
{
  DisableMustClean disabled;
  
  HanOutput::DQOutputMap_t * outputMap = new HanOutput::DQOutputMap_t();
  TSeqCollection *outputList = new TList();

  m_outputName = outputName_;
  
  //dqm_core::InputRootFile	input( inputName_ );
  HanInputRootFile input( inputName_, path_ );				// HanInputRootFile inherits from dqm_core::InputRootFile
  HanOutput output( outputName_, outputMap, outputList );
  output.setInput(const_cast<TDirectory*>(input.getBasedir()));
  
  dqm_core::LibraryManager::instance().loadLibrary( "libdqm_algorithms.so" );
  dqm_core::LibraryManager::instance().loadLibrary( "libdqm_summaries.so" );
  
  HanConfig config;
  std::cout << "Reading configuration and input histograms...\n" << std::flush;
  config.BuildMonitors( configName_, input, output );
  std::cout << "Preparing output...\n" << std::flush;
  config.BuildConfigOutput( configName_, input.file(), path_, outputMap, outputList );
  
  std::cout << "Analyzing histograms...\n" << std::flush;
  output.activate();
  input.activate();
  output.publishMissingDQPars();

  std::cout << "Writing output file...\n" << std::flush;
  output.deactivate(); // essential for the HanOutput object since it writes the output file
  
  std::cout << "Deleting objects from memory.\n" << std::flush;
  RecursivelyDeleteCollection(outputList);
  delete outputList;
  delete outputMap;
  return 0;
}


TFile*
HanApp::
OpenResultsFile() const
{
  if( m_outputName == "" )
    return 0;
  
  return TFile::Open( m_outputName.c_str() );
}

} // namespace dqi

