/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4TBSimEvent/LArG4H6FrontHit.h"


LArG4H6FrontHit::LArG4H6FrontHit():edep(0.),pos(),scnum(-1),mwpcnum(-1),x(-1),y(-1),track_id(-1),pcode(0)
{
}

LArG4H6FrontHit::LArG4H6FrontHit(int sc, double ed)
{
 scnum = sc;  edep = ed;
 mwpcnum=-1;  x=-1;  y=-1;
 pcode = 0; pos=CLHEP::Hep3Vector(0.,0.,0.);
 track_id = -1;
}

LArG4H6FrontHit::LArG4H6FrontHit(int mw, int x1, int y1)
{
 edep = 0;
 mwpcnum = mw;  x = x1;  y = y1;
 scnum = -1;
 pcode = 0;
 pos=CLHEP::Hep3Vector(0.,0.,0.);
 track_id = -1;
}

LArG4H6FrontHit::LArG4H6FrontHit(const LArG4H6FrontHit &right)
  
{
 edep = right.edep;
 pos = right.pos;
 scnum = right.scnum;
 mwpcnum = right.mwpcnum;
 x = right.x;  
 y = right.y;
 pcode = right.pcode;
 track_id = right.track_id;
 pos = right.pos;
}

LArG4H6FrontHit& LArG4H6FrontHit::operator=(const LArG4H6FrontHit &right)
{
 if (this != &right) {
   edep = right.edep;
   pos = right.pos;
   scnum = right.scnum;
   mwpcnum = right.mwpcnum;
   x = right.x;
   y = right.y;
   pcode = right.pcode;
   track_id = right.track_id;
   pos = right.pos;
 }
 return *this;
}

int LArG4H6FrontHit::operator==(const LArG4H6FrontHit &right)
{
 if(edep != right.edep) return 0;
 if(pos != right.pos) return 0;
 if(scnum != right.scnum) return 0;
 if(mwpcnum != right.mwpcnum) return 0;
 if(x != right.x) return 0;
 if(y != right.y) return 0;
 if(pcode != right.pcode) return 0;
 if(track_id != right.track_id) return 0;
 if(pos != right.pos) return 0;
 return 1;
}

