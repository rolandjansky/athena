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
	AGDDIntersection(const std::string& s):AGDDVolume(s) {}
        virtual void CreateVolume (const AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
private:
};

#endif
