/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_CALOHECRETRIEVER_H
#define JIVEXML_CALOHECRETRIEVER_H

#include <string>
#include <vector>
#include <cstddef>
#include <map>

#include "CaloIdentifier/CaloCell_ID.h"
#include "LArTools/LArCablingService.h"

#include "JiveXML/IDataRetriever.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

class IToolSvc;

class CaloIdManager;
class Identifier;
class CaloCellContainer;

namespace JiveXML{
  
  /**
   * @class CaloHECRetriever
   * @brief Retrieves all @c Calo Cluster @c objects 
   *
   *  - @b Properties
   *    - StoreGateKey: default is 'AllCalo'. Don't change.
   *	- HEClCellThreshold: default is 50 (MeV)
   *	- RetrieveHEC: general flag, default is true
   *    - DoHECCellDetails: default is false
   *    - CellConditionCut: default is false
   *    - LArChannelsToIgnoreM5: default is empty (none ignored). Input: vector of cells
   *   	- DoMaskLArChannelsM5: default is false (none masked)
   *   	- CellEnergyPrec: precision in int, default is 3 digits
   *   	- CellTimePrec: precision in int, default is 3 digits
   *   
   *  - @b Retrieved @b Data
   *    - location in phi and eta
   *    - numCells: number of cells in each cluster
   *    - cells: identifier and adc counts of each cell 
   */
  class CaloHECRetriever : virtual public IDataRetriever,
                                   public AthAlgTool {
    
    public:
      
      /// Standard Constructor
      CaloHECRetriever(const std::string& type,const std::string& name,const IInterface* parent);
      
      /// Retrieve all the data
      virtual StatusCode retrieve(ToolHandle<IFormatTool> FormatTool); 
      const DataMap getHECData(const CaloCellContainer* cellContainer);

      /// Return the name of the data type
      virtual std::string dataTypeName() const { return typeName; };

	
      ///Default AthAlgTool methods
      StatusCode initialize();

    private:
      ///The data type that is generated by this retriever
      const std::string typeName;

      const CaloIdManager* m_calo_id_man;
      const CaloCell_ID*   m_calocell_id;
      LArCablingService* m_larCablingSvc;
    
      /// for properties
      std::string m_sgKey; 
      double m_cellThreshold;
      int m_cellEnergyPrec;
      int m_cellTimePrec;
      bool m_hec;
      bool m_doHECCellDetails;
      bool m_cellConditionCut;
      std::vector<Identifier::value_type> m_LArChannelsToIgnoreM5;
      bool m_doMaskLArChannelsM5;
      bool m_doBadHEC; 
      IToolSvc* m_toolSvc;
  };
}
#endif
