/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUIDENTIFICATIONTAGTOOL_H 
#define TAUIDENTIFICATIONTAGTOOL_H 

/*****************************************************************************
Name    : TauIdentificationTagTool.h
Package : offline/PhysicsAnalysis/TauID/TauTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the TauIdentification Tag object - AnalysisTag.h. 
	  The TauIdentification Analysis Tag fragment is built here
          Different tau identification method information can be encoded in this word
*****************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <map>

/** Interface ID for TauIdentificationTagTool*/  
static const InterfaceID IID_TauIdentificationTagTool("TauIdentificationTagTool", 1, 0);

class TauIdentificationTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  TauIdentificationTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_TauIdentificationTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string, AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& tauIdTagCol, const int max);
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~TauIdentificationTagTool( );

private:

  /** Properties */
  std::string m_tauJetContainerName;
  double m_tauJetPtCut;
  double m_likelihood;
  double m_hadEnergyFrac;
 };

#endif // TAUIDENTIFICATIONTAGTOOL_H

