
 /***************************************************************************
                           ReprocessTriggerTowers.h  -  description
                              -------------------
     begin                : Tue May 18 2010
     copyright            : (C) 2010 by Alan Watson
     email                : Alan.Watson@CERN.CH
  ***************************************************************************/

 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/
 #ifndef ReprocessTriggerTowers_H
 #define ReprocessTriggerTowers_H

 // STL
 #include <string>
 #include <vector>

 // Athena/Gaudi
 #include "AthenaBaseComps/AthAlgorithm.h"
 #include "GaudiKernel/ServiceHandle.h"
 #include "GaudiKernel/ToolHandle.h"
 
 #include "AthContainers/DataVector.h"
 #include "GaudiKernel/DataSvc.h"
 #include "StoreGate/StoreGateSvc.h"

// LVL1 Calo Trigger includes
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
// #include "TrigT1CaloCalibTools/L1CaloTTIdTools.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

 namespace LVL1 {

   //Doxygen Class definition below:
   /**
  The algorithm responsible for simulating the Em/tau calo trigger.
   */
 class ReprocessTriggerTowers : public AthAlgorithm, public IIncidentListener
 {
  public:

   //-------------------------
   // Constructors/Destructors
   //
   // Athena requires that the constructor takes certain arguments
   // (and passes them directly to the constructor of the base class)
   //-------------------------

   ReprocessTriggerTowers( const std::string& name, ISvcLocator* pSvcLocator ) ;


   //------------------------------------------------------
   // Methods used by Athena to run the algorithm
   //------------------------------------------------------

   StatusCode initialize() ;
   StatusCode execute() ;
   StatusCode finalize() ;

  virtual void handle(const Incident&);

 private: // Private methods
    
   /** Simulate PreProcessor with user-defined settings */
   void preProcess();
   
   /** write output TriggerTowerCollection to TES. */
   void store();
   
  /** Compute L1CaloCoolChannelId (including support for old geometries) */
  L1CaloCoolChannelId channelId(double eta, double phi, int layer);

 private: // Private attributes

  /** Pointers to tools and services */
  ToolHandle<IL1TriggerTowerTool> m_TTtool;
  ToolHandle<IL1CaloMappingTool> m_mappingTool; 
  //L1CaloCondSvc* m_l1CondSvc;

   /** Where to find the TriggerTowers */
   std::string   m_InputTriggerTowerLocation ;
   /** Locations of outputs in StoreGate */
   std::string   m_OutputTriggerTowerLocation ;

  /** Maps to hold customized tower parameters */
  std::map<unsigned int, unsigned int> m_channelNoiseCuts;
  std::map<unsigned int, float> m_channelScales;

  /** The input and output TriggerTowers */
  const TriggerTowerCollection* m_inputTowers;
  DataVector<TriggerTower>* m_outputTowers;
  
  /** Local control flags */
  bool m_ZeroSuppress;
  bool m_doBCID;
  int  m_LUTStrategy;
  
  /** User-adjustable parameters */
  unsigned int m_emThresh;
  unsigned int m_hadThresh;
  
  static const int m_Regions = 6;
  enum Region { EMB=0, EMEC=1, FCAL1=2, Tile=3, HEC=4, FCAL23=5 };
  static const int m_nEMB = 15;
  static const int m_nEMEC = 14;
  static const int m_nFCAL1 = 4;
  static const int m_nTile = 15;
  static const int m_nHEC = 14;
  static const int m_nFCAL23 = 4;
  static const int m_numTowers[m_Regions];
  std::vector< std::vector<int> > m_Thresholds;
  std::vector< std::vector<float> > m_Rescale;
    
};

} // end of namespace bracket

 #endif












