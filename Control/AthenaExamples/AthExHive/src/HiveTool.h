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

  virtual StatusCode doSomething() const;

private:

  SG::ReadHandleKey<HiveDataObj>  m_rdh1{this, "Key_R1", "x1", "tool read key"};
  SG::WriteHandleKey<HiveDataObj> m_wrh1{this, "Key_W1", "X1", "tool write key"};

};

#endif
