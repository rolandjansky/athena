/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONTRUTHSUMMARYTOOL_H
#define MUON_IMUONTRUTHSUMMARYTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <string>

class Identifier;

namespace Trk {
  class Track;
}

static const InterfaceID IID_IMuonTruthSummaryTool("Muon::IMuonTruthSummaryTool",1,0);


namespace Muon {
  class MuonSegment;

  /** Interface for tools calculating hit count summaries for track */
  class IMuonTruthSummaryTool : virtual public IAlgTool {      
  public:

  public:
    /** IAlgTool interface */
    static const InterfaceID& interfaceID();

    /** clear tool */
    virtual void clear() = 0;

    /** init truth */
    virtual void init() = 0;

    /** get the associated barcode for the identifier, return -1 if the channel was not hit by a muon */
    virtual int getBarcode( const Identifier& id ) = 0;

    /** get the associated pdgId for a given barcode */
    virtual int getPdgId( int barcode ) = 0;

    /** add identifier */
    virtual void add( const Identifier& id, int level ) = 0;

    /** add segment */
    virtual void add( const MuonSegment& seg, int level ) = 0;

    /** add track */
    virtual void add( const Trk::Track& track, int level ) = 0;

    /** print summary */
    virtual std::string printSummary() = 0;
  };
  
  inline const InterfaceID& IMuonTruthSummaryTool::interfaceID()
  {
    return IID_IMuonTruthSummaryTool;
  }

} // end of name space

#endif

