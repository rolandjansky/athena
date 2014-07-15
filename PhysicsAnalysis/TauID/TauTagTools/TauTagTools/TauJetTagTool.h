/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUJETTAGTOOL_H 
#define TAUJETTAGTOOL_H 

/*****************************************************************************
Name    : TauJetTagTool.h
Package : offline/PhysicsAnalysis/TauID/TauTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the TauJet Tag objects - TauJetTagCollection.h. 
	  The TauJet Tag fragment is built here
*****************************************************************************/

#include "GaudiKernel/AlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <map>

class StoreGateSvc;

/** Interface ID for TauJetTagTool*/  
static const InterfaceID IID_TauJetTagTool("TauJetTagTool", 1, 0);

class TauJetTagTool : public AlgTool {

public:
  
  /** Standard Constructor */
  TauJetTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_TauJetTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& tauJetTagCol, const int max);
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~TauJetTagTool( );

private:

  /** Properties */
  std::string m_containerName;
  std::string m_tauDetailContainer;
  double m_tauJetPtCut;
  

  /** the attribute names */
  std::vector<std::string> m_ptStr; 
  std::vector<std::string> m_etaStr; 
  std::vector<std::string> m_phiStr; 
  std::vector<std::string> m_ntrkStr; 
  std::vector<std::string> m_pidStr; 

  /** Event Store */
  StoreGateSvc* m_storeGate;

 };

#endif // TAUJETTAGTOOL_H

