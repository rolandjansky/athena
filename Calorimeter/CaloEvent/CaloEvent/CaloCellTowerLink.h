// -*- c++ -*-
/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */

#ifndef CALOEVENT_CALOCELLTOWERLINK_H
#define CALOEVENT_CALOCELLTOWERLINK_H

#include "CLIDSvc/CLASS_DEF.h"

#include <string>
#include <vector>
#include <boost/tuple/tuple.hpp>

class CaloCell;
class IdentifierHash;

/// @brief Data objects storing cell-to-tower relation 
namespace CaloCellTowerLink {
  /// @name Basic payload describing cell-to-tower link
  /// @{
  typedef size_t                                 index_t;      ///> @brief Index type 
  typedef double                                 weight_t;     ///> @brief Weight type
  typedef boost::tuples::tuple<index_t,weight_t> base_t;       ///> @brief Payload for look-up
  typedef std::vector<index_t>                   index_vec_t;  ///> @brief Index vector type  
  typedef std::vector<weight_t>                  weight_vec_t; ///> @brief Weight vector type
  /// @brief Payload for lookup
  class Element : public base_t
  {
  public:
    /// @brief Default constructor
    Element();
    /// @brief Constructor with payload
    Element(index_t idx,weight_t wght);
    /// @name Accessors
    /// @{
    index_t index() const;    ///< @brief Tower index 
    weight_t weight() const;  ///< @brief Geometrical weight of cell in tower
    /// @}
  };
  /// @}
  /// @name Unpack payload
  /// @{
  static inline index_t  towerIndex(const Element& element) { return element.index();  } ///> @brief Unpack index from payload
  static inline weight_t cellWeight(const Element& element) { return element.weight(); } ///> @brief Unpack geometrical weight of cell in tower from payload
  /// @}
  /// @brief Container for 1-to-many cell-to-tower relations
  typedef std::vector<Element> ElementList;
  /// @brief Lookup for cell-to-tower relations
  typedef std::vector<ElementList> ElementLookup;
  /// @brief storage element tag 
  static const std::string extTag = "_CellLinks";
  
  /// @brief Map for all cell-to-tower relations
  class Map : public ElementLookup
  {
  public:
    /// @brief Default constructor
    Map();
    /// @brief Constructor with size specification
    Map(size_t size); 
    /// @brief Retrieve reference to list of cell-to-tower links for a given cell by pointer
    const CaloCellTowerLink::ElementList& elementList(const CaloCell* pCell) const;
    /// @brief Retrieve reference to list of cell-to-tower links for a given cell by hash id
    const CaloCellTowerLink::ElementList& elementList(IdentifierHash hashId) const;
    /// @brief Add cell-to-tower link to map using cell pointer
    void addCell(const CaloCell* pCell,index_t towerIndex,weight_t weight);
    /// @brief Add cell-to-tower link to map using cell hash id
    void addCell(IdentifierHash hashId,index_t towerIndex,weight_t weight); 
    /// @brief Add cell-to-many-tower links to map using cell pointer
    bool setCell(const CaloCell* pCell,const index_vec_t& towerIndices,const weight_vec_t& weights);
    /// @brief Add cell-to-many-tower links to map using cell hash id
    bool setCell(IdentifierHash hashId,const index_vec_t& towerIndices,const weight_vec_t& weights);
    /// @brief Reset map
    void reset();
    // /// @brief Size of lookup 
    // size_t size() const;
    // /// @brief Lookup is empty
    // bool empty() const;
    // /// @brief Clear
    // void clear();
  private:
    // /// @brief Lookup table
    //  std::vector<CaloCellTowerLink::ElementList> m_map;
    /// @brief Empty reference
    static CaloCellTowerLink::ElementList m_emptyRef;
  };
}

// inline size_t CaloCellTowerLink::Map::size() const   { return m_map.size(); }
// inline bool   CaloCellTowerLink::Map::empty() const  { return m_map.empty(); }
// inline void   CaloCellTowerLink::Map::clear()        { m_map.clear(); }

//typedef CaloCellTowerLink::Map CaloCellTowerLinkMap;

CLASS_DEF( CaloCellTowerLink::Map , 38415362 , 1 )
//CLASS_DEF( CaloCellTowerLink::Element, 94402964, 1 ) not needed

#endif
