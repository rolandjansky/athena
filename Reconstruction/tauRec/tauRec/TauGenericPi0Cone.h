/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauGenericPi0Cone.h
// package:     Reconstruction/tauRec
// authors:     Robert Clarke, Blake Burghgrave
// date:        2014-01-04
//
//
//-----------------------------------------------------------------------------

#ifndef TAUREC_TAUGENERICPI0CONE_H
#define	TAUREC_TAUGENERICPI0CONE_H

#include "tauRec/TauToolBase.h"

class TauGenericPi0Cone : public TauToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor
    //-------------------------------------------------------------
    TauGenericPi0Cone(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

    //-------------------------------------------------------------
    //! Destructor
    //-------------------------------------------------------------
    ~TauGenericPi0Cone();

    virtual StatusCode initialize();
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode finalize();

private:
    
    float m_pi0conedr;

};

#endif
