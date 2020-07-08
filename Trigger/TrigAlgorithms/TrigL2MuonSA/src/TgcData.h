/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_TGCDATA_H
#define  TRIGL2MUONSA_TGCDATA_H

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

class TgcHitData
{
   public:
      TgcHitData() :
	 eta(0),
	 phi(0),
	 r(0),
	 z(0),
	 width(0),
	 sta(0),
         isStrip(false),
	 bcTag(0),
         inRoad(false)
      {};
      ~TgcHitData() {};

   public:
      double eta;
      double phi;
      double r;
      double z;
      double width;
      int    sta;
      bool   isStrip;
      int    bcTag;
      bool   inRoad;
};

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

typedef std::vector<TgcHitData>  TgcHits;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

}

#endif  // TRIGL2MUONSA_TGCDATA_H
