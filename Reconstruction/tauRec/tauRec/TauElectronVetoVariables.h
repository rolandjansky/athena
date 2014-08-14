/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAU1P3PELEVETO_H
#define TAUREC_TAU1P3PELEVETO_H

#include "tauRec/TauToolBase.h"

class IExtrapolateToCaloTool;

/**
 * @brief Calculate variables sensitive on electrons.
 * 
 *  The variables are mainly used by the electron veto in the TauDiscriminant package.
 * 
 * @author Zofia Czyczula
 */

class TauElectronVetoVariables : public TauToolBase {
public:

    TauElectronVetoVariables(const std::string& type,
            const std::string& name,
            const IInterface* parent);

    virtual ~TauElectronVetoVariables();
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode initialize();
    virtual StatusCode eventInitialize(TauCandidateData *data);
    
    bool m_doCellCorrection; //!< enable cell origin correction
    ToolHandle<IExtrapolateToCaloTool> m_trackToCalo;
};

#endif
