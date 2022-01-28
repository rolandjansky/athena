/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_MMPREPDATARETRIEVER_H
#define JIVEXML_MMPREPDATARETRIEVER_H

#include "JiveXML/IDataRetriever.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"

#include <string>

namespace JiveXML {
  
  class MMPrepDataRetriever : virtual public IDataRetriever, public AthAlgTool {
    
  public:
    
    /// Standard Constructor
    MMPrepDataRetriever(const std::string& type, const std::string& name, const IInterface* parent);

     /// Retrieve all the data
    virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool);

    /// Return the name of the data type
    virtual std::string dataTypeName() const { return "MM"; };

    ///Default AthAlgTool methods
    StatusCode initialize();

  private:

    ///The storegate key for the MM collection
    SG::ReadHandleKey<Muon::MMPrepDataContainer> m_sgKey{this, "StoreGateKey", "MM_Measurements", "Name of the MMPrepDataContainer"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  };
  
}
#endif
