/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONEDMHELPERSVC_H
#define MUON_IMUONEDMHELPERSVC_H

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/IService.h"
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

namespace Muon {
 
  class MuonSegment;

  /** 
      Helper tool containing functionality needed by multiple tools. 
  */
  class IMuonEDMHelperSvc : virtual public IService {
  public:
    DeclareInterfaceID(IMuonEDMHelperSvc, 1, 0);

    /** @brief destructor */
    virtual ~IMuonEDMHelperSvc() {}
    
    /** @brief tries to get Identifier for measurement, if not possible it will return Identifier() */
    virtual Identifier getIdentifier( const Trk::MeasurementBase& meas ) const = 0;

    /** @brief returns the first MDT or CSC identifier of the segment */
    virtual Identifier chamberId( const MuonSegment& seg ) const = 0;

    /** @brief returns a set containing the chamber Ids of all MDT or CSC chambers on the segment */
    virtual std::set<Identifier> chamberIds( const MuonSegment& seg ) const = 0;

    /** @brief returns whether the segment is in the endcap */
    virtual bool isEndcap( const MuonSegment& seg ) const = 0;

    /** @brief returns whether the track is in the endcap */
    virtual bool isEndcap( const Trk::Track& seg ) const = 0;

    /** brief create a AtaPlane track parameter for segment */
    virtual const Trk::AtaPlane* createTrackParameters( const MuonSegment& seg, double momentum = 1., double charge = 0. ) const = 0;

    /** check whether track satisfies selection chi2 cut */
    virtual bool goodTrack( const Trk::Track& track, double chi2Cut ) const = 0;

    /** determines whether a track is a straight line track (no momentum fitted). */
    virtual bool isSLTrack( const Trk::Track& track ) const = 0;
  };
  
}

#endif
