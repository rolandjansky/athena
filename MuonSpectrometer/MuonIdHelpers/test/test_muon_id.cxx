/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: test_muon_id.cxx 594238 2014-04-25 09:00:53Z krasznaa $

#ifdef ROOTCORE

int main() { return 0; }

#else

#include "IdDictParser/IdDictParser.h"  
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h" 
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "GaudiKernel/System.h" 
#include <iostream> 

typedef Identifier::size_type  size_type ;

static void
check_muon_decoding(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_muon_decoding" << std::endl;

    MdtIdHelper mdt_id;
    if(mdt_id.initialize_from_dictionary (idd)) {
	std::cout << "check_muon_decoding - cannot init from mdt dict" << std::endl;
    }

    CscIdHelper csc_id;
    if(csc_id.initialize_from_dictionary (idd)) {
	std::cout << "check_muon_decoding - cannot init from csc dict" << std::endl;
    }

    MmIdHelper mm_id;
    if(mm_id.initialize_from_dictionary (idd)) {
	std::cout << "check_muon_decoding - cannot init from mm dict" << std::endl;
    }

    RpcIdHelper rpc_id;
    if(rpc_id.initialize_from_dictionary (idd)) {
	std::cout << "check_muon_decoding - cannot init from rpc dict" << std::endl;
    }

    TgcIdHelper tgc_id;
    if(tgc_id.initialize_from_dictionary (idd)) {
	std::cout << "check_muon_decoding - cannot init from tgc dict" << std::endl;
    }

    sTgcIdHelper sTgc_id;
    if(sTgc_id.initialize_from_dictionary (idd)) {
	std::cout << "check_muon_decoding - cannot init from sTgc dict" << std::endl;
    }

    // pixel_id.test_wafer_packing	();

}

/**
 * @brief      Tests the ATLAS Id xml files, i.e. updates of the MuonSpectrometer IdDict files
 *
 * @param[in]  argc  Length of argv
 * @param[in]  argv  Here, the second argument after the name of the executable has to be the ’top-level’ xml file: IdDictParser/ATLAS_IDS.xml
 *
 * @return     Returns 0 if the MuonIdHelpers classes were initialized successfully, otherwise 1.
 */
int main (int argc, char* argv[])  
{  
    if (argc < 2) return (1);  
  

    longlong startOfUserTime    = System::userTime( System::microSec );
    longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
    longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );



    IdDictParser parser;  

    parser.register_external_entity("MuonSpectrometer", "IdDictMuonSpectrometer_R.01.xml");

//    parser.register_external_entity("InnerDetector", "InDetIdDictFiles/IdDictInnerDetector_IBL-01.xml");
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


    check_muon_decoding(idd);

    return 0;  
}  

#endif // ROOTCORE
