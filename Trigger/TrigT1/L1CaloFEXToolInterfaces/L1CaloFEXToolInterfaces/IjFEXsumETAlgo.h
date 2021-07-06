/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//             Interface for jFEXsumETAlgo - Algorithm for Sum ET Algorithm in jFEX
//                              -------------------
//     begin                : 12 05 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#ifndef IjFEXsumETAlgo_H
#define IjFEXsumETAlgo_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/jFEXsumETTOB.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"

namespace LVL1 {

static const InterfaceID IID_IjFEXsumETAlgo("LVL1::IjFEXsumETAlgo",1, 0);

class IjFEXsumETAlgo : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID ( ) ;
        virtual StatusCode safetyTest() = 0;
        virtual StatusCode reset() =0;
        virtual void setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]) = 0;
        virtual void setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]) = 0;
        virtual int getTTowerET(unsigned int ) =0; 
        
        virtual void buildBarrelSumET()  =0;
        virtual void buildFWDSumET()  =0;
        virtual int getETlowerEta(uint )  =0;
        virtual int getETupperEta(uint )  =0;
        

        virtual std::unique_ptr<jFEXsumETTOB> getsumETTOBs() = 0;
        

    private:

};
inline const InterfaceID& LVL1::IjFEXsumETAlgo::interfaceID()
{
    return IID_IjFEXsumETAlgo;
}

}
#endif

