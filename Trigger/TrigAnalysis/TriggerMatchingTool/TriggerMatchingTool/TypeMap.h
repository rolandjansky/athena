// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMATCHINGTOOL_TYPEMAP_H
#define TRIGGERMATCHINGTOOL_TYPEMAP_H

#include "TrigNavStructure/Types.h"
#include "xAODBase/IParticle.h"
#include <map>

class TypeMap {
public:
  typedef std::pair<HLT::class_id_type,std::string> clid_string_t;
  TypeMap();
  bool isKnown(const xAOD::Type::ObjectType& recoType) const;
  clid_string_t get(const xAOD::Type::ObjectType& recoType) const;
private:
  std::map<xAOD::Type::ObjectType,clid_string_t> m_typemap;
};
#endif
