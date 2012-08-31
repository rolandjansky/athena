// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigVarInHlt_h__
#define __TrigVarInHlt_h__
/*
  TrigVarInHlt.h
*/

#include "TriggerMenuNtuple/FeatureIndex.h"
#include "TriggerMenuNtuple/ChainEntry.h"
#include "TriggerMenuNtuple/RoILinks.h"
#include "TH1.h"
#include "TH2.h"
#include <vector>
#include <string>
#include <map>
#include <iostream>


class TrigVarInHlt {
public:
  TrigVarInHlt(std::string chain_name="", bool ActiveOnly=false);
  ~TrigVarInHlt();

  template<class T>
  void fillVar(std::vector<T>& Vars,
	       std::vector<ChainEntry>& chains,
	       RoILinks& roi,
	       const std::string& feature,
	       std::vector<T>* var);

  template<class T>
  void fillVar(std::vector<T>& Vars,
	       std::vector<ChainEntry>& chains,
	       RoILinks& roi,
	       const std::string& feature,
	       const T var[],
	       int size);

  template<class T, class U>
  void fillVarPair(std::multimap<T, U>& varPair,
		   std::vector<ChainEntry>& chains,
		   RoILinks& roi,
		   const std::string& feature1,
		   const std::string& feature2,
		   std::vector<T>* var1,
		   std::vector<U>* var2);

  template<class T, class U>
  void fillVarPair(std::multimap<T, U>& varPair,
		   std::vector<ChainEntry>& chains,
		   RoILinks& roi,
		   const std::string& feature1,
		   const std::string& feature2,
		   const T var1[],
		   int size1,
		   const U var2[],
		   int size2);


  template<class T>
  void fillTH1(TH1* hist, std::vector<T>& var, float rescale=1.0);

  template<class T>
  void fillTH1abs(TH1* hist, std::vector<T>& var, float rescale=1.0);

  template<class T, class U>
  void fillTH2(TH2* hist, std::multimap<T, U>& varPair,
	       float rescale1=1.0, float rescale2=1.0);
  template<class T, class U>
  void fillTH2abs(TH2* hist, std::multimap<T, U>& varPair,
	       float rescale1=1.0, float rescale2=1.0);

protected:
  std::string mChainName;
  bool mActiveOnly;
};


template<class T>
void TrigVarInHlt::fillVar(std::vector<T>& Vars, 
			   std::vector<ChainEntry>& chains,
			   RoILinks& roi,
			   const std::string& feature,
			   std::vector<T>* var) {
  Vars.clear();
  std::vector<ChainEntry>::const_iterator pChain;
  for (pChain=chains.begin(); pChain!=chains.end(); ++pChain) {
    if( pChain->getName()==mChainName ) {
      std::vector<CombLinks> Combs = roi.getCombLinks(*pChain);
      std::vector<CombLinks>::const_iterator pComb;
      for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
	const FeatureIndex* fIndex = pComb->index(feature);
	const std::vector<FeatureIndex>* fIndexVec = pComb->indexVec(feature);
	if (fIndex){// Case 1: featureContainer is not a vector
	  if ( !mActiveOnly ||
	       ( mActiveOnly && fIndex->getStatus()) ){
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
	  if ( !mActiveOnly ||
	       ( mActiveOnly && Index->getStatus()) ){
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
void TrigVarInHlt::fillVar(std::vector<T>& Vars, 
			   std::vector<ChainEntry>& chains,
			   RoILinks& roi,
			   const std::string& feature,
			   const T var[],
			   int size) {
  Vars.clear();
  std::vector<ChainEntry>::const_iterator pChain;
  for (pChain=chains.begin(); pChain!=chains.end(); ++pChain) {
    if( pChain->getName()==mChainName ) {
      std::vector<CombLinks> Combs = roi.getCombLinks(*pChain);
      std::vector<CombLinks>::const_iterator pComb;
      for(pComb=Combs.begin(); pComb!=Combs.end(); ++pComb) {
	const FeatureIndex* fIndex = pComb->index(feature);
	const std::vector<FeatureIndex>* fIndexVec = pComb->indexVec(feature);
	if (fIndex){// Case 1: featureContainer is not a vector
	  if ( !mActiveOnly ||
	       ( mActiveOnly && fIndex->getStatus()) ){
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
	  if ( !mActiveOnly ||
	       ( mActiveOnly && Index->getStatus()) ){
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

 
template<class T, class U>
void TrigVarInHlt::fillVarPair(std::multimap<T, U>& varPair,
			       std::vector<ChainEntry>& chains,
			       RoILinks& roi,
			       const std::string& feature1,
			       const std::string& feature2,
			       std::vector<T>* var1,
			       std::vector<U>* var2){
   varPair.clear();
   std::vector<ChainEntry>::const_iterator pChain;
   for (pChain=chains.begin(); pChain!=chains.end(); ++pChain) {
     if( pChain->getName()==mChainName ) {
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
void TrigVarInHlt::fillVarPair(std::multimap<T, U>& varPair,
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
     if( pChain->getName()==mChainName ) {
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
void TrigVarInHlt::fillTH1(TH1* hist, std::vector<T>& var, float rescale) {
  if(hist){
    typename std::vector<T>::const_iterator p;
    for( p=var.begin(); p!=var.end(); ++p ){
      hist->Fill( (*p) * rescale );
    }
   }
}
template<class T>
void TrigVarInHlt::fillTH1abs(TH1* hist, std::vector<T>& var, float rescale) {
  if(hist){
    typename std::vector<T>::const_iterator p;
    for( p=var.begin(); p!=var.end(); ++p ){
	hist->Fill( fabs((*p) * rescale) ) ;
    }
  }
}

template<class T, class U>
void TrigVarInHlt::fillTH2(TH2* hist, std::multimap<T, U>& varPair,
  float rescale1, float rescale2) {
  if(hist){
    typename std::multimap<T, U>::const_iterator p;
    for( p=varPair.begin(); p!=varPair.end(); ++p ){
      hist->Fill( (*p).first * rescale1, (*p).second * rescale2 );
    }
  }
}

template<class T, class U>
void TrigVarInHlt::fillTH2abs(TH2* hist, std::multimap<T, U>& varPair,
  float rescale1, float rescale2) {
  if(hist){
    typename std::multimap<T, U>::const_iterator p;
    for( p=varPair.begin(); p!=varPair.end(); ++p ){
      hist->Fill( fabs((*p).first * rescale1), fabs((*p).second * rescale2) );
    }
  }
}


#endif // __TrigVarInHlt_h__
