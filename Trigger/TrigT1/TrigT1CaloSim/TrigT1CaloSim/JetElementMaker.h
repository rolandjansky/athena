/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// JetElementMaker class descriptionn
// ================================================



#ifndef JETELEMENTMAKER_H
#define JETELEMENTMAKER_H

// STL
#include <string>
#include <vector>

// Athena/Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/StoreGateSvc.h"

//LVL1 Calo trigger includes
#include "TrigT1CaloToolInterfaces/IL1JetElementTools.h"


//********************************************
//Default for parameters of this algorithm
//These can be overridden by job options.
//Descriptions are given in the class declaration
//********************************************

namespace LVL1 {

//class StoreGateSvc;

//                 JetElementMaker class declaration
/*
 An "Athena algorithm" is something which gets scheduled and controlled
 by the framework In its simplest definition it is something which
  - gets called for each event
  - can get anything it wants out of the Transient Event Store"
  - can put anything it makes into the store
 It must inherit from the  Algorithm base class
 */
/**
The JetElementMaker class takes Trigger Towers from the TES and
forms JetElements, which it then places back into the TES.
*/
class JetElementMaker : public AthAlgorithm
{
  typedef DataVector<JetElement> JECollection;
  
 public:

  //-------------------------
  // Constructors/Destructors
  //
  // Athena requires that the constructor takes certain arguments
  // (and passes them directly to the constructor of the base class)
  //-------------------------

  JetElementMaker( const std::string& name, ISvcLocator* pSvcLocator ) ;


  //------------------------------------------------------
  // Methods used by Athena to run the algorithm
  //------------------------------------------------------

  StatusCode initialize() ;
  StatusCode execute() ;
  StatusCode finalize() ;


 private:

   ToolHandle<LVL1::IL1JetElementTools> m_JetElementTool;

  /** location of TriggerTowers in TES */
  std::string m_triggerTowerLocation;
  /** locations within the TES to store collections of JEs*/
  std::string   m_jetElementLocation ;
  
};

} // end of namespace bracket
#endif
