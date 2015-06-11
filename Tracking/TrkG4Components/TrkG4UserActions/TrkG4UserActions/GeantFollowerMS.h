/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollowerMS.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef GeantFollowerMS_H
#define GeantFollowerMS_H

#include "FadsActions/UserAction.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"                                                                         
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h" 
#include <string>
#include <vector>

/** @class GeantFollowerMS

    @author Andreas.Salzburger@cern.ch
*/

namespace Trk {
    class IGeantFollowerMSHelper;
    class TrackingGeometry;
}

class StoreGateSvc;

class GeantFollowerMS: public FADS::UserAction {

  public:
    /** Standard FADS UsesAction */
    GeantFollowerMS(std::string s);

    /** All G4 interface methods */
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void SteppingAction(const G4Step*);
    
  private:
    std::string                               m_name;
    ToolHandle<Trk::IGeantFollowerMSHelper>     m_helper;
    mutable const Trk::IGeantFollowerMSHelper*  m_helperPointer;

                                                                                                                
    /** tracking geometry */                                                                                   
    mutable const Trk::TrackingGeometry*                         m_trackingGeometry;                           
    ServiceHandle<Trk::ITrackingGeometrySvc>                     m_trackingGeometrySvc;                   
    std::string                                                  m_trackingGeometryName;                       
    

};

#endif

