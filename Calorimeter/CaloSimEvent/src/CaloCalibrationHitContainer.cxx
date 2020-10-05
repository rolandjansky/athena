/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// 09-Feb-2004 WGS: The following code was duplicated from
// CaloHitContainer.cxx and modified for CaloCalibrationHits.

#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "GaudiKernel/System.h"
#include <cstdio>
#include <typeinfo>

#include <iostream> 
#include <sstream> 
#include <utility>


CaloCalibrationHitContainer::CaloCalibrationHitContainer(std::string collectionName)
: AthenaHitsVector<CaloCalibrationHit>(std::move(collectionName))
{

}

CaloCalibrationHitContainer::~CaloCalibrationHitContainer() 
{

}

CaloCalibrationHitContainer::operator std::string () const
{
 std::string newline( "\n" ) ;    
 std::ostringstream ss;
 ss << System::typeinfoName(typeid(*this));
 ss <<   ": content " ;
 ss << newline ;
 
 CaloCalibrationHitContainer::const_iterator it ;
 int counter = 0 ;
 CaloCalibrationHit * hit ;    
 
     for(it = this->begin() ; it != this->end() ; ++it ){ // Loop over Hits
     
       hit = *it ;
             
       ss << "CaloCalibrationHit[" ;
       ss << counter;
       ss << "] " ;

       ss << " ID = " << std::hex << hit->cellID().get_identifier32().get_compact()
          << std::dec << " ;  ";

       ss << " E=("
          << hit->energy(0) << ","
          << hit->energy(1) << ","
          << hit->energy(2) << ","
          << hit->energy(3) << ") MeV";

       ss << newline ;
     
       counter ++ ; 
       
     }
     
     ss << newline ;
     ss << "Number of Hits in this container : " ;
     ss << counter;
 
 
 return ss.str() ;
}
