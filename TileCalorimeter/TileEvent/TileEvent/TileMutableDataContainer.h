// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/TileMutableDataContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Helper for holding non-const raw data prior to recording in SG.
 *
 * ??? Make this more generic??
 */


#ifndef TILEEVENT_TILEMUTABLEDATACONTAINER_H
#define TILEEVENT_TILEMUTABLEDATACONTAINER_H


#include "TileIdentifier/TileFragHash.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "Identifier/IdentifierHash.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/ILockable.h"
#include "GaudiKernel/StatusCode.h"
#include <vector>
#include <memory>


/**
 * @brief Helper for holding non-const raw data prior to recording in SG.
 *
 * Sometimes one wants to construct a tile raw data container and then,
 * before it is recorded in SG, modify the contents.  This is problematic
 * as the container classes only support const access to the contents.
 *
 * This helper derives from a specific tile raw data container and also maintains
 * non-const pointers to the collections, which we can return.
 * However, once the object has been made const in SG, we set a flag
 * so that non-const access is no longer possible.
 *
 * After constructing one of these objects, one should call status()
 * to check for errors.
 *
 * This class does not have its own CLID.  Record in SG via a handle
 * of type matching the base tile raw data container.
 */
template <class BASE_T>
class TileMutableDataContainer
  : public BASE_T, public ILockable
{
public:
  typedef BASE_T BASE;
  typedef typename BASE::TYPE TYPE;
  typedef typename BASE::UNIT UNIT;
  typedef typename BASE::IDENTIFIABLE Collection;
  typedef typename BASE::TElement     Element;


  /**
   * @brief Constructor.
   * @param createColl If true, create all collections now.
   * @param type Hash type (from TileFragHash::TYPE).
   * @param unit Measurement units for amplitude data.
   * @param ownPolicy Ownership mode for collections.
   *
   * Call status() after this to check for errors.
   */
  TileMutableDataContainer (bool createColl = false,
                            TYPE type=TileFragHash::Default,
                            UNIT unit=TileRawChannelUnit::ADCcounts,
                            SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS);


  /**
   * @brief Copy constructor.
   * @param other Container to copy.
   *
   * This is a deep copy; all contained collections and channels will be copied.
   * Call status() after this to check for errors.
   */
  TileMutableDataContainer (const BASE& other);


  TileMutableDataContainer (const TileMutableDataContainer&) = delete;
  TileMutableDataContainer& operator= (const TileMutableDataContainer&) = delete;


  /**
   * @brief Add a collection to the container.
   * @param coll Collection ot add.
   * @param hash Hash value for the collection.
   *
   * We maintain a non-const reference to the collection.
   */
  StatusCode addCollection (std::unique_ptr<Collection> coll,
                            IdentifierHash hash);


  // Const version is inherited.
  using BASE::addCollection;

  
  /**
   * @brief Add a new channel.
   * @param rch Channel to add.
   *
   * This should be used for owning container (created with SG::OWN_ELEMENTS).
   * A new collection will be created if needed.
   * In that case, we maintain a non-const reference to it.
   */
  StatusCode push_back (std::unique_ptr<Element> rch);


  /**
   * @brief Add a new channel.
   * @param rch Channel to add.
   *
   * This should be used for non-owning container (created with SG::VIEW_ELEMENTS).
   * A new collection will be created if needed.
   * In that case, we maintain a non-const reference to it.
   */
  StatusCode push_back (Element* rch);


  /**
   * @brief Look up a (non-const) collection via hash.
   * @param hash Hash value to find.
   */
  Collection* indexFindPtr (IdentifierHash hash);

  
  // Const version is inherited.
  using BASE::indexFindPtr;


  /**
   * @brief Return the error status from the constructors.
   */
  StatusCode status() const;

  
  /**
   * @brief Lock this object.
   *
   * Called when this object is locked in SG.
   * Prohibit non-const acces to this container.
   */
  virtual void lock() override;


protected:
  /**
   * @brief Recycle this object for use in another event.
   *
   * This is called from AthenaKernel/RecyclableDataObject when this object
   * is released by StoreGate.  Unlock the object so that non-const access
   * is again possible, and clear out the contents if the collections.
   */
  void recycle();

  
private:
  /// Non-const references to collections, indexed by hash value.
  std::vector<Collection*> m_mutableCollections;

  /// Is the container locked?
  bool m_locked;

  /// StatusCode from constructors.
  StatusCode m_sc;

  /// Default type; reset to this on recycle.
  TYPE m_defaultType;

  /// Default unit; reset to this on recycle.
  UNIT m_defaultUnit;
};


#include "TileEvent/TileMutableDataContainer.icc"


#endif // not TILEEVENT_TILEMUTABLEDATACONTAINER_H
