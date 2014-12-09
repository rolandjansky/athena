/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDBolt_H
#define AGDDBolt_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDBolt: public AGDDVolume {
public:
	AGDDBolt(std::string s):AGDDVolume(s),_length(0),_headLength(0),
										 _diameter(0),_headDiameter(0) {}
	~AGDDBolt() {}
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
