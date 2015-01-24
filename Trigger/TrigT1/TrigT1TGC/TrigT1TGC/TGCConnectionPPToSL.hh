/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  Table of connection between Patch Panel and Sector Logic.
#ifndef TGCConnectionPPToSL_hh
#define TGCConnectionPPToSL_hh
#include "TrigT1TGC/TGCNumbering.hh"
#include "TrigT1TGC/TGCConnectionPPToSB.hh"
#include "TrigT1TGC/TGCConnectionSBToHPB.hh"
#include "TrigT1TGC/TGCConnectionHPBToSL.hh"

namespace LVL1TGCTrigger {

class TGCConnectionPPToSL {

public:
  TGCConnectionPPToSL();
  TGCConnectionPPToSL(const TGCConnectionPPToSL& );
  virtual ~TGCConnectionPPToSL(){};
  TGCConnectionPPToSL& operator =(const TGCConnectionPPToSL& );

  bool  readData(TGCRegionType type);
  const TGCConnectionPPToSB* getPPToSB() const { return &PPToSB; };
  const TGCConnectionSBToHPB* getSBToHPB() const { return &SBToHPB; };
  const TGCConnectionHPBToSL* getHPBToSL() const { return &HPBToSL; };

private:
  TGCConnectionPPToSB PPToSB;
  TGCConnectionSBToHPB SBToHPB;
  TGCConnectionHPBToSL HPBToSL;
};



} //end of namespace bracket

#endif // TGCConnectionPPToSL_hh
