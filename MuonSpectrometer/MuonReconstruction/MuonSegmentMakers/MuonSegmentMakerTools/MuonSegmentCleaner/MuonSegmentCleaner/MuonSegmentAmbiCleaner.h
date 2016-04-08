/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonSegmentCleaner_MuonSegmentAmbiCleaner_H
#define MuonSegmentCleaner_MuonSegmentAmbiCleaner_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonRecToolInterfaces/IMuonSegmentCleaner.h"

class RpcIdHelper;
class MdtIdHelper;
class CscIdHelper;
class TgcIdHelper;
class Identifier;

namespace Muon {
  class MuonSegment;
}
namespace MuonGM {
  class MuonDetectorManager;
}

namespace Trk {
  class RIO_OnTrack;
  class PrepRawData;
}

class MuonSegmentAmbiCleaner : public AthAlgTool, virtual public Muon::IMuonSegmentCleaner
{
 public: 
  /** constructor */
  MuonSegmentAmbiCleaner(const std::string&,const std::string&,const IInterface*);
  /** destructor */
  virtual ~MuonSegmentAmbiCleaner();

  /** to initiate private members */
  virtual StatusCode initialize(); 
  /** to delete private members */
  virtual StatusCode finalize(); 

  /** For one segment solve ambiguous RPC and TGC hits: different eta but same phi
      using the MDT extrapolated segment 
      Makes and output a new segment dropping the ambiguous hits */
  
  virtual const Muon::MuonSegment* resolve( const Muon::MuonSegment* segment ) const;

 private:

  const MuonGM::MuonDetectorManager*  m_detMgr;   //!< Pointer to the detector manager

  const RpcIdHelper*                  m_rpcIdHelper; //!< Pointer to RPC id helper
  const TgcIdHelper*                  m_tgcIdHelper; //!< Pointer to TGC id helper
  const CscIdHelper*                  m_cscIdHelper; //!< Pointer to CSC id helper
  const MdtIdHelper*                  m_mdtIdHelper; //!< Pointer to MDT id helper

  /** flag to print out debugging information */
  bool m_debug;
  /** flag to print out a summary of what comes in and what comes out */
  bool m_summary; 

};

#endif // MuonSegmentCleaner_MuonSegmentAmbiCleaner_H
