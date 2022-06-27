// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef TRIGHTTBANKSVC_H
#define TRIGHTTBANKSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/Service.h"
#include "TrigHTTMaps/ITrigHTTMappingSvc.h"
#include "TrigHTTBanks/ITrigHTTBankSvc.h"
#include "TrigHTTBanks/HTTFitConstantBank.h"
#include "TrigHTTBanks/HTTSectorSlice.h"
#include "TrigHTTBanks/HTTSectorBank.h"


class TrigHTTBankSvc : public extends< AthService, ITrigHTTBankSvc >
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthService

        TrigHTTBankSvc(const std::string& name, ISvcLocator* svc);
        virtual ~TrigHTTBankSvc() = default;

        virtual StatusCode initialize() override;

        ///////////////////////////////////////////////////////////////////////
        // ITrigHTTBankSvc

        virtual const HTTFitConstantBank* FitConstantBank_1st(int missedPlane = -1) override;
        virtual const HTTFitConstantBank* FitConstantBank_2nd(int missedPlane = -1) override;
        virtual const HTTSectorBank* SectorBank_1st() override;
        virtual const HTTSectorBank* SectorBank_2nd() override;
        virtual const HTTSectorSlice* SectorSlice() override;

    private:
	// Gaudi properties

	Gaudi::Property<std::string> m_path_constants_1st {this, "constants_1st", "", "Path of the 1st stage constants"};
	Gaudi::Property<std::string> m_path_constants_2nd {this, "constants_2nd", "", "Path of the 2nd stage constants"};
	Gaudi::Property<std::string> m_path_sectorbank_1st {this, "sectorBank_1st", "", "Path of the 1st stage sector bank"};
	Gaudi::Property<std::string> m_path_sectorbank_2nd {this, "sectorBank_2nd", "", "Path of the 2nd stage sectorbank"};
	Gaudi::Property<std::string> m_path_sectorslices {this, "sectorSlices", "", "Path of the sector slices"};
	Gaudi::Property<std::string> m_bankTypes {this, "BankType", "FILE", "FILE or COOL (COOL/DB to be implemented)"};
	Gaudi::Property<int> m_ncoords_1st {this, "NCoords_1st", 9, "Number of 1st stage coordinates"};
	Gaudi::Property<int> m_ncoords_2nd {this, "NCoords_2nd", 18, "Number of 2nd stage coordinates"};
	Gaudi::Property<std::string> m_path_NNconstants {this, "m_path_NNconstants", "", "Path for NN constants"};
	Gaudi::Property<std::vector<std::string> > m_path_constants_1st_noguess {this, "constantsNoGuess_1st", {}, "Path of the 1st stage constants without guessing"};
	Gaudi::Property<std::vector<std::string> > m_path_constants_2nd_noguess {this, "constantsNoGuess_2nd", {}, "Path of the 2nd stage constants without guessing"};

        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping;


        ///////////////////////////////////////////////////////////////////////
        // Storage pointers
	std::unique_ptr <HTTFitConstantBank> m_FitConstantBank_1st = nullptr;
	std::unique_ptr <HTTFitConstantBank> m_FitConstantBank_2nd = nullptr;
        std::vector<std::unique_ptr <HTTFitConstantBank> > m_FitConstantBankNoGuess_1st;
        std::vector<std::unique_ptr <HTTFitConstantBank> > m_FitConstantBankNoGuess_2nd;
        std::unique_ptr<HTTSectorBank> m_SectorBank_1st = nullptr;
        std::unique_ptr<HTTSectorBank> m_SectorBank_2nd = nullptr;
        std::unique_ptr<HTTSectorSlice> m_SectorSlices = nullptr;

        ///////////////////////////////////////////////////////////////////////
        // Methods for lazy loading.
        // missedPlane = -1 means banks where we guess any missing hits
        bool LoadFitConstantBank_1st(int missedPlane = -1);
        bool LoadFitConstantBank_2nd(int missedPlane = -1);
        bool LoadSectorBank_1st();
        bool LoadSectorBank_2nd();
        bool LoadSectorSlice();

};

#endif   // TRIGHTTBANKSVC_H
