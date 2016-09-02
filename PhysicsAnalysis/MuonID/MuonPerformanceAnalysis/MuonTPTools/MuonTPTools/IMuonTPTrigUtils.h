/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPTrigUtils.h
 *
 *  Created on: Oct 9, 2015
 *      Author: goblirsc
 */

#ifndef MUONTPTOOLS_MUONTPTOOLS_IMUONTPTRIGUTILS_H_
#define MUONTPTOOLS_MUONTPTOOLS_IMUONTPTRIGUTILS_H_

#include "AsgTools/IAsgTool.h"
#include "xAODMuon/Muon.h"


class IMuonTPTrigUtils : virtual public asg::IAsgTool  {
    ASG_TOOL_INTERFACE(IMuonTPTrigUtils)
public:

    // more efficient wrappers for the TrigMuonMatching methods
    virtual bool TrigDecision (const std::string & trig) = 0;
    virtual bool TrigDecision_RM (const std::string & trig) = 0;
    virtual float Trig_DR (const xAOD::Muon & mu, const std::string & trig, const float  dr = 10.) = 0;
    virtual bool Trig_Match (const xAOD::Muon & mu, const std::string & trig, const float  dr = 0.1) = 0;

    // methods providing extra functionality
    virtual bool Trig_Match_RM (const xAOD::Muon & mu, const std::string & trig, const float  dr = 0.1) = 0;
};



#endif /* MUONTPTOOLS_MUONTPTOOLS_IMUONTPTRIGUTILS_H_ */
