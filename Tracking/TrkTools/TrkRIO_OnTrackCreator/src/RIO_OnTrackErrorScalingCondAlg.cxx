/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkRIO_OnTrack/RIO_OnTrackErrorScaling.h"
#include "InDetRIO_OnTrack/PixelRIO_OnTrackErrorScaling.h"
#include "RIO_OnTrackErrorScalingCondAlg.h"
#include "RIO_OnTrackErrorScalingKit.h"

#include <limits>

RIO_OnTrackErrorScalingCondAlg::RIO_OnTrackErrorScalingCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_condSvc{"CondSvc", name}
{
}

StatusCode RIO_OnTrackErrorScalingCondAlg::initialize() {
  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_readKey.initialize());

  if (m_errorScalingType.size() != m_writeKey.size()) {
    ATH_MSG_FATAL("Number of error scaling types and out keys do not match: " << m_errorScalingType.size() << " != " << m_writeKey.size() 
                  << " There should be exactly one out key for each error scaling type.");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_writeKey.initialize() );

  m_kits.clear();
  m_kits.reserve(m_errorScalingType.size());
  unsigned int idx=0;
  for (const std::string &type_name : m_errorScalingType ) {
    if (addErrorScaling(type_name).isFailure()) return StatusCode::FAILURE;
    if (m_condSvc->regHandle(this, m_writeKey[idx]).isFailure()) {
      ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey[idx].key() << " with CondSvc");
      return StatusCode::FAILURE;
    }
    ++idx;
  }
  for (const std::string &attribut_name : m_attributIgnoreList ) {
    registerAttribute(attribut_name, std::numeric_limits<unsigned int>::max(),0);
  }

  return StatusCode::SUCCESS;
}


void RIO_OnTrackErrorScalingCondAlg::registerAttribute(std::string name, unsigned int type_idx, unsigned int param_idx) {
  if (!m_attributeMap.insert( std::make_pair(name, std::make_pair(type_idx,param_idx)) ).second) {
    std::stringstream message;
    message << "Failed to add RIO_OnTrackErrorScaling paramter : " << name << ".";
    throw std::runtime_error(message.str());
  }
}

StatusCode RIO_OnTrackErrorScalingCondAlg::addErrorScaling(const std::string &type_name) {
  const RIO_OnTrackErrorScalingKit *the_kit(nullptr);
  try {
    the_kit = &(RIO_OnTrackErrorScalingKitManager::instance().kit( type_name ));
  }
  catch (std::runtime_error &err) {
    std::stringstream types;
    RIO_OnTrackErrorScalingKitManager::instance().dumpKits(types);
    ATH_MSG_FATAL( "Invalide ErrorScalking type name : " << type_name << ". Registered types:" << types.str() );
    return StatusCode::FAILURE;
  }
  catch (std::exception &err) {
    std::stringstream types;
    RIO_OnTrackErrorScalingKitManager::instance().dumpKits(types);
    ATH_MSG_FATAL( "Caught exception: " << err.what() << " Invalide ErrorScalking type name : " << type_name << ". Registered types:" << types.str() );
    return StatusCode::FAILURE;
  }
  m_kits.push_back(the_kit);
  const char* const* parameters=the_kit->paramNames();
  for (unsigned int param_i=0; param_i<the_kit->nParametres(); ++param_i) {
    registerAttribute( std::string(parameters[param_i]), m_kits.size()-1, param_i);
  }

  return StatusCode::SUCCESS;
}

template <typename T>
class CSAccessCondReadHandleKey : public SG::ReadCondHandleKey<T> {
public:
  std::string dump() const {
    return this->getCS()->dump();
  }

  template <typename T_Obj>
  std::string dumpKeys() const {
    std::stringstream out;
    std::vector<std::string> keys_out;
    const_cast<StoreGateSvc *>(this->getCS())->keys<T_Obj>(keys_out,true,false);
    for(const std::string &a_key : keys_out) {
      out << " " << a_key;
    }
    return out.str();
  }
};


StatusCode RIO_OnTrackErrorScalingCondAlg::execute() {
  SG::ReadCondHandle<CondAttrListCollection> readHandle(m_readKey);
  if (!readHandle.isValid()) {
    return StatusCode::FAILURE;
  }
  assert( *readHandle );
  EventIDRange range;
  if (!readHandle.range(range)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }

  std::vector<SG::WriteCondHandle<RIO_OnTrackErrorScaling> > write_handles(m_writeKey.makeHandles());
  assert( write_handles.size() == m_kits.size() );
  std::vector< std::unique_ptr<RIO_OnTrackErrorScaling> > error_scaling;
  error_scaling.reserve( m_kits.size());

  unsigned int total_params=0;
  for (const RIO_OnTrackErrorScalingKit *a_kit: m_kits) {
    assert( a_kit );
    error_scaling.push_back( a_kit->create() );
    total_params += a_kit->nParametres();
  }
  assert( write_handles.size() == error_scaling.size());

  try {
    // now populate output conditions data objects from attribut lists.
    for (const std::pair<unsigned int, coral::AttributeList>& channel : **readHandle ) {
      const coral::AttributeList &alist = channel.second;
      unsigned int att_i=0;
      const std::string& attr_name=alist[att_i++].data<std::string>();
      try {
        std::pair<unsigned int, unsigned int> idx = m_attributeMap.at(attr_name);
        if (idx.first != std::numeric_limits<unsigned int>::max()) {
          assert( idx.first < error_scaling.size());
          assert( idx.second < error_scaling[idx.first]->params().size());

          const int nvals=alist[att_i++].data<int>();
          std::vector<double> &params = error_scaling[idx.first]->params()[idx.second];
          params.clear();
          params.reserve(nvals);
          assert( att_i == 2);
          for (int i=0;i<nvals;++i, ++att_i){
            assert( att_i < alist.size() );
            // ATH_MSG_VERBOSE("Parameter " << i << " = " << alist[att_i].data<double>() );
            params.push_back(alist[att_i].data<double>());
          }
        }
        --total_params;
      }
      catch (std::out_of_range &err) {
        ATH_MSG_FATAL("Unhandled attribute: " << attr_name);
        return StatusCode::FAILURE;
      }
      catch (std::exception &err) {
        ATH_MSG_FATAL("Unhandled attribute: " << attr_name << " what:" << err.what());
        return StatusCode::FAILURE;
      }
    }
    if (total_params!=0) {
      ATH_MSG_ERROR("Not all parameters of the output conditions data are filled from attribute list. " << total_params << " parameters are not filled.");
    }

    for (unsigned int key_i=0; key_i<write_handles.size(); ++key_i) {
      // assert( key_i < m_writeKey.size());
      assert( key_i < error_scaling.size());
      if (!error_scaling[key_i]->postProcess()) {
        ATH_MSG_ERROR("Conditions data for " << write_handles[key_i].key() << " not valid.");
        return StatusCode::FAILURE;
      }
      // CLID impl_clid=error_scaling[key_i]->clid();
      if (write_handles[key_i].record(range, std::move(error_scaling[key_i])).isFailure()) {
        ATH_MSG_FATAL("Could not record RIO_OnTrackErrorScaling " << write_handles[key_i].key()
                      << " with EventRange " << range
                      << " into Conditions Store");
        return StatusCode::FAILURE;
      }
    }
  }
  catch (coral::Exception& e) {
    ATH_MSG_ERROR("Problem with AttributeList decoding: " << e.what());
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode RIO_OnTrackErrorScalingCondAlg::finalize() {
  return StatusCode::SUCCESS;
}
