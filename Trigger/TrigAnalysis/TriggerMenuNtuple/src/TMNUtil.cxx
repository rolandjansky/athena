/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TMNUtil_cxx_
#define __TMNUtil_cxx_
/*
  TMNUtil.cxx
*/
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TDirectory.h"
#include <iostream>
#include "TriggerMenuNtuple/TMNUtil.h"
#include <algorithm>

using namespace std;

FeatureIndex featureIndex(int i, int status) {
  return FeatureIndex(i, status);
}

std::vector<FeatureIndex> featureIndexVec(const std::vector<int>& i, 
					  const std::vector<int>& status) {
  std::vector<FeatureIndex> x;
  for (unsigned int j=0; j<i.size(); ++j) {
    x.push_back(FeatureIndex(i[j], status[j]));
  }
  return x;
}

TMNUtil::TMNUtil() : 
  mSourceType(kD3PD),
  mMenuTree(0),
  trig_DB_SMK(0),
  mStrChain_Id(""),
  mStrChain_RoIType(""),
  mStrChain_RoIIndex("")
{
  setChainBranchNames(kTriggerNtuple);
}

template<class T>
void del_zero(T*& o) {
  if (o) delete o;
  o = 0;
}

TMNUtil::~TMNUtil() {
  std::map<std::string, vs_t*>::iterator p0;
  std::map<std::string, vi_t*>::iterator p1;
  std::map<std::string, vvi_t*>::iterator p2;

  for (p0=mBranchData_vs.begin(); p0!=mBranchData_vs.end(); ++p0) {
    del_zero(p0->second);
  }
  for (p1=mBranchData_vi.begin(); p1!=mBranchData_vi.end(); ++p1) {
    del_zero(p1->second);
  }
  for (p2=mBranchData_vvi.begin(); p2!=mBranchData_vvi.end(); ++p2) {
    del_zero(p2->second);
  }
}

void TMNUtil::setRoITypes(const std::vector<std::string>& x) {
  mRoITypes.clear();
  std::vector<std::string>::const_iterator p;
  for (p=x.begin(); p!=x.end(); ++p) {
    mRoITypes.insert(*p);
  }
}

void TMNUtil::initForD3PD(TTree* menu_tree, TTree* t_config) {
  mRoITypes.insert( "trig_RoI_L2_mu" );
  mRoITypes.insert( "trig_RoI_L2_e" );
  mRoITypes.insert( "trig_RoI_L2_tau" );
  mRoITypes.insert( "trig_RoI_L2_j" );
  mRoITypes.insert( "trig_RoI_L2_b" );

  mRoITypes.insert( "trig_RoI_EF_mu" );
  mRoITypes.insert( "trig_RoI_EF_e" );
  mRoITypes.insert( "trig_RoI_EF_tau" );
  mRoITypes.insert( "trig_RoI_EF_j" );
  mRoITypes.insert( "trig_RoI_EF_b" );

  mSourceType = kD3PD;
  readTrigConfMetadata(t_config);
  mMenuTree = menu_tree;
  setChainBranchNames(mSourceType);
  setBranchAddresses(mSourceType);
  mMenuTree->SetBranchAddress("trig_DB_SMK", &trig_DB_SMK);
}

void TMNUtil::init(TTree* menu_tree, TTree* mapping) {
  mSourceType = kTriggerNtuple;
  readChainFeatureMapping(mapping, 0);
  mMenuTree = menu_tree;
  setChainBranchNames(mSourceType);
  setBranchAddresses(mSourceType);
}

void TMNUtil::setBranchAddresses(NtupleSource_t t) {
  readBranches(mChainBranches, mRoIBranches_vs, mRoIBranches_vi, mRoIBranches_vvi);

  std::vector<TBranch*>::const_iterator p_br;
  std::map<std::string, vs_t*>::iterator p_vs;
  std::map<std::string, vi_t*>::iterator p_vi;
  std::map<std::string, vvi_t*>::iterator p_vvi;

  // Create data holders

  if (t == kTriggerNtuple) {
    mBranchData_vi[mStrChain_Id] = new vi_t();
  } else if (t == kD3PD) {
    mBranchData_vs[mStrChain_Id] = new vs_t();
  }

  // vi_t
  for (p_br=mRoIBranches_vi.begin(); p_br!=mRoIBranches_vi.end(); ++p_br) {
    mBranchData_vi[(*p_br)->GetName()] = new vi_t();
  }
  // vs_t
  for (p_br=mRoIBranches_vs.begin(); p_br!=mRoIBranches_vs.end(); ++p_br) {
    mBranchData_vs[(*p_br)->GetName()] = new vs_t();
  }
  // vvi_t
  mBranchData_vvi[mStrChain_RoIType] = new vvi_t();
  mBranchData_vvi[mStrChain_RoIIndex] = new vvi_t();
  for (p_br=mRoIBranches_vvi.begin(); p_br!=mRoIBranches_vvi.end(); ++p_br) {
    mBranchData_vvi[(*p_br)->GetName()] = new vvi_t();
  }

  // Set branch address
  // DO NOT DO mMenuTree->SetBranchStatus("*", 0);
  mMenuTree->SetBranchStatus(mStrChain_Id.c_str(), 1);
 
  if (t == kTriggerNtuple) {
    mMenuTree->SetBranchAddress(mStrChain_Id.c_str(), 
				&mBranchData_vi[mStrChain_Id]);
  } else if (t == kD3PD) {
    mMenuTree->SetBranchAddress(mStrChain_Id.c_str(), 
				&mBranchData_vs[mStrChain_Id]);
  }


  for (p_vs=mBranchData_vs.begin(); p_vs!=mBranchData_vs.end(); ++p_vs) {
//     cout << "DEBUG::setBranchAddress for " << p_vs->first<< endl;
    mMenuTree->SetBranchStatus(p_vs->first.c_str(), 1);
    mMenuTree->SetBranchAddress(p_vs->first.c_str(), &p_vs->second);
  }
  for (p_vi=mBranchData_vi.begin(); p_vi!=mBranchData_vi.end(); ++p_vi) {
    mMenuTree->SetBranchStatus(p_vi->first.c_str(), 1);
    mMenuTree->SetBranchAddress(p_vi->first.c_str(), &p_vi->second);
  }


  mMenuTree->SetBranchStatus(mStrChain_RoIType.c_str(), 1);
  mMenuTree->SetBranchAddress(mStrChain_RoIType.c_str(), 
			      &mBranchData_vvi[mStrChain_RoIType]);
  mMenuTree->SetBranchStatus(mStrChain_RoIIndex.c_str(), 1);
  mMenuTree->SetBranchAddress(mStrChain_RoIIndex.c_str(), 
			      &mBranchData_vvi[mStrChain_RoIIndex]);
  for (p_vvi=mBranchData_vvi.begin(); p_vvi!=mBranchData_vvi.end(); ++p_vvi) {
    mMenuTree->SetBranchStatus(p_vvi->first.c_str(), 1);
    mMenuTree->SetBranchAddress(p_vvi->first.c_str(), &p_vvi->second);
  }
}

void TMNUtil::readChainFeatureMapping(TTree* t, int ientry) {
  TClonesArray* c1=0;
  TClonesArray* c2=0;

  if (t) {
    t->SetBranchAddress("Chain", &c1);
    t->SetBranchAddress("Feature", &c2);
    t->GetEntry(ientry);
    //     cout << "C1 size: " << c1->GetEntries() << endl;
    //     cout << "C2 size: " << c2->GetEntries() << endl;
    for (int ii=0; ii<c1->GetEntries(); ++ii) {
      TObjString* s = dynamic_cast<TObjString*>( (*c1)[ii]);
      if (s) {
	ChainEntry::addChain(s->GetString().Data());
      } else {
	ChainEntry::addChain("");
      }
    }
    for (int ii=0; ii<c2->GetEntries(); ++ii) {
      TObjString* s = dynamic_cast<TObjString*>( (*c2)[ii]);
      if (s) {
	//	cout << "Feature: " << s->GetString().Data() << endl;
	CombLinks::addFeature(s->GetString().Data());
      } else {
      }
    }
  } else {
    gDirectory->ls();
  }
}

void TMNUtil::readTrigConfMetadata(TTree* t_config) {
  int smk=-9999;
  map<std::string, int>* hltmap=0;

  if (t_config) {
    t_config->SetBranchAddress("SMK", &smk);
    t_config->SetBranchAddress("HLTNameMap", &hltmap);
    for(int ientry=0; ientry<t_config->GetEntries(); ++ientry) {
      t_config->GetEntry(ientry);
      if(mMapOfChainIdMap.count(smk)) continue;

      std::map<int, std::string> ChainIdMap;
      if (hltmap) {
	int id=0;
	map<std::string, int>::const_iterator p;
	for (p=hltmap->begin(); p!=hltmap->end(); ++p) {
	  id = p->second;
	  if (p->first.find("EF_") == 0) id += ChainEntry::kEFChainIdOffset;
	  ChainIdMap.insert(std::pair<int, std::string>(id, p->first) );
	}
      }
      mMapOfChainIdMap.insert(std::pair<int, std::map<int, std::string> >(smk, ChainIdMap) );
    }
  } else {
    cout << "D3PD TrigConfTree is null" << endl;
  }
}

void TMNUtil::reconstructChainEntryRoILinks(std::vector<ChainEntry>& chains, 
					    RoILinks& roiLinks,
					    NtupleSource_t t) {
  ChainEntry tmp_chain;
  CombLinks tmp_links;
  unsigned int i, j;

  chains.clear();
  roiLinks.clear();

  unsigned int nChains=0;
  if (t == kTriggerNtuple) {
    nChains = mBranchData_vi[mStrChain_Id]->size();
  } else if (t == kD3PD) {
    nChains = mBranchData_vs[mStrChain_Id]->size();
  } else {
  }

  for (i=0; i<nChains; ++i) {
    int id = -1;
    if (t == kTriggerNtuple) {
      id = (*mBranchData_vi[mStrChain_Id])[i];
    } else if (t == kD3PD) {
      id = (*mBranchData_vs[mStrChain_Id])[i];
    }
    if (t == kTriggerNtuple) {
      chains.push_back(ChainEntry(ChainEntry::chainName(id), id));
    } else if (t == kD3PD) {
      std::map<int, std::string>& ChainIdMap = mMapOfChainIdMap[trig_DB_SMK];
      chains.push_back(ChainEntry(ChainIdMap[id], id));
    }
    //    cout << "Adding chain: " << ChainEntry::chainName(id) << endl;
    for (j=0; j<(*mBranchData_vvi[mStrChain_RoIType])[i].size(); ++j) {
      chains[i].addRoI( (*mBranchData_vvi[mStrChain_RoIType])[i][j], 
 			(*mBranchData_vvi[mStrChain_RoIIndex])[i][j]);
    }
  }

  std::set<std::string>::const_reverse_iterator p;
  std::set<std::string>::const_iterator pf;
  std::map<std::string, vi_t*>::iterator p1;
  std::map<std::string, vvi_t*>::iterator p2;
  std::set<std::string> br_names1, br_names2;
  std::string feature_name="";

  for (p=mRoITypes.rbegin(); p!=mRoITypes.rend(); ++p) {

    //    cout << "Check RoI type: " << (*p) << endl;
    const vi_t* type_br = mBranchData_vi[(*p)+"_type"];
    const vs_t* type_br_short = mBranchData_vs[(*p)+"_type"];

    const vi_t* lastStep_br = mBranchData_vi[(*p)+"_lastStep"];
    const vs_t* lastStep_br_short = mBranchData_vs[(*p)+"_lastStep"];

    const vi_t* active_br = mBranchData_vi[(*p)+"_active"];
    const vs_t* active_br_short = mBranchData_vs[(*p)+"_active"];

    const vs_t* tenumber_br = mBranchData_vs[(*p)+"_TENumber"];
    const vs_t* roinumber_br = mBranchData_vs[(*p)+"_roiNumber"];

    unsigned int n;
    if (type_br == 0  &&  type_br_short == 0 ) {
//       cout << "DEBUG::type_br && type_br_short is null" << endl;
      continue;
    } else if (type_br == 0){
      n = (unsigned int)type_br_short->size(); // RoI_XXX_type      
//       cout << "DEBUG:: n=" << n << endl;
    } else{
      n = (unsigned int)type_br->size(); // RoI_XXX_type
    }

    // Get vi_t feature names
    br_names1.clear();
    br_names2.clear();
    for (p1=mBranchData_vi.begin(); p1!=mBranchData_vi.end(); ++p1) {
      if ( p1->first.find(*p)==0  && 
	   p1->first.rfind("Status") != (p1->first.size()-6) &&
	   p1->first.rfind("_type") != (p1->first.size()-5) ) {
	br_names1.insert(p1->first);
      }
    }
    // Get vvi_t feature names
    for (p2=mBranchData_vvi.begin(); p2!=mBranchData_vvi.end(); ++p2) {
//       cout << "B: " << p2->first << ": status i: " 
// 	   << " " << p2->first.find("Status")
// 	   << " " << p2->first.find_last_of("Status")
// 	   << " " << p2->first.rfind("Status") << endl;
      if ( p2->first.find(*p)== 0 && 
	   p2->first.rfind("Status") != (p2->first.size()-6)) {
	  br_names2.insert(p2->first);
      }
    }

//     cout << "DEBUG:: checking br_name is finished" <<endl;

    // Get FeatureIndex
    for (i=0; i<n; ++i) {
      tmp_links = CombLinks();
 
      if (type_br == 0){
// 	cout << "DEBUG:: type_br==0" << endl;
	tmp_links.setRoIType( (*type_br_short)[i]);
      } else {
	tmp_links.setRoIType( (*type_br)[i]);
      }
      if (lastStep_br == 0){
// 	cout << "DEBUG:: lastStep_br==0" << endl;
	tmp_links.setLastStep( (*lastStep_br_short)[i]);
      } else {
	tmp_links.setLastStep( (*lastStep_br)[i]);
      }
      if (active_br){
	tmp_links.setActiveState( (*active_br)[i]);
      } else if (active_br_short) {
	tmp_links.setActiveState( (*active_br_short)[i]);
      }
      if (tenumber_br) {
	tmp_links.setTENumber( (*tenumber_br)[i]);
      }
      if (roinumber_br) {
	tmp_links.setRoINumber( (*roinumber_br)[i]);
      }

      // vi_t features
      for (pf=br_names1.begin(); pf!=br_names1.end(); ++pf) {
// 	cout << "DEBUG:: br_name1=" << *pf << endl;
	string::size_type ipos = pf->find( (*p)+"_");
	if (ipos != string::npos) {
	  feature_name = pf->substr(ipos+1+p->size());
	} else {
	  feature_name = *pf;
	}
// 	cout <<"DEBUG:: feature name=" << feature_name << endl;
// 	cout << "Feature : " << feature_name << " from " 
// 	     << (*pf) << " prefix=" << (*p)+"_" << endl;

	vi_t* feature_br = mBranchData_vi[*pf];
	vi_t* status_br = mBranchData_vi[(*pf)+"Status"];
	feature_br = mBranchData_vi[*pf];
	status_br = mBranchData_vi[(*pf)+"Status"];

	if (feature_br==0 || status_br==0) {
// 	  cout << "Feature index for RoI type=" << (*p)
// 	       << " feature=" << feature_name << " null" << endl;
// 	  cout << "  index_vec=" << feature_br 
// 	       << " status_vec=" << status_br << endl;
	} else {
	  //	  cout << "adding feature: " << (*pf) << endl;
	  tmp_links.addIndex(feature_name, 
			     featureIndex( (*feature_br)[i], 
					   (*status_br)[i]));
	}
      }
//       cout  << "DEBUG:: finish for vi..." << endl;

      // vvi_t features
      for (pf=br_names2.begin(); pf!=br_names2.end(); ++pf) {
	string::size_type ipos = pf->find( (*p)+"_");
	if (ipos != string::npos) {
	  feature_name = pf->substr(ipos+1+p->size());
	} else {
	  feature_name = *pf;
	}
// 	cout << "DEBUG:: create pointer for branch" << endl;
	vvi_t* feature_br = mBranchData_vvi[*pf];
	vvi_t* status_br = mBranchData_vvi[(*pf)+"Status"];
	feature_br = mBranchData_vvi[*pf];
	status_br = mBranchData_vvi[(*pf)+"Status"];


	if ( feature_br && status_br) {
	  //	  cout << "adding feature: " << (*pf) << endl;
	  tmp_links.addIndexVec(feature_name, 
				featureIndexVec( (*feature_br)[i], 
						 (*status_br)[i]));
	} else {
	  // 	  cout << "Feature index for RoI type=" << (*p)
	  // 	       << " feature=" << (*pf) << " null" << endl;
	  // 	  cout << "  index_vec=" << feature_br 
	  // 	       << " status_vec=" << status_br << endl;
	}
      }
      //      cout << "tmp links: " << tmp_links << endl;
      roiLinks.addCombLinks(tmp_links);
    }
    //    cout << "Reconstructed " << roiLinks.getCombLinks().size() << " RoIs" << endl;
    
  }
}

int TMNUtil::readBranches(std::vector<TBranch*>& chain_branches, 
			  std::vector<TBranch*>& roi_branches_vs, 
			  std::vector<TBranch*>& roi_branches_vi, 
			  std::vector<TBranch*>& roi_branches_vvi) {
  TObjArray* branches = mMenuTree->GetListOfBranches();
  TIter next(branches);
  TObject* obj;
  
  while ( (obj = next()) ) {
    //   for (int i=0; i<branches->GetEntries(); ++i) {
//     obj = (*branches)[i];
    // if (!obj) continue;
    const char* obj_name=obj->GetName();
    TBranch* b = dynamic_cast<TBranch*>(obj);
//     cout << "b = " << b << endl;
//     cout << "objname: " << obj_name << endl;
    if (!b) continue;

    std::string tmpname = obj_name;
    if (tmpname == mStrChain_Id) chain_branches.push_back(b);
    else if (tmpname == mStrChain_RoIType) chain_branches.push_back(b);
    else if (tmpname == mStrChain_RoIIndex) chain_branches.push_back(b);
    
    //    TObjArray* leaves = b->GetListOfLeaves();
//     cout << "branch: " << tmpname << endl;
//     cout << "  class name: " << b->GetClassName() << endl; // bingo!
//     cout << "  N leaves: " << leaves->GetEntries() << endl;

    const char* brcls = b->GetClassName();
    bool ok=false;
    std::set<std::string>::const_iterator ppp;
    for (ppp=mRoITypes.begin(); ppp!=mRoITypes.end(); ++ppp) {
//       if ( tmpname.find(*ppp)==0  &&
// 	   tmpname.rfind("_ROI") == (tmpname.size()-4) ) {
// 	string::size_type ipos = tmpname.find( (*ppp)+"_");
// 	string ROI_NAME="";
// 	if (ipos != string::npos) {
// 	  ROI_NAME = tmpname.substr(ipos+1+ppp->size());
// 	} else {
// 	  ROI_NAME = tmpname;
// 	}
// 	if( mRoITypesNameMap.find(*ppp)==mRoITypesNameMap.end() ) {
// 	  mRoITypesNameMap.insert( std::map<std::string, std::string>::value_type(*ppp, tmpname) );
// 	}
//       }

//       cout << "Checking " << tmpname << " against " << (*ppp) << endl;
      if ( tmpname.find(*ppp)==0  && tmpname != (*ppp)+"_n") {
	ok = true;
// 	cout << "ROI branch found: " << tmpname << endl;
	break;
      }
    }

    if (ok) {
      std::string brcls2 = brcls;
      if (brcls2 == "vector<short>") roi_branches_vs.push_back(b);
      else if (brcls2 == "vector<int>") roi_branches_vi.push_back(b);
      else if (brcls2 == "vector<vector<int> >") roi_branches_vvi.push_back(b);
      else {
	cout << "Unknown class type for branch : " << tmpname << endl;
      }
    }
  }

  return 0;
}

void TMNUtil::dumpChainRoIFeatures(const ChainEntry& chain_entry, 
				   const RoILinks& roiLinks) {
  std::vector<CombLinks> combs = roiLinks.getCombLinks(chain_entry);
  std::vector<CombLinks>::const_iterator p;

//   std::string chain_name("");
//   std::vector<std::string> all_features;
//   std::vector<std::string>::const_iterator pf;
//   const FeatureIndex* index=0;
//   const FeatureIndexVec_t* index_vec=0;

  cout << "#--------------------------------------------------" << endl;
  cout << "Chain " << chain_entry.getName() 
       << ": " << chain_entry.getRoITypes().size() << " RoI(s)" 
       << ", " << combs.size() << " comb(s)" << endl;
  for (p=combs.begin(); p!=combs.end(); ++p) {
    cout << "  " << (*p) << endl;
//     all_features = p->allFeatureNames();
//     for (pf=all_features.begin(); pf!=all_features.end(); ++pf) {
//       if (index = p->index(*pf)) {
//       } else if (index_vec = p->indexVec(*pf)) {
//       }
//     }
  }
}

void TMNUtil::dumpChainRoIFeatures(std::string& chain_name){
  std::vector<ChainEntry>::const_iterator pChain;
  for (pChain=mChainEntry.begin(); pChain!=mChainEntry.end(); ++pChain) {  
    if(pChain->getName() == chain_name){
      dumpChainRoIFeatures( *pChain, mRoILinks);
      return;
    }
  }
}


void TMNUtil::GetEntry(int ientry){
  mMenuTree->GetEntry(ientry);
  reconstructChainEntryRoILinks(mChainEntry, mRoILinks, mSourceType);
}

void TMNUtil::readNavigation(){
  mCombLinksMap.clear();
  reconstructChainEntryRoILinks(mChainEntry, mRoILinks, mSourceType);
}

unsigned int TMNUtil::getSMK() {
  return trig_DB_SMK;
}

const std::vector<CombLinks> TMNUtil::getCombLinks(ChainEntry::RoIType roitype, std::string& ChainName){
  std::vector<CombLinks> Combs;
  std::string roi_name = getRoIName( roitype ); 
  Combs = getCombLinks( roi_name, ChainName, true);

  return Combs;
}


CombLinks TMNUtil::mergeCombLinks( CombLinks comb1, CombLinks comb2,
				   const string MatchingFeature) {
  CombLinks comb;
  const FeatureIndex* fiRoI1 = comb1.index(MatchingFeature);
  const FeatureIndex* fiRoI2 = comb2.index(MatchingFeature);

  if( fiRoI1 ){
    if( fiRoI2 ) {
      if( fiRoI1->getIndex()!=fiRoI2->getIndex() ) return comb;

      comb.addIndex( MatchingFeature, *fiRoI1 );

      std::vector<std::string> features1 = comb1.FeatureNames();
      std::vector<std::string>::const_iterator p1;
      for (p1=features1.begin(); p1!=features1.end(); ++p1) {
	if( *p1==MatchingFeature ) continue;
	const FeatureIndex* tmp = comb1.index( *p1 );
	if(tmp) {
	  comb.addIndex( *p1, *tmp);
	}
      }
      std::vector<std::string> features2 = comb2.FeatureNames();
      std::vector<std::string>::const_iterator p2;
      for (p2=features2.begin(); p2!=features2.end(); ++p2) {
	if( *p2==MatchingFeature ) continue;
	const FeatureIndex* tmp = comb1.index( *p2 );
	if(tmp) {
	  comb.addIndex( *p2, *tmp);
	}
      }

      std::vector<std::string> featureVecs1 = comb1.FeatureVecNames();
      std::vector<std::string>::const_iterator pv1;
      for (pv1=featureVecs1.begin(); pv1!=featureVecs1.end(); ++pv1){
	const std::vector<FeatureIndex>* tmp = comb1.indexVec( *pv1 );
	if(tmp) {
	  comb.addIndexVec( *pv1, *tmp );
	}
      }
      std::vector<std::string> featureVecs2 = comb2.FeatureVecNames();
      std::vector<std::string>::const_iterator pv2;
      for (pv2=featureVecs2.begin(); pv2!=featureVecs2.end(); ++pv2){
	const std::vector<FeatureIndex>* tmp = comb2.indexVec( *pv2 );
	if(tmp) {
	  comb.addIndexVec( *pv2, *tmp );
	}
      }
    }  else {
      return comb1;
    }
  } else if ( fiRoI2 ) {
    return comb2;
  } else {
    return comb;
  }

  return comb;
}

const std::vector<CombLinks> TMNUtil::mergeCombLinksVec( std::vector<CombLinks>& combs1,
							 std::vector<CombLinks>& combs2,
							 string MatchingFeature) {
  std::vector<CombLinks> combs;

  const std::vector<int> RoIs1 = getIndex( MatchingFeature , combs1 , true );
  const std::vector<int> RoIs2 = getIndex( MatchingFeature , combs2 , true );

  if( RoIs1.size() == combs1.size() &&
      RoIs2.size() == combs2.size() )  {
    combs = combs1;

    for(int i=0; i<(int)RoIs2.size(); ++i) {
      if( find(RoIs1.begin(), RoIs1.end(), RoIs2[i])==RoIs1.end() ) {
	combs.push_back( combs2[i] );
      } else {
	for(int j=0; j<(int)RoIs1.size(); ++j){
	  if( RoIs1[j]==RoIs2[i]) {
	    combs[j] = mergeCombLinks( combs1[j], combs2[i], MatchingFeature );
	  }
	}
      }
    }
  }
  return combs;
}




const std::vector<int> TMNUtil::getRoINumbers(ChainEntry::RoIType roitype,
					      std::string ChainName){
  std::vector<int> numbers;
  std::map<int, CombLinks> RoIMap;

  if( mCombLinksMap.count(ChainName)!=0 ) {
    std::map<int, CombLinks>::const_iterator pMap;
    RoIMap = mCombLinksMap[ChainName];
    for( pMap=RoIMap.begin(); pMap!=RoIMap.end(); ++pMap){
      numbers.push_back( pMap->first );
    }
  } else {
    const vector<CombLinks> Combs = getCombLinks( roitype, ChainName );
    vector<CombLinks>::const_iterator pComb;
    std::string roi_name = getRoIName( roitype );
    for( pComb=Combs.begin(); pComb!=Combs.end(); ++pComb ) {
      const FeatureIndex* fIndex = pComb->index( roi_name );
      if(!fIndex) {
	continue;
      } else if ( fIndex->getIndex() < 0) {
	continue;
      } else {
	int i = fIndex->getIndex();
	numbers.push_back( i );
	RoIMap.insert( std::map<int, CombLinks>::value_type( i, *pComb) );
      }
    }
    mCombLinksMap.insert( std::map<std::string, std::map<int, CombLinks> >::value_type( ChainName, RoIMap ) );
  }

  return numbers;
}


int TMNUtil::getLastStep(std::string ChainName, int roi_number)
{
  if( mCombLinksMap.count(ChainName)!=0 ) {
    std::map<int, CombLinks> RoIMap = mCombLinksMap[ChainName];
    if ( RoIMap.count(roi_number)!=0 ) {
      return RoIMap[roi_number].lastStep();
    } else {
      return -1;
    }
  } else {
    std::cout << "getlastStep::Can not found map for " << ChainName << std::endl;
    std::cout << "             please call getRoINumbers(ChainEntry::RoIType roitype, string ChainName)" << std::endl;
    return -1;
  }
}

int TMNUtil::getIndexInRoI(std::string feature, std::string ChainName, int roi_number, bool ActiveOnly){
  if( mCombLinksMap.count(ChainName)!=0 ) {
    std::map<int, CombLinks> RoIMap = mCombLinksMap[ChainName];
    if ( RoIMap.count(roi_number)!=0 ) {
      const FeatureIndex* fIndex = RoIMap[roi_number].index( feature );
      if (fIndex) {
	if ( !ActiveOnly ||
	     ( ActiveOnly && fIndex->getStatus()) ){
	  return fIndex->getIndex();
	} else {
	  return -1;
	}
      } else {
	return -2;
      }
    } else {
      return -2;
    }
  } else {
    std::cout << "getIndexInRoI::Can not found map for " << ChainName << std::endl;
    std::cout << "               please call getRoINumbers(ChainEntry::RoIType roitype, string ChainName)" << std::endl;
    return -3;
  }
}

const std::vector<int>  TMNUtil::getIndexVecInRoI(std::string feature, std::string ChainName, int roi_number, bool ActiveOnly){
  vector<int> indices; 
  if( mCombLinksMap.count(ChainName)!=0 ) {
    std::map<int, CombLinks> RoIMap = mCombLinksMap[ChainName];
    const vector<FeatureIndex>* fIndexVec = RoIMap[roi_number].indexVec( feature );
    if ( fIndexVec && fIndexVec->size()>0 ) {
      if ( !ActiveOnly ||
	   ( ActiveOnly && fIndexVec->at(0).getStatus()) ){
	for( int i=0; i<(int)fIndexVec->size(); ++i ){
	  int index = fIndexVec->at(i).getIndex();
	  if( index>=0 ){
	    indices.push_back( index );
	  }
	}
      }
    }
    return indices;
  } else {
    std::cout << "getIndexVecInRoI::Can not found map for " << ChainName << std::endl;
    std::cout << "                 please call getRoINumbers(ChainEntry::RoIType roitype, string ChainName)" << std::endl;
    return indices;
  }
}






const std::vector<int> TMNUtil::getIndex(std::string feature, std::string ChainName, bool ActiveOnly){
  std::vector<int> index;
  std::vector<ChainEntry>::const_iterator pChain;
  for(pChain=mChainEntry.begin(); pChain!=mChainEntry.end(); ++pChain){
      if( pChain->getName()==ChainName ) {
      std::vector<CombLinks> Combs = mRoILinks.getCombLinks(*pChain);
      std::vector<CombLinks>::const_iterator pComb;
      for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
	const FeatureIndex* fIndex = pComb->index(feature);
	if (fIndex){// Case 1: featureContainer is not a vector
	  if ( !ActiveOnly ||
	       ( ActiveOnly && fIndex->getStatus()) ){
	    int i = fIndex->getIndex();
	    if( i>=0 ) {
	      index.push_back( i );
	    }
	  }// Active ?
	}
      }// Loop For CombLinks(=RoI)
    }// if ChainEntry to check ?
  }// Loop for ChainEntries

  return index;
}

const std::vector<std::vector<int> > TMNUtil::getIndexVec(std::string feature, std::string ChainName, bool ActiveOnly){
  std::vector<std::vector<int> > indices;

  std::vector<ChainEntry>::const_iterator pChain;
  for(pChain=mChainEntry.begin(); pChain!=mChainEntry.end(); ++pChain){
      if( pChain->getName()==ChainName ) {
      std::vector<CombLinks> Combs = mRoILinks.getCombLinks(*pChain);
      std::vector<CombLinks>::const_iterator pComb;
      for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
	const std::vector<FeatureIndex>* fIndexVec = pComb->indexVec(feature);

	if(fIndexVec) {
	  const FeatureIndex* fIndex = &((*fIndexVec)[0]);
	  if ( !ActiveOnly ||
	       ( ActiveOnly && fIndex->getStatus()) ){
	    std::vector<int> indexVec;
	    for(int i=0; i<(int)fIndexVec->size(); ++i){
	      int index = (fIndexVec->at(i)).getIndex();
	      if( index>=0 ) {
		indexVec.push_back( index );
	      }
	    }
	    indices.push_back( indexVec );
	  }// Active?
	}
      }// Loop For CombLinks(=RoI)
    }// if ChainEntry to check ?
  }// Loop for ChainEntries

  return indices;
}


const std::vector<int> TMNUtil::getIndex(std::string feature, std::vector<CombLinks>& Combs, bool ActiveOnly){
  std::vector<int> index;

  std::vector<CombLinks>::const_iterator pComb;
  for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
    const FeatureIndex* fIndex = pComb->index(feature);
    if (fIndex){
      if ( !ActiveOnly ||
	   ( ActiveOnly && fIndex->getStatus()) ){
	int i = fIndex->getIndex();
	if( i>=0 ) {
	  index.push_back( i );
	    }
      }// Active ?
    }
  }// Loop For CombLinks(=RoI)

  return index;
}

const std::vector<std::vector<int> > TMNUtil::getIndexVec(std::string feature, std::vector<CombLinks>& Combs, bool ActiveOnly){
  std::vector<std::vector<int> > indices;

  std::vector<CombLinks>::const_iterator pComb;
  for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
    const std::vector<FeatureIndex>* fIndexVec = pComb->indexVec(feature);

    if(fIndexVec) {
      const FeatureIndex* fIndex = &((*fIndexVec)[0]);
      if ( !ActiveOnly ||
	   ( ActiveOnly && fIndex->getStatus()) ){
	std::vector<int> indexVec;
	for(int i=0; i<(int)fIndexVec->size(); ++i){
	  int index = (fIndexVec->at(i)).getIndex();
	  if( index>=0 ) {
	    indexVec.push_back( index );
	  }
	}
	indices.push_back( indexVec );
      }// Active?
    }
  }// Loop For CombLinks(=RoI)

  return indices;
}



const std::vector<CombLinks> TMNUtil::getCombLinks(std::string feature, std::string ChainName, bool ActiveOnly){
  std::vector<CombLinks> RoIs;

  std::vector<ChainEntry>::const_iterator pChain;
  for(pChain=mChainEntry.begin(); pChain!=mChainEntry.end(); ++pChain){
    if( pChain->getName()==ChainName ) {
      std::vector<CombLinks> Combs = mRoILinks.getCombLinks(*pChain);
      std::vector<CombLinks>::const_iterator pComb;
      for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
	const FeatureIndex* fIndex = pComb->index(feature);
	const std::vector<FeatureIndex>* fIndexVec = pComb->indexVec(feature);
	if (fIndex){// Case 1: featureContainer is not a vector
	  if ( !ActiveOnly ||
	       ( ActiveOnly && fIndex->getStatus()) ){
	    int i = fIndex->getIndex();
	    if( i>=0 ) {
	      RoIs.push_back( *pComb );
	    }
	  }// Active ?
	} else if(fIndexVec) {//case 2: featureContainer is a vector
	  const FeatureIndex* Index = &((*fIndexVec)[0]);// Need to chanege(to select matched one)
	  if ( !ActiveOnly ||
	       ( ActiveOnly && Index->getStatus()) ){
	    int i = Index->getIndex();
	    if( i>=0 ) {
	      RoIs.push_back( *pComb );
	    }
	  }// Active?
	}
      }// Loop For CombLinks(=RoI)
    }// if ChainEntry to check ?
  }// Loop for ChainEntries

  return RoIs;
}

const CombLinks TMNUtil::getSameRoI(CombLinks in, int roi_type, std::string L1RoI) {
  CombLinks out;

  const FeatureIndex* infIndex = in.index(L1RoI);
  int index=-99;
  if(infIndex){
    index = infIndex->getIndex();
  }
  std::vector<CombLinks> Combs = mRoILinks.getCombLinks(roi_type);
  std::vector<CombLinks>::const_iterator pComb;
  for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
    const FeatureIndex* fIndex = pComb->index(L1RoI); 
    if(fIndex){
      if(fIndex->getIndex()==index) out = *pComb;
    }
  }
  return out;
}

const std::vector<CombLinks> TMNUtil::getSameRoI(std::vector<CombLinks>& in, int roi_type, std::string L1RoI) {
  std::vector<CombLinks> out;
  std::vector<CombLinks>::const_iterator pComb;
  for(pComb=in.begin(); pComb!=in.end(); ++pComb) {
    out.push_back( getSameRoI( *pComb, roi_type, L1RoI ) );
  }

  return out;
}

const CombLinks getSameRoI(CombLinks in, RoILinks& roi, int roi_type, std::string L1RoI) {
  CombLinks out;

  const FeatureIndex* infIndex = in.index(L1RoI);
  int index=-99;
  if(infIndex){
    index = infIndex->getIndex();
  }
  std::vector<CombLinks> Combs = roi.getCombLinks(roi_type);
  std::vector<CombLinks>::const_iterator pComb;
  for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
    const FeatureIndex* fIndex = pComb->index(L1RoI); 
    if(fIndex){
      if(fIndex->getIndex()==index) out = *pComb;
    }
  }
  return out;
}

const std::vector<CombLinks> getSameRoI(std::vector<CombLinks>& in, RoILinks& roi, int roi_type, std::string L1RoI) {
  std::vector<CombLinks> out;
  std::vector<CombLinks>::const_iterator pComb;
  for(pComb=in.begin(); pComb!=in.end(); ++pComb) {
    out.push_back( getSameRoI( *pComb, roi, roi_type, L1RoI ) );
  }

  return out;
}

void TMNUtil::setChainBranchNames(NtupleSource_t t) {
  if (t == kTriggerNtuple) {
    mStrChain_Id = "Chain_Id";
    mStrChain_RoIType = "Chain_RoIType";
    mStrChain_RoIIndex = "Chain_RoIIndex";
  } else if (t == kD3PD) {
    std::string prefix = "trig_Nav_chain_";
    mStrChain_Id = prefix + "ChainId";
    mStrChain_RoIType = prefix + "RoIType";
    mStrChain_RoIIndex = prefix + "RoIIndex";
  }
}

std::string TMNUtil::getRoIName(ChainEntry::RoIType roitype){
  std::string roi_name="";
  if( roitype==ChainEntry::kRoIType_L2_mu || roitype==ChainEntry::kRoIType_EF_mu) {
    roi_name="Muon_ROI";
  } else if ( roitype==ChainEntry::kRoIType_L2_e || roitype==ChainEntry::kRoIType_EF_e) {
    roi_name="EmTau_ROI";
  } else if ( roitype==ChainEntry::kRoIType_L2_tau || roitype==ChainEntry::kRoIType_EF_tau) {
    roi_name="EmTau_ROI";
  } else if ( roitype == ChainEntry::kRoIType_L2_b ||
	      roitype == ChainEntry::kRoIType_EF_b ||
	      roitype == ChainEntry::kRoIType_L2_j ||
	      roitype == ChainEntry::kRoIType_EF_j) {
    roi_name = "Jet_ROI";
  } else {
  }
  return roi_name;
}


ChainEntry::RoIType TMNUtil::getL2Type( ChainEntry::RoIType roitype ){
  if( roitype==ChainEntry::kMuonRoIType) {return ChainEntry::kRoIType_L2_mu; } 
  else if ( roitype==ChainEntry::kElectronRoIType) {return ChainEntry::kRoIType_L2_e; }
  else if ( roitype==ChainEntry::kTauRoIType) {return ChainEntry::kRoIType_L2_tau; }
  else if ( roitype==ChainEntry::kJetRoIType) {return ChainEntry::kRoIType_L2_j; }
  else if ( roitype==ChainEntry::kBjetRoIType) {return ChainEntry::kRoIType_L2_b; }
  else if ( roitype==ChainEntry::kMETRoIType) {return ChainEntry::kRoIType_L2_xe; }
  else if ( roitype==ChainEntry::kBphysicsRoIType) {return ChainEntry::kRoIType_L2_DiMu; }
  else { return ChainEntry::kUnknown;}
}

ChainEntry::RoIType TMNUtil::getEFType( ChainEntry::RoIType roitype ){
  if( roitype==ChainEntry::kMuonRoIType) {return ChainEntry::kRoIType_EF_mu; } 
  else if ( roitype==ChainEntry::kElectronRoIType) {return ChainEntry::kRoIType_EF_e; }
  else if ( roitype==ChainEntry::kTauRoIType) {return ChainEntry::kRoIType_EF_tau; }
  else if ( roitype==ChainEntry::kJetRoIType) {return ChainEntry::kRoIType_EF_j; }
  else if ( roitype==ChainEntry::kBjetRoIType) {return ChainEntry::kRoIType_EF_b; }
  else { return ChainEntry::kUnknown;}
}

void TMNUtil::clear() {
  mCombLinksMap.clear();
}

#endif//__TMNUtil_cxx_
