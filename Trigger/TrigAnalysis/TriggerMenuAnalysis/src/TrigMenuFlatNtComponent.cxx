/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigMenuFlatNtComponent.h
*/
#include "TriggerMenuAnalysis/TrigMenuFlatNtComponent.h"
#include "TriggerMenuAnalysis/TrigMenuNtupleAlg.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuAnalysis/CombLinksDef.h"

using namespace std;


RoINtComponent::RoINtComponent(const std::string& brname, 
			       ChainEntry::RoIType roi_type, 
			       RoILinksCnvTool* roitool, 
			       MsgStream* msg) : 
  m_branchName(brname), m_RoIType(roi_type), m_RoILinksCnvTool(roitool), 
  m_log(msg), 
  m_type(0), m_lastStep(0) {
}

RoINtComponent::~RoINtComponent() {
  std::map<int, vi_t*>::iterator p1;
  std::map<int, vvi_t*>::iterator p2;

  if (m_type) delete m_type;
  if (m_lastStep) delete m_lastStep;
  for (p1=m_featureIndex.begin(); p1!=m_featureIndex.end(); ++p1) {
    if (p1->second) delete p1->second;
  }
  m_featureIndex.clear();
  for (p1=m_featureStatus.begin(); p1!=m_featureStatus.end(); ++p1) {
    if (p1->second) delete p1->second;
  }
  m_featureStatus.clear();
  for (p2=m_featureIndexVec.begin(); p2!=m_featureIndexVec.end(); ++p2) {
    if (p2->second) delete p2->second;
  }
  m_featureIndex.clear();
  for (p2=m_featureStatusVec.begin(); p2!=m_featureStatusVec.end(); ++p2) {
    if (p2->second) delete p2->second;
  }
  m_featureStatusVec.clear();
}

void RoINtComponent::book(TTree* t) {
  const CombLinksDef* def = CombLinksDef::getCombLinksDef(m_RoIType);
  if (def) {
    int feature_id = 0;
    std::vector<std::string> vi_names = def->typeLabelNamesInt();
    std::vector<std::string> vvi_names = def->typeLabelNamesVectorOfInt();
    std::vector<std::string>::const_iterator p;

    (*m_log) << MSG::INFO << "Booking branches for RoIType: " << m_RoIType 
	    << endreq;
    
    m_type = new vi_t();
    m_lastStep = new vi_t();
    t->Branch( (m_branchName+"type").c_str(), &m_type);
    t->Branch( (m_branchName+"lastStep").c_str(), &m_lastStep);
    for (p=vi_names.begin(); p!=vi_names.end(); ++p) {
      feature_id = CombLinks::featureId(*p);
      m_featureIndex[feature_id] = new vi_t();
      m_featureStatus[feature_id] = new vi_t();
      t->Branch( (m_branchName+(*p)).c_str(), 
		 "std::vector<int>", 
		 &m_featureIndex[feature_id]);
      t->Branch( (m_branchName+(*p)+"Status").c_str(), 
		 "std::vector<int>", 
		 &m_featureStatus[feature_id]);
    }
    for (p=vvi_names.begin(); p!=vvi_names.end(); ++p) {
      feature_id = CombLinks::featureId(*p);
      m_featureIndexVec[feature_id] = new vvi_t();
      m_featureStatusVec[feature_id] = new vvi_t();
      t->Branch( (m_branchName+(*p)).c_str(), 
		 "std::vector<std::vector<int> >", 
		 &m_featureIndexVec[feature_id]);
      t->Branch( (m_branchName+(*p)+"Status").c_str(), 
		 "std::vector<std::vector<int> >", 
		 &m_featureStatusVec[feature_id]);
    }
  } else {
    (*m_log) << MSG::INFO 
	    << "Cannot retrieve CombLinksDef for RoI type: " << m_RoIType 
	    << endreq;
  }
}

void RoINtComponent::fill(const RoILinks& links) {
  const CombLinksDef* def = CombLinksDef::getCombLinksDef(m_RoIType);
  if (def) {
    std::vector<std::string> vi_names = def->typeLabelNamesInt();
    std::vector<std::string> vvi_names = def->typeLabelNamesVectorOfInt();
    std::vector<std::string>::const_iterator p;
    const std::vector<CombLinks>& comblinks = links.getCombLinks();
    std::vector<CombLinks>::const_iterator p_cl;
    int featureid=0;

    (*m_log) << MSG::DEBUG 
	    << "Number of CombLinks for RoI=" << m_RoIType 
	    << " is " << comblinks.size() << endreq;

    for (p_cl=comblinks.begin(); p_cl!=comblinks.end(); ++p_cl) {
      (*m_log) << MSG::DEBUG << "CombLink type: " << p_cl->RoIType() << endreq;
      if (p_cl->RoIType() != m_RoIType) continue;

      m_type->push_back(static_cast<int>(m_RoIType));
      m_lastStep->push_back(p_cl->lastStep());
      for (p=vi_names.begin(); p!=vi_names.end(); ++p) {
	featureid = CombLinks::featureId(*p);
	const FeatureIndex* fi = p_cl->index(featureid);
	int tmp_i = -1;
	int tmp_s = -1;
	if (fi) {
	  (*m_log) << MSG::DEBUG << "Feature " << (*p) << ": " << (*fi) 
		  << endreq;
	  tmp_i = fi->getIndex();
	  tmp_s = fi->getStatus();
	}
	m_featureIndex[featureid]->push_back(tmp_i);
	m_featureStatus[featureid]->push_back(tmp_s);
      }
      for (p=vvi_names.begin(); p!=vvi_names.end(); ++p) {
	featureid = CombLinks::featureId(*p);
	const std::vector<FeatureIndex>* fiv = p_cl->indexVec(featureid);
	vi_t vi;
	vi_t vs;
	if (fiv) {
	  (*m_log) << MSG::DEBUG << "Feature " << (*p) << ": " << (*fiv) 
		  << endreq;
	  for (unsigned int j=0; j<fiv->size(); ++j) {
	    vi.push_back( (*fiv)[j].getIndex());
	    vs.push_back( (*fiv)[j].getStatus());
	  }
	}
	m_featureIndexVec[featureid]->push_back(vi);
	m_featureStatusVec[featureid]->push_back(vs);
      }
    }
  }
}

void RoINtComponent::clear() {
  m_type->clear();
  m_lastStep->clear();
  std::map<int, vi_t*>::iterator p1;
  std::map<int, vvi_t*>::iterator p2;
  for (p1=m_featureIndex.begin(); p1!=m_featureIndex.end(); ++p1) {
    p1->second->clear();
  }
  for (p1=m_featureStatus.begin(); p1!=m_featureStatus.end(); ++p1) {
    p1->second->clear();
  }
  for (p2=m_featureIndexVec.begin(); p2!=m_featureIndexVec.end(); ++p2) {
    p2->second->clear();
  }
  for (p2=m_featureStatusVec.begin(); p2!=m_featureStatusVec.end(); ++p2) {
    p2->second->clear();
  }
}

TrigMenuFlatNtComponent::TrigMenuFlatNtComponent(NtupleAlgorithm* mainAlg, 
						 const NtComponent::NtComponentParameters& params) : 
  NtComponent::NtupleComponent(mainAlg, params), 
  m_trigDecisionTool(0), m_RoILinksCnvTool(0), 
  m_chainEntries(), m_RoILinks() {
  TrigMenuNtupleAlg* mymainAlg = dynamic_cast<TrigMenuNtupleAlg*>(mainAlg);
  if (mymainAlg) {
    m_trigDecisionTool = mymainAlg->trigDecisionTool();
    m_RoILinksCnvTool = mymainAlg->roILinksCnvTool();
  }
  m_chainId = 0;
  m_RoIType = 0;
  m_RoIIndex = 0;

}

TrigMenuFlatNtComponent::~TrigMenuFlatNtComponent() {
  m_chainEntries.clear();
  m_RoILinks.clear();
  if (m_chainId) delete m_chainId;
  if (m_RoIType) delete m_RoIType;
  if (m_RoIIndex) delete m_RoIIndex;
  m_RoINtComponents.clear();
}

StatusCode TrigMenuFlatNtComponent::book() {
  m_chainId = new vi_t();
  m_RoIType = new vvi_t();
  m_RoIIndex = new vvi_t();

  m_tree->Branch("Chain_Id", &m_chainId);
  m_tree->Branch("Chain_RoIType", &m_RoIType);
  m_tree->Branch("Chain_RoIIndex", &m_RoIIndex);

  (*m_msg) << MSG::INFO << "Creating branches" << endreq;
  addRoINtComponent("RoI_L2_mu_", ChainEntry::kRoIType_L2_mu);
  addRoINtComponent("RoI_EF_mu_", ChainEntry::kRoIType_EF_mu);
  addRoINtComponent("RoI_L2_TileMu_", ChainEntry::kRoIType_L2_TileMu);
  addRoINtComponent("RoI_EF_TileMu_", ChainEntry::kRoIType_EF_TileMu);

  addRoINtComponent("RoI_L2_e_", ChainEntry::kRoIType_L2_e);
  addRoINtComponent("RoI_EF_e_", ChainEntry::kRoIType_EF_e);

  addRoINtComponent("RoI_L2_tau_", ChainEntry::kRoIType_L2_tau);
  addRoINtComponent("RoI_EF_tau_", ChainEntry::kRoIType_EF_tau);

  addRoINtComponent("RoI_L2_j_", ChainEntry::kRoIType_L2_j);
  addRoINtComponent("RoI_EF_j_", ChainEntry::kRoIType_EF_j);

  (*m_msg) << MSG::INFO << "Creating branches for RoI Links" << endreq;
  std::vector<RoINtComponent>::iterator p;
  for (p=m_RoINtComponents.begin(); p!=m_RoINtComponents.end(); ++p) {
    p->book(m_tree);
  }

  (*m_msg) << MSG::INFO << "Done booking branches" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode TrigMenuFlatNtComponent::fill() {
  clear();
  m_RoILinksCnvTool->fill(m_chainEntries, m_RoILinks);

  //  int ichain=0;
  std::vector<ChainEntry>::const_iterator p;
  for (p=m_chainEntries.begin(); p!=m_chainEntries.end(); ++p) {
    //    m_chainId->push_back(ichain++);
    m_chainId->push_back(ChainEntry::chainId(p->getName()));
    const std::vector<int> types = p->getRoITypes();
    const std::vector<int> indices = p->getRoIIndices();
    m_RoIType->push_back(p->getRoITypes());
    m_RoIIndex->push_back(p->getRoIIndices());
  }

  std::vector<RoINtComponent>::iterator p2;
  for (p2=m_RoINtComponents.begin(); p2!=m_RoINtComponents.end(); ++p2) {
    p2->fill(m_RoILinks);
  }

  return StatusCode::SUCCESS;
}

void TrigMenuFlatNtComponent::clear() {
  m_chainEntries.clear();
  m_RoILinks.clear();
  m_chainId->clear();
  m_RoIType->clear();
  m_RoIIndex->clear();
  std::vector<RoINtComponent>::iterator p;
  for (p=m_RoINtComponents.begin(); p!=m_RoINtComponents.end(); ++p) {
    p->clear();
  }
}

void TrigMenuFlatNtComponent::addRoINtComponent(const std::string& brname, 
						ChainEntry::RoIType roi_type) {
  
  (*m_msg) << MSG::INFO << "Create RoINtComponent: " << brname << endreq;
  m_RoINtComponents.push_back(RoINtComponent(brname, roi_type, 
					    m_RoILinksCnvTool, m_msg));
}


