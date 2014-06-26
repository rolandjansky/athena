/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*!
 * file: TauDetailsManager.h
 *
 * This class provides a handle on the features used for tau/jet separation.
 * 
 * To add new details: first add the enum entry in TauDetails.h and then 
 * implement the code in TauDetailsManager.cxx which sets the value for each tau/event
 *
 * Author: Noel Dawe (Noel-dot-Dawe-AT-cern-dot-ch)
 */

#ifndef TAUDETAILSMANAGER_H
#define TAUDETAILSMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>

#include "TauDiscriminant/TauDetails.h"
#include "xAODTau/TauJet.h"
// #include "tauEvent/TauCommonDetails.h"
#include "xAODTau/TauDefs.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/MsgStreamMember.h"

using namespace std;

class TauDetailsManager
{
    public:

        static const float LOW_NUMBER;

        //!< Default constructor
        TauDetailsManager(StoreGateSvc* =0, bool isTrigger=false);

        //!< Destructor
        ~TauDetailsManager() {}

        const map<string,float*>* getFloatDetails() const { return &this->float_details; }

        const map<string,int*>* getIntDetails() const { return &this->int_details; }

        const float* getFloatDetailAddress(Details::FloatTauDetail) const;

        const int* getIntDetailAddress(Details::IntTauDetail) const;

        float getFloatDetailValue(Details::FloatTauDetail) const;

        int getIntDetailValue(Details::IntTauDetail) const;
        
        const float* getFloatDetailAddress(Details::FloatEventDetail) const;

        const int* getIntDetailAddress(Details::IntEventDetail) const;

        float getFloatDetailValue(Details::FloatEventDetail) const;

        int getIntDetailValue(Details::IntEventDetail) const;

		bool getDoTrigger() const { return doTrigger; }

        bool updateEvent();
        
        bool update(const xAOD::TauJet& tauJet); //keep for backward compatibility
        bool update_with_edm(xAOD::TauJet& tauJet);

	bool setNpi0(xAOD::TauJet& tauJet, int nPi0);

        friend MsgStream& operator<<(MsgStream&, const TauDetailsManager&);

        friend ostream& operator<<(ostream&, const TauDetailsManager&);

        //Declaring the Message method for further use
        MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
        
        //Declaring the Method providing Verbosity Level
        bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
    
    protected:

        template <class stream>
        void printOn(stream& o) const;
        
    private:

        map<string,float*> float_details;
        map<string,int*> int_details;
        vector<float> float_data;
        vector<int> int_data;
        vector<float> float_event_data;
        vector<int> int_event_data;
        StoreGateSvc* storeGate;
	bool doTrigger;
        mutable Athena::MsgStreamMember m_msg;

	float m_clusterCone;
};

template <class stream>
void TauDetailsManager::printOn(stream& o) const
{
    ios_base::fmtflags original_flags = (ios_base::fmtflags)o.flags();
    o << "\n\n";
    map<string,float*>::const_iterator it_float(this->float_details.begin());
    map<string,float*>::const_iterator it_float_end(this->float_details.end());
    const float* float_value(0);
    for(; it_float != it_float_end; ++it_float)
    {
        o << left << setw(40) << setfill('.') << (it_float->first);
        float_value = it_float->second;
        if (float_value)
            o << " " << *float_value << "\n";
        else
            o << " NULL\n";
    }
    
    map<string,int*>::const_iterator it_int(this->int_details.begin());
    map<string,int*>::const_iterator it_int_end(this->int_details.end());
    const int* int_value(0);
    for(; it_int != it_int_end; ++it_int)
    {
        o << left << setw(40) << setfill('.') << (it_int->first);
        int_value = it_int->second;
        if (int_value)
            o << " " << *int_value << "\n";
        else
            o << " NULL\n";
    }
    o.flags(original_flags);
}

inline MsgStream& operator<<(MsgStream& o, const TauDetailsManager& manager)
{
    manager.printOn<MsgStream>(o);
    return o;
}

inline ostream& operator<<(ostream& o, const TauDetailsManager& manager)
{
    manager.printOn<ostream>(o);
    return o;
}

#endif
