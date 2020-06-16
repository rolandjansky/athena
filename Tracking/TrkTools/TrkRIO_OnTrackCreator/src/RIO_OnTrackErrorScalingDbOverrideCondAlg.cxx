/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkRIO_OnTrack/RIO_OnTrackErrorScaling.h"
#include "RIO_OnTrackErrorScalingDbOverrideCondAlg.h"
#include "RIO_OnTrackErrorScalingKit.h"
#include "CxxUtils/checker_macros.h"

RIO_OnTrackErrorScalingDbOverrideCondAlg::RIO_OnTrackErrorScalingDbOverrideCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_condSvc{"CondSvc", name}
{
}

StatusCode RIO_OnTrackErrorScalingDbOverrideCondAlg::initialize ATLAS_NOT_THREAD_SAFE () {
  ATH_CHECK(m_condSvc.retrieve());
  if (m_writeKey.key().empty()) {
    ATH_MSG_FATAL("No conditions data write key specified.");
  }
  ATH_CHECK(m_writeKey.initialize());
  m_errorScalingDataKit = &(RIO_OnTrackErrorScalingKitManager::instance().kit( m_errorScalingTypeName ));
  unsigned int n_parameters_total=0;
  unsigned int n_parameter_sets = m_errorScalingDataKit->nParametres();
  if (n_parameter_sets>0) {
    if (!m_nParameterPerSet.empty()) {
      if (m_nParameterPerSet.size() != n_parameter_sets) {
        ATH_MSG_FATAL("PrametersPerSet must either be empty or match the number of parameters of the error scaling class " << m_errorScalingTypeName);
        return StatusCode::FAILURE;
      }
      
        for (unsigned int n_parameters : m_nParameterPerSet) {
          n_parameters_total += n_parameters;
        }
      
      m_useNParametersPerSet = m_nParameterPerSet;
    }
    else {
      unsigned int n_parameters_per_set=m_errorScalingParameters.size()/n_parameter_sets;
      n_parameters_total = n_parameter_sets * n_parameters_per_set;
      m_useNParametersPerSet.clear();
      m_useNParametersPerSet.reserve(n_parameter_sets);
      for (unsigned int param_set_i=0; param_set_i<n_parameter_sets; ++param_set_i ) {
        m_useNParametersPerSet.push_back( n_parameters_per_set);
      }
      ATH_MSG_VERBOSE("init n_parameters_total=" <<  n_parameters_total << " =?= " << m_errorScalingParameters.size()
                      << " n_parameters_per_set=" << n_parameters_per_set );
    }
    if (n_parameters_total != m_errorScalingParameters.size()) {
      ATH_MSG_FATAL("Number of parameters in ErorScalingParameters does not match expectation. Estimated " << n_parameters_total 
                    << " parameters in total for the " << n_parameter_sets << " parameter sets of " << m_errorScalingTypeName
                    << " but got " << m_errorScalingParameters.size());
      return StatusCode::FAILURE;
    }
  }
  if(msgLvl(MSG::VERBOSE)) {
    for(unsigned int param_i=0; param_i<m_errorScalingDataKit->nParametres(); ++param_i) {
      ATH_MSG_VERBOSE("No error scaling parameters for " << m_writeKey.key() << " " << m_errorScalingDataKit->paramNames()[param_i] 
                      << " n-params="
                      << m_useNParametersPerSet[param_i]);
    }
  }
  if (n_parameters_total==0 && !m_errorScalingParameters.empty()) {
    ATH_MSG_FATAL("ErorScalingParameters expected to be empty for " << m_errorScalingTypeName) ;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

namespace {
  EventIDBase timestamp (int t)
  {
    return EventIDBase (EventIDBase::UNDEFNUM,  // run
                        EventIDBase::UNDEFEVT,  // event
                        t);
  }

  EventIDBase infiniteIOVEend() {
    return timestamp(std::numeric_limits<int>::max() - 1);
  }

  template <typename T>
  std::ostream &operator<<(std::ostream &out, const std::vector<T> &a) {
    for (const T &elm : a) {
      out << " " << elm;
    }
    return out;
  }
}


StatusCode RIO_OnTrackErrorScalingDbOverrideCondAlg::execute() {
  SG::WriteCondHandle<RIO_OnTrackErrorScaling> write_handle(m_writeKey);
  if (!write_handle.isValid()) {
    EventIDRange range(timestamp(0),infiniteIOVEend());
    std::unique_ptr<RIO_OnTrackErrorScaling> error_scaling( m_errorScalingDataKit->create() );
    std::vector<float>::const_iterator  param_iter = m_errorScalingParameters.begin();
    unsigned int set_i=0;
    for (unsigned int n_params : m_useNParametersPerSet ) {
      assert( set_i < error_scaling->params().size() );
      error_scaling->params()[set_i].clear();
      error_scaling->params()[set_i].reserve(n_params);
      for(unsigned int param_i=0; param_i<n_params; ++param_i, ++param_iter) {
        assert( param_iter != m_errorScalingParameters.end() );
        error_scaling->params()[set_i].push_back( *param_iter );
      }
      ++set_i;
    }

    // dump error scaling parameters
    if(msgLvl(MSG::VERBOSE)) {
      for(unsigned int param_i=0; param_i<m_errorScalingDataKit->nParametres(); ++param_i) {
        if (param_i>= error_scaling->params().size()) {
          ATH_MSG_FATAL("No error scaling parameters for " << write_handle.key() << " " << m_errorScalingDataKit->paramNames()[param_i] << ".");
          return StatusCode::FAILURE;
        }
        
          ATH_MSG_VERBOSE("Parameters for " << write_handle.key() << " " << m_errorScalingDataKit->paramNames()[param_i] << error_scaling->params()[param_i] );
        
      }
    }

    if (!error_scaling->postProcess()) {
      ATH_MSG_ERROR("Conditions data for " << write_handle.key() << " not valid.");
      return StatusCode::FAILURE;
    }
    if (write_handle.record(range, std::move(error_scaling)).isFailure()) {
      ATH_MSG_FATAL("Could not record RIO_OnTrackErrorScaling " << write_handle.key()
                    << " with EventRange " << range
                    << " into Conditions Store");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode RIO_OnTrackErrorScalingDbOverrideCondAlg::finalize() {
  return StatusCode::SUCCESS;
}
