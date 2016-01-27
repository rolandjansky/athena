/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef GeoVPixelBasics_H
#define GeoVPixelBasics_H

#include "AthenaKernel/MsgStreamMember.h"
 
class OraclePixGeoAccessor;
class InDetMaterialManager;

//class MsgStreamMember;

/**
   Base class for new-style pixel geometry builders.
   There should be one builder class implementing the build( int) method for each
   level in the geometry (Pixel envelope, barrel/endcaps, layers, staves, modules, sensors...)
   The derived classes can override the return type of the build method with pointers to types
   derived from InDet::GeoComponent.
 */

class GeoVPixelBasics {

public:
  
  GeoVPixelBasics(const OraclePixGeoAccessor* accessor ,
		  InDetMaterialManager* matMgr, 
		  const Athena::MsgStreamMember& msgStr): m_geoAccessor(accessor),m_matMgr(matMgr),m_msg(msgStr) {}
    
  ~GeoVPixelBasics(){};
      
  // the geometry DB accessor
  const OraclePixGeoAccessor& geoAccessor() const {return *m_geoAccessor;}
    
  // the material manager ( material DB access)
  InDetMaterialManager* matMgr() const {return m_matMgr;}
      
  // the message stream (same for all derived classes)
  const Athena::MsgStreamMember& msgStream() const {return m_msg;}
	
  // the message stream (same for all derived classes)
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }

 private:
  
  const OraclePixGeoAccessor* m_geoAccessor;
  mutable InDetMaterialManager* m_matMgr;
  mutable Athena::MsgStreamMember m_msg;
};

#endif
