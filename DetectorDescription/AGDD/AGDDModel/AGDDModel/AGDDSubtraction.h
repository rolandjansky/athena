/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDSubtraction_H
#define AGDDSubtraction_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDSubtraction: public AGDDVolume {
public:
	AGDDSubtraction(const std::string& s,
                        AGDDVolumeStore& vs,
                        AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss) {}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
private:
};

#endif
