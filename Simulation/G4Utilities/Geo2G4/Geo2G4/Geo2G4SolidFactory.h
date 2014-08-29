/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Geo2G4SolidFactory_h
#define Geo2G4SolidFactory_h

#include <map>
#include <string>
#include "AthenaKernel/MsgStreamMember.h"

class G4VSolid;
class GeoShape;

class Geo2G4SolidFactory 
{
 public:
	Geo2G4SolidFactory();
	G4VSolid* Build(const GeoShape*, std::string name=std::string("")) const;
	/// Log a message using the Athena controlled logging system
	MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
	/// Check whether the logging system is active at the provided verbosity level
	bool msgLvl( MSG::Level lvl ) { return m_msg.get().level() <= lvl; }
 private:
	/// Private message stream member
	mutable Athena::MsgStreamMember m_msg;
};

#endif
