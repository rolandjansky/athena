/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IGeoIDSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_INTERFACES_IGEOIDSVC_H
#define ISF_INTERFACES_IGEOIDSVC_H 1

// Gaudi
#include "GaudiKernel/IInterface.h"

// Amg
#include "GeoPrimitives/GeoPrimitives.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// ISF Simulation includes
#include "ISF_Event/ISFParticle.h" 
 
namespace ISF {

  static const InterfaceID IID_IGeoIDSvc("IGeoIDSvc", 1, 0);

  enum InsideType {
    fOutside        = 0,
    fSurface        = 1,
    fInside         = 2,
   // to be used to allocate arrays
    fNumInsideTypes = 3
  };

  /**
   @class IGeoIDSvc

   The interface to chose between the sub geometry services,
   realized as an AlgTool since it does not have to be dynamically created
   
   The IGeoIDSvc registers the nextGeoSvcID to the ISFParticle.
       
   @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
   */
     
  class IGeoIDSvc : virtual public IInterface {
     public:
     
       /** Virtual destructor */
       virtual ~IGeoIDSvc(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IGeoIDSvc; }

       /** Athena algtool's Hooks */
       virtual StatusCode  initialize() = 0;
       virtual StatusCode  finalize() = 0;

       /** Checks if the given position (ISFParticle) is inside/outside/onsurface a given AtlasRegion */
       virtual ISF::InsideType inside(const Amg::Vector3D &pos, AtlasDetDescr::AtlasRegion geoID) const = 0;
       inline  ISF::InsideType inside(const ISFParticle& sp, AtlasDetDescr::AtlasRegion geoID) const;
       inline  ISF::InsideType inside(double x, double y, double z, AtlasDetDescr::AtlasRegion geoID) const;

       /** A static filter that returns the AtlasRegion of the given ISFParticle (position)
            -> returns ISF::fUndefinedGeoID if particle is on surface */
       virtual AtlasDetDescr::AtlasRegion identifyGeoID(const Amg::Vector3D &pos) const = 0;
       inline  AtlasDetDescr::AtlasRegion identifyGeoID(const ISFParticle& sp) const;
       inline  AtlasDetDescr::AtlasRegion identifyGeoID(double x, double y, double z) const;

       /** Find the AtlasRegion that the particle will enter with its next infinitesimal step
           along the given direction */
       virtual AtlasDetDescr::AtlasRegion identifyNextGeoID(const Amg::Vector3D &pos, const Amg::Vector3D &dir) const = 0;
       inline  AtlasDetDescr::AtlasRegion identifyNextGeoID(const ISFParticle& sp) const;
       inline  AtlasDetDescr::AtlasRegion identifyNextGeoID(double x, double y, double z,
                                               double dx, double dy, double dz) const;

       /** Find the AtlasRegion that the particle will enter with its next infinitesimal step
           along the given direction, and register this geoID to the particle */
       inline  AtlasDetDescr::AtlasRegion identifyAndRegNextGeoID(ISFParticle& sp) const;
  };


  // inline methods (basically wrapper for ISFParticle/coord. -> HepGeom::Point3D<double>)
  //
  // inside() wrappers
  inline ISF::InsideType IGeoIDSvc::inside(const ISFParticle& sp, AtlasDetDescr::AtlasRegion geoID) const {
    return inside( sp.position(), geoID);
  }
  inline ISF::InsideType IGeoIDSvc::inside(double x, double y, double z, AtlasDetDescr::AtlasRegion geoID) const {
    const Amg::Vector3D pos(x, y, z);
    return inside( pos, geoID);
  }

  // identifyGeoID() wrappers
  inline AtlasDetDescr::AtlasRegion IGeoIDSvc::identifyGeoID(const ISFParticle &sp) const { 
    return identifyGeoID( sp.position());
  }
  inline AtlasDetDescr::AtlasRegion IGeoIDSvc::identifyGeoID(double x, double y, double z) const { 
    const Amg::Vector3D pos(x, y, z);
    return identifyGeoID( pos);
  }

  // identifyNextGeoID() wrappers
  inline AtlasDetDescr::AtlasRegion IGeoIDSvc::identifyNextGeoID(const ISFParticle& sp) const {
    return identifyNextGeoID( sp.position(), sp.momentum());
  }
  inline AtlasDetDescr::AtlasRegion IGeoIDSvc::identifyNextGeoID(double x, double y, double z,
                                                    double dx, double dy, double dz) const {
    const Amg::Vector3D pos(x, y, z);
    const Amg::Vector3D dir(dx, dy, dz);
    return identifyNextGeoID( pos, dir);
  }

  // identifyAndRegNextGeoID() wrapper
  inline  AtlasDetDescr::AtlasRegion IGeoIDSvc::identifyAndRegNextGeoID( ISFParticle& part) const {
    AtlasDetDescr::AtlasRegion geoID = identifyNextGeoID( part.position(), part.momentum());
    part.setNextGeoID(geoID);
    return geoID;
  }

} // end of namespace

#endif // ISF_INTERFACES_IGEOIDSVC_H 
