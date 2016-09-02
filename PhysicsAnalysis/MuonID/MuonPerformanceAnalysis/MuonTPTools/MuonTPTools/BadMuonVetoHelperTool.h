/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * BadMuonVetoHelperTool.h
 *
 *  Created on: Jun 3, 2016
 *      Author: goblirsc
 */

#ifndef MUONTPTOOLS_MUONTPTOOLS_BadMuonVetoHelperTool_H_
#define MUONTPTOOLS_MUONTPTOOLS_BadMuonVetoHelperTool_H_


#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "MuonTPTools/IBadMuonVetoHelperTool.h"
#include "xAODEventInfo/EventInfo.h"


class BadMuonVetoHelperTool :
          public asg::AsgTool,
          virtual public IBadMuonVetoHelperTool {
    ASG_TOOL_CLASS(BadMuonVetoHelperTool, IBadMuonVetoHelperTool)
public:
	BadMuonVetoHelperTool(std::string myname);
    virtual StatusCode initialize  ();
    virtual StatusCode apply_if_missing (const xAOD::EventInfo* info) const;
    virtual StatusCode getDiscriminatingVars (xAOD::Muon::Quality WP, float & ID_Discriminant, float & ME_Discriminant) const ;
    virtual StatusCode getDiscriminatingVarsHighPt (float & ID_Discriminant, float & ME_Discriminant) const ;

protected:
    ToolHandle<CP::IMuonSelectionTool>         	m_MST;

    std::string 								m_muonContainer;

    SG::AuxElement::Decorator<bool>				m_vetovar_applied;
    std::map<xAOD::Muon::Quality, SG::AuxElement::Decorator<std::pair<float,float>	> >		m_vetovars;
    SG::AuxElement::Decorator<std::pair<float,float> >			m_vetovars_HighPt;

};


#endif /* MUONTPTOOLS_MUONTPTOOLS_BadMuonVetoHelperTool_H_ */
