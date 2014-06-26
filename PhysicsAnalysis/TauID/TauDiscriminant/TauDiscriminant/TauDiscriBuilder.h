/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauDiscriBuilder.h
// package:     PhysicsAnalysis/TauID/TauDiscriminant
// authors:     M. Wolter, A. Kaczmarska
// date:        13 March 2008
//-----------------------------------------------------------------------------

#ifndef DISCRIBUILDER_TAU_H
#define DISCRIBUILDER_TAU_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TauDiscriminant/TauDetailsManager.h"

#include <string>

class TauDiscriToolBase;
class StoreGateSvc;

class TauDiscriBuilder: public AthAlgorithm
{
    public:
        //-----------------------------------------------------------------
        // Contructor and destructor
        //-----------------------------------------------------------------
        TauDiscriBuilder( const std::string &name, ISvcLocator *pSvcLocator );
        ~TauDiscriBuilder();

        //-----------------------------------------------------------------
        // Gaudi algorithm hooks
        //-----------------------------------------------------------------
        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();

    private:
        std::string                         tauInputContainerName;
        ToolHandleArray<TauDiscriToolBase>  tools;
        TauDetailsManager*                  manager;
};
#endif // DISCRIBUILDER_TAU_H
