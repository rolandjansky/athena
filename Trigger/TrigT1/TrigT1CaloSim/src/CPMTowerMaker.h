/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ================================================
// CPMTowerMaker class descriptionn
// ================================================



#ifndef CPMTOWERMAKER_H
#define CPMTOWERMAKER_H

// STL
#include <string>
#include <vector>

// Athena/Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigT1CaloToolInterfaces/IL1CPMTowerTools.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloEvent/CPMTower_ClassDEF.h"



//********************************************
//Default for parameters of this algorithm
//These can be overridden by job options.
//Descriptions are given in the class declaration
//********************************************

namespace LVL1 {

//                 CPMTowerMaker class declaration
/*
 An "Athena algorithm" is something which gets scheduled and controlled
 by the framework In its simplest definition it is something which
  - gets called for each event
  - can get anything it wants out of the Transient Event Store"
  - can put anything it makes into the store
 It must inherit from the  AthAlgorithm base class
 */
/**
The CPMTowerMaker class takes Trigger Towers from the TES and
forms CPMTowers, which it then places back into the TES.
The CPMTowers so formed are used for the bytestream simulation.
*/
class CPMTowerMaker : public AthAlgorithm
{
  typedef DataVector<CPMTower> CPMTCollection;
  typedef DataVector<TriggerTower> TTCollection ;
 public:

  //-------------------------
  // Constructors/Destructors
  //
  // Athena requires that the constructor takes certain arguments
  // (and passes them directly to the constructor of the base class)
  //-------------------------

  CPMTowerMaker( const std::string& name, ISvcLocator* pSvcLocator ) ;

  //------------------------------------------------------
  // Methods used by Athena to run the algorithm
  //------------------------------------------------------

  StatusCode initialize() ;
  StatusCode execute() ;

 private:

  ToolHandle<LVL1::IL1CPMTowerTools> m_CPMTowerTool;
  SG::ReadHandleKey<TTCollection> m_triggerTowerLocation { this, "TriggerTowerLocation", TrigT1CaloDefs::TriggerTowerLocation};
  SG::WriteHandleKey<CPMTCollection> m_cpmTowerLocation { this, "CPMTowerLocation", TrigT1CaloDefs::CPMTowerLocation};

};

} // end of namespace bracket
#endif
