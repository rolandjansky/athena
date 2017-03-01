/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : JetFitterDummyClassifier.h
/// Package : BTagTools
/// Author  : Dan Guest
/// Created : Jan 2017
///
/// DESCRIPTION: filler for JetFitter classifier, does nothing
///
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include "GaudiKernel/ITHistSvc.h"


#include "JetTagTools/IJetFitterClassifierTool.h"
#include "JetTagTools/JetFitterDummyClassifier.h"

namespace Analysis {


  JetFitterDummyClassifier::JetFitterDummyClassifier(const std::string& name,
                                                     const std::string& n,
                                                     const IInterface* p):
    AthAlgTool(name, n,p)
  {
    declareInterface<IJetFitterClassifierTool>(this);
  }

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

  JetFitterDummyClassifier::~JetFitterDummyClassifier() {
  }

  StatusCode JetFitterDummyClassifier::initialize() {
    ATH_MSG_INFO(" Initialization of JetFitterDummyClassifier succesfull");
    return StatusCode::SUCCESS;
  }

  StatusCode JetFitterDummyClassifier::finalize() {
    ATH_MSG_INFO(" Finalization of JetFitterDummyClassifier succesfull");
    return StatusCode::SUCCESS;
  }


  StatusCode JetFitterDummyClassifier
  ::fillLikelihoodValues(xAOD::BTagging* /*BTag*/,
                         const std::string & /*jetauthor*/,
                         const std::string& /*inputbasename*/,
                         const std::string& /*outputbasename*/,
                         double /* jetpT */,
                         double /* jeteta */,
                         double /* IP3dlike=-5000 */) {
    return StatusCode::SUCCESS;

  }



}//end Analysis namespace
