/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <sstream>
#include <iomanip>

#include<sys/time.h>

#include "TrigmuFast/CalibEvent.h"
#include "TrigmuFast/CalibUti.h"

using namespace LVL2CALIBRATION;

CalibEvent::CalibEvent() : m_version(0),m_run_number(0),m_lvl1_id(0),
                           m_timestamp(0),m_ip_address(0),m_l2pu_id(0),
                           m_eta(0),m_phi(0),m_pt(0),m_mdt(0),m_rpc(0),m_tgc(0)
{
    m_mrod_trigger= 0x0;
    for (int i=0;i<4;++i) m_mrod_data[i] = 0x0;
    memset(m_name, '\0', sizeof(m_name) );
}

CalibEvent::CalibEvent(float eta, float phi, float pt) : 
            m_version(0),m_run_number(0),m_lvl1_id(0),
            m_timestamp(0),m_ip_address(0),m_l2pu_id(0),
            m_eta(eta),m_phi(phi),m_pt(pt),m_mdt(0),m_rpc(0),m_tgc(0) 
{
    m_mrod_trigger= 0x0;
    for (int i=0;i<4;++i) m_mrod_data[i] = 0x0;
    memset(m_name, '\0', sizeof(m_name) );
}

CalibEvent::CalibEvent(uint16_t version, uint32_t run_number, uint32_t lvl1_id, 
           uint16_t l2pu_id, uint32_t trigger_mrod, uint32_t data_mrod[4], 
	   std::string name, float eta, float phi, float pt):
           m_version(version),m_run_number(run_number),m_lvl1_id(lvl1_id),
           m_l2pu_id(l2pu_id),m_mrod_trigger(trigger_mrod),
           m_eta(eta),m_phi(phi),m_pt(pt),m_mdt(0),m_rpc(0),m_tgc(0)

{
    m_timestamp  = TStoWords();
    IPfromChar(fill_ip().c_str(),m_ip_address);
     
    for (int i=0;i<4;++i) m_mrod_data[i] = data_mrod[i];
    memset(m_name, '\0', sizeof(m_name) );
    for(unsigned int i=0;i<99&&i<name.length();++i) m_name[i] = name.c_str()[i];
}

CalibEvent::~CalibEvent() {
    if (m_mdt) delete m_mdt;
    if (m_rpc) delete m_rpc;
    if (m_tgc) delete m_tgc;
}

CalibEvent::CalibEvent(const CalibEvent& event) : CalibData() {
    
    MdtCalibFragment* mdt = event.mdt();
    RpcCalibFragment* rpc = event.rpc();
    TgcCalibFragment* tgc = event.tgc();
    
    m_version     = event.version();
    m_run_number  = event.run_number();
    m_lvl1_id     = event.lvl1_id();
    m_timestamp   = event.timestamp();
    m_l2pu_id     = event.l2pu_id();
    m_ip_address  = event.ip_address();
    
    m_mrod_trigger = event.mrod_trigger();
    for(int i=0;i<4;++i) m_mrod_data[i] = event.mrod_data(i);
   
    memset(m_name, '\0', sizeof(m_name) );
    for(unsigned int i=0;i<99&&i<event.name().length();++i) 
                           m_name[i] = event.name().c_str()[i];
    
    m_eta = event.eta();
    m_phi = event.phi();
    m_pt  = event.pt();
    
    m_mdt = (mdt)? new MdtCalibFragment(*mdt) : 0;
    m_rpc = (rpc)? new RpcCalibFragment(*rpc) : 0;
    m_tgc = (tgc)? new TgcCalibFragment(*tgc) : 0;
}


void 
CalibEvent::operator<<(const MdtCalibFragment& fragment) {
    if(m_mdt) delete m_mdt;
    m_mdt = new MdtCalibFragment(fragment);
}

void 
CalibEvent::operator<<(const RpcCalibFragment& fragment) {
    if(m_rpc) delete m_rpc;
    m_rpc = new RpcCalibFragment(fragment);
}

void 
CalibEvent::operator<<(const TgcCalibFragment& fragment) {
    if(m_tgc) delete m_tgc;
    m_tgc = new TgcCalibFragment(fragment);
}


std::string 
CalibEvent::timestamp_str(void) const  {
    struct timeval tv;
    tv.tv_sec = m_timestamp;
    return std::string(ctime(&tv.tv_sec));
}

std::string 
CalibEvent::ip_address_str(void) const {
    char IP[17];
    memset(IP, '\0', 17);
    IPfromLong(m_ip_address, IP);
    return std::string(IP);
}

void 
CalibEvent::decode_v0(uint8_t* data) {

    uint32_t word1 = wordFromLitEndChars(data);
    uint16_t con,tag;
    wordIntoBit16(word1,con,tag);
	
    uint32_t word3 = wordFromLitEndChars(data+2*WRDSIZE);
    uint16_t eta,phi;
    wordIntoBit16(word3,phi,eta);
    int32_t pt = wordFromLitEndChars(data+3*WRDSIZE);
	
    int16_t Eta = eta;
    int16_t Phi = phi;
	
    m_eta = ((float)Eta)/10000.;
    m_phi = ((float)Phi)/10000.;
    m_pt  = ((float)pt)/1000.;
	
    uint32_t word5 = wordFromLitEndChars(data+4*WRDSIZE);
    uint32_t word6 = wordFromLitEndChars(data+5*WRDSIZE);
    uint32_t word7 = wordFromLitEndChars(data+6*WRDSIZE);
    m_mdt = (con&0x0001)? new MdtCalibFragment(data+word5) : 0;
    m_rpc = (con&0x0010)? new RpcCalibFragment(data+word6) : 0;
    m_tgc = (con&0x0100)? new TgcCalibFragment(data+word7) : 0;
}


void 
CalibEvent::decode_v1(uint8_t* data) {

    uint32_t word1 = wordFromLitEndChars(data);
    uint16_t con,tag;
    wordIntoBit16(word1,con,tag);
    
    uint32_t word2 = wordFromLitEndChars(data+1*WRDSIZE);
    uint16_t data_size;
    wordIntoBit16(word2,data_size,m_version);

    m_run_number = wordFromLitEndChars(data+2*WRDSIZE);
    m_lvl1_id    = wordFromLitEndChars(data+3*WRDSIZE);
    m_timestamp  = wordFromLitEndChars(data+4*WRDSIZE);
    m_ip_address = wordFromLitEndChars(data+5*WRDSIZE);

    uint32_t word7 = wordFromLitEndChars(data+6*WRDSIZE);
    uint16_t trigger_mrod;
    wordIntoBit16(word7,trigger_mrod,m_l2pu_id);
    m_mrod_trigger = ((trigger_mrod&0xff00)<<8)|(trigger_mrod&0x00ff);
    
    uint32_t word8 = wordFromLitEndChars(data+7*WRDSIZE);
    uint32_t word9 = wordFromLitEndChars(data+8*WRDSIZE);
    uint16_t data_mrod[4];
    wordIntoBit16(word8,data_mrod[1],data_mrod[0]);
    wordIntoBit16(word9,data_mrod[3],data_mrod[2]);
    
    for(int i=0;i<4;++i) 
        m_mrod_data[i] = ((data_mrod[i]&0xff00)<<8)|(data_mrod[i]&0x00ff);
    
    uint32_t word10 = wordFromLitEndChars(data+9*WRDSIZE);
    uint16_t eta,phi;
    wordIntoBit16(word10,phi,eta);
    int32_t pt = wordFromLitEndChars(data+10*WRDSIZE);
	
    int16_t Eta = eta;
    int16_t Phi = phi;
	
    m_eta = ((float)Eta)/10000.;
    m_phi = ((float)Phi)/10000.;
    m_pt  = ((float)pt)/1000.;
	
    uint32_t word12 = wordFromLitEndChars(data+11*WRDSIZE);
    uint32_t word13 = wordFromLitEndChars(data+12*WRDSIZE);
    
    uint16_t rpc_start,mdt_start,tgc_start,csc_start;
    wordIntoBit16(word12,rpc_start,mdt_start);
    wordIntoBit16(word13,tgc_start,csc_start);
    
    //decode name
    memset(m_name,'\0',100);
    uint8_t name_size = *(data+EV1HDRSIZE);
    copyChars(data+EV1HDRSIZE+1,(uint8_t*)m_name,name_size-1);

    m_mdt = (con&0x0001)? new MdtCalibFragment(data+mdt_start) : 0;
    m_rpc = (con&0x0010)? new RpcCalibFragment(data+rpc_start) : 0;
    m_tgc = (con&0x0100)? new TgcCalibFragment(data+tgc_start) : 0;
}


CalibEvent::CalibEvent(uint8_t* data) :
    m_version(0),m_run_number(0),m_lvl1_id(0),
    m_timestamp(0),m_ip_address(0),m_l2pu_id(0),
    m_eta(0),m_phi(0),m_pt(0),m_mdt(0),m_rpc(0)
{
    memset(m_name, '\0', 100);
    
    if(checkIntegrity(data)) {
        
	uint16_t version = getVersion(data);

        switch (version) {
    
            case 0:  decode_v0(data);  break;
	    case 1:  decode_v1(data);  break;
	
	    default: break;
    
        }
    }
}


uint16_t 
CalibEvent::getVersion(uint8_t* data) const {

    uint32_t word2 = wordFromLitEndChars(data+1*WRDSIZE);
    
    uint16_t  version,size;    
    wordIntoBit16(word2,size,version);
    
    return version;
}

bool
CalibEvent::checkIntegrity_v0(uint8_t* data) const{

    uint32_t word1  = wordFromLitEndChars(data);
    uint32_t word2  = wordFromLitEndChars(data+1*WRDSIZE);
    uint32_t word5  = wordFromLitEndChars(data+4*WRDSIZE);
    uint32_t word6  = wordFromLitEndChars(data+5*WRDSIZE);
    uint32_t word7  = wordFromLitEndChars(data+6*WRDSIZE);
    uint32_t word8  = wordFromLitEndChars(data+7*WRDSIZE);
    uint32_t word9  = wordFromLitEndChars(data+8*WRDSIZE);
    
    uint32_t tral1 = wordFromLitEndChars(data+word9+EV0HDRSIZE);
    uint32_t tral2 = wordFromLitEndChars(data+word2-1*WRDSIZE);
    
    if( tral1!=TRLTAG ) return false;

    uint16_t  tag,con;
    uint16_t  tag2,con2;    
    wordIntoBit16(word1,con,tag);
    wordIntoBit16(tral2,tag2,con2);
    if( tag!=tag2 || con!=con2 ) return false;
    
    uint16_t dtag,dnum;
    
    wordIntoBit16(wordFromLitEndChars(data+word5),dnum,dtag);
    if(con&0x0001 && dtag!=MDTTAG) return false;
    
    wordIntoBit16(wordFromLitEndChars(data+word6),dnum,dtag);
    if(con&0x0010 && dtag!=RPCTAG) return false;
    
    wordIntoBit16(wordFromLitEndChars(data+word7),dnum,dtag);
    if(con&0x0100 && dtag!=TGCTAG) return false;
    
    wordIntoBit16(wordFromLitEndChars(data+word8),dnum,dtag);
    if(con&0x1000 && dtag!=CSCTAG) return false;

    return true;
}


bool 
CalibEvent::checkIntegrity_v1(uint8_t* data) const {

    uint32_t word1  = wordFromLitEndChars(data);
    uint32_t word2  = wordFromLitEndChars(data+1*WRDSIZE);
    uint32_t word12 = wordFromLitEndChars(data+11*WRDSIZE);
    uint32_t word13 = wordFromLitEndChars(data+12*WRDSIZE);
    uint32_t word14 = wordFromLitEndChars(data+13*WRDSIZE);
      
    uint16_t full_size,version,mdt_start,rpc_start,tgc_start,csc_start;
    wordIntoBit16(word2, full_size,version);
    wordIntoBit16(word12,rpc_start,mdt_start);
    wordIntoBit16(word13,tgc_start,csc_start);
    
    
    if(word14+EV1HDRSIZE+TRLSIZE!= full_size) return false;
    
    
    uint32_t tral1 = wordFromLitEndChars(data+word14+EV1HDRSIZE);
    uint32_t tral2 = wordFromLitEndChars(data+full_size-1*WRDSIZE);
    
    if( tral1!=TRLTAG ) return false;

    uint16_t  tag,con;
    uint16_t  tag2,con2;    
    wordIntoBit16(word1,con,tag);
    wordIntoBit16(tral2,tag2,con2);
    if( tag!=tag2 || con!=con2 ) return false;
    
    uint16_t dtag,dnum;
    
    wordIntoBit16(wordFromLitEndChars(data+mdt_start),dnum,dtag);
    if(con&0x0001 && dtag!=MDTTAG) return false;
    
    wordIntoBit16(wordFromLitEndChars(data+rpc_start),dnum,dtag);
    if(con&0x0010 && dtag!=RPCTAG) return false;
    
    wordIntoBit16(wordFromLitEndChars(data+tgc_start),dnum,dtag);
    if(con&0x0100 && dtag!=TGCTAG) return false;
    
    wordIntoBit16(wordFromLitEndChars(data+csc_start),dnum,dtag);
    if(con&0x1000 && dtag!=CSCTAG) return false;

    return true;
}



bool
CalibEvent::checkIntegrity(uint8_t* data) const {
    
    uint16_t version = getVersion(data);
    
    switch (version) {
    
        case 0:  return checkIntegrity_v0(data);  break;
	case 1:  return checkIntegrity_v1(data);  break;
	
	default: return false; break;
    
    }
    
    return false;
}

uint16_t CalibEvent::size(void) const {

    uint16_t size = 0x0;
    
    switch(m_version) {    
    
        case 0: size = EV0HDRSIZE+TRLSIZE; break;
	case 1: size = EV1HDRSIZE+TRLSIZE+name_size(); break;
    
        default:  break;
    }
    
    if (m_mdt) { size += m_mdt->size();}
    if (m_rpc) { size += m_rpc->size();}
    if (m_tgc) { size += m_tgc->size();}
    //if (m_csc) { size += m_csc->size();}
    return size;
}

uint16_t CalibEvent::payload_size(void) const {

    uint16_t dummy_size = 0x0;
    
    switch(m_version) {    
    
        case 0: dummy_size = EV0HDRSIZE+TRLSIZE; break;
	case 1: dummy_size = EV1HDRSIZE+TRLSIZE+name_size(); break;
    
        default:  break;
    }
    
    return this->size() - dummy_size;
}


int 
CalibEvent::encode_v0(uint8_t* buffer,uint16_t& lenght) const {
    
    // set the tag and the event type
    uint16_t tag = EVTTAG;
    uint16_t con = 0x0;
    if (m_mdt) con |= 0x0001;
    if (m_rpc) con |= 0x0010;
    if (m_tgc) con |= 0x0100;
    
    
    // encode the eta phi and pt values
    int16_t eta = (int) (m_eta*10000.);
    int16_t phi = (int) (m_phi*10000.);
    int32_t pt  = (int) (m_pt*1000.);
    
    uint32_t mdtstart = 0x0;
    uint32_t rpcstart = 0x0;
    uint32_t tgcstart = 0x0;
    uint32_t cscstart = 0x0;
    
    // set the data size 
    uint32_t fragSize = 0x0;
    if (m_mdt) { mdtstart = EV0HDRSIZE+fragSize; fragSize += m_mdt->size();}
    if (m_rpc) { rpcstart = EV0HDRSIZE+fragSize; fragSize += m_rpc->size();}
    if (m_tgc) { tgcstart = EV0HDRSIZE+fragSize; fragSize += m_tgc->size();}
    //if (m_csc) { cscstart = EVTHDRSIZE+fragSize; fragSize += m_csc->size();}

    uint8_t* word1 = buffer;
    uint8_t* word2 = buffer + 1*WRDSIZE;
    uint8_t* word3 = buffer + 2*WRDSIZE;
    uint8_t* word4 = buffer + 3*WRDSIZE;
    uint8_t* word5 = buffer + 4*WRDSIZE;
    uint8_t* word6 = buffer + 5*WRDSIZE;
    uint8_t* word7 = buffer + 6*WRDSIZE;
    uint8_t* word8 = buffer + 7*WRDSIZE;
    uint8_t* word9 = buffer + 8*WRDSIZE; 
   

    charsFromLitEndWord(bit16IntoWord(con,tag),word1);
    charsFromLitEndWord(this->size(),word2);
    charsFromLitEndWord(bit16IntoWord(phi,eta),word3);
    charsFromLitEndWord(pt,word4);
    charsFromLitEndWord(mdtstart,word5);
    charsFromLitEndWord(rpcstart,word6);
    charsFromLitEndWord(tgcstart,word7);
    charsFromLitEndWord(cscstart,word8);
    charsFromLitEndWord(fragSize,word9);
    
    lenght -= EV0HDRSIZE;
    
    if(m_mdt) {m_mdt->dumpWords(buffer+mdtstart,lenght);}
    if(m_rpc) {m_rpc->dumpWords(buffer+rpcstart,lenght);}
    if(m_tgc) {m_tgc->dumpWords(buffer+tgcstart,lenght);}
    //if(m_csc) {lenght -= m_csc->dumpWords(buffer+cscstart,lenght);}
    
    
    uint8_t* trailer = buffer+fragSize+EV0HDRSIZE;
    charsFromLitEndWord(TRLTAG,trailer);
    charsFromLitEndWord(bit16IntoWord(tag,con),trailer+1*WRDSIZE);
    
    lenght -= TRLSIZE;
    return this->size();
}



int 
CalibEvent::encode_v1(uint8_t* buffer,uint16_t& lenght) const {

    // set the tag and the event type
    uint16_t tag = EVTTAG;
    uint16_t con = 0x0;
    if (m_mdt) con |= 0x0001;
    if (m_rpc) con |= 0x0010;
    if (m_tgc) con |= 0x0100;


    //get name size
    uint8_t instance_name_size = name_size();


    //encode ROB IDs
    uint16_t trigger_mrod = (m_mrod_trigger>>8)|(m_mrod_trigger&0xff);
    uint16_t data_mrod[4];
    for(int i=0;i<4;++i) data_mrod[i]=(m_mrod_data[i]>>8)|(m_mrod_data[i]&0xff);


    // encode the eta phi and pt values
    int16_t eta = (int) (m_eta*10000.);
    int16_t phi = (int) (m_phi*10000.);
    int32_t pt  = (int) (m_pt*1000.);


    //reset counters for the data payloads
    uint16_t mdtstart = 0x0;
    uint16_t rpcstart = 0x0;
    uint16_t tgcstart = 0x0;
    uint16_t cscstart = 0x0;

    //set the counters for data payloads
    uint16_t fragSize = instance_name_size;
    if (m_mdt) { mdtstart = EV1HDRSIZE+fragSize; fragSize += m_mdt->size();}
    if (m_rpc) { rpcstart = EV1HDRSIZE+fragSize; fragSize += m_rpc->size();}
    if (m_tgc) { tgcstart = EV1HDRSIZE+fragSize; fragSize += m_tgc->size();}
    //if (m_csc) { cscstart = EVTHDRSIZE+fragSize; fragSize += m_csc->size();}

    uint8_t* word1  = buffer;
    uint8_t* word2  = buffer +  1*WRDSIZE;
    uint8_t* word3  = buffer +  2*WRDSIZE;
    uint8_t* word4  = buffer +  3*WRDSIZE;
    uint8_t* word5  = buffer +  4*WRDSIZE;
    uint8_t* word6  = buffer +  5*WRDSIZE;
    uint8_t* word7  = buffer +  6*WRDSIZE;
    uint8_t* word8  = buffer +  7*WRDSIZE;
    uint8_t* word9  = buffer +  8*WRDSIZE;
    uint8_t* word10 = buffer +  9*WRDSIZE;
    uint8_t* word11 = buffer + 10*WRDSIZE;
    uint8_t* word12 = buffer + 11*WRDSIZE;
    uint8_t* word13 = buffer + 12*WRDSIZE;
    uint8_t* word14 = buffer + 13*WRDSIZE;


    charsFromLitEndWord(bit16IntoWord(con,tag),word1);
    charsFromLitEndWord(bit16IntoWord(this->size(),m_version),word2);
    charsFromLitEndWord(m_run_number,word3);
    charsFromLitEndWord(m_lvl1_id,word4);
    charsFromLitEndWord(m_timestamp,word5);
    charsFromLitEndWord(m_ip_address,word6);
    charsFromLitEndWord(bit16IntoWord(trigger_mrod,m_l2pu_id),word7);
    charsFromLitEndWord(bit16IntoWord(data_mrod[1],data_mrod[0]),word8);
    charsFromLitEndWord(bit16IntoWord(data_mrod[3],data_mrod[2]),word9);
    
    charsFromLitEndWord(bit16IntoWord(phi,eta),word10);
    charsFromLitEndWord(pt,word11);
    charsFromLitEndWord(bit16IntoWord(rpcstart,mdtstart),word12);
    charsFromLitEndWord(bit16IntoWord(tgcstart,cscstart),word13);
    charsFromLitEndWord(fragSize,word14);
    
    //encode name
    uint8_t* word15 = buffer + 14*WRDSIZE;
    copyChars(&instance_name_size,word15,1);
    copyChars((const uint8_t*)m_name,word15+1,instance_name_size-1);
    
    
    lenght -= EV1HDRSIZE + instance_name_size;
    
    if(m_mdt) {m_mdt->dumpWords(buffer+mdtstart,lenght);}
    if(m_rpc) {m_rpc->dumpWords(buffer+rpcstart,lenght);}
    if(m_tgc) {m_tgc->dumpWords(buffer+tgcstart,lenght);}
    //if(m_csc) {lenght -= m_csc->dumpWords(buffer+cscstart,lenght);}
    
    
    uint8_t* trailer = buffer+fragSize+EV1HDRSIZE;
    charsFromLitEndWord(TRLTAG,trailer);
    charsFromLitEndWord(bit16IntoWord(tag,con),trailer+1*WRDSIZE);
    
    lenght -= TRLSIZE;
    return this->size();
}



int 
CalibEvent::dumpWords(uint8_t* buffer,uint16_t& lenght) const {

    if( this->size() > lenght ) return 0;

    switch(m_version) {
        case 0:  return encode_v0(buffer,lenght);  break;
	case 1:  return encode_v1(buffer,lenght);  break;
	
	default: return 0; break;   
    
    }
    
    return 0;
}



/*
std::ostream  &LVL2CALIBRATION::operator<< (std::ostream& stream, 
                                            const CalibEvent& data) {

    uint16_t size = data.size();
    uint8_t* buffer = new uint8_t[size];
    
    data.dumpWords(buffer,size);
    
    uint32_t word1 = wordFromLitEndChars(buffer);
    uint32_t word2 = wordFromLitEndChars(buffer+1*WRDSIZE);
    uint32_t word3 = wordFromLitEndChars(buffer+2*WRDSIZE);
    uint32_t word4 = wordFromLitEndChars(buffer+3*WRDSIZE);
    uint32_t word5 = wordFromLitEndChars(buffer+4*WRDSIZE);
    uint32_t word6 = wordFromLitEndChars(buffer+5*WRDSIZE);
    uint32_t word7 = wordFromLitEndChars(buffer+6*WRDSIZE);
    uint32_t word8 = wordFromLitEndChars(buffer+7*WRDSIZE);
    uint32_t word9 = wordFromLitEndChars(buffer+8*WRDSIZE);
    
    uint8_t* trailer  = buffer+word9+EV0HDRSIZE;
    uint8_t* lastword = buffer+word2-1*WRDSIZE;
    
    uint32_t tral1 = wordFromLitEndChars(lastword);
    
    uint16_t  tag,con,tag2,con2;
    
    wordIntoBit16(word1,con,tag);
    wordIntoBit16(tral1,tag2,con2);
    
    std::ostringstream tmp;
    if ( con&0x0001 ) { tmp << " mdt"; }
    if ( con&0x0010 ) { tmp << " rpc"; }
    if ( con&0x0100 ) { tmp << " tgc"; }
    if ( con&0x1000 ) { tmp << " csc"; }

    uint16_t version,data_size;
    wordIntoBit16(word2,data_size,version);

    LVL2CALIBRATION::printWord(stream,word1);
    stream << "CALIBRATION EVENT DATA,  containing" << tmp.str() << std::endl;
    LVL2CALIBRATION::printWord(stream,word2);   
    stream << "version=" << version << ", total size in bytes=" << data_size
           << std::endl;
	   
    LVL2CALIBRATION::printWord(stream,word3);
    stream << "eta=" << data.eta() << ",  phi=" << data.phi() << std::endl;
    LVL2CALIBRATION::printWord(stream,word4);
    stream << "pt=" << data.pt() << std::endl;
    LVL2CALIBRATION::printWord(stream,word5);
    stream << "mdt data at byte " << word5 << std::endl;
    LVL2CALIBRATION::printWord(stream,word6);
    stream << "rpc data at byte " << word6 << std::endl;
    LVL2CALIBRATION::printWord(stream,word7);
    stream << "tgc data at byte " << word7 << std::endl;
    LVL2CALIBRATION::printWord(stream,word8);
    stream << "csc data at byte " << word8 << std::endl;
    LVL2CALIBRATION::printWord(stream,word9);
    stream << "data size in bytes=" << word9 << " bytes" << std::endl;
    
    MdtCalibFragment* mdt = data.mdt();
    RpcCalibFragment* rpc = data.rpc();
    if(mdt) stream << *mdt;
    if(rpc) stream << *rpc;
    //if(m_tgc) stream << *m_tgc;
    //if(m_csc) stream << *m_csc;
    
    
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(trailer));
    stream << "Trailer tag" << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(lastword));
    stream << "END OF CALIB DATA EVENT,  containing " << tmp.str() << std::endl;
    
    delete[] buffer;
        
    return stream;
}
*/

std::ostream  &LVL2CALIBRATION::operator<< (std::ostream& stream, 
                                            const CalibEvent& data) {

    uint16_t size = data.size();
    uint8_t* buffer = new uint8_t[size];
    
    data.dumpWords(buffer,size);
    

    // print the very early header section
    uint32_t word1 = wordFromLitEndChars(buffer);
    uint32_t word2 = wordFromLitEndChars(buffer+1*WRDSIZE);

    uint16_t  tag,con;
    wordIntoBit16(word1,con,tag);
    
    std::ostringstream tmp;
    if ( con&0x0001 ) { tmp << " mdt"; }
    if ( con&0x0010 ) { tmp << " rpc"; }
    if ( con&0x0100 ) { tmp << " tgc"; }
    if ( con&0x1000 ) { tmp << " csc"; }

    uint16_t version,data_size;
    wordIntoBit16(word2,data_size,version);

    LVL2CALIBRATION::printWord(stream,word1);
    stream << "CALIBRATION EVENT DATA,  containing" << tmp.str() << std::endl;
    LVL2CALIBRATION::printWord(stream,word2);   
    stream << "version=" << version << ", total size in bytes=" << data_size
           << std::endl;
   
    uint8_t* trailer      = 0x0;
    uint8_t* lastword     = buffer+data_size-1*WRDSIZE;
    
    if (data.version()==1) {
        uint32_t run_number    = wordFromLitEndChars(buffer+2*WRDSIZE);
        uint32_t lvl1_id       = wordFromLitEndChars(buffer+3*WRDSIZE);
        uint32_t timestamp     = wordFromLitEndChars(buffer+4*WRDSIZE);
	uint32_t ip_address    = wordFromLitEndChars(buffer+5*WRDSIZE);
        uint32_t l2puid_tmrod  = wordFromLitEndChars(buffer+6*WRDSIZE);
	uint32_t data_mrod1    = wordFromLitEndChars(buffer+7*WRDSIZE);
        uint32_t data_mrod2    = wordFromLitEndChars(buffer+8*WRDSIZE);
        uint32_t eta_phi_data  = wordFromLitEndChars(buffer+9*WRDSIZE);
        uint32_t pt_data       = wordFromLitEndChars(buffer+10*WRDSIZE);
	uint32_t rpc_mdt_start = wordFromLitEndChars(buffer+11*WRDSIZE);
        uint32_t tgc_csc_start = wordFromLitEndChars(buffer+12*WRDSIZE);
        uint32_t payload_size  = wordFromLitEndChars(buffer+13*WRDSIZE);
	
	trailer       = buffer+payload_size+EV1HDRSIZE;
        
	uint16_t rpc_start,mdt_start,tgc_start,csc_start;
	wordIntoBit16(rpc_mdt_start,rpc_start,mdt_start);
	wordIntoBit16(tgc_csc_start,tgc_start,csc_start);
	
	
        LVL2CALIBRATION::printWord(stream,run_number);
        stream << "Run number=" << data.run_number() << std::endl;
	LVL2CALIBRATION::printWord(stream,lvl1_id);
        stream << "LVL1 ID=" << data.lvl1_id() << std::endl;
	LVL2CALIBRATION::printWord(stream,timestamp);
        stream << data.timestamp_str();
	LVL2CALIBRATION::printWord(stream,ip_address);
        stream << "IP=" << data.ip_address_str() << std::endl;
	
	LVL2CALIBRATION::printWord(stream,l2puid_tmrod);
        stream << "L2PUID=" << data.l2pu_id() << ",  trigger ROD=" << std::hex 
	       << std::setiosflags(std::ios::showbase) 
	       << data.mrod_trigger() << std::dec << std::noshowbase << std::endl;
	
	
	LVL2CALIBRATION::printWord(stream,data_mrod1);
	stream << "data MROD1=" << std::hex 
	       << std::setiosflags(std::ios::showbase)
	       << data.mrod_data(0) << ",  data MROD2=" << data.mrod_data(1) 
	       << std::dec << std::noshowbase << std::endl;
        LVL2CALIBRATION::printWord(stream,data_mrod2);
	stream << "data MROD3=" << std::hex 
	       << std::setiosflags(std::ios::showbase)
	       << data.mrod_data(2) << ",  data MROD4=" << data.mrod_data(3) 
	       << std::dec << std::noshowbase << std::endl;
        
	LVL2CALIBRATION::printWord(stream,eta_phi_data);
        stream << "eta=" << data.eta() << ",  phi=" << data.phi() << std::endl;
        LVL2CALIBRATION::printWord(stream,pt_data);
        stream << "pt=" << data.pt() << std::endl;
        LVL2CALIBRATION::printWord(stream,rpc_mdt_start);
        stream << "mdt data at byte " << mdt_start 
	       << ",  rpc data at byte " << rpc_start << std::endl;
	LVL2CALIBRATION::printWord(stream,tgc_csc_start);
        stream << "csc data at byte " << csc_start 
	       << ",  tgc data at byte " << tgc_start << std::endl;
        
	LVL2CALIBRATION::printWord(stream,payload_size);
        stream << "data size in bytes=" << payload_size << " bytes" 
	       << std::endl;
        
	uint8_t name_size = *(buffer+EV1HDRSIZE);
	for(int i=0;i<name_size/4;++i) {
	    uint32_t word = wordFromLitEndChars(buffer+(14+i)*WRDSIZE);
	    LVL2CALIBRATION::printWord(stream,word);
	    if (i<(name_size/4-1)) stream << std::endl;
        }
	stream << "instance name=" << data.name() << std::endl;
    }
    
    
    
    if (data.version()==0) {
	uint32_t eta_phi_data = wordFromLitEndChars(buffer+2*WRDSIZE);
	uint32_t pt_data      = wordFromLitEndChars(buffer+3*WRDSIZE);
	uint32_t payload_size = wordFromLitEndChars(buffer+8*WRDSIZE);
	
	uint32_t word5 = wordFromLitEndChars(buffer+4*WRDSIZE);
	uint32_t word6 = wordFromLitEndChars(buffer+5*WRDSIZE);
        uint32_t word7 = wordFromLitEndChars(buffer+6*WRDSIZE);
        uint32_t word8 = wordFromLitEndChars(buffer+7*WRDSIZE);
	
	trailer      = buffer+payload_size+EV0HDRSIZE;
	
	LVL2CALIBRATION::printWord(stream,eta_phi_data);
        stream << "eta=" << data.eta() << ",  phi=" << data.phi() << std::endl;
        LVL2CALIBRATION::printWord(stream,pt_data);
        stream << "pt=" << data.pt() << std::endl;
        LVL2CALIBRATION::printWord(stream,word5);
        stream << "mdt data at byte " << word5 << std::endl;
        LVL2CALIBRATION::printWord(stream,word6);
        stream << "rpc data at byte " << word6 << std::endl;
        LVL2CALIBRATION::printWord(stream,word7);
        stream << "tgc data at byte " << word7 << std::endl;
        LVL2CALIBRATION::printWord(stream,word8);
        stream << "csc data at byte " << word8 << std::endl;
        LVL2CALIBRATION::printWord(stream,payload_size);
        stream << "data size in bytes=" << payload_size << " bytes" << std::endl;
    }
 
   
    MdtCalibFragment* mdt = data.mdt();
    RpcCalibFragment* rpc = data.rpc();
    TgcCalibFragment* tgc = data.tgc();
    if(mdt) stream << *mdt;
    if(rpc) stream << *rpc;
    if(tgc) stream << *tgc;
    //if(m_csc) stream << *m_csc;
    
    
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(trailer));
    stream << "Trailer tag" << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(lastword));
    stream << "END OF CALIB DATA EVENT,  containing " << tmp.str() << std::endl;
    
    delete[] buffer;
        
    return stream;
}
