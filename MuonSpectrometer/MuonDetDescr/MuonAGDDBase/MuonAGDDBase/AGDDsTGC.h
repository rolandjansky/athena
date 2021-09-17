/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDsTGC_H
#define AGDDsTGC_H

#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>



class AGDDsTGC: public sTGCDetectorDescription, public AGDDVolume {
public:
    AGDDsTGC(const std::string& s);

        virtual void CreateVolume (const AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;

private:

};

#endif
