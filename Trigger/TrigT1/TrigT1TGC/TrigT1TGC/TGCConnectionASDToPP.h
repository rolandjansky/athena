/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCConnectionASDToPP_hh
#define TrigT1TGC_TGCConnectionASDToPP_hh

#include "TrigT1TGC/TGCNumbering.h"
#include <sstream>

namespace LVL1TGCTrigger {

class TGCConnectionASDToPP {
public:
  int getConnection(const int sideId, const int layer, const int chamber, const int line, int* pp, int* connector, int* channel) const;

  bool readData(TGCRegionType region, int type, TGCForwardBackwardType forward);  // read database

  void dump() const;  // print out connection table.

  TGCConnectionASDToPP();
  ~TGCConnectionASDToPP();
  TGCConnectionASDToPP(const TGCConnectionASDToPP& right);
  TGCConnectionASDToPP& operator=(const TGCConnectionASDToPP& right);

private:
  int m_totalNumberOfChannel;
  int m_type;
  TGCForwardBackwardType m_forwardBackward;
  // for ASD
  int* m_layerId;
  int* m_chamberId;
  int* m_lineId;
  // for PP
  int* m_PPId;
  int* m_connectorId;
  int* m_channelId;

  // Strip backwardLayer  ->               0  1  2  3  4  5  6  7  8
  static constexpr int s_forwardLayer[] = {2, 1, 0, 4, 3, 6, 5, 8, 7};
};

}  // end of namespace

#endif
