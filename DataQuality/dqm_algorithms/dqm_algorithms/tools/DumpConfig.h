/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file AlgorithmHelper.cpp does basic functions to get dqm_core::Results from algorithms 
 * \author Haleh Hadavand
 */
#ifndef DQM_ALGORITHMS_TEST_DUMPCONFIG_H
#define DQM_ALGORITHMS_TEST_DUMPCONFIG_H

#include <map>
#include <iostream>
#include <fstream>
#include <dqm_core/test/DummyAlgorithmConfig.h>

namespace dqm_algorithms
{
  namespace tools
    {
      class DumpConfig {
	public:
	DumpConfig(std::string ParameterName,dqm_core::test::DummyAlgorithmConfig & config, std::string algorithmname, std::string histogramname, std::string reffilename="", std::string refhistogramname="", float weight=1.,std::string regionname="" );
	~DumpConfig();
	
	void DumpOnlineConfig(std::string filename,bool dumpAgent=true);
        void DumpOfflineConfig(std::string filename);

	private:
	void WriteThresholdFromMap(std::map<std::string,double> object,std::string ParameterName,std::string Name);
	void DumpThresholds();
	void DumpParams();
	void DumpRegion();
	void DumpAgent();

        std::ofstream m_myfile;
	std::map<std::string,double> m_params;
	std::map<std::string,double> m_gthresh;
	std::map<std::string,double> m_rthresh;
	std::vector<std::string> m_red_id;
	std::vector<std::string> m_green_id;
	std::vector<std::string> m_param_id;
     
	std::string m_ParameterName;
	dqm_core::test::DummyAlgorithmConfig  m_config;
	std::string m_regionname;
	std::string m_algorithmname;
	std::string m_histogramname;
	std::string m_refhistogramname;
	std::string m_reffilename;
	float m_weight;
	
    };
  }
}

#endif
