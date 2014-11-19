/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FastSimModelCatalog_H
#define FastSimModelCatalog_H

#include "G4FastSimulation/FastSimModel.h"
#include "AthenaKernel/MsgStreamMember.h"
#include <map>
#include <string>

typedef std::map<std::string, FastSimModel*, std::less<std::string> >  fsModelCatalog;

class FastSimModelCatalog
{
public:
  ///
  static FastSimModelCatalog* GetInstance();
  ///
  void RegisterModel(FastSimModel* model);
  ///
  FastSimModel* GetModel(std::string name);
  ///
  void PrintAll();
  /// Log a message using the Athena controlled logging system
  MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
  /// Check whether the logging system is active at the provided verbosity level
  bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
 private:


private:
  /// Constructor
  inline FastSimModelCatalog();
  ///
  fsModelCatalog theCatalog;
  ///
  static FastSimModelCatalog* thePointer;
  /// Private message stream member
  mutable Athena::MsgStreamMember m_msg;
};

#endif
