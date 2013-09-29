/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSTATIONINTERSECTSVC_H
#define MUON_MUONSTATIONINTERSECTSVC_H

// std
#include <vector>

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IIncidentListener.h"

const InterfaceID IID_IMuonStationIntersectSvc("MuonStationIntersectSvc", 1, 0);

#include "GeoPrimitives/GeoPrimitives.h"

#include "MuonStationIntersectSvc/MuonStationIntersect.h"
#include "MuonIdHelpers/MdtIdHelper.h"

//class Incident;
class MdtIdHelper;
namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {
  class MuonIntersectGeometry;
}

class Identifier;
class MsgStream;

/**
  Athena service that provides a list of tubes crossed by a give trajectory.

 @author Niels van Eldik
*/
class MuonStationIntersectSvc : public AthService, virtual public IInterface,  virtual public IIncidentListener {
 public:
  MuonStationIntersectSvc(const std::string& name,ISvcLocator* sl);
  virtual ~MuonStationIntersectSvc();

  static const InterfaceID& interfaceID() { return IID_IMuonStationIntersectSvc; }
  virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

  virtual StatusCode initialize(void);
  virtual StatusCode finalize(void);  
 
  /** @brief calculate which MDT tubes were crossed by a trajectory
      @param id the Identifier of the chamber
      @param pos a impact position of the trajectory with the chamber
      @param dir the direction of the trajectory in the chamber
      @return a Muon::MuonStationIntersect object 
  */
  const Muon::MuonStationIntersect& tubesCrossedByTrack( const Identifier& id, const Amg::Vector3D& pos, 
							 const Amg::Vector3D& dir ) const;

  /** @brief get geometry description of the give chamber */
  const Muon::MuonIntersectGeometry* getChamberGeometry( const Identifier& id ) const;
 
  /** @brief get geometry description of the give chamber + neighbouring chambers */
  const std::vector<const Muon::MuonIntersectGeometry*>& getStationGeometry( const Identifier& id ) const;  


  /**  incident service handle for EndEvent */
  void handle(const Incident& inc);// maybe in the future clear per event

 private:

  class Imp;
  mutable Imp* m_imp;
};


#endif
