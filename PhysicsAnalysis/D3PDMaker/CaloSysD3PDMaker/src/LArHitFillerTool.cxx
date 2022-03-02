/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloSysD3PDMaker/src/ClusterSamplingFillerTool.cxx
 * @author maarten boonekamp snyder <maarten.boonekamp@cea.fr>
 * @date Sep, 2009
 * @brief Block filler tool for samplings from a CaloCluster.
 */


#include "LArHitFillerTool.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/TileID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include <sstream>


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
LArHitFillerTool::LArHitFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<LArHit> (type, name, parent),
        m_emid(0),
        m_fcalid(0),
        m_hecid(0),
        m_tileid(0),
        m_onlineid(0)
{
  // declareProperty ("SelectedCells",    m_writeSelectedCells=true);
  LArHitFillerTool::book().ignore();
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode LArHitFillerTool::initialize()
{
  ServiceHandle<StoreGateSvc> detStore("DetectorStore", name());
  ATH_CHECK ( detStore.retrieve() );
    
  ATH_CHECK ( m_caloMgrKey.initialize()     );
  ATH_CHECK ( detStore->retrieve(m_emid)    );
  ATH_CHECK ( detStore->retrieve(m_fcalid)  );
  ATH_CHECK ( detStore->retrieve(m_hecid)   );
  ATH_CHECK ( detStore->retrieve(m_tileid)  );
  ATH_CHECK ( detStore->retrieve(m_onlineid));
   
  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode LArHitFillerTool::book()
{
  CHECK( addVariable ("eta", m_eta) );
  CHECK( addVariable ("phi", m_phi) );
  CHECK( addVariable ("E",   m_e) );
  CHECK( addVariable ("Time",m_time) );
  CHECK( addVariable ("ID",  m_id) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode LArHitFillerTool::fill (const LArHit& p)
{

  Identifier id = p.cellID(); 

  //bool fcal = m_tileid->is_lar_fcal(id); 

  *m_e = p.energy();  
  *m_time = p.time();
  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
  ATH_CHECK(caloMgrHandle.isValid());
  const CaloDetDescrElement* dde = (*caloMgrHandle)->get_element(id);
  *m_eta = dde->eta();
  *m_phi = dde->phi(); 
  *m_id = CaloCell_GetDetectorInfo(id);
  
  return StatusCode::SUCCESS;
}



unsigned int LArHitFillerTool::CaloCell_GetDetectorInfo(Identifier &cellID)
{
  // AtlasID bit (4)
  // 1 : lar_em
  // 2 : lar_hec
  // 3 : lar_fcal
  // 4 : tile
  unsigned ATbit1 = m_emid->is_lar_em(cellID)     ? (1<<0) : 0;
  unsigned ATbit2 = m_hecid->is_lar_hec(cellID)   ? (1<<1) : 0;
  unsigned ATbit3 = m_fcalid->is_lar_fcal(cellID) ? (1<<2) : 0;
  unsigned ATbit4 = m_tileid->is_tile(cellID)     ? (1<<3) : 0;
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
    EMbit1 = unsigned(m_emid->sampling(cellID));
    EMbit3 = m_emid->is_em_barrel(cellID)       ? (1<<2) : 0;
    EMbit4 = m_emid->is_em_endcap_inner(cellID) ? (1<<3) : 0;
    EMbit5 = m_emid->is_em_endcap_outer(cellID) ? (1<<4) : 0;
  }
  unsigned EMbit  = (EMbit1 | EMbit3 | EMbit4 | EMbit5);

  //std::cout << "EM : " << EMbit1 << " " << EMbit3 << " " << EMbit4 << " " << EMbit5 << std::endl;

  // HEC (2)
  // 1-2: sampling
  //      0,1 = first wheel
  //      2,3 = second wheel
  unsigned HCbit1 = 0;
  if (ATbit2) {
    HCbit1 = unsigned(m_hecid->sampling(cellID));
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
    FCbit1 = unsigned(m_fcalid->module(cellID));
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
    TLbit1 = unsigned(m_tileid->sample(cellID));
    TLbit4 = m_tileid->is_tile_barrel(cellID)    ? (1<<3) : 0;
    TLbit5 = m_tileid->is_tile_extbarrel(cellID) ? (1<<4) : 0;
    TLbit6 = m_tileid->is_tile_gap(cellID)       ? (1<<5) : 0;
    TLbit7 = m_tileid->is_tile_gapscin(cellID)   ? (1<<6) : 0;
  }
  unsigned TLbit  = (TLbit1 | TLbit4 | TLbit5 | TLbit6 | TLbit7);

  //std::cout << "Tile : " << TLbit1 << " " << TLbit4 << " " << TLbit5 << " " << TLbit6 << " " << TLbit7 << std::endl;
  
  unsigned int CombBit = (ATbit | (EMbit<<4) | (HCbit<<9) | 
		      (FCbit<<11) | (TLbit<<13));

  return CombBit;
}
 


} // namespace D3PD
