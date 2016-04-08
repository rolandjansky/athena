///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISUTILS_KINEMATICSELECTOR_H 
#define ANALYSISUTILS_KINEMATICSELECTOR_H

/**
   @Project: AnalysisUtils
 
   @class KinematicSelector
   
   @author Amir Farbin <amir.farbin@cernSPAMNOT.ch>
   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>
   
   @date June 2009
   
   @brief Dual use tool (athena/ARA) for kinematic cuts.
*/

#include "AraTool/AraAlgToolWrapper.h"
#include "AnalysisUtils/KinematicSelectorCore.h"

class KinematicSelector : public AraAlgToolWrapper< KinematicSelectorCore >
{

public:
  /** Default constructor */
  KinematicSelector( const std::string& type,
                     const std::string& name,
                     const IInterface* parent )
    : AthAlgTool( type, name, parent ),
      AraAlgToolWrapper< KinematicSelectorCore > ( type, name, parent )  
  {/*AthAlgTool::declareInterface< KinematicSelectorCore >(this);*/}

};

#endif //ANALYSISUTILS_KINEMATICSELECTOR_H 
