/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUBUILDER_H
#define TAUREC_TAUBUILDER_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "tauRec/TauToolBase.h"

/**
 * @brief       Main class for tau candidate building and processing.
 * 
 *  This class loop over seeds from the seed container, 
 *  creates a new tau candidate if seed is passing the given cuts,
 *  and run the given tau tools on the created tau candidate.
 *  If a tool fails the tau candidate will be removed.
 * 
 * @authors     Srini Rajagopalan, Michael Heldmann, Lukasz Janyst, Anna Kaczmarska, Felix Friedrich
 */

class TauBuilder : public AthAlgorithm
{
  public:
    //-----------------------------------------------------------------
    // Constructor and destructor
    //-----------------------------------------------------------------
    TauBuilder(const std::string &name, ISvcLocator *pSvcLocator);
    ~TauBuilder();

    //-----------------------------------------------------------------
    // Gaudi algorithm hooks
    //-----------------------------------------------------------------
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

  private:
    std::string m_tauContainerName;             //!< tau output container
    std::string m_tauAuxContainerName;             //!< tau output aux store container
    std::string m_seedContainerName;            //!< seed input container
    double m_maxEta; //!< only build taus with eta_seed < m_maxeta
    double m_minPt;  //!< only build taus with pt_seed > m_minpt
    
    /** switch to create tau containers, 
     * if false tau containers will be updated only 
     */ 
    bool m_doCreateTauContainers;               
    
    ToolHandleArray<TauToolBase> m_tools;  //!< tools to process tau candidates
} ;

#endif // TAUREC_TAUBUILDER_H
