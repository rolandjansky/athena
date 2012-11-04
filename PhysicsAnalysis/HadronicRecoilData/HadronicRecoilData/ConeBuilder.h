/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HADRONICRECOILDATA_CONEBUILDER_H
#define HADRONICRECOILDATA_CONEBUILDER_H

#include "HadronicRecoilData/Cone.h"
#include "HadronicRecoilData/ConeContainer.h"
#include "HadronicRecoilData/EnergyCone.h"
//#include "HadronicRecoilData/EnergyConeContainer.h"
#include "HadronicRecoilData/EnergyList.h"
#include "HadronicRecoilData/EnergyListContainer.h"
#include "HadronicRecoilData/TransverseEnergy.h"
#include "HadronicRecoilData/General.h"
#include "TRandom3.h"

namespace HadronicRecoil {
  
  /** Returns true if the cone overlaps with other cones in the container */
  bool CheckOverlap( const ConeContainer* coneVec, double eta, double phi, double radius);
  
  
  /** Returns true if the "rough recoil" and the cone center are closer than Dphi */
  bool CheckOverlap( const TransverseEnergy& roughRecoil, double phi, double Dphi=0.6);

  /** Returns a cone randomly generated at a fixed eta,
      with a certain radius and away from the "rough recoil" */
  Cone* MakeRndCone( double eta,
		     double radius,
		     const ConeContainer* coneCont,
		     const TransverseEnergy& roughRecoil,
		     UInt_t seed=1234567890 );
  
/*   /\** Returns a EnergyList that is the merging of all the lists in a certain EnergyListContainer. */
/*       The pointer is owned by the user, which has to take care of destroying it. */
/*   *\/ */
/*   template<class T> EnergyList<T>* MergeLists( EnergyListContainer<T>* listCont) */
/*   { */
/*     // create a merged list */
/*     EnergyList<T>* mergedList = new EnergyList<T>(); */
      
/*     for(unsigned int i=0; i< listCont->size(); ++i) */
/*       { */
/* 	EnergyList<T>* eneList = listCont->at(i); */
	  
/* 	for(unsigned int j=0; j< eneList->Size() ; ++j ){ */
	  
/* 	  mergedList->Add( eneList->At(j) ) ; */
/* 	} */
/*       } */
      
/*     EnergyList<T>* cleanList = RemoveDuplicates( mergedList ); */
/*     delete mergedList; */
    
/*     return cleanList; */
/*   } */
  
/*   /\** Returns a EnergyList that is the merging of all the lists in a certain EnergyListContainer. */
/*       The pointer is owned by the user, which has to take care of destroying it. */
/*   *\/ */
/*   template<class T> EnergyList<T>* MergeLists( EnergyConeContainer<T>* listCont) */
/*   { */
/*       // create a merged list */
/*     EnergyList<T>* mergedList = new EnergyList<T>(); */
    
/*     for(unsigned int i=0; i< listCont->size(); ++i) */
/*       { */
/* 	EnergyCone<T>* eneList = listCont->at(i); */
	
/* 	for(unsigned int j=0; j< eneList->Size() ; ++j ){ */
	  
/* 	  mergedList->Add( eneList->At(j) ) ; */
/* 	} */
/*       } */
    
/*     EnergyList<T>* cleanList = RemoveDuplicates( mergedList ); */
/*     delete mergedList; */
    
/*     return cleanList; */
/*   } */
  
/*   /\** Remove duplicates from a list *\/ */
/*    template<class T> EnergyList<T>* RemoveDuplicates( EnergyList<T>* eneList ) */
/*   { */
/*     EnergyList<T>* cleanList = new EnergyList<T>(); */
    
/*     bool hasDupl = false; */
    
/*     for(unsigned int i=0; i< eneList->Size() ; ++i ){ */
/*       hasDupl = false; */
      
/*       for(unsigned int j=i+1; j< eneList->Size(); ++j ){ */
	
/* 	if ( eneList->At(i)==eneList->At(j) ) hasDupl = true; */
/*       } */
      
/*       if( !hasDupl ) cleanList->Add( eneList->At(i) ); */
/*     } */
    
/*     return cleanList; */
/*   } */
}

#endif
