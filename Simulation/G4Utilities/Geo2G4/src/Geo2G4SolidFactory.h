/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_Geo2G4SolidFactory_h
#define GEO2G4_Geo2G4SolidFactory_h

#include <map>
#include <string>

#include "GaudiKernel/ServiceHandle.h"
//#include "GaudiKernel/StatusCode.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArWheelSolid_type.h"

class G4VSolid;
class GeoShape;
class GeoUnidentifiedShape;
class Geo2G4SolidFactory
{
public:
  	typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  	typedef std::pair<LArWheelSolid_t, int> LArWheelSolidDef_t;
  	typedef std::map<std::string,  LArWheelSolidDef_t> LArWheelSolid_typemap;

  Geo2G4SolidFactory();
  G4VSolid* Build(const GeoShape*, std::string name=std::string("")) const;
  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

  /** @brief The standard @c StoreGateSvc/DetectorStore
    * Returns (kind of) a pointer to the @c StoreGateSvc
    */
   StoreGateSvc_t& detStore() const;
private:
  G4VSolid* createLArWheelSolid(const std::string& name, const LArWheelSolidDef_t & lwsdef) const;
  G4VSolid* createLArWheelSliceSolid(const GeoUnidentifiedShape* ) const;

  static const LArWheelSolid_typemap s_lwsTypes;

  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;
   /// Pointer to StoreGate (detector store by default)
   mutable StoreGateSvc_t m_detStore;
};

inline ServiceHandle<StoreGateSvc>& Geo2G4SolidFactory::detStore() const  {
	return m_detStore;
}

#endif
