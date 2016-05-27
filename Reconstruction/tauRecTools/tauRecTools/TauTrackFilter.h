/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauTrackFilter.h
// package:     Reconstruction/tauRec
// authors:     Robert Clarke, Blake Burghgrave
// date:        2014-01-04
//
//
//-----------------------------------------------------------------------------

#ifndef TAUREC_TAUTRACKFILTER_H
#define	TAUREC_TAUTRACKFILTER_H

#include "tauRecTools/TauRecToolBase.h"

class TauTrackFilter : public TauRecToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor
    //-------------------------------------------------------------
    TauTrackFilter(const std::string& name="TauTrackFilter");
    ASG_TOOL_CLASS2(TauTrackFilter, TauRecToolBase, ITauToolBase)

    //-------------------------------------------------------------
    //! Destructor
    //-------------------------------------------------------------
    ~TauTrackFilter();

    virtual StatusCode initialize();
    virtual StatusCode execute(xAOD::TauJet& pTau);
    virtual StatusCode finalize();
    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

    virtual void print() const { }

private:
    std::string m_configPath;
    std::string m_trackContainerName;
    std::string m_tauTrackConName;
    std::vector<bool> m_TrkPass;
    int m_nProng;
    int m_flag;

};

#endif
