/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_CSCPREPDATARETRIEVER_H
#define JIVEXML_CSCPREPDATARETRIEVER_H

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"

#include <string>

namespace JiveXML {
  
  class CscPrepDataRetriever : virtual public IDataRetriever, public AthAlgTool {

  public:
    /// Standard Constructor
    CscPrepDataRetriever(const std::string& type, const std::string& name, const IInterface* parent);

    /// Retrieve all the data
    virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool);

    /// Return the name of the data type
    virtual std::string dataTypeName() const { return "CSCD"; };

    ///Default AthAlgTool methods
    virtual StatusCode initialize();
    
  private:
    ///The storegate key for the CSC collection
    SG::ReadHandleKey<Muon::CscPrepDataContainer> m_sgKey{this, "StoreGateKey", "CSC_Clusters", "Name of the CscPrepDataContainer"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  };
  
}
#endif
