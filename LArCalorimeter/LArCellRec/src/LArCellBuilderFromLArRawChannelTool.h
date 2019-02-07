//Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARCELLBUILDERFROMLARRAWCHANNELTOOL_H
#define LARCELLREC_LARCELLBUILDERFROMLARRAWCHANNELTOOL_H
/** 
@class LArCellBuilderFromLArRawChannelTool 
@brief Building LArCell objects from LArRawChannel 


 AlgTool properties (name defined in cxx file): 
     RawChannelsName :  input RawChannelContainer 
     EThreshold:          energy threshold

   Modified:  Dec 4, 2002   Hong Ma 
     Use MakeLArCellFromRaw to make LArCell. 
   Modified: June 2, 2004, David Rousseau : converted to AlgTool
*/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "CaloInterface/ICaloCellMakerTool.h"
#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
//#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "AthAllocators/DataPool.h"
#include "LArCabling/LArOnOffIdMapping.h"


class CaloCellContainer ;
class CaloDetDescrManager ;
class CaloCell_ID;
class CaloCellContainer ;
class LArRawChannelContainer;
class LArCell;

/**
 * @class LArCellBuilderFromLArRawChannelTool
 * @brief An AlgTool class to create a LArCellContainer out of a LArRawChannel container.
 *
 * Inherits from ICaloCellMakerTool and should be called by an instance of the 
 * CaloCellMaker algorithm. 
 * 
 */


class LArCellBuilderFromLArRawChannelTool
  : public extends<AthAlgTool, ICaloCellMakerTool>
{
public:  
  /**
   * @brief Standard AlgTool constructor
   */
  LArCellBuilderFromLArRawChannelTool(const std::string& type, 
				      const std::string& name, 
				      const IInterface* parent);

  /**
   * @brief Destructor, deletes the MsgService.
   */
  virtual ~LArCellBuilderFromLArRawChannelTool();

  /**
   * @brief Initialize method.
   * @return Gaudi status code.
   *
   * Initialazes pointers to servies and private members variable.
   * Sets the m_subCalo variable based on the m_lArRegion jobOption.
   * Computes the total number of cells based on the subcalo hash-range. 
   *
   */
  virtual StatusCode initialize() override;

  /**
   * @brief process method as defined in ICaloCellMaker interface
   * @param theCellContainer Pointer to the CaloCellContainer we are working on
   * @param ctx The event context.
   * @return Gaudi status code.
   *
   */
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;

private: 

  SG::ReadHandleKey<LArRawChannelContainer>   m_rawChannelsKey;        //!< rdo container name (jO)
  bool            m_addDeadOTX;             //!< activate addition of missing cells from dead OTX
  int             m_initialDataPoolSize;    //!< Initial size of DataPool<LArCell>

  //Internally used variables
  unsigned m_nTotalCells;                   //!< Number of cells, set in Initialize()


  const CaloDetDescrManager* m_caloDDM;
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey;
  const LArOnlineID* m_onlineID;
  const CaloCell_ID*  m_caloCID;
  SG::ReadCondHandleKey<LArBadFebCont> m_missingFebKey;

};


#endif





