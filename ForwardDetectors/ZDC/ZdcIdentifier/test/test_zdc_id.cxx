/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /build/atlas/cvs/atlas/offline/InnerDetector/InDetDetDescr/InDetIdentifier/test/test_indet_id.cxx,v 1.11 2009-01-06 17:37:49 schaffer Exp $ 
  
#include "IdDictParser/IdDictParser.h"  
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h" 
#include "ZdcIdentifier/ZdcID.h"
#include "GaudiKernel/System.h" 
#include <iostream> 
 
typedef Identifier::size_type  size_type ;



static void
check_zdc_decoding(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_zdc_decoding" << std::endl;
    std::cout << "=========>  check_zdc_decoding" << std::endl;

    ZdcID zdc_id;

    if(zdc_id.initialize_from_dictionary (idd)) {
	std::cout << "check_zdc_decoding - cannot init from dict" << std::endl;
    }


    zdc_id.test_packing	();

}


int main (int argc, char* argv[])  
{  
    if (argc < 2) return (1);  
  

    longlong startOfUserTime    = System::userTime( System::microSec );
    longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
    longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );



    IdDictParser parser;  
  
//    parser.register_external_entity("LArCalorimeter", "IdDictLArCalorimeter_H8_2004.xml");
//    parser.register_external_entity("InnerDetector", "IdDictInnerDetector_slhc.xml");
//    parser.register_external_entity("InnerDetector", "IdDictInnerDetector_Cosmic.xml");
//    parser.register_external_entity("InnerDetector", "IdDictInnerDetector_CTB2004.xml");

    IdDictMgr& idd = parser.parse (argv[1]);  
//    IdDictMgr& idd = parser.parse (argv[1], "initial_layout");  


    std::cout << "Timing for parsing dictionaries " << std::endl;
    
    /// following lines could be platform dependent!
    longlong deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    ///
    longlong deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    ///
    longlong deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "tries, user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;




    std::cout << "got dict mgr " << std::endl;
//      std::cout << "regenerate for tag = initial_layout " << std::endl;
//      idd.generate_implementation ("initial_layout");  


    check_zdc_decoding(idd);
     
    return 0;  
}  
  
 
 
 
 
 
