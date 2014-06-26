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
#include "TauDiscriminant/Types.h"

#ifndef __STANDALONE
    //including the Message Stream Member
    #include "AthenaKernel/MsgStreamMember.h"
    #include "TauDiscriminant/TauDetailsManager.h"
#endif

using namespace std;

namespace TauID
{
    class MethodBase
    {
        public:

            ////////////////////////////////////////////////////////////
            /// The name is an arbitrary name set by the user and should
            /// have no influence on the behaviour of the method
            ////////////////////////////////////////////////////////////
            #ifdef __STANDALONE
            MethodBase(const string& _name = "", bool _verbose = false):
                name(_name),
                verbose(_verbose)
            {}
            #else
            MethodBase(const string& _name = ""):
                name(_name)
            {}
            #endif

            virtual ~MethodBase() {}

            ///////////////////////////////////////////////////////////
            /// Return the value of a continuous discriminant
            ///////////////////////////////////////////////////////////
            virtual float response() const =0;

            ///////////////////////////////////////////////////////////
            /// This method should only be used for cut-based
            /// methods for responses at different levels of
            /// "tightness." For continuous discriminants, this
            /// method should print a warning message and
            /// return the value of response() above.
            ///////////////////////////////////////////////////////////
            virtual float response(unsigned int level) const =0;

            string getName() const
            {
                return name;
            }

            ///////////////////////////////////////////////////////////
            /// Add a variable. You should not need to
            /// override this method.
            ///////////////////////////////////////////////////////////
            void addVariable(const string& _name, const void* value, char type = 'F')
            {
                if (!value)
                {
                    print("Variable pointer is NULL!");
                    return;
                }
                string localname = _name;
                // Convert to uppercase:
                std::transform(localname.begin(), localname.end(), localname.begin(), &upper);
                if (type == 'F')
                {
                    this->floatVariables[localname] = (const float*)value;
                }
                else if (type == 'I')
                {
                    this->intVariables[localname] = (const int*)value;
                }
                else
                {
                    print("Unsupported variable type!");
                }
            }

            #ifndef __STANDALONE
            //////////////////////////////////////////////////////////
            /// This method is used in Athena to set the
            /// variables instead of the addVariable method
            //////////////////////////////////////////////////////////
            void setDetails(const TauDetailsManager& manager)
            {
                const map<string,float*>* floatDetails = manager.getFloatDetails();
                map<string,float*>::const_iterator it1(floatDetails->begin());
                for (; it1 != floatDetails->end(); ++it1 )
                {
                    this->addVariable(it1->first,it1->second,'F');
                }
                const map<string,int*>* intDetails = manager.getIntDetails();
                map<string,int*>::const_iterator it2(intDetails->begin());
                for (; it2 != intDetails->end(); ++it2 )
                {
                    this->addVariable(it2->first,it2->second,'I');
                }
            }
            #endif

            void print(string message) const
            {
                #ifdef __STANDALONE
                if (this->verbose)
                {
                    cout << message << endl;
                }
                #else
                if (msgLvl(MSG::VERBOSE))
                {
                    msg(MSG::VERBOSE) << message << endreq;
                }
                #endif
            }

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
            virtual bool build(const string&, bool = false) =0;

            virtual Types::MethodType getType() const =0;

            #ifndef __STANDALONE
            //Declaring the Message method for further use
            MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl ; }

            //Declaring the Method providing Verbosity Level
            bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl ; }
            #endif

        private:

            static int upper(int c)
            {
                return std::toupper((unsigned char)c);
            }

            string name;

        protected:

            #ifdef __STANDALONE
            bool verbose;
            #else
            //Declaring private message stream member.
            mutable Athena::MsgStreamMember m_msg ;
            #endif

            map<string,const float*> floatVariables;
            map<string,const int*> intVariables;
    };
}
#endif
