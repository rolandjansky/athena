/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBEVENTSTREAMERTOOL_H
#define TBREC_TBEVENTSTREAMERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

static const InterfaceID IID_TBEventStreamerTool("TBEventStreamerTool", 1 , 0);

class TBEventStreamerTool : public AthAlgTool
{
 public:

  TBEventStreamerTool(const std::string& name, const std::string& type,
		      const IInterface* parent);
  virtual ~TBEventStreamerTool();

  virtual StatusCode accept() = 0;

  virtual StatusCode reject();

  virtual StatusCode initialize() override;

  virtual StatusCode initializeTool();

  static const InterfaceID& interfaceID() { return IID_TBEventStreamerTool;}
};
#endif
