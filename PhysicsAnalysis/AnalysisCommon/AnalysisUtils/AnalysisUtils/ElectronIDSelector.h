///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISUTILS_ELECTRONIDSELECTOR_H 
#define ANALYSISUTILS_ELECTRONIDSELECTOR_H

/**
   @Project: AnalysisUtils
 
   @class ElectronIDSelector
   
   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>
   
   @date August 2010
   
   @brief Dual use tool (athena/ARA) for specialized electron identification cuts.
*/

#include "AraTool/AraAlgToolWrapper.h"
#include "AnalysisUtils/ElectronIDSelectorCore.h"

class ElectronIDSelector : public AraAlgToolWrapper< ElectronIDSelectorCore >
{

public:
  /** Default constructor */
  ElectronIDSelector( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
    : AthAlgTool( type, name, parent ),
      AraAlgToolWrapper< ElectronIDSelectorCore > ( type, name, parent )  
  {/*AthAlgTool::declareInterface< ElectronIDSelectorCore >(this);*/}

};

#endif //ANALYSISUTILS_ELECTRONIDSELECTOR_H
