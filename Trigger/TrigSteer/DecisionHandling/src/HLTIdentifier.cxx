/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigConfHLTData/HLTUtils.h"
#include "DecisionHandling/HLTIdentifier.h"

using namespace HLT;
bool Identifier::s_reportStringIDs = true; // default for now

Identifier::Identifier( const std::string& stringID )
  : m_id( TrigConf::HLTUtils::string2hash( stringID, "Identifier" ) ) {}

std::string  Identifier::name() const {
  if ( Identifier::reportStringIDs() ) {
    return TrigConf::HLTUtils::hash2string( numeric(), "Identifier" );
  }
  return "";
}

MsgStream& operator<< ( MsgStream& m, const HLT::Identifier& id ) {
  if ( Identifier::reportStringIDs() ) {
    m << id.name() << " ID#" << id.numeric();
  } else {
    m << "ID#" << id.numeric();
  }
  return m;
}


Identifier Identifier::fromToolName( const std::string& tname ) {
  return Identifier(  tname.substr( tname.find('.') + 1 ) );
}
