/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AUTHOR: Noel Dawe
#ifndef FAKETAUBITS_H
#define FAKETAUBITS_H

#include <bitset>
#include <algorithm>
#include "xAODTau/TauJet.h"
#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"

namespace TauBit
{
    enum TauBit
    {
        BDT_LOOSE_BKG,
        BDT_MEDIUM_BKG,
        BDT_TIGHT_BKG
    };
}

class FakeTauBits
{
    typedef TauBit::TauBit TauBit;
    friend class FakeTauBitsContainer;

    public:
        
        FakeTauBits(const xAOD::TauJet* tau):
            tau(tau)
        {}

        void setBit(TauBit bit, bool value)
        {
            bits[bit] = value;
        }

        bool getBit(TauBit bit) const
        {
            return bits[bit];
        }
   
    private:
        
        std::bitset<32> bits;
        const xAOD::TauJet* tau;
};

class FakeTauBitsContainer: public DataVector<FakeTauBits>
{
    public:
   
        FakeTauBitsContainer( SG::OwnershipPolicy own = SG::OWN_ELEMENTS ):
            DataVector<FakeTauBits>( own ) {}

        const FakeTauBits* getBitsAssocTo(const xAOD::TauJet* tau) const
        {
            if (!tau)
                return 0;
            FakeTauBitsContainer::const_iterator it(this->begin());
            FakeTauBitsContainer::const_iterator it_end(this->end());
            for(; it != it_end; ++it)
            {
                if (tau == (*it)->tau)
                    return *it;
            }
            return 0;
        }

        const FakeTauBits* getBitsAssocTo(const xAOD::TauJet& tau) const
        {
            FakeTauBitsContainer::const_iterator it(this->begin());
            FakeTauBitsContainer::const_iterator it_end(this->end());
            for(; it != it_end; ++it)
            {
                if (&tau == (*it)->tau)
                    return *it;
            }
            return 0;
        }
};

SG_BASE( FakeTauBitsContainer, DataVector<FakeTauBits> );

CLASS_DEF( FakeTauBitsContainer , 1316558256 , 1 )

#endif // FAKETAUBITS
