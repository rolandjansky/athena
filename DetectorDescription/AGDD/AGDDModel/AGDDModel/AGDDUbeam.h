/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDUbeam_H
#define AGDDUbeam_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDUbeam: public AGDDVolume {
public:
	AGDDUbeam(const std::string& s,
                  AGDDVolumeStore& vs,
                  AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss),m_width(0),m_smallWidth(0),m_height(0),
            m_smallHeight(0),m_z(0) {}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
	
	void SetWidth(double l) {m_width=l;}
	void SetSmallWidth(double l) {m_smallWidth=l;}
	void SetHeigth(double l) {m_height=l;}
	void SetSmallHeigth(double l) {m_smallHeight=l;}
	void SetLength(double l) {m_z=l;}
	
	double GetWidth() const {return m_width;}
	double GetSmallWidth() const {return m_smallWidth;}
	double GetHeigth() const {return m_height;}
	double GetSmallHeigth() const {return m_smallHeight;}
	double GetLength() const {return m_z;}
	
	double m_width;
	double m_smallWidth;
	double m_height;
	double m_smallHeight;
	double m_z;
};

#endif
