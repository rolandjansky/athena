/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TruthTausToSG.h
 * @author Ryan Reece <ryan.reece@cern.ch>
 * @date December, 2009
 * @brief Create collection of true hadronic taus in StoreGate
 */

#ifndef TauD3PDMaker_TruthTausToSG_h
#define TauD3PDMaker_TruthTausToSG_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TauTools/TruthHandler.h"
#include <string>

class TruthTausToSG: public AthAlgorithm
{
    public:
        TruthTausToSG(const std::string& name, ISvcLocator* pSvcLocator);

        StatusCode initialize();
        StatusCode finalize();
        StatusCode execute();

    private:
        // configurables
        std::string m_truth_container;
        std::string m_output_container;

        // other private
        TruthHandler m_truth_handler;
};

#endif // TauD3PDMaker_TruthTausToSG_h
