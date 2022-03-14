/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file CaloD3PDMaker/src/CaloCellDetailsFillerTool.cxx
 * @author Hong Ma
 * @date Mar 2010
 * @brief Block filler tool for CaloCell
 * adopt CBNTAA_CaloCell features. 
 */


#include "CaloCellDetailsFillerTool.h"

#include "GaudiKernel/StatusCode.h" 
#include "CLHEP/Units/SystemOfUnits.h"

#include "CaloConditions/ICaloBadChanTool.h"
#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/CaloCell_Base_ID.h"
#include "CaloEvent/CaloCellContainer.h"

#include "CaloEvent/CaloCell.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"
#include <sstream>
#include <cmath>

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
CaloCellDetailsFillerTool::CaloCellDetailsFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<CaloCell> (type, name, parent),
        m_saveCellGain(false), m_saveCellQuality(false), 
	m_saveDetInfo(false),m_saveTimeInfo(false),m_saveCellStatus(false),
	m_saveId(false),m_savePosition(false), m_saveSigma(false),
    m_pb_tool("CaloBadChanTool"),
    m_useNoise(false)
{

  declareProperty("SaveCellGain",m_saveCellGain);
  declareProperty("SaveCellQuality",m_saveCellQuality);
  declareProperty("SaveDetInfo",m_saveDetInfo);
  declareProperty("SaveTimeInfo",m_saveTimeInfo);
  declareProperty("SaveBadCellStatus",m_saveCellStatus); 
  declareProperty("SaveId",m_saveId);
  declareProperty("SavePositionInfo",m_savePosition);
  declareProperty("SaveSigma",m_saveSigma);

  declareProperty("BadChannelTool",m_pb_tool);

  m_fitQCells = 0;
  m_gainCells = 0;
  m_xCells = 0;
  m_yCells = 0;
  m_zCells = 0;
  m_detCells = 0;
  m_timeCells = 0;
  m_badCell = 0;
  m_offId = 0;
  m_sigma = 0;
}


StatusCode CaloCellDetailsFillerTool::initialize()
{
  if (m_saveCellStatus) {
    CHECK( m_pb_tool.retrieve());
  }


  // retrieve the noise tool
  if(m_saveSigma) m_useNoise = true;
  if (m_useNoise) {
    CHECK( m_caloNoiseKey.initialize (SG::AllowEmpty) );
  }
  else {
    CHECK( m_caloNoiseKey.initialize (false) );
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode CaloCellDetailsFillerTool::book()
{
   if (m_saveCellQuality) CHECK( addVariable  ("QCells",m_fitQCells) ) ;
   if (m_saveCellGain)    CHECK( addVariable  ("GainCells",m_gainCells) );


   if  (m_savePosition) {
     CHECK( addVariable  ("xCells", m_xCells) );
     CHECK( addVariable  ("yCells", m_yCells) );
     CHECK( addVariable  ("zCells", m_zCells) );
   }

   if (m_saveDetInfo) CHECK( addVariable  ("DetCells",m_detCells));
   if (m_saveTimeInfo) CHECK( addVariable  ("TimeCells",m_timeCells));
   if (m_saveCellStatus) CHECK( addVariable  ("BadCells",m_badCell)); 
   if (m_saveId) CHECK( addVariable ("IdCells",m_offId) );       

   if (m_saveSigma) CHECK( addVariable ("Sigma", m_sigma) );
   

  return StatusCode::SUCCESS;
    
}



StatusCode CaloCellDetailsFillerTool::fill ( const CaloCell& c)
{
  const CaloCell* cell = &c; 


  Identifier id  = cell->ID(); 	      

  if (m_savePosition) {
    *m_xCells = cell->x() ;
    *m_yCells = cell->y() ;
    *m_zCells = cell->z() ;
    if (std::abs (*m_zCells) < 1e-8) *m_zCells = 0;
  }

// pack into one work the quality and provenance information

  if (m_saveCellQuality) {
    int quality = ( (cell->quality()&0xFFFF) | ((cell->provenance()&0xFFFF) <<16));
    *m_fitQCells = quality;
  }
  if (m_saveCellGain)    *m_gainCells = cell->gain();
  if (m_saveTimeInfo) *m_timeCells =cell->time() ;

  if (m_saveDetInfo)
    *m_detCells = CaloCell_GetDetectorInfo(*cell);


  if (m_saveId)       *m_offId = id.get_identifier32().get_compact()     ;
  if (m_saveCellStatus)    *m_badCell = m_pb_tool->caloStatus(id).packedData() ;

  if (!m_caloNoiseKey.empty()) {
    SG::ReadCondHandle<CaloNoise> caloNoise{m_caloNoiseKey};
    *m_sigma = caloNoise-> getNoise(cell->ID(), cell->gain());
  }

  return StatusCode::SUCCESS ;
}




unsigned
CaloCellDetailsFillerTool::CaloCell_GetDetectorInfo (const CaloCell& cell) const
{
  const CaloDetDescriptor& desc = *cell.caloDDE()->descriptor();
  const CaloCell_Base_ID& calo_id = *desc.get_calo_helper();
  Identifier cellID = cell.ID();

  // AtlasID bit (4)
  // 1 : lar_em
  // 2 : lar_hec
  // 3 : lar_fcal
  // 4 : tile
  unsigned ATbit1 = desc.is_lar_em()   ? (1<<0) : 0;
  unsigned ATbit2 = desc.is_lar_hec()  ? (1<<1) : 0;
  unsigned ATbit3 = desc.is_lar_fcal() ? (1<<2) : 0;
  unsigned ATbit4 = desc.is_tile()     ? (1<<3) : 0;
  unsigned ATbit  = (ATbit1 | ATbit2 | ATbit3 | ATbit4);

  //std::cout << "ATLAS Calo(EM,HEC,FCal,Tile) : " << ATbit1 << " " << ATbit2 << " " << ATbit3 << " " << ATbit4 << std::endl;
  
  // EM bit (5)
  // 1-2 : sampling
  //       0,1,2,3
  //       0 presampler
  //       1,2,3 each layer
  // 3   : barrel
  // 4   : endcap_inner
  // 5   : endcap_outer
  unsigned EMbit1 = 0;
  unsigned EMbit3 = 0;
  unsigned EMbit4 = 0;
  unsigned EMbit5 = 0;
  if (ATbit1) {
    const LArEM_Base_ID& emid = *calo_id.em_idHelper();
    EMbit1 = unsigned(emid.sampling(cellID));
    EMbit3 = emid.is_em_barrel(cellID)       ? (1<<2) : 0;
    EMbit4 = emid.is_em_endcap_inner(cellID) ? (1<<3) : 0;
    EMbit5 = emid.is_em_endcap_outer(cellID) ? (1<<4) : 0;
  }
  unsigned EMbit  = (EMbit1 | EMbit3 | EMbit4 | EMbit5);

  //std::cout << "EM : " << EMbit1 << " " << EMbit3 << " " << EMbit4 << " " << EMbit5 << std::endl;

  // HEC (2)
  // 1-2: sampling
  //      0,1 = first wheel
  //      2,3 = second wheel
  unsigned HCbit1 = 0;
  if (ATbit2) {
    const LArHEC_Base_ID& hecid = *calo_id.hec_idHelper();
    HCbit1 = unsigned(hecid.sampling(cellID));
  }
  unsigned HCbit = HCbit1;

  //std::cout << "HEC :  " << HCbit1 << std::endl;

  // FCal (2)
  // 1-2 : module
  //       1,2,3
  //       1 EM
  //       2,3 Hadronic
  //
  unsigned FCbit1 = 0;
  if (ATbit3) {
    const LArFCAL_Base_ID& fcalid = *calo_id.fcal_idHelper();
    FCbit1 = unsigned(fcalid.module(cellID));
  }
  unsigned FCbit = FCbit1;

  //std::cout << "FCal : " << FCbit1 << std::endl;

  // Tile bit (8)
  // 1-3 : sample
  //       0 = SAMP_A
  //       1 = SAMP_B, SAMP_BC, SAMP_C
  //       2 = SAMP_D
  //       3 = SAMP_E
  //       4 = SAMP_X
  // 4   : barrel
  // 5   : extbarrel
  // 6   : gap
  // 7   : gapscin
  unsigned TLbit1 = 0;
  unsigned TLbit4 = 0;
  unsigned TLbit5 = 0;
  unsigned TLbit6 = 0;
  unsigned TLbit7 = 0;
  if (ATbit4) {
    const Tile_Base_ID& tileid = *calo_id.tile_idHelper();
    TLbit1 = unsigned(tileid.sample(cellID));
    TLbit4 = tileid.is_tile_barrel(cellID)    ? (1<<3) : 0;
    TLbit5 = tileid.is_tile_extbarrel(cellID) ? (1<<4) : 0;
    TLbit6 = tileid.is_tile_gap(cellID)       ? (1<<5) : 0;
    TLbit7 = tileid.is_tile_gapscin(cellID)   ? (1<<6) : 0;
  }
  unsigned TLbit  = (TLbit1 | TLbit4 | TLbit5 | TLbit6 | TLbit7);

  //std::cout << "Tile : " << TLbit1 << " " << TLbit4 << " " << TLbit5 << " " << TLbit6 << " " << TLbit7 << std::endl;
  
  unsigned CombBit = (ATbit | (EMbit<<4) | (HCbit<<9) | 
		      (FCbit<<11) | (TLbit<<13));

  return CombBit;
}
 

} // end of D3PD namespace
