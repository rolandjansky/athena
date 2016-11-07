/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCellLinkContainerCnv_p2.cxx,v 1.9 2009-02-18 22:50:08 hma Exp $
/* @file  CaloCellLinkContainerCnv_p2.cxx
 * @author Ilija Vukotic <ivukotic@cern.ch>, scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief T/P conversions for CaloCellLinkContainerCnv_p2.
 */


#include "DataModel/ElementLink.h"
#include "CaloTPCnv/CaloCellLinkContainerCnv_p2.h"
#include "AthenaKernel/errorcheck.h"

#include "AthenaKernel/IThinningSvc.h"
#include "CxxUtils/fpcompare.h"

#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;


namespace {

     
/**
 * @brief Helper class, to hold index and weight for one cell.
 */
struct cell_t
{
  cell_t (int the_cell = 0,
          float the_weight = 0)
    : cell (the_cell), weight (the_weight) {}
  bool operator< (const cell_t& other) const { return cell < other.cell; }
  bool operator== (const cell_t& other) const { return cell == other.cell; }

  /// Cell index.
  int cell;

  /// Cell weight.
  float weight;
};


/// Scale factor for saving number of weights in sequence.
const float NWEIGHT_SCALE = 1000;

/// Bit shift for storing number of bits in sequence.
const int INDEX_BITS = 18;

/// Bit mask to get cell index.
const unsigned int INDEX_MASK = (1<<INDEX_BITS) - 1;

/// Number of bits for storing cells-in-sequence count.
const int ISEQ_BITS = 32 - INDEX_BITS;

/// Bit mask for cells-in-sequence count.
const unsigned int ISEQ_MAX = (1 << ISEQ_BITS) - 1;


} // anonymous namespace


/**
 * @brief Convert from persistent to transient object.
 * @param pers The persistent object to convert.
 * @param trans The transient object to which to convert.
 * @param log Error logging stream.
 */
void
CaloCellLinkContainerCnv_p2::persToTrans(const CaloCellLinkContainer_p2* pers,
                                         CaloCellLinkContainer* trans,
                                         MsgStream &/*log*/)
{
  trans->clear();
  trans->resize(pers->m_nClusters);

  if (pers->m_nClusters > pers->m_vISizes.size() ||
      pers->m_nClusters > pers->m_vWSizes.size())
  {
    REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING,
                                 "CaloCellLinkContainerCnv_p2")
      << "Inconsistent sequence array lengths: "
      << pers->m_nClusters
      << " " << pers->m_vISizes.size()
      << " " << pers->m_vWSizes.size() << "\n";
    return;
  }

  std::vector<unsigned>::const_iterator cIterI = pers->m_linkI.begin();
  std::vector<unsigned>::const_iterator cIterIe = pers->m_linkI.end();
  std::vector<float>::const_iterator cIterW = pers->m_linkW.begin();
  std::vector<float>::const_iterator cIterWe = pers->m_linkW.end();
  
  for (unsigned int iCluster=0; iCluster<pers->m_nClusters; iCluster++) {

    std::vector<float>::const_iterator cIterWb = cIterW;

    bool bad = false;
    CaloCellLink* lnk=new CaloCellLink();
    trans->at(iCluster)=lnk; // puts an empty link == cluster

    float weight = 0;
    unsigned int nw = 0;

    // Get number of cells and pre-allocate vector.
    unsigned int ncells = 0;
    for(unsigned iCell=0; iCell<pers->m_vISizes[iCluster]; iCell++) {
      if (cIterI >= cIterIe) {
        REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING,
                                     "CaloCellLinkContainerCnv_p2")
          << "Cell index array overrun";
        bad = true;
        goto endcells;
      }
      ncells += (*(cIterI+iCell)) >> INDEX_BITS;
    }
    if (ncells > 1000) ncells = 1000; // Don't go crazy.
    lnk->m_constituents.reserve (ncells);

    // Unpacking indices and making links
    for(unsigned iCell=0; iCell<pers->m_vISizes[iCluster]; iCell++) {
      unsigned int compositeI = *cIterI++;
      unsigned int realI = compositeI & INDEX_MASK;
      unsigned int lSEQ = compositeI>>INDEX_BITS;
      for(unsigned seq=0;seq<lSEQ;seq++) {
        if (nw == 0) {
          if (cIterW >= cIterWe) {
            REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING,
                                         "CaloCellLinkContainerCnv_p2")
              << "Cell weight array overrun";
            bad = true;
            goto endcells;
          }
          float compositeW = *cIterW++;
          nw = static_cast<unsigned int> (compositeW*(1./NWEIGHT_SCALE));
          weight = compositeW - (nw*NWEIGHT_SCALE);
        }

        ElementLink<CaloCellContainer> el_link(pers->m_contName, realI+seq);
        lnk->m_constituents.push_back(std::make_pair (el_link,weight));
        --nw;
      }
    }//End loop over cells in cluster

    if (nw != 0) {
      REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING,
                                   "CaloCellLinkContainerCnv_p2")
        << "Weight/index arrays out of sync";
      bad = true;
    }

    if (cIterW - cIterWb != static_cast<int> (pers->m_vWSizes[iCluster])) {
      REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING,
                                   "CaloCellLinkContainerCnv_p2")
        << "Wrong number of weight sequences";
      bad = true;
    }
  endcells:

    if (lnk->size() > 0 && pers->m_contName.empty()) {
      REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING,
                                   "CaloCellLinkContainerCnv_p2")
        << "Missing cell container name";
      bad = true;
    }
  
    if (bad) {
      REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING,
                                   "CaloCellLinkContainerCnv_p2")
        << "Discarding cluster cells";
      lnk->m_constituents.clear();
    }
    
  } // End loop over clusters
}

	
/**
 * @brief Convert from transient to persistent object.
 * @param trans The transient object to convert.
 * @param pers The persistent object to which to convert.
 * @param log Error logging stream.
 */
void
CaloCellLinkContainerCnv_p2::transToPers(const CaloCellLinkContainer* trans,
                                         CaloCellLinkContainer_p2* pers,
                                         MsgStream & /*log*/)
{
  unsigned int nclus = trans->size();
  pers->m_nClusters = nclus; // number of clusters in container

  // Gives it size of trans container ->
  // will store number of indices per cluster.
  pers->m_vISizes.resize(nclus);
  pers->m_vWSizes.resize(nclus);

  // Guess.
  pers->m_linkI.reserve (5*nclus);
  pers->m_linkW.reserve (5*nclus);

  pers->m_contName.clear();
  SG::sgkey_t first_key = 0;


  IThinningSvc * thinningSvc = IThinningSvc::instance();
  const bool thinning = thinningSvc && thinningSvc->thinningOccurred();

  for (unsigned int iCluster = 0; iCluster < nclus; ++iCluster) {

    // This is actually a Navigable - contains links to the cells.
    const CaloCellLink& lnk = *(*trans)[iCluster];

    // ??? Making this static saves memory allocations, but means
    // we're not thread-safe.
    static std::vector<cell_t> cells;
    cells.clear();
    cells.reserve (lnk.size()+1);
    CaloCellLink::cell_iterator it_cell   = lnk.begin();
    CaloCellLink::cell_iterator it_cell_e = lnk.end();
	
    for (;it_cell!=it_cell_e;it_cell++) {  // Loop over cells in cluster.
      Navigable<CaloCellContainer,double>::constituent_const_iter citer =
        it_cell.getInternalIterator();

      // Put stuff into array for later sorting.
      float weight = citer->second;
      // Trying to store weights > NWEIGHT_SCALE will lead to corrupted data.
      if (fabs (weight) > NWEIGHT_SCALE) {
        REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING,
                                     "CaloCellLinkContainerCnv_p2")
          << "Crazy cluster cell weight " << weight
          << " reset to 1";
        weight = 1;
      }

      size_t index = citer->first.index();
  
      if(thinning){
        const SG::DataProxy* dp  = citer->first.proxy();
	const std::size_t persIdx = dp ?
	            thinningSvc->index(dp, index) :
	            thinningSvc->index(citer->first.getStorableObjectPointer(), index);

	if( IThinningSvc::RemovedIdx == persIdx ) {

	  //REPORT_MESSAGE_WITH_CONTEXT (MSG::DEBUG,
          //                           "CaloCellLinkContainerCnv_p2")
          // << " Cell index has been thinned away, skip all cells  " ;

	  cells.clear();
	  break; 

	}else
	  {
	    //REPORT_MESSAGE_WITH_CONTEXT (MSG::DEBUG, 
            //                         "CaloCellLinkContainerCnv_p2")
	     //			       << " cell index changed from  "<< index << " to " <<persIdx <<endmsg ;
            index = persIdx;
	  }
      }

      SG::sgkey_t key = citer->first.key();
      citer->first.source()->tryELRemap (key, index, key, index);

      if (pers->m_contName.empty()) {
        pers->m_contName = *citer->first.source()->keyToString (key);
        first_key = key;
      }
      else if (first_key != key) {
        REPORT_MESSAGE_WITH_CONTEXT (MSG::ERROR,
                                     "CaloCellLinkContainerCnv_p2")
          << "Multiple container names seen; "
          << "cannot be persistified correctly: "
          << pers->m_contName
          << " != " << *citer->first.source()->keyToString (key)
          << "; cell skipped";
        continue;
      }

      if ((index & ~INDEX_MASK) != 0) {
        REPORT_MESSAGE_WITH_CONTEXT (MSG::WARNING,
                                     "CaloCellLinkContainerCnv_p2")
          << "Bad cell index " << index
          << " had high bits masked off.";
        index &= INDEX_MASK;
      }
      cells.push_back (cell_t (index, weight));

    } // End loop over cells

    // Sort links according to cell indices.
    std::sort (cells.begin(), cells.end());

    unsigned int& nlseq = pers->m_vISizes[iCluster];
    unsigned int& nwseq = pers->m_vWSizes[iCluster];
    nlseq = 0;
    nwseq = 0;
    int last_cell = -9999;
    float last_weight = -9999;
    unsigned int nl_in_seq = 0;
    unsigned int nw_in_seq = 0;

    // Sentinel.
    cells.push_back (cell_t (-99999, -99999));
    
    size_t ncells = cells.size();
    for (size_t i = 0; i < ncells; ++i) {
      const cell_t& cell = cells[i];

      if (static_cast<int>(last_cell + nl_in_seq) == cell.cell &&
          nl_in_seq < ISEQ_MAX)
      {
        ++nl_in_seq;
      }
      else {
        if (nl_in_seq) {
          pers->m_linkI.push_back (last_cell | (nl_in_seq << INDEX_BITS));
          ++nlseq;
        }
        last_cell = cell.cell;
        nl_in_seq = 1;
      }

      // Don't let the weight sequence length get too large,
      // or we'll lose precision on the weight.  A limit of 255
      // allows the weight to be represented within ~0.02.
      //
      // We also need to be careful comparing weights.
      // Because we lose precision due to adding the sequence length,
      // it's possible to have a case where last_weight != cell.weight,
      // but they're equal after packing.  That means that if we
      // then copy the data, the persistent data won't be exactly
      // the same.  Try to take this into account in the comparison.
      float w1 = (nw_in_seq+1)*NWEIGHT_SCALE + last_weight;
      float w2 = (nw_in_seq+1)*NWEIGHT_SCALE + cell.weight;
      using CxxUtils::fpcompare::equal;
      if (equal (nextafterf (w1, w2), w2) && nw_in_seq < 255)
      {
        ++nw_in_seq;
      }
      else {
        if (nw_in_seq) {
          pers->m_linkW.push_back (nw_in_seq*NWEIGHT_SCALE + last_weight);
          ++nwseq;
        }
        last_weight = cell.weight;
        nw_in_seq = 1;
      }
    }
  } // End loop over clusters
}


/**
 * @brief Convert from persistent to transient object, with untyped pointers.
 * @param pers The persistent object to convert.
 * @param trans The transient object to which to convert.
 * @param log Error logging stream.
 */
void CaloCellLinkContainerCnv_p2::persToTransUntyped(const void* pers,
                                                     void* trans,
                                                     MsgStream& log)
{
  persToTrans (reinterpret_cast<const CaloCellLinkContainer_p2*> (pers),
               reinterpret_cast<CaloCellLinkContainer*> (trans),
               log);
}


/**
 * @brief Convert from transient to persistent object, with untyped pointers.
 * @param trans The transient object to convert.
 * @param pers The persistent object to which to convert.
 * @param log Error logging stream.
 */
void CaloCellLinkContainerCnv_p2::transToPersUntyped(const void* trans,
                                                     void* pers,
                                                     MsgStream& log)
{
  transToPers (reinterpret_cast<const CaloCellLinkContainer*> (trans),
               reinterpret_cast<CaloCellLinkContainer_p2*> (pers),
               log);
}


/**
 * @brief Return the @c std::type_info for the transient type.
 */
const std::type_info& CaloCellLinkContainerCnv_p2::transientTInfo() const
{
  return typeid (CaloCellLinkContainer);
}

/** return C++ type id of the persistent class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloCellLinkContainerCnv_p2::persistentTInfo() const
{
  return typeid (CaloCellLinkContainer_p2);
}


