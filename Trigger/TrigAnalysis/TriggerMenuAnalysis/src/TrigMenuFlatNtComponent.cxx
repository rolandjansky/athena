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
  mBranchName(brname), mRoIType(roi_type), mRoILinksCnvTool(roitool), 
  mLog(msg), 
  mType(0), mLastStep(0) {
}

RoINtComponent::~RoINtComponent() {
  std::map<int, vi_t*>::iterator p1;
  std::map<int, vvi_t*>::iterator p2;

  if (mType) delete mType;
  if (mLastStep) delete mLastStep;
  for (p1=mFeatureIndex.begin(); p1!=mFeatureIndex.end(); ++p1) {
    if (p1->second) delete p1->second;
  }
  mFeatureIndex.clear();
  for (p1=mFeatureStatus.begin(); p1!=mFeatureStatus.end(); ++p1) {
    if (p1->second) delete p1->second;
  }
  mFeatureStatus.clear();
  for (p2=mFeatureIndexVec.begin(); p2!=mFeatureIndexVec.end(); ++p2) {
    if (p2->second) delete p2->second;
  }
  mFeatureIndex.clear();
  for (p2=mFeatureStatusVec.begin(); p2!=mFeatureStatusVec.end(); ++p2) {
    if (p2->second) delete p2->second;
  }
  mFeatureStatusVec.clear();
}

void RoINtComponent::book(TTree* t) {
  const CombLinksDef* def = CombLinksDef::getCombLinksDef(mRoIType);
  if (def) {
    int feature_id = 0;
    std::vector<std::string> vi_names = def->typeLabelNamesInt();
    std::vector<std::string> vvi_names = def->typeLabelNamesVectorOfInt();
    std::vector<std::string>::const_iterator p;

    (*mLog) << MSG::INFO << "Booking branches for RoIType: " << mRoIType 
	    << endreq;
    
    mType = new vi_t();
    mLastStep = new vi_t();
    t->Branch( (mBranchName+"type").c_str(), &mType);
    t->Branch( (mBranchName+"lastStep").c_str(), &mLastStep);
    for (p=vi_names.begin(); p!=vi_names.end(); ++p) {
      feature_id = CombLinks::featureId(*p);
      mFeatureIndex[feature_id] = new vi_t();
      mFeatureStatus[feature_id] = new vi_t();
      t->Branch( (mBranchName+(*p)).c_str(), 
		 "std::vector<int>", 
		 &mFeatureIndex[feature_id]);
      t->Branch( (mBranchName+(*p)+"Status").c_str(), 
		 "std::vector<int>", 
		 &mFeatureStatus[feature_id]);
    }
    for (p=vvi_names.begin(); p!=vvi_names.end(); ++p) {
      feature_id = CombLinks::featureId(*p);
      mFeatureIndexVec[feature_id] = new vvi_t();
      mFeatureStatusVec[feature_id] = new vvi_t();
      t->Branch( (mBranchName+(*p)).c_str(), 
		 "std::vector<std::vector<int> >", 
		 &mFeatureIndexVec[feature_id]);
      t->Branch( (mBranchName+(*p)+"Status").c_str(), 
		 "std::vector<std::vector<int> >", 
		 &mFeatureStatusVec[feature_id]);
    }
  } else {
    (*mLog) << MSG::INFO 
	    << "Cannot retrieve CombLinksDef for RoI type: " << mRoIType 
	    << endreq;
  }
}

void RoINtComponent::fill(const RoILinks& links) {
  const CombLinksDef* def = CombLinksDef::getCombLinksDef(mRoIType);
  if (def) {
    std::vector<std::string> vi_names = def->typeLabelNamesInt();
    std::vector<std::string> vvi_names = def->typeLabelNamesVectorOfInt();
    std::vector<std::string>::const_iterator p;
    const std::vector<CombLinks>& comblinks = links.getCombLinks();
    std::vector<CombLinks>::const_iterator p_cl;
    int featureid=0;

    (*mLog) << MSG::DEBUG 
	    << "Number of CombLinks for RoI=" << mRoIType 
	    << " is " << comblinks.size() << endreq;

    for (p_cl=comblinks.begin(); p_cl!=comblinks.end(); ++p_cl) {
      (*mLog) << MSG::DEBUG << "CombLink type: " << p_cl->RoIType() << endreq;
      if (p_cl->RoIType() != mRoIType) continue;

      mType->push_back(static_cast<int>(mRoIType));
      mLastStep->push_back(p_cl->lastStep());
      for (p=vi_names.begin(); p!=vi_names.end(); ++p) {
	featureid = CombLinks::featureId(*p);
	const FeatureIndex* fi = p_cl->index(featureid);
	int tmp_i = -1;
	int tmp_s = -1;
	if (fi) {
	  (*mLog) << MSG::DEBUG << "Feature " << (*p) << ": " << (*fi) 
		  << endreq;
	  tmp_i = fi->getIndex();
	  tmp_s = fi->getStatus();
	}
	mFeatureIndex[featureid]->push_back(tmp_i);
	mFeatureStatus[featureid]->push_back(tmp_s);
      }
      for (p=vvi_names.begin(); p!=vvi_names.end(); ++p) {
	featureid = CombLinks::featureId(*p);
	const std::vector<FeatureIndex>* fiv = p_cl->indexVec(featureid);
	vi_t vi;
	vi_t vs;
	if (fiv) {
	  (*mLog) << MSG::DEBUG << "Feature " << (*p) << ": " << (*fiv) 
		  << endreq;
	  for (unsigned int j=0; j<fiv->size(); ++j) {
	    vi.push_back( (*fiv)[j].getIndex());
	    vs.push_back( (*fiv)[j].getStatus());
	  }
	}
	mFeatureIndexVec[featureid]->push_back(vi);
	mFeatureStatusVec[featureid]->push_back(vs);
      }
    }
  }
}

void RoINtComponent::clear() {
  mType->clear();
  mLastStep->clear();
  std::map<int, vi_t*>::iterator p1;
  std::map<int, vvi_t*>::iterator p2;
  for (p1=mFeatureIndex.begin(); p1!=mFeatureIndex.end(); ++p1) {
    p1->second->clear();
  }
  for (p1=mFeatureStatus.begin(); p1!=mFeatureStatus.end(); ++p1) {
    p1->second->clear();
  }
  for (p2=mFeatureIndexVec.begin(); p2!=mFeatureIndexVec.end(); ++p2) {
    p2->second->clear();
  }
  for (p2=mFeatureStatusVec.begin(); p2!=mFeatureStatusVec.end(); ++p2) {
    p2->second->clear();
  }
}

TrigMenuFlatNtComponent::TrigMenuFlatNtComponent(NtupleAlgorithm* mainAlg, 
						 const NtComponent::NtComponentParameters& params) : 
  NtComponent::NtupleComponent(mainAlg, params), 
  mTrigDecisionTool(0), mRoILinksCnvTool(0) {
  TrigMenuNtupleAlg* mymainAlg = dynamic_cast<TrigMenuNtupleAlg*>(mainAlg);

  mTrigDecisionTool = mymainAlg->trigDecisionTool();
  mRoILinksCnvTool = mymainAlg->roILinksCnvTool();
}

TrigMenuFlatNtComponent::~TrigMenuFlatNtComponent() {
  mChainEntries.clear();
  mRoILinks.clear();
  if (mChainId) delete mChainId;
  if (mRoIType) delete mRoIType;
  if (mRoIIndex) delete mRoIIndex;
  mRoINtComponents.clear();
}

StatusCode TrigMenuFlatNtComponent::book() {
  mChainId = new vi_t();
  mRoIType = new vvi_t();
  mRoIIndex = new vvi_t();

  m_tree->Branch("Chain_Id", &mChainId);
  m_tree->Branch("Chain_RoIType", &mRoIType);
  m_tree->Branch("Chain_RoIIndex", &mRoIIndex);

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
  for (p=mRoINtComponents.begin(); p!=mRoINtComponents.end(); ++p) {
    p->book(m_tree);
  }

  (*m_msg) << MSG::INFO << "Done booking branches" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode TrigMenuFlatNtComponent::fill() {
  clear();
  mRoILinksCnvTool->fill(mChainEntries, mRoILinks);

  //  int ichain=0;
  std::vector<ChainEntry>::const_iterator p;
  for (p=mChainEntries.begin(); p!=mChainEntries.end(); ++p) {
    //    mChainId->push_back(ichain++);
    mChainId->push_back(ChainEntry::chainId(p->getName()));
    const std::vector<int> types = p->getRoITypes();
    const std::vector<int> indices = p->getRoIIndices();
    mRoIType->push_back(p->getRoITypes());
    mRoIIndex->push_back(p->getRoIIndices());
  }

  std::vector<RoINtComponent>::iterator p2;
  for (p2=mRoINtComponents.begin(); p2!=mRoINtComponents.end(); ++p2) {
    p2->fill(mRoILinks);
  }

  return StatusCode::SUCCESS;
}

void TrigMenuFlatNtComponent::clear() {
  mChainEntries.clear();
  mRoILinks.clear();
  mChainId->clear();
  mRoIType->clear();
  mRoIIndex->clear();
  std::vector<RoINtComponent>::iterator p;
  for (p=mRoINtComponents.begin(); p!=mRoINtComponents.end(); ++p) {
    p->clear();
  }
}

void TrigMenuFlatNtComponent::addRoINtComponent(const std::string& brname, 
						ChainEntry::RoIType roi_type) {
  
  (*m_msg) << MSG::INFO << "Create RoINtComponent: " << brname << endreq;
  mRoINtComponents.push_back(RoINtComponent(brname, roi_type, 
					    mRoILinksCnvTool, m_msg));
}


