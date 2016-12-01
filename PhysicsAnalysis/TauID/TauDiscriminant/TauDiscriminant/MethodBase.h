/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Author: Noel Dawe (Noel%dot%Dawe%at%cern%dot%ch)
 */

#ifndef METHODBASE_H
#define METHODBASE_H

#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include <deque>
#include "TauDiscriminant/Types.h"

#include "AsgTools/AsgMessaging.h"

#include "xAODTau/TauJet.h"

//using namespace std;

namespace TauID
{
    class MethodBase : public asg::AsgMessaging
    {
        public:

            ////////////////////////////////////////////////////////////
            /// The name is an arbitrary name set by the user and should
            /// have no influence on the behaviour of the method
            ////////////////////////////////////////////////////////////
            MethodBase(const std::string& name = ""):
                AsgMessaging(name),
                m_name(name)
            {}

            virtual ~MethodBase() {}

            ///////////////////////////////////////////////////////////
            /// Return the value of a continuous discriminant
            ///////////////////////////////////////////////////////////
            virtual float response(xAOD::TauJet& tau) =0;

            ///////////////////////////////////////////////////////////
            /// This method should only be used for cut-based
            /// methods for responses at different levels of
            /// "tightness." For continuous discriminants, this
            /// method should print a warning message and
            /// return the value of response() above.
            ///////////////////////////////////////////////////////////
            virtual float response(xAOD::TauJet& tau, unsigned int level) =0;

            std::string getName() const
            {
                return m_name;
            }

	    void registerVariable(const std::string& name, char type);

	    void registerVariables(std::vector<std::string>& names, char type);

	    bool updateVariables(xAOD::TauJet& tau);

	    const std::map<std::string, const float*>* getFloatPointers();

	    const std::map<std::string, const int*>* getIntPointers();

            ////////////////////////////////////////////////////////////
            /// Build the discriminant from an input file
            /// The first parameter is a filename. Your method
            /// should be saved in only one file.
            /// Specifying a list of files here separated
            /// by commas, for example, is not acceptable.
            /// The boolean parameter is optional and may be
            /// used to optionally validate your method after
            /// building from your input file.
            ////////////////////////////////////////////////////////////
            virtual bool build(const std::string&, bool = false) =0;

            virtual Types::MethodType getType() const =0;

        private:

            static int upper(int c)
            {
                return std::toupper((unsigned char)c);
            }

            std::string m_name;

            std::deque<float> m_floatVariableStore;
            std::deque<int> m_intVariableStore;

        protected:

            std::map<std::string, const float*> m_floatVars;
            std::map<std::string, const int*> m_intVars;
	    std::map<std::string, SG::AuxElement::ConstAccessor<float>> m_floatAccessors;
	    std::map<std::string, SG::AuxElement::ConstAccessor<int>> m_intAccessors;
    };
}
#endif
