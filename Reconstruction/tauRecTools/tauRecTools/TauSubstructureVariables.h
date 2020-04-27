/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUSUBSTRUCTUREBUILDER_H
#define TAUREC_TAUSUBSTRUCTUREBUILDER_H

#include "tauRecTools/TauRecToolBase.h"

#include "GaudiKernel/SystemOfUnits.h"

/**
 * @brief Calculate variables from the tau substructure.
 * 
 * @author M. Trottier-McDonald
 * @author Felix Friedrich
 * 
 */

class TauSubstructureVariables : public TauRecToolBase
{
    public: 
	    ASG_TOOL_CLASS2(TauSubstructureVariables, TauRecToolBase, ITauToolBase)
        
        static const double DEFAULT;

        TauSubstructureVariables(const std::string& name="TauSubstructureVariables");

        ~TauSubstructureVariables();

        virtual StatusCode execute(xAOD::TauJet& pTau) override;
        virtual StatusCode initialize() override;
        virtual StatusCode finalize() override;

    private:
        /** Maximal pile up correction in GeV for a tau candidate.
         *  Used for the caloIso corrected variable.
         */
        Gaudi::Property<double> m_maxPileUpCorrection {this, "maxPileUpCorrection", 4 * Gaudi::Units::GeV }; 
        Gaudi::Property<double> m_pileUpAlpha {this, "pileUpAlpha", 1.0};         //!< slope of the pileup correction
        
        /** 
         * enable cell origin correction 
         * eta and phi of the cells are corrected wrt to the origin of the tau vertex
         */
        Gaudi::Property<bool> m_doVertexCorrection {this, "VertexCorrection", false};
};

#endif
