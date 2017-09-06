/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_BINTHRESH_H
#define DQM_ALGORITHMS_BINTHRESH_H

#include <string>

#include "dqm_core/Algorithm.h"

namespace dqm_algorithms {
	
	class BinThresh : public dqm_core::Algorithm {
	public:
		
		BinThresh();
		
		virtual ~BinThresh();
		virtual dqm_core::Algorithm*  clone();
		virtual dqm_core::Result*     execute( const std::string& name, 
											  const TObject& data,
											  const dqm_core::AlgorithmConfig& config );
		using dqm_core::Algorithm::printDescription;
		virtual void                  printDescription(std::ostream& out);

	protected:
		struct mask_limits {
			bool Mask;
			double WarningValue;
			double ErrorValue;
		};

	protected:
		
		std::string  m_name;
		
		//Histogram configuration parameters
		int m_NbinsX;
		int m_NbinsY;
		
		int Publish_GetFromMap(const std::map<std::string, double> & params);
		int TypePublish_GetFromMap(const std::map<std::string, double> & params);
		int UseValue_GetFromMap(const std::map<std::string, double> & params);
		int TypeValue_GetFromMap(const std::map<std::string, double> & params);
		int BinMinEntries_GetFromMap(const std::map<std::string, double> & params);
		std::vector<BinThresh::mask_limits> Limits1D_GetFromMap( const std::map<std::string, double> & params, 
																const std::map<std::string, double> & warning_params, 
																const std::map<std::string, double> & error_params );
		std::vector< std::vector<BinThresh::mask_limits> > Limits2D_GetFromMap( const std::map< std::string, double> & params, 
																			   const std::map< std::string, double> & warning_params, 
																			   const std::map< std::string, double> & error_params );
	};
	
} //namespace dqm_algorithms

#endif // DQM_ALGORITHMS_BINTHRESH_H
