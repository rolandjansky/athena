/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDColor_H
#define AGDDColor_H

#include <string>

class AGDDColor {
public:
	AGDDColor(std::string n,double a,double b,double c):name(n),red(a),green(b),blue(c) 
	{
		RegisterToStore();
	}
	double Red() {return red;}
	double Green() {return green;}
	double Blue() {return blue;}
	std::string GetName() {return name;}
private:
	std::string name;
	void RegisterToStore();
	double red,green,blue;
};

#endif
