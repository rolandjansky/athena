/*
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//             Interface for jFEXForwardJetsAlgo - Algorithm for forward Jets in jFEX
//                              -------------------
//     begin                : 07 06 2021
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************

#ifndef IjFEXFORWARDJETSALGO_H
#define IjFEXFORWARDJETSALGO_H

#include "GaudiKernel/IAlgTool.h"
//#include "L1CaloFEXSim/jFEXForwardJetsAlgo.h"
#include "L1CaloFEXSim/jFEXForwardJetsInfo.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"

namespace LVL1{

    static const InterfaceID IID_IjFEXForwardJetsAlgo("LVL1::IjFEXForwardJetsAlgo",1, 0);
     
    class IjFEXForwardJetsAlgo : virtual public IAlgTool{
    public:
      static const InterfaceID& interfaceID ( ) ;
      virtual StatusCode safetyTest() = 0;
      virtual StatusCode reset() =0;
      
      virtual void setup(int inputTable[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width], int, int)  = 0;    

      virtual std::array<float,3> globalEtaPhiEt(int) =0;
      virtual std::unordered_map<int, jFEXForwardJetsInfo> FcalJetsTowerIDLists() =0;
      virtual std::unordered_map<int, jFEXForwardJetsInfo> isSeedLocalMaxima() =0;
      virtual std::unordered_map<int, jFEXForwardJetsInfo> calculateJetETs() =0;
      virtual void setFPGAEnergy(std::unordered_map<int,std::vector<int> > et_map)   =0;

   private:

    }; 

    inline const InterfaceID& LVL1::IjFEXForwardJetsAlgo::interfaceID()
    {
      return IID_IjFEXForwardJetsAlgo;
    }
}
#endif

