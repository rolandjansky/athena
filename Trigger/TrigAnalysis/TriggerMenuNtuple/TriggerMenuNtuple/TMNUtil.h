/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TMNUtil_h__
#define __TMNUtil_h__
/*
  TMNUtil.h
*/
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include "TH1.h"
#include "TH2.h"
#include "TriggerMenuNtuple/FeatureIndex.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/RoILinks.h"
#include "TriggerMenuNtuple/CombLinks.h"

class TTree;
class TBranch;

FeatureIndex featureIndex(int i, int status);
std::vector<FeatureIndex> featureIndexVec(const std::vector<int>& i, 
					  const std::vector<int>& status);


class TMNUtil {
public:
  enum NtupleSource_t {
    kTriggerNtuple=0,
    kD3PD=1
  };

public:
  typedef std::vector<int> vi_t;
  typedef std::vector<short> vs_t;
  typedef std::vector<std::vector<int> > vvi_t;

  TMNUtil();
  ~TMNUtil();

  void setRoITypes(const std::vector<std::string>& x);
  void setRoITypes(const std::set<std::string>& x) { mRoITypes = x; }

  void init(TTree* t_menu, TTree* mapping); // For NTUP_TRIG
  void initForD3PD(TTree* t_menu, TTree* t_config); // For D3PD

  void readChainFeatureMapping(TTree* t, int ientry=0);
  void readTrigConfMetadata(TTree* t_config);

  void setSourceType(NtupleSource_t t) { mSourceType = t; }
  NtupleSource_t sourceType() const { return mSourceType; }
  const std::vector<ChainEntry>& getChainEntries() const { return mChainEntry; }
  const RoILinks getRoILinks(){ return mRoILinks; }

  void reconstructChainEntryRoILinks(std::vector<ChainEntry>& chains, 
				     RoILinks& roiLinks,
				     NtupleSource_t t=kTriggerNtuple);


  int readBranches(std::vector<TBranch*>& chain_branches, 
		   std::vector<TBranch*>& roi_branches_vs, 
		   std::vector<TBranch*>& roi_branches_vi, 
		   std::vector<TBranch*>& roi_branches_vvi);
  void setBranchAddresses(NtupleSource_t t);

  void dumpChainRoIFeatures(const ChainEntry& chain_entry, 
			    const RoILinks& roiLinks);
  void dumpChainRoIFeatures(std::string& chain_name);

  void GetEntry(int ientry);
  void readNavigation();
  unsigned int getSMK();


  const std::vector<CombLinks> getCombLinks(ChainEntry::RoIType roitype, std::string& ChainName);

  const std::vector<int> getRoINumbers(ChainEntry::RoIType roitype, std::string ChainName);
  int getLastStep(std::string ChainName, int roi_number);
  int getIndexInRoI(std::string feature, std::string ChainName, int roi_number, bool ActiveOnly=false);
  const std::vector<int> getIndexVecInRoI(std::string feature, std::string ChainName, int roi_number, bool ActiveOnly=false);



  CombLinks mergeCombLinks( CombLinks comb1, CombLinks comb2,
			    const std::string MatchingFeature);
  const std::vector<CombLinks> mergeCombLinksVec( std::vector<CombLinks>& combs1,
						  std::vector<CombLinks>& combs2,
						  std::string MatchingFeature);

  const std::vector<int> getIndex(std::string feature, std::string ChainName, bool ActiveOnly=true);
  const std::vector<std::vector<int> > getIndexVec(std::string feature, std::string ChainName, bool ActiveOnly=true);

  const std::vector<int> getIndex(std::string feature, std::vector<CombLinks>& Combs, bool ActiveOnly=true);
  const std::vector<std::vector<int> > getIndexVec(std::string feature, std::vector<CombLinks>& Combs, bool ActiveOnly=true);

  const std::vector<CombLinks> getCombLinks(std::string feature, std::string ChainName, bool ActiveOnly=true);

  const CombLinks getSameRoI(CombLinks in, int roi_type, std::string L1ROI);
  const std::vector<CombLinks> getSameRoI(std::vector<CombLinks>& in,
					  int roi_type,
					  std::string L1RoI);

  void clear();

private:
  void setChainBranchNames(NtupleSource_t t);
  std::string getRoIName(ChainEntry::RoIType roitype);
  ChainEntry::RoIType getL2Type( ChainEntry::RoIType roitype );
  ChainEntry::RoIType getEFType( ChainEntry::RoIType roitype );

private:
  NtupleSource_t mSourceType;

  TTree* mMenuTree;
  std::set<std::string> mRoITypes;
  std::map<int, std::map<int, std::string> > mMapOfChainIdMap; //[SMK - [ChainId-ChainName]]
  std::map<int, std::string> mChainIdMap;//[chainName-ChainName] for current event
  unsigned int trig_DB_SMK;


  std::vector<TBranch*> mChainBranches;
  std::vector<TBranch*> mRoIBranches_vs;
  std::vector<TBranch*> mRoIBranches_vi;
  std::vector<TBranch*> mRoIBranches_vvi;

  std::map<std::string, vs_t*> mBranchData_vs;
  std::map<std::string, vi_t*> mBranchData_vi;
  std::map<std::string, vvi_t*> mBranchData_vvi;
  std::map<std::string, std::string> mRoITypesNameMap;
  std::map<std::string, std::map<int, CombLinks> > mCombLinksMap;

  std::vector<ChainEntry> mChainEntry;
  RoILinks mRoILinks;

  std::string mStrChain_Id;
  std::string mStrChain_RoIType;
  std::string mStrChain_RoIIndex;
};

template<class T>
void fillVar(std::vector<T>& Vars,
	     std::string ChainName,
	     bool ActiveOnly,
	     std::vector<ChainEntry>& chains,
	     RoILinks& roi,
	     const std::string& feature,
	     std::vector<T>* var) {
  Vars.clear();
  std::vector<ChainEntry>::const_iterator pChain;
  for (pChain=chains.begin(); pChain!=chains.end(); ++pChain) {
    if( pChain->getName()==ChainName ) {
      std::vector<CombLinks> Combs = roi.getCombLinks(*pChain);
      std::vector<CombLinks>::const_iterator pComb;
      for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
	const FeatureIndex* fIndex = pComb->index(feature);
	const std::vector<FeatureIndex>* fIndexVec = pComb->indexVec(feature);
	if (fIndex){// Case 1: featureContainer is not a vector
	  if ( !ActiveOnly ||
	       ( ActiveOnly && fIndex->getStatus()) ){
	    int i = fIndex->getIndex();
	    int n = var->size();
	    if( i>=0 && i<n ) {
	      Vars.push_back( (*var)[i]);
	    } else if(i!=-1){
	      std::cout << "Index of " << feature << " " <<
		i << " is out of range...(Max:" << n << ")" << std::endl;
	    }
	  }// Active ?
	} else if(fIndexVec) {//case 2: featureContainer is a vector
	  const FeatureIndex* Index = &((*fIndexVec)[0]);// Need to chanege(to select matched one)
	  if ( !ActiveOnly ||
	       ( ActiveOnly && Index->getStatus()) ){
	    int i = Index->getIndex();
	    int n = var->size();
	    if( i>=0 && i<n ) {
	      Vars.push_back( (*var)[i]);
	    } else if(i!=-1){
	      std::cout << "Index of " << feature << " : " <<
		i << " is out of range...(Max:" << n << ")" << std::endl;
	    }
	  }// Active?
	}
      }// Loop For CombLinks(=RoI)
    }// if ChainEntry to check ?
  }// Loop for ChainEntries
  return;
}


template<class T>
void fillVar(std::vector<T>& Vars,
	     std::string ChainName,
	     bool ActiveOnly,
	     std::vector<ChainEntry>& chains,
	     RoILinks& roi,
	     const std::string& feature,
	     const T var[],
	     int size) {
  Vars.clear();
  std::vector<ChainEntry>::const_iterator pChain;
  for (pChain=chains.begin(); pChain!=chains.end(); ++pChain) {
    if( pChain->getName()==ChainName ) {
      std::vector<CombLinks> Combs = roi.getCombLinks(*pChain);
      std::vector<CombLinks>::const_iterator pComb;
      for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
	const FeatureIndex* fIndex = pComb->index(feature);
	const std::vector<FeatureIndex>* fIndexVec = pComb->indexVec(feature);
	if (fIndex){// Case 1: featureContainer is not a vector
	  if ( !ActiveOnly ||
	       ( ActiveOnly && fIndex->getStatus()) ){
	    int i = fIndex->getIndex();
	    int n = size;
	    if( i>=0 && i<n ) {
	      Vars.push_back( var[i] );
	    } else if(i!=-1){
	      std::cout << "Index of " << feature << " : " <<
		i << "is out of range...(Max:" << n << ")" << std::endl;
	    }
	  }// Active ?
	} else if(fIndexVec) {//case 2: featureContainer is a vector
	  const FeatureIndex* Index = &((*fIndexVec)[0]);// Need to chanege(to select matched one)
	  if ( !ActiveOnly ||
	       ( ActiveOnly && Index->getStatus()) ){
	    int i = Index->getIndex();
	    int n = size;
	    if( i>=0 && i<n ) {
	      Vars.push_back( var[i]);
	    } else if(i!=-1){
	      std::cout << "Index of " << feature << " : " <<
		i << "is out of range...(Max:" << n << ")" << std::endl;
	    }
	  }// Active?
	}
      }// Loop For CombLinks(=RoI)
    } // if ChainEntry to check ?
  }// Loop for ChainEntries
  return;
}

template<class T>
void fillVar(std::vector<T>& Vars,
	     std::vector<CombLinks> Combs,
	     bool ActiveOnly,
	     const std::string& feature,
	     const T var[],
	     int size) {
  Vars.clear();
  std::vector<CombLinks>::const_iterator pComb;
  for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
    const FeatureIndex* fIndex = pComb->index(feature);
    const std::vector<FeatureIndex>* fIndexVec = pComb->indexVec(feature);
    if (fIndex){// Case 1: featureContainer is not a vector
      if ( !ActiveOnly ||
	   ( ActiveOnly && fIndex->getStatus()) ){
	int i = fIndex->getIndex();
	int n = size;
	if( i>=0 && i<n ) {
	  Vars.push_back( var[i] );
	} else if(i!=-1){
	  std::cout << "Index of " << feature << " : " <<
	    i << "is out of range...(Max:" << n << ")" << std::endl;
	}
      }// Active ?
    } else if(fIndexVec) {//case 2: featureContainer is a vector
      const FeatureIndex* Index = &((*fIndexVec)[0]);// Need to chanege(to select matched one)
      if ( !ActiveOnly ||
	   ( ActiveOnly && Index->getStatus()) ){
	int i = Index->getIndex();
	int n = size;
	if( i>=0 && i<n ) {
	  Vars.push_back( var[i]);
	} else if(i!=-1){
	  std::cout << "Index of " << feature << " : " <<
	    i << "is out of range...(Max:" << n << ")" << std::endl;
	}
      }// Active?
    }
  }// Loop For CombLinks(=RoI)
  return;
}


 
template<class T, class U>
void fillVarPair(std::multimap<T, U>& varPair,
		 std::string ChainName,
		 std::vector<ChainEntry>& chains,
		 RoILinks& roi,
		 const std::string& feature1,
		 const std::string& feature2,
		 std::vector<T>* var1,
		 std::vector<U>* var2){
   varPair.clear();
   std::vector<ChainEntry>::const_iterator pChain;
   for (pChain=chains.begin(); pChain!=chains.end(); ++pChain) {
     if( pChain->getName()==ChainName ) {
       std::vector<CombLinks> Combs = roi.getCombLinks(*pChain);
       std::vector<CombLinks>::const_iterator pComb;
       for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
	 FeatureIndex* tmp_fi = NULL;
	 //----- find FeatureIndex -----// 
	 //-- feature 1 --//
	 tmp_fi = const_cast<FeatureIndex*>( pComb->index(feature1) );
	 if( !tmp_fi ) {
	   const std::vector<FeatureIndex>* fIndexVec = pComb->indexVec(feature1);	   
	   if(fIndexVec) {
	     tmp_fi = const_cast<FeatureIndex*>( &((*fIndexVec)[0]) );
	   }
	 }
	 const FeatureIndex* fIndex1 = tmp_fi;
	 //-- feature 2 --//
	 tmp_fi = NULL;
	 tmp_fi = const_cast<FeatureIndex*>( pComb->index(feature2) );
	 if( !tmp_fi ) {
	   const std::vector<FeatureIndex>* fIndexVec2 = pComb->indexVec(feature2);	   
	   if(fIndexVec2) {
	     tmp_fi = const_cast<FeatureIndex*>( &((*fIndexVec2)[0]) );
	   }
	 }
	 const FeatureIndex* fIndex2 = tmp_fi;

	 if (fIndex1 && fIndex2){
	   if ( fIndex1->getStatus() && fIndex2->getStatus() ){
	     int i1 = fIndex1->getIndex();
	     int n1 = var1->size();
	     int i2 = fIndex2->getIndex();
	     int n2 = var2->size();
	     if( i1>=0 && i1<n1 ) {
	       if( i2>=0 && i2<n2 ){
		 varPair.insert( std::pair<T, U>( (*var1)[i1], (*var2)[i2]) );
	       } else if(i2!=-1){
		 std::cout << "Index of " << feature2 << " " <<
		   i2 << " is out of range...(Max:" << n2 << ")" << std::endl;
	       }
	     } else if(i1!=-1){
	       std::cout << "Index of " << feature1 << " " <<
		 i1 << " is out of range...(Max:" << n1 << ")" << std::endl;
	     }
	   }// Active ?
	 }//Index is exist?
       }// Loop For CombLinks(=RoI)
    }// if ChainEntry to check ?
   }// Loop for ChainEntries
   return;
}


template<class T, class U>
void fillVarPair(std::multimap<T, U>& varPair,
		 std::string ChainName,
		 std::vector<ChainEntry>& chains,
		 RoILinks& roi,
		 const std::string& feature1,
		 const std::string& feature2,
		 const T var1[],
		 int size1,
		 const U var2[],
		 int size2){
  varPair.clear();
  std::vector<ChainEntry>::const_iterator pChain;
  for (pChain=chains.begin(); pChain!=chains.end(); ++pChain) {
     if( pChain->getName()==ChainName ) {
       std::vector<CombLinks> Combs = roi.getCombLinks(*pChain);
       std::vector<CombLinks>::const_iterator pComb;
       for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
	 FeatureIndex* tmp_fi = NULL;
	 //----- find FeatureIndex -----// 
	 //-- feature 1 --//
	 tmp_fi = const_cast<FeatureIndex*>( pComb->index(feature1) );
	 if( !tmp_fi ) {
	   const std::vector<FeatureIndex>* fIndexVec = pComb->indexVec(feature1);	   
	   if(fIndexVec) {
	     tmp_fi = const_cast<FeatureIndex*>( &((*fIndexVec)[0]) );
	   }
	 }
	 const FeatureIndex* fIndex1 = tmp_fi;
	 //-- feature 2 --//
	 tmp_fi = NULL;
	 tmp_fi = const_cast<FeatureIndex*>( pComb->index(feature2) );
	 if( !tmp_fi ) {
	   const std::vector<FeatureIndex>* fIndexVec2 = pComb->indexVec(feature2);	   
	   if(fIndexVec2) {
	     tmp_fi = const_cast<FeatureIndex*>( &((*fIndexVec2)[0]) );
	   }
	 }
	 const FeatureIndex* fIndex2 = tmp_fi;
	 
	 if (fIndex1 && fIndex2){
	   if ( fIndex1->getStatus() && fIndex2->getStatus() ){
	     int i1 = fIndex1->getIndex();
	     int n1 = size1;
	     int i2 = fIndex2->getIndex();
	     int n2 = size2;
	     if( i1>=0 && i1<n1 ) {
	       if( i2>=0 && i2<n2 ){
		 varPair.insert( std::pair<T, U>(var1[i1], var2[i2]));
	       } else if(i2!=-1){
		 std::cout << "Index of " << feature2 << " " <<
		   i2 << " is out of range...(Max:" << n2 << ")" << std::endl;
	       }
	     } else if(i1!=-1){
	       std::cout << "Index of " << feature1 << " " <<
		 i1 << " is out of range...(Max:" << n1 << ")" << std::endl;
	     }
	   }// Active ?
	 }//Index is exist?
       }// Loop For CombLinks(=RoI)
     }// if ChainEntry to check ?
  }// Loop for ChainEntries
  return;
}


template<class T>
void fillTH1(TH1* hist, std::vector<T>& var, float rescale=1) {
  if(hist){
    typename std::vector<T>::const_iterator p;
    for( p=var.begin(); p!=var.end(); ++p ){
      hist->Fill( (*p) * rescale );
    }
   }
}
template<class T>
void fillTH1abs(TH1* hist, std::vector<T>& var, float rescale=1) {
  if(hist){
    typename std::vector<T>::const_iterator p;
    for( p=var.begin(); p!=var.end(); ++p ){
	hist->Fill( fabs((*p) * rescale) ) ;
    }
  }
}

template<class T, class U>
void fillTH2(TH2* hist, std::multimap<T, U>& varPair,
  float rescale1=1, float rescale2=1) {
  if(hist){
    typename std::multimap<T, U>::const_iterator p;
    for( p=varPair.begin(); p!=varPair.end(); ++p ){
      hist->Fill( (*p).first * rescale1, (*p).second * rescale2 );
    }
  }
}

template<class T, class U>
void fillTH2abs(TH2* hist, std::multimap<T, U>& varPair,
  float rescale1=1, float rescale2=1) {
  if(hist){
    typename std::multimap<T, U>::const_iterator p;
    for( p=varPair.begin(); p!=varPair.end(); ++p ){
      hist->Fill( fabs((*p).first * rescale1), fabs((*p).second * rescale2) );
    }
  }
}


std::vector<int> getIndex(std::string feature,
			  std::string ChainName,
			  bool ActiveOnly,
			  std::vector<ChainEntry>& chains,
			  RoILinks& roi);

const CombLinks getSameRoI(CombLinks in, RoILinks& roi, int roi_type, std::string L1ROI);

const std::vector<CombLinks> getSameRoI(std::vector<CombLinks>& in,
					RoILinks& roi, int roi_type,
					std::string L1RoI);


#endif // __TMNUtil_h__
