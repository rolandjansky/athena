/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauContainer.h
// package:     Reconstruction/tauEvent
// authors:     Lukasz Janyst
// date:        2007-01-24
// description: Container for Taus
//-----------------------------------------------------------------------------

#ifndef TAU_CONTAINER_ANALYSIS_H
#define TAU_CONTAINER_ANALYSIS_H

#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "tauEvent/TauJet.h"

#include "NavFourMom/IParticleContainer.h"
// Needed for constructing the mirror container hierarchy
// This tells StoreGate that DV<Analysis::TauJet> derives from DV<IParticle>
DATAVECTOR_BASE( Analysis::TauJet, IParticle );

namespace Analysis
{
    //-------------------------------------------------------------------------
    // this must be new class for persistency reasons
    //-------------------------------------------------------------------------
    class TauJetContainer: public DataVector<TauJet>
    {
        public:
            TauJetContainer( SG::OwnershipPolicy own = SG::OWN_ELEMENTS ):
                DataVector<TauJet>( own ) {}

            //-----------------------------------------------------------------
            //! Set ROI ID and author (trigger purposes)
            //-----------------------------------------------------------------
            void setROIAuthor( const std::string& RoiAuthor )
            {
                m_ROIauthor = RoiAuthor;
            }

            //-----------------------------------------------------------------
            //! Retrieve ROI ID and author (trigger purposes)
            //-----------------------------------------------------------------
            const std::string& getROIAuthor() const
            {
                return m_ROIauthor;
            }

        private:
            std::string m_ROIauthor;
    };
}

SG_BASE( Analysis::TauJetContainer, DataVector<Analysis::TauJet> );

CLASS_DEF( Analysis::TauJetContainer, 56426922, 1 )

#endif // TAU_CONTAINER_ANALYSIS
