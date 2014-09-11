//-*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

namespace Analysis {

  class IJetFitterTagInfo;

  static const InterfaceID 
  IID_IJetFitterClassifierTool("Analysis::IJetFitterClassifierTool", 1, 0);

  class IJetFitterClassifierTool : virtual public IAlgTool 
  {
    
  public:
    
    virtual ~IJetFitterClassifierTool(){};
    
    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;
    

    virtual StatusCode fillLikelihoodValues(xAOD::BTagging* BTag,
					    const std::string & jetauthor,
					    const std::string& inputbasename,
					    const std::string& outputbasename,
					    double jetpT,
					    double jeteta,
					    double IP3dlike=-5000) = 0;


    static const InterfaceID& interfaceID() { 
      return IID_IJetFitterClassifierTool; 
    };


  }; 
}

#endif // BTAGTOOL_IJETFITTERCLASSIFIERTOOL_C
