/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef ANALYSISUTILS_CONTAINERFILTER_H 
#define ANALYSISUTILS_CONTAINERFILTER_H 

#include "AraTool/AraToolBase.h"
#include "AnalysisUtils/ContainerFilterCore.h"

class ContainerFilter : public AraAlgToolWrapper< ContainerFilterCore >
{

public:
  /** Default constructor */
  ContainerFilter( const std::string& type,
                   const std::string& name,
                   const IInterface* parent )
    : AthAlgTool( type, name, parent ),
      AraAlgToolWrapper< ContainerFilterCore > ( type, name, parent )  
  {/*AthAlgTool::declareInterface< ContainerFilterCore >(this);*/}

};

#endif // ANALYSISUTILS_CONTAINERFILTER_H 
