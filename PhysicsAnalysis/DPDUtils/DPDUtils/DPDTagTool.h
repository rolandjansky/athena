/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DPDTAGTOOL_H
#define DPDTAGTOOL_H
/**
 * @author Tulay Cuhadar Donszelmann
 * @author Balint Radics 
 * @date Jan 2009
 */

#include <string>
#include <map>

#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"
#include "GaudiKernel/ServiceHandle.h"

//////////////////////////////////////////////////////////////////////////////


// Interface ID for DPDTagTool
static const InterfaceID IID_DPDTagTool("DPDTagTool", 1, 0);

class DPDTagTool : public AthAlgTool {

 public:

  // Standard Constructor
  DPDTagTool(const std::string&, const std::string&, const IInterface*);

  // AlgTool and IAlgTool interface methods
  static const InterfaceID& interfaceID() { return IID_DPDTagTool; };

  // Overriding initialize, finalize and execute
  virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>&, const int);
  virtual StatusCode execute(TagFragmentCollection&, const int);
  virtual StatusCode stop();
  virtual StatusCode finalize();

 private:
  ServiceHandle<StoreGateSvc> m_pMetaDataStore;

  std::string m_desdskimdecisioncontainer;
  std::string m_daodskimdecisioncontainer;

};

#endif // DPDTAGTOOL_H
