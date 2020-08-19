/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackStateOnBLayerInfo_H
#define TrackStateOnBLayerInfo_H

#include "Identifier/Identifier.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"


namespace InDet {

  enum TrackStateOnBLayerType{insideGoodModule=0, 
			      nearGoodModuleEdge=1, 
			      insideBadModule=2, 
			      nearBadModuleEdge=3, 
			      unknownBLayerTrackState=4, 
			      numberOfTrackStateOnBLayerType=5};

  class TrackStateOnBLayerInfo{

  public:
    TrackStateOnBLayerInfo();
    ~TrackStateOnBLayerInfo() = default;

    inline TrackStateOnBLayerType type() const { return m_type; }
    inline Identifier moduleId() const { return m_moduleId; }
    inline Identifier pixelId() const { return m_pixelId; }
    inline const Amg::Vector3D& globalPosition() const { return m_globalPosition; }
    inline double localX() const { return m_locX; }
    inline double localY() const { return m_locY; }
    inline double theta() const { return m_theta; }
    inline double phi() const { return m_phi; }
    inline double errLocalX() const { return m_errLocX; }
    inline double errLocalY() const { return m_errLocY; }
    inline double distToModuleEdgePhi() const { return m_distToModEdgePhi; }
    inline double distToModuleEdgeEta() const { return m_distToModEdgeEta; }
    inline double goodFraction() const {return m_goodFrac;};

    inline void type(TrackStateOnBLayerType a) { m_type=a; }
    inline void moduleId(Identifier a) { m_moduleId=a; }
    inline void pixelId(Identifier a) { m_pixelId=a; }
    inline void globalPosition(Amg::Vector3D a) { m_globalPosition=a; }
    inline void localX(double a) { m_locX=a; }
    inline void localY(double a) { m_locY=a; }
    inline void theta(double a) { m_theta=a; }
    inline void phi(double a) { m_phi=a; }
    inline void errLocalX(double a) { m_errLocX=a; }
    inline void errLocalY(double a) { m_errLocY=a; }
    inline void distToModuleEdgePhi(double a) { m_distToModEdgePhi=a; }
    inline void distToModuleEdgeEta(double a) { m_distToModEdgeEta=a; }
    inline void goodFraction(double a) {m_goodFrac=a;};


  private:

    TrackStateOnBLayerType m_type;
    Identifier m_moduleId;  /// duplicated: can get from pixel id
    Identifier m_pixelId;
    Amg::Vector3D m_globalPosition;
    double m_locX;
    double m_locY;
    double m_errLocX;
    double m_errLocY;
    double m_theta;
    double m_phi;
    double m_distToModEdgePhi;
    double m_distToModEdgeEta;
    double m_goodFrac;

  };

 
  
}// end namespace

#endif
