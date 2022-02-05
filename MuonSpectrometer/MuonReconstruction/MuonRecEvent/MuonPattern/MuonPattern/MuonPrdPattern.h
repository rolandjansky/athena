/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonPrdPattern.h
//   Header file for class MuonPrdPattern
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUON_MUONPRDPATTERN_H
#define MUON_MUONPRDPATTERN_H

#include <vector>

#include "MuonPattern/MuonPattern.h"

namespace Muon {

    /**
        @brief Class to store a pattern in the muon system containing PrepRawData pointers.
               The class inherits from MuonPattern.

    */
    class MuonPrdPattern : public MuonPattern {
    public:
        typedef std::vector<const Trk::PrepRawData*> PrdVector;

    public:
        /** Constructor */
        MuonPrdPattern(const Amg::Vector3D& pos, const Amg::Vector3D& dir);

        /** Constructor with vector of prds */
        MuonPrdPattern(const Amg::Vector3D& pos, const Amg::Vector3D& dir, const PrdVector& prds);

        /** Destructor */
        ~MuonPrdPattern();

        /** add hit to pattern */
        virtual void addPrd(const Trk::PrepRawData* prd);

        /** Number or PrepRawData contained by this Pattern */
        virtual unsigned int numberOfContainedPrds() const;

        /** returns the PrepRawData objects depending on the integer, return zero if index out of range */
        virtual const Trk::PrepRawData* prd(unsigned int index) const;

        /** clone methode */
        virtual MuonPrdPattern* clone() const;

        const PrdVector& prepRawDataVec() const;

    private:
        /** vector of PrepRawData pointers */
        PrdVector m_prds;
    };

    inline void MuonPrdPattern::addPrd(const Trk::PrepRawData* prd) {
        if (prd) m_prds.push_back(prd);
    }

    inline unsigned int MuonPrdPattern::numberOfContainedPrds() const { return m_prds.size(); }

    inline const Trk::PrepRawData* MuonPrdPattern::prd(unsigned int index) const {
        if (index < numberOfContainedPrds()) return m_prds[index];
        return 0;
    }

    inline MuonPrdPattern* MuonPrdPattern::clone() const { return new MuonPrdPattern(*this); }

    inline const std::vector<const Trk::PrepRawData*>& MuonPrdPattern::prepRawDataVec() const { return m_prds; }
}  // namespace Muon

#endif
