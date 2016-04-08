/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TwoPoint_H
#define TwoPoint_H

class TwoPoint {
public:
	TwoPoint():m_x(0),m_y(0) {;}
	TwoPoint(double x, double y):m_x(x),m_y(y) {;}
	double x() {return m_x;}
	double y() {return m_y;}
	void x(double v) {m_x=v;}
	void y(double v) {m_y=v;}
private:
	double m_x;
	double m_y;
};

#endif
