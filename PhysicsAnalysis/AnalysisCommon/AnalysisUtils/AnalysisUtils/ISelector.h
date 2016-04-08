/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef ANALYSISUTILS_ISELECTOR_H 
#define ANALYSISUTILS_ISELECTOR_H 

/**
   @Project: AnalysisUtils
   
   @class ISelector
   
   @author Amir Farbin <amir.farbin@cernSPAMNOT.ch>
   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>
   
   @date June 2009
   
   @brief Dual use tool (athena/ARA) for any cuts. This is the base class.
*/

#include "AraTool/AraAlgToolWrapper.h"
#include "AnalysisUtils/ISelectorCore.h"

class ISelector : public AraAlgToolWrapper< ISelectorCore >
{

public:
  /** Default constructor */
  ISelector( const std::string& type,
             const std::string& name,
             const IInterface* parent )
    : AthAlgTool( type, name, parent ),
      AraAlgToolWrapper< ISelectorCore > ( type, name, parent )  
  {/*AthAlgTool::declareInterface< ISelectorCore >(this);*/}

};


#endif // ANALYSISUTILS_ISELECTOR_H 
