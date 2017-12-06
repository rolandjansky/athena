/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_MDTROD_ENCODER_H
#define MUONBYTESTREAM_MDTROD_ENCODER_H

#include "MuonRDO/MdtCsm.h"
#include <stdint.h>

#include "ByteStreamData/RawEvent.h"

class MdtIdHelper;

namespace MuonGM {
   class MuonDetectorManager;
}

// #include "MuonByteStream/RPC_Hid2RESrcID.h"

/** This class provides conversion from LArRawChannel to ROD format. 
   * @author H. Ma
   * @version  0-0-1 , Oct 7, 2002

   * Modified, Jan 02, 2003
       Split from LArROD_Decoder.

   * Adapted for Muons by Ketevi A. Assamagan
   * Jan 14 2003, BNL
   * Conversion from RpcFiredChannel, CoinMatrix, Pad to ROD format

   * Adapted for MDTs by Stefano Rosati
   * Mar 02 2003 CERN
   * Conversion from MdtAmtHit and MdtCsm to MROD format
   */

class MdtROD_Encoder
{

public: 

	/** constructor 
	*/
	MdtROD_Encoder(); 


	/** destructor 
	*/
	~MdtROD_Encoder(); 

	/** initialize the map 
	*/

	/** add Mdt Csms to the current list 
	*/ 
        void add(const MdtCsm* csm); 

	/** clear the current csm list
	*/ 
        void clear(); 

	/** convert all Csm in the current list to 
	  a vector of 32bit words
	*/ 

        void fillROD(std::vector<uint32_t>& v) ; 


private: 

	//	RPC_Hid2RESrcID* m_hid2re; 
	std::vector<const MdtCsm*> m_vMdtCsm;
	const MdtIdHelper* m_mdtIdHelper;
	const MuonGM::MuonDetectorManager* m_mdm;
	int m_BMGid;
} ; 

#endif


