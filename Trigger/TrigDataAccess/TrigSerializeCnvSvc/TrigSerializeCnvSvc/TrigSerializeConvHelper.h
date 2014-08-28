/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class TrigSerializeConvHelper
 * it is the worker class for templated converters
 * non templated code should be concentrated here 
 * to avoid massive recompilation of the templated code
 * @author Jiri Masik <Jiri.Masik@cern.ch>
 */


#ifndef TRIGSERCONVHELPER_H
#define TRIGSERCONVHELPER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigSerializeCnvSvc/ITrigSerConvHelper.h"

//fwd 
class MsgStream;
class ITrigSerializerToolBase;
class TrigSerTPTool;
class ITrigSerializeGuidHelper;
class StoreGateSvc;
class IClassIDSvc;

class TrigSerializeConvHelper : virtual public ITrigSerializeConvHelper, public AthAlgTool {
  
public:
  TrigSerializeConvHelper(const std::string& name, const std::string& type,
			  const IInterface* parent);
  virtual ~TrigSerializeConvHelper();
  StatusCode createObj(const std::string &clname, IOpaqueAddress* iAddr, void *&ptr, bool isxAOD = false);
  StatusCode createRep(const std::string &clname, void *ptr, std::vector<uint32_t> &out, bool isxAOD = false);
  StatusCode initialize();

private:
  ToolHandle< ITrigSerializerToolBase > m_serializerTool;
  ToolHandle<TrigSerTPTool>             m_TPTool;
  ToolHandle<ITrigSerializeGuidHelper>  m_guidTool;
  bool                                  m_doTP;
  std::map<std::string,std::string>     m_oldEDMmap;
};


#endif
