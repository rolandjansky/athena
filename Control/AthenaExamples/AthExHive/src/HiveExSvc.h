#ifndef HIVEEXSVC_H
#define HIVEEXSVC_H 1
 
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ContextSpecificPtr.h"

#include "AthExHive/IHiveExSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/EventContext.h"
#include <string>
#include <vector>

class HiveExSvc : public extends1<AthService,IHiveExSvc> {

public:
   HiveExSvc(const std::string& name, ISvcLocator* svc);
   virtual ~HiveExSvc();
 
public:
   virtual StatusCode initialize();
   virtual StatusCode finalize();
 
public:
  
  int get() const;
  void set(const int&);

  void add(const std::string&, const int&);

  int getTL() const;
  void setTL(const int&);


private:

  class CSPLog {
  public:
    CSPLog(MsgStream &log): m_log(log){};
    void operator()(int *n) {  m_log << " val: " << *n << std::endl; }
    void operator()(Gaudi::Hive::ContextIdType i, int *n) { 
      m_log << "s: " << i << " val: " << *n << std::endl; }

  private:
    MsgStream &m_log;
  } ;


  Gaudi::Hive::ContextSpecificPtr<int> m_ci;
  Gaudi::Hive::ContextSpecificData<int> m_di;

  struct sDat {
    sDat() : nSlot(EventContext::INVALID_CONTEXT_ID),
	     nEvt(EventContext::INVALID_CONTEXT_EVT), time(0), name(""){};
    sDat(EventContext::ContextID_t s, EventContext::ContextEvt_t e, 
	 const int& t, const std::string& n): nSlot(s), nEvt(e), time(t),
					      name(n) {};
    EventContext::ContextID_t nSlot;
    EventContext::ContextEvt_t nEvt;
    int time;
    std::string name;
  };

  Gaudi::Hive::ContextSpecificData< std::vector< sDat > > m_dq;

  //  thread_local int s_i;

};

#endif
