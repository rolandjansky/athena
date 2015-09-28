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

#include "AsgTools/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TauDiscriminant/TauDiscriToolBase.h"

#include <string>

class TauDiscriminantProcessor;

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
        std::string m_tauInputContainerName;
        ToolHandleArray<TauDiscriToolBase> m_tools;
	std::unique_ptr<TauDiscriminantProcessor> m_tauIDTool;
};
#endif // DISCRIBUILDER_TAU_H
