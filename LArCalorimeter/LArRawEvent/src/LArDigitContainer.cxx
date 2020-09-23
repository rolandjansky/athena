/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArDigitContainer.h"
#include "AthenaKernel/BaseInfo.h"
#include <stdio.h>
#include <typeinfo>



LArDigitContainer::operator std::string () const {
 
 char * stCounter = new char[48] ;
 char * nameOfContainer = new char[48] ;
 
 const char * stname = typeid( *this ).name() ; 
 int lname ; 
 sscanf( stname , "%80d%47s" , &lname , nameOfContainer ) ;
 
 std::string newline( "\n" ) ;    
 std::string digitContainerString = nameOfContainer ;
             digitContainerString +=   ": content " ;
             digitContainerString +=  newline ;
 
 LArDigitContainer::const_iterator it ;
 int counter = 0 ;
 const LArDigit * digit ;    
 
     for(it = this->begin() ; it != this->end() ; ++it ){ // Loop over Hits
     
       digit = *it ;
             
       sprintf( stCounter , "%d" , counter ) ;     
        
       digitContainerString += "LArDigit[" ;
       digitContainerString += stCounter ;
       digitContainerString += "] = " ;
       digitContainerString += (std::string) (*digit) ;
       digitContainerString += newline ;
     
       counter ++ ; 
       
     }
     
     sprintf( stCounter , "%d" , counter ) ;
     digitContainerString += newline ;
     digitContainerString += "Number of Digits in this container : " ;
     digitContainerString += stCounter ;
 
 
 delete[] stCounter ;
 delete[] nameOfContainer ;
 
 return digitContainerString ;

}

SG_ADD_BASE (LArDigitContainer, DataVector<LArDigit> );
