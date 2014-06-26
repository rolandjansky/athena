/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AUTHOR: Noel Dawe
#ifndef FAKETAUSCORES_H
#define FAKETAUSCORES_H

#include <algorithm>
#include "xAODTau/TauJet.h"
#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"

namespace TauScore
{
    enum TauScore
    {
        BDT_TRANS_BKG,
        BDT_TRANS_SIG,
	BDT_PI0_PRIMARY,
	BDT_PI0_SECONDARY,
        __END__
    };
}

class FakeTauScores
{
    // DR: clang32 is confused by this
    // typedef TauScore::TauScore TauScore;
    friend class FakeTauScoresContainer;

    public:
        
        FakeTauScores(const xAOD::TauJet* tau):
            scores(TauScore::__END__,0.),
            tau(tau)
        {}

	  void setScore(TauScore::TauScore score, float value)
        {
            scores[score] = value;
        }

        float getScore(TauScore::TauScore score) const
        {
            return scores[score];
        }
   
    private:
        
        std::vector<float> scores;
        const xAOD::TauJet* tau;
};

class FakeTauScoresContainer: public DataVector<FakeTauScores>
{
    public:
   
        FakeTauScoresContainer( SG::OwnershipPolicy own = SG::OWN_ELEMENTS ):
            DataVector<FakeTauScores>( own ) {}

        const FakeTauScores* getScoresAssocTo(const xAOD::TauJet* tau) const
        {
            if (!tau)
                return 0;
            FakeTauScoresContainer::const_iterator it(this->begin());
            FakeTauScoresContainer::const_iterator it_end(this->end());
            for(; it != it_end; ++it)
            {
                if (tau == (*it)->tau)
                    return *it;
            }
            return 0;
        }

        const FakeTauScores* getScoresAssocTo(const xAOD::TauJet& tau) const
        {
            FakeTauScoresContainer::const_iterator it(this->begin());
            FakeTauScoresContainer::const_iterator it_end(this->end());
            for(; it != it_end; ++it)
            {
                if (&tau == (*it)->tau)
                    return *it;
            }
            return 0;
        }
};

SG_BASE( FakeTauScoresContainer, DataVector<FakeTauScores> );

CLASS_DEF( FakeTauScoresContainer , 1249450621 , 1 )

#endif // FAKETAUSCORES
