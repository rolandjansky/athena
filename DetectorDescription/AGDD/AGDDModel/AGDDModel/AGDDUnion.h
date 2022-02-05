/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDUnion_H
#define AGDDUnion_H

#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>

class AGDDUnion: public AGDDVolume {
public:
	AGDDUnion(const std::string& s,
                  AGDDVolumeStore& vs,
                  AGDDSectionStore& ss)
          : AGDDVolume(s,vs,ss) {}
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
private:
};

#endif
