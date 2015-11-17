/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ZDCDATAACCESSV2_H
#define TRIGT1CALOBYTESTREAM_ZDCDATAACCESSV2_H

#include <string>
#include <vector>

#include "ZdcByteStream/IZdcDataAccessV2.h"
#include "AsgTools/AsgTool.h"


//namespace LVL1BS {

class ZdcByteStreamReadV1V2Tool;

/** Tool to retrieve all TriggerTowers
 *  from bytestream.
 *
 *  @author Alexander Mazurov <alexander.mazurov@cern.ch>
 */

class ZdcDataAccessV2 : public asg::AsgTool,
	virtual public IZdcDataAccessV2
{
	ASG_TOOL_CLASS(ZdcDataAccessV2, IZdcDataAccessV2)
 public:
	ZdcDataAccessV2(const std::string& name);

   virtual StatusCode initialize();

   virtual StatusCode loadTriggerTowers(xAOD::TriggerTowerContainer& container);
   virtual StatusCode PrintTriggerTowers();

 private:
   std::string m_message;
   /// Tool that does the actual work
   ToolHandle<ZdcByteStreamReadV1V2Tool> m_tool;
};

//} // end namespace

#endif
