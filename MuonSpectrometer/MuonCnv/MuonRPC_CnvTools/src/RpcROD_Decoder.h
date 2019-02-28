/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_RPCROD_DECODER_H
#define MUONBYTESTREAM_RPCROD_DECODER_H

#include <stdint.h>
#include <cassert>

#include "TrigT1RPChardware/RPCRODDecode.h"
#include "TrigT1RPChardware/RPCRXRODDecode.h"
#include "TrigT1RPChardware/RPCRODStructure.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"

#include "MuonRPC_CnvTools/IRpcROD_Decoder.h"

#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcPad.h"
#include "RPCcablingInterface/RpcPadIdHash.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcSectorLogicContainer.h"

#include "MuonIdHelpers/RpcIdHelper.h"

#include "eformat/Issue.h"
#include "eformat/SourceIdentifier.h"

#include "StoreGate/StoreGateSvc.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/GaudiException.h"

// start preparing for BS errors 
//#include "MuonByteStreamErrors/RpcByteStreamErrorContainer.h"
//#include "RpcByteStreamAccess/IRPC_ByteStreamErrorSvc.h"

// #include "minibench.h"
class StoreGateSvc;


namespace Muon
{
  
  inline bool ensure_more_data(int index, int size, MsgStream& log, bool& printMessage, const std::string& message) {
    if ( index >= size ) {
      if (printMessage && log.level() <= MSG::WARNING) {
        log << MSG::WARNING << "Unexpected end of RPC data: " << message << endmsg;
        printMessage = false;
      }
      return false;
    }
    return true;
  }
  
  
  class RpcROD_Decoder :virtual public IRpcROD_Decoder,  public AthAlgTool 
  {
    
  public: 
    
    RpcROD_Decoder(const std::string& type, 
                   const std::string& name,
                   const IInterface* p) ;
    
    virtual ~RpcROD_Decoder(); 
    
    
    
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
    
    // implementation of the abstract interface
    StatusCode fillCollections(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag, 
                               RpcPadContainer& rdoIdc,
                               const std::vector<IdentifierHash> &collections, RpcSectorLogicContainer*) const override;
    
    int specialROBNumber() const {return m_specialROBNumber;}
    
    bool isSector13Data() const {return m_sector13Data;}
    virtual void setSLdecodingRequest() override {m_decodeSL=true;} 
    
    
  private:
    
    //niko
    bool m_decodeSL; 
    //ServiceHandle<IRPC_ByteStreamErrorSvc> m_byteStreamErrSvc;

    typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType BS;
    
    

    StatusCode fillCollection_v240(BS data, const uint32_t data_size, RpcPad& v) const;
    
    StatusCode fillCollection_v300(BS data, const uint32_t data_size, RpcPad& v,
                                   const uint16_t& subDetector, RpcSectorLogicContainer*) const;
    
    StatusCode fillCollection_v301(BS data, const uint32_t data_size, RpcPad& v,
                                   const uint16_t& subDetector, RpcSectorLogicContainer*) const;
    
    // decoding of real data - 2010 & 2011 _v302 
    StatusCode fillCollection_v302new(BS data, const uint32_t data_size, RpcPad& v,
                                   const uint32_t& sourceId, RpcSectorLogicContainer* ) const;
    StatusCode fillCollection_v302(BS data, const uint32_t data_size, RpcPad& v,
                                   const uint32_t& sourceId,RpcSectorLogicContainer* ) const;
    // decoding of real data - 2010 & 2011 _v302 
    StatusCode fillCollectionsFromRob_v302(BS data, const uint32_t data_size, std::map<Identifier,RpcPad*>& vmap,
				      const uint32_t& sourceId, RpcSectorLogicContainer* ) const;
    
    
    // fragment each of the 32 bit words into 2 16 bit words!
    inline std::vector<uint16_t> get16bits(BS data, 
                                           const int size,
                                           const int nHeader,
                                           const int nFooter) const;
    
    inline std::vector<uint16_t> get16bits_v301(BS data, 
                                                const int size,
                                                const int nHeader,
                                                const int nFooter) const;
    
    //====LBTAG==== Added 02112008 for buffer format check
    mutable int m_previous;
    mutable int m_printerror;
    mutable bool m_RPCcheckform[13];
    mutable int  m_RPCcheckfail[13];
    IntegerProperty m_maxprinterror;
    
    //====LBTAG==== Added 02112008 for buffer format check
    StatusCode checkdataformat(std::vector<uint16_t>*, int, int) const;
    void printcheckformat() const;
    
  private:
    
    //RpcPadIdHash*                      m_hashfunc;
    //ServiceHandle<StoreGateSvc>        m_storeGate;
    const IRPCcablingSvc*              m_cabling;
    const RpcIdHelper*                 m_pRpcIdHelper;
    
    // re-define the ROB number
    IntegerProperty m_specialROBNumber;
    // flag to read old sector 13 data
    BooleanProperty m_sector13Data;
    // mutable MiniBenchInternals::Bench m_bench;

    //RpcByteStreamErrorContainer *m_bsErrCont;
    //std::string m_bsErrContainerName; 
  }; 
  
  inline StatusCode Muon::RpcROD_Decoder::checkdataformat(std::vector<uint16_t>* pdata, int ini, int end) const
  {
  
  // ===================================  
  // Check ROB fragment structure
  // ===================================  
  
    
    int  decoded;
    MatrixReadOut matrix;
    StatusCode sc = StatusCode::SUCCESS;
    
    //initialize check vector 
    for (int i=0; i<13; i++)m_RPCcheckform[i]=false;
    
    //Loop on buffer 
    for (int i=ini; i < end; i++){          
      decoded = ((*pdata)[i] & 0xf000)>> 12 ;
      
      //RX Header
      if(decoded==0x9){
        if(m_previous == 0){
          m_RPCcheckform[0]=true;
          m_previous = 1;
        }
        else {
          ++m_RPCcheckfail[0]; 
          m_previous=0;
	  //m_bsErrCont->addError((*pdata)[i],Muon::RXHeaderErrors);
          sc = StatusCode::FAILURE;
          return sc;
        }
      }
      
      // RX SubHeader
      else if(decoded==0xe){
        if(m_previous == 1){
          m_previous = 2;
          m_RPCcheckform[1]=true;
        }
        else {
          ++m_RPCcheckfail[1]; 
          m_previous=0;
          sc = StatusCode::FAILURE;
          return sc;
        }
      }
      //PAD Header
      else if(decoded==0x5){
        if(m_previous == 2){
          m_previous = 3;
          m_RPCcheckform[2]=true;
        }
        else {
          ++m_RPCcheckfail[2];
          m_previous=0;
          sc = StatusCode::FAILURE;
          return sc;
        }
      }
      
      //PAD or SL Subheader
      else if(decoded==0x6){
        if(m_previous == 3){
          m_previous = 4;
          m_RPCcheckform[3]=true;
        }
        else if(m_previous == 8){
          m_previous = 9;
          m_RPCcheckform[10]=true;
        }
        else {
          ++m_RPCcheckfail[3];
          m_previous=0;
          sc = StatusCode::FAILURE;
          return sc;
        }
      }
      
      //CM Header
      else if(decoded==0xc){
        if(m_previous == 4){
          m_previous = 5;
          m_RPCcheckform[4]=true;
        }
        else {
          ++m_RPCcheckfail[4];
          m_previous=0;
          sc = StatusCode::FAILURE;
          return sc;
        }
      }
      
      //CM Subheader
      else if(decoded==0x8){
        if(m_previous == 5){
          m_previous=6;
          m_RPCcheckform[5]=true;
        }
        else {
          ++m_RPCcheckfail[5];
          m_previous=0;
          sc = StatusCode::FAILURE;
          return sc;
        }
      }
      
      //CM Footer
      else if(decoded==0x4){
        if(m_previous == -1 || m_previous == 6){
          m_previous=4;
          m_RPCcheckform[6]=true;
          if(matrix.checkCRC8((ubit16)(*pdata)[i])){
            m_RPCcheckform[12]=true;
          }
          else {
            ++m_RPCcheckfail[12];
            sc = StatusCode::RECOVERABLE;
          }
        }
        else {
          ++m_RPCcheckfail[6];
          m_previous=0;
          sc = StatusCode::FAILURE;
          return sc;
        }
      }
      
      //PAD Prefooter
      else if(decoded==0xa){
        if(m_previous == 4){
          m_previous=7;
          m_RPCcheckform[7]=true;
        }
        else {
          ++m_RPCcheckfail[7];
          m_previous=0;
          sc = StatusCode::FAILURE;
          return sc;
        }
      }
      
      //PAD Footer
      else if(decoded==0x7){
        if(m_previous == 7){
          m_previous=2;
          m_RPCcheckform[8]=true;
        }
        else {
          ++m_RPCcheckfail[8];
          m_previous=0;
          sc = StatusCode::FAILURE;
          return sc;
        }
      }
      
      //SL Header
      else if(decoded==0xd){
        if(m_previous == 2){
          m_previous=8;
          m_RPCcheckform[9]=true;
        }
        else {
          ++m_RPCcheckfail[9];
          m_previous=0;
          sc = StatusCode::FAILURE;
          return sc;
        }
      }
      
      //SL Footer
      else if(decoded==0xf){
        if(m_previous == 9 || m_previous == -1){
          m_previous=10;
          m_RPCcheckform[10]=true;
        }
        else{
          ++m_RPCcheckfail[10];
          m_previous=0;
          sc = StatusCode::FAILURE;
          return sc;
        }
      }
      //RX Footer
      else if(decoded==0xb){
        if(m_previous == 10){
          m_previous=0;
          // ===== end of fragment reached =====
          return sc;
        }
        else{
          ++m_RPCcheckfail[11];
          m_previous=0;
          sc = StatusCode::FAILURE;
          return sc;
        }
      }
      else { m_previous=-1;
      }
    }
    sc = StatusCode::FAILURE;
    return sc;
  }
  
  inline void Muon::RpcROD_Decoder::printcheckformat() const {
    
    // ===================================  
    // Print check ROB fragment statistics
    // ===================================  
    IMessageSvc* msgSvc = 0;
    StatusCode sc = service("MessageSvc", msgSvc);
    if (sc == StatusCode::FAILURE)
      throw GaudiException("RpcROD_Decoder::printcheckformat: MessageSvc not found", name(), sc); 
    MsgStream log(msgSvc, "RpcROD_Decoder::printcheckformat");
    log <<  MSG::INFO << " ============ FINAL RPC DATA FORMAT STAT. =========== " << endmsg;
    log <<  MSG::INFO << " RX Header Errors............." << m_RPCcheckfail[0] << endmsg;
    log <<  MSG::INFO << " RX SubHeader Errors.........." << m_RPCcheckfail[1] << endmsg;
    log <<  MSG::INFO << " PAD Header Errors............" << m_RPCcheckfail[2] << endmsg;
    log <<  MSG::INFO << " PAD/SL SubHeader Errors......" << m_RPCcheckfail[3] << endmsg;
    log <<  MSG::INFO << " CM Header Errors............." << m_RPCcheckfail[4] << endmsg;
    log <<  MSG::INFO << " CM SubHeader Errors.........." << m_RPCcheckfail[5] << endmsg;
    log <<  MSG::INFO << " CM Footer Errors............." << m_RPCcheckfail[6] << endmsg;
    log <<  MSG::INFO << " PAD PreFooter Errors........." << m_RPCcheckfail[7] << endmsg;
    log <<  MSG::INFO << " PAD Footer Errors............" << m_RPCcheckfail[8] << endmsg;
    log <<  MSG::INFO << " SL Header Errors............." << m_RPCcheckfail[9] << endmsg;
    log <<  MSG::INFO << " SL Footer Errors............." << m_RPCcheckfail[10] << endmsg;
    log <<  MSG::INFO << " RX Footer Errors............." << m_RPCcheckfail[11] << endmsg;
    log <<  MSG::INFO << " CRC8 check Failures.........." << m_RPCcheckfail[12] << endmsg;
    log <<  MSG::INFO << " ==================================================== " << endmsg;
    
  }
  
  inline StatusCode 
  RpcROD_Decoder::fillCollections(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag, 
                                  RpcPadContainer& rdoIdc,
                                  const std::vector<IdentifierHash> &collections, RpcSectorLogicContainer* RPC_SECTORLOGIC) const
  {

    try 
    {
      robFrag.check ();
    }
    catch (eformat::Issue &ex)
    {
      
      ATH_MSG_VERBOSE(ex.what());
      return StatusCode::FAILURE;  // error in fragment
    }
    
    
    // get the pointer to the data
    BS data;
    robFrag.rod_data(data);
    
    
    // here decide the version of the decoding to be called
    uint32_t version = robFrag.rod_version(); 
    uint32_t sourceId= robFrag.source_id();
    uint32_t rod_sourceId= robFrag.rod_source_id();
    uint16_t subDetector=(sourceId & 0xff0000)>>16;
    
    ATH_MSG_VERBOSE("ROD version: "   << MSG::hex << version 
      << MSG::dec << "  ROB source ID: " << MSG::hex << sourceId 
      << MSG::dec << "  ROD source ID: " << MSG::hex << rod_sourceId 
      << MSG::dec << "  Subdetector: "   << MSG::hex << subDetector 
      << MSG::dec);
    
    
    // chose the right decoding routine
    bool isSimulated = (data[0]==0xee1234ee)? true : false;
    int type = 0;
    
    if ( ((version&0x03000000) == 0x03000000) && 
        (data[2] != 0x00)                    && 
        ((data[0]&0xffff0000) == 0) ) 
    {
      type = 1;
      ATH_MSG_VERBOSE("choosing fillCollection_v300");
    }
    else if (version == 0x2400000 || isSimulated) 
    {
      type = 0;
      ATH_MSG_VERBOSE("choosing fillCollection_v240");
    }
    else if ( ( (version&0x03000000) == 0x03000000) && 
             ( (data[0]&0xffff0000) != 0)          && 
             ( m_sector13Data ) ) 
    {
      type = 2;
      ATH_MSG_VERBOSE("choosing fillCollection_v301");
    }
    else if ( ( (version&0x03000000) == 0x03000000) && 
	      ( (data[0]&0xffff0000) != 0) ) // this is the current data format - 2011 May 13
    {
      type = 3;
      ATH_MSG_VERBOSE("choosing fillCollection_v302");
    }
    
    
    
    // for the time being use the old decoding schema .... to be optimized
    
    
    StatusCode cnv_sc;

    /*
    // unpack the 32 bits words into 16 bits 
    // no ROD header and footer 
    std::vector<uint16_t> p = get16bits_v301(data,robFrag.rod_ndata(),0,0);
    
    // two RX fragment in the data vector: look for the RX HEADERS
    int krx=0;
    if( ((p[0] & 0xf000) >> 12)==0x9 ){
      for(unsigned int kid=1; kid<p.size(); ++kid){        
        if( ((p[kid] & 0xf000) >> 12)==0x9 ) {krx=kid; break;}
      }
    }
    
    if(krx != 0){
      
      //== LBTAG == Check buffer format for first RX frag.
      StatusCode cdf = checkdataformat(&p,0,krx);  
      if( cdf.isFailure() && m_printerror <= m_maxprinterror) {
        if (cdf == StatusCode::FAILURE)ATH_MSG_VERBOSE("RPC RX fragment format error --> Event is recoverable, no action");
        if (cdf == StatusCode::RECOVERABLE)ATH_MSG_VERBOSE("RPC CRC transmission error --> Event is recoverable, no action");
        if( m_printerror == m_maxprinterror )ATH_MSG_VERBOSE(" Limit for RPC data format error reached. No more warnings for this error "); 
        ++m_printerror;
      }
      
      //== LBTAG == Check buffer format for second RX frag.
      cdf = checkdataformat(&p,krx,p.size());  
      if( cdf.isFailure() && m_printerror <= m_maxprinterror) {
        if (cdf == StatusCode::FAILURE)ATH_MSG_VERBOSE("RPC RX fragment format error --> Event is recoverable, no action");
        if (cdf == StatusCode::RECOVERABLE)ATH_MSG_VERBOSE("RPC CRC transmission error --> Event is recoverable, no action");
        if( m_printerror == m_maxprinterror )ATH_MSG_VERBOSE(" Limit for RPC fragment error reached. No more warnings for this error "); 
        ++m_printerror;
      }
    }
    else {
      if( ((p[0] & 0xf000) >> 12)==0x9 && ((p[p.size()-1] & 0xf000) >> 12)==0xb){
        StatusCode cdf = checkdataformat(&p,0,p.size());  
        if( cdf.isFailure() && m_printerror <= m_maxprinterror) {
          if (cdf == StatusCode::FAILURE)ATH_MSG_VERBOSE("RPC RX fragment format error --> Event is recoverable, no action");
          if (cdf == StatusCode::RECOVERABLE)ATH_MSG_VERBOSE("RPC CRC transmission error --> Event is recoverable, no action");
          if( m_printerror == m_maxprinterror )ATH_MSG_VERBOSE(" Limit for RPC fragment error reached. No more warnings for this error "); 
          ++m_printerror;
        }
      }
    }
    */

    // here optimize decoding of ROB fragment (for data only type==3)
    if (type==3)
    {
      std::map<Identifier,RpcPad*> mapOfCollections; 
      std::vector<IdentifierHash>::const_iterator it = collections.begin();
      for( ; it!=collections.end(); ++it)
      {

        // Using RpcPadCache
        RpcPadContainer::IDC_WriteHandle lock = rdoIdc.getWriteHandle( (*it) );

        if(lock.alreadyPresent() ){
          ATH_MSG_DEBUG ( "RPC RDO collection already exist with collection hash = " << static_cast<unsigned int>(*it) << " converting is skipped!");
        }
        else{
          ATH_MSG_VERBOSE(" Created new Pad Collection Hash ID = " << static_cast<unsigned int>(*it) );

		      // create new collection - I should be doing this with unique_ptr but it requires changing downstream functions
          RpcPad* coll = new RpcPad((m_cabling->padHashFunction())->identifier(*it), *it);
          mapOfCollections[coll->identify()]=coll;
	      }// endif collection not found in the container 
	    }//end loop over vector of hash id 

      if (mapOfCollections.empty()) {
        ATH_MSG_VERBOSE("mapOfCollections is empty; fillCollectionsFromRob_v302 will not be called");
        cnv_sc = StatusCode::SUCCESS;
        return cnv_sc;
      }

      // RpcPadCollections not decoded and in container are identified and passed explicitly to decoder
      cnv_sc = fillCollectionsFromRob_v302(data,robFrag.rod_ndata(),mapOfCollections,rod_sourceId, RPC_SECTORLOGIC);
      if (cnv_sc!=StatusCode::SUCCESS)
      {
	    if (cnv_sc==StatusCode::RECOVERABLE) 
	      {
		      ATH_MSG_DEBUG("Decoding errors found ");
	      }
	    else 
        return cnv_sc; // exit if failure 
	    }

      // All un-decoded collections were decoded successfully, so they are passed back to the IDC
	    //for (std::map<Identifier,RpcPad*>::const_iterator it=mapOfCollections.begin(); it!=mapOfCollections.end(); ++it)
      for (std::map<Identifier,RpcPad*>::const_iterator it=mapOfCollections.begin(); it!=mapOfCollections.end(); ++it)
	    {
	      // add collection into IDC using RpcPadCache functions
        
        RpcPadContainer::IDC_WriteHandle lock = rdoIdc.getWriteHandle( ((*it).second)->identifyHash() );

	      if ( !lock.alreadyPresent() )
	      {
          // Take the pointer and pass ownership to unique_ptr and pass to the IDC_WriteHandle
          StatusCode status_lock = lock.addOrDelete( std::unique_ptr<RpcPad>( (*it).second ) );

		      if(status_lock != StatusCode::SUCCESS)
		      {
		        ATH_MSG_ERROR("Failed to add RPC PAD collection to container" );
		        //report the error condition
		      }
          else 
            ATH_MSG_DEBUG("Adding RpcPad collection with hash "<<(int)((*it).second)->identifyHash()<<" to the RpcPad Container | size = "<<((*it).second)->size());
        }
      }
      return cnv_sc;
    }//endif (type==3)
    
    std::vector<IdentifierHash>::const_iterator it = collections.begin();
    for( ; it!=collections.end(); ++it)
    {
      RpcPadContainer::const_iterator itColl = rdoIdc.indexFind(*it);
      if (itColl == rdoIdc.end())
      {
        msg(MSG::VERBOSE) << " Created new Pad Collection Hash ID = " << static_cast<unsigned int>(*it) << endmsg;
        
        // create new collection
        RpcPad* coll = new RpcPad((m_cabling->padHashFunction())->identifier(*it), *it);
        
        //convert collection - note case3 will never be used due to statement above
        switch(type)
        {
          case 0: cnv_sc = fillCollection_v240(data,robFrag.rod_ndata(),*coll); break;
          case 1: cnv_sc = fillCollection_v300(data,robFrag.rod_ndata(),*coll,subDetector, RPC_SECTORLOGIC); break;
          case 2: cnv_sc = fillCollection_v301(data,robFrag.rod_ndata(),*coll,subDetector, RPC_SECTORLOGIC ); break;
          case 3: cnv_sc = fillCollection_v302(data,robFrag.rod_ndata(),*coll,sourceId, RPC_SECTORLOGIC); break;
	        //case 3: cnv_sc = fillCollection_v302new(data,robFrag.rod_ndata(),*coll,sourceId); break;

          default: fillCollection_v240(data,robFrag.rod_ndata(),*coll); break;
        }
        
        if( cnv_sc.isFailure() )
        {
          ATH_MSG_VERBOSE("Error into the RPC fillCollections decoding");
        }
        
        // add collection into IDC
        if (rdoIdc.addCollection(coll, *it).isFailure())
        {
          msg(MSG::ERROR) << "Failed to add RPC PAD collection to container" << endmsg;
          //report the error condition
        }
        else
          ATH_MSG_DEBUG("Adding RpcPad collection with hash "<<(int)(*it)<<" to the RpcPad Container | size = "<<coll->size());
      }
    }
    return cnv_sc;
  } // end fillCollections
  
  
  // ----  Implement the template method: 
  /**   fill RpcPads from a block of integers 
   New version for data format 3.1 (ATLAS cosmics - NEW RPC READOUT)
   */ 
  
  inline StatusCode
  RpcROD_Decoder::fillCollection_v302(BS data, const uint32_t data_size,
                                      RpcPad& v, const uint32_t& sourceId, RpcSectorLogicContainer* sectorLogicContainer ) const 
  {

    /* for (unsigned int i = 0; i<1000; ++i) { */
    /*   //std::cout<<" aaa "<<std::endl; */
    /*   msg(MSG::VERBOSE) << "try to increase cpu time "<<log(pow(((double)i+1.)/999.,3))<<endmsg; */
    /* } */

    // m_bench.point(1);
    // unpack the 32 bits words into 16 bits 
    // no ROD header and footer 
    std::vector<uint16_t> p = get16bits_v301(data,data_size,0,0);
    
    const int size = p.size();
    
    ATH_MSG_VERBOSE("**********Decoder v302 dumping the words******** ");
////  notice that instructions in between   #ifndef NVERBOSE are executed in normal jobs with standard (INFO) output level !!!!
/* #ifndef NVERBOSE */
/*     std::cout<<" printing something for testing"<<std::endl; */
/* #endif */

    if (size > 0  && msgLvl(MSG::VERBOSE) ) {
      msg(MSG::VERBOSE) << "The size of this ROD-read is " << size << endmsg;
      int  decoded             ;
      char decoded_char[1000]  ;
      for (int i=0; i < size; i++){     
        
        decoded = (p[i] & 0xf000)>> 12 ;
        if(decoded<0x4 )sprintf(decoded_char,"Hit data"       );
        if(decoded==0x4)sprintf(decoded_char,"CM Footer"      );
        if(decoded==0x5)sprintf(decoded_char,"PAD Header"     );
        if(decoded==0x6)sprintf(decoded_char,"PAD/SL Subheader"  );
        if(decoded==0x7)sprintf(decoded_char,"PAD Footer"     );
        if(decoded==0x8)sprintf(decoded_char,"CM Subheader"   );
        if(decoded==0x9)sprintf(decoded_char,"RX Header"      );
        if(decoded==0xa)sprintf(decoded_char,"PAD Prefooter"  );
        if(decoded==0xb)sprintf(decoded_char,"RX Footer"      );
        if(decoded==0xc)sprintf(decoded_char,"CM Header"      );
        if(decoded==0xd)sprintf(decoded_char,"SL Header"      );
        if(decoded==0xe)sprintf(decoded_char,"RX Subheader");
        if(decoded==0xf)sprintf(decoded_char,"SL Footer"      );
        
        msg(MSG::VERBOSE) << "word " << i <<" = "<< MSG::hex << p[i] << MSG::dec 
        << " " << MSG::hex << decoded << MSG::dec << " " << decoded_char << endmsg;
      }
    }
    
    
    if (size == 0) {
      ATH_MSG_VERBOSE(" Buffer size 0 ! ");
      return StatusCode::FAILURE;
    }
    
    
    
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "The source ID is: " << MSG::hex << sourceId << endmsg;
    
    
    uint16_t sectorForCabling = 0;
    uint16_t sector = 0;
    
    // counter of the numerb of sector logic fragments found
    uint16_t SLindex = 0;
    
    // the identifier of this collection 
    Identifier thisPadOfflineId = v.identify();
    
    // Identifier of the collection in data
    Identifier padOfflineId;
    bool foundPad=false;
    
    
    if (msgLvl(MSG::VERBOSE ) )
      msg(MSG::VERBOSE) << "The offline ID request for conversion is "
      << m_pRpcIdHelper->show_to_string(thisPadOfflineId) << endmsg;
    
    bool isSLHeader    =false;
    bool isSLSubHeader =false;
    bool isSLFooter    =false;
    bool isSLFragment  =false;
    bool isRXHeader    =false;
    bool isRXFooter    =false;
    bool isPADFragment =false;
    bool isPadHeader   =false;
    bool isPadSubHeader=false;
    bool isPadPreFooter=false;
    bool isPadFooter   =false;
    
    //return;
    
    RPCRXRODDecode myRPC;
    RXReadOutStructure  RXROS;
    PadReadOutStructure PDROS;
    SectorLogicRXReadOutStructure SLROS;
    MatrixReadOutStructure matrixROS;
    RPCRODStructure rodReadout;
    
    char recField;
    unsigned short int PadID=99;
    uint16_t slfel1id=0;
    uint16_t slid=0;
    uint16_t slbcid=0;
    uint16_t slstatus=0;
    uint16_t slcrc=0;
    unsigned int SLBodyWords = 0;
    unsigned int SL_data_sise = 500; //same value used for the size of SLBuff
    unsigned short int SLBuff[500];
    
    // decode the source Id and set side and sector number
    rodReadout.decodeSourceID(sourceId);
    uint16_t subDetectorID = (sourceId & 0xff0000)>>16;
    
    uint16_t side  = (subDetectorID == eformat::MUON_RPC_BARREL_A_SIDE) ? 1:0; 
    uint16_t rodId =  rodReadout.getSourceIDRODID();
    if (msgLvl(MSG::VERBOSE) )
    {
      
      msg(MSG::VERBOSE) << "subDetectorID = 0x" 
      << MSG::hex << subDetectorID << MSG::dec 
      << endmsg;
      
      msg(MSG::VERBOSE) << "rodID = 0x" << MSG::hex
      << rodId << MSG::dec << endmsg;
      msg(MSG::VERBOSE) << "The side is " << side 
      << endmsg;
    }
    
    //RpcSectorLogic* sl;
    
    for (uint16_t i = 0; i < size; ++i) 
    {
      
      //std::cout << "REGISTER: " << i << std::endl;
      
      isRXHeader    =false;
      isRXFooter    =false;
      isPadHeader   =false;
      isPadSubHeader=false;
      isPadPreFooter=false;
      isPadFooter   =false;
      isSLHeader    =false;
      isSLSubHeader    =false;
      isSLFooter    =false;
      uint32_t currentWord = p[i];
      
      
      RXROS.decodeFragment(currentWord,recField);
      PDROS.decodeFragment(currentWord,recField);
      SLROS.decodeFragment(currentWord,recField);
      
      if (RXROS.isHeader()) {
        isRXHeader=true;
        isSLFragment=false;
        isPADFragment=false;
      } else if (PDROS.isSubHeader() && !isSLFragment){
        isPadSubHeader=true;        
      } else if (RXROS.isFooter())   {
        isRXFooter=true;
      } else if (PDROS.isHeader())   {
        isPadHeader=true;
        isSLFragment=false;
      } else if (PDROS.isPreFooter()){
        isPadPreFooter=true;
      } else if (PDROS.isFooter() )  {
        isPadFooter=true;
      } else if (SLROS.isHeader())   {
        isSLHeader=true;
        isSLFragment=true;
        isPADFragment=false;
      } else if (SLROS.isFooter())   {
        isSLFooter=true;
        isSLFragment=false;
      } else if (SLROS.isSubHeader() && isSLFragment) {
        isSLSubHeader=true;
      }
      
      
      if (msgLvl(MSG::VERBOSE) ) {
        char decoded_char[256] ;
        if (isRXHeader) {
          sprintf(decoded_char," RX Header" );    
        }else if (isRXFooter) {
          sprintf(decoded_char," RX Footer" );
        }else if (isSLHeader) {
          sprintf(decoded_char," SL Header" );
        }else if (isSLSubHeader){
          sprintf(decoded_char," SL SubHeader" );
        }else if (isSLFooter){
          sprintf(decoded_char," SL Footer" );
        }else if (isPadHeader) {
          sprintf(decoded_char," Pad Header" );
        }else if (isPadSubHeader) {
          sprintf(decoded_char," Pad SubHeader" );
        }else if (isPadPreFooter) {
          sprintf(decoded_char," Pad PreFooter" );
        }else if (isPadFooter) {
          sprintf(decoded_char," Pad Footer" );    
        }else if (isSLFragment)  {
          sprintf(decoded_char," SL Fragment" );
        }else if (isPADFragment) {
          sprintf(decoded_char," Pad Fragment" );    
        }else{
          sprintf(decoded_char," Undecoded" );    
        }
        
        
        msg(MSG::VERBOSE)  << i << " -->current data word is " 
        << MSG::hex << currentWord<< MSG::dec
        << decoded_char << endmsg;
        
      }
      if(isRXHeader) { 
        if (msgLvl(MSG::VERBOSE) )
        {
          msg(MSG::VERBOSE)  << " this is a RX Header " << endmsg;
          msg(MSG::VERBOSE) <<" Sector ID="    <<RXROS.RXid()<<endmsg;
        }
        
        // get the sector id according to the new format
        // not yet implemented in the readout classes
        
        //uint16_t rxid = (currentWord & 0x800) >> 11;
        
        uint16_t rxid = RXROS.RXid();
        sectorForCabling = 2*rodId+rxid;
        sector = side*32+sectorForCabling;
        
        //fix for M3 
        if ((rodId==3 || rodId==1) && (m_specialROBNumber > 0) ) {
          sector=39;
          sectorForCabling=7;
          //fix for M3 with runnumber up to 11533 (SFI)
          if(m_specialROBNumber == 0x650001){
            sector=40;
            sectorForCabling=8;
          }
        }
        else if ( (rodId==4 || rodId==2) && (m_specialROBNumber > 0)) {
          sector=40;
          sectorForCabling=8;
        }     
        
      } else if(isRXFooter) { 
        ATH_MSG_VERBOSE(" this is a RX Footer ");
      } else if(isSLHeader || isSLFragment || isSLSubHeader || isSLFooter)  {
        
        // push only the lowest 16 bits
        int foundSL = myRPC.pushWord(currentWord,0);
        
        if(isSLHeader) { 
          SLBodyWords=0;
          slfel1id = SLROS.fel1id();
          slid = SLROS.slid();
          
          ATH_MSG_VERBOSE(" SL Header: slfel1id " << slfel1id 
			  << " slid: " << slid);
        }
        else if (isSLSubHeader) {
          slbcid = SLROS.slbcid();
          
          ATH_MSG_VERBOSE("SL subheader: slbcid: "
			  << slbcid);
          
        }
        // Decode the sector logic footer
        else if(isSLFooter ) {
          
          
          if (SLindex>1) {
            ATH_MSG_VERBOSE("More than 2 SL fragments in sector " << sector);
          }
          
          
          if (msgLvl(MSG::VERBOSE) )
          {
            
            msg(MSG::VERBOSE) 
            << " Number of data words in SectorLogicReadOut= "
            << SLBodyWords << endmsg;  
            msg(MSG::VERBOSE) 
            << " TEST SL: "<< foundSL << endmsg;
            
            // Print out a raw dump of the SL fragment 
            for(unsigned short j=0; j<SLBodyWords; j++) {
              msg(MSG::VERBOSE) << " SL data word " << j << " : " << MSG::hex
              << SLBuff[j] << MSG::dec << endmsg;
            }
          }
          
          // Found the sector logic footer, the sector logic fragment 
          // can be added to the sector logic container
          
          SectorLogicRXReadOut* sectorLogic = myRPC.SLFragment();
          
          if (sectorLogicContainer && 
              !sectorLogicContainer->findSector(sector,side) ) {
            
            slstatus = SLROS.status();
            slcrc = SLROS.crc();
            // fill the hit content for the first sector
            RpcSectorLogic* sl =
            new RpcSectorLogic(sector,slfel1id,slbcid,slstatus,slcrc);
            bool inputHeaderFound = false;
            bool outputHeaderFound = false;
            ATH_MSG_VERBOSE("New RpcSectorLogic: sector=" << sector
            << " fel1id=" << slfel1id
			    << " BCID=" <<slbcid);
            
            uint16_t rowinBcid = 999;
            uint16_t slPadId   = 999;
            
            for (int islwords = 0 ; islwords<sectorLogic->numberOfInputWords() ; ++islwords) {
              
              uint16_t ptid;
              uint16_t roi;
              uint16_t outerPlane; 
              uint16_t overlapPhi; 
              uint16_t overlapEta;
              
              uint16_t slWord = sectorLogic->readSLHitCurrent();
              
              ATH_MSG_VERBOSE("SLhit word: " << std::hex << slWord << std::dec);
              SLROS.decodeFragment(slWord,recField);
              
              uint16_t triggerBcid;
              
              if (SLROS.isInputHeader()) {
                rowinBcid = SLROS.inputRowinBcid();
                slPadId = SLROS.inputPadId();
                inputHeaderFound = true;
              }
              else if (SLROS.isInputBody()) {
                if (!inputHeaderFound) {
                  ATH_MSG_VERBOSE("ERROR: inputSLHeader missing !!");
                } else {
                  ptid = SLROS.inputThreshold();
                  roi  = SLROS.inputRoi();
                  outerPlane = SLROS.inputOuterPlane();
                  overlapPhi = SLROS.inputOverlapPhi();
                  overlapEta = SLROS.inputOverlapEta();
                  triggerBcid = SLROS.inputTriggerBcid();
                  
                  RpcSLTriggerHit* slHit =
                  new RpcSLTriggerHit(  rowinBcid,  slPadId,  ptid,  roi,
                                      outerPlane,  overlapPhi,  overlapEta,
                                      triggerBcid);
                  slHit->setIsInput(true);
                  sl->push_back(slHit);
                  ATH_MSG_VERBOSE("New input RpcSLTriggerHit: ptid, roi=  " << ptid << " " << roi);                  
                }
              }
              else if (SLROS.isOutputHeader()) {
                rowinBcid = SLROS.outputRowinBcid();
                outputHeaderFound=true;
              }
              else if (SLROS.isOutputBody()) {
                if (!outputHeaderFound) {
                  rowinBcid = 999;
                  if (msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE) 
                    << "ERROR: outputSLHeader missing !!" << endmsg;
                }
                else {
                  if (SLROS.isOutputDecoded()) {
                    
                    outerPlane=0;
                    overlapPhi=0;
                    
                    for (int icand = 0 ; icand<SLROS.nTriggerCand() ; ++icand) {
                      
                      ptid = SLROS.outputThreshold(icand);
                      triggerBcid = SLROS.outputTriggerBcid(icand);
                      roi = SLROS.outputRoi(icand);
                      overlapEta=SLROS.outputOverlap(icand);
                      RpcSLTriggerHit* slHit =
                      new RpcSLTriggerHit( rowinBcid,  0,  ptid,  roi,
                                          outerPlane,  overlapPhi,  overlapEta,
                                          triggerBcid);
                      slHit->setIsInput(false);
                      // use only one out od the two sectors for the moment (???)
                      sl->push_back(slHit);
                      ATH_MSG_VERBOSE("New output RpcSLTriggerHit: ptid, roi=  " << ptid << " " << roi);
                      
                    } 
                    
                    if (SLROS.hasMoreThan2TriggerCand()) {
                      sl->setHasMoreThan2TriggerCand(true);
                    }
                    
                  }
                }
              }
            }
            
            
            
            
            if(sectorLogicContainer) sectorLogicContainer->push_back(sl);
            
            if (sectorLogicContainer && 
                !sectorLogicContainer->setSector(sector,side) ) {
              ATH_MSG_VERBOSE("Sector " << sector 
			      << " decoded more than once in SL");
            }
            
          }
          
        } else {
          if ( SLBodyWords >= SL_data_sise ) {
            ATH_MSG_VERBOSE("Sector Logic payload corrupted");
            return StatusCode::FAILURE;
          }
          SLBuff[SLBodyWords]=currentWord;
          SLBodyWords++;
        } // isSLHeader
        
      } else if(isPadHeader || isPADFragment)  {
        
        // Now decoding the header of the pad
        ATH_MSG_VERBOSE(" Pad Header or Pad Fragment " );
        
        PDROS.decodeFragment(currentWord,recField);
        
        if(recField == 'H') {
          
          PadID=PDROS.padid();
          
          // special setup for Dec06 sector 13 data
          if (m_sector13Data) {
            if (PadID<3) {
              sector=56;
            }
            else {
              sector=55;
              // move the pad value to the 0-2 range;
              PadID -= 3;
            }
          }
          
          side = (sector<32) ? 0:1;
          uint16_t sectorLogic = sector-side*32;
          
          // get the offline ID of the pad
          if(!m_cabling->giveOffflineID(side,sectorLogic,PadID,padOfflineId))
          {
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << "Cannot retrieve the OfflineID for the PAD n. " 
            << PadID << " at side " << side << " and  sector " 
            << sectorLogic << endmsg;
          } else {
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << "ID " << m_pRpcIdHelper->show_to_string(padOfflineId)
            << " associated to PAD n. " << PadID << " at side " 
            << side << " and  sector " << sectorLogic << endmsg; 
          }
          
          // check if it's the pad to convert
          if (thisPadOfflineId == padOfflineId) {
            
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << " match found with ID " 
            << m_pRpcIdHelper->show_to_string(thisPadOfflineId)
            << " requested for the conversion; return this collection" 
            << endmsg; 
            
            foundPad = true;
            
            v.setOnlineId(PadID);
            v.setSector(sector);
            
            // set the lvl1 id
            v.setLvl1Id(PDROS.l1id());
            
          }
          else
          {
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << " match NOT found with ID "  
            << m_pRpcIdHelper->show_to_string(thisPadOfflineId)
            << " requested for the conversion" << endmsg;  
          }
          
        }
        



        // if it's a subheader, set the bcid
        if (recField == 'S') {
          
          if (foundPad) {
            v.setBcId(PDROS.bcid());
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << "Found the subheader, setting bcid to: " << PDROS.bcid() 
            << endmsg;
          }
          
        }
        
        if (recField == 'P') {
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
          << "Found the prefooter" << endmsg;
          //          v.setStatus(currentWord&0x0fff );
          v.setStatus(PDROS.status());
          
          if ( currentWord&0x0fff ) {
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)
            << "Pad Busy status not zero ! value: " << MSG::hex
            << (currentWord & 0x0fff) << MSG::dec << endmsg;
          }
        }
        
        if(recField == 'F') {
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " Pad Footer " << endmsg;
          v.setErrorCode(PDROS.errorCode());
          if (msgLvl(MSG::VERBOSE) && PDROS.errorCode()!=0) {
            
            msg(MSG::VERBOSE) 
            << "Pad Error flag not zero ! value: " << MSG::hex
            << PDROS.errorCode() << MSG::dec << endmsg;  
          }
          
          // found the pad, bail out
          if (foundPad) {
            foundPad=false; 
            //std::cout << "found pad " << PadID << " sector " << sector << " " << sector%2 << std::endl;
            // M.C. : CONTINUE TO SCAN TILL THE END of the ROB TO FIND LAST SectorLogic:
            if (!(PadID>3&&sector%2>0)){
              return StatusCode::SUCCESS;
            }          
          }
        }
        
        isPadFooter ? isPADFragment=false : isPADFragment=true; 
        
        if (msgLvl(MSG::VERBOSE))
        {          
          msg(MSG::VERBOSE) 
          << " current word "<< MSG::hex << currentWord << MSG::dec << endmsg;
          
          msg(MSG::VERBOSE) 
          << " ==isPADFragment= " << isPADFragment << endmsg;
          msg(MSG::VERBOSE) 
          << " calling pushword: " << MSG::hex << currentWord << MSG::dec << endmsg;
        }
        
        int foundCM = 0;
        foundCM = myRPC.pushWord(currentWord,0);
        
        if(foundCM==1) {
          
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << myRPC.CMFragment() << endmsg;
          
          // If the pad is the good one, add the CMs to the container   
          if (foundPad) {
            MatrixReadOut* matrix = myRPC.CMFragment();
            
            //std::cout << myRPC.CMFragment()<< std::endl;
            
            matrixROS = matrix->getHeader();  
            uint16_t cmaId  = matrixROS.cmid();
            uint16_t fel1id = matrixROS.fel1id();
            
            matrixROS = matrix->getSubHeader();
            uint16_t febcid = matrixROS.febcid();
            
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << "Creating a new CM, cmaId=" << cmaId << " fel1id=" << fel1id
            << " febcid=" << febcid << endmsg;
            
            // Create the new cm
            RpcCoinMatrix * coinMatrix = new RpcCoinMatrix (padOfflineId,cmaId,fel1id,febcid);
            
            matrixROS = matrix->getFooter();
            coinMatrix->setCRC(matrixROS.crc());
            
            //std::cout << matrix->numberOfBodyWords() << std::endl;
            
            // Loop on the hits and push them in the coin matrix
            for (int i=0 ; i<matrix->numberOfBodyWords() ; ++i) {
              
              matrixROS = matrix->getCMAHit(i);
              
              uint16_t bcid = matrixROS.bcid();
              uint16_t time = matrixROS.time();
              uint16_t ijk  = matrixROS.ijk();
              
              RpcFiredChannel* firedChan=0;
              
              if (ijk < 7 ) {
                uint16_t channel = matrixROS.channel();
                firedChan = new RpcFiredChannel(bcid,time,ijk,channel);
                
		ATH_MSG_VERBOSE (
                "Adding a fired channel, bcid=" << bcid << " time=" 
                << " ijk=" << ijk << " channel=" << channel);
                
                // add the fired channel to the matrix
                coinMatrix->push_back(firedChan); 
              }
              else if (ijk==7) {
                uint16_t overlap   = matrixROS.overlap();
                uint16_t threshold = matrixROS.threshold();
                firedChan = new RpcFiredChannel(bcid,time,ijk,threshold,overlap);
                
                ATH_MSG_VERBOSE (
                "Adding a fired channel, bcid=" << bcid << " time=" 
                << " ijk=" << ijk << " overlap=" << overlap 
                << " threshold=" << threshold);
                
                // add the fired channel to the matrix
                coinMatrix->push_back(firedChan);
                
              }
            }
            
            v.push_back(coinMatrix);
            
          } // end of the matrix decoding
          
          (myRPC.CMFragment())->reset();
          
        } // end of the pad decoding
        
      }
      
    }
    ATH_MSG_VERBOSE ("Required Pad NOT FOUND in this ROB");
    // m_bench.point(2);
    return StatusCode::SUCCESS; 
  }
  
  //#include "RpcROD_Decoder_v302.cxx"

  /****** OLD FORMATS FOR SECTOR LOGIC *********/
  /**   fill RpcPads from a block of integers 
   New version for data format 3.1 (ATLAS cosmics - NEW RPC READOUT)
   */ 
  inline StatusCode 
  RpcROD_Decoder::fillCollection_v301(BS data, const uint32_t data_size,
                                      RpcPad& v, const uint16_t& subDetector, RpcSectorLogicContainer* sectorLogicContainer ) const
  {
    
    // unpack the 32 bits words into 16 bits 
    // no ROD header and footer 
    std::vector<uint16_t> p = get16bits_v301(data,data_size,0,0);
    
    const int size = p.size();
    
    //#ifndef NVERBOSE
    ATH_MSG_VERBOSE ("**********Decoder dumping the words******** ");
    if (size > 0 && msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "The size of this ROD-read is " << size << endmsg;
      for (int i=0; i < size; i++)
        msg(MSG::VERBOSE) << "word " << i << " = " << MSG::hex << p[i] << MSG::dec << endmsg;
    }
    //#endif
    
    uint16_t side  = (subDetector == eformat::MUON_RPC_BARREL_A_SIDE) ? 1:0;    
    
    
    // TMP FIXME the sector number needs to be fixed !!
    uint16_t sector = 0;
    
    // counter of the numerb of sector logic fragments found
    uint16_t SLindex = 0;
    
    // the identifier of this collection 
    Identifier thisPadOfflineId = v.identify();
    
    // Identifier of the collection in data
    Identifier padOfflineId;
    bool foundPad=false;
    
    
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "The offline ID request for conversion is " 
    << m_pRpcIdHelper->show_to_string(thisPadOfflineId) << endmsg;
    
    
    bool isSLHeader    =false;
    bool isSLFooter    =false;
    bool isSLFragment  =false;
    bool isRXHeader    =false;
    bool isRXFooter    =false;
    bool isPADFragment =false;
    bool isPadHeader   =false;
    bool isPadSubHeader=false;
    bool isPadPreFooter=false;
    bool isPadFooter   =false;
    
    //return;
    
    RPCRODDecode myRPC;
    RXReadOutStructure  RXROS;
    PadReadOutStructure PDROS;
    SectorLogicReadOutStructure SLROS;
    MatrixReadOutStructure matrixROS;
    
    
    char recField;
    unsigned short int PadID=99;
    unsigned int SLBodyWords=0;
    unsigned int SL_data_sise = 500; //same value used for the size of SLBuff
    unsigned short int SLBuff[500];
    
    //RpcSectorLogic* sl;
    
    for (uint16_t i = 0; i < size; ++i) 
    {
      
      //std::cout << "REGISTER: " << i << std::endl;
      
      isRXHeader    =false;
      isRXFooter    =false;
      isPadHeader   =false;
      isPadSubHeader=false;
      isPadPreFooter=false;
      isPadFooter   =false;
      isSLHeader    =false;
      isSLFooter    =false;
      uint32_t currentWord = p[i];
      
      //#ifndef NVERBOSE
      ATH_MSG_VERBOSE (" -->current data word is "<<std::hex<<currentWord<<std::dec);
      //#endif  
      
      RXROS.decodeFragment(currentWord,recField);
      PDROS.decodeFragment(currentWord,recField);
      SLROS.decodeFragment(currentWord,recField);
      
      if (RXROS.isHeader() && !isSLFragment) isRXHeader=true;
      else if (RXROS.isFooter() && !isSLFragment) isRXFooter=true;
      else if (PDROS.isHeader() && !isSLFragment) isPadHeader=true;
      else if (PDROS.isSubHeader() && !isSLFragment) isPadSubHeader=true;
      else if (PDROS.isPreFooter() && !isSLFragment) isPadPreFooter=true;
      else if (PDROS.isFooter() && !isSLFragment) isPadFooter=true;
      else if (SLROS.isHeader()) isSLHeader=true;
      else if (SLROS.isFooter()) isSLFooter=true;
      
      //#ifndef NVERBOSE
      if (msgLvl(MSG::VERBOSE))
	{
	  msg(MSG::VERBOSE) <<" RX Header: "<<isRXHeader
			    <<" RX Footer: "<<isRXFooter
			    <<endmsg;
	  msg(MSG::VERBOSE) <<" Pad Header: "<<isPadHeader
			    <<" Pad SubHeader: "<<isPadSubHeader
			    <<" Pad PreFooter: "<<isPadPreFooter
			    <<" Pad Footer: "<<isPadFooter
			    <<endmsg;
	  msg(MSG::VERBOSE) <<" isPADFragment: "<<isPADFragment
			    <<endmsg;
	  msg(MSG::VERBOSE) <<" SL Header: "<<isSLHeader
			    <<" SL Footer: "<<isSLFooter
			    <<endmsg;
	  msg(MSG::VERBOSE) <<" isSLFragment: "<<isSLFragment
			    <<endmsg;
	}
      //#endif   
      
      if(isRXHeader) { 
        //#ifndef NVERBOSE
        ATH_MSG_VERBOSE (" this is a RX Header ");
        ATH_MSG_VERBOSE (" Sector ID="<<RXROS.RXid());
        //#endif
        
        // set sector ID
        // sector = RXROS.RXid();
        
      } else if(isRXFooter) { 
        //#ifndef NVERBOSE
        ATH_MSG_VERBOSE (" this is a RX Footer ");
        //#endif
      } else if(isSLHeader || isSLFragment)  {
        
        isSLFooter ? isSLFragment=false : isSLFragment=true;
        
        // push only the lowest 16 bits
        int foundSL = myRPC.pushWord(currentWord,0);
        
        // Check the Sector Logic Fragment      
        if(foundSL && msgLvl(MSG::VERBOSE)) 
	  {
          //#ifndef NVERBOSE
          msg(MSG::VERBOSE) <<"SectorLogicReadOut checkFragment: "
          << myRPC.SLFragment()->checkFragment()<<endmsg;
          msg(MSG::VERBOSE) << myRPC.SLFragment()<<endmsg;
          //#endif
          }
        
        if(isSLHeader) SLBodyWords=0;
        
        // Decode the sector logic footer
        else if(isSLFooter ) {
          
          //#ifndef NVERBOSE
          if (SLindex>1) {
            msg(MSG::ERROR) << "More than 2 SL fragments in sector " << sector << endmsg;
          }
          //#endif
          
	  if (msgLvl(MSG::VERBOSE))
	    {
	      //#ifndef NVERBOSE
	      msg(MSG::VERBOSE) <<" Number of data words in SectorLogicReadOut= "
				<<SLBodyWords<<endmsg;   
	      msg(MSG::VERBOSE) <<" TEST SL: "<<foundSL<<endmsg;
	      //#endif  
          
	      //#ifndef NVERBOSE
	      // Print out a raw dump of the SL fragment 
	      for(unsigned short j=0; j<SLBodyWords; j++) {
		msg(MSG::VERBOSE) <<" SL data word "<<j<<" : "<<std::hex<<SLBuff[j]<<MSG::dec<<endmsg;
	      }
	  }
          //#endif
          
          // Found the sector logic footer, the sector logic fragment 
          // can be added to the sector logic container
          
          SectorLogicReadOut* sectorLogic = myRPC.SLFragment();
          // decode the SL hits and push back in the sector logic
          uint16_t nSLlink = 8;
          uint16_t nSLgate = 7;
          
          if ( sectorLogicContainer                   && 
              !sectorLogicContainer->findSector(56,0) && 
              !sectorLogicContainer->findSector(55,0)   ) {
            // fill the hit content for the first sector
            RpcSectorLogic* sl1 = new RpcSectorLogic(56,0,0,0);
            RpcSectorLogic* sl2 = new RpcSectorLogic(55,0,0,0);
            
            for (int igate = 0 ; igate < nSLgate ; ++igate) {
              
              for (int ilink = 0 ; ilink < nSLlink ; ++ilink) {
                
                // choose the sectorlogic to be updated according to the link number 
                // TMP FIXME !
                RpcSectorLogic* sl;
                if (ilink<3) {
                  sl = sl1;
                }
                else {
                  sl = sl2;
                }
                
                uint16_t ptid  = sectorLogic->ptid(ilink,igate);
                //#ifndef NVERBOSE
                ATH_MSG_VERBOSE ("SL: pt for link=" << ilink 
				 << " gate=" << igate << "  is: " << ptid);
                //#endif
                if (ptid != 0) {  
                  //#ifndef NVERBOSE
                  ATH_MSG_VERBOSE ("SL: found an hit for link=" << ilink
				   << " gate=" << igate);
                  //#endif
                  uint16_t cmadd = sectorLogic->cmadd(ilink,igate);
                  
                  uint16_t bcid  = igate;
                  uint16_t tower = ilink+2*SLindex;
                  
                  uint16_t opl   = sectorLogic->opl(ilink,igate);
                  uint16_t ovphi = sectorLogic->ovphi(ilink,igate);
                  uint16_t oveta = sectorLogic->oveta(ilink,igate);
                  
                  uint16_t triggerBcid  = sectorLogic->bcid(ilink,igate);
                  
                  // create the hit and push back in the sl
                  RpcSLTriggerHit* slHit = new RpcSLTriggerHit(bcid,tower,ptid,cmadd,
                                                               opl,ovphi,oveta,triggerBcid);
                  sl->push_back(slHit);
                  
                }
                
                
              }
              
            }
            
	    if (msgLvl(MSG::VERBOSE)){
            msg(MSG::VERBOSE) << "Size of sector 55: " << sl2->size() << endmsg;
            msg(MSG::VERBOSE) << "Size of sector 56: " << sl1->size() << endmsg;
            }
            // flag the two sectors as decoded
            //bool setSector1 = sectorLogicContainer->setSector(56,0);
            //bool setSector2 = sectorLogicContainer->setSector(55,0);
            
            // add dummy trigger rates
            sl1->addTriggerRate(0.0);
            sl2->addTriggerRate(0.0);
            
            if ( sectorLogicContainer ) sectorLogicContainer->push_back(sl1);
            if ( sectorLogicContainer ) sectorLogicContainer->push_back(sl2);
            
          }
          
        } else {
          if (SLBodyWords>=SL_data_sise) {
            if (msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE)
              << "Sector Logic payload corrupted" << endmsg;
            return StatusCode::FAILURE;
          }
          SLBuff[SLBodyWords]=currentWord;
          SLBodyWords++;
        } // isSLHeader
        
      } else if(isPadHeader || isPADFragment)  {
        
        // Now decoding the header of the pad
        //#ifndef NVERBOSE
        ATH_MSG_VERBOSE (" Pad Header or Pad Fragment ");
        //#endif
        
        PDROS.decodeFragment(currentWord,recField);
        
        if(recField == 'H') {
          
          PadID=PDROS.padid();
          
          if (PadID<3) {
            sector=56;
          }
          else {
            sector=55;
            // move the pad value to the 0-2 range;
            PadID -= 3;
          }
          
          side = (sector<32) ? 0:1;
          uint16_t sectorLogic = sector-side*32;
          
          // get the offline ID of the pad
          if(!m_cabling->giveOffflineID(side,sectorLogic,PadID,padOfflineId))
          {
            if (msgLvl(MSG::VERBOSE) ) 
              msg(MSG::VERBOSE) 
              << "Cannot retrieve the OfflineID for the PAD n. " 
              << PadID << " at side " << side << " and  sector " 
              << sectorLogic << endmsg;
          }
          else 
            if (msgLvl(MSG::VERBOSE) ) 
              msg(MSG::VERBOSE) 
              << "ID " << m_pRpcIdHelper->show_to_string(padOfflineId)
              << " associated to PAD n. " << PadID << " at side " 
              << side << " and  sector " << sectorLogic << endmsg; 
          
          // check if it's the pad to convert
          if (thisPadOfflineId == padOfflineId) {
            
            if (msgLvl(MSG::VERBOSE) ) 
              msg(MSG::VERBOSE) 
              << " match found with ID " 
              << m_pRpcIdHelper->show_to_string(thisPadOfflineId)
              << " requested for the conversion; return this collection" 
              << endmsg; 
            
            foundPad = true;
            
            //#ifndef NVERBOSE
            ATH_MSG_VERBOSE ("Found the pad to convert !");
            //#endif
            v.setOnlineId(PadID);
            v.setSector(sector);
            
            // set the lvl1 id
            v.setLvl1Id(PDROS.l1id());
            
          }
          else
          {
            if (msgLvl(MSG::VERBOSE) ) 
              msg(MSG::VERBOSE) 
              << " match NOT found with ID " 
              << m_pRpcIdHelper->show_to_string(thisPadOfflineId)
              << " requested for the conversion" << endmsg; 
          } 
        }
        
        // if it's a subheader, set the bcid
        if (recField == 'S') {
          
          if (foundPad) {
            v.setBcId(PDROS.bcid());
            //#ifndef NVERBOSE
            ATH_MSG_VERBOSE ("Found the subheader, setting bcid to: " 
			     << PDROS.bcid());
            //#endif
          }
          
        }
        
        if (recField == 'P') {
          //#ifndef NVERBOSE
          ATH_MSG_VERBOSE ("Found the prefooter");
          //#endif  
          //v.setStatus(PDROS.status());
          //if (PDROS.status()!=0) {
          //#ifndef NVERBOSE
          //  msg(MSG::WARNING) << "Pad Busy status not zero ! value: " << MSG::hex
          //      << PDROS.status() << MSG::dec << endmsg;
          //#endif  
          //}
        }
        
        if(recField == 'F') {
          //#ifndef NVERBOSE
          ATH_MSG_VERBOSE (" Pad Footer ");
          //#endif
          v.setErrorCode(PDROS.errorCode());
          if (PDROS.errorCode()!=0) {
            //#ifndef NVERBOSE
            ATH_MSG_VERBOSE ("Pad Error flag not zero ! value: " << MSG::hex
			     << PDROS.errorCode() << MSG::dec);
            //#endif  
          }
          
          // found the pad, bail out
          if (foundPad) {
            foundPad=false;
            return StatusCode::SUCCESS;
          }
        }
        
        isPadFooter ? isPADFragment=false : isPADFragment=true; 
        
        //#ifndef NVERBOSE
	if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) <<" current word "<<std::hex<<currentWord<<MSG::dec<<endmsg;
        
        msg(MSG::VERBOSE) <<" ==isPADFragment= "<<isPADFragment<<endmsg;
        msg(MSG::VERBOSE) <<" calling pushword: "<<std::hex<<currentWord<<MSG::dec<<endmsg;
	}
        //#endif
        
        int foundCM = 0;
        foundCM = myRPC.pushWord(currentWord,0);
        
        if(foundCM==1) {
          //#ifndef NVERBOSE
          ATH_MSG_VERBOSE (myRPC.CMFragment());
          //#endif
          // If the pad is the good one, add the CMs to the container   
          if (foundPad) {
            MatrixReadOut* matrix = myRPC.CMFragment();
            
            //std::cout << myRPC.CMFragment()<< std::endl;
            
            matrixROS = matrix->getHeader();  
            uint16_t cmaId  = matrixROS.cmid();
            uint16_t fel1id = matrixROS.fel1id();
            
            matrixROS = matrix->getSubHeader();
            uint16_t febcid = matrixROS.febcid();
            
            //#ifndef NVERBOSE
            ATH_MSG_VERBOSE ("Creating a new CM, cmaId=" << cmaId << " fel1id=" << fel1id
			     << " febcid=" << febcid);
            //#endif
            
            // Create the new cm
            RpcCoinMatrix * coinMatrix = new RpcCoinMatrix (padOfflineId,cmaId,fel1id,febcid);
            
            //std::cout << matrix->numberOfBodyWords() << std::endl;
            
            // Loop on the hits and push them in the coin matrix
            for (int i=0 ; i<matrix->numberOfBodyWords() ; ++i) {
              
              matrixROS = matrix->getCMAHit(i);
              
              uint16_t bcid = matrixROS.bcid();
              uint16_t time = matrixROS.time();
              uint16_t ijk  = matrixROS.ijk();
              
              RpcFiredChannel* firedChan=0;
              
              if (ijk < 7 ) {
                uint16_t channel = matrixROS.channel();
                firedChan = new RpcFiredChannel(bcid,time,ijk,channel);
                //#ifndef NVERBOSE
                ATH_MSG_VERBOSE ("Adding a fired channel, bcid=" << bcid << " time=" 
				 << " ijk=" << ijk << " channel=" << channel);
                //#endif
                
                // add the fired channel to the matrix
                coinMatrix->push_back(firedChan);    
              }
              else if (ijk==7) {
                uint16_t overlap   = matrixROS.overlap();
                uint16_t threshold = matrixROS.threshold();
                firedChan = new RpcFiredChannel(bcid,time,ijk,threshold,overlap);
                //#ifndef NVERBOSE
                ATH_MSG_VERBOSE ("Adding a fired channel, bcid=" << bcid << " time=" 
				 << " ijk=" << ijk << " overlap=" << overlap 
				 << " threshold=" << threshold);
                //#endif
                
                // add the fired channel to the matrix
                coinMatrix->push_back(firedChan);    
              }   
            }
            
            v.push_back(coinMatrix);
            
          } // end of the matrix decoding
          
          (myRPC.CMFragment())->reset();
          
        } // end of the pad decoding
        
      }
      
    }
    
    return StatusCode::SUCCESS; 
  }
  
  
  /**   fill RpcPads from a block of integers 
   New version for data format 3.0 (ATLAS cosmics)
   */ 
  inline StatusCode
  RpcROD_Decoder::fillCollection_v300(BS data, const uint32_t data_size,
                                      RpcPad& v, const uint16_t& subDetector, RpcSectorLogicContainer* sectorLogicContainer ) const 
  {
    
    //#ifndef NVERBOSE
    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << "**********Decoder dumping the words******** " << endmsg;
      if (data_size > 0 ) {
       msg(MSG::VERBOSE) << "The size of this ROD-read is " << data_size << endmsg;
       for (unsigned int i=0; i < data_size; i++)
         msg(MSG::VERBOSE) << "word " << i << " = " << MSG::hex << data[i] << MSG::dec << endmsg;
     }
   }
    //#endif
    
    uint16_t side  = (subDetector == eformat::MUON_RPC_BARREL_A_SIDE) ? 1:0;    
    
    
    // TMP FIXME the sector number needs to be fixed !!
    uint16_t sector = 0;
    
    // counter of the numerb of sector logic fragments found
    uint16_t SLindex = 0;
    
    // the identifier of this collection 
    Identifier thisPadOfflineId = v.identify();
    
    // Identifier of the collection in data
    Identifier padOfflineId;
    bool foundPad=false;
    
    bool isSLHeader    =false;
    bool isSLFooter    =false;
    bool isSLFragment  =false;
    bool isRXHeader    =false;
    bool isRXFooter    =false;
    bool isPADFragment =false;
    bool isPadHeader   =false;
    bool isPadSubHeader=false;
    bool isPadPreFooter=false;
    bool isPadFooter   =false;
    
    //return;
    
    RPCRODDecode myRPC;
    RXReadOutStructure  RXROS;
    PadReadOutStructure PDROS;
    SectorLogicReadOutStructure SLROS;
    MatrixReadOutStructure matrixROS;
    
    
    char recField;
    unsigned short int PadID=99;
    unsigned int SLBodyWords=0;  
    unsigned int SL_data_size = 500; // same value used for the size of SLBuff
    unsigned short int SLBuff[500];
    
    RpcSectorLogic* sl=0;
    
    for (uint16_t i = 0; i < data_size; ++i) 
    {
      
      //std::cout << "REGISTER: " << i << std::endl;
      
      isRXHeader    =false;
      isRXFooter    =false;
      isPadHeader   =false;
      isPadSubHeader=false;
      isPadPreFooter=false;
      isPadFooter   =false;
      isSLHeader    =false;
      isSLFooter    =false;
      uint32_t currentWord = data[i];
      
      //#ifndef NVERBOSE
      ATH_MSG_VERBOSE (" -->current data word is "<<std::hex<<currentWord<<std::dec);
      //#endif  
      
      RXROS.decodeFragment(currentWord,recField);
      PDROS.decodeFragment(currentWord,recField);
      SLROS.decodeFragment(currentWord,recField);
      
      if (RXROS.isHeader() && !isSLFragment) isRXHeader=true;
      else if (RXROS.isFooter() && !isSLFragment) isRXFooter=true;
      else if (PDROS.isHeader() && !isSLFragment) isPadHeader=true;
      else if (PDROS.isSubHeader() && !isSLFragment) isPadSubHeader=true;
      else if (PDROS.isPreFooter() && !isSLFragment) isPadPreFooter=true;
      else if (PDROS.isFooter() && !isSLFragment) isPadFooter=true;
      else if (SLROS.isHeader()) isSLHeader=true;
      else if (SLROS.isFooter()) isSLFooter=true;
      
      //#ifndef NVERBOSE
      if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) <<" RX Header: "<<isRXHeader
      <<" RX Footer: "<<isRXFooter
      <<endmsg;
      msg(MSG::VERBOSE) <<" Pad Header: "<<isPadHeader
      <<" Pad SubHeader: "<<isPadSubHeader
      <<" Pad PreFooter: "<<isPadPreFooter
      <<" Pad Footer: "<<isPadFooter
      <<endmsg;
      msg(MSG::VERBOSE) <<" isPADFragment: "<<isPADFragment
      <<endmsg;
      msg(MSG::VERBOSE) <<" SL Header: "<<isSLHeader
      <<" SL Footer: "<<isSLFooter
      <<endmsg;
      msg(MSG::VERBOSE) <<" isSLFragment: "<<isSLFragment
      <<endmsg;
      }
      //#endif   
      
      if(isRXHeader) { 
	//#ifndef NVERBOSE
	 if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) <<" this is a RX Header "<<endmsg;
        msg(MSG::VERBOSE) <<" Sector ID="<<RXROS.RXid()<<endmsg;
	 }
	 //#endif
        
        // set sector ID, old format of RX header
        //sector = RXROS.RXid();
        sector = RXROS.status();
        
        uint16_t errorCode = RXROS.errorCode();
        
        // set to zero the index of the SL fragment
        SLindex=0;
        
        // check if the SL already exists
        if (sectorLogicContainer && 
            !sectorLogicContainer->findSector(sector,0)) {
          
          // Create the new sector logic object
          sl = new RpcSectorLogic(sector,0,0,errorCode);    
          
        }
        else if (sectorLogicContainer){
          for ( RpcSectorLogicContainer::iterator itSL = sectorLogicContainer->begin() ;
               itSL != sectorLogicContainer->end() ; ++itSL ) {
            if ( (*itSL)->sectorId() == sector ) {
              sl = (*itSL);
              break;
            }
          }
        }
        
      } else if(isRXFooter) { 
	//#ifndef NVERBOSE
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<" this is a RX Footer "<<endmsg;
	//#endif
      } else if(isSLHeader || isSLFragment)  {
        
        isSLFooter ? isSLFragment=false : isSLFragment=true;
        
        // push only the lowest 16 bits
        int foundSL = myRPC.pushWord(currentWord,0);
        
        // Check the Sector Logic Fragment      
        if(foundSL) 
	  {
	  //#ifndef NVERBOSE
	  if (msgLvl(MSG::VERBOSE)) 
	    {
	      msg(MSG::VERBOSE) <<"SectorLogicReadOut checkFragment: "
				<< myRPC.SLFragment()->checkFragment()<<endmsg;
	      msg(MSG::VERBOSE) << myRPC.SLFragment()<<endmsg;
	    }
	  //#endif
	  }
        
        if(isSLHeader) SLBodyWords=0;
        
        // Decode the sector logic footer
        else if(isSLFooter ) {
          
#ifndef NVERBOSE
          if (SLindex>1) {
            msg(MSG::ERROR) << "More than 2 SL fragments in sector " << sector << endmsg;
          }
#endif
          
#ifndef NVERBOSE
          msg(MSG::VERBOSE) <<" Number of data words in SectorLogicReadOut= "
          <<SLBodyWords<<endmsg;   
          msg(MSG::VERBOSE) <<" TEST SL: "<<foundSL<<endmsg;
#endif  
          
#ifndef NVERBOSE
          // Print out a raw dump of the SL fragment 
          for(unsigned short j=0; j<SLBodyWords; j++) {
            msg(MSG::VERBOSE) <<" SL data word "<<j<<" : "<<std::hex<<SLBuff[j]<<MSG::dec<<endmsg;
          }
#endif
          
          // Found the sector logic footer, the sector logic fragment 
          // can be added to the sector logic container
          
          
          // Check if the sector was already added to the container
          if ( sectorLogicContainer &&
              !sectorLogicContainer->findSector(sector, SLindex) ) {
            
            SectorLogicReadOut* sectorLogic = myRPC.SLFragment();
            // decode the SL hits and push back in the sector logic
            uint16_t nSLlink = 2;
            uint16_t nSLgate = 7;
            
            // fill the hit content
            for (int igate = 0 ; igate < nSLgate ; ++igate) {
              
              for (int ilink = 0 ; ilink < nSLlink ; ++ilink) {
                
                uint16_t ptid  = sectorLogic->ptid(ilink,igate);
                
                if (ptid != 0) {
                  
                  uint16_t cmadd = sectorLogic->cmadd(ilink,igate);
                  
                  uint16_t bcid  = igate;
                  uint16_t tower = ilink+2*SLindex;
                  
                  uint16_t opl   = sectorLogic->opl(ilink,igate);
                  uint16_t ovphi = sectorLogic->ovphi(ilink,igate);
                  uint16_t oveta = sectorLogic->oveta(ilink,igate);
                  
                  uint16_t triggerBcid  = sectorLogic->bcid(ilink,igate);
                  
                  // create the hit and push back in the sl
                  RpcSLTriggerHit* slHit = new RpcSLTriggerHit(bcid,tower,ptid,cmadd,
                                                               opl,ovphi,oveta,triggerBcid);
                  sl->push_back(slHit);
                  
                }
              }
              
            }
            
            // set the trigger rates
            // add the trigger rate of the second tower only if it is 
            // the first sector logic fragment
            sl->addTriggerRate(sectorLogic->padTriggerRate(0));
            if (SLindex==0) {
              sl->addTriggerRate(sectorLogic->padTriggerRate(1));
            }
            
            // push back the counters
            uint16_t nSLcount = sectorLogic->numberOfCounterWords();
            
            for ( int icount =0 ; icount<nSLcount ; ++icount ) {
              
              uint16_t counter = ((sectorLogic->readSLCounterCurrent()) & 0x1fff);
              sl->addCounter(counter);
            }
            
            // Flag the sector as initialized
            //bool setSector = sectorLogicContainer->setSector(sector,SLindex);
            
            if (SLindex==0 && sectorLogicContainer) {
              sectorLogicContainer->push_back(sl);
            }
          }
          
          // increment the SLindex counter of the number of fragments
          SLindex++;
          
          
        } else { 
          if ( SLBodyWords >= SL_data_size ) {
            if (msgLvl(MSG::VERBOSE) ) msg(MSG::VERBOSE)
              << "Sector Logic payload corrupted" << endmsg;
            return StatusCode::FAILURE;
          } 
          SLBuff[SLBodyWords]=currentWord;
          SLBodyWords++;
        } // isSLHeader
        
      } else if(isPadHeader || isPADFragment)  {
        
        // Now decoding the header of the pad
#ifndef NVERBOSE
        msg(MSG::VERBOSE)<< " Pad Header or Pad Fragment " << endmsg;
#endif
        
        PDROS.decodeFragment(currentWord,recField);
        
        if(recField == 'H') {
          
          PadID=PDROS.padid();
          
          uint16_t status = 0;
          
          side = (sector<32) ? 0:1;
          uint16_t sectorLogic = sector-side*32;
#ifndef NVERBOSE
          msg(MSG::VERBOSE) <<" Pad Identifier= "<<PadID<< " Status: " << status << endmsg;
#endif
          // get the offline ID of the pad
          if(!m_cabling->giveOffflineID(side,sectorLogic,PadID,padOfflineId))
          {
            if (msgLvl(MSG::VERBOSE) ) 
              msg(MSG::VERBOSE) 
              << "Cannot retrieve the OfflineID for the PAD n. " 
              << PadID << " at side " << side << " and  sector " 
              << sectorLogic << endmsg;
          }
          else 
            if (msgLvl(MSG::VERBOSE) ) 
              msg(MSG::VERBOSE) 
              << "ID " << m_pRpcIdHelper->show_to_string(padOfflineId)
              << " associated to PAD n. " << PadID << " at side " 
              << side << " and  sector " << sectorLogic << endmsg;
          
          // check if it's the pad to convert
          if (thisPadOfflineId == padOfflineId) {
            if (msgLvl(MSG::VERBOSE) ) 
              msg(MSG::VERBOSE) 
              << " match found with ID " 
              << m_pRpcIdHelper->show_to_string(thisPadOfflineId)
              << " requested for the conversion; return this collection" 
              << endmsg; 
            
            foundPad = true; 
#ifndef NVERBOSE
            msg(MSG::VERBOSE) << "Found the pad to convert !" << endmsg;
#endif
            v.setOnlineId(PadID);
            v.setStatus(status);
            v.setSector(sector);
            
            // set the lvl1 id
            v.setLvl1Id(PDROS.l1id());
            
          }
          else
          {
            if (msgLvl(MSG::VERBOSE) ) 
              msg(MSG::VERBOSE) 
              << " match NOT found with ID " 
              << m_pRpcIdHelper->show_to_string(thisPadOfflineId)
              << " requested for the conversion" << endmsg;
          } 
        }
        
        // if it's a subheader, set the bcid
        if (recField == 'S') {
          
          if (foundPad) {
            v.setBcId(PDROS.bcid());
#ifndef NVERBOSE
            msg(MSG::VERBOSE) << "Found the subheader, setting bcid to: " 
            << PDROS.bcid() << endmsg;
#endif
          }
          
        }
        
        
        if(recField == 'F') {
#ifndef NVERBOSE
          msg(MSG::VERBOSE) <<" Pad Footer "<<endmsg;
#endif
          // found the pad, bail out
          if (foundPad) {
            foundPad=false;
            return StatusCode::SUCCESS;
          }
        }
        
        isPadFooter ? isPADFragment=false : isPADFragment=true; 
        
#ifndef NVERBOSE
        msg(MSG::VERBOSE) <<" current word "<<std::hex<<currentWord<<MSG::dec<<endmsg;
        
        msg(MSG::VERBOSE) <<" ==isPADFragment= "<<isPADFragment<<endmsg;
        msg(MSG::VERBOSE) <<" calling pushword: "<<std::hex<<currentWord<<MSG::dec<<endmsg;
#endif
        
        int foundCM = 0;
        foundCM = myRPC.pushWord(currentWord,0);
        
        if(foundCM==1) {
#ifndef NVERBOSE
          msg(MSG::VERBOSE) <<myRPC.CMFragment()<<endmsg;
#endif
          // If the pad is the good one, add the CMs to the container   
          if (foundPad) {
            MatrixReadOut* matrix = myRPC.CMFragment();
            
            //std::cout << myRPC.CMFragment()<< std::endl;
            
            matrixROS = matrix->getHeader();  
            uint16_t cmaId  = matrixROS.cmid();
            uint16_t fel1id = matrixROS.fel1id();
            
            matrixROS = matrix->getSubHeader();
            uint16_t febcid = matrixROS.febcid();
            
#ifndef NVERBOSE
            msg(MSG::VERBOSE) << "Creating a new CM, cmaId=" << cmaId << " fel1id=" << fel1id
            << " febcid=" << febcid << endmsg;
#endif
            
            // Create the new cm
            RpcCoinMatrix * coinMatrix = new RpcCoinMatrix (padOfflineId,cmaId,fel1id,febcid);
            
            //std::cout << matrix->numberOfBodyWords() << std::endl;
            
            // Loop on the hits and push them in the coin matrix
            for (int i=0 ; i<matrix->numberOfBodyWords() ; ++i) {
              
              matrixROS = matrix->getCMAHit(i);
              
              uint16_t bcid = matrixROS.bcid();
              uint16_t time = matrixROS.time();
              uint16_t ijk  = matrixROS.ijk();
              
              RpcFiredChannel* firedChan = 0;
              
              if (ijk < 7 ) {
                uint16_t channel = matrixROS.channel();
                firedChan = new RpcFiredChannel(bcid,time,ijk,channel);
#ifndef NVERBOSE
                msg(MSG::VERBOSE) << "Adding a fired channel, bcid=" << bcid << " time=" 
                << " ijk=" << ijk << " channel=" << channel << endmsg;
#endif
                // add the fired channel to the matrix
                coinMatrix->push_back(firedChan); 
              }
              else if (ijk==7) {
                uint16_t overlap   = matrixROS.overlap();
                uint16_t threshold = matrixROS.threshold();
                firedChan = new RpcFiredChannel(bcid,time,ijk,threshold,overlap);
#ifndef NVERBOSE
                msg(MSG::VERBOSE) << "Adding a fired channel, bcid=" << bcid << " time=" 
                << " ijk=" << ijk << " overlap=" << overlap 
                << " threshold=" << threshold << endmsg;
#endif
                // add the fired channel to the matrix
                coinMatrix->push_back(firedChan);  
              } 
            }
            
            v.push_back(coinMatrix);
            
          } // end of the matrix decoding
          
          (myRPC.CMFragment())->reset();
          
        } // end of the pad decoding
        
      }
      
    }
    
    return StatusCode::SUCCESS; 
  }
  
  
  
  /**   fill RpcPads from a block of integers 
   Decode collection for old data format 2.4.0
   */ 
  inline StatusCode
  RpcROD_Decoder::fillCollection_v240(BS data, const uint32_t data_size,
                                      RpcPad& v) const 
  {
    
    const int rodHeader = 8;
    const int rodFooter = 3;
    bool printMessage = true; // to print only once per call
    
    /*
     msg(MSG::VERBOSE) << "**********Decoder dumping the words******** " << endmsg;
     if (data_size > 0 ) {
     msg(MSG::VERBOSE) << "The size of this ROD-read is " << data_size << endmsg;
     for (int i=0; i < data_size; i++)
     msg(MSG::VERBOSE) << "word " << i << " = " << MSG::hex << p[i] << MSG::dec << endmsg;
     }
     */  
    
    // the identifier of this collection 
    Identifier thisPadOfflineId = v.identify();
    
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
      << "The offline ID request for conversion is " 
      << m_pRpcIdHelper->show_to_string(thisPadOfflineId) 
      << endmsg;
    
    // remove the rod header and footer then
    // convert the rest of 32-bits into 16-bit words
    std::vector<uint16_t> v16 = get16bits ( data, data_size, rodHeader, rodFooter );
    
    int word16Count=0;
    int size16 = v16.size();
    
    assert ( size16 > 0 && size16 == (int)( 2 * (data_size-rodHeader-rodFooter) ) );
    
    RPCRODStructure  rodReadout; 
    RXReadOutStructure  rxReadout; 
    PadReadOutStructure  padReadout; 
    MatrixReadOutStructure  matrixReadout; 
    
    // first 8 32-bit words are the ROD headers
    // then come a series of 16-bit words: the data
    // followed by 3 32-bit ROD footer
    
    // before moving to the first receiver header
    // find out where we are: half A or C?
    
    rodReadout.decodeSourceID ( data[3] );
    uint16_t subDetectorID = rodReadout.getSourceIDSubdetectorID();
    
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "subDetectorID = 0x" 
      << MSG::hex << subDetectorID << MSG::dec << endmsg;
    
    uint16_t side  = (subDetectorID == eformat::MUON_RPC_BARREL_A_SIDE) ? 0:1; 
    uint16_t rodId =  rodReadout.getSourceIDRODID();
    
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "rodID = 0x" << MSG::hex 
      << rodId << MSG::dec << endmsg;
    
    assert (rodId <= 15);
    
    
    // loop over 2 possible receivers per ROD   
    char rxHeader ='U';
    if (!ensure_more_data(word16Count,size16,msg(),printMessage,"start of data")) 
      return StatusCode::FAILURE;
    uint16_t receiverHeaderFragment = v16[word16Count];
    rxReadout.decodeFragment(receiverHeaderFragment,rxHeader);
    if (rxHeader == 'H') { 
      word16Count += 1;
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
        << "Found a receiver header " << endmsg;
    }
    else
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
        << "RpcROD_Decoder::ERROR : Expecting a receiver header " 
        << "bailing out" << " Fragment ID is " << MSG::hex << rxHeader 
        << MSG::dec << endmsg;
    
    
    while (rxHeader == 'H' && word16Count < size16) {
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
        << "The receiver header word is " << MSG::hex 
        << receiverHeaderFragment << MSG::dec << endmsg; 
      uint16_t slogic = 2*rodId + rxReadout.RXid();
      uint16_t sectorID = side*32 + slogic;
      assert (slogic <= 31);
      char padHeader = 'U';
      uint16_t padHeaderFragment = v16[word16Count];
      padReadout.decodeFragment(padHeaderFragment,padHeader);
      if (padHeader == 'H') {
        word16Count += 1;
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Found a pad header "
          << endmsg;
      }
      else
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)
          << "Rpc_ROD_Decoder::ERROR : Expecting a pad header " 
          << " Fragment ID is " << padHeader << endmsg;
      
      while (padHeader == 'H') {
        uint16_t padId = padReadout.padid();
        
        // uint16_t status = padReadout.status();
        uint16_t status = 0;
        
        Identifier padOfflineId;
        if(!m_cabling->giveOffflineID(side,slogic,padId,padOfflineId))
        {
          if (msgLvl(MSG::VERBOSE) ) 
            msg(MSG::VERBOSE) 
            << "Cannot retrieve the OfflineID for the PAD n. " 
            << padId << " at side " << side << " and  sector " 
            << slogic << endmsg;
        }
        else 
          if (msgLvl(MSG::VERBOSE) ) 
            msg(MSG::VERBOSE) 
            << "ID " << m_pRpcIdHelper->show_to_string(padOfflineId)
            << " associated to PAD n. " << padId << " at side " 
            << side << " and  sector " << slogic << endmsg;
        
        // check if this the collection requested
        // otherwise created a new collection and
        // record the new collection into StoreGate
        // so that the next time the new collection is requested,
        // it will be already in StoreGate!!!
        if (thisPadOfflineId == padOfflineId) {
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << "Found the collection to return " 
            << m_pRpcIdHelper->show_to_string(thisPadOfflineId) 
            << endmsg;
          v.setOnlineId(padId);
          v.setStatus(status);
          v.setSector(sectorID);
        } else {
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << m_pRpcIdHelper->show_to_string(thisPadOfflineId) 
            << "!=" << m_pRpcIdHelper->show_to_string(padOfflineId)
            << endmsg;
        } 
        char cmaHeader    = 'U';
        if (!ensure_more_data(word16Count,size16,msg(),printMessage,"padHeader")) break;
        uint16_t cmaHeaderFragment = v16[word16Count];
        matrixReadout.decodeFragment(cmaHeaderFragment,cmaHeader);
        if (cmaHeader == 'H') {
          word16Count += 1;
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << "Found a cma header" << endmsg;
        } else
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)
            << "RpcROD_Decoder::ERROR : Expecting a cma header" 
            << " Fragment ID is " << cmaHeader << endmsg;
        
        while (cmaHeader == 'H') {
          uint16_t cmaId  = matrixReadout.cmid();
          uint16_t fel1id = matrixReadout.fel1id();
          char cmaSubHeader = 'U';
          uint16_t febcid   = static_cast<uint16_t>(-1);
          if (!ensure_more_data(word16Count,size16,msg(),printMessage,"cmaHeader")) break;
          uint16_t cmaSubHeaderFragment = v16[word16Count];
          matrixReadout.decodeFragment(cmaSubHeaderFragment,cmaSubHeader);
          if (cmaSubHeader == 'S') {
            febcid = matrixReadout.febcid();
            word16Count += 1;
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
              << "Found a cma sub-header" << endmsg;
          } else
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)
              << "RpcROD_Decoder::ERROR : Expecting a cma sub-header" 
              << " Fragment ID is " << cmaSubHeader << endmsg;
          
          if (!ensure_more_data(word16Count,size16,msg(),printMessage,"cmtSubHeader")) break;
          RpcCoinMatrix * coinMatrix = new RpcCoinMatrix (padOfflineId,cmaId,fel1id,febcid);
          char cmaBody = 'U';
          uint16_t cmaBodyFragment = v16[word16Count];
          matrixReadout.decodeFragment(cmaBodyFragment,cmaBody);
          if (cmaBody == 'B') {
            word16Count += 1;
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
              << "Found a cma body "  << MSG::hex << cmaBodyFragment << endmsg;
          } else
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
              << "Expecting cma body" <<  " Fragment ID is " << cmaBody 
              << endmsg;
          while (cmaBody == 'B') {
            uint16_t bcid = matrixReadout.bcid();
            uint16_t time = matrixReadout.time();
            uint16_t ijk  = matrixReadout.ijk();
            
            RpcFiredChannel* firedChannel=0;
            if (ijk < 7)
            {
              uint16_t channel = matrixReadout.channel();
              firedChannel = new RpcFiredChannel(bcid,time,ijk,channel);
              coinMatrix->push_back(firedChannel);
            }
            else if (ijk == 7)
            {
              uint16_t threshold = matrixReadout.threshold();
              uint16_t overlap = matrixReadout.overlap();
              firedChannel = new RpcFiredChannel(bcid,time,ijk,threshold,overlap);
              coinMatrix->push_back(firedChannel);
            }       
            else
            {
              if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
                << "RpcROD_Decoder::ERROR : Wrong ijk value " << ijk 
                << "in cma body " << endmsg;
            }
            if (!ensure_more_data(word16Count,size16,msg(),printMessage,"cmaBody")) {
              delete coinMatrix;
              coinMatrix = 0;
              break;
            }
            cmaBodyFragment = v16[word16Count];
            matrixReadout.decodeFragment(cmaBodyFragment,cmaBody);
            if (cmaBody == 'B') {
              word16Count += 1;
              if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
                << "Found a cma body" << " " << MSG::hex 
                << cmaBodyFragment << MSG::dec << endmsg;
            } 
            else ATH_MSG_VERBOSE(" No more body fragment found " << cmaBody << " " << MSG::hex << cmaBodyFragment << MSG::dec);
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
              << "End of a cma body" << endmsg;
          } // end of fired channels
          char cmaFooter = 'U';
          if (!ensure_more_data(word16Count,size16,msg(),printMessage,"after cmaBody")) {
            delete coinMatrix;
            coinMatrix = 0;
            break;
          }
          uint16_t cmaFooterFragment = v16[word16Count];
          matrixReadout.decodeFragment(cmaFooterFragment,cmaFooter);
          if (cmaFooter == 'F') {
            uint16_t crc = matrixReadout.crc();
            if (coinMatrix) coinMatrix->setCRC(crc); // Added to try to "fix" CID 12374
            else ATH_MSG_ERROR("Trying to call null coinMatrix - this should never happen!");
            word16Count += 1;
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
              << "Found a cma Footer " << MSG::hex << cmaFooterFragment 
              << MSG::dec << endmsg;
          } else { // of cma, record it into a pad
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)
              << "RpcROD_Decoder::ERROR : Expecting a cma Footer" 
              << " Fragment ID is " << cmaFooter << " " << MSG::hex 
              << cmaFooterFragment << MSG::dec << endmsg;
          }
          
          if (thisPadOfflineId == padOfflineId) v.push_back(coinMatrix);
          else delete coinMatrix;
          
          if (!ensure_more_data(word16Count,size16,msg(),printMessage,"cmaFooter")) break;
          cmaHeaderFragment = v16[word16Count];
          matrixReadout.decodeFragment(cmaHeaderFragment,cmaHeader);
          if (cmaHeader == 'H') {
            word16Count += 1;
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
              << "Found a cma header" << endmsg;
          } 
          else if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << "End of all CMAs" << endmsg;
        } // end of cma
        char padFooter = 'U';
        uint16_t errorCode = static_cast<uint16_t>(-1);
        if (!ensure_more_data(word16Count,size16,msg(),printMessage,"after CMAs")) break;
        uint16_t padFooterFragment = v16[word16Count];
        padReadout.decodeFragment(padFooterFragment,padFooter);
        if (padFooter == 'F') {
          word16Count += 1;
          errorCode = padReadout.errorCode();
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << "Found a pad footer " << endmsg;
          if (thisPadOfflineId == padOfflineId) {
            v.setErrorCode(errorCode);
            // we found the pad and it is filled so clean up and return quickly!
            return StatusCode::FAILURE;
          }
        } else
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)
            << "RpcROD_Decoder::ERROR : Expecting a pad footer " 
            << " Fragment ID is " << padFooter << " " << MSG::hex 
            << padFooterFragment << MSG::dec << endmsg;
        if (!ensure_more_data(word16Count,size16,msg(),printMessage,"padFooter")) break;
        padHeaderFragment = v16[word16Count];
        padReadout.decodeFragment(padHeaderFragment,padHeader);
        if (padHeader == 'H') {
          word16Count += 1;
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << "Found a pad header " << endmsg;
        } 
        else if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
          << "End of all pads " << endmsg;
      } // end of pads
      
      char rxFooter = 'U';
      if (!ensure_more_data(word16Count,size16,msg(),printMessage,"after pads")) break;
      uint16_t receiverFooterFragment = v16[word16Count];
      rxReadout.decodeFragment(receiverFooterFragment,rxFooter);
      if (rxFooter == 'F') {
        word16Count += 1;
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
          << "Found a receiver footer " << endmsg;
      } else 
        if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
          << "RpcROD_Decoder::ERROR : Expecting a receiver footer " 
          << " Fragment ID is " << rxFooter << " " << MSG::hex 
          << receiverFooterFragment << MSG::dec << endmsg;
      
      
      if( word16Count < size16 ) {
        receiverHeaderFragment = v16[word16Count];
        rxReadout.decodeFragment(receiverHeaderFragment,rxHeader);
        if (rxHeader == 'H') { 
          word16Count += 1;
          if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) 
            << "Found a receiver header " << endmsg;
        }  
        else if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE)        
          << "End of all receivers " << endmsg;
      }
      
    } // end of receivers: while (rxHeader == 'H' && wordCount < size16)
    
    // end of the ROD
    
    return StatusCode::FAILURE; 
  }
  
  
  inline std::vector<uint16_t> RpcROD_Decoder::get16bits_v301(BS  v32, 
                                                              const int size,
                                                              const int nHeader,
                                                              const int nFooter) const {
    uint32_t mask = 0x0000FFFF;
    uint32_t pos[2] = {16, 0};
    
    std::vector<uint16_t> result;
    
    for (int i=nHeader; i< (size-nFooter); i++) {
      for(uint32_t j=0; j<2; j++) {
        uint32_t vshift =  v32[i] >> pos[j];
        uint16_t fragment = (uint16_t) ( vshift & mask );
        result.push_back(fragment);
      }
    }
    return result;
  }
  
  
  
  inline std::vector<uint16_t> RpcROD_Decoder::get16bits(BS  v32, 
                                                         const int size,
                                                         const int nHeader,
                                                         const int nFooter) const {
    
    uint32_t mask = 0x0000FFFF;
    uint32_t pos[2] = {0, 16};
    
    std::vector<uint16_t> result;
    
    for (int i=nHeader; i< (size-nFooter); i++) {
      for(uint32_t j=0; j<2; j++) {
        uint32_t vshift =  v32[i] >> pos[j];
        uint16_t fragment = (uint16_t) ( vshift & mask );
        result.push_back(fragment);
      }
    }
    return result;
  }
  
} // end of NameSpace
#endif



