/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_JETSEEDBUILDER_H
#define	TAUREC_JETSEEDBUILDER_H

#include "tauRecTools/TauRecToolBase.h"


/**
 * @brief Class to build tau jet seed.
 * @author N.Meyer <nicom@cern.ch>
 * @author Felix Friedrich
*/

class JetSeedBuilder : public TauRecToolBase {

  public:
    
    ASG_TOOL_CLASS2( JetSeedBuilder, TauRecToolBase, ITauToolBase )

    /** @brief Constructor */ 
    JetSeedBuilder(const std::string& name);
    
    /** @brief Destructor */
    virtual ~JetSeedBuilder();

    /** @brief Initialization of this tool */
    virtual StatusCode initialize() override;
   
    /** @brief Executation of this tool */ 
    virtual StatusCode execute(xAOD::TauJet& pTau) const override;
    
    /** @brief Finalization of this tool */
    virtual StatusCode finalize() override;
};

#endif	/* JETSEEDBUILDER_H */

