/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/CalibFragment.h"
#include "TrigmuFast/CalibUti.h"

using namespace LVL2CALIBRATION;


MdtCalibFragment::MdtCalibFragment() {
    m_data.clear();
}


MdtCalibFragment::MdtCalibFragment(const MdtCalibFragment& event) : CalibData()
{
    m_data.clear();
    m_data = event.data();
}


MdtCalibFragment::MdtCalibFragment(uint8_t* data) {
    if ( checkIntegrity(data) ) {
    
        uint32_t word1 = wordFromLitEndChars(data);
        uint16_t num,tag;
	wordIntoBit16(word1,num,tag); 
        
	MdtCalibData mdt;
	
	for (uint16_t i=0;i<num;++i) {
	    MdtCalibData tube(data+MDTHDRSIZE+i*mdt.size());
	    *this << tube;
	}
	
	return;
    }
    m_data.clear();
}


bool
MdtCalibFragment::checkIntegrity(uint8_t* buffer) const {
    
    uint32_t word1 = wordFromLitEndChars(buffer);
    uint32_t word2 = wordFromLitEndChars(buffer+1*WRDSIZE);
    uint32_t word3 = wordFromLitEndChars(buffer+2*WRDSIZE);
    uint32_t tral1 = wordFromLitEndChars(buffer+word3+MDTHDRSIZE);
    uint32_t tral2 = wordFromLitEndChars(buffer+word2-1*WRDSIZE);
    
    if( tral1!=TRLTAG ) return false;
    
    uint16_t  tag,num;
    MdtCalibData mdt;
    
    wordIntoBit16(word1,num,tag);
    uint32_t dataSize = num*(mdt.size());    
    if ( word2!=dataSize+MDTHDRSIZE+TRLSIZE) return false;
    if ( word3!=dataSize ) return false;
    
    uint16_t tag2,num2;
    wordIntoBit16(tral2,tag2,num2);
    if( tag!=tag2 || num!=num2 ) return false;
    
        
    return true;
}


MdtCalibFragment&
MdtCalibFragment::operator<<(const MdtCalibData& tube) {
    m_data.push_back(tube);
    return *this;
}

int
MdtCalibFragment::dumpWords(uint8_t* buffer,uint16_t& lenght) const {
    
    if ( lenght<this->size() ) return 0;
    
    MdtCalibData mdt;
    
    uint16_t num = m_data.size();
    uint32_t dataSize = num*mdt.size();
    uint32_t totSize  = dataSize + MDTHDRSIZE + TRLSIZE;
    
    uint8_t* word1 = buffer;
    uint8_t* word2 = buffer+1*WRDSIZE;
    uint8_t* word3 = buffer+2*WRDSIZE;
    uint8_t* start = buffer+3*WRDSIZE;
    uint8_t* trail = start + dataSize;
    
    charsFromLitEndWord(bit16IntoWord(num,MDTTAG),word1);
    charsFromLitEndWord(totSize,word2);
    charsFromLitEndWord(dataSize,word3);
    
    lenght -= MDTHDRSIZE;
    
    std::list<MdtCalibData>::const_iterator it = m_data.begin();
    while( it!=m_data.end() ) {
        (*it).dumpWords(start,lenght);
	start += mdt.size();
	++it;
    }
    
    charsFromLitEndWord(TRLTAG,trail);
    charsFromLitEndWord(bit16IntoWord(MDTTAG,num),trail+1*WRDSIZE);
    
    lenght -= TRLSIZE;
    return this->size();
}


std::ostream  &LVL2CALIBRATION::operator<<(std::ostream& stream,
                                const MdtCalibFragment& data) {

    uint16_t size = data.size();
    uint8_t* buffer = new uint8_t[size];
    
    data.dumpWords(buffer,size);
    
    uint32_t word1 = wordFromLitEndChars(buffer);
    uint32_t word2 = wordFromLitEndChars(buffer+1*WRDSIZE);
    uint32_t word3 = wordFromLitEndChars(buffer+2*WRDSIZE);
    uint8_t* trailer  = buffer+word3+MDTHDRSIZE;
    uint8_t* lastword = buffer+word2-1*WRDSIZE;
    uint32_t tral1 = wordFromLitEndChars(lastword);
    
    uint16_t  tag,num,tag2,num2;
    
    wordIntoBit16(word1,num,tag);
    wordIntoBit16(tral1,tag2,num2);

    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer));
    stream << "  MDT FRAGMENT,  number of hits= " << num << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+1*WRDSIZE));   
    stream << "  Total size in bytes=" << word2 << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+2*WRDSIZE));
    stream << "  Mdt data size in bytes=" << word3 << std::endl;
    
    std::list<MdtCalibData> tubes = data.data();
    std::list<MdtCalibData>::const_iterator it = tubes.begin();
    while( it!=tubes.end() ) {
        stream << *it;
        ++it;
    }
    
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(trailer));
    stream << "  Trailer tag" << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(lastword));
    stream << "  END OF MDT FRAGMENT,  number of hits=" << num2 << std::endl;
    
    delete[] buffer;

    return stream;
}



RpcCalibFragment::RpcCalibFragment() : m_subsystemId(0),m_sectorId(0),
                                       m_padId(0),m_status(0),m_error_code(0) {
    m_data.clear();
}

RpcCalibFragment::RpcCalibFragment(uint16_t subsystemId,uint16_t sectorId,
    uint16_t padId,uint16_t status,uint16_t error_code) :
    m_subsystemId(subsystemId),m_sectorId(sectorId),m_padId(padId),
    m_status(status),m_error_code(error_code) {
    if ( (m_padId&      0xfff0) != 0x0) m_padId      = 0x0;
    if ( (m_status&     0xff00) != 0x0) m_status     = 0x0;
    if ( (m_error_code& 0xf000) != 0x0) m_error_code = 0x0;
    m_data.clear();
}

RpcCalibFragment::RpcCalibFragment(const RpcCalibFragment& event):CalibData() {
    m_subsystemId = event.subsystemId();
    m_sectorId = event.sectorId();
    m_padId = event.padId();
    m_status = event.status();
    m_error_code = event.error_code();
    m_data.clear();
    m_data = event.data();
}


RpcCalibFragment::RpcCalibFragment(uint8_t* data) {
    if ( checkIntegrity(data) ) {
    
        uint32_t word1 = wordFromLitEndChars(data);
        uint16_t num,tag;
	wordIntoBit16(word1,num,tag);
	uint32_t word2 = wordFromLitEndChars(data+1*WRDSIZE);
	wordIntoBit16(word2,m_sectorId,m_subsystemId);
	uint32_t word3 = wordFromLitEndChars(data+2*WRDSIZE);
        
	m_padId  = (word3 >> 28);
	m_status = (word3 >> 16)&0xff;
	m_error_code = word3&0xfff;
	
	uint16_t rpc_data_size = 0;
	
	for (uint16_t i=0;i<num;++i) {
	    RpcCalibData matrix(data+RPCHDRSIZE+rpc_data_size);
	    *this << matrix;
	    rpc_data_size += matrix.size();
	}
	
	return;
    }
    m_padId  = 0;
    m_status = 0;
    m_error_code = 0;
    m_data.clear();
}


bool
RpcCalibFragment::checkIntegrity(uint8_t* buffer) const {
    
    uint32_t word1 = wordFromLitEndChars(buffer);
    uint32_t word4 = wordFromLitEndChars(buffer+3*WRDSIZE);
    uint32_t word5 = wordFromLitEndChars(buffer+4*WRDSIZE);
    uint32_t tral1 = wordFromLitEndChars(buffer+word5+RPCHDRSIZE);
    uint32_t tral2 = wordFromLitEndChars(buffer+word4-1*WRDSIZE);
    
    if( tral1!=TRLTAG ) return false;
    
    uint16_t  tag,num;
    //MdtCalibData mdt;
    
    wordIntoBit16(word1,num,tag);
    uint32_t dataSize = 0;
    
    for (uint16_t i=0;i<num;++i) {
	RpcCalibData matrix(buffer+RPCHDRSIZE+dataSize);
	dataSize += matrix.size();
    }
    
    if ( word4!=dataSize+RPCHDRSIZE+TRLSIZE) return false;
    if ( word5!=dataSize ) return false;
    
    uint16_t tag2,num2;
    wordIntoBit16(tral2,tag2,num2);
    if( tag!=tag2 || num!=num2 ) return false;
    
        
    return true;
}


RpcCalibFragment&
RpcCalibFragment::operator<<(const RpcCalibData& matrix) {
    m_data.push_back(matrix);
    return *this;
}

int
RpcCalibFragment::dumpWords(uint8_t* buffer,uint16_t& lenght) const {
    
    if ( lenght<this->size() ) return 0;
    
    
    uint16_t num = m_data.size();
    uint32_t dataSize = 0;
    std::list<RpcCalibData>::const_iterator it = m_data.begin();
    while( it != m_data.end() ) {
        dataSize += (*it).size();
	++it;
    }
    
    
    uint32_t totSize  = dataSize + RPCHDRSIZE + TRLSIZE;
    
    uint8_t* word1 = buffer;
    uint8_t* word2 = buffer+1*WRDSIZE;
    uint8_t* word3 = buffer+2*WRDSIZE;
    uint8_t* word4 = buffer+3*WRDSIZE;
    uint8_t* word5 = buffer+4*WRDSIZE; 
    uint8_t* start = buffer+5*WRDSIZE;
    uint8_t* trail = start + dataSize;
    
    uint32_t data = (m_padId << 28) | (m_status << 16) | m_error_code;
    
    charsFromLitEndWord(bit16IntoWord(num,RPCTAG),word1);
    charsFromLitEndWord(bit16IntoWord(m_sectorId,m_subsystemId),word2);
    charsFromLitEndWord(data,word3);
    charsFromLitEndWord(totSize,word4);
    charsFromLitEndWord(dataSize,word5);
    
    lenght -= RPCHDRSIZE;
    
    it = m_data.begin();
    while( it!=m_data.end() ) {
        (*it).dumpWords(start,lenght);
	//std::cout << lenght << std::endl;
	start += (*it).size();
	++it;
    }
    
    charsFromLitEndWord(TRLTAG,trail);
    charsFromLitEndWord(bit16IntoWord(RPCTAG,num),trail+1*WRDSIZE);
    
    lenght -= TRLSIZE;
    return this->size();
}


std::ostream  &LVL2CALIBRATION::operator<<(std::ostream& stream,
                                const RpcCalibFragment& data) {

    uint16_t size = data.size();
    uint8_t* buffer = new uint8_t[size];
    
    data.dumpWords(buffer,size);
    
    uint32_t word1 = wordFromLitEndChars(buffer);
    //uint32_t word2 = wordFromLitEndChars(buffer+1*WRDSIZE);
    uint32_t word3 = wordFromLitEndChars(buffer+2*WRDSIZE);
    uint32_t word4 = wordFromLitEndChars(buffer+3*WRDSIZE);
    uint32_t word5 = wordFromLitEndChars(buffer+4*WRDSIZE);
    uint8_t* trailer  = buffer+word5+RPCHDRSIZE;
    uint8_t* lastword = buffer+word4-1*WRDSIZE;
    uint32_t tral1 = wordFromLitEndChars(lastword);
    
    uint16_t  tag,num,tag2,num2;
    wordIntoBit16(word1,num,tag);
    wordIntoBit16(tral1,tag2,num2);
    
    uint16_t padId  = (word3 >> 28); 
    uint16_t status = (word3 >> 16)&0xff;
    uint16_t error_code = word3&0xfff;

    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer));
    stream << "  RPC FRAGMENT,  number of matrices= " << num << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+1*WRDSIZE));
    stream << "  SUBYSID = " << data.subsystemId() << ",  sector ID = " 
           << data.sectorId() << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+2*WRDSIZE));
    stream << "  PAD ID = " << padId << ",  status word = " << status 
           << ",  error code = " << error_code << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+3*WRDSIZE));   
    stream << "  Total size in bytes=" << word4 << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+4*WRDSIZE));
    stream << "  RPC data size in bytes=" << word5 << std::endl;
    
    std::list<RpcCalibData> matrices = data.data();
    std::list<RpcCalibData>::const_iterator it = matrices.begin();
    while( it!=matrices.end() ) {
        stream << *it;
        ++it;
    }
    
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(trailer));
    stream << "  Trailer tag" << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(lastword));
    stream << "  END OF RPC FRAGMENT,  number of matrices=" << num2 << std::endl;
    
    delete[] buffer;
    
    return stream;
}


TgcCalibFragment::TgcCalibFragment() : m_systemId(0),m_subsystemId(0),
                                       m_sectorId(0),m_RoInumber(0)
{
    m_data.clear();
}

TgcCalibFragment::TgcCalibFragment(uint16_t systemId, uint16_t subsystemId,
    uint16_t sectorId, uint16_t roiNumber) :
    m_systemId(systemId), m_subsystemId(subsystemId), m_sectorId(sectorId),
    m_RoInumber(roiNumber)
{
    m_data.clear();
}

TgcCalibFragment::TgcCalibFragment(const TgcCalibFragment& event):CalibData(){
    m_systemId = event.systemId();
    m_subsystemId = event.subsystemId();
    m_sectorId = event.sectorId();
    m_RoInumber = event.roiNumber();
    m_data.clear();
    m_data = event.data();
}


TgcCalibFragment::TgcCalibFragment(uint8_t* data) {
    if ( checkIntegrity(data) ) 
    {
        uint32_t word1 = wordFromLitEndChars(data);
        uint16_t num,tag;
	wordIntoBit16(word1,num,tag);
	uint32_t word2 = wordFromLitEndChars(data+1*WRDSIZE);
	wordIntoBit16(word2,m_subsystemId,m_systemId);
	uint32_t word3 = wordFromLitEndChars(data+2*WRDSIZE);
	wordIntoBit16(word3,m_RoInumber,m_sectorId);
	
	uint16_t tgc_data_size = 0;
	
	for (uint16_t i=0;i<num;++i) {
	    TgcCalibData hit(data+TGCHDRSIZE+tgc_data_size);
	    *this << hit;
	    tgc_data_size += hit.size();
	}
	
     
	return;
    }
 
    m_systemId = 0;
    m_subsystemId = 0;
    m_sectorId = 0;
    m_RoInumber  = 0;
    m_data.clear();
}


bool
TgcCalibFragment::checkIntegrity(uint8_t* buffer) const 
{
    uint32_t word1 = wordFromLitEndChars(buffer);
    uint32_t word4 = wordFromLitEndChars(buffer+3*WRDSIZE);
    uint32_t word5 = wordFromLitEndChars(buffer+4*WRDSIZE);
    uint32_t tral1 = wordFromLitEndChars(buffer+word5+TGCHDRSIZE);
    uint32_t tral2 = wordFromLitEndChars(buffer+word4-1*WRDSIZE);
    
    if( tral1!=TRLTAG ) return false;

    uint16_t  tag,num;
    //TgcCalibData tgc_hit;
    
    wordIntoBit16(word1,num,tag);
    uint32_t dataSize = 0;
    
    for (uint16_t i=0;i<num;++i) {
	TgcCalibData tgc_rod(buffer+TGCHDRSIZE+dataSize);
	dataSize += tgc_rod.size();
    }

    if ( word4!=dataSize+TGCHDRSIZE+TRLSIZE) return false;
    if ( word5!=dataSize ) return false;
    
    uint16_t tag2,num2;
    wordIntoBit16(tral2,tag2,num2);
    if( tag!=tag2 || num!=num2 ) return false;
    
   
    return true;
}


TgcCalibFragment&
TgcCalibFragment::operator<<(const TgcCalibData& hit) {
    m_data.push_back(hit);
    return *this;
}

int
TgcCalibFragment::dumpWords(uint8_t* buffer,uint16_t& lenght) const {
    
    if ( lenght<this->size() ) return 0;
    

    uint16_t num = m_data.size();
    uint32_t dataSize = 0;
    std::list<TgcCalibData>::const_iterator it = m_data.begin();
    while( it != m_data.end() ) {
        dataSize += (*it).size();
	++it;
    }
    
    
    uint32_t totSize  = dataSize + TGCHDRSIZE + TRLSIZE;

    uint8_t* word1 = buffer;
    uint8_t* word2 = buffer+1*WRDSIZE;
    uint8_t* word3 = buffer+2*WRDSIZE;
    uint8_t* word4 = buffer+3*WRDSIZE;
    uint8_t* word5 = buffer+4*WRDSIZE; 
    uint8_t* start = buffer+5*WRDSIZE;
    uint8_t* trail = start + dataSize;
    

    charsFromLitEndWord(bit16IntoWord(num,TGCTAG),word1);
    charsFromLitEndWord(bit16IntoWord(m_subsystemId,m_systemId),word2);
    charsFromLitEndWord(bit16IntoWord(m_RoInumber,m_sectorId),word3);
    charsFromLitEndWord(totSize,word4);
    charsFromLitEndWord(dataSize,word5);
    
    
    lenght -= TGCHDRSIZE;
    
    it = m_data.begin();
    while( it!=m_data.end() ) {
        (*it).dumpWords(start,lenght);
	//std::cout << lenght << std::endl;
	start += (*it).size();
	++it;
    }
    
    charsFromLitEndWord(TRLTAG,trail);
    charsFromLitEndWord(bit16IntoWord(TGCTAG,num),trail+1*WRDSIZE);
    
    lenght -= TRLSIZE;

    return this->size();
}


std::ostream  &LVL2CALIBRATION::operator<<(std::ostream& stream,
                                const TgcCalibFragment& data) {

    uint16_t size = data.size();
    uint8_t* buffer = new uint8_t[size];
    
    data.dumpWords(buffer,size);
    
    uint32_t word1 = wordFromLitEndChars(buffer);
    //uint32_t word2 = wordFromLitEndChars(buffer+1*WRDSIZE);
    //uint32_t word3 = wordFromLitEndChars(buffer+2*WRDSIZE);
    uint32_t word4 = wordFromLitEndChars(buffer+3*WRDSIZE);
    uint32_t word5 = wordFromLitEndChars(buffer+4*WRDSIZE);
    uint8_t* trailer  = buffer+word5+TGCHDRSIZE;
    uint8_t* lastword = buffer+word4-1*WRDSIZE;
    uint32_t tral1 = wordFromLitEndChars(lastword);
    
    uint16_t  tag,num,tag2,num2;
    wordIntoBit16(word1,num,tag);
    wordIntoBit16(tral1,tag2,num2);
    
    //uint16_t padId  = (word3 >> 28); 
    //uint16_t status = (word3 >> 16)&0xff;
    //uint16_t error_code = word3&0xfff;

    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer));
    stream << "  TGC FRAGMENT,  number of TGC ROD= " << num << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+1*WRDSIZE));
    stream << "  SYSTEMID = " << data.systemId() << ",  SUBYSID = " 
           << data.subsystemId() << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+2*WRDSIZE));
    stream << "  ROD ID = " << data.sectorId() << ",  ROI NUM = " 
           << data.roiNumber() << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+3*WRDSIZE));   
    stream << "  Total size in bytes=" << word4 << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+4*WRDSIZE));
    stream << "  TGC data size in bytes=" << word5 << std::endl;
    
    std::list<TgcCalibData> hit = data.data();
    std::list<TgcCalibData>::const_iterator it = hit.begin();
    while( it!=hit.end() ) {
        stream << *it;
        ++it;
    }
    
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(trailer));
    stream << "  Trailer tag" << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(lastword));
    stream << "  END OF TGC FRAGMENT,  number of hits=" << num2 << std::endl;
    
    delete[] buffer;
    
    return stream;
}
