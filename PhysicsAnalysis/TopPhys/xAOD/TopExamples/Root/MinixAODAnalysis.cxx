/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopExamples//MinixAODAnalysis.h"

#include "TopConfiguration/TopConfig.h"

#include "TopEvent/EventTools.h"
#include "TopEvent/TopEventMaker.h"
#include "TopEvent/Event.h"

#include "TopExamples/DefaultPlots.h"

#include "TopEventSelectionTools/PlotManager.h"
#include "TH1.h"
#include "TH1D.h"
#include "TFile.h"
#include "TDirectoryFile.h"

namespace top{
  MinixAODAnalysis::MinixAODAnalysis( const std::string& name ) :
    asg::AsgTool( name ),
    m_config(nullptr),
    
    m_topTools(nullptr),
    m_sfCalc(nullptr),
    m_evtMaker(nullptr),
    
    m_outputFileName("SetMe"),
    m_outputFile(nullptr),
    
    m_firstEvent(true),
    m_sampleName("SetMe"),
    m_plots()
  {
    declareProperty( "config" , m_config );
    declareProperty( "outputFileName" , m_outputFileName = "miniout.root" );
    declareProperty( "sampleName" , m_sampleName = "Example" );
  }
  
  StatusCode MinixAODAnalysis::initialize()
  {
    
    m_topTools = std::unique_ptr<top::TopToolStore> ( new top::TopToolStore( "top::TopToolStore" ) );
    top::check( m_topTools->setProperty("config",m_config) , "Failed to set property" );
    top::check( m_topTools->initialize() , "Failed to initialize" );    
    
    m_sfCalc = std::unique_ptr<top::ScaleFactorCalculator> ( new top::ScaleFactorCalculator( "top::ScaleFactorCalculator" ) );
    top::check( m_sfCalc->setProperty("config",m_config) , "Failed to set property" );
    top::check( m_sfCalc->initialize() , "Failed to initialize" ); 
   
    m_evtMaker = std::unique_ptr<top::TopEventMaker> ( new top::TopEventMaker( "top::TopEventMaker" ) );
    top::check( m_evtMaker->setProperty("config",m_config) , "Failed to set property" );
    top::check( m_evtMaker->initialize() , "Failed to initialize" );
       
    m_outputFile = new TFile(m_outputFileName.c_str(),"RECREATE");
    
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode MinixAODAnalysis::execute()
  {
    
    ///-- Re-do Scale Factors - You cannot do this with a flat ntuple --///
    top::check( m_sfCalc->execute() , "Failed to calculate scale factors" );    

    const xAOD::SystematicEventContainer* allSystematics = m_evtMaker->systematicEvents( m_config->sgKeyTopSystematicEvents() );
//     ATH_MSG_INFO(" Total number of SystematicEvents (inc Nominal) = "<<allSystematics->size());
    
    ///-- OK, this is a bit ugly, it will be cleaned up massively in future versions --///
    ///-- This is a demonstration program only --///
    
    
    if (m_firstEvent) {
      m_firstEvent = false;
    
      // Setup a top::PlotManager for each channel and each syst
      for (auto channel : *m_config->allSelectionNames()) {
        std::unordered_map<std::string,top::PlotManager*> channelPlotMap;
        for (auto currentSystematic : *allSystematics) {
          channelPlotMap.insert( std::make_pair( 
                m_config->systematicName( currentSystematic->hashValue() ) ,
                new top::PlotManager(m_sampleName,channel,m_config->systematicName( currentSystematic->hashValue() ) ,m_outputFile) 
                         ) );
        }
        m_plots.insert( std::make_pair( channel , channelPlotMap ) );
      }
      
      for (Itr i=m_plots.begin();i!=m_plots.end();++i) {
        for (Itr2 j=(*i).second.begin();j!=(*i).second.end();++j) {
          top::addPlots( *((*j).second) );
        }
      }
      
    }
    
    for (auto currentSystematic : *allSystematics) {
      ///-- Make a top::Event --///
      top::Event topEvent = m_evtMaker->makeTopEvent( *currentSystematic );

      ///-- Loop over all selections --///
      for (auto sel : *m_config->allSelectionNames()) {
        Itr channelPlotMap = m_plots.find( sel );
        if (channelPlotMap != m_plots.end()) {
        
          ///-- If selection passes, plot the top::Event --///
          if (currentSystematic->isAvailable<int>(sel)){
            bool passSelection = currentSystematic->auxdataConst<int>(sel) == 1 ? true : false;
            if (passSelection){
              
              Itr2 systPlotMap = (*channelPlotMap).second.find( m_config->systematicName( currentSystematic->hashValue() ) );
              if (systPlotMap != (*channelPlotMap).second.end()) {
                top::fillPlots(topEvent ,*((*systPlotMap).second) , 1. );
              }
              
            }
            
          }
        }
      }
    }
    
    return StatusCode::SUCCESS; 
  }
    
  
  StatusCode MinixAODAnalysis::finalize()
  {
    m_outputFile->cd();
    m_outputFile->Write();
    
    return StatusCode::SUCCESS;  
  }
  
}
