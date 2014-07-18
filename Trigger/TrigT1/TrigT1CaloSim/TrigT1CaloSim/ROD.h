/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          ROD.h  -  description
                             -------------------
    begin                : Wed Dec 13 2000
    copyright            : (C) 2000 by moyse
    email                : moyse@heppch.ph.qmw.ac.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _ROD_H_
#define _ROD_H_

// STL
#include <string>
#include <vector>

//Athena
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "DataModel/DataVector.h"
#include "StoreGate/StoreGateSvc.h"

//CTP
#include "TrigT1Interfaces/SlinkWord.h"
 
 // Include for the configuration service:
#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/CaloInfo.h"

//TrigT1Calo
#include "TrigT1CaloEvent/EmTauROI.h"
#include "TrigT1Interfaces/EmTauCTP.h"
#include "TrigT1Interfaces/JetCTP.h"
#include "TrigT1CaloEvent/JetROI.h"
#include "TrigT1CaloEvent/EnergyRoI.h"
#include "TrigT1CaloEvent/EnergyRoI_ClassDEF.h"
#include "TrigT1CaloEvent/JetEtRoI.h"

#include "TrigT1Interfaces/TrigT1CaloDefs.h"
//#include "TrigT1Interfaces/CTPJetEnergyConfig.h"


 /**LVL1 namespace.
 This belongs to the TrigT1Calo simulation.
 */
namespace LVL1 {

using namespace TrigConf;

  //ROD class declaration
/**
The ROD Athena Algorithm takes the output from the LVL1 Calo Trigger simulations and
produces the words, as would be produced by the hardware ReadOut Drivers (RODs) & Common Merger Modules (CMMs);
i.e.
-Cluster (Em/Tau) CMM to CTP<br>
-Jet CMM to CTP<br>
-Cluster (Em/Tau) ROD to RoIB<br>
-Jet ROD to RoIB<br>
*@author Ed Moyse
*/
class ROD : public Algorithm
{
  typedef DataVector<LVL1::EmTauROI> t_emTauRoIContainer;
  typedef DataVector<LVL1::JetROI> t_jetRoIContainer;
  typedef EnergyRoI t_EnergyRoIContainer;

 public:
  //-------------------------

  ROD( const std::string& name, ISvcLocator* pSvcLocator ) ;
  ~ROD();


  //------------------------------------------------------
  // Methods used by Athena to run the algorithm
  //------------------------------------------------------

  StatusCode initialize() ;
  StatusCode beginRun() ;
  StatusCode execute() ;
  StatusCode finalize() ;
  /** create objects to be passed to CTP. */
  void formCTPObjects();
  /** dump details of any threshold overflows (i.e. more RoIs pass threshold(s) than can be indicated to CTP) */
  void printOverflows() const;



private: // Private methods
	/** adds slink header */
  void addHeader( DataVector<LVL1CTP::SlinkWord>* slink, unsigned int subDetID, unsigned int moduleId);
  /** add Slink tail */
  void addTail( DataVector<LVL1CTP::SlinkWord>* slink, unsigned int numberOfDataWords);
  /** save Slink Objects to SG */
  void saveSlinkObjects();
  /** save CTP Objects to SG */
  void saveCTPObjects();
  /** get ROIwords and form Slink words from them, adding header and tail. */
  void formSlinkObjects();
  /** Get Jet and EmTau RoI objects. */
  void loadRoIs();
  /** process Em/Tau RoIs extracting RoI words and counting multiplicities */
  void processRoIs(const t_emTauRoIContainer* EMs);
  /** process Jet RoIs extracting RoI words and counting multiplicities */
  void processRoIs(const t_jetRoIContainer* jets);
  /** process Energy RoI, extracting RoI words and counting multiplicities */
  void processRoIs(const t_EnergyRoIContainer* energy);
  /** Create the object vectors to be stored in SG and clear mult vectors*/
  void assignVectors();
  /** creates a new SlinkWord object with the passed word, and returns a pointer.*/
  LVL1CTP::SlinkWord* getWord(unsigned int tword);
  /** returns a mult. data word with 3 bits per threshold, starting at thresh 1 and ending at thresh 8.
  <pre> 6*0 | 2b (reserved) | 1b Parity | Th8 | Th7 | Th6 | Th5 | Th4| Th3 | Th2 | Th1 | 0</pre>*/
  unsigned int EmTauCTPWord(unsigned int first, unsigned int last);
  /** returns a mult. data word with 3 bits per threshold, starting at thresh first and ending at thresh last.
  <pre> 2*0 | 2b (reserved) | 1b Parity | 4b JetEtMap | Th8 | Th7 | Th6 | Th5 | Th4| Th3 | Th2 | Th1 | 0</pre>*/
  unsigned int jetCTPWord0();
  /** returns a mult. data word with 3 bits per threshold
  <pre> 14*0 | 2b (reserved) | 1b Parity | R4 | R3 | R2 | R1 | L4 | L3 | L2 | L1 | 0</pre>*/
  unsigned int jetCTPWord1();
  /** form EmTauCTP objects */
  void formEmTauCTPObjects();
  /** form Jet Trigger CTP objects */
  void formJetCTPObjects();
  /** add tails to Slink objects. */
  void finaliseSlinkObjects();

  /** prints out the Slink info. */
  void dumpSlinks() const;
  /** returns JetEt */
  unsigned int getJetEt() const;
  /**  Obtain factors for each jet threshold in JetEt sum */
  void jetEtFactors();
  /** estimates Jet ET and does trigger. */
  void jetEtTrigger();

private: // Private attributes
  /** Pointer to SG service*/
  ServiceHandle<StoreGateSvc> m_storeGate;
  ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;

  unsigned int m_eventNumber;
  std::string m_emTauRoILocation ;
  std::string m_emTauCTPLocation ;
  std::string m_emTauSlinkLocation ;
  std::string m_jepSlinkLocation ;
  std::string m_JetRoILocation ;
  std::string m_JetCTPLocation ;
  std::string m_JetEtRoILocation ;
  std::string m_CTPOutputLocation ;
  std::string m_RoIBOutputLocation;
  std::string m_energyRoILocation;
  /** there are 4 CP RoI RODs which have a Slink cable connected to the RoIB. This array holds pointers to 4
  DataVectors containing the Slink words*/
  DataVector<LVL1CTP::SlinkWord>* m_CPRoIROD[TrigT1CaloDefs::numOfCPRoIRODs];
  /** there are 2 Jet RoI RODs which have a Slink cable connected to the RoIB. This array holds pointers to 2
  DataVectors containing the Slink words*/
  DataVector<LVL1CTP::SlinkWord>* m_jepRoIROD[TrigT1CaloDefs::numOfJEPRoIRODs];
  /** The energy Slink will actually be on one of the JEP Slinks, but this is easier*/
  //DataVector<LVL1CTP::SlinkWord>* m_energyRoIROD;
  /** See above comment*/
  //DataVector<LVL1CTP::SlinkWord>* m_jetEtRoIROD;
  /** the number of times each CP (Em/Tau) threshold was passed*/
  unsigned int m_emTauMult[TrigT1CaloDefs::numOfCPThresholds];
  /** the number of times each Jet threshold was passed*/
  unsigned int m_jetMult[TrigT1CaloDefs::numOfJetThresholds];
  /** the number of times each Forward Jet threshold was passed*/
  unsigned int m_leftFwdJetMult[TrigT1CaloDefs::numOfFwdJetThresholds];
  /** the number of times each Forward Jet threshold was passed*/
  unsigned int m_rightFwdJetMult[TrigT1CaloDefs::numOfFwdJetThresholds];
  /** factors for JetEtSum calculation */
  unsigned int m_factor[TrigT1CaloDefs::numOfJetThresholds];
  unsigned int m_fwdfactor[TrigT1CaloDefs::numOfFwdJetThresholds];
  /** JetEtSum RoI */
  JetEtRoI* m_jetEtRoI;
  /** CTP info*/
  EmTauCTP* m_emTauCTP;
  /** CTP info*/
  JetCTP* m_JetCTP;
  /** CTP info */
  //JetEtCTP* m_JetEtCTP;
  /** records the thresholds with overflows i.e. where there are more RoIs passing thresholds than we can indicate to CTP*/
  std::vector<unsigned int>* m_emTauOverflow;
 /** records the thresholds with overflows i.e. where there are more RoIs passing thresholds than we can indicate to CTP*/
  std::vector<unsigned int>* m_jetOverflow;
  /** contains thresholds sorted by ascending energy*/
  std::map<unsigned int, unsigned int> m_sortedThresholds;
  std::map<unsigned int, unsigned int> m_sortedFwdThresholds;

  unsigned int m_jetEtThreshMap;

  /** Multiplicity limits for CTP */
  static const unsigned int m_bitsPerThresh = 3;
  static const unsigned int m_bitsPerFwdThresh = 2;
  
};

} // end of namespace bracket


#endif











