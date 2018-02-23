/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#ifndef TRIGGLOBALEFFICIENCYCORRECTION_EFFICIENCIES_H
#define TRIGGLOBALEFFICIENCYCORRECTION_EFFICIENCIES_H 1

#include <utility>

namespace TrigGlobEffCorr
{

class Efficiencies
{
public:
	Efficiencies() = default;
	Efficiencies(double e) : m_effs(e,e) {}
	Efficiencies(const Efficiencies&) = default;
	Efficiencies(Efficiencies&&) = default;
	Efficiencies& operator=(const Efficiencies&) = default;
	Efficiencies& operator=(Efficiencies&&) = default;
	~Efficiencies() = default;
	double& data() { return m_effs.first; }
	double& mc() { return m_effs.second; }
	double data() const { return m_effs.first; }
	double mc() const { return m_effs.second; }	
	Efficiencies operator~() const { return Efficiencies(1. - m_effs.first, 1. - m_effs.second); }
	Efficiencies operator+(const Efficiencies& rhs) const { return Efficiencies(m_effs.first+rhs.m_effs.first, m_effs.second+rhs.m_effs.second); }
	Efficiencies operator-(const Efficiencies& rhs) const { return Efficiencies(m_effs.first-rhs.m_effs.first, m_effs.second-rhs.m_effs.second); }
	Efficiencies operator*(const Efficiencies& rhs) const { return Efficiencies(m_effs.first*rhs.m_effs.first, m_effs.second*rhs.m_effs.second); }
	Efficiencies& operator+=(const Efficiencies& rhs) { m_effs.first+=rhs.m_effs.first; m_effs.second+=rhs.m_effs.second; return *this; }
	Efficiencies& operator*=(const Efficiencies& rhs) { m_effs.first*=rhs.m_effs.first; m_effs.second*=rhs.m_effs.second; return *this; }
protected:
	Efficiencies(double vd, double vm) : m_effs(vd,vm) {}
	std::pair<double,double> m_effs;
};

}

#endif
