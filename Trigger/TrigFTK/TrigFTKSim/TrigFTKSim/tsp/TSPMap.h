/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TSPMAP_H
#define TSPMAP_H

#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/FTKHit.h"

#include <vector>

/* This class describe the information needed to interpret
 * the contente, in bits, of the pattern at the next level
 */
class TSPMap {
private:
  const FTKSSMap *m_amssmap;
  const FTKSSMap *m_tspssmap;

  int m_nplanes; // number of planes
  int *m_nbits; // number of bits used do describe each layer
  int **m_nfactor; // it contains the number of positions per layer per dimension
  int **m_internal_nbits; // internal number of bits, is ceil(log2(m_nfactor))
  int *m_bitoffset; // offset in the bitmask
  int **m_internal_bitoffset; // internal offset for layers with several dimensions
  int *m_ndim; // dimension of the plane
  int *m_npos; // total number of positions in the layer

  unsigned int m_totbits; // total number of DC bits

  // section for the GC lookup table
  int m_max_nbits; // maximum number of bits
  std::vector< std::vector<int> > m_B2G_table; // table that converts from Binary to Gray
  std::vector< std::vector<int> > m_G2B_table; // table that converts from Gray to Binary

  void generateGrayCodeTables();

public:
  TSPMap();
  TSPMap(const FTKSSMap *, const FTKSSMap *);
  TSPMap(const TSPMap &);
  ~TSPMap();

  void generate(const FTKSSMap *, const FTKSSMap *);

  const int &getNPlanes() const { return m_nplanes; }
  const int &getNBits(int ipl) const { return m_nbits[ipl]; }
  const int &getNPositions(int ipl) const { return m_npos[ipl]; }
  const int &getNBits(int ipl, int idim) const { return m_internal_nbits[ipl][idim]; }
  const int &getFactor(int ipl, int idim) const { return m_nfactor[ipl][idim]; }
  const int &getBitOffset(int ipl) const { return m_bitoffset[ipl]; }
  const int &getInternalBitOffset(int ipl, int idim) const { return m_internal_bitoffset[ipl][idim]; }
  const int &getDim(int ipl) const { return m_ndim[ipl]; }

  const unsigned int &getTotBits() const { return m_totbits; }

  int IEEE2GC(int, int);
  int GC2IEEE(int, int);
  int getPosition(int, int);

  int getHighResSSPart(FTKHit const&) const;
};
#endif // TSPMAP_H
