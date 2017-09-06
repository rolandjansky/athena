/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_BINPRINT_H
#define DQM_ALGORITHMS_BINPRINT_H

#include <string>

#include "dqm_core/Algorithm.h"


namespace dqm_algorithms {
	
	class BinPrint : public dqm_core::Algorithm {
	public:
		
		BinPrint();
		
		virtual ~BinPrint();
		virtual dqm_core::Algorithm*  clone();
		virtual dqm_core::Result*     execute( const std::string& name, 
											  const TObject& data,
											  const dqm_core::AlgorithmConfig& config );
		using dqm_core::Algorithm::printDescription;
		virtual void                  printDescription(std::ostream& out);
		
	protected:
		
		std::string  m_name;
		
		int m_NbinsX;
		int m_NbinsY;
		int m_UnMask_All;
		int m_UseValue;
		int m_TypeValue;
		double m_Value;
		int m_UseMaskValue;
		double m_MaskValue;
		
		int UseValue_GetFromMap(const std::map<std::string, double> & params);
		int TypeValue_GetFromMap(const std::map<std::string, double> & params);
		double Value_GetFromMap(const std::map<std::string, double> & params);
		int UseMaskValue_GetFromMap(const std::map<std::string, double> & params);
		double MaskValue_GetFromMap(const std::map<std::string, double> & params);
		std::vector<bool> Mask1D_GetFromMap(const std::map<std::string, double> & params);
		std::vector< std::vector<bool> > Mask2D_GetFromMap(const std::map<std::string, double> & params);
	};
	
} //namespace dqm_algorithms

#endif // DQM_ALGORITHMS_BINPRINT_H
