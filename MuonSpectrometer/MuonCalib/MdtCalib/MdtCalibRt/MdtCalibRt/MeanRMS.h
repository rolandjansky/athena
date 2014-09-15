/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "cmath"


namespace MuonCalib {



class MeanRMS
	{
	public:
		inline MeanRMS() : m_sum_x(0.), m_sum_x_sq(0.), n_ent(0) {}
		inline void Insert(double x)
			{
			m_sum_x += x;
			m_sum_x_sq += x*x;
			n_ent++;
			}
		inline double GetMean() const
			{
			if(n_ent==0) return 0;
			return m_sum_x/static_cast<double>(n_ent);
			}
		inline double GetRMS() const
			{
			if(n_ent==0) return 0;
			double mean=GetMean();
			return std::sqrt(m_sum_x_sq/static_cast<double>(n_ent) - mean*mean);
			}
		inline int GetN() const
			{
			return n_ent;
			}
	private:
		double m_sum_x;
		double m_sum_x_sq;
		int n_ent;	
		
	};
}
