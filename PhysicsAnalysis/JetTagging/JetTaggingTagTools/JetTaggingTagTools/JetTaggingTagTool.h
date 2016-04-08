/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGGINGTAGTOOL_H 
#define JETTAGGINGTAGTOOL_H 

/*****************************************************************************
Name    : JetTaggingTagTool.h
Package : offline/PhysicsAnalysis/JetTagging/JetTaggingTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the Flavor Tagging Event Tag object - AnalysisTag.h. 
	  The JetTagging Analysis Tag fragment is built here
          For example encoding the results of hypotheses on different channels
*****************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <map>

/** Interface ID for JetTaggingTagTool*/  
static const InterfaceID IID_JetTaggingTagTool("JetTaggingTagTool", 1, 0);

class JetTaggingTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  JetTaggingTagTool(const std::string& type, const std::string& name, const IInterface* 
parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_JetTaggingTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& tauIdTagCol, const int max);
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~JetTaggingTagTool( );

private:

  /** Properties */
  std::string m_jetContainerName;
  double m_jetPtCut;
 };

#endif // JETTAGGINGTAGTOOL_H



