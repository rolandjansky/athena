/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDBolt_H
#define AGDDBolt_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDBolt: public AGDDVolume {
public:
	AGDDBolt(const std::string& s,
                 AGDDVolumeStore& vs,
                 AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss),m_length(0),m_headLength(0),
										 m_diameter(0),m_headDiameter(0) {}
	~AGDDBolt() {}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
	
	void SetHeadLength(double l) {m_headLength=l;}
	void SetLength(double l) {m_length=l;}
	void SetHeadDiameter(double l) {m_headDiameter=l;}
	void SetDiameter(double l) {m_diameter=l;}
	
	double GetHeadLength() const {return m_headLength;}
	double GetLength() const {return m_length;}
	double GetHeadDiameter() const {return m_headDiameter;}
	double GetDiameter() const {return m_diameter;}
	
	double m_length;
	double m_headLength;
	double m_diameter;
	double m_headDiameter;
};

#endif
