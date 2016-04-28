/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdio.h>
#include <typeinfo>

#include "LArTBEvent/LArTBBeamChambersCont.h"



LArTBBeamChambersCont::operator std::string (){
 
 char * stCounter = new char[48] ;
 char * nameOfContainer = new char[48] ;
 
 const char * stname = typeid( *this ).name() ; 
 int lname ; 
 sscanf( stname , "%80d%47s" , &lname , nameOfContainer ) ;
 
 std::string newline( "\n" ) ;    
 std::string beamchambersContString = nameOfContainer ;
             beamchambersContString +=   ": content " ;
             beamchambersContString +=  newline ;
 
 LArTBBeamChambersCont::const_iterator it ;
 int counter = 0 ;
 const LArTBBeamChambers * beamch ;    
 
     for(it = this->begin() ; it != this->end() ; it++ ){ 
     
       beamch = *it ;
             
       sprintf( stCounter , "%d" , counter ) ;     
        
       beamchambersContString += "LArTBBeamChambers[" ;
       beamchambersContString += stCounter ;
       beamchambersContString += "] = " ;
       beamchambersContString += (std::string) (*beamch) ;
       beamchambersContString += newline ;
     
       counter ++ ; 
       
     }
     
     sprintf( stCounter , "%d" , counter ) ;
     beamchambersContString += newline ;
     beamchambersContString += "Number of LArTBBeamChambers in this container : " ;
     beamchambersContString += stCounter ;
 
 
 delete[] stCounter ;
 delete[] nameOfContainer ;
 
 return beamchambersContString ;

}
