/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "tauRecTools/TauRecToolBase.h"

class TauGenericPi0Cone : public TauRecToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor
    //-------------------------------------------------------------
    TauGenericPi0Cone(const std::string& name="TauGenericPi0Cone");
    ASG_TOOL_CLASS2(TauGenericPi0Cone, TauRecToolBase, ITauToolBase)

    //-------------------------------------------------------------
    //! Destructor
    //-------------------------------------------------------------
    ~TauGenericPi0Cone();

    virtual StatusCode initialize() override;
    virtual StatusCode execute(xAOD::TauJet& pTau) override;
    virtual StatusCode finalize() override;
    virtual StatusCode eventInitialize() override { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() override { return StatusCode::SUCCESS; }

private:

};

#endif
