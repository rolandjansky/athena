/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_MDTPREPDATARETRIEVER_H
#define JIVEXML_MDTPREPDATARETRIEVER_H

#include <string>

#include "JiveXML/IDataRetriever.h"

#include "AthenaBaseComps/AthAlgTool.h"

class MdtIdHelper;

namespace JiveXML {
  
  class MdtPrepDataRetriever : virtual public IDataRetriever, public AthAlgTool {
    
  public:
    
    /// Standard Constructor
    MdtPrepDataRetriever(const std::string& type, const std::string& name, const IInterface* parent);
  
    /// Retrieve all the data
    virtual StatusCode retrieve(ToolHandle<IFormatTool> FormatTool);

    /// Return the name of the data type
    virtual std::string dataTypeName() const { return m_typeName; };

    ///Default AthAlgTool methods
    StatusCode initialize();

  private:
   
   ///The data type that is generated by this retriever
    const std::string m_typeName;

    ///The storegate key for the CSC collection
    std::string m_sgKey;

    /// MDT identifier helper
    const MdtIdHelper* m_mdtIdHelper;

    int m_adcCut;
    bool m_obeyMasked;
  };
  
}
#endif
