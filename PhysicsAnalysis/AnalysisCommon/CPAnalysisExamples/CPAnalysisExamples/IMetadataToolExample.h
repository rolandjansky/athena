/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CPANALYSISEXAMPLES_IMETADATATOOLEXAMPLE_H
#define CPANALYSISEXAMPLES_IMETADATATOOLEXAMPLE_H 1

#include "AsgTools/IAsgTool.h"

namespace CP {

class IMetadataToolExample : public virtual asg::IAsgTool {
   public:
      ASG_TOOL_INTERFACE( IMetadataToolExample ) //declares the interface to athena

      //this is just a dummy interface
      //technically not necessary

};

}

#endif //> !CPANALYSISEXAMPLES_IMETADATATOOLEXAMPLE_H
