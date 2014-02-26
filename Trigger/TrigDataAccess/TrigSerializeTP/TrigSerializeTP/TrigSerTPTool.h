// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @class  TrigSerTPTool
 * @author Jiri Masik <Jiri.Masik@cern.ch>
 * based on the code of AthenaROOTAccess by Scott Snyder
 */

#ifndef TRIGSERIALIZETP_TRIGSERTPTOOL_H
#define TRIGSERIALIZETP_TRIGSERTPTOOL_H 1

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ClassID.h"
#include <string>
#include <vector>
#include <map>


class MsgStream;
class TClass;
class ITPCnvBase;
class IMessageSvc;
class IDictLoaderSvc;
class ITPCnvSvc;

class TrigSerTPTool : public AlgTool {
public:

  static const InterfaceID& interfaceID( ) {
    static const InterfaceID
      IID_TrigSerTPTool("TrigSerTPTool", 1, 0);
    return IID_TrigSerTPTool;
  }

  TrigSerTPTool(const std::string& type, const std::string& name,
			    const IInterface* parent);

  StatusCode initialize();

  void*  convertTP(const std::string &transName, void *trans, std::string &persName);
  void*  convertPT(const std::string &persName, void *pers, std::string &transName);
  std::string persClassName(const std::string &transClassName);
  std::string persClassNameFromCnv(const std::string &cnvname) const;

  StatusCode finalize();

  ~TrigSerTPTool();

private:
  StatusCode initCnvter();
  StatusCode forceDictLoading(const std::string &clname) const;
  StatusCode testTPCnv(const std::string &clname);
  TClass*    getClass(const std::string &clname) const;

private:
  MsgStream                               *m_log;
  MsgStream                               *m_logTP;
  std::map<std::string, std::string>       m_TPmap; //configure - lookup of 
  std::map<std::string, std::string>       m_PTmap; //
  TClass                                  *m_tpCnvBaseClass;
  std::map<std::string, ITPCnvBase*>       m_convBasePtr;
  std::map<void*, TClass*>                 m_convObj; 
  std::vector<std::string>                 m_activeClasses;
  ServiceHandle<IMessageSvc>               m_msgsvcTP;
  ServiceHandle<IDictLoaderSvc>            m_dictSvc;
  bool                                     m_useAthDictLoader;
  bool                                     m_online;
  ServiceHandle<ITPCnvSvc>                 m_tpcnvsvc;
};

#endif // !TRIGSERIALIZETP_TRIGSERTPTOOL_H
