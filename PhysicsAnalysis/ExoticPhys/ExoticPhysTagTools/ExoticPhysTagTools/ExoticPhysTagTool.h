/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXOTICPHYSTAGTOOL_H 
#define EXOTICPHYSTAGTOOL_H 

/*****************************************************************************
Name    : ExoticPhysTagTool.h
Package : offline/PhysicsAnalysis/ExoticPhys/ExoticPhysTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the ExoticPhys Tag object - AnalysisTag.h. 
	  The ExoticPhys Analysis Tag fragment is built here
          For example encoding the results of hypotheses on different channels
*****************************************************************************/

#include "GaudiKernel/AlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <map>

/** Interface ID for ExoticPhysTagTool*/  
static const InterfaceID IID_ExoticPhysTagTool("ExoticPhysTagTool", 1, 0);

class ExoticPhysTagTool : public AlgTool {

public:
  
  /** Standard Constructor */
  ExoticPhysTagTool(const std::string& type, const std::string& name, const IInterface* 
parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_ExoticPhysTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& exoticTagCol, const int max);
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~ExoticPhysTagTool( );

private:

  /** Properties */
  std::string m_jetContainerName;
  double m_jetPtCut;

  /** Event Store */
  StoreGateSvc* m_storeGate;

 };

#endif // EXOTICPHYSTAGTOOL_H



