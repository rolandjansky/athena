/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <string>
#include <sstream>
#include <bitset>

// Framework
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"

// L1
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigT1Interfaces/RecJetEtRoI.h"
#include "TrigT1Interfaces/RecEnergyRoI.h"

// HLT
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTSequence.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/Navigation.icc"
#include "TrigNavigation/NavigationCore.icc"
#include "TrigNavigation/TriggerElement.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigSteering/TrigSteer.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

// Local
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigCostMonitor/TrigNtElemTool.h"

//---------------------------------------------------------------------------------------
Trig::TrigNtElemTool::TrigNtElemTool(const std::string &name,
             const std::string &type,
             const IInterface  *parent)
  :AthAlgTool(name, type, parent),
   m_parentAlg(0),
   m_navig(0),
   m_clidSvc(0),
   m_elemTools(this),
   m_Config(0)
{
  declareInterface<Trig::ITrigNtTool>(this);
  declareInterface<Trig::TrigNtElemTool>(this);

  declareProperty("saveNavigation", m_saveNavigation = false, "save navigation");
  declareProperty("collectRoIData", m_collectRoIData = true,  "save RoI data");
  declareProperty("collectClid",    m_collectClid    = false, "collect attached feature clids");
  declareProperty("printClid",      m_printClid      = false, "print attached feature clids");
  declareProperty("printDebug",     m_printDebug     = false, "print debug info");
  declareProperty("filterTE",       m_filterTE       = false, "filter trigger elements");
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtElemTool::initialize()
{    
  // Get message service and print out properties
  


  // Get class id service
  if(m_printClid) { 
    if(service("ClassIDSvc", m_clidSvc).isFailure() || !m_clidSvc) {    
      ATH_MSG_WARNING("Could not retrieve ClidSvc" );
    }
  }
 
  CHECK(m_elemTools.retrieve());
  ATH_MSG_DEBUG("Retrieved " << m_elemTools );
 
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtElemTool::finalize()
{
  //
  // Clean up
  //  
  ATH_MSG_DEBUG("finalize()" );

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtElemTool::SetSteer(const HLT::TrigSteer *ptr)
{
  if(ptr) { 
    m_parentAlg = ptr; 
  } else {
    ATH_MSG_WARNING("Null HLT::TrigSteer pointer" );
  }
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtElemTool::Fill(TrigMonConfig *confg)
{
  //
  // Copy configuration
  //
  m_Config = 0;

  if(confg) {
    m_Config = confg;
  } else {
    ATH_MSG_WARNING("Null TrigMonConfig pointer" );
    return false;
  }
  
  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtElemTool::Fill(TrigMonEvent &event)
{
  //
  // Collect RoIs and TriggerElements 
  //
  
  // Reset current state
  m_Map.clear(); 
  m_Roi.clear();

  if(!m_parentAlg || !(m_parentAlg->getAlgoConfig())) {
    ATH_MSG_WARNING("Null AlgoConfig pointer!" );
    return false;
  }

  // Get current navigation pointer
  m_navig = m_parentAlg -> getAlgoConfig() -> getNavigation();
  if(!m_navig) {
    ATH_MSG_WARNING("Failed to get HLT::Navigation pointer" );
    return false;
  } 

  //TimM - change strategy, collect all 
  ATH_MSG_DEBUG("Collecting RoI Data" );
  FillAllRoI( m_navig->getInitialNode() );
  
  //
  // Make TrigMonTE for initial TriggerElement
  //
  TrigMonTE &elem_init = MakeElem(m_navig -> getInitialNode());
  std::vector<HLT::TriggerElement *> init_vec(1, m_navig -> getInitialNode());

  //
  // Recursively collect TriggerElements
  //
  CollectTEs(elem_init, init_vec);  

  //
  // Add TrigMonTEs
  //
  for(TEMap::iterator pit = m_Map.begin(); pit != m_Map.end(); ++pit) {
    TrigMonTE &elem = pit->second;
    
    if(m_saveNavigation && PassFilter(elem)) { 
      event.add<TrigMonTE>(elem);
    }
  }

  ATH_MSG_DEBUG("Exporting RoIs, Size: " << m_Roi.size());
  for(RoiMap::iterator rit = m_Roi.begin(); rit != m_Roi.end(); ++rit) {
    RoiData &data = rit->second;
    if(data.roiValid && data.roiMon.getRoiType() != TrigMonRoi::kNone) {
      event.add<TrigMonRoi>(data.roiMon);
    }
  }

  ATH_MSG_DEBUG("# of TEs " << m_Map.size() );

  return true;
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtElemTool::CollectTEs(TrigMonTE &elem_parent,
              const std::vector<HLT::TriggerElement *> &te_vec)
{
  //
  // Recursive function to collect TEs below L1TH level
  //  
  
  for(unsigned int i = 0; i < te_vec.size(); ++i) {
    HLT::TriggerElement *hlt_te = te_vec[i];
    if(!hlt_te) {
      ATH_MSG_DEBUG("Null TE pointer" );
      continue;
    }
    
    //
    // Make new TrigMonTE
    //
    TrigMonTE &elem_child = MakeElem(hlt_te);
    
    //
    // Add links
    //
    elem_parent.addChildIndex(elem_child.getIndex());
    elem_child.addParentIndex(elem_parent.getIndex());  
    
    //
    // Recursive call beyond ROI trigger elements
    //
    if(m_saveNavigation || m_navig -> isInitialNode(hlt_te)) {
      CollectTEs(elem_child, hlt_te->getRelated(HLT::TriggerElement::seedsRelation));
    }
  }
}

//---------------------------------------------------------------------------------------
TrigMonTE& Trig::TrigNtElemTool::MakeElem(const HLT::TriggerElement *hlt_te)
{
  //
  // Find or create TrigMonTE object
  //
  TEMap::iterator pit = m_Map.find(hlt_te);
  if(pit != m_Map.end()) {
    return pit -> second;
  }
  
  //
  // Make new TE
  //
  pit = m_Map.insert(TEMap::value_type(hlt_te,  TrigMonTE(hlt_te->getId(), m_Map.size()))).first;
  TrigMonTE &elem = pit->second;

  // Process tools
  if(!m_elemTools.empty() && m_Config) {
    // Find sequence config
    std::vector<TrigConfSeq>::const_iterator iseq = m_Config->findSeq(elem);

    if(iseq != m_Config->end<TrigConfSeq>()) {
      ToolHandleArray<IElemNtTool>::iterator itElem = m_elemTools.begin();
      for( ; itElem != m_elemTools.end(); ++itElem ) {
        if( (*itElem)->Fill(elem, *iseq, hlt_te) ) {
          ATH_MSG_WARNING("tool " << (*itElem).typeAndName() << " failed Fill()." );
        }
      }
    }
  }
  
  //
  // Collect states
  //    
  if(m_navig -> isTerminalNode(hlt_te)) elem.addState(TrigMonTE::terminalNode);
  if(m_navig -> isTopological(hlt_te))  elem.addState(TrigMonTE::topologicalTE);
  if(hlt_te -> getActiveState())        elem.addState(TrigMonTE::activeState);
  if(hlt_te -> getErrorState())         elem.addState(TrigMonTE::errorState);
  
  if(m_navig -> isInitialNode(hlt_te))  { 
    //
    // Set initial state and do nothing else
    //
    elem.setType(TrigMonTE::kINIT);  
  } else if(m_navig -> isRoINode(hlt_te)) {
    //
    // Extract ROI data
    //
    //ReadRoiId(elem, hlt_te); // TimM removed - RoI are collected differently now
    elem.setType(TrigMonTE::kROI);
  } else {
    //
    // Check if this is L1 threshold node: seeded by ROI node
    //
    const std::vector<HLT::TriggerElement *> &parent_vec = hlt_te->getRelated(HLT::TriggerElement::seededByRelation);
    
    for(unsigned int i = 0; i < parent_vec.size(); ++i) {
      const HLT::TriggerElement *parent_te = parent_vec[i];
      
      if(parent_te && m_navig -> isRoINode(parent_te)) {
        elem.setType(TrigMonTE::kL1TH);
        break;
      }
    }
    
    //
    // Check if this is TE output of some L2 or HLT chain
    //
    // TODO Support for unified HLT here
    if(IsChainOutputTE(hlt_te, 2)) elem.addState(TrigMonTE::outputL2Node);
    if(IsChainOutputTE(hlt_te, 3)) elem.addState(TrigMonTE::outputEFNode);

    //
    // Add RoiIds because we will erase links between trigger elements
    //
    // TimM - removed this for now while RoI strategy is updated
    //
    // if(m_filterTE) {
    //   const std::vector<HLT::TriggerElement *> &roi_vec = HLT::Navigation::getRoINodes(hlt_te);
    //   for(unsigned int iroi = 0; iroi < roi_vec.size(); ++iroi) {
    //     ReadRoiId(elem, roi_vec[iroi]);
    //   }
    // }
  }
  
  //
  // Colect features
  //
  if(m_collectClid) {
    const std::vector<HLT::TriggerElement::FeatureAccessHelper> &fvec = hlt_te->getFeatureAccessHelpers();
    
    for(unsigned int ifeat = 0; ifeat < fvec.size(); ++ifeat) {
      // Add feature's clid
      const HLT::TriggerElement::FeatureAccessHelper &feat = fvec[ifeat];
      elem.addClid(feat.getCLID());
      
      if(m_printClid && m_clidSvc) {
        std::string tname;
        m_clidSvc -> getTypeNameOfID(feat.getCLID(), tname).ignore();
        ATH_MSG_DEBUG("FeatureAccessHelper[" << ifeat << "] = " << tname );
      }
    }
  }

  return elem;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtElemTool::IsChainOutputTE(const HLT::TriggerElement *hlt_te, int level) const
{
  //
  // Check if this TE is output TE for any L2 or EF chain
  //
  bool matched = false;
  const std::vector<const HLT::SteeringChain *> act_vec = m_parentAlg->getActiveChains();

  for(unsigned int i = 0; i < act_vec.size(); ++i) {
    const HLT::SteeringChain *str_chain = act_vec[i];
    if(!str_chain) {
      ATH_MSG_WARNING("Null HLT::SteeringChain pointer!" );
      continue;
    }
    
    //
    // Only use passed chains
    //
    if(!(str_chain->chainPassedRaw() || str_chain->chainPassed())) continue;

    const TrigConf::HLTChain *hlt_chain = str_chain -> getConfigChain();
    if(!hlt_chain) {
      ATH_MSG_WARNING("Null TrigConf::HLTChain pointer!" );
      continue;
    }

    //
    // Iterate over configured signatures for this chain
    //
    const std::vector<TrigConf::HLTSignature*> &sig_vec = hlt_chain->signatureList();

    for(unsigned int isig = 0; isig < sig_vec.size(); ++isig) {
      const TrigConf::HLTSignature *hsig = sig_vec[isig];
      if(!hsig) continue;
     
      //
      // Iterate over configured sequences for this chain
      //
      const std::vector<TrigConf::HLTTriggerElement*> &seq_vec = hsig->outputTEs();
      for(unsigned int iseq = 0; iseq < seq_vec.size(); ++iseq) {
        const TrigConf::HLTTriggerElement *conf_te = seq_vec[iseq];
        if(!conf_te) continue;
  
        // Compare HLTTriggerElem and TriggerElement by id
        if(conf_te->id () == hlt_te->getId()) { 
          matched = true;
          break;
        }
      }
    }
  }
  
  //
  // Check chain level
  //
  if(matched) { 
    if(level == 2 && m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ) return true;
    if(level == 3 && m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::EF ) return true;
    if(level == 2 && m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::HLT) return true;
  }

  return false;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtElemTool::PassFilter(TrigMonTE &elem) const
{
  //
  // Filter TrigMonTE itself and its data
  //
  if(!m_filterTE) return true;
  
  if(elem.getType() == TrigMonTE::kINIT) { 
    return false;
  } else if(elem.getType() == TrigMonTE::kROI) {
    return false;
  } else if(elem.getType() == TrigMonTE::kL1TH) {
    return true;
  } else if(elem.getType() == TrigMonTE::kELEM) {
    //
    // Select active nodes matching active chains
    //
    // TODO Support for unified HLT here
    if(elem.getActiveState() && (elem.isOutputL2Node() || elem.isOutputEFNode())) {
      // Clean up indexes
      elem.clearIndexes();
      elem.clearClids();
      return true; 
    }
  }

  return false;
}

//---------------------------------------------------------------------------------------
void Trig::TrigNtElemTool::ReadRoiId(TrigMonTE UNUSED(&elem), const HLT::TriggerElement UNUSED(*hlt_te))
{

//   //
//   // Extract all roi data from ROI node
//   //
//   if(!m_navig || !hlt_te) {
//     return;
//   }

  
//   RoiMap::iterator rit = m_Roi.find(hlt_te);
//   if(rit == m_Roi.end()) {
//     //
//     // Attempt to read al possible roi sources
//     //
//     RoiData data;

//     m_navig -> getFeature<TrigRoiDescriptor>(hlt_te, data.roiHlt);
    
//     if(m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 || 
//       m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::HLT) {
//       //
//       // Get specialized roi
//       //
//       m_navig -> getFeature<LVL1::RecEmTauRoI> (hlt_te, data.roiEm);
//       m_navig -> getFeature<LVL1::RecJetRoI>   (hlt_te, data.roiJet);
//       m_navig -> getFeature<LVL1::RecMuonRoI>  (hlt_te, data.roiMuon);
//       m_navig -> getFeature<LVL1::RecEnergyRoI>(hlt_te, data.roiEnergy);
//       m_navig -> getFeature<LVL1::RecJetEtRoI> (hlt_te, data.roiJetEt);
//     }
    
//     //
//     // Create RoI descriptor
//     //
//     if(data.roiHlt) {
//       data.roiMon = TrigMonRoi(data.roiHlt->roiWord());
//       data.roiValid = true;

//       if(m_collectRoIData) {
//         data.roiMon.addWord(0);
//         data.roiMon.setRoiId(data.roiHlt->roiId());
//         data.roiMon.setNL1th(hlt_te->getRelated(HLT::TriggerElement::seedsRelation).size());
//         data.roiMon.setEtaPhi(data.roiHlt->eta(), data.roiHlt->phi());

//         float _etaWidth = std::fabs( data.roiHlt->etaPlus() - data.roiHlt->etaMinus() );
//         float _phiWidth = std::fabs( data.roiHlt->phiPlus() - data.roiHlt->phiMinus() ); // Wrap phi not needed here it seems
//         data.roiMon.setRoIArea( _etaWidth, _phiWidth );
  
//         if(data.roiEm) {
//           data.roiMon.setType(TrigMonRoi::kEmTau);
//           data.roiMon.addVar( TrigMonVar(kRoIET, data.roiEm->et()) );
//           data.roiMon.addVar( TrigMonVar(kRoIIsTau, (data.roiEm->roiType() == LVL1::TrigT1CaloDefs::TauRoIWordType ? 1. : 0.)) ); // If false then is electron RoI
//           data.roiMon.addVar( TrigMonVar(kRoIIsolationBits, (float) data.roiEm->isolation()) );
//           // Check this bit pattern can be saved in a float
//           if ( data.roiEm->isolation() != (unsigned int) data.roiMon.getVarVal().back()) {
//             std::bitset<32> _a( data.roiEm->isolation() );
//             std::bitset<32> _b( (unsigned int) data.roiMon.getVarVal().back() );
//             ATH_MSG_WARNING("Encoding of EMTAU RoI isolation bits in a float failed, " << _a << " != " << _b );
//           }
// ATH_MSG_INFO("TC --- > ROI EMTAU E:" << data.roiEm->et() << " eta:" << data.roiHlt->etaPlus());
//         }  
//         if(data.roiJet) {
//           data.roiMon.setType(TrigMonRoi::kJet);
//           data.roiMon.addVar( TrigMonVar(kRoIET, data.roiJet->etSmall()) );
//           data.roiMon.addVar( TrigMonVar(kRoIETLarge, data.roiJet->etLarge()) );
// ATH_MSG_INFO("TC --- > ROI JET E:" << data.roiJet->etLarge() << " eta:" << data.roiHlt->etaPlus());
//         }
//         if(data.roiMuon) {
//           data.roiMon.setType(TrigMonRoi::kMuon);
//           data.roiMon.addVar( TrigMonVar(kRoIET, data.roiMuon->getThresholdValue()) );
//           data.roiMon.addVar( TrigMonVar(kRoIMuonCharge, (float) data.roiMuon->candidateCharge()) );
// ATH_MSG_INFO("TC --- > ROI MUON E:" << data.roiMuon->getThresholdValue() << " eta:" << data.roiHlt->etaPlus());
//         }
//       }
//     } else if(data.roiEnergy) {
//       // Use 0 word (see Lvl1Converter.cxx)
//       data.roiMon = TrigMonRoi(data.roiEnergy->roiWord0());
//       data.roiValid = true;

//       if(m_collectRoIData) {
//         data.roiMon.addWord(data.roiEnergy->roiWord1());
//         data.roiMon.addWord(data.roiEnergy->roiWord2());
//         data.roiMon.setNL1th(hlt_te->getRelated(HLT::TriggerElement::seedsRelation).size()); 
//         data.roiMon.setType(TrigMonRoi::kEnergy);
//         data.roiMon.setRoiId(Trig::getRoiId_Energy());
//         data.roiMon.addVar( TrigMonVar(kRoIEnergyVectorX,   data.roiEnergy->energyX()) );
//         data.roiMon.addVar( TrigMonVar(kRoIEnergyVectorY,   data.roiEnergy->energyY()) );
//         data.roiMon.addVar( TrigMonVar(kRoIET,              data.roiEnergy->energyT()) );
//         data.roiMon.addVar( TrigMonVar(kRoIEnergyOverflowX, (float) data.roiEnergy->overflowX()) );
//         data.roiMon.addVar( TrigMonVar(kRoIEnergyOverflowY, (float) data.roiEnergy->overflowX()) );
//         data.roiMon.addVar( TrigMonVar(kRoIEnergyOverflowT, (float) data.roiEnergy->overflowT()) );
//         ATH_MSG_INFO("TC --- > ROI ET E:" << data.roiEnergy->energyT());

//       }
//     } else if(data.roiJetEt) {
//       data.roiMon = TrigMonRoi(data.roiJetEt->roiWord());
//       data.roiValid = true;

//       if(m_collectRoIData) {
//         data.roiMon.addWord(0);
//         data.roiMon.setNL1th(hlt_te->getRelated(HLT::TriggerElement::seedsRelation).size()); 
//         data.roiMon.setType(TrigMonRoi::kJetEt);
//         data.roiMon.setRoiId(Trig::getRoiId_JetEt());
//         ATH_MSG_INFO("TC --- > ROI ET-JET ");

//       }
//     } else {
//       if(m_printDebug) {
//         ATH_MSG_INFO("RoI TE without RoI descriptor: " << hlt_te->getId() );
//         std::string str;
//         m_navig->printASCIIArt(str, hlt_te);
//         std::cout << str << std::endl;

//         const std::vector<HLT::TriggerElement*> &stes = m_navig->getDirectSuccessors(hlt_te);
//         for(unsigned int i = 0; i < stes.size(); ++i) {
//           HLT::TriggerElement *te = stes[i];
//           if(!te) continue;
    
//           std::string seqname = "unknown";
//           if (m_Config) {
//             std::vector<TrigConfSeq>::const_iterator sit = m_Config->findSeq(te->getId());
//             if(sit != m_Config->end<TrigConfSeq>()) {
//               seqname = sit->getName();
//             }
//           }
    
//           ATH_MSG_INFO("Successor TE: " << seqname << "/" << te->getId() );
//         }
//       }
//     }

//     // Save this roi
//     rit = m_Roi.insert(RoiMap::value_type(hlt_te, data)).first;
//   }
    
//   if(outputLevel() <= MSG::DEBUG) {
    
//     const TrigMonRoi &roi = rit->second.roiMon;
//     float eta = -1999.0, phi = -1999.0;    
    
//     if(roi.getVarKey().size() > 1 && roi.getVarVal().size() > 1) {
//       if(roi.getVarKey()[0] == kRoIEta) eta = roi.getVarVal()[0];
//       if(roi.getVarKey()[1] == kRoIPhi) phi = roi.getVarVal()[1];
//     }

//     unsigned rword = 0;
//     if(!roi.getWord().empty()) rword = roi.getWord().front();

//     ATH_MSG_DEBUG("Filling TrigMonRoi: id, eta, phi, word: " << int(roi.getRoiId()) << ", " << eta << ", " << phi << ", " << rword );
//   }

//   //
//   // Set RoiId
//   //
//   elem.addRoiId(rit->second.roiMon.getRoiId());
}

void Trig::TrigNtElemTool::FillAllRoI(const HLT::TriggerElement* initialNode) {

  const std::vector<HLT::TriggerElement*> firstLevel = m_navig->getDirectSuccessors( initialNode );
  for (unsigned te = 0; te < firstLevel.size(); ++te) {

    const HLT::TriggerElement* hlt_te = firstLevel.at(te);
    if (m_navig -> isRoINode(hlt_te) == false) {
      continue;
    }

    RoiData data;
    m_navig -> getFeature<TrigRoiDescriptor>(hlt_te, data.roiHlt);
    m_navig -> getFeature<LVL1::RecEmTauRoI> (hlt_te, data.roiEm);
    m_navig -> getFeature<LVL1::RecJetRoI>   (hlt_te, data.roiJet);
    m_navig -> getFeature<LVL1::RecMuonRoI>  (hlt_te, data.roiMuon);
    m_navig -> getFeature<LVL1::RecEnergyRoI>(hlt_te, data.roiEnergy);
    m_navig -> getFeature<LVL1::RecJetEtRoI> (hlt_te, data.roiJetEt);

    if(data.roiHlt) {
      data.roiMon = TrigMonRoi(data.roiHlt->roiWord());
      data.roiValid = true;
      data.roiMon.addWord(0);
      data.roiMon.setRoiId(data.roiHlt->roiId());
      data.roiMon.setNL1th(hlt_te->getRelated(HLT::TriggerElement::seedsRelation).size());
      data.roiMon.setEtaPhi(data.roiHlt->eta(), data.roiHlt->phi());
      float etaWidth = std::fabs( data.roiHlt->etaPlus() - data.roiHlt->etaMinus() );
      float phiWidth = std::fabs( data.roiHlt->phiPlus() - data.roiHlt->phiMinus() ); // Wrap phi not needed here it seems
      data.roiMon.setRoIArea( etaWidth, phiWidth );

      ATH_MSG_DEBUG("--- > New HLT RoI eta:" << data.roiHlt->etaPlus() << " id:" << data.roiHlt->roiId());

      if(data.roiEm) {
        data.roiMon.setType(TrigMonRoi::kEmTau);
        data.roiMon.addVar( TrigMonVar(kRoIET, data.roiEm->et()) );
        data.roiMon.addVar( TrigMonVar(kRoIIsTau, (data.roiEm->roiType() == LVL1::TrigT1CaloDefs::TauRoIWordType ? 1. : 0.)) ); // If false then is electron RoI
        data.roiMon.addVar( TrigMonVar(kRoIIsolationBits, (float) data.roiEm->isolation()) );
        // Check this bit pattern can be saved in a float
        if ( data.roiEm->isolation() != (unsigned int) data.roiMon.getVarVal().back()) {
          std::bitset<32> a( data.roiEm->isolation() );
          std::bitset<32> b( (unsigned int) data.roiMon.getVarVal().back() );
          ATH_MSG_WARNING("Encoding of EMTAU RoI isolation bits in a float failed, " << a << " != " << b );
        }
        ATH_MSG_DEBUG("----- > RoI is EMTAU ET:" << data.roiEm->et() << " isTau:" << (int)(data.roiEm->roiType() == LVL1::TrigT1CaloDefs::TauRoIWordType) << " eta:" << data.roiHlt->etaPlus() << " id:" << data.roiHlt->roiId());
      } else if(data.roiJet) {
        data.roiMon.setType(TrigMonRoi::kJet);
        data.roiMon.addVar( TrigMonVar(kRoIET, data.roiJet->etSmall()) );
        data.roiMon.addVar( TrigMonVar(kRoIETLarge, data.roiJet->etLarge()) );
        ATH_MSG_DEBUG("----- > RoI is JET ETLarge:" << data.roiJet->etLarge() << " eta:" << data.roiHlt->etaPlus() << " id:" << data.roiHlt->roiId());
      } else if(data.roiMuon) {
        data.roiMon.setType(TrigMonRoi::kMuon);
        data.roiMon.addVar( TrigMonVar(kRoIET, data.roiMuon->getThresholdValue()) );
        data.roiMon.addVar( TrigMonVar(kRoIMuonCharge, (float) data.roiMuon->candidateCharge()) );
        ATH_MSG_DEBUG("----- > RoI is MUON ET:" << data.roiMuon->getThresholdValue() << " eta:" << data.roiHlt->etaPlus() << " id:" << data.roiHlt->roiId());
      } else {
        data.roiValid = false;
        ATH_MSG_DEBUG("Got an UNKNOWN HLT RoI, not from a L1 source");
      }
    } else if(data.roiEnergy) {
      // Use 0 word (see Lvl1Converter.cxx)
      data.roiMon = TrigMonRoi(data.roiEnergy->roiWord0());
      data.roiValid = true;
      data.roiMon.addWord(0);
      data.roiMon.setType(TrigMonRoi::kEnergy);
      // data.roiMon.addWord(data.roiEnergy->roiWord1());
      // data.roiMon.addWord(data.roiEnergy->roiWord2());
      data.roiMon.setNL1th(hlt_te->getRelated(HLT::TriggerElement::seedsRelation).size()); 
      data.roiMon.setRoiId(Trig::getRoiId_Energy());
      data.roiMon.addVar( TrigMonVar(kRoIEnergyVectorX,   data.roiEnergy->energyX()) );
      data.roiMon.addVar( TrigMonVar(kRoIEnergyVectorY,   data.roiEnergy->energyY()) );
      data.roiMon.addVar( TrigMonVar(kRoIET,              data.roiEnergy->energyT()) );
      data.roiMon.addVar( TrigMonVar(kRoIEnergyOverflowX, (float) data.roiEnergy->overflowX()) );
      data.roiMon.addVar( TrigMonVar(kRoIEnergyOverflowY, (float) data.roiEnergy->overflowX()) );
      data.roiMon.addVar( TrigMonVar(kRoIEnergyOverflowT, (float) data.roiEnergy->overflowT()) );
      ATH_MSG_DEBUG("--- > New Total Energy RoI ET:" << data.roiEnergy->energyT());
    } else if(data.roiJetEt) {
      data.roiMon = TrigMonRoi(data.roiJetEt->roiWord());
      data.roiValid = true;
      data.roiMon.addWord(0);
      data.roiMon.setNL1th(hlt_te->getRelated(HLT::TriggerElement::seedsRelation).size()); 
      data.roiMon.setType(TrigMonRoi::kJetEt);
      data.roiMon.setRoiId(Trig::getRoiId_JetEt());
      ATH_MSG_WARNING("--- > New JetET RoI. These should not be used any more, how did we find one?");
    } else {
      ATH_MSG_DEBUG("RoI TE without RoI descriptor id:" << hlt_te->getId() );
    }

    if (data.roiValid == true) {
      m_Roi.insert(RoiMap::value_type(hlt_te, data));//.first;
    }
  }

}
