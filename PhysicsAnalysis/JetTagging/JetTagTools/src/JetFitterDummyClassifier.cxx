/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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


  StatusCode JetFitterDummyClassifier::initialize() {
    ATH_MSG_DEBUG(" Initialization of JetFitterDummyClassifier succesfull");
    return StatusCode::SUCCESS;
  }

  StatusCode JetFitterDummyClassifier::finalize() {
    ATH_MSG_DEBUG(" Finalization of JetFitterDummyClassifier succesfull");
    return StatusCode::SUCCESS;
  }


  StatusCode JetFitterDummyClassifier
  ::fillLikelihoodValues(xAOD::BTagging* /*BTag*/,
                         const std::string & /*jetauthor*/,
                         const std::string& /*inputbasename*/,
                         const std::string& /*outputbasename*/,
                         float /* jetpT */,
                         float /* jeteta */,
                         float /* IP3dlike=-5000 */) const
  {
    return StatusCode::SUCCESS;
  }


}//end Analysis namespace
