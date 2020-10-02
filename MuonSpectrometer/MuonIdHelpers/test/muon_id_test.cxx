/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ROOTCORE

int main() { return 0; }

#else

#include "IdDictParser/IdDictParser.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "GaudiKernel/System.h" 
#include <iostream> 

int check_hash_retrieval(MuonIdHelper *idhelp) {
    IdentifierHash hash;
    Identifier id;
    IdContext context = idhelp->channel_context();
    for (unsigned int i=0; i<idhelp->channel_hash_max(); ++i) {
        if (idhelp->get_id(i, id, &context)) {
            std::cout << "ERROR: Failed to retrieve identifier from hash=" << i << std::endl;
            return 1;
        }
        if (idhelp->get_channel_hash(id, hash)) {
            std::cout << "ERROR: Failed to retrieve identifier hash from identifier=" << id.get_compact() << std::endl;
            return 1;
        }
        if (hash!=i) {
            std::cout << "ERROR: Identifier hash from identifier=" << id.get_compact() << " (" << hash << ") differs from original hash=" << i << std::endl;
            return 1;
        }
    }
    return 0;
}

// for the CSCs, also test the geometrical hash to real hash conversion
int check_CSC_hash_conversion(CscIdHelper *idhelp) {
    IdentifierHash hash;
    IdentifierHash geo_hash;
    Identifier id;
    // check detectorElement Identifiers
    std::cout << "checking detector element hashes..." << std::endl;
    IdContext context = idhelp->detectorElement_context();
    for (unsigned int i=0; i<idhelp->detectorElement_hash_max(); ++i) {
        if (idhelp->get_id(i, id, &context)) {
            std::cout << "ERROR: Failed to retrieve identifier from hash=" << i << std::endl;
            return 1;
        }
        if (idhelp->get_geo_detectorElement_hash(id, geo_hash)) {
            std::cout << "ERROR: Failed to retrieve geometrical identifier hash from identifier=" << id.get_compact() << std::endl;
            return 1;
        }
        if (idhelp->get_hash_fromGeoHash(geo_hash, hash, &context)) {
            std::cout << "ERROR: Failed to retrieve real identifier hash from geometrical identifier hash=" << geo_hash << std::endl;
            return 1;
        }
        if (hash!=i) {
            std::cout << "ERROR: Identifier hash from identifier=" << id.get_compact() << " (" << hash << ") differs from original hash=" << i << std::endl;
            return 1;
        }
    }
    // check module Identifiers
    std::cout << "checking module hashes..." << std::endl;
    context = idhelp->module_context();
    for (unsigned int i=0; i<idhelp->module_hash_max(); ++i) {
        if (idhelp->get_id(i, id, &context)) {
            std::cout << "ERROR: Failed to retrieve identifier from hash=" << i << std::endl;
            return 1;
        }
        if (idhelp->get_geo_module_hash(id, geo_hash)) {
            std::cout << "ERROR: Failed to retrieve geometrical identifier hash from identifier=" << id.get_compact() << std::endl;
            return 1;
        }
        if (idhelp->get_hash_fromGeoHash(geo_hash, hash, &context)) {
            std::cout << "ERROR: Failed to retrieve real identifier hash from geometrical identifier hash=" << geo_hash << std::endl;
            return 1;
        }
        if (hash!=i) {
            std::cout << "ERROR: Identifier hash from identifier=" << id.get_compact() << " (" << hash << ") differs from original hash=" << i << std::endl;
            return 1;
        }
    }
    // check channel Identifiers
    std::cout << "checking channel hashes..." << std::endl;
    context = idhelp->channel_context();
    for (unsigned int i=0; i<idhelp->channel_hash_max(); ++i) {
        if (idhelp->get_id(i, id, &context)) {
            std::cout << "ERROR: Failed to retrieve identifier from hash=" << i << std::endl;
            return 1;
        }
        if (idhelp->get_geo_channel_hash(id, geo_hash)) {
            std::cout << "ERROR: Failed to retrieve geometrical identifier hash from identifier=" << id.get_compact() << std::endl;
            return 1;
        }
        if (idhelp->get_hash_fromGeoHash(geo_hash, hash, &context)) {
            std::cout << "ERROR: Failed to retrieve real identifier hash from geometrical identifier hash=" << geo_hash << std::endl;
            return 1;
        }
        if (hash!=i) {
            std::cout << "ERROR: Identifier hash from identifier=" << id.get_compact() << " (" << hash << ") differs from original hash=" << i << std::endl;
            return 1;
        }
    }
    return 0;
}

int check_muon_decoding(IdDictMgr& idd, bool hasCSC, bool hasSTgc, bool hasMM)
{

    MdtIdHelper mdt_id;
    if(mdt_id.initialize_from_dictionary (idd)) {
	    std::cout << "check_muon_decoding - cannot init from mdt dict" << std::endl;
        return 1;
    }
    if (check_hash_retrieval(&mdt_id)) return 1;

    RpcIdHelper rpc_id;
    if(rpc_id.initialize_from_dictionary (idd)) {
	    std::cout << "check_muon_decoding - cannot init from rpc dict" << std::endl;
        return 1;
    }
    if (check_hash_retrieval(&rpc_id)) return 1;

    TgcIdHelper tgc_id;
    if(tgc_id.initialize_from_dictionary (idd)) {
	    std::cout << "check_muon_decoding - cannot init from tgc dict" << std::endl;
        return 1;
    }
    if (check_hash_retrieval(&tgc_id)) return 1;

    if (hasCSC) {
        CscIdHelper csc_id;
        if(csc_id.initialize_from_dictionary (idd)) {
            std::cout << "check_muon_decoding - cannot init from csc dict" << std::endl;
            return 1;
        }
        if (check_hash_retrieval(&csc_id)) return 1;
        if (check_CSC_hash_conversion(&csc_id)) return 1;
    }

    if (hasSTgc) {
        TgcIdHelper sTgc_id;
        if(sTgc_id.initialize_from_dictionary (idd)) {
            std::cout << "check_muon_decoding - cannot init from sTgc dict" << std::endl;
            return 1;
        }
        if (check_hash_retrieval(&sTgc_id)) return 1;
    }

    if (hasMM) {
        MmIdHelper mm_id;
        if(mm_id.initialize_from_dictionary (idd)) {
            std::cout << "check_muon_decoding - cannot init from mm dict" << std::endl;
            return 1;
        }
        if (check_hash_retrieval(&mm_id)) return 1;
    }

    return 0;

}

int checkDictFile(std::string filename, bool hasCSC=true, bool hasSTgc=true, bool hasMM=true) {
    std::cout << "=========>  checking dictionnary file=" << filename << std::endl;
    IdDictParser parser;  
    parser.register_external_entity("MuonSpectrometer", filename);
    IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");  
    return check_muon_decoding(idd, hasCSC, hasSTgc, hasMM);
}

/**
 * @brief      Tests the MuonSpectrometer IdDict files
 *
 * @return     Returns 0 if the MuonIdHelpers classes were initialized successfully and retrieval of channel identifiers and hashes work, otherwise 1.
 */
int main ()
{    
    // check Run 2 layout (no STgc/MM)
    if (checkDictFile("IdDictMuonSpectrometer_R.03.xml", true, false, false)) return 1;
    // check asymmetric Run 3 layout
    if (checkDictFile("IdDictMuonSpectrometer_R.09.02.Asym.xml")) return 1;
    // check symmetric Run 3 layout (no CSC)
    if (checkDictFile("IdDictMuonSpectrometer_R.09.02.xml", false, true, true)) return 1;

    return 0;
}  

#endif // ROOTCORE
