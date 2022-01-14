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
class AGDDVolumeStore;
class AGDDSectionStore;


class AGDDsTGC: public sTGCDetectorDescription, public AGDDVolume {
public:
    AGDDsTGC(const std::string& s,
             AGDDDetectorStore& ds,
             AGDDVolumeStore& vs,
             AGDDSectionStore& ss);
             
        virtual void CreateVolume (AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;

private:

};

#endif
