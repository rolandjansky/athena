/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PixelGeoBuilder_H
#define PixelGeoBuilder_H

#include "AthenaKernel/MsgStreamMember.h"
 
class OraclePixGeoAccessor;
class InDetMaterialManager;
class PixelID;
class IPixelGeometrySvc;

namespace InDetDD{
  class PixelDetectorManager;
  class SiCommonItems;
}

//class MsgStreamMember;

/**
   Base class for new-style pixel geometry builders.
   There should be one builder class implementing the build( int) method for each
   level in the geometry (Pixel envelope, barrel/endcaps, layers, staves, modules, sensors...)
   The derived classes can override the return type of the build method with pointers to types
   derived from InDet::GeoComponent.
 */

class PixelGeoBuilderBasics {

  friend class PixelGeoBuilder;

public:
  
  PixelGeoBuilderBasics() {};
  PixelGeoBuilderBasics(const OraclePixGeoAccessor* accessor ,
			InDetMaterialManager* matMgr, 
			const Athena::MsgStreamMember& msgStr,
			bool geoComp,
			const PixelID* idHelper,
			InDetDD::PixelDetectorManager* detMgr,
			InDetDD::SiCommonItems* commonItems) : 
    m_geoAccessor(accessor),m_matMgr(matMgr),m_msg(msgStr), m_bGeoComponent(geoComp),
    m_idHelper(idHelper),m_detectorManager(detMgr),m_commonItems(commonItems) {}
    
  ~PixelGeoBuilderBasics() {};
      
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

  // GeoComponent
  bool isGeoComponent() const { return m_bGeoComponent; }

  // Pixel identifier
  const PixelID* getIdHelper() const { return m_idHelper; } 

  InDetDD::PixelDetectorManager* getDetectorManager() const { return m_detectorManager; }
  InDetDD::SiCommonItems* getCommonItems() const { return m_commonItems; }

  // epsilon (1 um by default)
  double epsilon() const { return 0.001; }

 private:
  
  const OraclePixGeoAccessor* m_geoAccessor;
  mutable InDetMaterialManager* m_matMgr;
  mutable Athena::MsgStreamMember m_msg;
  bool m_bGeoComponent;
  const PixelID* m_idHelper;
  mutable InDetDD::PixelDetectorManager* m_detectorManager;
  InDetDD::SiCommonItems* m_commonItems;
};


class PixelGeoBuilder{

public:

  // Standard constructor
  //  PixelGeoBuilder( const OraclePixGeoAccessor* ,const InDetMaterialManager*, const Athena::MsgStreamMember& msgStr);
  PixelGeoBuilder( PixelGeoBuilderBasics* basics);
  PixelGeoBuilder( const PixelGeoBuilderBasics* basics);

  ~PixelGeoBuilder();

  // the geometry DB accessor
   const OraclePixGeoAccessor& geoAccessor() const {return m_basics->geoAccessor();}

  // the material manager ( material DB access)
  InDetMaterialManager* matMgr() const {return m_basics->matMgr();}

  // the message stream (same for all derived classes)
  const Athena::MsgStreamMember& msgStream() const {return m_basics->msgStream();}

  // the message stream (same for all derived classes)
  MsgStream& msg (MSG::Level lvl) const { return m_basics->msg(lvl); }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) const { return m_basics->msgLvl(lvl); }

  // GeoComponent
  bool isGeoComponent() const { return m_basics->isGeoComponent(); }

  // Pixel identifier
  const PixelID* getIdHelper() const { return m_basics->getIdHelper(); } 

  // the material manager ( material DB access)
  InDetDD::PixelDetectorManager* getDetectorManager() const { return m_basics->getDetectorManager(); }
  InDetDD::SiCommonItems* getCommonItems() const { return m_basics->getCommonItems(); }

  PixelGeoBuilderBasics* getBasics() const { return m_basics; }

  // epsilon (1 um by default)
  double epsilon() const { return m_basics->epsilon(); }


private:

  PixelGeoBuilderBasics* m_basics;
};


/* inline PixelGeoBuilder::PixelGeoBuilder( const OraclePixGeoAccessor* ga, */
/* 					 const InDetMaterialManager* pmm, */
/* 					 const Athena::MsgStreamMember& msgStr) : */
/*   m_geoAccessor(ga), m_matMgr( const_cast<InDetMaterialManager*>(pmm)), m_msg(msgStr), m_basics(0) */
/* {} */


inline PixelGeoBuilder::PixelGeoBuilder( PixelGeoBuilderBasics* b)
{
  m_basics = new PixelGeoBuilderBasics((b->m_geoAccessor),b->m_matMgr, b->m_msg, b->m_bGeoComponent,  
				       b->m_idHelper, b->m_detectorManager, b->m_commonItems);
  //  m_basics.setCommonItems(b->m_commonItems);
}

inline PixelGeoBuilder::PixelGeoBuilder( const PixelGeoBuilderBasics* b)
{
  m_basics = new PixelGeoBuilderBasics((b->m_geoAccessor),b->m_matMgr, b->m_msg, b->m_bGeoComponent,
				       b->m_idHelper, b->m_detectorManager, b->m_commonItems);
  //  m_basics.setCommonItems(b->m_commonItems);
}

inline PixelGeoBuilder::~PixelGeoBuilder()
{
  delete m_basics;
}


#endif
