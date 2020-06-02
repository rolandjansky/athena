/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSTATIONINTERSECTSVC_H
#define MUON_MUONSTATIONINTERSECTSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonStationIntersectSvc/MuonStationIntersect.h"

const InterfaceID IID_IMuonStationIntersectSvc("MuonStationIntersectSvc", 1, 0);

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Muon {
  class MdtIntersectGeometry;
}

class MdtCondDbData;

/**
  Athena service that provides a list of tubes crossed by a give trajectory.

 @author Niels van Eldik
*/

class MuonStationIntersectSvc : public AthService, virtual public IInterface{
 public:
  MuonStationIntersectSvc(const std::string& name,ISvcLocator* sl);
  virtual ~MuonStationIntersectSvc()=default;

  static const InterfaceID& interfaceID() { return IID_IMuonStationIntersectSvc; }
  virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

  virtual StatusCode initialize(void);
 
  /** @brief calculate which MDT tubes were crossed by a trajectory
      @param id the Identifier of the chamber
      @param pos a impact position of the trajectory with the chamber
      @param dir the direction of the trajectory in the chamber
      @param condData the MDT conditions data for eliminating dead chambers/multilayers
      @param detMgr the MuonDetectorManager, needed by the MuonStationIntersect object
      @return a Muon::MuonStationIntersect object 
  */
  const Muon::MuonStationIntersect tubesCrossedByTrack( const Identifier& id, const Amg::Vector3D& pos, 
							 const Amg::Vector3D& dir, const MdtCondDbData* condData,
							 const MuonGM::MuonDetectorManager* detMgr) const;

  /** @brief get geometry description of the given chamber + neighbouring chambers */
  const std::vector<std::unique_ptr<Muon::MdtIntersectGeometry> > getStationGeometry( const Identifier& id, const MdtCondDbData* dbData, const MuonGM::MuonDetectorManager* detMgr ) const;

 private:

  /** @brief get identifiers of chamber corresponding to this id and its neighbors */
  const std::vector<Identifier> binPlusneighbours( const Identifier& id ) const;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

};


#endif
