/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSimEvent/LArHitContainer.h"
#include <typeinfo>
#include <stdio.h>

#include <iostream> 

LArHitContainer::LArHitContainer(const std::string& collectionName)
: AthenaHitsVector<LArHit>(collectionName)
{

}

LArHitContainer::~LArHitContainer() 
{

}

LArHitContainer::operator std::string () const {
 
 char * stCounter = new char[48] ;
 char * nameOfContainer = new char[48] ;
 
 const char * stname = typeid( *this ).name() ; 
 int lname ; 
 sscanf( stname , "%80d%47s" , &lname , nameOfContainer ) ;
 
 std::string newline( "\n" ) ;    
 std::string hitContainerString = nameOfContainer ;
             hitContainerString +=   ": content " ;
             hitContainerString +=  newline ;
 
 LArHitContainer::const_iterator it ;
 int counter = 0 ;
 LArHit * hit ;    
 
     for(it = this->begin() ; it != this->end() ; ++it ){ // Loop over Hits
     
       hit = *it ;
             
       sprintf( stCounter , "%d" , counter ) ;     
        
       hitContainerString += "LArHit[" ;
       hitContainerString += stCounter ;
       hitContainerString += "] " ;

       sprintf( stCounter , " ID = %x ;  " ,hit->cellID().get_identifier32().get_compact() ) ;
       hitContainerString += stCounter ;

       sprintf( stCounter , " E= %f MeV ;  " , hit->energy() ) ;     
       hitContainerString += stCounter ;
       sprintf( stCounter , " t= %f ns ; " , hit->time() ) ;     
       hitContainerString += stCounter ;

//        hitContainerString += (std::string) (*hit) ;
       hitContainerString += newline ;
     
       counter ++ ; 
       
     }
     
     sprintf( stCounter , "%d" , counter ) ;
     hitContainerString += newline ;
     hitContainerString += "Number of Hits in this container : " ;
     hitContainerString += stCounter ;
 
 
 delete[] stCounter ;
 delete[] nameOfContainer ;
 
 return hitContainerString ;

}
