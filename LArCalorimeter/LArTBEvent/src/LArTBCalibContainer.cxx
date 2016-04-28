/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdio.h>
#include <typeinfo>

#include "LArTBEvent/LArTBCalibContainer.h"

LArTBCalibContainer::operator std::string (){
 
 char * stCounter = new char[48] ;
 char * nameOfContainer = new char[48] ;
 
 const char * stname = typeid( *this ).name() ; 
 int lname ; 
 sscanf( stname , "%80d%47s" , &lname , nameOfContainer ) ;
 
 std::string newline( "\n" ) ;    
 std::string calibContainerString = nameOfContainer ;
             calibContainerString +=   ": content " ;
             calibContainerString +=  newline ;
 
 LArTBCalibContainer::const_iterator it ;
 int counter = 0 ;
 const LArTBCalib * calib ;    
 
     for(it = this->begin() ; it != this->end() ; it++ ){ 
     
       calib = *it ;
             
       sprintf( stCounter , "%d" , counter ) ;     
        
       calibContainerString += "LArTBCalib[" ;
       calibContainerString += stCounter ;
       calibContainerString += "] = " ;
       calibContainerString += (std::string) (*calib) ;
       calibContainerString += newline ;
     
       counter ++ ; 
       
     }
     
     sprintf( stCounter , "%d" , counter ) ;
     calibContainerString += newline ;
     calibContainerString += "Number of LArTBCalib in this container : " ;
     calibContainerString += stCounter ;
 
 
 delete[] stCounter ;
 delete[] nameOfContainer ;
 
 return calibContainerString ;

}
