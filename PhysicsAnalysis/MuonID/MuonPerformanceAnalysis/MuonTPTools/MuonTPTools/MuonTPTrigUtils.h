/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPTrigUtils.h
 *
 *  Created on: Oct 9, 2015
 *      Author: goblirsc
 */

#ifndef MUONTPTOOLS_MUONTPTOOLS_MUONTPTRIGUTILS_H_
#define MUONTPTOOLS_MUONTPTOOLS_MUONTPTRIGUTILS_H_

#include "MuonTPTools/IMuonTPTrigUtils.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "AsgTools/AsgTool.h"
#include "TrigMuonMatching/ITrigMuonMatching.h"


class MuonTPTrigUtils :
          public asg::AsgTool,
          virtual public IMuonTPTrigUtils {
    ASG_TOOL_CLASS(MuonTPTrigUtils, IMuonTPTrigUtils)
public:
    MuonTPTrigUtils(std::string myname);
    virtual StatusCode      initialize  ();

    // more efficient wrappers for the TrigMuonMatching methods
    virtual bool TrigDecision ( const std::string & trig);
    virtual bool TrigDecision_RM (const std::string & trig);
    virtual float Trig_DR (const xAOD::Muon & mu, const std::string & trig, const float  dr = 10.);
    virtual bool Trig_Match (const xAOD::Muon & mu, const std::string & trig, const float  dr = 0.1);

    // methods providing extra functionality
    virtual bool Trig_Match_RM (const xAOD::Muon & mu, const std::string & trig, const float  dr = 0.1);

protected:

    // internal aux methods
    virtual bool Trig_Match_HLT (const xAOD::Muon & mu, const std::string & trig, const float  dr = 0.1);
    virtual bool Trig_Match_HLT_RM (const xAOD::Muon & mu, const std::string & trig, const float  dr = 0.1);
    virtual bool Trig_Match_L1 (const xAOD::Muon & mu, const std::string & trig, const float  dr = 0.1);

    ToolHandle<Trig::TrigDecisionTool>     m_trigTool;
    ToolHandle<Trig::ITrigMuonMatching>    m_matchTool;
};

#endif /* MUONTPTOOLS_MUONTPTOOLS_MUONTPTRIGUTILS_H_ */
