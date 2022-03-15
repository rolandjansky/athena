/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOEVENT_CALOCLUSTERCELLLINK_H
#define CALOEVENT_CALOCLUSTERCELLLINK_H

#include "AthLinks/DataLink.h"
#include "CaloEvent/CaloCellContainer.h"
#include <map> 
#include <iterator>

/** 
 *  @class CaloClusterCellLink
 *  @author Walter Lampl
 *  @brief Bookkeeping of cells that make up a cluster 
 *  Simplified replacement for CaloCellLink, CaloClusterNavigable and Navigable classes 
 *  used during run 1.
 */



class CaloClusterCellLink {

 public:
  typedef double weight_t;

 private:
  //Typedef std::map<unsigned,weight_t> linkAndWeightCollType;
  typedef std::vector<std::pair<unsigned, weight_t> > linkAndWeightCollType;

 public:
  /** @brief default constructor (for persistency only)*/
  CaloClusterCellLink();

 /**@brief useful constructor 
  * @param[in] cellCont Pointer to the CaloCellContainer from which this cluster is built
  */
  CaloClusterCellLink(const CaloCellContainer* cellCont);

 /**@brief Constructor from @c DataLink.
  * @param[in] cellCont Link to the CaloCellContainer from which this cluster is built
  */
  CaloClusterCellLink(const DataLink<CaloCellContainer>& cellCont);

  
  /**@brief standard destructor 
   */
  ~CaloClusterCellLink(); 

  /** 
   *  @class CaloClusterCellLink::const_iterator
   *  @brief const_iterator to loop over cells belonging to a cluster
   */
  class const_iterator
  {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = const CaloCell*;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    const_iterator() = delete; //C++11 

    /**@brief constructor *
     * @param[in] cellCont Pointer to the CaloCellContainer
     * @param[in] it map-iterator to the map insinde CaloClusterLinkContainer 
     */
    const_iterator(const CaloCellContainer* cellCont, linkAndWeightCollType::const_iterator it) :
      m_ccc(cellCont), m_it(it) {};

    /**@brief dereferencing operator
     * @return pointer to CaloCell
     */
    const CaloCell* operator*() const {return (*m_ccc)[m_it->first];}
    const CaloCell* operator->() const {return (*m_ccc)[m_it->first];}

    /**@brief Accessor for weight associated to this cell
     */
    weight_t weight() const {return m_it->second;}

    /**@brief Accessor for the index of the cell in the CaloCellContainer
     */
    unsigned index() const {return m_it->first;}

    const_iterator operator++() {++m_it; return *this;}
    const_iterator operator--() {--m_it; return *this;}
    const_iterator operator++(int) {++m_it; return *this;}
    const_iterator operator--(int) {--m_it; return *this;}
    bool operator==(const const_iterator& b) const { return m_it==b.m_it;}
    bool operator!=(const const_iterator& b) const { return m_it!=b.m_it;}
    
  private:
    const CaloCellContainer* m_ccc;
    linkAndWeightCollType::const_iterator m_it;
  };//end class CaloClusterCellLink::const_iterator


  /// Dummy iterator to be used as begin/end if no cell-links are present
  const static CaloClusterCellLink::const_iterator dummyIt;

  /**@brief const begin method
   * @return const_iterator to the first cell belonging to this cluster
   */
  const_iterator begin() const {return const_iterator(m_cellCont, m_indicesAndWeights.begin());} 

   /**@brief const end method
   * @return const_iterator end iterator
   */
  const_iterator end() const {return const_iterator(m_cellCont, m_indicesAndWeights.end());} 

  /**@brief size method
   *@return The number of cells 
   */
  size_t size() const {return m_indicesAndWeights.size();}

  //Non-const interface:

  /** 
   *  @class CaloClusterCellLink::const_iterator
   *  @brief const_iterator to loop over cells belonging to a cluster
   */
  class iterator
  {
    friend class CaloClusterCellLink;
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = const CaloCell*;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    iterator() = delete;

    /**@brief constructor *
     * @param[in] cellCont Pointer to the CaloCellContainer
     * @param[in] it map-iterator to the map insinde CaloClusterLinkContainer 
     */
  iterator(const CaloCellContainer* ccc, linkAndWeightCollType::iterator it) :
    m_ccc(ccc), m_it(it) {};
    const CaloCell* operator*() const {return (*m_ccc)[m_it->first];}
    const CaloCell* operator->() const {return (*m_ccc)[m_it->first];}
    /**@brief Accessor for weight associated to this cell
     */
    weight_t weight() const {return m_it->second;}

    /**@brief Accessor for the index of the cell in the CaloCellContainer
     */
    unsigned index() const {return m_it->first;}

    iterator operator++() {++m_it; return *this;}
    iterator operator--() {--m_it; return *this;}
    iterator operator++(int) {++m_it; return *this;}
    iterator operator--(int) {--m_it; return *this;}
    bool operator==(const iterator& b) const { return m_it==b.m_it;}
    bool operator!=(const iterator& b) const { return m_it!=b.m_it;}

    //Non-const manipulations:
    /**@brief Update the weight 
     * @param newWeight The new weight
     */
    void reweight(const weight_t newWeight) {m_it->second=newWeight;}
 
  private:
    const CaloCellContainer* m_ccc;
    linkAndWeightCollType::iterator m_it;
  };//end class CaloClusterCellLink::iterator

 /**@brief non-const begin method
   * @return begin iterator
   */
  iterator begin() {return iterator(m_cellCont, m_indicesAndWeights.begin());} 

  /**@brief const end method
   * @return end iterator
   */
  iterator end() {return iterator(m_cellCont, m_indicesAndWeights.end());} 

 
  /**@brief Method to add a cell to the cluster
   * @param cellIdx Index of the cell in the CaloCellContainer
   * @param weight cell weight
   * @return true if this cell was added (always in this implementation)
   */
  bool addCell(const unsigned cellIdx, const weight_t weight=1.0);

   /**@brief Method to remove a cell 
   * @param cellItr Iterator pointing to the cell to be removed
   * @return iterator to the cell following the removed one
   */
  CaloClusterCellLink::iterator removeCell(iterator cellItr);
  

  /**@brief Method to remove a cell by pointer (slow!)
   * @param pointer to the cell in teh cell container
   * @return True if the cell was removed, False if it was not found.
   */
  bool removeCell(const CaloCell* ptr);
  

 /**@brief Method to access underlying cell container
   * @return const pointer to CaloCellContainer
   */
  const CaloCellContainer* getCellContainer() const { return m_cellCont.cptr();}
  const DataLink<CaloCellContainer>& getCellContainerLink() const { return m_cellCont;}

  /// Method to reserve space the underlying vector<pair> 
  void reserve(const size_t s) { m_indicesAndWeights.reserve(s);}


  /// Method to remove all cells;
  void clear() {m_indicesAndWeights.clear();}

  /**@brief Prepare for writing.
   */
  void toPersistent(); 

 private:
  DataLink<CaloCellContainer> m_cellCont;
  linkAndWeightCollType m_indicesAndWeights;

  const static linkAndWeightCollType m_dummyIndicesAndWeights; //0-size vector for dummy iterator

};

inline bool CaloClusterCellLink::addCell(const unsigned cellIdx, const weight_t weight) {

  /* map version - with uniqueness check
  std::pair<linkAndWeightCollType::iterator,bool> tryInsert=
    m_indicesAndWeights.insert(std::make_pair(cellIdx,weight));
  if (!tryInsert.second) {
    //Cell existed before, need to overwrite
    tryInsert.first->second=weight;
  }
  return !tryInsert.second;
  */
  m_indicesAndWeights.emplace_back(cellIdx,weight);
  return true;

}

#endif


