/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonSegmentCleaner_MuonSegmentAmbiCleaner_H
#define MuonSegmentCleaner_MuonSegmentAmbiCleaner_H

#include "MuonRecToolInterfaces/IMuonSegmentCleaner.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace Muon {
  class MuonSegment;
}

class MuonSegmentAmbiCleaner : public AthAlgTool, virtual public Muon::IMuonSegmentCleaner
{
 public: 
  MuonSegmentAmbiCleaner(const std::string&,const std::string&,const IInterface*);
  virtual ~MuonSegmentAmbiCleaner()=default;

  virtual StatusCode initialize(); 

  /** For one segment solve ambiguous RPC and TGC hits: different eta but same phi
      using the MDT extrapolated segment 
      Makes and output a new segment dropping the ambiguous hits */
  
  virtual const Muon::MuonSegment* resolve( const Muon::MuonSegment* segment ) const;

 private:

  ServiceHandle<Muon::IMuonIdHelperSvc>     m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  /** flag to print out debugging information */
  bool m_debug;
  /** flag to print out a summary of what comes in and what comes out */
  bool m_summary; 

};

#endif // MuonSegmentCleaner_MuonSegmentAmbiCleaner_H
