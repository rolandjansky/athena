/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


 /***************************************************************************
                           EnergyTrigger.h  -  description
                              -------------------
     begin                : Wed Dec 13 2000
     copyright            : (C) 2000 by moyse
     email                : moyse@heppch.ph.qmw.ac.uk
  ***************************************************************************/

 #ifndef ENERGYTRIGGER_H
 #define ENERGYTRIGGER_H

 // STL
 #include <string>
 #include <vector>
 #include <map>

 // Athena/Gaudi
 #include "AthenaBaseComps/AthAlgorithm.h"
 #include "GaudiKernel/ServiceHandle.h"  
 #include "GaudiKernel/ToolHandle.h"
 

 namespace LVL1 {

   //Doxygen class description below:
   /**
 This class simulates the missing ET and total ET triggers.
 EnergyTrigger uses EnergyCrate and JetElement objects in order to closely follow
 the layout of the hardware.
   */
 class EnergyTrigger : public AthAlgorithm
 {

  public:

   //-------------------------
   // Constructors/Destructors
   //
   // Athena requires that the constructor takes certain arguments
   // (and passes them directly to the constructor of the base class)
   //-------------------------

   EnergyTrigger( const std::string& name, ISvcLocator* pSvcLocator ) ;

   virtual ~EnergyTrigger();


   //------------------------------------------------------
   // Methods used by Athena to run the algorithm
   //------------------------------------------------------

   StatusCode initialize() ;
   StatusCode execute() ;
   StatusCode finalize() ;

private:
 /* StoreGate keys */
  std::string   m_JetElementLocation ;
  std::string   m_jemEtSumsLocation ;
  std::string   m_energyRoILocation ;
  std::string   m_energyCTPLocation ;
};

} // end of namespace bracket

#endif
 
