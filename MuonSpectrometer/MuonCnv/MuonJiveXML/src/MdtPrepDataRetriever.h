/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_MDTPREPDATARETRIEVER_H
#define JIVEXML_MDTPREPDATARETRIEVER_H

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"

#include <string>

namespace JiveXML {
  
  class MdtPrepDataRetriever : virtual public IDataRetriever, public AthAlgTool {
    
  public:
    
    /// Standard Constructor
    MdtPrepDataRetriever(const std::string& type, const std::string& name, const IInterface* parent);
  
    /// Retrieve all the data
    virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool);

    /// Return the name of the data type
    virtual std::string dataTypeName() const { return "MDT"; };

    ///Default AthAlgTool methods
    StatusCode initialize();

  private:

    ///The storegate key for the MDT collection
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_sgKey{this, "StoreGateKey", "MDT_DriftCircles", "Name of the MdtPrepDataContainer"};

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    int m_adcCut;
    bool m_obeyMasked;
  };
  
}
#endif
