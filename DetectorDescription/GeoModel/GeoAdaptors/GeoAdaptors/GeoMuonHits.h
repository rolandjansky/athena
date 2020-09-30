/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOADAPTORS_GEOMUONHITS_H
#define GEOADAPTORS_GEOMUONHITS_H
//----------------------------------------------------------//
//                                                          //
// An adaptor for MuonHits.                                 //
//                                                          //
// Joe Boudreau Feb 24.                                     //
//                                                          //
// This adaptor class allows Muon(MDT,RPC,TGC, CSC) Hits to //
// behave as if they knew which detector they were in.      //
//                                                          //
// imt 2005/11/4 PrepRawData just need access to helpers... //
//                                                          //
//----------------------------------------------------------//
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "Identifier/Identifier.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"

namespace MuonGM {
  class MuonDetectorManager;
}

class MDTSimHit;
class RPCSimHit;
class TGCSimHit;
class CSCSimHit;
class sTGCSimHit;
class MMSimHit;



class MdtIdHelper;
class RpcIdHelper;
class TgcIdHelper;
class CscIdHelper;
class sTgcIdHelper;
class MmIdHelper;

namespace MuonGM{
  class RpcReadoutElement;
  }
class Identifier;


class GeoMuonHitBase
{
protected:
  const MuonGM::MuonDetectorManager* mgr() const;

private:
  static const MuonGM::MuonDetectorManager* init();
};


class GeoMDTHit : public GeoMuonHitBase
{
 public:

  // Constructor:
  GeoMDTHit(const MDTSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

   // Underlying hit.
  const MDTSimHit &data() const { return *m_hit;}

  // Is this hit OK?
  operator bool () const { return true; }

 private:
  const MDTSimHit* m_hit;
};


class GeoRPCHit : public GeoMuonHitBase
{
 public:

  // Constructor:
  GeoRPCHit(const RPCSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Underlying hit.
  const RPCSimHit &data() const { return *m_hit;}
  
  // Is this hit OK?
  operator bool () const { return true; }

 private:
  const RPCSimHit* m_hit;
};


class GeoTGCHit : public GeoMuonHitBase
{
 public:

  // Constructor:
  GeoTGCHit(const TGCSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Underlying hit.
  const TGCSimHit &data() const { return *m_hit;}

  // Is this hit OK?
  operator bool () const { return true; }

 

 private:
  const TGCSimHit *m_hit;
};


class GeoCSCHit : public GeoMuonHitBase
{
 public:

  // Constructor:
  GeoCSCHit(const CSCSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Underlying hit.
  const CSCSimHit &data() const { return *m_hit;}

  // Is this hit OK?
  operator bool () const { return true; }

 private:
  const CSCSimHit                                 *m_hit;
};


class GeoMMHit : public GeoMuonHitBase
{
 public:

  // Constructor:
  GeoMMHit(const MMSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Get the a 3Dpoint with x and y = TrkLocalPos (tracking definition)
  Amg::Vector3D getTrkLocalPosition() const;
  
  // Get the global position from a TrkLocalPos (tracking definition)
  Amg::Vector3D getTrkGlobalPosition() const;
  
  // Underlying hit.
  const MMSimHit &data() const { return *m_hit;}

  // Is this hit OK?
  operator bool () const { return true; }

 

 private:
  const MMSimHit *m_hit;
};


class GeosTGCHit : public GeoMuonHitBase
{
 public:

  // Constructor:
  GeosTGCHit(const sTGCSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Underlying hit.
  const sTGCSimHit &data() const { return *m_hit;}

  // Is this hit OK?
  operator bool () const { return true; }


 private:
  const sTGCSimHit *m_hit;
};

#include "GeoAdaptors/GeoMuonHits.icc"

#endif
