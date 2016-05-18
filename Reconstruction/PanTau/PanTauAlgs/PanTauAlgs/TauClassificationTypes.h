/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauClassificationTypes.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PANTAUALGS_TAUCLASSIFICATIONTYPES_H
#define PANTAUALGS_TAUCLASSIFICATIONTYPES_H

namespace PanTau {
    namespace TauClassificationTypes {
        enum TruthTauType {
            TruthOneProng=0,
            TruthOneProngNeutrals=1,
            TruthThreeProng=2,
            TruthThreeProngNeutrals=3,
            TruthKaon=4,
            TruthElectron=5,
            TruthMuon=6,
            TruthFake=7,
            TruthOther=8,
            NumberOfTruthTauTypes=9
        };
        enum SeedTauType {
            SeedOneProng=0,
            SeedOneProngNeutrals=1,
            SeedThreeProng=2,
            SeedThreeProngNeutrals=3,
            SeedThreeAndOneProng=4,
            SeedThreeAndOneProngNeutrals=5,
            SeedElectron=6,
            SeedMuon=7,
            SeedOther=8,
            SeedMissing=9,
            NumberOfSeedTauTypes=10,
            TauRecBoth=0,
            TauRecBothMissing=1,
            TauRec=2,
            Tau1p3p=3,
            NumberOfTauRecTypes=4,
            UnknownSeedType = 10
        };
        
        enum PanTauRecoMode {
            Reco_1prong_0neutrals   = 0,
            Reco_1prong_1neutral    = 1,
            Reco_1prong_Xneutrals   = 2,
            Reco_3prong_0neutrals   = 3,
            Reco_3prong_Xneutrals   = 4,
            Reco_BadPt              = -2,
            Reco_Error              = -3,
            Reco_InvalidInSeed      = -4,
            Reco_nModes             = 8
        };
        
//         enum CandTauType {
//             TauRecBoth=0,
//             TauRecBothMissing=1,
//             TauRec=2,
//             Tau1p3p=3,
//             NumberOfCandTauTypes=4
//         };
        
        inline std::string getRecoModeName(PanTauRecoMode recoMode) {
            switch(recoMode) {
                case Reco_1prong_0neutrals: return "1p0n";
                case Reco_1prong_1neutral:  return "1p1n";
                case Reco_1prong_Xneutrals: return "1pXn";
                case Reco_3prong_0neutrals: return "3p0n";
                case Reco_3prong_Xneutrals: return "3pXn";
                case Reco_BadPt:            return "BadPt";
                case Reco_Error:            return "Error";
                default:                    return "Unkown";
            }
        }
    }
} // end of namespace

#endif //PANTAUALGS_TAUCLASSIFICATIONTYPES_H


