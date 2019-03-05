// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTOWERSTORE_H
#define CALOTOWERSTORE_H
/********************************************************************

NAME:     CaloTowerStore
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  S. Laplace
CREATED:  May 2005

PURPOSE:  Intermediate store for cell/tower maps. This is NOT to be
          confused with CaloTowerContainer, even though
          it is a container class for CaloTower objects. In particular,
          this class is NOT inherited from CaloTowerContainer classes.

Updated:  Jul 2009, sss

          Rewritten to greatly compress the mapping tables,
          to save virtual memory.

          To understand what's happening, it helps to go through
          the optimizations step-by-step.

          Originally, the table was stored like this:

            vector<vector<pair<unsigned int, double> > >

          The outer vector was indexed by the tower number;
          for each tower, we had a list of (cell hash, weight) pairs.

          The first thing to notice is that there are only a very
          few distinct weights used.  The weight is the ratio
          of tower/cell size, capped at 1.  The tower segmentations
          used during reconstruction give a maximum of five
          distinct weight values (1/n, where n is a small integer).
          So we can start by moving the weights to a separate list,
          and storing just the indices into that list.  I.e., something
          like

              struct Entry {
                unsigned int hash : ...;
                unsigned int weight_index : ...;
              };
            vector<vector<Entry> >

          The bitfield sizes can be set to pack Entry into a 32-bit word.

          The next step is to flatten the nested vector: there may
          be many towers, each with not so many cells.  So, instead of
          a nested vector, we can store two vectors, one for the
          cell entries:

            vector<Entry> m_entries;

          and one for the towers:

            vector<unsigned int> m_towers;

          where the entries here give the indices into m_entries.

          The next thing to notice is that the entries list contains
          many runs of cells with consecutive hash codes, plus even longer
          runs where the hash codes are regularly spaced at intervals
          of 64.  So we can compress things further: for each entry,
          we now store:

            - The starting hash code
            - The number of cells that this entry represents.
            - A flag for the stride (1 or 64).
            - The weight index.

          This can still be packed into 32 bits.

          When we do this, we also need to add to each tower the
          total count of cells.  We need this when building towers
          in order to reserve space for the cell list, and don't want
          to have to run through the entries a separate time to count.
          The cell count plus the entry index can be packed into 32 bits.

          Finally, we notice that seqeunces of entries used
          by adjacent towers tend to be quite similar, except that
          the hashes are offset by a small integer.  This offset may
          not be constant over the entire sequence, though.  For example,
          consider these two adjacent towers:

                hash    windex  stride  ncells
                15008   1       64      4
                25600   0        1      1
                31232   3        1      1

                15008   1       64      4
                25601   0        1      1
                31233   3        1      1

          The first entry has an offset of 0, the other two have
          an offset of 1.

          What we do is this.  For each tower, we encode two offsets,
          and also a count of the number of entries using the first
          offset (call this n1).  We choose n1 to be in the range 0--3;
          this catches most of the observed patterns.  If n1 is 0,
          then the first offset is applied to all entries.

          It's not possible to usefully pack both offsets and n1 along with
          an entry index and a count of the cells into 32 bits.  But
          instead of storing the entry index, we can store the number
          of entries used by this tower.  (That suffices since we only
          ever iterate forwards over the entire container.)  We also need
          to store a flag to tell if we're reusing the list of entries
          from the previous tower (with offsets).  So the information
          required for each tower is:

           - The number of entries this tower uses.
           - A flag to say if the next tower will reuse the entries
             for this tower.
           - The number of cells for this tower.
           - The first and second offset, and the count of entries
             using the first offset.

          With judicious choices, this fits in 32 bits.

          Further optimizations are possible.
          Many adjacent towers use have an OFFS1,OFFS2,OFFS1 pattern
          rather and OFFS1,OFFS2.  Examination of the compressed
          entry lists shows that the hash codes are often regularly
          spaced; adding a second outer stride could compress these
          further.

          I think we're reaching diminishing returns, though, so those
          compressions haven't been implemented.  With the current
          version, the entry lists made by the standard reconstruction
          have a length of about ~17000 words.

          The upshot is that memory usage is reduced by ~20x.
          Execution time is not observed to change significantly.

********************************************************************/

// include header files
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "CxxUtils/CachedValue.h"
#include <vector>
#include <utility>


class CaloDetDescrManager;


class CaloTowerStore
{
private:
  struct Entry
  {
    static const unsigned int hash_width = 18;
    static const unsigned int hash_max = (1<<hash_width)-1;
    static const unsigned int windex_width = 6;
    static const unsigned int windex_max = (1<<windex_width)-1;
    static const unsigned int ncells_width = 7;
    static const unsigned int ncells_max = (1<<ncells_width)-1;
    static const unsigned int stride_width = 1;
    static const unsigned int stride_max = (1<<stride_width)-1;

    // Work around link problems with some gcc versions.
    //static const unsigned int large_stride = 64;
    //static const unsigned int small_stride =  1;
    enum {
      large_stride = 64,
      small_stride = 1
    };

    unsigned int hash : hash_width;
    unsigned int windex : windex_width;
    unsigned int ncells : ncells_width;
    unsigned int stride : stride_width;

    Entry (unsigned int the_hash = 0,
           unsigned int the_windex = 0,
           unsigned int the_ncells = 1,
           unsigned int the_stride = 0);
  };

  struct Tower
  {
    static const unsigned int nentries_width = 5;
    static const unsigned int nentries_max = (1<<nentries_width)-1;
    static const unsigned int backref_next_width = 1;
    static const unsigned int backref_next_max = (1<<backref_next_width)-1;
    static const unsigned int n1_width = 2;
    static const unsigned int n1_max = (1<<n1_width)-1;
    static const unsigned int ncells_width = 8;
    static const unsigned int ncells_max = (1<<ncells_width)-1;
    static const unsigned int offs1_width = 8;
    static const unsigned int offs1_max = (1<<offs1_width)-1;
    static const unsigned int offs2_width = 8;
    static const unsigned int offs2_max = (1<<offs2_width)-1;

    unsigned int nentries : nentries_width;
    bool         backref_next : backref_next_width;
    unsigned int n1 : n1_width;
    unsigned int ncells : ncells_width;
    unsigned int offs1 : offs1_width;
    unsigned int offs2 : offs2_width;

    Tower (unsigned int the_nentries, unsigned int the_ncells);
    Tower (unsigned int the_nentries, unsigned int the_ncells,
           unsigned int the_n1,
           unsigned int the_offs1, unsigned int the_offs2);
  };


 public:

  /// \brief constructor
  CaloTowerStore();

  /// \brief destructor
  ~CaloTowerStore();

  class cell_iterator
  {
  public:
    cell_iterator (std::vector<Entry>::const_iterator it,
                   unsigned int n1,
                   unsigned int offs1,
                   unsigned int offs2,
                   std::vector<double>::const_iterator weights)
      : m_weights (weights),
        m_it (it),
        m_hash (0),
        m_nleft (0),
        m_n1 (n1),
        m_offs (offs1),
        m_offs2 (offs2),
        m_stride (0)
    { }
    cell_iterator (std::vector<Entry>::const_iterator it)
      : m_it (it),
        m_hash(0),
        m_nleft(0),
        m_n1(0),
        m_offs(0),
        m_offs2(0),
        m_stride(0)
    { }
    unsigned int hash()
    {
      if (m_nleft == 0) {
        m_hash = m_it->hash + m_offs;
        m_nleft = m_it->ncells;
        m_stride = m_it->stride ? Entry::large_stride : Entry::small_stride;
      }
      return m_hash;
    }
    double weight() const { return m_weights[m_it->windex]; }
    cell_iterator& operator++ () {
      if (m_nleft == 0) hash();
      --m_nleft;
      if (m_nleft > 0) {
        m_hash += m_stride;
      }
      else {
        ++m_it;
        if (m_n1 && --m_n1 == 0) {
          m_offs = m_offs2;
        }
      }
      return *this;
    }
    bool operator!= (const cell_iterator& other)
    { return this->m_it != other.m_it;}


  private:
    std::vector<double>::const_iterator m_weights;
    std::vector<Entry>::const_iterator m_it;
    unsigned int m_hash;
    unsigned int m_nleft;
    unsigned int m_n1;
    unsigned int m_offs;
    unsigned int m_offs2;
    unsigned int m_stride;
  };

  class tower_iterator
  {
  public:
    tower_iterator (std::vector<Tower>::const_iterator it,
                    const std::vector<Entry>::const_iterator entry,
                    const CaloTowerStore& store)
      : m_entry (entry),
        m_it (it),
        m_store (store)
    {}
    cell_iterator firstCell() const
    { return cell_iterator (m_entry, m_it->n1, m_it->offs1, m_it->offs2,
                            m_store.m_weights.begin()); }
    cell_iterator lastCell() const
    { return cell_iterator (m_entry + m_it->nentries); }
    size_t size() const { return m_it->ncells; }
    tower_iterator& operator++ () {
      if (!m_it->backref_next)
        m_entry += m_it->nentries;
      ++m_it;
      return *this;
    }

    tower_iterator& operator+= (size_t offs)
    {
      m_it += offs;
      m_entry = m_store.m_entries.begin() +
        (*m_store.m_entry_index.ptr())[m_it - m_store.m_towers.begin()];
      return *this;
    }

  protected:
    std::vector<Entry>::const_iterator m_entry;
    std::vector<Tower>::const_iterator m_it;
    const CaloTowerStore& m_store;
  };


  tower_iterator towers() const
  {
    return tower_iterator (m_towers.begin(),
                                                         m_entries.begin(),
                                                         *this);
  }


  typedef CaloTowerSeg::SubSegIterator<tower_iterator> tower_subseg_iterator;


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
  tower_subseg_iterator towers (const CaloTowerSeg::SubSeg& subseg) const;


  size_t size() const { return m_towers.size(); }

  /// \brief setup trigger
  bool buildLookUp(const CaloDetDescrManager& theManager,
                   const CaloTowerSeg& theTowerSeg,
                   const std::vector<CaloCell_ID::SUBCALO>& theCalos);

 private:
  friend class tower_iterator;

  void pushTower (unsigned int nentries, unsigned int ncells);


  /**
   * @brief Check m_entry_index and fill it in if we haven't done so yet.
   */
  void checkEntryIndex() const;

  CaloTowerSeg m_seg;
  std::vector<Entry> m_entries;
  std::vector<Tower> m_towers;
  std::vector<double> m_weights;

  /// One of these for each entry in m_towers, giving the index
  /// of the corresponding entry in m_entries.
  /// This is only needed for the case of iterating over a window;
  /// it is otherwise not filled in.
  /// Use a CachedValue for thread-safety.
  CxxUtils::CachedValue<std::vector<unsigned short> > m_entry_index;
};


//************************************************************************



#endif
