/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloTowerStore
PACKAGE:  offline/Calorimeter/CaloUtils

AUTHORS:  S. Laplace
CREATED:  May 2005

PURPOSE:  Intermediate store for cell/tower maps

Updated:  Ilija Vukotic August 2008. 
		  basically rewrote it completely.	
		  add reverse lookup in order to speed things up
		  When I see how many linebreaks some people use I think of two things:
		  	1. they programm on their mobile phone
			2. they are paid per line of programm :-)

********************************************************************/
#include "CaloUtils/CaloTowerStore.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
// include header files
#include "Identifier/IdentifierHash.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include <cmath>
#include <utility>
#include <limits>
#include <cassert>
////////////////////////////////
// Constructor and Destructor //
////////////////////////////////

// constructor
CaloTowerStore::CaloTowerStore(){}

// destructor
CaloTowerStore::~CaloTowerStore(){}

////////////////////////
// Build LookUp Table //
////////////////////////


/**
 * @brief Return an iterator for looping over all towers
 *        defined by the window @c subseg.
 *
 *        No end iterator is defined; use subseg.size()
 *        to tell when to stop the iteration.
 *
 *        The iteration may not be in tower index order.
 *        Use the @c itower() method of the iterator
 *        to find the current tower index.
 */
CaloTowerStore::tower_subseg_iterator
CaloTowerStore::towers (const CaloTowerSeg::SubSeg& subseg) const
{
  checkEntryIndex();
  return tower_subseg_iterator::make (towers(), subseg);
}


bool CaloTowerStore::buildLookUp(const CaloDetDescrManager& theManager,
                                 const CaloTowerSeg& theTowerSeg,
                                 const std::vector<CaloCell_ID::SUBCALO>& theCalos)
{
  m_seg = theTowerSeg;
  m_entries.clear();
  m_towers.clear();
  m_weights.clear();
  m_entry_index.store (std::vector<unsigned short>());
  m_entry_index.reset();

  ///////////////////////
  // Store Preparation //
  ///////////////////////

  // messaging
  IMessageSvc* theMsgSvc;
  StatusCode sc = Gaudi::svcLocator()->service("MessageSvc",theMsgSvc);
  if(sc.isFailure()){
    std::cout <<  "Cannot locate MessageSvc" << std::endl;
  }
  MsgStream msg(theMsgSvc,"CaloTowerStore");

  // get cell description manager
  if ( ! theManager.isInitialized() ){
	msg << MSG::ERROR<< "CaloDetDescrManager is not initialized, module unusable!"<< endmsg;
	return false;
	}
  const CaloCell_ID& cellIdHelper = *theManager.getCaloCell_ID();

  // Get list of Calos to be considered
  size_t sizeCalos = theCalos.size();

  unsigned int ntowers = theTowerSeg.neta() * theTowerSeg.nphi();
  msg << MSG::DEBUG << " number of towers " << ntowers << endmsg;
  std::vector< std::vector<Entry> > ttcmatrix;
  ttcmatrix.resize (ntowers);
  m_weights.reserve (10);
  m_weights.push_back (1);
  
  for ( unsigned int iCalo=0; iCalo<sizeCalos; iCalo++ ){  // Loop over calos    //

      // find numerical ranges
      IdentifierHash firstIndex, lastIndex;
      cellIdHelper.calo_cell_hash_range((int)theCalos[iCalo], firstIndex, lastIndex);
      msg << MSG::DEBUG << " firstInder,lastIndex " << firstIndex << " " << lastIndex << endmsg;

      
      // consistent phi convention
      CaloPhiRange correctPhi;
      
	  for( size_t cellIndex = firstIndex; cellIndex < lastIndex;   cellIndex++){     // Cell Loop  //

		  
	  const CaloDetDescrElement* theDDE = theManager.get_element(cellIndex);

	  if(theDDE==0) {
	    msg << MSG::ERROR<< " CellIndex =  "<< cellIndex<< " has a DDE pointer NULL " << endmsg;
	    continue;
	  }

	  // pick up cell dimensions and raw position
	  double cellDeta = theDDE->deta();
	  double cellDphi = theDDE->dphi();
	  double etaRaw   = theDDE->eta_raw();
	  double phiRaw   = correctPhi.fix(theDDE->phi_raw());
	  
	  //////////////////////////////
	  // Calculate Cell Fragments //
	  //////////////////////////////
	  
	  // calculate cell/tower size ratio (integer,[1..n])
	  // DR : round rather than truncate
	  size_t ke = (size_t) (cellDeta/theTowerSeg.deta()+0.5);
	  ke = (ke==0) ? 1 : ke;
	  size_t kp = (size_t) (cellDphi/theTowerSeg.dphi()+0.5);
	  kp = (kp==0) ? 1 : kp;
	  // signal weight
	  double theWeight = 1. / ( (double) ke * kp );
	  // fractional cell sizes
	  double cellDdeta = cellDeta / (double) ke;
	  double cellDdphi = cellDphi / (double) kp;
	  double etaMin    = etaRaw - cellDeta / 2.;
	  double phiMin    = phiRaw - cellDphi / 2.;
	  
	  //////////////////////////
	  // Loop Cell Fragments  //
	  //////////////////////////
	  

	  for ( size_t ie=1; ie<=ke; ie++ ){	  							// outer (eta) fragment loop
	      double cellEta  = etaMin + ((double)ie - 0.5) * cellDdeta; 	// eta of fragment
	      for ( size_t ip=1; ip<=kp; ip++ ){ 							// inner (phi) fragement loop
                double cellPhi = phiMin + ((double)ip - 0.5) * cellDdphi;  	// phi of fragment
		  	   
                CaloTowerSeg::index_t etaIndex = theTowerSeg.etaIndex(cellEta);
                CaloTowerSeg::index_t phiIndex = theTowerSeg.phiIndex(cellPhi);
		 
                if ( etaIndex != CaloTowerSeg::outOfRange &&
                     phiIndex != CaloTowerSeg::outOfRange )
                {
                  CaloTowerSeg::index_t towerIndex = theTowerSeg.etaphi (etaIndex, phiIndex);
                  size_t iw = 0;
                  for (; iw < m_weights.size(); iw++) {
                    if (m_weights[iw] == theWeight) break;
                  }
                  if (iw == m_weights.size())
                    m_weights.push_back (theWeight);

                  if (towerIndex < ntowers) {
                    ttcmatrix[towerIndex].push_back(Entry(cellIndex,iw));
                  }
                } // index retrieval check
		 
              } // cell y loop
	    } // cell x loop

	} // cell index loop
      

    }

  int nraw = 0;
  m_towers.reserve (ttcmatrix.size()+1);
  m_entries.reserve (17500); // Typical observed size.
  for (size_t i = 0; i < ttcmatrix.size(); i++) {
    size_t last_nentries = m_entries.size();
    const std::vector<Entry>& v = ttcmatrix[i];
    nraw += v.size();

    if (!v.empty()) {
      Entry ent (v[0]);
      unsigned int stride = 0;
      for (size_t j = 1; j < v.size(); j++) {
        if (stride == 0) {
          if (v[j].hash - ent.hash == (int)Entry::large_stride) {
            stride = Entry::large_stride;
            ent.stride = 1;
          }
          else {
            ent.stride = 0;
            stride = Entry::small_stride;
          }
        }
        if ((int)(v[j].hash - v[j-1].hash) == (int)stride &&
            v[j].windex == ent.windex &&
            ent.ncells < Entry::ncells_max)
          ++ent.ncells;
        else {
          m_entries.push_back (ent);
          ent = v[j];
          stride = 0;
        }
      }
      m_entries.push_back (ent);
    }

    pushTower (m_entries.size() - last_nentries, v.size());
    //m_towers.push_back (Tower (m_entries.size() - last_nentries, v.size()));
  }

#if 0  
  {
    printf ("zzztowers calos: ");
    for (size_t z = 0; z < theCalos.size(); z++) printf ("%d ", theCalos[z]);
    printf ("ntower: %d nent: %d nentraw: %d weights: ",
            ntowers, m_entries.size(), nraw);
    for (size_t z = 0; z < m_weights.size(); z++) printf ("%f ", m_weights[z]);
    printf ("\n");

    static FILE* flog = 0;
    if (!flog) flog = fopen ("towstore.dump", "w");
    fprintf (flog, "========================================\n");
    size_t ic = 0;
    bool backref = false;
    for (size_t it = 0; it < m_towers.size(); it++) {
      fprintf (flog, "tow %d %d %d %d %d %d\n", it, m_towers[it].ncells,
               m_towers[it].nentries,
               m_towers[it].n1, m_towers[it].offs1, m_towers[it].offs2);
      if (!backref) {
        for (size_t z = 0; z < m_towers[it].nentries; z++) {
          fprintf (flog, "%6d %6d %2d %2d %3d\n", ic, m_entries[ic].hash,
                   m_entries[ic].windex, m_entries[ic].stride,
                   m_entries[ic].ncells);
          ++ic;
        }
      }
      backref = m_towers[it].backref_next;
    }
  }
#endif
    

  return 1;
}


/**
 * @brief Check m_entry_index and fill it in if we haven't done so yet.
 */
void CaloTowerStore::checkEntryIndex() const
{
  if (!m_entry_index.isValid()) {
    std::vector<unsigned short> entries;
    size_t sz = m_towers.size();
    entries.resize (sz);
    size_t ndx = 0;
    for (size_t i = 0; i < sz; i++) {
      entries[i] = ndx;
      if (!m_towers[i].backref_next)
        ndx += m_towers[i].nentries;
    }
    assert (ndx < std::numeric_limits<unsigned short>::max());
    m_entry_index.set (std::move (entries));
  }
}


void CaloTowerStore::pushTower (unsigned int nentries,
                                unsigned int ncells)
{
  if (!m_towers.empty() && m_towers.back().nentries == nentries) {
    unsigned int pos1 = m_entries.size() - 2*nentries;
    unsigned int pos2 = m_entries.size() -   nentries;
    unsigned int phase = 0;
    unsigned int n1 = 0;
    unsigned int offs1 = 0;
    unsigned int offs2 = 0;
    for (size_t i = 0; i < nentries; i++) {
      const Entry& ent1 = m_entries[pos1+i];
      const Entry& ent2 = m_entries[pos2+i];
      if (ent1.windex != ent2.windex ||
          ent1.ncells != ent2.ncells ||
          ent1.stride != ent2.stride)
      {
        phase = 3;
        break;
      }
      unsigned int offs = ent2.hash - ent1.hash;
      if (phase == 0) {
        offs1 = offs;
        n1 = 1;
        phase = 1;
      }
      else if (phase == 1) {
        if (offs == offs1)
          ++n1;
        else {
          offs2 = offs;
          phase = 2;
        }
      }
      else if (phase == 2) {
        if (offs != offs2) {
          phase = 3;
          break;
        }
      }
    }

    if (phase == 1)
      n1 = 0;

    if (phase < 3 &&
        n1 <= Tower::n1_max &&
        offs1 <= Tower::offs1_max &&
        offs2 <= Tower::offs2_max)
    {
      //m_towers.back().nentries = 0;
      assert (!m_towers.empty());
      m_towers.back().backref_next = true;
      m_towers.push_back (Tower (nentries, ncells, n1, offs1, offs2));
      m_entries.resize (m_entries.size() - nentries);
      return;
    }
  }
  m_towers.push_back (Tower (nentries, ncells));
}


CaloTowerStore::Entry::Entry (unsigned int the_hash,
                              unsigned int the_windex,
                              unsigned int the_ncells /*= 1*/,
                              unsigned int the_stride /*= 0*/)
  : hash (the_hash),
    windex (the_windex),
    ncells (the_ncells),
    stride (the_stride)
{
  assert (the_hash   <= hash_max);
  assert (the_windex <= windex_max);
  assert (the_ncells <= ncells_max);
  assert (the_stride <= stride_max);
}


CaloTowerStore::Tower::Tower (unsigned int the_nentries,
                              unsigned int the_ncells)
  : nentries (the_nentries),
    backref_next (false),
    n1 (0),
    ncells (the_ncells),
    offs1 (0),
    offs2 (0)
{
  assert (the_nentries <= nentries_max);
  assert (the_ncells <= ncells_max);
}


CaloTowerStore::Tower::Tower (unsigned int the_nentries,
                              unsigned int the_ncells,
                              unsigned int the_n1,
                              unsigned int the_offs1,
                              unsigned int the_offs2)
  : nentries (the_nentries),
    backref_next (false),
    n1 (the_n1),
    ncells (the_ncells),
    offs1 (the_offs1),
    offs2 (the_offs2)
{
  assert (the_nentries <= nentries_max);
  assert (the_ncells <= ncells_max);
  assert (the_n1 <= n1_max);
  assert (the_offs1 <= offs1_max);
  assert (the_offs2 <= offs2_max);
}


