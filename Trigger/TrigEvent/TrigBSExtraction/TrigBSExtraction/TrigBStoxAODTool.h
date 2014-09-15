/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGBSEXTRACTION_TRIGBSTOXAODTOOL_H
#define TRIGBSEXTRACTION_TRIGBSTOXAODTOOL_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigBSExtraction/ITrigBStoxAODTool.h"
#include "TrigSerializeCnvSvc/ITrigSerConvHelper.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODJetCnv/IJetCnvTool.h"
#include "xAODTrigMissingETCnv/ITrigMissingETCnvTool.h"
#include "xAODTrigMuonCnv/ITrigMuonEFInfoToMuonCnvTool.h"
// #include "xAODCaloEventCnv/ICaloClusterCnvTool.h"
// #include "xAODTrigEgammaCnv/ITrigPhotonCnvTool.h"

#include "TrigNavigation/Navigation.h"

/**
 * @brief Tool used by TrigBSExtraction to convert to xAOD
 */

namespace BStoXAODHelper{
  struct IHelper;
}

class TrigBStoxAODTool : public AthAlgTool, public virtual ITrigBStoxAODTool {
public:
  TrigBStoxAODTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TrigBStoxAODTool();   
  virtual StatusCode initialize();
  StatusCode convert(); 
  StatusCode rewireNavigation(HLT::Navigation* nav);

 private:  
  
  StatusCode classLabel2CLIDLabel(const std::vector<std::string>& property,
			      std::vector<std::pair<CLID,
			      std::string> >& decoded );
  
  std::string m_l2ResultKey;  //!< key of HLTResult for L2
  std::string m_efResultKey;  //!< key of HLTResult for EF
  std::string m_hltResultKey;  //!< key of HLTResult for HLT
  std::vector<std::string> m_containersToConvert;

  std::vector<std::pair<CLID,std::string> > m_clid_labels;

  //ToolHandle<xAODMaker::ITrigPhotonCnvTool> m_trigphotontool;
  ToolHandle<xAODMaker::ITrigMissingETCnvTool> m_trigmettool;
  ToolHandle<ITrigMuonEFInfoToMuonCnvTool> m_trigmuontool;
  ToolHandle<xAODMaker::IJetCnvTool> m_jettool;


  std::map<CLID,BStoXAODHelper::IHelper*> m_helpers; //collection clid -> helper
};

#endif // TRIGBSEXTRACTION_TRIGBSTOXAODTOOL_H
