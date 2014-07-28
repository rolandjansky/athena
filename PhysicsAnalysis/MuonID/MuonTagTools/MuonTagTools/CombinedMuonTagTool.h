/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#ifndef COMBINEDMUONTAGTOOL_H 
//#define COMBINEDMUONTAGTOOL_H 

/*****************************************************************************
Name    : CombinedMuonTagTool.h
Package : offline/PhysicsAnalysis/MuonID/MuonTagTools
Author  : Ketevi A. Assamagan
Created : January 2006
Purpose : build the CombinedMuon Tag object - AnalysisTag.h. 
	  The CombinedMuon Analysis Tag fragment is built here
          MOORE and Muonboy information can be encoded in this word
*****************************************************************************/
/*
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/AlgTool.h"
#include "TagEvent/TagFragmentCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeSpecification.h"

#include "StoreGate/StoreGateSvc.h"

#include <vector>
#include <map>

namespace Analysis {
  class MuonContainer;
}

//namespace xAOD {
//  class MuonContainer;
//}
*/
/** Interface ID for CombinedMuonTagTool*/  
/*static const InterfaceID IID_CombinedMuonTagTool("CombinedMuonTagTool", 1, 0);

class CombinedMuonTagTool : public AlgTool {

public:
*/
  /** Standard Constructor */
// CombinedMuonTagTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
//  static const InterfaceID& interfaceID( ) { return IID_CombinedMuonTagTool; };

  /** Overriding initialize, finalize and execute */
/* virtual StatusCode initialize();
  virtual StatusCode attributeSpecification(std::map<std::string,AthenaAttributeType>& attrMap, const int max);
  virtual StatusCode execute(TagFragmentCollection& muonIdTagCol, const int max);
  virtual StatusCode finalize();

protected:
*/
   /** Standard destructor */
/*   virtual ~CombinedMuonTagTool( );

private:
*/
  /** Properties */
/* std::vector<std::string> m_containerNames;
  double m_muonPtCut;
  bool m_authorBased;
*/
  /** Event Store */
// ServiceHandle<StoreGateSvc> m_storeGate;

  //xAOD::MuonContainer * m_userMuonContainer;
// Analysis::MuonContainer * m_userMuonContainer;

//};

//#endif // MUONTAGTOOL_H


