/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCConnectionASDToPP_hh
#define TGCConnectionASDToPP_hh

#include "TrigT1TGC/TGCNumbering.h"
#include <fstream>
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
  static const int s_forwardLayer[];
};

} //end of namespace bracket

#endif // TGCConnectionASDToPP_hh
