/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDBolt_H
#define AGDDBolt_H

#include "AGDD2Geo/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDBolt: public AGDDVolume {
public:
	AGDDBolt(std::string s):AGDDVolume(s) {}
	void CreateVolume();
	void CreateSolid();
	
	void SetHeadLength(double l) {_headLength=l;}
	void SetLength(double l) {_length=l;}
	void SetHeadDiameter(double l) {_headDiameter=l;}
	void SetDiameter(double l) {_diameter=l;}
	
	double GetHeadLength() {return _headLength;}
	double GetLength() {return _length;}
	double GetHeadDiameter() {return _headDiameter;}
	double GetDiameter() {return _diameter;}
	
	double _length;
	double _headLength;
	double _diameter;
	double _headDiameter;
};

#endif
