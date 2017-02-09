/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_JETFITTERDUMMYCLASSIFIER_C
#define BTAGTOOL_JETFITTERDUMMYCLASSIFIER_C

/******************************************************
    @class JetFitterDummyClassifier
     Package : JetTagTools
     Created : March 2007

     DESCRIPTION: filler for JetFitter classifier, does nothing

********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "JetTagTools/IJetFitterClassifierTool.h"

namespace Analysis {


static const InterfaceID IID_JetFitterDummyClassifier("Analysis::JetFitterDummyClassifier", 1, 0);

  class IJetFitterTagInfo;

  class JetFitterDummyClassifier : public AthAlgTool, public IJetFitterClassifierTool {

  public:

    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_JetFitterDummyClassifier; };

    JetFitterDummyClassifier(const std::string& name,
			const std::string& n, const IInterface* p);
    ~JetFitterDummyClassifier();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    // IJetFitterClassifier interface
    StatusCode fillLikelihoodValues(xAOD::BTagging* BTag,
                                    const std::string & jetauthor,
                                    const std::string& inputbasename,
                                    const std::string& outputbasename,
                                    double jetpT,
                                    double jeteta,
                                    double IP3dlike=-5000);


  private:
  };

}//end Analysis namespace

#endif
