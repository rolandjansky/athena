// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef ITRIGHTTBANKSVC_H
#define ITRIGHTTBANKSVC_H

/**
 * This file defines an interface for a bank service. The service is in charge
 * of loading all constants, including the fit coefficients, etc
 * Other classes can use this service to retrieve the banks.
 */


#include "GaudiKernel/IService.h"
#include "GaudiKernel/IInterface.h"

class HTTFitConstantBank;
class HTTSectorSlice;
class HTTSectorBank;

class ITrigHTTBankSvc : virtual public IService
{
    public:
	DeclareInterfaceID(ITrigHTTBankSvc, 1, 0);
        virtual ~ITrigHTTBankSvc() = default;

        virtual const HTTFitConstantBank* FitConstantBank_1st(int missedPlane = -1) = 0;
        virtual const HTTFitConstantBank* FitConstantBank_2nd(int missedPlane = -1) = 0;
        virtual const HTTSectorSlice* SectorSlice() = 0;
        virtual const HTTSectorBank* SectorBank_1st() = 0;
        virtual const HTTSectorBank* SectorBank_2nd() = 0;
};

#endif   // ITRIGHTTBANKSVC_H
