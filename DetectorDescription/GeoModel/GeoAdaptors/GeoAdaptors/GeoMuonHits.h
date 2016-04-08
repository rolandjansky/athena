/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _GeoMuonHits_h_
#define _GeoMuonHits_h_
//----------------------------------------------------------//
//                                                          //
// And adaptor for MuonHits.                                //
//                                                          //
// Joe Boudreau Feb 24.                                     //
//                                                          //
// This adaptor class allows Muon(MDT,RPC,TGC, CSC) Hits to //
// behave as if they knew which detector they were in..     //
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
class GenericMuonSimHit;



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

class GeoMDTHit {

 public:

  // Constructor:
  GeoMDTHit(const MDTSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

   // Underlying hit.
  const MDTSimHit &data() const { return *_hit;}

  // Is this hit OK?
  operator bool () const { return _man; }

 private:

  static void init();
  
  const MDTSimHit* _hit;
  static const MuonGM::MuonDetectorManager* _man;
  static const MdtIdHelper* _mdtID;
};

class GeoRPCHit {

 public:

  // Constructor:
  GeoRPCHit(const RPCSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Underlying hit.
  const RPCSimHit &data() const { return *_hit;}
  
  // Is this hit OK?
  operator bool () const { return _man; }

 private:
  static void init();
  
  const RPCSimHit* _hit;
  static const MuonGM::MuonDetectorManager* _man;
  static const RpcIdHelper* _rpcID;

};

class GeoTGCHit {

 public:

  // Constructor:
  GeoTGCHit(const TGCSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Underlying hit.
  const TGCSimHit &data() const { return *_hit;}

  // Is this hit OK?
  operator bool () const { return _man; }

 

 private:
  static void init();
  
  const TGCSimHit *_hit;
  static const MuonGM::MuonDetectorManager* _man;
  static const TgcIdHelper* _tgcID;

};


class GeoCSCHit {

 public:

  // Constructor:
  GeoCSCHit(const CSCSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Underlying hit.
  const CSCSimHit &data() const { return *_hit;}

  // Is this hit OK?
  operator bool () const { return _man; }

 private:
  static void init();
  
  const CSCSimHit                                 *_hit;
  static const MuonGM::MuonDetectorManager* _man;
  static const CscIdHelper* _cscID;

};

class GeoMMHit {

 public:

  // Constructor:
  GeoMMHit(const GenericMuonSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Get the a 3Dpoint with x and y = TrkLocalPos (tracking definition)
  Amg::Vector3D getTrkLocalPosition() const;
  
  // Get the global position from a TrkLocalPos (tracking definition)
  Amg::Vector3D getTrkGlobalPosition() const;
  
  // Underlying hit.
  const GenericMuonSimHit &data() const { return *_hit;}

  // Is this hit OK?
  operator bool () const { return _man; }

 

 private:
  static void init();
  
  const GenericMuonSimHit *_hit;
  static const MuonGM::MuonDetectorManager* _man;
  static const MmIdHelper* _mmID;

};

class GeosTGCHit {

 public:

  // Constructor:
  GeosTGCHit(const GenericMuonSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Underlying hit.
  const GenericMuonSimHit &data() const { return *_hit;}

  // Is this hit OK?
  operator bool () const { return _man; }

 

 private:
  static void init();
  
  const GenericMuonSimHit *_hit;
  static const MuonGM::MuonDetectorManager* _man;
  static const sTgcIdHelper* _stgcID;

};

#include "GeoAdaptors/GeoMuonHits.icc"

#endif
