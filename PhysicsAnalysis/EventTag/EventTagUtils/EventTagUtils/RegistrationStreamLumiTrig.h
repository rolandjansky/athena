/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGISTRATIONSVC_REGSTREAMDEFMETA_H
# define REGISTRATIONSVC_REGSTREAMDEFMETA_H

/**
 * @file RegistrationStreamLumiTrig.h
 * @brief Copy the complete and incomplete lumi blocks from AOD
 * and store them to TAG metadata
 * @author Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "TROOT.h"
#include <map>


class IAddressCreator;
class StoreGateSvc;
class CollectionMetadata;
class LumiBlockCollection;

namespace TrigConf {
   class ITrigConfigSvc;
}


class RegistrationStreamLumiTrig : public AthAlgorithm, virtual public IIncidentListener 
{
public:

  /// Standard RegistrationStreamLumiTrig Constructor
  RegistrationStreamLumiTrig(const std::string& name,
                            ISvcLocator* pSvcLocator);

  /// Initialize RegistrationStreamLumiTrig
  virtual StatusCode initialize();
  void handle(const Incident& incident);
  void handleLumiBlocks(const Incident& incident);
  void handleTrigConf(const Incident& incident);
  void addkeys(std::string trigKey, int trigValue, std::string iovtype="RunLB");
  virtual StatusCode execute();

  /// Terminate RegistrationStreamLumiTrig
  virtual StatusCode stop();
  virtual StatusCode finalize();

protected:

  /// Standard Destructor
  virtual ~RegistrationStreamLumiTrig();

  ServiceHandle<StoreGateSvc> m_pOutputStore;
  ServiceHandle<StoreGateSvc> m_pInputStore;
  StringProperty  m_LBColl_name;
  StringProperty  m_incompleteLBColl_name;

  TString xmlstring;
  TString incompletexmlstring;
  std::string m_lumixml;
  std::string m_incompletelumixml;
  bool m_gettriggerconf;

  std::map<std::string, std::string> m_lumitrig;

  const xAOD::EventInfo* m_eventInfo;
  ServiceHandle< TrigConf::ITrigConfigSvc > m_trigConfSvc;
  unsigned long m_run;
  unsigned int  m_lb;

  int m_LBStart;
  int m_SMKStart;
  int m_L1PSKStart;
  int m_HLTPSKStart;

  bool m_isFirstTrigIncidence;

};


#endif 
