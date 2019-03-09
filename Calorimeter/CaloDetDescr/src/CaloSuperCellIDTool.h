// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloDetDescr/src/CaloSuperCellIDTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Tool to map between calorimeter cells and supercells.
 */


#ifndef CALODETDESCR_CALOSUPERCELLIDTOOL_H
#define CALODETDESCR_CALOSUPERCELLIDTOOL_H


#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "Identifier/IdentifierHash.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


class CaloCell_ID;
class CaloCell_SuperCell_ID;

class CaloSuperCellIDTool
  : public extends1<AthAlgTool, ICaloSuperCellIDTool>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CaloSuperCellIDTool (const std::string& type,
                       const std::string& name,
                       const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Given an offline cell identifier, return the corresponding
   *        supercell identifier.  If none exists, an invalid identifier
   *        is returned.
   */
  virtual Identifier offlineToSuperCellID (const Identifier& id) const;


  /**
   * @brief Given a supercell identifier, return the list of corresponding
   *        offline cell identifiers.
   */
  virtual std::vector<Identifier>
  superCellToOfflineID (const Identifier& id) const;


  /**
   * @brief Given an offline region identifier, return the corresponding
   *        supercell region identifier(s).  There will normally
   *        be only one, but it's possible for there to be multiple
   *        matches.  If none exists, an invalid identifier
   *        is returned.
   */
  virtual std::vector<Identifier>
  offlineToSuperCellRegion (const Identifier& reg_id) const;


  /**
   * @brief Given a supercell region identifier, return the corresponding
   *        offline region identifier(s).  There will normally
   *        be only one, but it's possible for there to be multiple
   *        matches.  If none exists, an invalid identifier
   *        is returned.
   */
  virtual std::vector<Identifier>
  superCellToOfflineRegion (const Identifier& reg_id) const;


private:
  /// FCAL is a special case.
  void initFCALIDMap();

  // During initialization, we create a table describing the mapping
  // beween offline and supercell regions.  There is one entry per
  // unique pair of offline/supercell regions, sorted in increasing
  // hash order.  (So there may be multiple entries for a given
  // offline or supercell region, but such entries should be adjacent
  // in the table.)

  /// Initialize the mapping table.
  void initIDMap ();

  /// Mapping table entry.
  struct IDMapElt {
    /// Offline region hash for this entry.
    IdentifierHash m_cell_reg;

    /// Supercell region hash for this entry.
    IdentifierHash m_sc_reg;

    /// Number of offline cells per supercell, in eta/phi.
    int m_etadiv;
    int m_phidiv;

    /// Offline minimum and maximum (inclusive) eta indices for this entry.
    /// Normally just the min/max of the region, but may be smaller
    /// if there is only a partial overlap with a supercell region.
    int m_cell_ietamin;
    int m_cell_ietamax;

    /// Supercell minimum and maximum (inclusive) eta indices for this entry.
    /// Normally just the min/max of the region, but may be smaller
    /// if there is only a partial overlap with an offline region.
    int m_sc_ietamin;
    int m_sc_ietamax;

    /// Offset between the first defined cell in the region and the point
    /// were offline and supercells are aligned.  This matters, for examine,
    /// in the first barrel region of sampling 1, for which the innermost strip
    /// doesn't exist, and the minimum eta for the region is 1.  However,
    /// at (the low edge) of the eta=1 cell, the offline cells and supercells
    /// don't align, so one needs to subtract one.
    int m_cell_ieta_adj;
    int m_sc_ieta_adj;
  };

  /// Add an entry to the region mapping table.
  void addMapEntry (const IDMapElt& elt);

  /// List of mapping table entries.
  std::vector<IDMapElt> m_idmap;

  /// Entry I contains the index in the mapping table of the first entry
  /// for the offline region with hash I, or -1 if there is no mapping entry.
  std::vector<int> m_offlineIndex;

  /// Entry I contains the index in the mapping table of the first entry
  /// for the supercell region with hash I, or -1 if there is no mapping entry.
  std::vector<int> m_superCellIndex;

  /// Entry I contains one past the index in the mapping table of the last
  /// entry for the supercell region with hash I,
  /// or -1 if there is no mapping entry.
  std::vector<int> m_superCellIndexEnd;

  /// hashTable for FCAL
  std::vector<Identifier> m_fcal_fromCell;
  std::vector< std::vector<Identifier> > m_fcal_fromSuperCell;

  /// Entry point for calorimeter ID helpers.
  const CaloCell_ID* m_cell_helper;
  const CaloCell_SuperCell_ID* m_sc_helper;
};


#endif // not CALODETDESCR_CALOSUPERCELLIDTOOL_H
