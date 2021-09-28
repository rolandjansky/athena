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
	AGDDUnion(const std::string& s):AGDDVolume(s) {}
        virtual void CreateVolume (const AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
private:
};

#endif
