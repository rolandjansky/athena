/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETFROMJETSMT_H
#define TRIGEFMISSINGET_EFMISSINGETFROMJETSMT_H

/********************************************************************

NAME:     EFMissingETFromJetsMT.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET
AUTHOR:   Gabriel Gallardo
CREATED:  Feb 19, 2018

BASED ON: EFMissingETFromJets.h
AUTHORS:  Florian U. Bernlochner, Doug Schaefer, Justin Chiu


PURPOSE:  Updates TrigMissingETHelper using info from jets

 ********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODJet/JetContainer.h"
#include "TrigEFMissingET/IMissingETTool.h"

using xAOD::JetContainer;

/**
  @class EFMissingETFromJetsMT
  \brief Updates metHelper object with jets
  \author Gabriel Gallardo
  \date Feb 19, 2019
 **/

class EFMissingETFromJetsMT : public extends<AthAlgTool, IMissingETTool> 
{
  public:

    EFMissingETFromJetsMT(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

    ~EFMissingETFromJetsMT();

    virtual StatusCode initialize() override;

    /**
    This function does two things:
    1. It initializes the `met` object so that `EFMissingETFromHelper` knows that it will receive input from MetFromJets 
    2. It fills in the jet components of the `metHelper` object 
    It is meant to be called by the `EFMissingETAlgMT` class 
    **/
    virtual StatusCode update( xAOD::TrigMissingET *met,
             TrigEFMissingEtHelper *metHelper, const EventContext& ctx ) const override;


  private:
    EFMissingETFromJetsMT();

    Gaudi::Property<float> m_etacut                 {this, "EtaSeparation", 2.2 , "Cut to split into forward and central jets -- needs to be positive"};
    Gaudi::Property<float> m_forward_ptcut          {this, "CentralpTCut", 0.0 , "pT Cut for central jets"};
    Gaudi::Property<float> m_central_ptcut          {this, "ForwardpTCut", 0.0 ," pT Cut for forward jets"};
    SG::ReadHandleKey<JetContainer> m_jetsKey       {this, "JetsCollection", "jets", "Collection containing all jets" };

    // TileGap3Correction obsolete? Will remove in future MR if this is the case.
    // Commented out for the time being because the corresponding code is not thread-safe and is raising warnings. @ggallard
    // Gaudi::Property<bool> m_applyTileGap3Correction {this, "ApplyTileGap3Correction", false, "ApplyTileGap3Correction"};
};

#endif // TRIGEFMISSINGET_EFMISSINGETFROMJETSMT_H
