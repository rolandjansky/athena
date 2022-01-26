/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_STGCPREPDATARETRIEVER_H
#define JIVEXML_STGCPREPDATARETRIEVER_H

#include "JiveXML/IDataRetriever.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include <string>

namespace JiveXML {
  
  class sTgcPrepDataRetriever : virtual public IDataRetriever, public AthAlgTool {
    
  public:
    
    /// Standard Constructor
    sTgcPrepDataRetriever(const std::string& type, const std::string& name, const IInterface* parent);

     /// Retrieve all the data
    virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool);

    /// Return the name of the data type
    virtual std::string dataTypeName() const { return "STGC"; };

    ///Default AthAlgTool methods
    StatusCode initialize();

  private:

    ///The storegate key for the STGC collection
    SG::ReadHandleKey<Muon::sTgcPrepDataContainer> m_sgKey{this, "StoreGateKey", "STGC_Measurements", "Name of the sTGCPrepDataContainer"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  };
  
}
#endif
