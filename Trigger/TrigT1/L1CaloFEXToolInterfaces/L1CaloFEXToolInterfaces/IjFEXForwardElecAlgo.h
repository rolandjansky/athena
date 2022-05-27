/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//             Interface for jFEXForwardElecAlgo - Algorithm for Forward Electron Algorithm in jFEX
//                              -------------------
//     begin                : 16 11 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#ifndef IjFEXForwardElecAlgo_H
#define IjFEXForwardElecAlgo_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/jFEXForwardElecInfo.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"


namespace LVL1 {

static const InterfaceID IID_IjFEXForwardElecAlgo("LVL1::IjFEXForwardElecAlgo",1, 0);

class IjFEXForwardElecAlgo : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID ( ) ;
        virtual StatusCode safetyTest() = 0;
        virtual StatusCode reset() =0;
	virtual void setup(int inputTable[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width], int, int ) =0;

	//virtual std::array<float,4> getEtaPhiEt(uint) =0;                                                                                    
        virtual std::array<float,2> getEtaPhi(uint) =0;
        virtual std::array<uint,2> getEtEmHad(uint) =0;

	virtual std::unordered_map<uint, jFEXForwardElecInfo> ClusterTTIDLists() =0;  
	virtual std::unordered_map<uint, jFEXForwardElecInfo> LocalMaxima() =0;
	virtual std::unordered_map<uint, jFEXForwardElecInfo> calculateEDM() =0;

	virtual bool isNeighbour(float aeta, int deltaNeta, float deltaPhi) =0;    

        virtual void setFPGAEnergy(std::unordered_map<int,std::vector<int> >,std::unordered_map<int,std::vector<int> > )  =0;



    private:

};
inline const InterfaceID& LVL1::IjFEXForwardElecAlgo::interfaceID()
{
    return IID_IjFEXForwardElecAlgo;
}

}
#endif

