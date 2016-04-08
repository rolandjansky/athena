/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIGGSPHYSTAGTOOL_H 
#define HIGGSPHYSTAGTOOL_H 

/*****************************************************************************
Name    : HiggsPhysTagTool.h
Package : offline/PhysicsAnalysis/HiggsPhys/HiggsPhysTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the HiggsPhys Tag object - AnalysisTag.h. 
	  The HiggsPhys Analysis Tag fragment is built here
          For example encoding the results of hypotheses on different channels
*****************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <map>

/** Interface ID for HiggsPhysTagTool*/  
static const InterfaceID IID_HiggsPhysTagTool("HiggsPhysTagTool", 1, 0);

class HiggsPhysTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  HiggsPhysTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_HiggsPhysTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& higgsTagCol, const int max);  
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~HiggsPhysTagTool( );

private:

  /** Properties */
  std::string m_jetContainerName;
  double m_jetPtCut;
 };

#endif // HIGGSPHYSTAGTOOL_H



