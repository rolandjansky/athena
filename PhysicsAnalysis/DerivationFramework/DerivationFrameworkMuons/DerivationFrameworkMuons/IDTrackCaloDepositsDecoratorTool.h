/*
 * IDTrackCaloDepositsDecoratorTool.h
 *
 *  Created on: Oct 29, 2015
 *      Author: mbellomo
 */

#ifndef IDTRACKCALODEPOSITSDECORATORTOOL_H_
#define IDTRACKCALODEPOSITSDECORATORTOOL_H_


#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "DerivationFrameworkMuons/IIDTrackCaloDepositsDecoratorTool.h"


#ifndef XAOD_ANALYSIS
#include "ICaloTrkMuIdTools/ITrackDepositInCaloTool.h"
#endif
class IDTrackCaloDepositsDecoratorTool :
          public asg::AsgTool,
          virtual public IIDTrackCaloDepositsDecoratorTool {
    ASG_TOOL_CLASS(IDTrackCaloDepositsDecoratorTool, IIDTrackCaloDepositsDecoratorTool)
public:
    IDTrackCaloDepositsDecoratorTool(std::string myname);
    virtual StatusCode      initialize  ();
    StatusCode              decorate (const xAOD::IParticle* part) const;
protected:

#ifndef XAOD_ANALYSIS
    ToolHandle<ITrackDepositInCaloTool>  m_trkDepositInCalo;
#endif
    StatusCode    recompute_and_decorate (const xAOD::IParticle* part) const;

    SG::AuxElement::Decorator< float > m_dec_EMB1_dep;
    SG::AuxElement::Decorator< float > m_dec_EMB1_eloss;
    SG::AuxElement::Decorator< float > m_dec_EMB2_dep;
    SG::AuxElement::Decorator< float > m_dec_EMB2_eloss;
    SG::AuxElement::Decorator< float > m_dec_EMB3_dep;
    SG::AuxElement::Decorator< float > m_dec_EMB3_eloss;
    SG::AuxElement::Decorator< float > m_dec_EME1_dep;
    SG::AuxElement::Decorator< float > m_dec_EME1_eloss;
    SG::AuxElement::Decorator< float > m_dec_EME2_dep;
    SG::AuxElement::Decorator< float > m_dec_EME2_eloss;
    SG::AuxElement::Decorator< float > m_dec_EME3_dep;
    SG::AuxElement::Decorator< float > m_dec_EME3_eloss;
    SG::AuxElement::Decorator< float > m_dec_TileBar0_dep;
    SG::AuxElement::Decorator< float > m_dec_TileBar0_eloss;
    SG::AuxElement::Decorator< float > m_dec_TileBar1_dep;
    SG::AuxElement::Decorator< float > m_dec_TileBar1_eloss;
    SG::AuxElement::Decorator< float > m_dec_TileBar2_dep;
    SG::AuxElement::Decorator< float > m_dec_TileBar2_eloss;
    SG::AuxElement::Decorator< float > m_dec_TileExt0_dep;
    SG::AuxElement::Decorator< float > m_dec_TileExt0_eloss;
    SG::AuxElement::Decorator< float > m_dec_TileExt1_dep;
    SG::AuxElement::Decorator< float > m_dec_TileExt1_eloss;
    SG::AuxElement::Decorator< float > m_dec_TileExt2_dep;
    SG::AuxElement::Decorator< float > m_dec_TileExt2_eloss;
    SG::AuxElement::Decorator< float > m_dec_HEC0_dep;
    SG::AuxElement::Decorator< float > m_dec_HEC0_eloss;
    SG::AuxElement::Decorator< float > m_dec_HEC1_dep;
    SG::AuxElement::Decorator< float > m_dec_HEC1_eloss;
    SG::AuxElement::Decorator< float > m_dec_HEC2_dep;
    SG::AuxElement::Decorator< float > m_dec_HEC2_eloss;
    SG::AuxElement::Decorator< float > m_dec_HEC3_dep;
    SG::AuxElement::Decorator< float > m_dec_HEC3_eloss;

};

#endif 
