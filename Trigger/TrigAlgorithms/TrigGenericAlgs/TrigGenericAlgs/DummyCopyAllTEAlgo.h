/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigGenericAlgs_DummyCopyAllTEAlgo_h
#define TrigGenericAlgs_DummyCopyAllTEAlgo_h

#include <string>
#include "TrigInterfaces/AllTEAlgo.h"

namespace HLT {
  class TriggerElement;
}

/**
 * @brief Algorithm creating TE when nothing none of TEs present at the input is present
 * @warning If in the configuration the empty input is given then it will always create TE.
 * @warning This algorithm doe not create RoI descriptor. Nor it creates RoI type node.
 **/


namespace PESA
{
    class  DummyCopyAllTEAlgo : public HLT::AllTEAlgo {
        public:
            DummyCopyAllTEAlgo(const std::string& name, ISvcLocator* pSvcLocator); 
            HLT::ErrorCode hltInitialize() { return HLT::OK; } 
            HLT::ErrorCode hltFinalize()   { return HLT::OK; }

            HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
                    unsigned int output);

        private:

    };
}
#endif
