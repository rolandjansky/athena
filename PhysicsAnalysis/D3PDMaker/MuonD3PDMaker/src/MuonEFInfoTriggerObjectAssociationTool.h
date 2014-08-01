/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonEFInfoTriggerObjectAssociationTool.h
 * @author Srivas Prasad <srivas.prasad@cern.ch>
 * @date Jan 2010
 * @brief Associate from an Analysis::Muon to matching Event Filter trigger object (TrigMuonEFInfo)
 */

#ifndef MUOND3PDMAKER_MUONEFINFOTRIGGEROBJECTASSOCIATIONTOOL_H
#define MUOND3PDMAKER_MUONEFINFOTRIGGEROBJECTASSOCIATIONTOOL_H 1


#include "TriggerD3PDMaker/TriggerObjectAssociationTool.h"
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigObjectMatching/TrigMuonEFInfoDistanceFunctor.h"
//#include Trigger/TrigEvent/TrigMuonEvent/TrigMuonEvent/TrigMuonEFInfo.h"
#include <string>



namespace D3PD {


/**
 * @brief Associate from an Analysis::Muon to matching Event Filter trigger object.
 */
  typedef TriggerObjectAssociationTool<Analysis::Muon, TrigMuonEFInfo>
  MuonEFInfoTriggerObjectAssociationToolBase;

class MuonEFInfoTriggerObjectAssociationTool
  : public MuonEFInfoTriggerObjectAssociationToolBase
{
public:
  typedef MuonEFInfoTriggerObjectAssociationToolBase Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MuonEFInfoTriggerObjectAssociationTool (const std::string& type,
					  const std::string& name,
					  const IInterface* parent);

  virtual const TrigMuonEFInfo* get(const Analysis::Muon& p);

private:
  // EFInfo track type to match to. Can be (muid) combined, mugirl, muon spectrometer, or extrapolated track
  // see Trigger/TrigAnalysis/TrigObjectMatching/TrigObjectMatching/TrigMuonEFInfoDistanceFunctor.h
  // and Trigger/TrigEvent/TrigMuonEvent/TrigMuonEvent/TrigMuonEFInfo.h
  std::string m_efTrigMatchType;
};


} // namespace D3PD



#endif
