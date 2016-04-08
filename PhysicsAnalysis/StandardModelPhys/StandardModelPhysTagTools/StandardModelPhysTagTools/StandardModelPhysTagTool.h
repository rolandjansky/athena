/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STANDARDMODELPHYSTAGTOOL_H 
#define STANDARDMODELPHYSTAGTOOL_H 

/*****************************************************************************
Name    : StandardModelPhysTagTool.h
Package : offline/PhysicsAnalysis/StandardModelPhys/StandardModelPhysTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the StandardModelPhys Tag object - AnalysisTag.h. 
	  The StandardModelPhys Analysis Tag fragment is built here
          For example encoding the results of hypotheses on different channels
*****************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <map>

/** Interface ID for StandardModelPhysTagTool*/  
static const InterfaceID IID_StandardModelPhysTagTool("StandardModelPhysTagTool", 1, 0);

class StandardModelPhysTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  StandardModelPhysTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_StandardModelPhysTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& smTagCol, const int max);
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~StandardModelPhysTagTool( );

private:

  /** Properties */
  std::string m_jetContainerName;
  double m_jetPtCut;
 };

#endif // STANDARDMODELPHYSTAGTOOL_H



