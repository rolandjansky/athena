/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiHanAlgorithmConfig_h
#define dqiHanAlgorithmConfig_h

#include "dqm_core/AlgorithmConfig.h"
#include "dqm_core/exceptions.h"

class TDirectory;
class TFile;
class TKey;


namespace dqi {

class HanConfigAssessor;


class HanAlgorithmConfig : public dqm_core::AlgorithmConfig {
public:

  HanAlgorithmConfig( const HanConfigAssessor& hca, TFile* config );

  HanAlgorithmConfig( TObject* reference, 
		      const std::map< std::string, double >& parameters,
		      const std::map< std::string, double >& greenThresholds,
		      const std::map< std::string, double >& redThresholds,
		      const HanConfigAssessor* hca);

  virtual ~HanAlgorithmConfig();
  
  virtual TObject*                                      getReference() const throw (dqm_core::BadConfig);
  
  virtual const std::map< std::string, double >&  getParameters() const;
  
  virtual const std::map< std::string, double >&               getGreenThresholds() const;
  
  virtual const std::map< std::string, double >&               getRedThresholds() const;


protected:

  void CopyAlgConfig( const HanConfigAssessor& hca );

  TFile* m_file;
  
  TObject*  m_ref;
  std::map< std::string, double >  m_pars;
  std::map<std::string,double>  m_grthr;
  std::map<std::string,double>  m_rdthr;
  const HanConfigAssessor* m_hca;

private:
};

} // namespace dqi

#endif

