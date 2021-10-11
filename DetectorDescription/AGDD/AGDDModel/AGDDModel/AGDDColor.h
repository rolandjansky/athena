/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDColor_H
#define AGDDColor_H

#include <string>

class AGDDColorStore;

class AGDDColor {
public:
	AGDDColor(AGDDColorStore& cs,
                  const std::string& n,
                  double a,double b,double c):m_name(n),m_red(a),m_green(b),m_blue(c) 
	{
		RegisterToStore(cs);
	}
	double Red() const {return m_red;}
	double Green() const {return m_green;}
	double Blue() const {return m_blue;}
	std::string GetName() const {return m_name;}
private:
	std::string m_name;
	void RegisterToStore(AGDDColorStore& cs);
	double m_red,m_green,m_blue;
};

#endif
