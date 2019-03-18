/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//--------------------------------------------------------------
// ivukotic@cern.ch added 3 functions for performace reasons.
// use them carefully
// 2 for updating begin and end calo iteerators
// and push back fast
// -------------------------------------------------------------

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "AthenaKernel/errorcheck.h"
#include "CLHEP/Geometry/Vector3D.h"
#include <boost/algorithm/cxx11/partition_point.hpp>
#include <atomic>


namespace {


/**
 * @brief Integer base-2 log.
 * @param x Number of which to take the log.  Must be > 0.
 * @return The base-2 log of x, rounded up.
 */
int ilog2 (size_t x)
{
  int l=0;
  --x;
  while (x >= 1) {
    ++l;
    x >>= 1;
  }
  return l;
}


std::atomic<int> nWarningsToBePrinted = 20;


} // anomymous namespace


CaloCellContainer::CaloCellContainer(SG::OwnershipPolicy ownPolicy ) : 
  DataVector<CaloCell>(ownPolicy),
  m_hasTotalSize(false),
  m_isOrdered(false),
  m_isOrderedAndComplete(false)
{ 
  //initialise with empty iterators
  for (int iCalo=0; iCalo< static_cast<int>(CaloCell_ID::NSUBCALO); ++iCalo){
    m_hasCalo.push_back(false);
    m_indexFirstCellCalo.push_back(-1);
    m_indexLastCellCalo.push_back(-2);
    m_beginConstCalo.push_back(this->begin());
    m_endConstCalo.push_back(this->begin());
    m_beginCalo.push_back(this->begin());
    m_endCalo.push_back(this->begin());
  }
}


CaloCellContainer::CaloCellContainer(size_t n, SG::OwnershipPolicy ownPolicy)
  : CaloCellContainer (ownPolicy)
{
  resize(n);
}


void CaloCellContainer::push_back(CaloCell * theCell)
{
  resetLookUpTable();

  // only allow const push_back if view cells
  // if (ownPolicy()==SG::VIEW_ELEMENTS){
  //  std::cout << "ERROR Should not push_back non const cell in VIEW CaloCellContainer" << std::endl;
  // }
  

  this->setHasCalo(theCell->caloDDE()->getSubCalo()) ;
  
  DataVector<CaloCell>::push_back(theCell)  ;

}

/* to speed up reading of compactifed data */
void CaloCellContainer::push_back_fast(CaloCell * theCell)
{
   DataVector<CaloCell>::push_back(theCell);
}


void CaloCellContainer::print() const {
  REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, "CaloCellContainer")
    << "CaloCellContainer::print not implemented";
}



CaloCellContainer::const_iterator CaloCellContainer::beginConstCalo(CaloCell_ID::SUBCALO caloNum) const {
  return m_beginConstCalo[static_cast<int>(caloNum)];
}

CaloCellContainer::iterator CaloCellContainer::beginCalo(CaloCell_ID::SUBCALO caloNum)
{
  return m_beginCalo[static_cast<int>(caloNum)];
}

CaloCellContainer::iterator CaloCellContainer::endCalo(CaloCell_ID::SUBCALO caloNum)
{
  return m_endCalo[static_cast<int>(caloNum)];
}

CaloCellContainer::const_iterator CaloCellContainer::endConstCalo(CaloCell_ID::SUBCALO caloNum) const {
  return m_endConstCalo[static_cast<int>(caloNum)];
}

int CaloCellContainer::indexFirstCellCalo(const CaloCell_ID::SUBCALO caloNum) const {
  return m_indexFirstCellCalo[static_cast<int>(caloNum)];
}

int CaloCellContainer::indexLastCellCalo(const CaloCell_ID::SUBCALO caloNum) const {
  return m_indexLastCellCalo[static_cast<int>(caloNum)];
}

int CaloCellContainer::nCellsCalo(const CaloCell_ID::SUBCALO caloNum) const {
  return this->indexLastCellCalo(caloNum)-this->indexFirstCellCalo(caloNum)+1;
}


bool CaloCellContainer::checkOrderedAndComplete() const {
  //should have maximum hash size in any case
  if (!this->hasTotalSize()) return false;
  
  CaloCellContainer::const_iterator itr=this->begin(),itrEnd=this->end();
  unsigned int index=0;
  for (;itr!=itrEnd;++itr){
    if ( ((*itr)->caloDDE())->calo_hash()!=index) { 
      return false;
    }
    ++index;
  }
  return true;
}
 
void CaloCellContainer::setIsOrderedAndComplete(const bool orderedAndComplete) {
  m_isOrderedAndComplete = orderedAndComplete ;
}

void CaloCellContainer::setHasTotalSize(const bool hasTotalSize){
  m_hasTotalSize = hasTotalSize;
}

bool CaloCellContainer::checkOrdered() const {
  CaloCellContainer::const_iterator itr=this->begin(),itrEnd=this->end();
  unsigned int hashPrev=0;
  unsigned int index=0;
  
  for (;itr!=itrEnd;++itr){

    const CaloCell * theCell=*itr;

    /* std::cout << "DRDR checking cell " << index << " " << hashPrev 
	      << " add itr  " << &(*itr) 
	      << " add cell " << theCell 
	      << std::endl; */

    const CaloDetDescrElement * theDDE = 0;
    
    if (theCell==0){
      REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, "CaloCellContainer")
        << MSG::WARNING << "CaloCellContainer NULL CELL ";
    } else {
	theDDE=theCell->caloDDE();
      if (theDDE==0){
        REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, "CaloCellContainer")
          << MSG::WARNING << "CaloCellContainer WARNING NULL DDE ";
      }
      else{
	unsigned int hash=theDDE->calo_hash();
	if 	( hash < hashPrev) return false ;
	hashPrev=hash;
      }
      
    }
    
    ++index;
    
  }
	
  return true;
}
 
void CaloCellContainer::setIsOrdered(const bool ordered) {
  m_isOrdered = ordered;
}


bool CaloCellContainer::hasCalo(const CaloCell_ID::SUBCALO caloNum) const {
  return m_hasCalo[static_cast<int>(caloNum)];
}

void CaloCellContainer::setHasCalo(const CaloCell_ID::SUBCALO caloNum) {

  const int iCalo=static_cast<int>(caloNum);

  m_hasCalo[iCalo] = true;
}


namespace {


struct is_calo
{
  is_calo (int c) : m_c (c) {}
  bool operator() (const CaloCell* cell) const
  { return cell->caloDDE()->getSubCalo() == m_c; }
  int m_c;
};


} // anonymous namespace


/**
 * @brief Recursively find division points between subcalos in the container.
 *
 * Should run in ~ m log n time, where n is the number of cells and m is the
 * number of subcalos.
 *
 * The cells need to be sorted for this to work.
 */
void
CaloCellContainer::updateCaloIteratorsOrdered (CaloCellContainer::iterator beg,
                                               CaloCellContainer::iterator end)
{
  if (beg == end) return;
  int beg_calo = (*beg)->caloDDE()->getSubCalo();
  int end_calo = (*(end-1))->caloDDE()->getSubCalo();
  if (beg_calo == end_calo) {
    m_beginCalo[beg_calo] = beg;
    m_endCalo[beg_calo] = end;
    m_beginConstCalo[beg_calo] = beg;
    m_endConstCalo[beg_calo] = end;
    m_indexFirstCellCalo[beg_calo] = beg - this->begin();
    m_indexLastCellCalo[beg_calo] = end - this->begin() - 1;
  }
  else {
    CaloCellContainer::iterator beg2 =
      boost::algorithm::partition_point (beg, end, is_calo (beg_calo));
    updateCaloIteratorsOrdered (beg, beg2);
    updateCaloIteratorsOrdered (beg2, end);
  }
}


void CaloCellContainer::updateCaloIterators()
{

  //initialise
  for (int iCalo=0; iCalo< static_cast<int>(CaloCell_ID::NSUBCALO); ++iCalo){
    m_indexFirstCellCalo[iCalo]=-1;
    m_indexLastCellCalo[iCalo]=-2;
    m_beginConstCalo[iCalo]=this->begin();
    m_endConstCalo[iCalo]=this->begin();
    m_beginCalo[iCalo]=this->begin();
    m_endCalo[iCalo]=this->begin();
  }

  if (m_isOrdered || m_isOrderedAndComplete) {
    updateCaloIteratorsOrdered (begin(), end());
    return;
  }

  CaloCellContainer::const_iterator itr=this->begin(),itrEnd=this->end();
  CaloCellContainer::iterator itrNonConst=this->begin();
  int previousCellCalo=-1;
  int index=0;
  for (;itr!=itrEnd;++itr){
    const int iCalo= static_cast<int>( (*itr)->caloDDE()->getSubCalo());
    if (iCalo!=previousCellCalo){
       
      // new calo domain
      if (m_indexFirstCellCalo[iCalo]>=0){
        REPORT_MESSAGE_WITH_CONTEXT (MSG::ERROR, "CaloCellContainer")
          << " Specific calo cells are split."
          << " Specific calo iterators will be wrong ";
      }
      m_indexFirstCellCalo[iCalo]=index;
      m_beginConstCalo[iCalo]=itr;
      m_beginCalo[iCalo]=itrNonConst;
	
      if (previousCellCalo>=0){ 
         m_indexLastCellCalo[previousCellCalo]=index-1;
	 m_endConstCalo[previousCellCalo]=itr;
	 m_endCalo[previousCellCalo]=itrNonConst;
	  
      }
    }
    previousCellCalo=iCalo;
    ++itrNonConst;   
    ++index;
  }

 //case the calo is the last filled
  if (previousCellCalo>=0){
 	m_indexLastCellCalo[previousCellCalo]=index-1;
	m_endConstCalo[previousCellCalo]=itr;
	m_endCalo[previousCellCalo]=itrNonConst;
  }

}


void CaloCellContainer::updateCaloBeginIterators(int ic,int ind) {
    if (ic==0) {
    	m_beginConstCalo[ic]=this->begin();
	m_beginCalo[ic]=this->begin();
	m_indexFirstCellCalo[ic]=0;
	}else{
	m_beginConstCalo[ic]=this->end();
        m_beginCalo[ic]=this->end(); 
        m_indexFirstCellCalo[ic]=ind;
	}
}

void CaloCellContainer::updateCaloEndIterators(int ic,int ind){
   	m_endConstCalo[ic]=this->end();
        m_endCalo[ic]=this->end(); 
        m_indexLastCellCalo[ic]=ind-1;
}


const CaloCell * CaloCellContainer::findCell(const IdentifierHash   theHash) const
{
  int ndx = findIndex (theHash);
  if (ndx >= 0)
    return (*this)[ndx];
  return 0;
}


 CaloCell * CaloCellContainer::findCell(const IdentifierHash   theHash)
{
  int ndx = findIndex (theHash);
  if (ndx >= 0)
    return (*this)[ndx];
  return 0;
}


int CaloCellContainer::findIndex(const IdentifierHash   theHash) const
{
  //cache Identifierhash->unsigned int conversion (maybe too much)
  const unsigned int theIndex = theHash ;  

  // if container is complete and in order use directly the container
  if (isOrderedAndComplete()) return theIndex;

  if (this->empty()) return -1;

  const CxxUtils::PackedArray& lookUpTable = getLookUpTable();
  if (theIndex < lookUpTable.size() )
    return (int)lookUpTable[theIndex] - 1;
  return -1;
}


/**
 * @brief Look up a group of cells by IdentifierHash.
 *        This is a templated version that can be instantiated for both
 *        const and non-const versions.
 * @param cont The container from which to fetch cells.
 * @param theVectorHash Vector of desired IdentifierHash's.
 * @param theCellVector Found cells will be appended to this vector.
 */
template <class CONT, class VECT>
void CaloCellContainer::findCellVectorT (CONT& cont, const std::vector<IdentifierHash> & theVectorHash, VECT& theCellVector)
{
  theCellVector.reserve (theCellVector.size() + theVectorHash.size());
  if (cont.isOrderedAndComplete()) {

    for (IdentifierHash hash : theVectorHash) {
      theCellVector.push_back( cont[hash] );
    }
    return;
  }

  if (cont.empty()) return ;

  const CxxUtils::PackedArray& lookUpTable = cont.getLookUpTable();

  //look in look up table (do not put empty cells)

  size_t tbsize = lookUpTable.size();
  for (IdentifierHash hash : theVectorHash) {
    // careful that the look up table might be shorter than the maximum hash
    if (hash>=tbsize) continue ;
    int ndx = (int)lookUpTable[ hash ] - 1;
    if (ndx >= 0) {
      auto theCell = cont[ndx];
      if (theCell!=0) {
        theCellVector.push_back( theCell);
      }
    }
    
  }
}


void CaloCellContainer::findCellVector(const std::vector<IdentifierHash> & theVectorHash,CellVector & theCellVector) const
{
  findCellVectorT (*this, theVectorHash, theCellVector);
#if 0
  theCellVector.reserve (theCellVector.size() + theVectorHash.size());
  if (isOrderedAndComplete()) {

    for (IdentifierHash hash : theVectorHash) {
      theCellVector.push_back( (*this)[hash] );
    }
    return;
  }

  if (this->empty()) return ;

  const CxxUtils::PackedArray& lookUpTable = getLookUpTable();

  //look in look up table (do not put empty cells)

  size_t tbsize = lookUpTable.size();
  for (IdentifierHash hash : theVectorHash) {
    // careful that the look up table might be shorter than the maximum hash
    if (hash>=tbsize) continue ;
    int ndx = (int)lookUpTable[ hash ] - 1;
    if (ndx >= 0) {
      const CaloCell * theCell = (*this)[ndx];
      if (theCell!=0) {
        theCellVector.push_back( theCell);
      }
    }
    
  }
#endif
}

void CaloCellContainer::findCellVector(const std::vector<IdentifierHash> & theVectorHash, MutableCellVector & theCellVector)
{
  findCellVectorT (*this, theVectorHash, theCellVector);
}

void CaloCellContainer::resetLookUpTable()
 { 
  m_lookUpTable.reset();
 }

void CaloCellContainer::initializeLookUpTable()
{
  this->resetLookUpTable();
  getLookUpTable();
}


void CaloCellContainer::order()
{
  if (this->hasTotalSize()){
    this->orderWhenComplete();
  }
  else{
    this->orderWhenIncomplete();
  }
  
}

void CaloCellContainer::orderWhenIncomplete() 
{
  // If the number of cells is small compared to the maximum number
  // of cells, best to just use the STL sort and then remove duplicates.
  // Otherwise, sort by inserting into a table based on the hash code.
  // Break point chosen pretty much arbitrarily.  Could be further
  // tuned, but it probably doesn't really make much difference.
  if (size() < 500) {
    // Small number of cells.  Use STL sort.
    sort (orderWithCaloHash());

    // Nb. Need to convert to int --- otherwise, 0-1 will wrap around
    // to maxuint.
    for (int i = 0; i < static_cast<int>(size())-1; ) {
      if ((*this)[i]->caloDDE()->calo_hash() ==
          (*this)[i+1]->caloDDE()->calo_hash())
      {
        REPORT_MESSAGE_WITH_CONTEXT (MSG::ERROR, "CaloCellContainer")
          << "Duplicated cell: hash= " << (*this)[i]->caloDDE()->calo_hash();
        erase (begin() + i);
      }
      else
        ++i;
    }
  }
  else {
    // Sort for large container.

    // Find the maximum possible hash code.
    const CaloDetDescriptor* desc = front()->caloDDE()->descriptor();
    IdentifierHash max_hash = desc->get_calo_helper()->calo_cell_hash_max();

    // Make a temporary container that can hold all possible cells.
    DataVector<CaloCell> tmp (max_hash+1, ownPolicy());

    // Move cells into the temporary container, according to hashes.
    DataVector<CaloCell>::iterator itend = end();
    for (DataVector<CaloCell>::iterator it = begin(); it != itend; ++it) {
      IdentifierHash hash = (*it)->caloDDE()->calo_hash();  
      assert (hash < tmp.size());
	
      if (tmp[hash] != 0) {
        REPORT_MESSAGE_WITH_CONTEXT (MSG::ERROR, "CaloCellContainer")
          << "Duplicated cell: hash= " << hash;
      }
      else {
        using std::iter_swap;
        iter_swap (tmp.begin() + hash, it);
      }
    }

    // Now, move the cells back, eliminating spaces.
    itend = tmp.end();
    DataVector<CaloCell>::iterator itout = begin();
    for (DataVector<CaloCell>::iterator it = tmp.begin(); it != itend; ++it) {
      if (*it != 0) {
        using std::iter_swap;
        iter_swap (itout, it);
        ++itout;
      }
    }

    // May be less, if there were duplicates.
    assert (itout <= end());
    resize (itout - begin());
  }
  initializeLookUpTable();
  setIsOrdered(true);
}

void CaloCellContainer::orderWhenComplete() 
{
  DataVector<CaloCell> tmp (ownPolicy());
  tmp.swap (*this);
  reserve (tmp.size());

  DataVector<CaloCell>::iterator tmpend = tmp.end();
  for (DataVector<CaloCell>::iterator it = tmp.begin(); it != tmpend; ++it) {
    IdentifierHash hash = (*it)->caloDDE()->calo_hash();

    if (hash >= size())
      resize (hash + 1);

    if ((*this)[hash] != 0) {
      REPORT_MESSAGE_WITH_CONTEXT (MSG::ERROR, "CaloCellContainer")
        << "Duplicated cell: hash= " << hash;
    }
    else {
      using std::iter_swap;
      iter_swap (begin() + hash, it);
    }
  }

  // set the Ordered And Complete flag
  this->setIsOrderedAndComplete(true);
  
  // look up table is not necessary anymore
  this->resetLookUpTable();
}

const CxxUtils::PackedArray& CaloCellContainer::getLookUpTable() const
{
  // FIXME should have an argument about duplicates
  // fill the look up table
  // note that if CaloCellContainer is not completely filled, there 
  // is no guarantee it has the correct size, hence the size should 
  // be checked at every call
  // (CaloCellContainer has no way to know the maximum size)
  // Now (30.07.2008) have it always max number of cells (hardcoded) to speed up.

  if (!m_lookUpTable.isValid()) {

    unsigned int theSize=0;
    size_t ncells = size();
    CxxUtils::PackedArray lookUpTable (std::max (1, ilog2 (ncells + 1)));

    if (ncells > 0) {

      // Set the size of the lookup table.
      // If the cells are sorted, we can look at the last cell to know
      // the maximum hash we're dealing with.  Otherwise, set it to the
      // maximum cell hash value.
      if (ncells == 0) {
        // Special case --- don't do anything for an empty container.
        theSize = 0;
      }
      else if (isOrdered()) {
        // Set LUT size from the hash of the last cell.
        theSize = back()->caloDDE()->calo_hash()+1;
      }
      else {
        // Find the maximum hash.
        const CaloDetDescriptor* desc = front()->caloDDE()->descriptor();
        IdentifierHash max_hash = desc->get_calo_helper()->calo_cell_hash_max();
        theSize = max_hash + 1;
      }
      lookUpTable.resize (theSize);
  
      for (size_t icell = 0; icell < ncells; icell++) {

        const CaloCell * theCell = (*this)[icell];
        const IdentifierHash theHash=theCell->caloDDE()->calo_hash();

        assert (theHash < theSize);
		
        if (nWarningsToBePrinted > 0) {
          // We may actually enter here nWarningsToBePrinted times
          // if multiple threads race here, but it doesn't matter.
          int nwarn = nWarningsToBePrinted--;
          int iold = (int)(lookUpTable[theHash])-1;
          if (iold >= 0) {
            if ((size_t)iold == icell) {
              REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, "CaloCellContainer")
                << "Repeated cell. hash= " << theHash;
            }
            else if ((*this)[iold]->ID()==theCell->ID()) {
              REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, "CaloCellContainer")
                << "Duplicated cell. hash=" 
                << theHash 
                << " E1=" << (*this)[iold]->e()
                << " E2=" << theCell->e();
              // should delete the cell?
            } else {
              REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING, "CaloCellContainer")
                << "Should be impossible ! Repeated cell. hash= " << theHash;
            }
            if (nwarn==0) {
              REPORT_MESSAGE_WITH_CONTEXT (MSG::INFO, "CaloCellContainer")
                << "No more WARNING will be printed";
            } 
          }
        } // end of print out

        lookUpTable[theHash]=icell+1;
      }
    }
    m_lookUpTable.set (std::move (lookUpTable));
  }

  return *m_lookUpTable.ptr();
}


bool CaloCellContainer::orderWithCaloHash::operator() ( const CaloCell * a ,const CaloCell * b ) const {
	  return a->caloDDE()->calo_hash() < b->caloDDE()->calo_hash(); }
bool CaloCellContainer::orderWithCaloHash::operator() ( const CaloCell & a ,const CaloCell & b ) const {
	  return a.caloDDE()->calo_hash() < b.caloDDE()->calo_hash(); }


namespace SG {


template <>
bool
findInContainer (const CaloCellContainer& data,
                 CaloCell const * const & element,
                 CaloCellContainer::size_type& index)
{
  int ndx = data.findIndex (element->caloDDE()->calo_hash());
  if (ndx >= 0) {
    index = ndx;
    return true;
  }
  return false;
}




} // namespace SG
