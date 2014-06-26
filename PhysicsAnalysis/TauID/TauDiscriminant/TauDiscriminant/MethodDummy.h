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

using namespace std;

namespace TauID
{
    class MethodDummy : public MethodBase
    {
        public:

            //!< Default constructor
            #ifdef __STANDALONE
            MethodDummy(const string& _name = "", bool _verbose = false):
                MethodBase(_name,_verbose)
            {}
            #else
            MethodDummy(const string& _name = ""):
                MethodBase(_name)
            {}
            #endif

            //!< Destructor
            ~MethodDummy()
            {}

            bool build(const string& filename, bool checkTree = false);

            float response() const;

            float response(unsigned int level) const
            {
                if (level != 0)
                {
                    print("MethodDummy does not output more than one possible response.");
                }
                return response();
            }

            Types::MethodType getType() const
            {
                return Types::DUMMY;
            }
    };
}
#endif
