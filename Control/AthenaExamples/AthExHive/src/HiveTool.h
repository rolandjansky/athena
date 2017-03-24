/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_HIVETOOL_H
#define ATHEXHIVE_HIVETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthExHive/IHiveTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "AthExHive/HiveDataObj.h"

#include <string>

class HiveTool : public extends1<AthAlgTool,IHiveTool> {
public:
  HiveTool( const std::string&, const std::string&, const IInterface* );
  virtual ~HiveTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

// the magic method this tool provides
   virtual StatusCode saySomething() const;

  virtual void ss() { ATH_MSG_INFO("...ssss..."); }

private:
  std::string m_myMessage;

  SG::ReadHandleKey<HiveDataObj> m_rdh1;
  SG::WriteHandleKey<HiveDataObj> m_wrh1;

};

#endif
