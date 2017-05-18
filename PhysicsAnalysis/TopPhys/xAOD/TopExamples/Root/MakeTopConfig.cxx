/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopExamples/MakeTopConfig.h"
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/TopPersistentSettings.h"
#include "TopEvent/EventTools.h"

namespace top{
  MakeTopConfig::MakeTopConfig( const std::string& name ) :
    asg::AsgMetadataTool( name )
  {
  }
  
  std::shared_ptr<top::TopConfig> MakeTopConfig::makeTopConfig()
  {
    ///-- Get the settings from the input file
    const top::TopPersistentSettings* settings(nullptr);
    top::check( inputMetaStore()->retrieve(settings,"TopPersistentSettings") , "Failed to retrieve TopPersistentSettings");
    
    ///-- Create the top::TopConfig from the TopPersistentSettings --///
    std::shared_ptr<top::TopConfig> config( new top::TopConfig( settings ) ); 
    
    return config;
  }
}