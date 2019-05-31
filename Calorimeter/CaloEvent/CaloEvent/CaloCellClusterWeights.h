// -*- c++ -*-
/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */

#ifndef CALOEVENT_CALOCELLCLUSTERWEIGHTS_H
#define CALOEVENT_CALOCELLCLUSTERWEIGHTS_H

#include "CLIDSvc/CLASS_DEF.h"

#include <string>
#include <vector>
#include <boost/tuple/tuple.hpp>

class CaloCell;

/// @brief Hash lookup of calibration weights for calorimeter cells
///
/// This object wraps a random access lookup of calibration weights based on the hash id of a calorimeter cell. It is meant to 
/// provide these weights to several tools, thus e.g. reducing the number of searches for cells in topo-clusters.
/// 
/// The data object reserves a fixed memory segment sufficient to store one weight per cell, and is of a fixed size which is defaulted to
/// the total number of calorimeter cells, but can also be set at construction. 
class CaloCellClusterWeights
{
public:

  /// @name Storage and payload types
  /// @{
  typedef std::vector<double>                 weight_t;        ///< @brief Type of weight
  typedef boost::tuples::tuple<bool,weight_t> value_t;         ///< @brief Type of payload
  typedef std::vector<value_t>                store_t;         ///< @brief Store type
  typedef store_t::const_iterator             const_iterator;  ///< @brief Iterator type for const access
  typedef store_t::iterator                   iterator;        ///< @brief Iterator type 
  /// @}

  /// @brief Default constructor
  ///
  /// The constructed data object provides a reserved and sized store appropriate for the total number of cells in the calorimeter.
  /// The store is pre-filled with the default payload value (0).
  CaloCellClusterWeights();
  /// @brief Constructor with client-defined size
  ///
  /// A store with the specified size is created and pre-filled with the default payload value (0).
  /// 
  /// @param size requested store size
  CaloCellClusterWeights(size_t size);
  /// @brief Copy constructor
  CaloCellClusterWeights(const CaloCellClusterWeights& cellClusterWeights);
  /// @brief Destructor 
  virtual ~CaloCellClusterWeights();

  /// @name Random access to payload
  /// @{
  /// @brief Accessing value using operator
  ///
  /// @return Stored value at given index if index is valid, else payload default value (0).
  /// @param  hash index (calorimeter cell hash id)
  const weight_t& operator[](size_t hash) const; 
  /// @brief Accessing value using function and hash
  ///
  /// @return Stored value at given index if index is valid, else payload default value (0).
  /// @param  hash index (calorimeter cell hash id)
  const weight_t& at(size_t hash) const;
  /// @brief Accessing value using function and cell pointer
  ///
  /// @return Stored value at given index if index is valid, else payload default value (0).
  /// @param  hash index (calorimeter cell hash id)
  const weight_t& at(const CaloCell* pCell) const;
  /// @brief Safe checking if cell is used by any cluster
  ///
  /// @return @c true if cell is part of any cluster and has valid hash index
  bool check(size_t hash) const;
  /// @brief Safe checking if cell is used by any cluster
  ///
  /// @return @c true if cell is part of any cluster and has valid hash index
  bool check(const CaloCell* pCell) const;
  /// @brief Fast checking if cell is used by any cluster
  ///
  ///  @return @c true if cell is part of any cluster
  bool fastCheck(size_t hash) const;
  /// @brief Fast checking if cell is used by any cluster
  ///
  ///  @return @c true if cell is part of any cluster
  bool fastCheck(const CaloCell* pCell) const;
  /// @}

  /// @name Store modifiers
  /// @{
  /// @brief Loading a value by hash id
  ///
  /// This method overwrites the stored value at the given index. It also sets the "used" flag in the store for the cell.
  /// @param hash index in store
  /// @param value value to be stored at index
  void set(size_t hash,double value);
  /// @brief Loading a value by cell pointer
  ///
  /// This method overwrites the stored value at the given index. It also sets the "used" flag in the store for the cell.
  /// @param hash index in store
  /// @param value value to be stored at index
  void set(const CaloCell* pCell,double value);
  /// @brief Reset the store
  ///
  /// All stored values are overwritten with the default payload value (0).
  void reset();                       
  /// @brief Clear the store
  ///
  /// All stored values are overwritten with the default payload value (0).
  void clear();
  /// @brief Clear a specific entry in the store
  ///
  /// The value stored at the given index is overwritten with the default payload value (0).
  /// @param hash index in store
  void clear(size_t hash);
  /// @}

  /// @name Store parameters
  /// @{
  size_t size() const;     ///< @brief Store size
  size_t reserve() const;  ///< @brief Reserved memory
  /// @}

  /// @name Iterators
  /// @{
  const_iterator begin() const; ///< @brief Constant access iterator pointing to beginning of the store
  const_iterator end() const;   ///< @brief Constant iterator indicating the end of the store
  iterator begin();             ///< @brief Access iterator pointing to beginning of the store
  iterator end();               ///< @brief Iterator indicating the end of the store
  /// @}

  /// @name Store access
  /// @{
  const store_t& store() const; ///< @brief Read-only access to underlying store 
  store_t& store();             ///< @brief Read/write access to underlying store 
  /// @}

  /// @brief Construct a SG key for this object
  static std::string key(const std::string& pref);

private:

  store_t _hashTable; ///< @brief Store implementation

  static weight_t _defaultValue; 
};

CLASS_DEF( CaloCellClusterWeights, 249044352, 1 )

inline CaloCellClusterWeights::const_iterator CaloCellClusterWeights::begin() const { return _hashTable.begin(); }
inline CaloCellClusterWeights::iterator       CaloCellClusterWeights::begin()       { return _hashTable.begin(); }

inline CaloCellClusterWeights::const_iterator CaloCellClusterWeights::end() const { return _hashTable.end(); }
inline CaloCellClusterWeights::iterator       CaloCellClusterWeights::end()       { return _hashTable.end(); }

inline size_t CaloCellClusterWeights::size()    const { return _hashTable.size(); }
inline size_t CaloCellClusterWeights::reserve() const { return _hashTable.capacity(); }

inline const CaloCellClusterWeights::store_t& CaloCellClusterWeights::store() const { return _hashTable; } 
inline       CaloCellClusterWeights::store_t& CaloCellClusterWeights::store()       { return _hashTable; } 

inline std::string CaloCellClusterWeights::key(const std::string& pref) { return pref+std::string("_CCW"); } 
#endif
