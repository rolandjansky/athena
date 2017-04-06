/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  const MDTSimHit &data() const { return *m_hit;}

  // Is this hit OK?
  operator bool () const { return s_man; }

 private:

  static void init();
  
  const MDTSimHit* m_hit;
  static const MuonGM::MuonDetectorManager* s_man;
  static const MdtIdHelper* s_mdtID;
};

class GeoRPCHit {

 public:

  // Constructor:
  GeoRPCHit(const RPCSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Underlying hit.
  const RPCSimHit &data() const { return *m_hit;}
  
  // Is this hit OK?
  operator bool () const { return s_man; }

 private:
  static void init();
  
  const RPCSimHit* m_hit;
  static const MuonGM::MuonDetectorManager* s_man;
  static const RpcIdHelper* s_rpcID;

};

class GeoTGCHit {

 public:

  // Constructor:
  GeoTGCHit(const TGCSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Underlying hit.
  const TGCSimHit &data() const { return *m_hit;}

  // Is this hit OK?
  operator bool () const { return s_man; }

 

 private:
  static void init();
  
  const TGCSimHit *m_hit;
  static const MuonGM::MuonDetectorManager* s_man;
  static const TgcIdHelper* s_tgcID;

};


class GeoCSCHit {

 public:

  // Constructor:
  GeoCSCHit(const CSCSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Underlying hit.
  const CSCSimHit &data() const { return *m_hit;}

  // Is this hit OK?
  operator bool () const { return s_man; }

 private:
  static void init();
  
  const CSCSimHit                                 *m_hit;
  static const MuonGM::MuonDetectorManager* s_man;
  static const CscIdHelper* s_cscID;

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
  const GenericMuonSimHit &data() const { return *m_hit;}

  // Is this hit OK?
  operator bool () const { return s_man; }

 

 private:
  static void init();
  
  const GenericMuonSimHit *m_hit;
  static const MuonGM::MuonDetectorManager* s_man;
  static const MmIdHelper* s_mmID;

};

class GeosTGCHit {

 public:

  // Constructor:
  GeosTGCHit(const GenericMuonSimHit & h);

  // Get the absolute global position:
  Amg::Vector3D getGlobalPosition() const;

  // Underlying hit.
  const GenericMuonSimHit &data() const { return *m_hit;}

  // Is this hit OK?
  operator bool () const { return s_man; }

 

 private:
  static void init();
  
  const GenericMuonSimHit *m_hit;
  static const MuonGM::MuonDetectorManager* s_man;
  static const sTgcIdHelper* s_stgcID;

};

#include "GeoAdaptors/GeoMuonHits.icc"

#endif
