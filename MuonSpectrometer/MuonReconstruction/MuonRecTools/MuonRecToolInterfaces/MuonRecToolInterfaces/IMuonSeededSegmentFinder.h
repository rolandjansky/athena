/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEEDEDSEGMENTFINDER
#define MUON_IMUONSEEDEDSEGMENTFINDER

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
// New Small Wheel
#include "MuonPrepRawData/sTgcPrepDataCollection.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"

#include <set>
#include <vector>

static const InterfaceID IID_IMuonSeededSegmentFinder("Muon::IMuonSeededSegmentFinder", 1, 0);

namespace Muon {
class MuonSegment;
}

namespace Muon {

/** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */
class IMuonSeededSegmentFinder : virtual public IAlgTool
{
public:
  /** access to tool interface */
  static const InterfaceID& interfaceID();

  /** @brief find segments in a set of chambers starting from seeding TrackParameters */
  virtual std::unique_ptr<Trk::SegmentCollection> find( const Trk::TrackParameters& pars, const std::set<Identifier>& chIds ) const = 0;

  /** @brief find segments in a set of chambers starting from seeding TrackParameters (version with Hashes) */
  virtual std::unique_ptr<Trk::SegmentCollection> find( const Trk::TrackParameters& pars, const std::set<IdentifierHash>& chIdHs ) const = 0;

  /** @brief find segments in a set of MdtPrepData starting from seeding TrackParameters */
  virtual std::unique_ptr<Trk::SegmentCollection> find( const Trk::TrackParameters& pars, const std::vector<const MdtPrepData*>& mdtPrds ) const = 0;


  /** @brief retrieve MDT PRD collections for the given hashes */
  virtual void extractMdtPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const MdtPrepDataCollection*>& target  ) const = 0;

  /** @brief retrieve RPC PRD collections for the given hashes */
  virtual void extractRpcPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const RpcPrepDataCollection*>& target ) const = 0;

  /** @brief retrieve TGC PRD collections for the given hashes */
  virtual void extractTgcPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const TgcPrepDataCollection*>& target ) const = 0;

  /** @brief retrieve CSC PRD collections for the given hashes */
  virtual void extractCscPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const CscPrepDataCollection*>& target ) const = 0;

  // New Small Wheel
  /** @brief retrieve STGC PRD collections for the given hashes */
  virtual void extractsTgcPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const sTgcPrepDataCollection*>& target ) const = 0;

  /** @brief retrieve MM PRD collections for the given hashes */
  virtual void extractMMPrdCols( const std::set<IdentifierHash>& chIdHs, std::vector<const MMPrepDataCollection*>& target ) const = 0;


};

inline const InterfaceID& IMuonSeededSegmentFinder::interfaceID()
{
  return IID_IMuonSeededSegmentFinder;
}

} // end of name space

#endif

