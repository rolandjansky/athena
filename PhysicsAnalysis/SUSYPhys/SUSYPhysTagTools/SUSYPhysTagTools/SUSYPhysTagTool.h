/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SUSYPHYSTAGTOOL_H 
#define SUSYPHYSTAGTOOL_H 

/*****************************************************************************
Name    : SUSYPhysTagTool.h
Package : offline/PhysicsAnalysis/SUSYPhys/SUSYPhysTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the SUSYPhys Tag object - AnalysisTag.h. 
	  The SUSYPhys Analysis Tag fragment is built here
          For example encoding the results of hypotheses on different channels
*****************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <map>

/** Interface ID for SUSYPhysTagTool*/  
static const InterfaceID IID_SUSYPhysTagTool("SUSYPhysTagTool", 1, 0);

class SUSYPhysTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  SUSYPhysTagTool(const std::string& type, const std::string& name, const IInterface* 
parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_SUSYPhysTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string, AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& susyTagCol, const int max);
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~SUSYPhysTagTool( );

private:

  /** Properties */
  std::string m_jetContainerName;
  double m_jetPtCut;
 };

#endif // BPHYSTAGTOOL_H



