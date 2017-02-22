/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMUONTPVERTEXHELPER_H
#define IMUONTPVERTEXHELPER_H

#include "AsgTools/IAsgTool.h"
#include "MuonPerformanceHistUtils/Probe.h"
#include "xAODBPhys/BPhysHelper.h"

// Small helpet tool to handle all work involved in extracting vertex association info
// for JPsi TP pairs. 
// Please call checkVertexMatch first, it will find the correct vertex for our probe

class IMuonTPVertexHelper : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMuonTPVertexHelper)
public:
    // find the vertex (if any) corresponding to our TP pair. 
    // return false if no vertex found
    // otherwise, return true and call probe.setVertex(..)
    virtual bool checkVertexMatch (Probe& probe) = 0;
    // get vertex displacement in mm. 
    // if no vertex assigned to the probe or if something goes wrong, return -1e9
    // pv_t determines to which PV the displacement is calculated, see https://svnweb.cern.ch/cern/wsvn/atlasoff/Event/xAOD/xAODBPhys/trunk/xAODBPhys/BPhysHelper.h
    virtual float getVertexDisplacement (Probe & probe, xAOD::BPhysHelper::pv_type pv_t = xAOD::BPhysHelper::PV_MIN_A0) = 0;
    // get vertex time of flight in ps. 
    // if no vertex assigned to the probe or if something goes wrong, return -1e=9
    // pv_t determines to which PV the pseudo-proper time tau is calculated, see https://svnweb.cern.ch/cern/wsvn/atlasoff/Event/xAOD/xAODBPhys/trunk/xAODBPhys/BPhysHypoHelper.h
    virtual float getVertexTau (Probe & probe, xAOD::BPhysHelper::pv_type pv_t = xAOD::BPhysHelper::PV_MIN_A0) = 0;
    // get chi2 of vertex fit 
    virtual float getChi2(Probe & probe) = 0;
    // get dof of vertex fit 
    virtual int getDof(Probe& probe) = 0;
    // get invariant mass of refittet tracks
    virtual float getInvMass(Probe& probe) = 0;
};


#endif // IMUONTPVERTEXHELPER_H
