/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOP_TOPEVENT_PSEUDOTOPRESULT_H
#define ANALYSISTOP_TOPEVENT_PSEUDOTOPRESULT_H

/**
  * @author Andrea Knue <aknue@cern.ch>
  **/ 

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxElement.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/CLASS_DEF.h"
#include <vector>

namespace xAOD{

  /// Aux Container 
  class PseudoTopResultAuxContainer : public AuxContainerBase {
    public:
      /// Default constructor
      PseudoTopResultAuxContainer();     
  }; // end Aux container
  
  /// Interface class
  class PseudoTopResult : public SG::AuxElement {
    public:
      /// Default constructor
      PseudoTopResult();
      /// Default desturctor
      virtual ~PseudoTopResult(){}    

      void IniVar(bool);
         
  };   
  typedef DataVector < xAOD::PseudoTopResult > PseudoTopResultContainer;   
}

// Dictonaries
CLASS_DEF( xAOD::PseudoTopResult , 135846343 , 1 )
CLASS_DEF( xAOD::PseudoTopResultContainer , 1219079565 , 1 )
CLASS_DEF( xAOD::PseudoTopResultAuxContainer , 1244378748 , 1 )

#endif
