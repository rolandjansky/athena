/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONEDMHELPERTOOL_H
#define MUON_MUONEDMHELPERTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "Identifier/Identifier.h"
#include "TrkParameters/TrackParameters.h"

#include <set>
#include <string>

namespace Trk {
  class MeasurementBase;
  class Track;
  class IExtrapolator;
}

static const InterfaceID IID_MuonEDMHelperTool("Muon::MuonEDMHelperTool",1,0);

namespace Muon {
 
  class MuonSegment;
  class MuonIdHelperTool;

  /** 
      Helper tool containing functionality needed by multiple tools. 
  */
  class MuonEDMHelperTool : public AthAlgTool {
  public:
    /** @brief default AlgTool constructor */
    MuonEDMHelperTool(const std::string&, const std::string&, const IInterface*);
    
    /** @brief destructor */
    ~MuonEDMHelperTool();
    
    /** @brief initialize method, method taken from bass-class AlgTool */
    StatusCode initialize();

    /** @brief finialize method, method taken from bass-class AlgTool */
    StatusCode finalize();
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonEDMHelperTool; }

    /** @brief tries to get Identifier for measurement, if not possible it will return Identifier() */
    Identifier getIdentifier( const Trk::MeasurementBase& meas ) const;

    /** @brief returns the first MDT or CSC identifier of the segment */
    Identifier chamberId( const MuonSegment& seg ) const;

    /** @brief returns a set containing the chamber Ids of all MDT or CSC chambers on the segment */
    std::set<Identifier> chamberIds( const MuonSegment& seg ) const;

    /** @brief returns whether the segment is in the endcap */
    bool isEndcap( const MuonSegment& seg ) const;

    /** @brief returns whether the track is in the endcap */
    bool isEndcap( const Trk::Track& seg ) const;

    /** brief create a AtaPlane track parameter for segment */
    const Trk::AtaPlane* createTrackParameters( const MuonSegment& seg, double momentum = 1., double charge = 0. ) const;

    /** check whether track satisfies selection chi2 cut */
    bool goodTrack( const Trk::Track& track, double chi2Cut ) const;

    /** create a perigee parameters from a track parameters */
    const Trk::Perigee* createPerigee( const Trk::TrackParameters& pars ) const;
    
    /** determines whether a track is a straight line track (no momentum fitted). */
    bool isSLTrack( const Trk::Track& track );

  private:
    ToolHandle<Muon::MuonIdHelperTool>  m_idHelper;
    ToolHandle<Trk::IExtrapolator>      m_extrapolator;  

  };
  
}

#endif
