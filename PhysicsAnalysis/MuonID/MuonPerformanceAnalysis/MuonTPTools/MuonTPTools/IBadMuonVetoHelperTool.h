/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * IEventBadMuonVetoDecorator.h
 *
 *  Created on: Jun 3, 2016
 *      Author: goblirsc
 */

/// This tools obtains the values of |sigma_qoverp(X)/qoverp(X)| / |sigma_qoverp(CB)/qoverp(CB)| (X = ID, ME)
/// for the muons in the event most likely to fail the MCP BadMuon veto.
/// Can retrieve the vars for various muon WP.


#ifndef MUONTPTOOLS_MUONTPTOOLS_IBadMuonVetoHelperTool_H_
#define MUONTPTOOLS_MUONTPTOOLS_IBadMuonVetoHelperTool_H_


#include "AsgTools/IAsgTool.h"
#include "xAODMuon/Muon.h"


class IBadMuonVetoHelperTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IBadMuonVetoHelperTool)
public:
	virtual StatusCode getDiscriminatingVars (xAOD::Muon::Quality WP, float & ID_Discriminant, float & ME_Discriminant) const = 0;
	virtual StatusCode getDiscriminatingVarsHighPt (float & ID_Discriminant, float & ME_Discriminant) const = 0;
};


#endif /* MUONTPTOOLS_MUONTPTOOLS_IBadMuonVetoHelperTool_H_ */
