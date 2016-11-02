//Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/ReadHandleKey.h"

#include "CaloInterface/ICaloCellMakerTool.h"
#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "AthAllocators/DataPool.h"


class CaloCellContainer ;
class LArCablingService;
class CaloDetDescrManager ;
class CaloCell_ID;
class CaloCellContainer ;
class ILArBadChanTool;
class LArCell;

/**
 * @class LArCellBuilderFromLArRawChannelTool
 * @brief An AlgTool class to create a LArCellContainer out of a LArRawChannel container.
 *
 * Inherits from ICaloCellMakerTool and should be called by an instance of the 
 * CaloCellMaker algorithm. 
 * 
 */


class LArCellBuilderFromLArRawChannelTool: public AthAlgTool, 
					   virtual public ICaloCellMakerTool
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
  ~LArCellBuilderFromLArRawChannelTool();

  /**
   * @brief Initialize method.
   * @return Gaudi status code.
   *
   * Initialazes pointers to servies and private members variable.
   * Sets the m_subCalo variable based on the m_lArRegion jobOption.
   * Computes the total number of cells based on the subcalo hash-range. 
   *
   */
  virtual StatusCode initialize();

  /**
   * @brief process method as defined in ICaloCellMaker interface
   * @param theCellContainer Pointer to the CaloCellContainer we are working on
   * @return Gaudi status code.
   *
   */
  virtual StatusCode process( CaloCellContainer * theCellContainer) ;

  /** IOV Call Back for cabling service
   */
  virtual StatusCode cablingSvc_CB (IOVSVC_CALLBACK_ARGS);

  /** IOV Call Back for missing FEB tool/DB
   */
  virtual StatusCode missingFEB_CB (IOVSVC_CALLBACK_ARGS);

  /** Callback added to handle Data-driven GeoModel initialisation
   */
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);


private: 
  /**
   * @brief Initializes @c m_hwHashToCaloDDEmap
   *
   * Iteratres over all CaloDetDescr elements, finds the corresponding online hash 
   * identifier and fill the corresponding entry in @c m_hwHashToCaloDDEmap 
   *(that is actually a vector) with the pointer to the CaloDDE. Elemtens that are 
   * not found are represented by a NULL pointer. 
   * 
   */
  StatusCode caloDDEsInitialize() ;

  /**
   * @brief Adds one cell to the cell container
   * @param theCellContainer Pointer to the CaloCellContainer we are working on
   * @param theRawChannel Pointer to the current LArRawChannel
   * @param complete Wether we deal with a complete set of LArRawChannels or with a subset
   *
   * Transforms the LArRawChannel into a LArCell and adds it to the cell container. 
   * If @c complete is false, this is done by push_back, otherwise the cell is 
   * inserted at the position given by it's hash.
   * 
   */
  
/**
   * @brief Converts one LArRawChannel into a LArCell
   * @param theRawChannel Pointer to a LArRawChannel
   * @param hash (output) Refernce to the hash index of the cell
   * @param pool Memory pool for cell allocation.
   * @return LArCell object (owned by a DataPool). NULL if no DDE is found.
   *
   */
  LArCell* getCell(const LArRawChannel& theRawChannel,
                   IdentifierHash& hash,
                   DataPool<LArCell>& pool);

  /**
   * @brief Gets a CaloDetectorDescriptionElement
   * @param hwId Online identifier whose CaloDDE should be found.
   * @return Pointer to a CaloDDE.
   *
   * This method uses the @c m_hwHashToCaloDDEmap to find a CaloDDE in a efficient way.
   * It uses the @c LArOnlineID helper class to transform teh onlineID into a 
   * channel hash.
   * Returns a NULL pointer if the element cannot be found.
   */
  inline const CaloDetDescrElement * caloDDE (const HWIdentifier & hwId) const;


 /** 
   * @brief Fast method to fill the LArCellContainer 
   * @param rawColl Pointer to the LArRawChannelContainer (input)
   * @param theCellContainer Pointer to the LArCellContainer (output)
   * @return Gaudi StatusCode, FAILURE if fast building failed.
   *
   *  
   */
  StatusCode fillCompleteCellCont(const LArRawChannelContainer * rawColl, CaloCellContainer * theCellContainer);
  
  
  // Variables set by jobOptions
  //std::string     m_rawChannelsName;        //!< rdo container name (jO)
  SG::ReadHandleKey<LArRawChannelContainer>   m_rawChannelsKey;        //!< rdo container name (jO)
  bool            m_addDeadOTX;             //!< activate addition of missing cells from dead OTX
  int             m_initialDataPoolSize;    //!< Initial size of DataPool<LArCell>

  //Internally used variables
  unsigned m_nTotalCells;                   //!< Number of cells, set in Initialize()


  const CaloDetDescrManager* m_caloDDM;
  ToolHandle<LArCablingService> m_cablingSvc;
  const LArOnlineID* m_onlineID;
  const CaloCell_ID*  m_caloCID;
  ToolHandle<ILArBadChanTool> m_badChannelTool;

  /// Association between online hash and pointer to CaloDDE
  std::vector<const CaloDetDescrElement*> m_hwHashToCaloDDEmap;

  /// vector of dummy LArRawChannels (to fill in dead cells)
  std::vector<LArRawChannel> m_deadFEBChannels;

  /// Number of dead FEBs:
  unsigned m_nDeadFEBs;

};


inline const CaloDetDescrElement * LArCellBuilderFromLArRawChannelTool::caloDDE (const HWIdentifier & hwId) const {
  const IdentifierHash hwIDhash=m_onlineID->channel_Hash(hwId);
  return m_hwHashToCaloDDEmap[hwIDhash];    
}


#endif





