/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

///////////////////////// -*- C++ -*- /////////////////////////////
// LumiCalcSvc.h 
// Header file for class LumiCalcSvc
// Author: B.Radics<radbal@cern.ch>
//         based on Richard Hawkings' LumiCalc.py
/////////////////////////////////////////////////////////////////// 
#ifndef LUMIBLOCKCOMPS_LUMICALCSVC_H
#define LUMIBLOCKCOMPS_LUMICALCSVC_H

/**
 * @class LumiCalcSvc
 * @brief This implementes the methods for ILumiCalcSvc
 */

// Gaudi/Athena access
//#include "LumiBlockComps/ILumiCalcSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h" 
#include "AthenaBaseComps/AthService.h"
#include "LumiCalc/LumiCalculator.h"
#include "LumiBlockComps/ILumiCalcSvc.h"

class TTree;
class ILumiCalcSvc;
class StoreGateSvc;

class LumiCalcSvc : public extends<AthService, ILumiCalcSvc, IIncidentListener>
{

public: 

  /// Constructor
  LumiCalcSvc(const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~LumiCalcSvc();

  /// Gaudi Service Implementation
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode stop() override;
  //@}

  /// Incident service handle listening for BeginInputFile and EndInputFile.
  virtual void handle(const Incident& incident) override;

  // print some values (inst lumi, lumiblock number, time, prescales, etc.) to screen
  virtual void print() override {};

  // main user function
  // By default tries both "LumiBlocks" and "IncompleteLumiBlocks"
  virtual StatusCode calcLumi() override;

  // retrieve the details of the calculation as a TTree table
  virtual TTree * getLumiTree() override { return m_LumiTree; }

  /// register trigger
  virtual
  bool registerLBCollection(const TString& tname, const TString& regexpr, const std::list<TString>& trigpar) override;

private:
  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  StatusCode doDbQuery(StoreGateSvc_t sg);
  void doRecordTree(bool b){m_recordTTree = b;}
  void printTree();


  StoreGateSvc_t m_pMetaDataStore;
  StoreGateSvc_t m_pInputStore;

  //  StoreGateSvc* p_inputstore; // input metadata store
  //  StoreGateSvc* p_metadatastore; // metadata store
  ITHistSvc * m_tHistSvc;

  // Configurable Properties
  StringArrayProperty m_triggers;
  StringArrayProperty m_lbcnames;
  StringProperty m_sourcedb;
  StringProperty m_parlumiestfolder;
  StringProperty m_parlumilvl1folder;
  StringProperty m_parlumihltfolder;
  StringProperty m_parlvl1prescalesfolder;
  StringProperty m_parlvl1menufolder;
  StringProperty m_parhltmenufolder;
  StringProperty m_UseLumiTag;
  StringProperty m_UseLumiMethod;
  int m_UseLumiChannel;
  BooleanProperty m_UseMC;
  BooleanProperty m_Online;
  BooleanProperty m_Verbose;
  BooleanProperty m_UseInputStore;

  // The Lumicalculator;
  LumiCalculator * m_lumicalc;

  // A TTree to bookkeep the calculation
  TTree * m_LumiTree;
  bool m_recordTTree;

  // Bookkeep if file is partially read
  bool m_fileCurrentlyOpened;

  // lbc trigger combo registry
  std::map< TString, tvtPair > m_registry;
}; 


#endif //> !ATHENAKERNEL_LUMICALCSVC_H

#endif
