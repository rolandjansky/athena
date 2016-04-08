/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Polyplane_H
#define Polyplane_H

class Polyplane {
public:
	Polyplane(double ri, double ro, double z):m_rin(ri),m_rou(ro),m_z(z) {;}
	double rin() {return m_rin;}
	double rou() {return m_rou;}
	double z() {return m_z;}
	void rin(double v) {m_rin=v;}
	void rou(double v) {m_rou=v;}
	void z(double v) {m_z=v;}
private:
	double m_rin;
	double m_rou;
	double m_z;
};

#endif
