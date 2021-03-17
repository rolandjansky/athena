/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDTOOLS_MUONTRACKTAGTESTTOOL_H
#define MUONCOMBINEDTOOLS_MUONTRACKTAGTESTTOOL_H

#include "MuonCombinedToolInterfaces/IMuonTrackTagTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkGeometry/TrackingGeometry.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkParameters/TrackParameters.h"

#include <mutex>
#include <string>

#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkGeometry/TrackingGeometry.h"


namespace MuonCombined {

class MuonTrackTagTestTool : public AthAlgTool, virtual public IMuonTrackTagTool {

  public:
    MuonTrackTagTestTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonTrackTagTestTool()=default;

    StatusCode initialize() override;

    double chi2(const Trk::TrackParameters& idParsAtEntry, const Trk::TrackParameters& msParsAtEntry) const override;
    double chi2(const Trk::Track& id, const Trk::Track& ms) const override;

  private:
    ToolHandle<Trk::IExtrapolator> m_extrapolator{this,"ExtrapolatorTool","Trk::Extrapolator/AtlasExtrapolator",};
    SG::ReadCondHandleKey<Trk::TrackingGeometry> m_trackingGeometryReadKey{this, "TrackingGeometryReadKey", "", "Key of input TrackingGeometry"};    
    ServiceHandle<Trk::ITrackingGeometrySvc> m_trackingGeometrySvc{this,"TrackingGeometrySvc","AtlasTrackingGeometrySvc"};  // Services are assumed to be thread-safe
   
    // end LEGACY_TRKGEOM

    double m_chi2cut;
#ifdef MUONCOMBDEBUG
    bool m_truth;
#endif
    inline const Trk::TrackingVolume* getVolume(const std::string&& vol_name) const{
        /// Tracking geometry is provided by the TrackingGeometryAlg
        if (!m_trackingGeometryReadKey.empty()){
           SG::ReadCondHandle<Trk::TrackingGeometry>  handle(m_trackingGeometryReadKey, Gaudi::Hive::currentContext());
           if (!handle.isValid()){
               ATH_MSG_WARNING("Could not retrieve a valid tracking geometry");
               return nullptr;
           }
           return handle.cptr()->trackingVolume(vol_name);
 
        }
        return m_trackingGeometrySvc->trackingGeometry()->trackingVolume(vol_name);    
    }

};

}  // namespace MuonCombined


#endif
