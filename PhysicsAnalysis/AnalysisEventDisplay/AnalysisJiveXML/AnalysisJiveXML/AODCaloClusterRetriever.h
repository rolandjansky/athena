/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_AODCALOCLUSTERRETRIEVER_H
#define JIVEXML_AODCALOCLUSTERRETRIEVER_H

#include <string>
#include <vector>
#include <map>

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"

class CaloClusterContainer;

namespace JiveXML{
  
  /**
   * @class AODCaloClusterRetriever
   * @brief Retrieves all @c Calo Cluster @c objects 
   *
   *  - @b Properties
   *    - FavouriteJetCollection
   *    - OtherJetCollections
   *    - DoWriteHLT
   *
   *  - @b Retrieved @b Data
   *    - Usual four-vector: phi, eta, et
   *    - No cells in AOD Clusters. numCells is placeholder
   *      for compatibility with 'full' clusters in AtlantisJava
   */
  class AODCaloClusterRetriever : virtual public IDataRetriever,
                                   public AthAlgTool {
    
    public:
      
      /// Standard Constructor
      AODCaloClusterRetriever(const std::string& type,const std::string& name,const IInterface* parent);
      
      /// Retrieve all the data
      virtual StatusCode retrieve(ToolHandle<IFormatTool> &FormatTool); 
      const DataMap getData(const CaloClusterContainer*, bool calibFlag);

      /// Return the name of the data type
      virtual std::string dataTypeName() const { return m_typeName; };

    private:
      const std::string m_typeName;

      std::string m_sgKeyFavourite;
      std::vector<std::string> m_otherKeys;
      bool m_doWriteHLT;
  };
}
#endif
