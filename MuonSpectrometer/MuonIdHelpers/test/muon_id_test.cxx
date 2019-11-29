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

// for the CSCs, test the geometrical hash to real hash conversion
int check_CSC_hash_conversion(CscIdHelper *idhelp) {
    IdentifierHash hash;
    IdentifierHash geo_hash;
    Identifier id;
    Identifier id2;
    // check channel Identifiers
    std::cout << "checking channel hashes..." << std::endl;
    IdContext context = idhelp->channel_context();
    for (unsigned int i=0; i<idhelp->channel_hash_max(); ++i) {
        // get Identifier from positional hash
        if (idhelp->get_id_fromPosHash(i, id)) {
            std::cout << "ERROR: Failed to retrieve identifier from hash=" << i << std::endl;
            return 1;
        }
        // get geometrical hash from Identifier
        if (idhelp->get_channel_hash(id, geo_hash)) {
            std::cout << "ERROR: Failed to retrieve geometrical identifier hash from identifier=" << id.get_compact() << std::endl;
            return 1;
        }
        // get Identifier from geometrical hash
        if (idhelp->get_id(geo_hash, id2, &context)) {
            std::cout << "ERROR: Failed to retrieve real identifier hash from geometrical identifier hash=" << geo_hash << std::endl;
            return 1;
        }
        if (id.get_compact()!=id2.get_compact()) {
            std::cout << "ERROR: Original Identifier=" << id.get_compact() << " differs from retrieved Identifier=" << id2.get_compact() << std::endl;
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

    RpcIdHelper rpc_id;
    if(rpc_id.initialize_from_dictionary (idd)) {
	    std::cout << "check_muon_decoding - cannot init from rpc dict" << std::endl;
        return 1;
    }

    TgcIdHelper tgc_id;
    if(tgc_id.initialize_from_dictionary (idd)) {
	    std::cout << "check_muon_decoding - cannot init from tgc dict" << std::endl;
        return 1;
    }

    if (hasCSC) {
        CscIdHelper csc_id;
        if(csc_id.initialize_from_dictionary (idd)) {
            std::cout << "check_muon_decoding - cannot init from csc dict" << std::endl;
            return 1;
        }
        if (check_CSC_hash_conversion(&csc_id)) return 1;
    }

    if (hasSTgc) {
        TgcIdHelper sTgc_id;
        if(sTgc_id.initialize_from_dictionary (idd)) {
            std::cout << "check_muon_decoding - cannot init from sTgc dict" << std::endl;
            return 1;
        }
    }

    if (hasMM) {
        MmIdHelper mm_id;
        if(mm_id.initialize_from_dictionary (idd)) {
            std::cout << "check_muon_decoding - cannot init from mm dict" << std::endl;
            return 1;
        }
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

    return 0;
}  

#endif // ROOTCORE
