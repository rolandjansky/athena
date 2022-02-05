/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloSysD3PDMaker/src/CaloCalibHitFillerTool.cxx
 * @author P. Strizenec, based on code from ssnyder
 * @date Feb, 2013
 * @brief Block filler tool for Calibration Hits.
 */


#include "CaloCalibHitFillerTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDmDetDescr/CaloDmDescrManager.h"
#include <sstream>


namespace D3PD {


CaloCalibHitFillerTool::CaloCalibHitFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : BlockFillerTool<CaloCalibrationHit> (type, name, parent),
        m_dm_ddm(0),
        m_emid(0),
        m_fcalid(0),
        m_hecid(0),
        m_tileid(0),
        m_onlineid(0),
        m_cellid(0),
        m_dmid(0),
        m_isDM(false),
        m_isbasic(true),
        m_savepartID(false), m_savefullID(false),
        m_savetoten(true), m_savevisen(true),m_saveallen(false)
{
  declareProperty("isDeadMaterial",m_isDM); 
  declareProperty("isBasic",m_isbasic); 
  declareProperty("SaveParticleID",m_savepartID); 
  declareProperty("SaveFullCellID",m_savefullID); 
  declareProperty("SaveTotalEnergy",m_savetoten); 
  declareProperty("SaveVisibleEnergy",m_savevisen); 
  declareProperty("SaveAllEnergies",m_saveallen); 
  CaloCalibHitFillerTool::book().ignore();
}


StatusCode CaloCalibHitFillerTool::initialize()
{
  ServiceHandle<StoreGateSvc> detStore("DetectorStore", name());
  ATH_CHECK ( detStore.retrieve() );
    
  ATH_CHECK ( m_caloMgrKey.initialize()     );
  ATH_CHECK ( detStore->retrieve(m_emid)    );
  ATH_CHECK ( detStore->retrieve(m_fcalid)  );
  ATH_CHECK ( detStore->retrieve(m_hecid)   );
  ATH_CHECK ( detStore->retrieve(m_tileid)  );
  ATH_CHECK ( detStore->retrieve(m_onlineid));
  if(m_isDM) {
     ATH_CHECK ( detStore->retrieve(m_dmid) );
     m_dm_ddm = CaloDmDescrManager::instance(); 
     if(!m_dm_ddm) {
        std::cout<<"Could not get CaloDmDescrManager"<<std::endl;
        return StatusCode::FAILURE;
     }
  } else {
     ATH_CHECK ( detStore->retrieve(m_cellid) );
  }
   
  if(m_saveallen) { m_savetoten = false; m_savevisen = false; }

  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode CaloCalibHitFillerTool::book()
{

   
  if(m_isbasic) { 
     CHECK( addVariable ("eta", m_eta) );
     CHECK( addVariable ("phi", m_phi) );
  }
  if(m_savepartID) {
     CHECK( addVariable ("particleID", m_particleID) );
  }
  if(m_savefullID){
    if(m_isDM) {
      CHECK( addVariable ("detzside", m_det) );
      CHECK( addVariable ("type", m_side) );
      CHECK( addVariable ("sampling", m_sam) );
      CHECK( addVariable ("region", m_reg) );
      CHECK( addVariable ("ieta", m_etatow) );
      CHECK( addVariable ("iphi", m_phimod) );
    } else {   
      CHECK( addVariable ("detector", m_det) );
      CHECK( addVariable ("sample", m_sam) );
      CHECK( addVariable ("side", m_side) );
      CHECK( addVariable ("reg_sec", m_reg) );
      CHECK( addVariable ("eta_tower", m_etatow) );
      CHECK( addVariable ("phi_module", m_phimod) );
    }
  } 
  if(m_isbasic){
     CHECK( addVariable ("ID",  m_id) );
  }
  if(m_savetoten && m_isbasic) {
    CHECK( addVariable ("EnergyTot",   m_etot) );
  }
  if(m_savevisen && m_isbasic) {
    CHECK( addVariable ("EnergyVis",   m_evis) );
  }
  if(m_saveallen) {
    CHECK( addVariable ("EnergyEm",   m_e_em) );
    CHECK( addVariable ("EnergyNonEm",m_e_nonem) );
    CHECK( addVariable ("EnergyInv",  m_e_inv) );
    CHECK( addVariable ("EnergyEsc",  m_e_esc) );

  }
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
StatusCode CaloCalibHitFillerTool::fill (const CaloCalibrationHit& p)
{
  MsgStream log(msgSvc(), name());

  Identifier id = p.cellID(); 

  // Check if we have good ID
  if(m_isDM && (! (m_dmid->is_lar(id) || m_dmid->is_tile(id)))) {
     ATH_MSG_ERROR( "Should be DM calibration hit, but do not have DM identifier: "<<id.getString() );
     return StatusCode::FAILURE;
  }
  //bool fcal = m_tileid->is_lar_fcal(id); 

  if(m_saveallen) {
    *m_e_em = p.energyEM();  
    *m_e_nonem = p.energyNonEM();
    *m_e_inv = p.energyInvisible();
    *m_e_esc = p.energyEscaped();
  }
  if(m_savetoten && m_isbasic) *m_etot=p.energyTotal(); 
  if(m_savevisen && m_isbasic) *m_evis=p.energyEM() + p.energyNonEM();

  if(m_isbasic) {
    if(!m_isDM) {
      SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
      ATH_CHECK(caloMgrHandle.isValid());
      const CaloDetDescrElement* dde = (*caloMgrHandle)->get_element(id);
      if(dde) {
        *m_eta = dde->eta();
        *m_phi = dde->phi(); 
      }
    } else {
      const CaloDmDescrElement *el = m_dm_ddm->get_element(id); 
      if(el) {
        *m_eta = el->eta();
        *m_phi = el->phi();
      }
    }
  }
  if(m_savefullID){
   if(m_isDM) {
     *m_det = m_dmid->pos_neg_z(id);
     *m_side = m_dmid->dmat(id);
     *m_sam = m_dmid->sampling(id);
     *m_reg = m_dmid->region(id);
     *m_etatow = m_dmid->eta(id);
     *m_phimod = m_dmid->phi(id);
   } else {  
     *m_det = m_cellid->sub_calo(id);
     *m_sam = m_cellid->calo_sample(id);
     if(m_cellid->is_tile(id)) {
       *m_side = m_cellid->side(id);
       *m_reg =  m_cellid->section(id);
       *m_etatow = m_cellid->tower(id);
       *m_phimod = m_cellid->module(id);
     } else {
       *m_side = m_cellid->pos_neg(id);
       *m_reg =  m_cellid->region(id);
       *m_etatow = m_cellid->eta(id);
       *m_phimod = m_cellid->phi(id);
     } 
   }
  } 
  if(m_isbasic){
    *m_id = CaloHit_GetDetectorInfo(id);
  }
  
  return StatusCode::SUCCESS;
}



unsigned int CaloCalibHitFillerTool::CaloHit_GetDetectorInfo(Identifier &cellID)
{
   if(!m_isDM) {
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
   } else { // How to code the DM identifier ?
       unsigned int SideBit = 0; // bit 0
       if(m_dmid->pos_neg_z(cellID) < 0) SideBit = 1;
       unsigned int DetBit = 0; // bit 1
       if(m_dmid->is_tile(cellID)) DetBit = (1<<1);
       unsigned int TypeBit = m_dmid->dmat(cellID)<<2; // bit 2-3
       unsigned int SamplBit = m_dmid->sampling(cellID)<<4; // bit 4-5
       unsigned int RegionBit = m_dmid->region(cellID)<<6; // bit 6-8
       unsigned int EtaBit = m_dmid->eta(cellID)<<9; // bit 9-14
       unsigned int PhiBit = m_dmid->phi(cellID)<<15; // bit 15-21
       unsigned int CombBit = (SideBit | DetBit | TypeBit | SamplBit | RegionBit | EtaBit | PhiBit);
       return CombBit;
   }
}
 


} // namespace D3PD
