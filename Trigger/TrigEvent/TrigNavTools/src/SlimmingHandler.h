/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SlimmingHandler_h
#define SlimmingHandler_h




#include <string>
#include <vector>
#include <stdint.h>
#include "TrigSteeringEvent/HLTResult.h"
#include "xAODTrigger/TrigNavigation.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaKernel/ISlimmingHdlr.h"



template<class ResultType>
class SlimmingHandler : public ::Athena::ISlimmingHdlr {
public:
  SlimmingHandler( ResultType *result, 
		   const INamedInterface *requestor,  
		   const HLT::TrigNavigationSlimmingTool *slimmer );
  
  void commit();
  
  void rollback();
  
  void *object();
  
  std::type_info& type_id();
  
  const ::INamedInterface* requester();
  
  
  void setName(const std::string &name) { m_name = name; };
  std::string name() { return m_name; };
 private:  
  ResultType *m_result;
  const INamedInterface *m_requester;
  
  // nav and cuts data so that you can undo a slim
  std::vector<uint32_t> m_unslimmedNavData;
  
  const HLT::TrigNavigationSlimmingTool *m_slimmer;
  
  std::string m_name;  
};

namespace {
  template<class ResultType>
    void get_data(ResultType* r,   std::vector<uint32_t>& place );  
  template<> void get_data(HLT::HLTResult* r,   std::vector<uint32_t>& place ) {
    place = r->getNavigationResult();  
  }
  template<> void get_data(xAOD::TrigNavigation* r,   std::vector<uint32_t>& place ) {
    place = r->serialized();
  }


  template<class ResultType>
    void replace_data(ResultType* r,  const std::vector<uint32_t>& newdata );
  
  template<> void replace_data(HLT::HLTResult* r, const  std::vector<uint32_t>& newdata ) {
    r->getNavigationResult()  = newdata;  
  }

  template<> void replace_data(xAOD::TrigNavigation* r, const  std::vector<uint32_t>& newdata ) {
    r->setSerialized(  newdata );  
  }


}


template<class ResultType>
SlimmingHandler<ResultType>::SlimmingHandler(ResultType* result,
					     const INamedInterface* requester, 
					     const HLT::TrigNavigationSlimmingTool* slimmer)
  : Athena::ISlimmingHdlr(),
    m_result(result), 

    m_requester(requester), 
    m_slimmer(slimmer) {
  
  // copy the nav data and cuts data so you can undo a slim 
  get_data(m_result, m_unslimmedNavData );
  m_name = requester->name() + "Handler";

}

template<class ResultType>
void SlimmingHandler<ResultType>::commit() {
  
  // do slimming and serialized to the m_result
  std::vector<uint32_t> temp;
  
  if( m_slimmer->doSlimming( temp ).isFailure() ) {
    Athena::MsgStreamMember mlog(Athena::Options::Eager, m_name);
    MsgStream log = mlog.get();    
    log << MSG::WARNING << "HLTResultSlimmingHandler failed execute().  Unable to proceed... bailing out" << endmsg;
    return;
  } 
  
  replace_data(m_result, temp);
  
  //log << MSG::DEBUG << "Performed actual slimming" << endmsg;
}

template<class ResultType>
void SlimmingHandler<ResultType>::rollback() {

  //  Athena::MsgStreamMember mlog(Athena::Options::Eager, m_name);
  //  MsgStream log = mlog.get();
  
  //  replace_data(m_result, m_unslimmedNavData);

  // // write the saved navigation data back into the hltresult
  // std::vector<uint32_t>& navData = m_result->getNavigationResult();
  // std::vector<unsigned int>& cuts = m_result->getNavigationResultCuts();

  // navData.clear();
  // navData.insert(navData.begin(), m_unslimmedNavData.begin(), m_unslimmedNavData.end());
  // cuts.clear();
  // cuts.insert(cuts.begin(), m_unslimmedCuts.begin(), m_unslimmedCuts.end());
  //  log << MSG::DEBUG << "Rolled back the slimming" << endmsg;
}

template<class ResultType>
void *SlimmingHandler<ResultType>::object() {
  return m_result;
}

template<class ResultType>
const ::INamedInterface* SlimmingHandler<ResultType>::requester() {
  return m_requester;
}

template<class ResultType>
std::type_info& SlimmingHandler<ResultType>::type_id() {
  const std::type_info& i(typeid(HLT::HLTResult));
  return const_cast<std::type_info&>(i);
}



#endif

