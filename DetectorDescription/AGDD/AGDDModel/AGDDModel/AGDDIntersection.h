/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDIntersection_H
#define AGDDIntersection_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDIntersection: public AGDDVolume {
public:
	AGDDIntersection(const std::string& s,
                         AGDDVolumeStore& vs,
                         AGDDSectionStore& ss):AGDDVolume(s,vs,ss) {}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
private:
};

#endif
