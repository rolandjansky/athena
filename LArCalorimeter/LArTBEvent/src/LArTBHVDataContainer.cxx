/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdio.h>
#include <typeinfo>

#include "LArTBEvent/LArTBHVDataContainer.h"



LArTBHVDataContainer::operator std::string (){
 
 char * stCounter = new char[48] ;
 char * nameOfContainer = new char[48] ;
 
 const char * stname = typeid( *this ).name() ; 
 int lname ; 
 sscanf( stname , "%80d%47s" , &lname , nameOfContainer ) ;
 
 std::string newline( "\n" ) ;    
 std::string hvdataContainerString = nameOfContainer ;
             hvdataContainerString +=   ": content " ;
             hvdataContainerString +=  newline ;
 
 LArTBHVDataContainer::const_iterator it ;
 int counter = 0 ;
 const LArTBHVData * hvdata ;    
 
     for(it = this->begin() ; it != this->end() ; it++ ){ // Loop over Hits
     
       hvdata = *it ;
             
       sprintf( stCounter , "%d" , counter ) ;     
        
       hvdataContainerString += "LArTBHVData[" ;
       hvdataContainerString += stCounter ;
       hvdataContainerString += "] = " ;
       hvdataContainerString += (std::string) (*hvdata) ;
       hvdataContainerString += newline ;
     
       counter ++ ; 
       
     }
     
     sprintf( stCounter , "%d" , counter ) ;
     hvdataContainerString += newline ;
     hvdataContainerString += "Number of LArTBHVData in this container : " ;
     hvdataContainerString += stCounter ;
 
 
 delete[] stCounter ;
 delete[] nameOfContainer ;
 
 return hvdataContainerString ;

}
