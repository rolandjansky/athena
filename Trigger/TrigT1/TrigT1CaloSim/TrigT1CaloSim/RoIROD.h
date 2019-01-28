/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          RoIROD.h  -  description
                             -------------------
    begin                : Mon Jul 28 2014
    email                : Alan.Watson@CERN.CH
 ***************************************************************************/


#ifndef _RoIROD_H_
#define _RoIROD_H_

// STL
#include <string>
#include <vector>

//Athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthContainers/DataVector.h"
#include "StoreGate/StoreGateSvc.h"

// Specifics
#include "TrigT1Interfaces/SlinkWord.h" 
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

// Inputs
#include "TrigT1CaloEvent/CPMTobRoI_ClassDEF.h"
#include "TrigT1CaloEvent/JEMTobRoI_ClassDEF.h"
#include "TrigT1CaloEvent/CMXRoI.h"

 /**LVL1 namespace.
 This belongs to the TrigT1Calo simulation.
 */
namespace LVL1 {

//RoIROD class declaration
/**
The RoIROD Athena Algorithm takes the output from the LVL1 Calo Trigger simulations and
produces the words, as would be produced by the hardware ReadOut Drivers (RODs);
i.e.
-Cluster (Em & Tau) ROD to RoIB<br>
-Jet/Energy ROD to RoIB<br>
*/
class RoIROD : public AthAlgorithm
{
  typedef DataVector<LVL1::CPMTobRoI> t_cpmTobRoIContainer;
  typedef DataVector<LVL1::JEMTobRoI> t_jemTobRoIContainer;
  typedef CMXRoI                      t_EnergyRoIContainer;

 public:
  //-------------------------

  RoIROD( const std::string& name, ISvcLocator* pSvcLocator ) ;
  ~RoIROD();


  //------------------------------------------------------
  // Methods used by Athena to run the algorithm
  //------------------------------------------------------

  StatusCode initialize() ;
  StatusCode execute() ;
  StatusCode finalize() ;

private: // Private methods
	/** adds slink header */
  void addHeader( DataVector<LVL1CTP::SlinkWord>* slink, unsigned int subDetID, unsigned int moduleId);
  /** add Slink tail */
  void addTail( DataVector<LVL1CTP::SlinkWord>* slink, unsigned int numberOfDataWords);
  /** save Slink Objects to SG */
  void saveSlinkObjects();
  /** get ROIwords and form Slink words from them, adding header and tail. */
  void formSlinkObjects();
  /** Create the object vectors to be stored in SG and clear mult vectors*/
  void assignVectors();
  /** creates a new SlinkWord object with the passed word, and returns a pointer.*/
  LVL1CTP::SlinkWord* getWord(unsigned int tword);

  /** prints out the Slink info. */
  void dumpSlinks() const;

private: // Private attributes
  unsigned int m_eventNumber;
  std::string m_emTauRoILocation ;
  std::string m_emTauSlinkLocation ;
  std::string m_JetRoILocation ;
  std::string m_energyRoILocation;
  std::string m_jepSlinkLocation ;
  /** there are 4 CP RoI RODs which have a Slink cable connected to the RoIB. This array holds pointers to 4
  DataVectors containing the Slink words*/
  DataVector<LVL1CTP::SlinkWord>* m_CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs];
  /** there are 2 Jet RoI RODs which have a Slink cable connected to the RoIB. This array holds pointers to 2
  DataVectors containing the Slink words*/
  DataVector<LVL1CTP::SlinkWord>* m_jepRoIROD[TrigT1CaloDefs::numOfJEPRoIRODs];
  
};

} // end of namespace bracket


#endif











