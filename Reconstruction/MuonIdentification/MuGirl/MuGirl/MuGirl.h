/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_MUGIRL_H
#define MUGIRL_MUGIRL_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuGirl/IMuGirlRecoTool.h"

//using namespace MuGirlNS;

class MuGirl: public AthAlgorithm
{
public:
    MuGirl(const std::string& name, ISvcLocator* pSvcLocator);
    ~MuGirl(void);

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:

    // Tool Handles
    ToolHandle<MuGirlNS::IMuGirlRecoTool> m_pMuGirlReconstruction;       /**< The MuGirl reconstruction tool */
};

#endif // MUGIRL_MUGIRL_H
