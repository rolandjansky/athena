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
	AGDDComposition(std::string s):AGDDVolume(s) {}
	~AGDDComposition() {}
        virtual void CreateVolume (const AGDDBuilder& builder) override;
private:
};

#endif
