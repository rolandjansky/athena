// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file CaloEvent/CaloTester.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Helpers for creating calorimeter objects for unit tests.
 */


#ifndef CALOEVENT_CALOTESTER_H
#define CALOEVENT_CALOTESTER_H


#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloHelpersTest.h"
#include <memory>
#include <vector>


/**
 * @brief Helpers for creating calorimeter objects for unit tests.
 *
 * Helpers to create ID helpers, detector description, and cells
 * for use in unit tests, etc.
 * Builds an idealized geometry.
 *
 * Warning: DD built here is known to be incorrect for tile/fcal.
 * Some parts of DD are likely to be incomplete for all subcalorimeters.
 * Use with care.
 */
class CaloTester
{
public:
  /**
   * @brief Constructor.
   *        The constructor will create the ID helpers.
   *        A CaloDetDescrManager instance will also be created, but it will
   *        not be recorded in the detector store unless @c record_mgr
   *        is called.
   */
  CaloTester();


  /**
   * @brief Record the CaloDetDescrManager in the detector store.
   *        This may create the detector store as a side effect.
   */
  StatusCode record_mgr();


  /**
   * @brief Return a region descriptor corresponding to a given cell.
   * @param hashid Cell hash id.
   *
   * The desciptor will be created if needed and registered with the manager.
   */
  const CaloDetDescriptor* find_dd (int hashid);


  /**
   * @brief Return a region descriptor corresponding to a given cell.
   * @param reg_id Cell identifier.
   *
   * The desciptor will be created if needed and registered with the manager.
   */
  const CaloDetDescriptor* find_dd (const Identifier& cell_id);


  /**
   * @brief Create a calorimeter cell.
   * @param hashid Cell hash id.
   */
  std::unique_ptr<CaloCell> make_cell (int hashid);


  /**
   * @brief Return a vector of all cells.
   *        The cells remain owned by the Tester object.
   */
  std::vector<CaloCell*> get_cells();


  /**
   * @brief Return a vector of all cells as const pointers.
   *        The cells remain owned by the Tester object.
   */
  std::vector<const CaloCell*> get_const_cells();


  /**
   * @brief Return a CaloCellContainer for all cells.
   *        This will be a view container;
   *        the cells remain owned by the Tester object.
   */
  std::unique_ptr<CaloCellContainer> make_ccc();


  /// Return the CaloDetDescrManager.
  CaloDetDescrManager&   mgr() { return *m_mgr; }


  /// Return the ID helpers.
  const CaloHelpersTest& helpers() const { return m_helpers; }


  /// Return the LArEM ID helper.
  const LArEM_ID&        emID() const { return m_helpers.emID(); }


  /// Return the LArHEC ID helper.
  const LArHEC_ID&       hecID() const { return m_helpers.hecID(); }


  /// Return the LArFCAL ID helper.
  const LArFCAL_ID&      fcalID() const { return m_helpers.fcalID(); }


  /// Return the LArMiniFCAL ID helper.
  const LArMiniFCAL_ID&  minifcalID() const { return m_helpers.minifcalID(); }


  /// Return the Tile ID helper.
  const TileID&          tileID() const { return m_helpers.tileID(); }


  /// Return the Calorimeter ID helper.
  const CaloCell_ID&     caloID() const { return m_helpers.caloID(); }


private:
  /**
   * @brief Create all calo cells and save in internal list.
   */
  void make_cells();

  /// The ID helpers.
  CaloHelpersTest m_helpers;

  /// All Tile detdescr elements.  The manager does not take
  /// ownership of these.
  std::vector<std::unique_ptr<CaloDetDescrElement> > m_tileddes;

  /// The cells made by @c make_cells().
  std::vector<std::unique_ptr<CaloCell> > m_cells;

  /// The manager object.
  CaloDetDescrManager* m_mgr;

  /// Owning pointer to the manager object.
  /// This is set at the start, and cleared if the object is recorded in SG.
  std::unique_ptr<CaloDetDescrManager> m_mgr_up;
};


#endif // not CALOEVENT_CALOTESTER_H
