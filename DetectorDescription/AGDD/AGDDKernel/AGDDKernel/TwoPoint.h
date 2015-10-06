/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TwoPoint_H
#define TwoPoint_H

class TwoPoint {
public:
	TwoPoint():_x(0),_y(0) {;}
	TwoPoint(double x, double y):_x(x),_y(y) {;}
	double x() {return _x;}
	double y() {return _y;}
	void x(double v) {_x=v;}
	void y(double v) {_y=v;}
private:
	double _x;
	double _y;
};

#endif
