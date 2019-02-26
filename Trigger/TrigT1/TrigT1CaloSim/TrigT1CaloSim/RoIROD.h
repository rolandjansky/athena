/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          RoIROD.h  -  description
                             -------------------
    begin                : Mon Jul 28 2014
    email                : Alan.Watson@CERN.CH
 ***************************************************************************/


#ifndef TRIGT1CALOSIM_ROIROD_H
#define TRIGT1CALOSIM_ROIROD_H

// STL
#include <string>
#include <vector>

//Athena
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthContainers/DataVector.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKeyArray.h"

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
class RoIROD : public AthReentrantAlgorithm
{
  typedef DataVector<LVL1::CPMTobRoI> t_cpmTobRoIContainer;
  typedef DataVector<LVL1::JEMTobRoI> t_jemTobRoIContainer;
  typedef CMXRoI                      t_EnergyRoIContainer;
  typedef DataVector<LVL1CTP::SlinkWord> t_SlinkContainer;
  typedef std::unique_ptr<t_SlinkContainer> t_SlinkPtr;

 public:
  //-------------------------

  RoIROD( const std::string& name, ISvcLocator* pSvcLocator ) ;
  virtual ~RoIROD();


  //------------------------------------------------------
  // Methods used by Athena to run the algorithm
  //------------------------------------------------------

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

private: // Private methods
	/** adds slink header */
  void addHeader (t_SlinkContainer& slink,
                  unsigned int subDetID,
                  unsigned int moduleId,
                  const EventContext& ctx) const;
  /** add Slink tail */
  void addTail (t_SlinkContainer& slink,
                unsigned int numberOfDataWords) const;
  /** save Slink Objects to SG */
  StatusCode saveSlinkObjects (t_SlinkPtr CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs],
                               t_SlinkPtr jepRoIROD[TrigT1CaloDefs::numOfJEPRoIRODs],
                               const EventContext& ctx) const;
  /** get ROIwords and form Slink words from them, adding header and tail. */
  void formSlinkObjects (t_SlinkPtr CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs],
                         t_SlinkPtr jepRoIROD[TrigT1CaloDefs::numOfJEPRoIRODs],
                         const EventContext& ctx) const;
  /** Create the object vectors to be stored in SG and clear mult vectors*/
  void assignVectors (t_SlinkPtr CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs],
                      t_SlinkPtr jepRoIROD[TrigT1CaloDefs::numOfJEPRoIRODs]) const;
  /** creates a new SlinkWord object with the passed word, and returns a pointer.*/
  std::unique_ptr<LVL1CTP::SlinkWord> getWord(unsigned int tword) const;

  /** prints out the Slink info. */
  void dumpSlinks (t_SlinkPtr CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs]) const;

private: // Private attributes
  SG::ReadHandleKey<t_cpmTobRoIContainer> m_emTauRoILocation
  { this, "EmTauRoILocation", TrigT1CaloDefs::CPMTobRoILocation, "" };
  SG::ReadHandleKey<t_jemTobRoIContainer> m_JetRoILocation
  { this, "JetRoILocation", TrigT1CaloDefs::JEMTobRoILocation, "" };
  SG::ReadHandleKey<t_EnergyRoIContainer> m_energyRoILocation
  { this, "EnergyRoILocation", TrigT1CaloDefs::CMXRoILocation, "" };

  std::string m_emTauSlinkLocation ;
  SG::WriteHandleKeyArray<t_SlinkContainer> m_emTauSlinkKeys
  { this, "EmTauSlinkKeys", {}, "" };

  std::string m_jepSlinkLocation ;
  SG::WriteHandleKeyArray<t_SlinkContainer> m_jepSlinkKeys
  { this, "JEPSlinkKeys", {}, "" };
};

} // end of namespace bracket


#endif
