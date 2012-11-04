/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HADRONICRECOILDATA_ENERGYHELPER_H
#define HADRONICRECOILDATA_ENERGYHELPER_H

#include "HadronicRecoilData/EnergyList.h"
#include "HadronicRecoilData/EnergyListContainer.h"
#include "HadronicRecoilData/TransverseEnergy.h"

namespace HadronicRecoil {

  /** Returns a EnergyList that is the merging of all the lists in a certain EnergyListContainer.
      The pointer is owned by the user, which has to take care of destroying it.
  */
  template<class T> EnergyList<T> MergeLists( EnergyListContainer<T>& listCont)
  {
    // create a merged list
    EnergyList<T> mergedList;
      
    for(unsigned int i=0; i< listCont.size(); ++i)
      {
	EnergyList<T> eneList = listCont.at(i);
	  
	for(unsigned int j=0; j< eneList.Size() ; ++j ){
	  
	  mergedList.AddElement( eneList.At(j) ) ;
	}
      }
      
    EnergyList<T> cleanList = RemoveDuplicates( mergedList );
    
    return cleanList;
  }
  /** Remove duplicates from a list */
   template<class T> EnergyList<T> RemoveDuplicates( EnergyList<T>& eneList )
  {
    EnergyList<T> cleanList;
    
    bool hasDupl = false;
    
    for(unsigned int i=0; i< eneList.Size() ; ++i ){
      hasDupl = false;
      
      for(unsigned int j=i+1; j< eneList.Size(); ++j ){
	
	if ( eneList.At(i)==eneList.At(j) ) hasDupl = true;
      }
      
      if( !hasDupl ) cleanList.AddElement( eneList.At(i) );
    }
    
    return cleanList;
  }

  
  /** Returns a EnergyList that is the merging of all the lists in a certain EnergyListContainer.
      The pointer is owned by the user, which has to take care of destroying it.
  */
  template<class T> EnergyList<T> MergeLists( EnergyConeContainer<T>& listCont)
  {
      // create a merged list
    EnergyList<T> mergedList;
    
    for(unsigned int i=0; i< listCont.size(); ++i)
      {
	EnergyCone<T>* eneList = listCont.at(i);
	
	for(unsigned int j=0; j< eneList->Size() ; ++j ){
	  
	  mergedList.AddElement( eneList->At(j) ) ;
	}
      }
    
    EnergyList<T> cleanList = RemoveDuplicates( mergedList );
    
    return cleanList;
  }
  

   /** Returns a EnergyList that is formed with all the lements of the container.
       The pointer is owned by the user, which has to take care of destroying it. */
   template<class T> EnergyList<T> SumAll( const DataVector<T>& container )
     {
       EnergyList<T> list;
      
       for(unsigned int i=0; i < container.size(); ++i)
	 {
	   list.AddElement( container.at(i) );
	 }
      
      return list;
     }
}
#endif
