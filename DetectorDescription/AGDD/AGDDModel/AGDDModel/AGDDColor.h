/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDColor_H
#define AGDDColor_H

#include <string>

class AGDDColor {
public:
	AGDDColor(std::string n,double a,double b,double c):m_name(n),m_red(a),m_green(b),m_blue(c) 
	{
		RegisterToStore();
	}
	double Red() {return m_red;}
	double Green() {return m_green;}
	double Blue() {return m_blue;}
	std::string GetName() {return m_name;}
private:
	std::string m_name;
	void RegisterToStore();
	double m_red,m_green,m_blue;
};

#endif
