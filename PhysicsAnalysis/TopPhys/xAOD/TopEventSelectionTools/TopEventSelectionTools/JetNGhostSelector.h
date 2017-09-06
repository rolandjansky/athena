/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: JetNGhostSelector.h
// Description:
// Author: Fabian Wilk
// Created: Fri May  8 15:38:21 2015

#ifndef _JETNGHOSTSELECTOR_H_
#define _JETNGHOSTSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

    class JetNGhostSelector : public SignValueSelector {
    public:
	explicit JetNGhostSelector(const std::string& params);

	/*!
	 * @brief Apply on reco level.
	 * There is no sensible application for a ghost tagging selector at reco
	 * level, hence return true for any input.
	 */
	bool apply(const top::Event&) const override { return true; }

	/*!
	 * @brief Apply on particle level.
	 * Will calculate the number of ghost tagged jet for the requested type.
	 * Will return true if that number is equal to order larger than the cut
	 * value, false otherwise.
	 */
	bool applyParticleLevel(const top::ParticleLevelEvent& plEvent) const override;

	std::string name() const override;
    private:
	std::string parseType( const std::string & params );

	enum Type {
	    c = static_cast<int>( 'c' ),
	    C = static_cast<int>( 'C' ),
	    b = static_cast<int>( 'b' ),
	    B = static_cast<int>( 'B' ),
	    t = static_cast<int>( 't' ),
	    W = static_cast<int>( 'W' ),
	    Z = static_cast<int>( 'Z' ),
	    H = static_cast<int>( 'H' ),
	    tau,
	    Invalid
	};
	Type m_type;

	std::string m_name;
    };


}

#endif /* _JETNGHOSTSELECTOR_H_ */
