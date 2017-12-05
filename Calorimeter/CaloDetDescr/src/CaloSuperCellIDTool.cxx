/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloDetDescr/src/CaloSuperCellIDTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Tool to map between calorimeter cells and supercells.
 */


#include "CaloSuperCellIDTool.h"
#include "CaloIdentifier/CaloCell_Base_ID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "AthenaKernel/errorcheck.h"
#include "boost/format.hpp"

#include <iostream>


namespace {


int posneg_or_section (const CaloCell_Base_ID* idhelper, Identifier id)
{
  if (idhelper->is_tile (id))
    return idhelper->section (id);
  return idhelper->pos_neg (id);
}


int sampling_or_side (const CaloCell_Base_ID* idhelper, Identifier id)
{
  if (idhelper->is_tile (id))
    return idhelper->side (id);
  return idhelper->sampling (id);
}


}  // anonymous namespace




/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
CaloSuperCellIDTool::CaloSuperCellIDTool (const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
  : base_class (type, name, parent),
    m_calo_id_manager (0)
{
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode CaloSuperCellIDTool::initialize()
{
  CHECK( base_class::initialize() );
  CHECK( detStore()->retrieve (m_calo_id_manager, "CaloIdManager") );

  initIDMap ();

  return StatusCode::SUCCESS;
}


/**
 * @brief Initialize the mapping table.
 */
void CaloSuperCellIDTool::initIDMap ()
{
  const CaloCell_Base_ID* cell_helper = m_calo_id_manager->getCaloCell_ID();
  const CaloCell_Base_ID* sc_helper = m_calo_id_manager->getCaloCell_SuperCell_ID();

  m_offlineIndex.clear();
  m_superCellIndex.clear();
  m_superCellIndexEnd.clear();
  m_idmap.clear();

  // One entry in the index tables for each region hash; -1 means no entry.
  m_offlineIndex.resize (cell_helper->calo_region_hash_max(), -1);
  m_superCellIndex.resize (sc_helper->calo_region_hash_max(), -1);
  m_superCellIndexEnd.resize (sc_helper->calo_region_hash_max(), -1);

  // Loop over all offline regions.
  for (const Identifier& cell_reg : cell_helper->reg_range()) {
    if (cell_helper->is_em(cell_reg) ||
        cell_helper->is_hec(cell_reg) ||
        cell_helper->is_fcal(cell_reg))
    {
      int sub_calo = cell_helper->sub_calo (cell_reg);
      int pos_neg = cell_helper->pos_neg (cell_reg);
      int sampling = cell_helper->sampling (cell_reg);
      int cell_ietamin = cell_helper->eta_min (cell_reg); 
      int cell_ietamax = cell_helper->eta_max (cell_reg);
      float cell_etasize = cell_helper->etaGranularity(cell_reg);
      float cell_phisize = cell_helper->phiGranularity(cell_reg);
      float cell_etamin = cell_helper->eta0 (cell_reg);
      float cell_etamax = cell_etamin +
        cell_etasize*(cell_ietamax - cell_ietamin + 1);

      float inv_cell_etasize = 1. / cell_etasize;
      float inv_cell_phisize = 1. / cell_phisize;

      // Find all overlapping supercell regions in the same sampling
      // and make table entries.  HEC supercells are summed
      // over samplings, so don't make sampling requirements there.
      for (const Identifier& sc_reg : sc_helper->reg_range()) {
        if (sc_helper->sub_calo (sc_reg) == sub_calo &&
            sc_helper->pos_neg (sc_reg) == pos_neg &&
            (sub_calo == CaloCell_ID::LARHEC ||
             sc_helper->sampling (sc_reg) == sampling))
        {
          int sc_ietamin = sc_helper->eta_min (sc_reg);
          int sc_ietamax = sc_helper->eta_max (sc_reg);
          float sc_etasize = sc_helper->etaGranularity(sc_reg);
          float sc_phisize = sc_helper->phiGranularity(sc_reg);
          float sc_etamin = sc_helper->eta0 (sc_reg);
          float sc_etamax= sc_etamin + sc_etasize*(sc_ietamax - sc_ietamin + 1);

          // Find the overlap between the offline and supercell regions.
          float etamin = std::max (cell_etamin, sc_etamin);
          float etamax = std::min (cell_etamax, sc_etamax);

          if (etamin < etamax - 1e-4) {
            // There's overlap --- make a table entry.
            IDMapElt elt;
            elt.m_cell_reg = cell_helper->calo_region_hash (cell_reg);
            elt.m_sc_reg = sc_helper->calo_region_hash (sc_reg);
            elt.m_etadiv = int (sc_etasize * inv_cell_etasize + 0.1);
            elt.m_phidiv = int (sc_phisize * inv_cell_phisize + 0.1);

            if (sub_calo == CaloCell_ID::LARHEC) {
              // FIXME: Shouldn't have to special-case this.
              elt.m_cell_ieta_adj = 0;
              elt.m_sc_ieta_adj   = 0;
            }
            else {
              elt.m_cell_ieta_adj = cell_ietamin;
              elt.m_sc_ieta_adj   = sc_ietamin;
            }

            elt.m_cell_ietamin = int ((etamin - cell_etamin) * inv_cell_etasize +
                                      cell_ietamin + 0.1);
            elt.m_cell_ietamax = int ((etamax - cell_etamin) * inv_cell_etasize +
                                      cell_ietamin - 0.1);
            const float inv_sc_etasize = 1. / sc_etasize;
            elt.m_sc_ietamin = int ((etamin - sc_etamin) * inv_sc_etasize +
                                    sc_ietamin + 0.1);
            elt.m_sc_ietamax = int ((etamax - sc_etamin) * inv_sc_etasize +
                                    sc_ietamin - 0.1);

            addMapEntry (elt);
          }
        }
      }
    }
    else if (cell_helper->is_tile(cell_reg)) {
      int section = cell_helper->section (cell_reg);
      if (section != TileID::BARREL && section != TileID::EXTBAR)
        continue;
      int sub_calo = cell_helper->sub_calo (cell_reg);
      int side = cell_helper->side (cell_reg);
      Identifier sc_reg = sc_helper->region_id (sub_calo, section, side, 0);

      IDMapElt elt;
      elt.m_cell_reg = cell_helper->calo_region_hash (cell_reg);
      elt.m_sc_reg = sc_helper->calo_region_hash (sc_reg);
      elt.m_etadiv = 1;
      elt.m_phidiv = 1;
      elt.m_cell_ieta_adj = 0;
      elt.m_sc_ieta_adj = 0;
      elt.m_cell_ietamin = cell_helper->eta_min (cell_reg);
      elt.m_cell_ietamax = cell_helper->eta_max (cell_reg);
      elt.m_sc_ietamin = sc_helper->eta_min (sc_reg);
      elt.m_sc_ietamax = sc_helper->eta_max (sc_reg);
      addMapEntry (elt);
    }
  }

 
  // Allow dumping the mapping table for validation.
  if (msgLvl (MSG::DEBUG)) {
    msg(MSG::DEBUG) << "CaloSuperCellIDTool mapping table:\n";
    msg(MSG::DEBUG) << "LArEM ----------------------------\n";
    for (const IDMapElt& elt : m_idmap) {
      Identifier cell_reg = cell_helper->region_id (elt.m_cell_reg);
      Identifier sc_reg = sc_helper->region_id (elt.m_sc_reg);
      msg(MSG::DEBUG) <<
        boost::format
          ("  %3d %d/%2d/%2d/%d %3d %d/%2d/%2d/%d %d %d %3d %3d %3d %3d %3d %3d\n") %
        (int)elt.m_cell_reg %
        cell_helper->sub_calo(cell_reg) %
        posneg_or_section (cell_helper, cell_reg) %
        sampling_or_side (cell_helper, cell_reg) %
        cell_helper->region(cell_reg) %
        (int)elt.m_sc_reg %
        sc_helper->sub_calo(sc_reg) %
        posneg_or_section (sc_helper, sc_reg) %
        sampling_or_side (sc_helper, sc_reg) %
        sc_helper->region(sc_reg) %

        elt.m_etadiv % elt.m_phidiv %
        elt.m_cell_ietamin % elt.m_cell_ietamax %
        elt.m_sc_ietamin % elt.m_sc_ietamax %
        elt.m_cell_ieta_adj % elt.m_sc_ieta_adj;
    }
    msg(MSG::DEBUG) << endmsg;
  }    

  initFCALIDMap();
  
  msg(MSG::INFO ) << "Done with initIDMap" << endmsg;
}


/**
 * @brief Helper to add an entry to the region mapping table.
 */
void CaloSuperCellIDTool::addMapEntry (const IDMapElt& elt)
{
  assert (elt.m_cell_reg < m_offlineIndex.size());
  if (m_offlineIndex[elt.m_cell_reg] == -1)
    m_offlineIndex[elt.m_cell_reg] = m_idmap.size();

  assert (elt.m_sc_reg < m_superCellIndex.size());
  if (m_superCellIndex[elt.m_sc_reg] == -1)
    m_superCellIndex[elt.m_sc_reg] = m_idmap.size();
  assert (elt.m_sc_reg < m_superCellIndexEnd.size());
  m_superCellIndexEnd[elt.m_sc_reg] = m_idmap.size()+1;

  m_idmap.push_back (elt);
}


/**
 * @brief FCAL is a special case.
 */
void CaloSuperCellIDTool::initFCALIDMap ()
{
  const LArFCAL_Base_ID* sfcal_helper = m_calo_id_manager->getFCAL_SuperCell_ID();
  const LArFCAL_Base_ID* fcal_helper = m_calo_id_manager->getFCAL_ID();

  m_fcal_fromCell.clear();
  m_fcal_fromSuperCell.clear();
  m_fcal_fromCell.resize(fcal_helper->channel_hash_max());
  m_fcal_fromSuperCell.resize(sfcal_helper->channel_hash_max());
  
  for (const Identifier& cell_id : fcal_helper->fcal_range()) {
    const int sc_phi = fcal_helper->phi (cell_id);
    const int sc_lay = fcal_helper->module (cell_id);
    const int cell_ieta = fcal_helper->eta (cell_id);
    int sc_pn   =  fcal_helper->pos_neg( cell_id ); 
    int sc_ieta = -1;
    if (sc_lay==3) {
      sc_ieta = cell_ieta / 4;
    } 
    else if (sc_lay==2) {
      sc_ieta = cell_ieta / 4;
    }
    else if (sc_lay==1) {
      if (cell_ieta < 16)
        sc_ieta = 0;
      else if (cell_ieta < 24)
        sc_ieta = 1;
      else
        sc_ieta = 2 + (cell_ieta-24)/4;
    }
    Identifier sc_id =  sfcal_helper->channel_id(sc_pn, sc_lay, sc_ieta, sc_phi);
    IdentifierHash sc_hash = sfcal_helper->channel_hash( sc_id );
    IdentifierHash cell_hash = fcal_helper->channel_hash( cell_id );
    m_fcal_fromCell[ cell_hash ] = sc_id;
    m_fcal_fromSuperCell[ sc_hash ].push_back( cell_id );
  }

  // Allow dumping the mapping table for validation.
  if (msgLvl (MSG::DEBUG)) {
    msg(MSG::DEBUG) << "\n LArFCAL ---------------------------\n";
    for (const Identifier& sc_id : sfcal_helper->fcal_range()) {
      IdentifierHash sc_hash = sfcal_helper->channel_hash( sc_id );
      std::vector<Identifier> cells = m_fcal_fromSuperCell[ sc_hash ];
      msg(MSG::DEBUG) <<
	boost::format
	("  %5d %2d/%2d/%2d/%2d ... %2d cells\n") % 
	sc_hash %
	sfcal_helper->pos_neg(sc_id) %
	sfcal_helper->module(sc_id) %
	sfcal_helper->eta(sc_id) %
	sfcal_helper->phi(sc_id) %
	(int)cells.size();
    }
    msg(MSG::DEBUG) << endmsg;
  }
}


/**
 * @brief Given an offline cell identifier, return the corresponding
 *        supercell identifier.  If none exists, an invalid identifier
 *        is returned.
 */
Identifier
CaloSuperCellIDTool::offlineToSuperCellID (const Identifier& id) const
{
  const CaloCell_Base_ID* cell_helper = m_calo_id_manager->getCaloCell_ID();

  if (cell_helper->is_em(id) || cell_helper->is_hec(id)) {
    // Look for the first entry in the mapping table for this offline region.
    const CaloCell_Base_ID* sc_helper   = m_calo_id_manager->getCaloCell_SuperCell_ID();

    Identifier reg_id = cell_helper->region_id (id);
    IdentifierHash rhash = cell_helper->calo_region_hash (reg_id);
    assert (rhash < m_offlineIndex.size());
    int ndx = m_offlineIndex[rhash];
    if (ndx < 0)
      return Identifier();
    
    // Now search through all entries for this offline region to find one
    // that includes this cell.
    int ieta = cell_helper->eta (id);
    
    do {
      const IDMapElt& elt = m_idmap[ndx];
      if (elt.m_cell_ietamin <= ieta && elt.m_cell_ietamax >= ieta) {
	// Found a matching entry.  Calculate the corresponding supercell
	// indices and return the new ID.
	
	int ieta_sc = ((ieta - elt.m_cell_ietamin + elt.m_cell_ieta_adj) /
		       elt.m_etadiv) +
	  elt.m_sc_ietamin - elt.m_sc_ieta_adj;

	return sc_helper->cell_id (sc_helper->region_id (elt.m_sc_reg),
				   ieta_sc,
				   cell_helper->phi(id) / elt.m_phidiv);
      }
      ++ndx;
    } while (ndx < (int)m_idmap.size() && m_idmap[ndx].m_cell_reg == rhash);
  }

  else if (cell_helper->is_fcal(id)) {
    const LArFCAL_ID* fcal_helper = m_calo_id_manager->getFCAL_ID();
    IdentifierHash cell_hash = fcal_helper->channel_hash(id);
    return m_fcal_fromCell[ cell_hash ];
  }

  else if (cell_helper->is_tile(id)) {
    int section = cell_helper->section (id);
    int sample_offline = cell_helper->sample(id);
    int tower = cell_helper->tower(id);

    // A couple special cases in the transition region.
    // cf. http://hep.uchicago.edu/atlas/tilecal/level1/geometry.html
    // The cell at the end of the barrel is grouped with barrel cells,
    // rather than with the endcap cells at the same eta, and analogously
    // for the D4 cell.
    // Be careful: tower indices start with 0 here, but with 1 on the
    // referenced diagram.
    //

    if (section == TileID::BARREL && tower == 9 &&
        sample_offline == TileID::SAMP_A)
    {
      tower = 8;
    }
    else if (section == TileID::GAPDET && tower == 9 &&
             sample_offline == TileID::SAMP_C)
    {
      section = TileID::EXTBAR;
    }

    else if (section == TileID::GAPDET && tower == 8 &&
             sample_offline == TileID::SAMP_D)
    {
      tower = 9;
      section = TileID::EXTBAR;
    }

    if (section != TileID::BARREL && section != TileID::EXTBAR)
      return Identifier();

    int sample_sc = sample_offline;
    if (sample_sc != TileID::SAMP_D) sample_sc = TileID::SAMP_A;

    const CaloCell_Base_ID* sc_helper   =
      m_calo_id_manager->getCaloCell_SuperCell_ID();

    return sc_helper->cell_id (cell_helper->sub_calo(id),
                               section,
                               cell_helper->side(id),
                               cell_helper->module(id),
                               tower,
                               sample_sc);
  }

  return Identifier();
}


/**
 * @brief Given a supercell identifier, return the list of corresponding
 *        offline cell identifiers.
 */
std::vector<Identifier>
CaloSuperCellIDTool::superCellToOfflineID (const Identifier& id) const
{
  std::vector<Identifier> out;
  const CaloCell_SuperCell_ID* sc_helper =  m_calo_id_manager->getCaloCell_SuperCell_ID();

  // Look for the first entry in the mapping table for this supercell region.
  if (sc_helper->is_em (id) || sc_helper->is_hec (id)) {
    const CaloCell_Base_ID* cell_helper = m_calo_id_manager->getCaloCell_ID();

    Identifier reg_id = sc_helper->region_id (id);
    IdentifierHash rhash = sc_helper->calo_region_hash (reg_id);
    assert (rhash < m_superCellIndex.size());
    int ndx = m_superCellIndex[rhash];
    if (ndx < 0)
      return out;
    int end = m_superCellIndexEnd[rhash];
    if (end < 0)
      return out;
    
    // Now search through all entries for this supercell region to find one
    // that includes this supercell.
    int ieta = sc_helper->eta (id);
    
    for (; ndx < end; ++ndx) {
      const IDMapElt& elt = m_idmap[ndx];
      if (elt.m_sc_reg == rhash &&
          elt.m_sc_ietamin <= ieta &&
          elt.m_sc_ietamax >= ieta)
      {
	// Found a matching entry.  
	// Find the overlapping eta range in the offline region.
	
	int ieta0 = (ieta - elt.m_sc_ietamin + elt.m_sc_ieta_adj) * elt.m_etadiv +
	  elt.m_cell_ietamin - elt.m_cell_ieta_adj;
	Identifier cell_reg_id = cell_helper->region_id (elt.m_cell_reg);
	int ieta = std::max (ieta0, elt.m_cell_ietamin);
	int ietamax = std::min (ieta0 + elt.m_etadiv - 1, elt.m_cell_ietamax);
	
	// Add all matching cells to the output list.
	int iphi0 = sc_helper->phi (id) * elt.m_phidiv;
	for (; ieta <= ietamax; ++ieta) {
	  for (int ip = 0; ip < elt.m_phidiv; ip++) {
	    out.push_back(cell_helper->cell_id (cell_reg_id, ieta, iphi0+ip));
	  }
	}
      }
    }
  }

  else if ( sc_helper->is_fcal( id ) ) {
    const LArFCAL_Base_ID* sfcal_helper = sc_helper->fcal_idHelper();
    IdentifierHash sc_hash = sfcal_helper->channel_hash( id );
    out = m_fcal_fromSuperCell[ sc_hash ];
  }

  else if (sc_helper->is_tile (id)) {
    int module = sc_helper->module(id);
    int tower = sc_helper->tower(id);
    int sample = sc_helper->sample(id);
    
    const CaloCell_Base_ID* cell_helper = m_calo_id_manager->getCaloCell_ID();
    const Tile_Base_ID* tile_helper = cell_helper->tile_idHelper();

    Identifier reg_id = tile_helper->region_id (sc_helper->section(id),
                                                sc_helper->side(id));

    Identifier cell_id;

    // Transition region special cases.
    if (tower == 8 && sample == TileID::SAMP_A) {
      if (tile_helper->cell_id (reg_id, module, 9, sample, cell_id))
        out.push_back (cell_id);
    }
    if (tower == 9) {
      Identifier greg_id = tile_helper->region_id (TileID::GAPDET,
                                                   sc_helper->side(id));
      if (tile_helper->cell_id (greg_id, module, 8, TileID::SAMP_D, cell_id))
        out.push_back (cell_id);
      if (sample == TileID::SAMP_A) {
        if (tile_helper->cell_id (greg_id, module, 9, TileID::SAMP_C, cell_id))
          out.push_back (cell_id);
      }
      return out;
    }


    if (tile_helper->cell_id (reg_id, module, tower, sample, cell_id))
      out.push_back (cell_id);

    if (sample == TileID::SAMP_A) {
      if(tile_helper->cell_id(reg_id, module, tower, TileID::SAMP_BC, cell_id))
        out.push_back (cell_id);
      if(tile_helper->cell_id(reg_id, module, tower, TileID::SAMP_D, cell_id))
        out.push_back (cell_id);
    }
  }

  return out;
}


/**
 * @brief Given an offline region identifier, return the corresponding
 *        supercell region identifier(s).  There will normally
 *        be only one, but it's possible for there to be multiple
 *        matches.  If none exists, an invalid identifier
 *        is returned.
 */
std::vector<Identifier>
CaloSuperCellIDTool::offlineToSuperCellRegion (const Identifier& reg_id) const
{
  const CaloCell_Base_ID* cell_helper = m_calo_id_manager->getCaloCell_ID();
  const CaloCell_Base_ID* sc_helper =
    m_calo_id_manager->getCaloCell_SuperCell_ID();

  std::vector<Identifier> out;

  // Look for the first entry in the mapping table for this offline region.
  IdentifierHash rhash = cell_helper->calo_region_hash (reg_id);
  assert (rhash < m_offlineIndex.size());
  int ndx = m_offlineIndex[rhash];
  while (ndx >= 0 &&
         ndx < (int)m_idmap.size() &&
         m_idmap[ndx].m_cell_reg == rhash)
  {
    out.push_back (sc_helper->region_id (m_idmap[ndx].m_sc_reg));
    ++ndx;
  }

  return out;
}


/**
 * @brief Given a supercell region identifier, return the corresponding
 *        offline region identifier(s).  There will normally
 *        be only one, but it's possible for there to be multiple
 *        matches.  If none exists, an invalid identifier
 *        is returned.
 */
std::vector<Identifier>
CaloSuperCellIDTool::superCellToOfflineRegion (const Identifier& reg_id) const
{
  const CaloCell_Base_ID* cell_helper = m_calo_id_manager->getCaloCell_ID();
  const CaloCell_Base_ID* sc_helper =
    m_calo_id_manager->getCaloCell_SuperCell_ID();

  std::vector<Identifier> out;

  // Look for the first entry in the mapping table for this offline region.
  IdentifierHash rhash = sc_helper->calo_region_hash (reg_id);
  assert (rhash < m_superCellIndex.size());
  int ndx = m_superCellIndex[rhash];
  int end = m_superCellIndexEnd[rhash];
  for (; ndx < end; ++ndx) {
    if (m_idmap[ndx].m_sc_reg == rhash)
      out.push_back (cell_helper->region_id (m_idmap[ndx].m_cell_reg));
  }

  return out;
}

