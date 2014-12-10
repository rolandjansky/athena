/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Polyplane_H
#define Polyplane_H

class Polyplane {
public:
	Polyplane(double ri, double ro, double z):_rin(ri),_rou(ro),_z(z) {;}
	double rin() {return _rin;}
	double rou() {return _rou;}
	double z() {return _z;}
	void rin(double v) {_rin=v;}
	void rou(double v) {_rou=v;}
	void z(double v) {_z=v;}
private:
	double _rin;
	double _rou;
	double _z;
};

#endif
