/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETSTATUS_DETSTATSVC_H
#define DETSTATUS_DETSTATSVC_H
// DetStatusSvc.h - concrete implementation of service managing detector status
// Richard Hawkings, started 13/12/06

#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGate.h"
#include "DetectorStatus/IDetStatusSvc.h"

class StoreGateSvc;
class DetStatusMap;

class DetStatusSvc : public virtual IDetStatusSvc, public virtual AthService
{
  template <class TYPE> class SvcFactory;

public:
  DetStatusSvc(const std::string& name, ISvcLocator* svc);
  virtual ~DetStatusSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  virtual const InterfaceID& type() const;

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // print all the detector status information
  virtual void print() const;

  // return a pointer to the DetStatus object with the given name
  // return 0 if there is none, or the DetStatus map cannot be found
  virtual const DetStatus* findStatus(const std::string& name) const;

  // return const iterators to the DetStatusMap, or null if it cannot be found
  virtual void getIter(DetStatusMap::const_iterator& begin,
		       DetStatusMap::const_iterator& end) const;

  // return true if the event is vetoed, according to the jo requirements
  virtual bool vetoed() const;

 private:
  StatusCode parseDetReqString();
  StatusCode update( IOVSVC_CALLBACK_ARGS );
  const DetStatusMap* getStatusMap() const;

  ServiceHandle<StoreGateSvc> m_detStore;

  std::string m_par_conddbfolder;
  // detstatus parameters given as pairs of values in separate params
  std::vector<std::string> m_par_detname;
  std::vector<int> m_par_detreq;
  // detstatus parameters given as a single string
  std::string m_par_detreqs;
  // resulting actual list of pairs used
  std::vector<std::string> m_detname;
  std::vector<int> m_detreq;
  const std::string m_sgkey;
};

#endif // DETSTATUS_DETSTATSVC_H

