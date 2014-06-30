/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RAWINFOSUMMARYFORTAGTOOL_H 
#define RAWINFOSUMMARYFORTAGTOOL_H 

/*****************************************************************************
Name    : RawInfoSummaryForTagTool.h
Package : offline/PhysicsAnalysis/EventTag/EventTagUtils
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : Tool to build the data quality tag
*****************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include <map>

/** Interface ID for RawInfoSummaryForTagTool*/  
static const InterfaceID IID_RawInfoSummaryForTagTool("RawInfoSummaryForTagTool", 1, 0);

class RawInfoSummaryForTagTool : public AthAlgTool {

public:
  
  /** Standard Constructor */
  RawInfoSummaryForTagTool(const std::string& type, const std::string& name, const IInterface* 
parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_RawInfoSummaryForTagTool; };

  /** Overriding initialize, finalize and execute */
  virtual StatusCode initialize();
  //virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap);
  virtual StatusCode attributeSpecification(std::vector<AthenaAttributeSpecification>& attrMap);
  virtual StatusCode execute(TagFragmentCollection& dataQualityTagCol);  
  virtual StatusCode finalize();

protected:

   /** Standard destructor */
   virtual ~RawInfoSummaryForTagTool( );

private:

  /** Properties */
  std::string m_dataObjectName;

  /** Event Store */
  StoreGateSvc* m_storeGate;

 };

#endif // RAWINFOSUMMARYFORTAGTOOL_H



