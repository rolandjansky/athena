/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//             Interface for jFEXmetAlgo - Algorithm for MET Algorithm in jFEX
//                              -------------------
//     begin                : 14 05 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#ifndef IjFEXmetAlgo_H
#define IjFEXmetAlgo_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/jFEXmetTOB.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"

namespace LVL1 {

static const InterfaceID IID_IjFEXmetAlgo("LVL1::IjFEXmetAlgo",1, 0);

class IjFEXmetAlgo : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID ( ) ;
        virtual StatusCode safetyTest() = 0;
        virtual StatusCode reset() =0;
        virtual void setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]) = 0;
        virtual void setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]) = 0;
        
        virtual void buildBarrelmet()  =0;
        virtual void buildFWDmet()  =0;
        virtual int GetMetXComponent()  =0;
        virtual int GetMetYComponent()  =0;
        virtual int getTTowerET(unsigned int TTID ) =0; 
        virtual void setFPGAEnergy(std::map<int,std::vector<int> > et_map)   =0;
        

        virtual std::unique_ptr<jFEXmetTOB> getmetTOBs() = 0;
        

    private:

};
inline const InterfaceID& LVL1::IjFEXmetAlgo::interfaceID()
{
    return IID_IjFEXmetAlgo;
}

}
#endif

