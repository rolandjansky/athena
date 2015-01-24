/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCConnectionASDToPP.hh,v 1.1.1.10 2003-11-03 10:18:17 mishino Exp $
#ifndef TGCConnectionASDToPP_hh
#define TGCConnectionASDToPP_hh

#include "TrigT1TGC/TGCNumbering.hh"
#include <fstream>
#include <sstream>

namespace LVL1TGCTrigger {

class TGCConnectionASDToPP {
public:
  int getConnection(const int sideId, const int layer, const int chamber, const int line, int* pp, int* connector, int* channel);

  bool readData(TGCRegionType region, int type, TGCForwardBackwardType forward);  // read database

  void dump() const;  // print out connection table.

  TGCConnectionASDToPP();
  ~TGCConnectionASDToPP();
  TGCConnectionASDToPP(const TGCConnectionASDToPP& right);
  TGCConnectionASDToPP& operator=(const TGCConnectionASDToPP& right);

private:
  int totalNumberOfChannel;
  int type;
  TGCForwardBackwardType forwardBackward;
  // for ASD
  int* layerId;
  int* chamberId;
  int* lineId;
  // for PP
  int* PPId;
  int* connectorId;
  int* channelId;
  static const int forwardLayer[];
};

} //end of namespace bracket

#endif // TGCConnectionASDToPP_hh
