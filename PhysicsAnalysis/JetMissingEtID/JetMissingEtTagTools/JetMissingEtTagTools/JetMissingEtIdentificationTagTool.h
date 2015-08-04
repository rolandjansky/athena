/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMISSINGETIDENTIFICATIONTAGTOOL_H 
#define JETMISSINGETIDENTIFICATIONTAGTOOL_H 

/*****************************************************************************
Name    : JetMissingEtIdentificationTagTool.h
Package : offline/PhysicsAnalysis/JetMissingEtID/JetMissingEtTagTools
Purpose : build the JetMissingEtIdentification Tag object - AnalysisTag.h. 
	  The JetMissingEtIdentification Analysis Tag fragment is built here
          Jet identification method and MissingEt Calculation detailed information 
          can be encoded in this word
*****************************************************************************/

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
//#include "StoreGate/StoreGateSvc.h"
#include "TagEvent/TagFragmentCollection.h"
 #include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

/** Interface ID for JetMissingEtIdentificationTagTool*/  
static const InterfaceID IID_JetMissingEtIdentificationTagTool("JetMissingEtIdentificationTagTool", 1, 0);
class IJetCalibrationTool;

class JetMissingEtIdentificationTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  JetMissingEtIdentificationTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_JetMissingEtIdentificationTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& jetMissingEtTagCol, const int max);
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~JetMissingEtIdentificationTagTool( );

private:
   
   /** Properties */
   std::string m_jetContainerName;
   std::string m_missingEtObjectName;
   double m_badjetPtCut;
   bool   m_useEMScale; //Emergency fix for HI (W.L, 22.11.2010)
   /** Event Store */
   
};

#endif // JETMISSINGETIDENTIFICATIONTAGTOOL_H

