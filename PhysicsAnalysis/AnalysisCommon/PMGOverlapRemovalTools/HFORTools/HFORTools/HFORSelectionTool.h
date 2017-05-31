/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HFORTOOLS_HFORSELECTIONTOOL_H
#define HFORTOOLS_HFORSELECTIONTOOL_H 1

///
///Example usage code:
///Athena only:
///To configure the tool, add the following to your joboptions:
/// myTool = CfgMgr.HFORSelectionTool("ToolInstanceName",Property=4.0)
/// import ROOT
/// myTool.ENumProperty = ROOT.HFORSelectionTool.Val2
///To use the tool in your algorithm (can use regular ToolHandle or asg::AnaToolHandle):
/// #include "HFORTools/IHFORSelectionTool.h"
/// ToolHandle<IHFORSelectionTool> myTool("HFORSelectionTool/ToolInstanceName")
/// CHECK( myTool.retrieve() );
/// myTool->isSelected(...); //or whatever methods are implemented in IHFORSelectionTool
///
///Dual use: Alternative for c++ configuration and usage (with or without athena):
/// #include "AsgTools/AnaToolHandle.h"
/// #include "HFORTools/IHFORSelectionTool.h"
/// asg::AnaToolHandle<IHFORSelectionTool> myTool("HFORSelectionTool/ToolInstanceName");
/// myTool.setProperty( "Property", 4.0 );
/// myTool.setProperty( "ENumProperty",
/// myTool.initialize();
/// myTool->isSelected(....); //or whatever methods are implemented in IHFORSelectionTool

#include <vector>
#include <map>
#include <string>

#include "AsgTools/AsgMetadataTool.h"

#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEventAuxContainer.h"

#include "HFORTools/IHFORSelectionTool.h"
#include "HFORTools/HFOR_Truth.h"

class HFORSelectionTool: public asg::AsgMetadataTool, public virtual IHFORSelectionTool {
 public:
  //constructor for athena can be created using special macro
  //Note: if you add a second interface to your tool, you must use: ASG_TOOL_CLASS2( ToolName, Interface1, Interface2)
  ASG_TOOL_CLASS( HFORSelectionTool , IHFORSelectionTool )

	HFORSelectionTool( const std::string& name );

  //Initialize the tool
  virtual StatusCode  initialize() override;

  // Make sure we clean the the configuration information, forcing the call to
  // setSampleType when the next file is opened.
  virtual StatusCode beginInputFile() override ;

  //Called at the end of file processing, puts out some book keeping
  //information and clear the bookkeeping counters
  virtual StatusCode endInputFile() override ;

  //This method will signal the event to be kept (true) or to be removed (false)
  //based on the overlap removal result
  virtual bool isSelected() override;

  //Helper to get back the current sample type
  HFORType     getSampleType() override ;

  //Helper to get back the name of the current sample (light, bb, cc, c or unknown)
  std::string  getSampleName() override ;

  // Return a string with the LAST type of event classification
  //(bb, cc, c, light, kill or unknown)
  HFORType     getDecisionType() override ;

 private:
  //Sets the sample type based on the sample DSID
  StatusCode   setSampleType() ;

  double            m_matchCone  ;
  std::string       m_truthJetCollectionName; //used in JetBasedHFOR
  HFORType          m_sampleType ;
  HFORType          m_decisionType ;
  unsigned long int m_sampleRunNumber  ;
  std::string       m_sampleName ;

  unsigned long int m_evtCounterAll ;
  unsigned long int m_evtCounterKilled ;

  HFOR_Truth        m_hforTruth ;

  std::string       m_runConfigFile ;
  std::string       m_HFORStrategy ;
  bool              m_isConfigured ;
};


#endif //> !HFORTOOLS_HFORSELECTIONTOOL_H
