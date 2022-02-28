/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDComposition_H
#define AGDDComposition_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDComposition: public AGDDVolume {
public:
	AGDDComposition(const std::string& s,
                        AGDDVolumeStore& vs,
                        AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss) {}
	virtual ~AGDDComposition() {}
        virtual void CreateVolume (AGDDBuilder& builder) override;
private:
};

#endif
