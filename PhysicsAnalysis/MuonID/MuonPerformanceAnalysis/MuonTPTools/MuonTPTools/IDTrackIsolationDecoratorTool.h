/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * IDTrackIsolationDecoratorTool.h
 *
 *  Created on: Sep 29, 2015
 *      Author: goblirsc
 */

#ifndef IDTRACKISOLATIONDECORATORTOOL_H_
#define IDTRACKISOLATIONDECORATORTOOL_H_


#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "MuonTPTools/IIDTrackIsolationDecoratorTool.h"

#ifndef XAOD_ANALYSIS
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"
#include "RecoToolInterfaces/IsolationCommon.h"
#endif

#include "xAODPrimitives/IsolationType.h"


class IDTrackIsolationDecoratorTool :
          public asg::AsgTool,
          virtual public IIDTrackIsolationDecoratorTool {
    ASG_TOOL_CLASS(IDTrackIsolationDecoratorTool, IIDTrackIsolationDecoratorTool)
public:
    IDTrackIsolationDecoratorTool(std::string myname);
    virtual StatusCode      initialize  ();
    StatusCode                  decorate (const xAOD::IParticle* part) const;
protected:
#ifndef XAOD_ANALYSIS
    ToolHandle<xAOD::ITrackIsolationTool>                        m_track_iso_tool;
    ToolHandle<xAOD::ICaloTopoClusterIsolationTool>     m_calo_iso_tool;
#endif
    StatusCode    recompute_and_decorate (const xAOD::IParticle* part) const;

    std::vector<xAOD::Iso::IsolationType> m_trk_isos;
    std::vector<xAOD::Iso::IsolationType> m_calo_isos;
    SG::AuxElement::Decorator< float > m_dec_ptcone40;
    SG::AuxElement::Decorator< float > m_dec_ptcone30;
    SG::AuxElement::Decorator< float > m_dec_ptvarcone40;
    SG::AuxElement::Decorator< float > m_dec_ptvarcone30;
    SG::AuxElement::Decorator< float > m_dec_topoetcone40;
    SG::AuxElement::Decorator< float > m_dec_topoetcone20;
    SG::AuxElement::Decorator< float > m_dec_etcore;
    SG::AuxElement::ConstAccessor<float> m_ptcone40_acc;
    SG::AuxElement::ConstAccessor<float> m_ptcone30_acc;
    SG::AuxElement::ConstAccessor<float> m_ptvarcone40_acc;
    SG::AuxElement::ConstAccessor<float> m_ptvarcone30_acc;
    SG::AuxElement::ConstAccessor<float> m_topoetcone40_acc;
    SG::AuxElement::ConstAccessor<float> m_topoetcone20_acc;

#ifndef XAOD_ANALYSIS
    xAOD::TrackCorrection                m_trk_corr;
    xAOD::CaloCorrection                 m_calo_corr;
#endif
};

#endif /* IDTRACKISOLATIONDECORATORTOOL_H_ */
