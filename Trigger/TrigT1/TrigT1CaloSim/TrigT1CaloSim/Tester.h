/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          Tester.h  -  description
                             -------------------
    begin                : Wed Dec 13 2000
    email                : moyse@heppch.ph.qmw.ac.uk
 ***************************************************************************/

#ifndef _TESTER_H_
#define _TESTER_H_

// STL
#include <string>
#include <vector>

// Athena/Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/DataObject.h"
#include "AthContainers/DataVector.h"
//#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

//TrigT1Calo includes
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloEvent/EmTauROI.h"
#include "TrigT1CaloEvent/JetROI.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/SlinkWord.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"
#include "TrigT1CaloUtils/JetEnergyModuleKey.h"
#include "TrigT1CaloUtils/CPAlgorithm.h"
#include "TrigT1CaloUtils/JetAlgorithm.h"
#include "TrigT1CaloToolInterfaces/IL1EmTauTools.h"
#include "TrigT1CaloToolInterfaces/IL1JetTools.h"
#include "TrigT1CaloToolInterfaces/IL1EtTools.h"



//Default for parameters of this algorithm
//These can be overridden by job options.
//Descriptions are given in the class declaration


#define DEFAULT_TriggerTowerLocation    "/Event/LVL1TriggerTowers"
#define DEFAULT_EmTauROILocation     "/Event/EmTauROIs"
#define DEFAULT_actualROIWordLocation "TestVectors/RoI_elnew.dat"

 /**LVL1 namespace.
 This belongs to the TrigT1Calo atrig simulation.
 */
namespace LVL1 {

/**
 The Tester algorithm checks the performance of the LVL1 em simulation.
*/
class Tester : public AthAlgorithm
{

 public:

  //-------------------------
  // Constructors/Destructors
  //
  // Athena requires that the constructor takes certain arguments
  // (and passes them directly to the constructor of the base class)
  //-------------------------

  Tester( const std::string& name, ISvcLocator* pSvcLocator ) ;


  //------------------------------------------------------
  // Methods used by Athena to run the algorithm
  //------------------------------------------------------

  StatusCode initialize() ;
  StatusCode execute() ;
  StatusCode finalize() ;




 private:
 /**this is a TriggerTower container*/
 typedef DataVector<EmTauROI>               t_EmTauROICollection ;
 typedef DataVector<JetROI>                 t_JetROICollection ;
 typedef DataVector<JEMHits> JEMHitsCollection;
 typedef DataVector<JEMEtSums> JEMEtSumsCollection;

private: // Private methods
  /** loads the Trigger Towers from the TES.*/
	void loadTriggerTowers();
  void loadEmTauROIs();
	void loadActualROIWord();
	void compareROIWords();
	void printTriggerTowerValues();
	void printEmTauROIValues();
  /** load and lookat SlinkObject. Form lost of EmTauRoIwords from them and check coordinates. */
  void examineSlinkObjects();
  /** Compare RoI coords reconstructed from RoI words with coords from original RoI objects */
  void compareRoIWordCoords();
  /** returns a vector of RoIwords extracted from the Slink fragment. Very much a cludge at the moment */
  std::vector<unsigned int>* extractRoIWords();
  /** dump the cells belonging to an RoI. */
  void dumpROICells();
  void dumpJEMResults();
  /** dump the whole collection of EDM objects */
  void dumpEDM();
  

private: // Private attributes

	std::string   m_TriggerTowerLocation ;
        std::string   m_JetElementLocation ;
	std::string   m_jemHitsLocation ;
	std::string   m_jemEtSumsLocation ;
	std::string   m_CPMTobRoILocation ;
	std::string   m_emCMXDataLocation ;
	std::string   m_tauCMXDataLocation ;
	std::string   m_JEMTobRoILocation ;
	std::string   m_JetCMXDataLocation ;
        std::string   m_CPMCMXDataLocation ;
        std::string   m_CPCMXTopoDataLocation ; 
        std::string   m_JetCMXTopoDataLocation ; 
        std::string   m_EnergyTopoDataLocation ; 
        std::string   m_EmTauCTPLocation ; 
        std::string   m_JetCTPLocation ; 
        std::string   m_EnergyCTPLocation ;

  /** contains the external ROIs that will be saved
  to the TES */
  DataVector<EmTauROI>* m_VectorOfEmTauROIs;


	/** locations within the TES to store collections of EmTauROIs*/
        std::string   m_EmTauROILocation ;
	std::string m_actualROIWordLocation ;
        std::string   m_JetROILocation ;

	std::vector<int> m_actualROIWords;
	std::vector<int> m_generatedROIWords;
	std::vector<int> m_eventsWithErrors;
	int m_numberOfErrors;
	int m_numberOfEvents;
	int m_eventNumber;
	int m_mode;

  std::string m_EmTauSlinkLocation ;
  
  ToolHandle<LVL1::IL1EmTauTools> m_EmTauTool;
  ToolHandle<LVL1::IL1JetTools> m_JetTool;
  ToolHandle<LVL1::IL1EtTools> m_EtTool;

  /** there are 4 CP RoI RODs which have a Slink cable connected to the RoIB. This array holds pointers to 4
  DataVectors containing the Slink words*/
  const DataVector<LVL1CTP::SlinkWord >* m_emTauSlink[TrigT1CaloDefs::numOfCPRoIRODs];

  static const unsigned int m_dumpTTs            =1;//00001
  static const unsigned int m_dumpEmTauRoIs      =2;//00010
  static const unsigned int m_compareEmTauRoIs   =4;//00100
  static const unsigned int m_compRoIWrdCoords   =8;//01000
  static const unsigned int m_dumpRoICells       =16;//10000
  static const unsigned int m_dumpJEMResults     =32;//100000
  static const unsigned int m_testEtTool         =64;//100000
  static const unsigned int m_testJetTool        =128;//100000
  static const unsigned int m_dumpEDM            =256;
  
};

} // end of namespace bracket


#endif











