/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTROD_DECODER_H
#define MUONBYTESTREAM_MDTROD_DECODER_H

#include "MdtRODReadOut.h"
#include "MdtCsmReadOut.h"
#include "MdtAmtReadOut.h"
#include "MdtHptdcReadOut.h"

#include "MuonRDO/MdtCsmContainer.h"
#include "MuonRDO/MdtCsm.h"
#include "MuonRDO/MdtAmtHit.h"
#include "MuonIdHelpers/MdtIdHelper.h"

#include "MDT_Hid2RESrcID.h"
#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"

#include <stdint.h>
#include <map>
#include <vector>
#include <cassert>

#include "StoreGate/StoreGateSvc.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "eformat/SourceIdentifier.h"
#include "eformat/Version.h"
#include "ByteStreamData/RawEvent.h"

#include "Identifier/Identifier.h"



//using eformat::helper::SourceIdentifier; 

typedef std::map <uint16_t, MdtAmtHit*, std::less<uint16_t> > leading_amt_map;

class MdtROD_Decoder :public AthAlgTool
{

public: 

        /** constructor 
        */

        MdtROD_Decoder(const std::string& type, const std::string& name,
        const IInterface* parent ) ;

        /** destructor 
        */
        virtual ~MdtROD_Decoder(); 

        /** AlgTool InterfaceID
        */
        static const InterfaceID& interfaceID( ) ;

        virtual StatusCode initialize();
        virtual StatusCode finalize();

        /** TODO Add documentation. What is a MDT_Hid2RESrcID?*/
        MDT_Hid2RESrcID* getHid2RE() {return m_hid2re;}

        /** Convert ROD Data words into MDr RDOs.
        @param robFrag Input. Take this and decode it, and fill rdoIDC
        @param rdoIDC  Output. Will be filled with data (and MUST exist before calling this function)*/ 
        //void fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
	//		    MdtCsmContainer& rdoIDC);

        StatusCode fillCollections(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
			    MdtCsmContainer& rdoIDC);

        int specialROBNumber() {return m_specialROBNumber;}
        
        MdtCsm* getCollection (MdtCsmContainer& rdoIdc, Identifier ident);

private:

        StoreGateSvc *m_EvtStore;
        MDT_Hid2RESrcID* m_hid2re;
        MuonMDT_CablingSvc* m_cabling;

        const MdtIdHelper* m_mdtIdHelper;

        /** TMP special ROB number for sector13 runs*/
        int m_specialROBNumber;

        // Methods for the decoding
        MdtRODReadOut * m_rodReadOut;
        MdtCsmReadOut * m_csmReadOut;
        MdtAmtReadOut * m_amtReadOut;
        MdtHptdcReadOut * m_hptdcReadOut;
        
        bool m_BMEpresent;
        bool m_BMGpresent;
        int m_BMEid;
        int m_BMGid;
        
	//        bool    m_debug; //!< If true, output debugging information
        //MsgStream       m_log;
}; 

/*
inline void MdtROD_Decoder::fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
					   MdtCsmContainer& rdoIDC)
{
  std::vector<unsigned int> ChambersToDecode;

  this->fillCollection (robFrag, rdoIDC, ChambersToDecode);
}
*/
#endif



