/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEAVYIONPHYSTAGTOOL_H 
#define HEAVYIONPHYSTAGTOOL_H 

/*****************************************************************************
Name    : HeavyIonPhysTagTool.h
Package : offline/PhysicsAnalysis/HeavyIonPhys/HeavyIonPhysTagTools
Author  : Ketevi A. Assamagan
Created : June 2006
Purpose : build the HeavyIonPhys Tag object - AnalysisTag.h. 
	  The HeavyIonPhys Analysis Tag fragment is built here
          For example encoding the results of hypotheses on different channels
*****************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "TagEvent/TagFragmentCollection.h"

#include <map>

/** Interface ID for HeavyIonPhysTagTool*/  
static const InterfaceID IID_HeavyIonPhysTagTool("HeavyIonPhysTagTool", 1, 0);

class HeavyIonPhysTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  HeavyIonPhysTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_HeavyIonPhysTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& heavyIonTagCol, const int max);
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~HeavyIonPhysTagTool( );

private:

  /** Properties */
  std::string m_jetContainerName;
  double m_jetPtCut;
 };

#endif // HEAVYIONPHYSTAGTOOL_H

