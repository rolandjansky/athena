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
	AGDDSubtraction(const std::string& s):AGDDVolume(s) {}
        virtual void CreateVolume (const AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;
private:
};

#endif
