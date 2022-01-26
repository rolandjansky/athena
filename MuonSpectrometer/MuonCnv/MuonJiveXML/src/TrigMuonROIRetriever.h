/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_TRIGMUONROIRETRIEVER_H
#define JIVEXML_TRIGMUONROIRETRIEVER_H

#include <string>

#include "JiveXML/IDataRetriever.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"


namespace JiveXML{

  class TrigMuonROIRetriever : virtual public IDataRetriever, public AthAlgTool {

  public:

    /// Standard Constructor
    TrigMuonROIRetriever(const std::string& type, const std::string& name, const IInterface* parent);

    /// Retrieve all the data
    virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool);

    /// Return the name of the data type
    virtual std::string dataTypeName() const { return "TrigMuonROI"; };

  private:
    ///The storegate key for the LVL1 Muon RoIs
    SG::ReadHandleKey<LVL1_ROI> m_sgKey{this, "StoreGateKey", "LVL1_ROI", "Storegate key for LVL1 Muon RoIs"};
  };

}
#endif
