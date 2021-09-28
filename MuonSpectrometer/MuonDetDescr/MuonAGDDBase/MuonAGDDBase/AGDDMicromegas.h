/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDMicromegas_H
#define AGDDMicromegas_H

#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "AGDDKernel/AGDDVolume.h"
#include <string>
#include <vector>
#include <iostream>



class AGDDMicromegas: public MMDetectorDescription, public AGDDVolume {
public:
    AGDDMicromegas(const std::string& s);

        virtual void CreateVolume (const AGDDBuilder& builder) override;
	virtual void CreateSolid (const AGDDBuilder& builder) override;

private:

};

#endif
