/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//             Interface for jFEXPileupAndNoise - Algorithm for Pileup and Noise in jFEX
//                              -------------------
//     begin                : 24 05 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#ifndef IjFEXPileupAndNoise_H
#define IjFEXPileupAndNoise_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"

namespace LVL1 {

static const InterfaceID IID_IjFEXPileupAndNoise("LVL1::IjFEXPileupAndNoise",1, 0);

class IjFEXPileupAndNoise : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID ( ) ;
        virtual StatusCode initialize() = 0;
        virtual StatusCode safetyTest() = 0;
        virtual StatusCode reset() = 0;

        virtual void setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]) =0;
        virtual void setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]) =0;        
        
        virtual std::unordered_map<int,std::vector<int> > GetEt_values() =0;        
        virtual std::unordered_map<int,std::vector<int> > Get_EM_Et_values() =0;        
        virtual std::unordered_map<int,std::vector<int> > Get_HAD_Et_values() =0;        
        
        virtual std::vector<float> CalculatePileup() =0;
        virtual void ApplyPileup2Jets(bool) =0;
        virtual void ApplyPileup2Met(bool)  =0;        
        virtual void ApplyNoise2Jets(bool) =0;
        virtual void ApplyNoise2Met(bool)  =0;     
        
    private:

};

inline const InterfaceID& LVL1::IjFEXPileupAndNoise::interfaceID()
{
    return IID_IjFEXPileupAndNoise;
}

}
#endif

