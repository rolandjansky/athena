/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          DumpTriggerObjects.h  -  description
                             -------------------
    begin                : Wed Dec 13 2000
    copyright            : (C) 2000 by moyse
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef TRIGT1CALO_DumpTriggerObjects
#define TRIGT1CALO_DumpTriggerObjects

// Utilities
#include <cmath>

// STL
#include <string>
#include <vector>

// Athena/Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "DataModel/DataVector.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

//#include "StoreGate/DataHandle.h"

#include <fstream>

//TrigT1Calo includes
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"

#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/JetElement_ClassDEF.h"
 

//default for parameters
#define DEFAULT_TriggerTowerLocation    "/Event/LVL1TriggerTowers"
#define DEFAULT_JetElementLocation      "/Event/LVL1JetElements"

 /**LVL1 namespace.
 This belongs to the TrigT1Calo atrig simulation.
 */
namespace LVL1 {

/**
 The DumpTriggerObjects algorithm shows how to access EmTauROIs.
*/

class DumpTriggerObjects : public Algorithm
{

 public:
 
  typedef const DataVector<JetElement>               JECollection ;
  typedef const DataVector<TriggerTower>               TTCollection ;

  //-------------------------
  // Constructors/Destructors
  //
  // Athena requires that the constructor takes certain arguments
  // (and passes them directly to the constructor of the base class)
  //-------------------------

  DumpTriggerObjects( const std::string& name, ISvcLocator* pSvcLocator ) ;

  virtual ~DumpTriggerObjects();


  //------------------------------------------------------
  // Methods used by Athena to run the algorithm
  //------------------------------------------------------

  StatusCode initialize() ;
  StatusCode execute() ;
  StatusCode finalize() ;



private: // Private methods
StatusCode loadTriggerTowers();

//returns Phi bin number
int Phi2Bin(double phi);

//returns Eta bin number
int JEEta2Bin(double eta);

//Fills a 2D array with JetElement data
void JELoadArray();

//Prints out array values in JetElement format
void JEDumpOutput();

//returns eta bin number
int TTEta2Bin(double eta);

//Fills two 2D arrays with Electromagnetic and Hadronic data
void TTLoadArray();

//Prints out array values in TriggerTower format
void TTDumpOutput();

//Creats a new data array
int **InitialiseArray();

//Destroys a data array
int **FinaliseArray(int **data);

//Prints out Highest TT unit based on sum of energies
void TTHighestOutput();

//Prints out highest JetElement unit based on sum of energies
void JEHighestOutput();

//Prints a 4x4 TriggerTower data block
void printTTUnit(std::ofstream *output, int eta, int phi);

//Prints a 4x8 JetElement data block followed by 4 figures.
void printJEUnit(std::ofstream *output, int eta, int phi);

//Prints out the whole array
void DumpArray(int **data, const char *str);

private: // Private attributes

     /**  this map object holds the trigger towers :
the towers each have a "key" (in this case
an integer) which uniquely identifies them.  */
   std::map<int, TriggerTower *> * m_ttContainer;
   std::map<int, JetElement *> * m_jetElementContainer;
  
    /** pointer to TTs stored in storegate*/
   const TTCollection* m_storedTTs;
   const JECollection* jetElements;
   ServiceHandle<StoreGateSvc> m_storeGate;

   std::string   m_triggerTowerLocation ;
   std::string   m_jetElementLocation ;
   std::ofstream* m_oFile;

  /**  this map object holds the trigger towers :
  the towers each have a "key" (in this case
  an integer) which uniquely identifies them.  */

  int **EMData;
  int **HData;  
  int PhiMax;	//Maximum number of Phi bins
  int EtaMax;   //Maximum number of Eta bins
};

} // end of namespace bracket


#endif











