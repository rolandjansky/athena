/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_RPCROD_ENCODER_H
#define MUONBYTESTREAM_RPCROD_ENCODER_H

#include <stdint.h>

#include "AthenaKernel/MsgStreamMember.h"

#include "TrigT1RPChardware/MatrixReadOutStructure.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcFiredChannel.h"

#include "ByteStreamData/RawEvent.h"

#include "RPC_Hid2RESrcID.h"

#include "CxxUtils/checker_macros.h"

/** This class provides conversion from BS to ROD format. 
   * @author H. Ma
   * @version  0-0-1 , Oct 7, 2002

   * Modified, Jan 02, 2003
       Split from LArROD_Decoder.

   * Adapted for Muons by Ketevi A. Assamagan
   * Jan 14 2003, BNL
   * Conversion from RpcFiredChannel, CoinMatrix, Pad to ROD format
   */

class RpcROD_Encoder
{

public: 

	/** constructor 
	*/
	RpcROD_Encoder(); 


	/** destructor 
	*/
	~RpcROD_Encoder(); 

	/** initialize the map 
	*/
	void set(RPC_Hid2RESrcID* hid2re) ;

	/** add Rpc pads to the current list 
	*/ 
        void add(const RpcPad* rc); 

	/** clear the current pad list
	*/ 
        void clear(); 

	/** convert all pad in the current list to 
	  a vector of 32bit words
	*/ 

        void fillROD(std::vector<uint32_t>& v) ; 

	MsgStream & msg (MSG::Level lvl) const {return m_msg << lvl;};
	bool msgLevel (MSG::Level lvl) {return m_msg.get().level() <= lvl;};

private:
	void packFragments(const std::vector<uint16_t>& v16, 
			   std::vector<uint32_t>& v,
			   int n) const;

	uint32_t set32bits(const unsigned short int * v16, 
			   const unsigned short int * pos,
			   const unsigned short int n) const;

private: 

	RPC_Hid2RESrcID* m_hid2re; 
	std::vector<const RpcPad*> m_vRpcPad;  


	mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;
} ; 

#endif
