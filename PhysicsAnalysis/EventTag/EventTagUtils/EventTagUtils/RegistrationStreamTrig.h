/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGISTRATIONSVC_REGSTREAMDEFMETA_H
# define REGISTRATIONSVC_REGSTREAMDEFMETA_H

/**
 * @file RegistrationStreamTrig.h
 * @brief Copy the trigger configuration into the TAG file
 * @author Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "TROOT.h"
#include <map>


class IAddressCreator;
class StoreGateSvc;
class CollectionMetadata;

namespace TrigConf {
   class ITrigConfigSvc;
}


class RegistrationStreamTrig : public AthAlgorithm, virtual public IIncidentListener 
{
public:

  /// Standard RegistrationStreamTrig Constructor
  RegistrationStreamTrig(const std::string& name,
                            ISvcLocator* pSvcLocator);

  /// Initialize RegistrationStreamTrig
  virtual StatusCode initialize();
  void handle(const Incident& incident);
  void handleTrigConf(const Incident& incident);
  void addkeys(std::string trigKey, int trigValue, std::string iovtype="RunLB");
  virtual StatusCode execute();

  /// Terminate RegistrationStreamTrig
  virtual StatusCode stop();
  virtual StatusCode finalize();

protected:

  ServiceHandle<StoreGateSvc> m_pOutputStore;
  StringProperty  m_LBColl_name;
  StringProperty  m_incompleteLBColl_name;

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
