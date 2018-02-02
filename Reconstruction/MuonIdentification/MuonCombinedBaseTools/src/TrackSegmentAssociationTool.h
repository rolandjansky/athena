/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKSEGMENTASSOCIATIONTOOL_H
#define TRACKSEGMENTASSOCIATIONTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkSegment/SegmentCollection.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "StoreGate/ReadHandleKey.h"

/** @class TrackSegmentAssociationTool
    
    TrackSegmentAssociationTool matches muon segments to a track

*/

static const InterfaceID IID_TrackSegmentAssociationTool("Muon::TrackSegmentAssociationTool",1,0);

namespace Muon{

  
  class TrackSegmentAssociationTool : public AthAlgTool{
  public:
    TrackSegmentAssociationTool(const std::string&, const std::string&, const IInterface*);

    /** default destructor **/
    ~TrackSegmentAssociationTool();

    /** standard initialization method **/
    StatusCode initialize();
    
    /** standard finalization method **/
    StatusCode finalize();

    /** Returns a list of segments that match with the input Muon. */
    bool associatedSegments(const xAOD::Muon& muon, 
                            std::vector< ElementLink<xAOD::MuonSegmentContainer> >& associatedSegments ) const;

    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_TrackSegmentAssociationTool; }
  private:

    const xAOD::MuonSegmentContainer* retrieveSegments( std::string location ) const;

    ToolHandle<Muon::MuonEDMHelperTool>  m_helper;
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer;

    SG::ReadHandleKey<xAOD::MuonSegmentContainer> m_segments{this,"MuonSegmentLocation","MuonSegments","muon segments"};

  };
}

#endif
