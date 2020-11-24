//-*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_IJETFITTERCLASSIFIERTOOL_C
#define BTAGTOOL_IJETFITTERCLASSIFIERTOOL_C

/******************************************************
    @class IJetFitterClassifierTool
    Package : JetTagTools 
    Created : Aug 2012
    DESCRIPTION:
    General interface for JetFitter based classifiers. Used as the base 
    for JetFitterNNTool, but needn't be NN based. 

    @author Dan Guest 
    
********************************************************/

#include "GaudiKernel/IAlgTool.h"
#include "xAODBTagging/BTagging.h"
#include <string>

#include "xAODBTagging/ftagfloat_t.h"

namespace Analysis {

  class IJetFitterTagInfo;

  class IJetFitterClassifierTool : virtual public IAlgTool 
  {
    
  public:
    DeclareInterfaceID( IJetFitterClassifierTool, 1, 0 );
    
    virtual ~IJetFitterClassifierTool() = default;
    
    virtual StatusCode fillLikelihoodValues(xAOD::BTagging* BTag,
					    const std::string & jetauthor,
					    const std::string& inputbasename,
					    const std::string& outputbasename,
					    ftagfloat_t jetpT,
					    ftagfloat_t jeteta,
					    ftagfloat_t IP3dlike=-5000) const = 0;
  }; 
}

#endif // BTAGTOOL_IJETFITTERCLASSIFIERTOOL_C
