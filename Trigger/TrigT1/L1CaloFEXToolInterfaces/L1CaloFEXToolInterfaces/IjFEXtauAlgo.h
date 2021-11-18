/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//             Interface for jFEXtauAlgo - Algorithm for Tau Algorithm in jFEX
//                              -------------------
//     begin                : 18 02 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#ifndef IjFEXtauAlgo_H
#define IjFEXtauAlgo_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/jFEXtauTOB.h"
#include "L1CaloFEXSim/jTowerContainer.h"

namespace LVL1 {

static const InterfaceID IID_IjFEXtauAlgo("LVL1::IjFEXtauAlgo",1, 0);

class IjFEXtauAlgo : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID ( ) ;
        virtual StatusCode safetyTest() = 0;
        virtual void setup(int TTwindow[5][5], int seed[3][3]) = 0;
        virtual bool isSeedLocalMaxima() = 0;
        virtual void buildSeeds() = 0;
        virtual void setFirstEtRing(int First_ETring[]) =0;

        virtual int getTTowerET(unsigned int TTID )  =0; 
        virtual int getRealPhi (unsigned int TTID )  =0; 
        virtual int getRealEta (unsigned int TTID )  =0;
        virtual int getClusterEt()  =0;
        virtual int getIsLocalMaxima()  =0;
        virtual int getFirstEtRing()  =0;
        virtual void setFPGAEnergy(std::unordered_map<int,std::vector<int> > et_map)   =0;
        

        virtual std::unique_ptr<jFEXtauTOB> getTauTOBs(int, int) = 0;
        

    private:

};
inline const InterfaceID& LVL1::IjFEXtauAlgo::interfaceID()
{
    return IID_IjFEXtauAlgo;
}

}
#endif

