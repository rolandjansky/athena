/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUJETTAGTOOL_H 
#define TAUJETTAGTOOL_H 

/*****************************************************************************
Name    : TauJetTagTool.h
Package : offline/PhysicsAnalysis/TauID/TauTagTools
Purpose : build the TauJet Tag objects - TauJetTagCollection.h. 
	  The TauJet Tag fragment is built here
*****************************************************************************/

// Framework includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <map>

/** Interface ID for TauJetTagTool*/  
static const InterfaceID IID_TauJetTagTool("TauJetTagTool", 1, 0);

namespace TauAnalysisTools {
  class ITauSelectionTool;
}

class TauJetTagTool : public AthAlgTool {

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
  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_eleBDTLoose;
  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_eleBDTMedium;
  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_eleBDTTight;
  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_muonVeto;
  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_jetBDTLoose;
  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_jetBDTMedium;
  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_jetBDTTight;
  // ToolHandle<TauAnalysisTools::ITauSelectionTool> m_jetLLHLoose;
  // ToolHandle<TauAnalysisTools::ITauSelectionTool> m_jetLLHMedium;
  // ToolHandle<TauAnalysisTools::ITauSelectionTool> m_jetLLHTight;
  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_eleOLR;

  double m_tauJetPtCut;

  std::string m_containerName;

  /** the attribute names */
  std::vector<std::string> m_ptStr; 
  std::vector<std::string> m_etaStr; 
  std::vector<std::string> m_phiStr; 
  std::vector<std::string> m_ntrkStr; 
  std::vector<std::string> m_pidStr; 
 };

#endif // TAUJETTAGTOOL_H

