/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCDETTOOL_H
#define ZDCDETTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "AthenaKernel/MsgStreamMember.h"

class ZDC_DetTool : public GeoModelTool 
{

public:

  ZDC_DetTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~ZDC_DetTool() override final;
  
  virtual StatusCode create() override final;

  MsgStream& msg     (MSG::Level lvl) const { return m_msg               << lvl; }
  bool       msgLevel(MSG::Level lvl)       { return m_msg.get().level() <= lvl; }

 private:
  
  mutable Athena::MsgStreamMember m_msg;
};

#endif // ZDCDETTOOL_H
