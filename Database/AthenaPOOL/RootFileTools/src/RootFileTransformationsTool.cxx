/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file RootFileTransformationsTool.cxx
 *  @brief Tool to call RootFileTransformations to optimize output files
 *  @author Gerhard Brandt <gbrandt@cern.ch>
 *  $Id:  $
 **/

#include <stdio.h>
#include "RootFileTransformationsTool.h"
#include "cobs.h"

RootFileTransformationsTool::RootFileTransformationsTool(const std::string& type,
                                                         const std::string& name,
                                                         const IInterface* parent)
        : AthAlgTool(type, name, parent)
{
   // Declare IRootFileTransformationsTool interface
   declareInterface<IRootFileTransformationsTool>(this);
}
//__________________________________________________________________________
RootFileTransformationsTool::~RootFileTransformationsTool() {
}
//__________________________________________________________________________
StatusCode RootFileTransformationsTool::initialize() {
   ATH_MSG_INFO ("Initializing " << name() << " - package version " << PACKAGE_VERSION);

   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode RootFileTransformationsTool::finalize() {   
   return(::AlgTool::finalize());
}
//__________________________________________________________________________
StatusCode RootFileTransformationsTool::reorderBaskets(std::string fileName, std::string option) {
   ATH_MSG_INFO("Reordering baskets on file " << fileName << " using " << option);

   std::string optFileName = fileName+".opt.root";   
   (void) remove(optFileName.c_str());
      
   cobs* mcobs = new cobs(fileName, optFileName);

   // set debug level of optimizer
   if ( msg().level() <= MSG::DEBUG) {
       mcobs->setDebug(true);
       // dump tree info before operations
       mcobs->printTrees(fileName);
   } 

   if (option=="SortBasketsByOffset") {    
     if (mcobs->order(1))
         (void)rename(optFileName.c_str(), fileName.c_str());
     else 
         (void)remove(optFileName.c_str());
   } 
   else if(option=="SortBasketsByEntry") {    
     if (mcobs->order(2))
         (void)rename(optFileName.c_str(), fileName.c_str());
     else 
         (void)remove(optFileName.c_str());
   }  
   else if(option=="SortBasketsByBranch") {    
     if (mcobs->order(3))
         (void)rename(optFileName.c_str(), fileName.c_str());
      else 
          (void)remove(optFileName.c_str());
   }
   else {
      ATH_MSG_WARNING("Unsupported ordering option " << option << ". Not ordering file " << fileName);
   }
   if ( msg().level() <= MSG::DEBUG) {
     mcobs->printTrees(fileName);
   }    
   
   delete mcobs;
   
   ATH_MSG_INFO("Finished optimizing " << fileName);
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode RootFileTransformationsTool::resizeBaskets(std::string fileName, std::string algo, std::string treesToExclude, std::string memPerTree, int maxMemory, float minComp) {

   ATH_MSG_INFO ("Optimizing basket sizes of the file " << fileName << "\tusing " << algo << "\ttreesToExclude: "<< treesToExclude << "\tmemPerTree: " << memPerTree << "\ttotal memory: " << maxMemory << "\tmin compression: " << minComp);

   std::string optFileName = fileName+".opt.root";
   (void) remove(optFileName.c_str());
   
   cobs* mcobs = new cobs(fileName, optFileName);   
   // mcobs->setDebugLevel(1);
   mcobs->setDedicatedMemory(maxMemory);
   mcobs->setMinCompressionFactor(minComp);
   // mcobs->setTreeToSkip(); // here decode treesToExclude and set them
   // mcobs->setTreeMemory(mem,treeName); // here decode memPerTree and set them
   
   if (algo=="1") { // currently only one method exists.
       if (mcobs->resizeBaskets())
           (void)rename(optFileName.c_str(), fileName.c_str());
       else 
           (void)remove(optFileName.c_str());
   } 
   else {
     ATH_MSG_WARNING("Unsupported resizing option " << algo << ". Not resizing baskets on file " << fileName);
   }
   
   delete mcobs;

   ATH_MSG_INFO("Finished optimizing " << fileName);
   return(StatusCode::SUCCESS);
}
