/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//             Interface for jFEXLargeRJetAlgo - Algorithm for small R jet Algorithm in jFEX
//                              -------------------
//     begin                : 21 01 2021
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************

#ifndef IJFEXLARGERJETALGO_H
#define IJFEXLARGERJETALGO_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/jFEXLargeRJetTOB.h"
#include "L1CaloFEXSim/jTowerContainer.h"

namespace LVL1{

    static const InterfaceID IID_IjFEXLargeRJetAlgo("LVL1::IjFEXLargeRJetAlgo",1, 0);
     
    class IjFEXLargeRJetAlgo : virtual public IAlgTool{
    public:
      static const InterfaceID& interfaceID ( ) ;
      virtual StatusCode safetyTest() = 0;
      virtual void setupCluster(int inputTable[15][15]) = 0;
      virtual unsigned int getRingET() = 0;
      virtual unsigned int getLargeClusterET(unsigned int smallClusterET, unsigned int largeRingET) = 0;
      virtual std::unique_ptr<jFEXLargeRJetTOB> getLargeRJetTOBs() = 0;

   private:

    }; 
    inline const InterfaceID& LVL1::IjFEXLargeRJetAlgo::interfaceID()
    {
      return IID_IjFEXLargeRJetAlgo;
    }

}

#endif


