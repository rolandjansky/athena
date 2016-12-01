/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * MethodDummy is used for speed benchmarking the other Methods.
 * A better approximation of a Method's processing time per classification instance
 * is after subtraction of the processing time per entry of MethodDummy to remove
 * data I/O time and other constant overhead.
 *
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef METHODDUMMY_H
#define METHODDUMMY_H

#include "TauDiscriminant/MethodBase.h"

//using namespace std;

namespace TauID
{
    class MethodDummy : public MethodBase
    {
        public:

            //!< Default constructor
            MethodDummy(const std::string& name = ""):
                MethodBase(name)
            {}

            //!< Destructor
            ~MethodDummy()
            {}

            bool build(const std::string& filename, bool checkTree = false);

            float response(xAOD::TauJet& tau);

            float response(xAOD::TauJet& tau, unsigned int level)
            {
                if (level != 0)
                {
                    ATH_MSG_VERBOSE("MethodDummy does not output more than one possible response.");
                }
                return response(tau);
            }

            Types::MethodType getType() const
            {
                return Types::DUMMY;
            }
    };
}
#endif
