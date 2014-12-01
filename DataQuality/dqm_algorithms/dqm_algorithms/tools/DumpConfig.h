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

        std::ofstream _myfile;
	std::map<std::string,double> params;
	std::map<std::string,double> gthresh;
	std::map<std::string,double> rthresh;
	std::vector<std::string> red_id;
	std::vector<std::string> green_id;
	std::vector<std::string> param_id;
     
	std::string _ParameterName;
	dqm_core::test::DummyAlgorithmConfig  _config;
	std::string _regionname;
	std::string _algorithmname;
	std::string _histogramname;
	std::string _refhistogramname;
	std::string _reffilename;
	float _weight;
	
    };
  }
}

#endif
