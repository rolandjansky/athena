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

#include "tauRec/TauToolBase.h"

class TauTrackFilter : public TauToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor
    //-------------------------------------------------------------
    TauTrackFilter(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

    //-------------------------------------------------------------
    //! Destructor
    //-------------------------------------------------------------
    ~TauTrackFilter();

    virtual StatusCode initialize();
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode finalize();

private:
    
    std::string m_trackContainerName;
    std::vector<bool> m_TrkPass;
    int m_nProng;
    int m_flag;
    //TODO some of this probably need to be public to be useful...

};

#endif
