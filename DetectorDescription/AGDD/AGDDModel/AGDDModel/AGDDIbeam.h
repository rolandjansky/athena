/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDIbeam_H
#define AGDDIbeam_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDIbeam: public AGDDVolume {
public:
	AGDDIbeam(std::string s):AGDDVolume(s),_width(0),_smallWidth(0),_height(0),
								_smallHeight(0),_z(0) {}
	void CreateVolume();
	void CreateSolid();
	
	void SetWidth(double l) {_width=l;}
	void SetSmallWidth(double l) {_smallWidth=l;}
	void SetHeigth(double l) {_height=l;}
	void SetSmallHeigth(double l) {_smallHeight=l;}
	void SetLength(double l) {_z=l;}
	
	double GetWidth() {return _width;}
	double GetSmallWidth() {return _smallWidth;}
	double GetHeigth() {return _height;}
	double GetSmallHeigth() {return _smallHeight;}
	double GetLength() {return _z;}
	
	double _width;
	double _smallWidth;
	double _height;
	double _smallHeight;
	double _z;
};

#endif
