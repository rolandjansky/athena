/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/CalibData.h"
#include "TrigmuFast/CalibUti.h"
#include "MuonRDO/TgcRawData.h"

#include <math.h>
#include <iomanip>
#include <sstream>

using namespace LVL2CALIBRATION;

CalibData::CalibData() {}

CalibData::~CalibData() {}

std::ostream& LVL2CALIBRATION::operator<<(std::ostream& stream, const CalibData* data) {

    uint16_t size = data->size();
    
    uint8_t* buff = new uint8_t[size];
    
    uint16_t written = data->dumpWords(buff,size);
        
    stream.write( (char *)buff,written*sizeof(uint8_t) );
    
    delete[] buff;
    
    return stream;
    
}



MdtCalibData::MdtCalibData(): m_id(0),m_leadingCoarse(0),m_leadingFine(0),
                          m_trailingCoarse(0),m_trailingFine(0),m_width(0),
			  m_phi(0) {}

MdtCalibData::MdtCalibData(uint32_t word, uint16_t leadingCoarse, 
                           uint16_t leadingFine, uint16_t trailingCoarse, 
	                   uint16_t trailingFine, uint16_t width, float phi) {
    
    int StationName,StationEta,StationPhi,MultiLayer,Layer,Tube;
    WordIntoMdtId(word,StationName,StationEta,StationPhi,MultiLayer,Layer,Tube);     
   
    if (StationName>=0 && StationPhi!=0) {
        m_id             = word;
        m_leadingCoarse  = leadingCoarse;
        m_leadingFine    = leadingFine;
        m_trailingCoarse = trailingCoarse;
        m_trailingFine   = trailingFine;
        m_width          = width;
	m_phi            = phi;
        return;
    }
    m_id             = 0;
    m_leadingCoarse  = 0;
    m_leadingFine    = 0;
    m_trailingCoarse = 0;
    m_trailingFine   = 0;
    m_width          = 0;
    m_phi            = 0; 
}

MdtCalibData::MdtCalibData(const MdtCalibData& data) : CalibData() {

    m_id             = data.id();
    m_leadingCoarse  = data.leadingCoarse();
    m_leadingFine    = data.leadingFine();
    m_trailingCoarse = data.trailingCoarse();
    m_trailingFine   = data.trailingFine();
    m_width          = data.width();
    m_phi            = data.phi(); 
}


MdtCalibData::MdtCalibData(uint8_t* buffer) {

    if( checkIntegrity(buffer) ) {   
	uint32_t word;
	uint16_t phi;
	
	m_id = wordFromLitEndChars(buffer);
	word = wordFromLitEndChars(buffer+1*WRDSIZE);
	wordIntoBit16(word,m_leadingFine,m_leadingCoarse);
	word = wordFromLitEndChars(buffer+2*WRDSIZE);
	wordIntoBit16(word,m_trailingFine,m_trailingCoarse);
	word = wordFromLitEndChars(buffer+3*WRDSIZE);
	wordIntoBit16(word,phi,m_width);
	//word = wordFromLitEndChars(buffer+4*WRDSIZE);
	
	int16_t Phi = phi;
	m_phi = ((float)Phi)/10000.;
	
	
	return;
    }
    
    m_id = 0;
    m_leadingCoarse  = 0;
    m_leadingFine    = 0;
    m_trailingCoarse = 0;
    m_trailingFine   = 0;
    m_width          = 0; 
    m_phi            = 0;
}


bool MdtCalibData::checkIntegrity(uint8_t* buffer) const {

    int StationName,StationEta,StationPhi,MultiLayer,Layer,Tube;
    uint32_t firstWord = wordFromLitEndChars(buffer);
    WordIntoMdtId(firstWord,StationName,StationEta,StationPhi,MultiLayer,Layer,Tube);
    
    if (StationName==0 && StationEta==0 && StationPhi==0) return false;
    
    uint32_t word4 = wordFromLitEndChars(buffer+3*WRDSIZE);
    uint16_t width;
    uint16_t phi;    
    wordIntoBit16(word4,phi,width);
    
    int16_t Phi = phi; 
    float TPhi = ((float)Phi)/10000.;
    
    if ( TPhi<-M_PI || TPhi>M_PI ) return false;
        
    return true;
}


void
MdtCalibData::getOfflineId(int& StationName, int& StationEta, int& StationPhi,
                           int& MultiLayer, int& Layer, int& Tube) const {
	
    WordIntoMdtId(m_id,StationName,StationEta,StationPhi,MultiLayer,Layer,Tube);
}

int MdtCalibData::dumpWords(uint8_t* buffer,uint16_t& lenght) const {

    if ( lenght<this->size() ) return 0;
    
    uint8_t* word1 = buffer;
    uint8_t* word2 = buffer + 1*WRDSIZE;
    uint8_t* word3 = buffer + 2*WRDSIZE;
    uint8_t* word4 = buffer + 3*WRDSIZE;
    
    int16_t phi = (int) (m_phi*10000.);
    
    charsFromLitEndWord(m_id,word1);
    charsFromLitEndWord(bit16IntoWord(m_leadingFine,m_leadingCoarse),word2); 
    charsFromLitEndWord(bit16IntoWord(m_trailingFine,m_trailingCoarse),word3); 
    charsFromLitEndWord(bit16IntoWord(phi,m_width),word4);
    
    lenght -= this->size();
    return this->size();
}


std::ostream  &LVL2CALIBRATION::operator<<(std::ostream& stream,const MdtCalibData& data) {

    int StationName;
    int StationEta;
    int StationPhi;
    int MultiLayer;
    int Layer;
    int Tube;
    
    data.getOfflineId(StationName,StationEta,StationPhi,MultiLayer,Layer,Tube);
    
    uint16_t size = data.size();
    uint8_t* buffer = new uint8_t[size];
    
    data.dumpWords(buffer,size);
 

    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer));
    stream << "  Tube at Name/Eta/Phi/Mul/Lay/Tube = "
           << "/" << StationName << "/" << StationEta << "/" << StationPhi
           << "/" << MultiLayer  << "/" << Layer      << "/" << Tube
           << std::endl;
    
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+1*WRDSIZE));
    stream << "    Leading edge: coarse time=" << data.leadingCoarse() 
           << ",  fine time=" << data.leadingFine() << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+2*WRDSIZE));   
    stream << "    Trailing edge: coarse time=" << data.trailingCoarse() 
           << ",  fine time=" << data.trailingFine() << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+3*WRDSIZE));
    stream << "    Width =" << data.width() 
           << ",  Phi=" << data.phi() << std::endl;
    
    delete[] buffer;
    
    return stream;
}




RpcCalibData::RpcCalibData() : m_onlineId(0),m_fel1Id(0),m_febcId(0),m_crc(0) {
    m_hits.clear();
}

RpcCalibData::RpcCalibData(uint16_t onlineId, uint16_t fel1Id, uint16_t febcId,
                           uint16_t crc): m_onlineId(onlineId),m_fel1Id(fel1Id),
		                          m_febcId(febcId),m_crc(crc) {
    if ( (m_onlineId&0xfff8) != 0x0) m_onlineId = 0x0;
    if ( (m_fel1Id&  0xfe00) != 0x0) m_fel1Id = 0x0;
    if ( (m_febcId&  0xf000) != 0x0) m_febcId = 0x0;
    if ( (m_crc&     0xff00) != 0x0) m_crc = 0x0;
    m_hits.clear();
}

RpcCalibData::RpcCalibData(const RpcCalibData& data) : CalibData() {
    
    m_onlineId = data.onlineId();
    m_fel1Id   = data.fel1Id();
    m_febcId   = data.febcId();
    m_crc      = data.crc();
    m_hits     = data.hits();
}

RpcCalibData::RpcCalibData(uint8_t* buffer) {

    if( checkIntegrity(buffer) ) {   
	uint32_t word1 = wordFromLitEndChars(buffer);
	uint32_t word2 = wordFromLitEndChars(buffer+1*WRDSIZE);
	uint16_t data1;
	
	wordIntoBit16(word1,m_febcId,data1);
	m_onlineId = data1 >> 13;
	m_fel1Id   = data1 & 0x1ff;
	
	uint16_t num = hitNum();
	
	wordIntoBit16(word2,num,m_crc);
	
	for(uint16_t i=0;i<num/2+num%2;++i) {
	    uint32_t word = wordFromLitEndChars(buffer+(2+i)*WRDSIZE);
	    uint16_t hit1 = 0x0;
	    uint16_t hit2 = 0x0;
	    wordIntoBit16(word,hit2,hit1);
	    m_hits.push_back(hit1);
	    if(m_hits.size()<num) m_hits.push_back(hit2);
	}
	
	return;
    }
    
    m_onlineId = 0;
    m_fel1Id   = 0;
    m_febcId   = 0;
    m_crc      = 0;
    m_hits.clear();
}

bool RpcCalibData::checkIntegrity(uint8_t* buffer) const {

    uint32_t word2 = wordFromLitEndChars(buffer+1*WRDSIZE);
    uint16_t crc;
    uint16_t hitNum;    
    wordIntoBit16(word2,hitNum,crc);
    
    for (uint16_t i=0;i<hitNum/2+hitNum%2;++i) {
        uint32_t word = wordFromLitEndChars(buffer+(2+i)*WRDSIZE);
	uint16_t hit1 = 0x0;
	uint16_t hit2 = 0x0;
	wordIntoBit16(word,hit2,hit1);
	uint16_t test;
	test = (hit1 >> 5) & 0x7;
	if(test>7) return false;
	test = (hit2 >> 5) & 0x7;
	if(test>7) return false;
    }
        
    return true;
}

void
RpcCalibData::getHit(uint16_t bcid, uint16_t time, uint16_t ijk, uint16_t ch) {

    if (ijk==7) return;
    uint16_t data;
    if( RpcHitIntoBit16(bcid,time,ijk,ch,0,0,data) ) m_hits.push_back(data);		     
}

void
RpcCalibData::getHit(uint16_t bcid, uint16_t time, uint16_t ijk, uint16_t ovl, uint16_t thr) {

    if (ijk < 7) return;
    uint16_t data;
    if( RpcHitIntoBit16(bcid,time,ijk,0,ovl,thr,data) ) m_hits.push_back(data);		     
}

bool 
RpcCalibData::giveHit (uint16_t  num, uint16_t& bcid, uint16_t&time, 
                                      uint16_t& ijk, uint16_t& channel, 
				      uint16_t& ovl, uint16_t& th) const {
    if(num > hitNum()-1) return false;
    std::list<uint16_t>::const_iterator it = m_hits.begin();
    for(int i=0;i<num;++i) ++it;
    Bit16IntoRpcHit (*it,bcid,time,ijk,channel,ovl,th);
    return true;
}

int RpcCalibData::dumpWords(uint8_t* buffer,uint16_t& lenght) const {

    if ( lenght<this->size() ) return 0;

    uint16_t data1 = (m_onlineId<<13) | m_fel1Id;
    
    uint8_t* word1 = buffer;
    uint8_t* word2 = buffer + 1*WRDSIZE;
    
    charsFromLitEndWord(bit16IntoWord(m_febcId,data1),word1); 
    charsFromLitEndWord(bit16IntoWord(hitNum(),m_crc),word2); 

    uint8_t* dataword = buffer + 2*WRDSIZE;
    uint16_t datasize = hitNum()/2 + hitNum()%2;

    std::list<uint16_t>::const_iterator data = m_hits.begin();

    for(uint16_t i=0;i<datasize;++i) {
        uint16_t hit1 = *data;
	++data;
	uint16_t hit2 = 0x0;
	if(data!=m_hits.end()) {
	     hit2 = *data;
	     ++data;
	}
	charsFromLitEndWord(bit16IntoWord(hit2,hit1),dataword+i*WRDSIZE); 
    }
    
    lenght -= this->size();
    return this->size();
}

std::ostream  &LVL2CALIBRATION::operator<<(std::ostream& stream,const RpcCalibData& data) {

    uint16_t size = data.size();
    uint8_t* buffer = new uint8_t[size];
    
    data.dumpWords(buffer,size);
 

    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer));
    stream << "  CMAID = " << data.onlineId() << ",  FEL1ID = " << std::hex
           << data.fel1Id() << ",  FEBCID = " << data.febcId() << std::dec
	   << std::endl;
    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+1*WRDSIZE));
    stream << "  CRC = " << data.crc() << ",  HitNum = " << data.hitNum()
           << std::endl;
    
    uint8_t* dataword = buffer + 2*WRDSIZE;
    uint16_t hitNum   = data.hitNum();
    uint16_t datasize = hitNum/2 + hitNum%2;

    for(uint16_t i=0;i<datasize;++i) {
        LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(dataword+i*WRDSIZE));
        
	uint16_t bcid  = 0x0;
	uint16_t time  = 0x0;
	uint16_t ijk   = 0x0;
	uint16_t strip = 0x0;
 	uint16_t ovl   = 0x0;
	uint16_t thr   = 0x0;
	
	if( data.giveHit(i*2,bcid,time,ijk,strip,ovl,thr) ) {
	    if(ijk!=7) stream << "    BC=" << bcid << ",TI=" << time 
	                      << ",ijk=" << ijk << ",CH=" << std::setw(2) 
			      << strip << "     | ";
            else       stream << "    BC=" << bcid << ",TI=" << time 
	                      << ",ijk=" << ijk << ",OV=" << ovl 
			      << ",TH=" << thr << " | ";
	} else {
	    stream << "                          | ";
	}
	
	if( data.giveHit(i*2+1,bcid,time,ijk,strip,ovl,thr) ) {
	    if(ijk!=7) stream << "BC=" << bcid << ",TI=" << time 
	                      << ",ijk=" << ijk
	                      << ",CH=" << std::setw(2) << strip;
            else       stream << "BC=" << bcid << ",TI=" << time 
	                      << ",ijk=" << ijk << ",OV=" << ovl 
			      << ",TH=" << thr;
	} 
	
	stream  << std::endl;
    }
    
    delete[] buffer;
    
    return stream;
}


void TgcCalibData::initCounters() 
{
    m_counters.clear();
    m_counters.reserve(7);
    
    TGC_BYTESTREAM_FRAGMENTCOUNT counter;
    counter.count = 0;
    
    counter.id = 1;
    m_counters.push_back(counter);
    
    counter.id = 2;
    m_counters.push_back(counter);
    
    counter.id = 3;
    m_counters.push_back(counter);
    
    counter.id = 4;
    m_counters.push_back(counter);
   
    counter.id = 5;
    m_counters.push_back(counter);
    
    counter.id = 8;
    m_counters.push_back(counter);
    
    counter.id = 9;
    m_counters.push_back(counter);
}

TgcCalibData::TgcCalibData()
{
    initCounters();
    m_readout_hit.clear();
    m_readout_triplet_strip.clear();
    m_readout_tracklet.clear();
    m_readout_hipt.clear();
    m_readout_sl.clear();
}


TgcCalibData::TgcCalibData(const TgcCalibData& data) : CalibData()
{   
    m_counters              = data.counters(); 
    m_readout_hit           = data.readout_hit();
    m_readout_triplet_strip = data.readout_triplet_strip();
    m_readout_tracklet      = data.readout_tracklet();
    m_readout_hipt          = data.readout_hipt();
    m_readout_sl            = data.readout_sl();
}


TgcCalibData::TgcCalibData(uint8_t* buffer) {

    initCounters();
    if( checkIntegrity(buffer) ) 
    {   
        unsigned int iBs = 0;
        TGC_BYTESTREAM_FRAGMENTCOUNT counter;
        for (unsigned int iCnt = 0; iCnt < 7; iCnt++)
        {
            fromBS32(wordFromLitEndChars(buffer+iBs*WRDSIZE), counter);
            if (counter.id == (m_counters[iCnt]).id)
            {
                (m_counters[iCnt]).count = counter.count;
                iBs++;
            }
        }
	     
	for (unsigned int iCnt = 0; iCnt < 7; iCnt++)
        {
            if (m_counters[iCnt].count == 0) continue;
	    uint32_t word = 0x0;
            switch ( (m_counters[iCnt]).id )
            {
                case 1: // Raw data format: ignore
                {
                    for (unsigned iFrag = 0; iFrag < m_counters[iCnt].count; iFrag++)
                    {
                        iBs++;
                    }
                    break;
                }

                case 2: // TgcRawData::TYPE_HIT
                {
                    for (unsigned iFrag = 0; iFrag < m_counters[iCnt].count; iFrag++)
                    {
                        fromBS32(wordFromLitEndChars(buffer+WRDSIZE*iBs), word);
			m_readout_hit.push_back(word);
			iBs++;
                    }
                    break;
                }
                
		case 3: // TgcRawData::TYPE_TRACKLET
                {
		    TGC_BYTESTREAM_READOUTTRIPLETSTRIP rostrip;
                    for (unsigned iFrag = 0; iFrag < m_counters[iCnt].count; iFrag++)
                    {
                        fromBS32(wordFromLitEndChars(buffer+iBs*WRDSIZE), rostrip);
			fromBS32(wordFromLitEndChars(buffer+iBs*WRDSIZE), word);
                        if (rostrip.slbType == TgcRawData::SLB_TYPE_TRIPLET_STRIP)
                        {
                            m_readout_triplet_strip.push_back(word);
                        }
                        else
                        {
                            m_readout_tracklet.push_back(word);
                        }
                        iBs++;
                    }
                    break;
                }
                
		case 8: // TgcRawData::TYPE_HIPT
                {
                    for (unsigned iFrag = 0; iFrag < m_counters[iCnt].count; iFrag++)
                    {
                        fromBS32(wordFromLitEndChars(buffer+iBs*WRDSIZE), word);
			m_readout_hipt.push_back(word);
                        iBs++;
                    }
                    break;
                }
                
		case 9: // TgcRawData::TYPE_SL
                {
                    for (unsigned iFrag = 0; iFrag < m_counters[iCnt].count; iFrag++)
                    {
                        fromBS32(wordFromLitEndChars(buffer+iBs*WRDSIZE), word);
			m_readout_sl.push_back(word);
		        iBs++;
                    }
                    break;
                }
                default:
		break;
            
            }
        }

	return;
    }

    initCounters();
    m_readout_hit.clear();
    m_readout_triplet_strip.clear();
    m_readout_tracklet.clear();
    m_readout_hipt.clear();
    m_readout_sl.clear();
}

void 
TgcCalibData::getHit(const TGC_BYTESTREAM_READOUTHIT roh,
                     const TGC_BYTESTREAM_READOUTTRIPLETSTRIP tps,
		     const TGC_BYTESTREAM_READOUTTRACKLET rot,
		     const TGC_BYTESTREAM_HIPT hpt,
		     const TGC_BYTESTREAM_SL sl)
{
    *this << roh;
    *this << tps;
    *this << rot;
    *this << hpt;
    *this << sl;
}


TgcCalibData& 
TgcCalibData::operator<<(TGC_BYTESTREAM_READOUTHIT roh)
{
    uint32_t word = toBS32(roh);
    if (word != 0x0) 
    {
        (m_counters[1]).count++;
	m_readout_hit.push_back(word);
    }
    return *this;
}

TgcCalibData& 
TgcCalibData::operator<<(TGC_BYTESTREAM_READOUTTRIPLETSTRIP rot)
{
    uint32_t word = toBS32(rot);
    if (word != 0x0) 
    {
        (m_counters[2]).count++;
	m_readout_triplet_strip.push_back(word);
    }
    return *this;
}

TgcCalibData& 
TgcCalibData::operator<<(TGC_BYTESTREAM_READOUTTRACKLET rot)
{    
    uint32_t word = toBS32(rot);
    if (word != 0x0) 
    {
        (m_counters[2]).count++;
	m_readout_tracklet.push_back(word);
    }
    return *this;
}

TgcCalibData& 
TgcCalibData::operator<<(TGC_BYTESTREAM_HIPT hpt)
{    
    uint32_t word = toBS32(hpt);
    if (word != 0x0) 
    {
        (m_counters[5]).count++;
	m_readout_hipt.push_back(word);
    }
    return *this;
}

TgcCalibData& 
TgcCalibData::operator<<(TGC_BYTESTREAM_SL sl)
{    
    uint32_t word = toBS32(sl);
    if (word != 0x0) 
    {
        (m_counters[6]).count++;
	m_readout_sl.push_back(word);
    }
    return *this;
}
    
    
    
std::list<TGC_BYTESTREAM_READOUTHIT> 
TgcCalibData::readoutHit (void) const 
{
    std::list<TGC_BYTESTREAM_READOUTHIT> roh_list;
    roh_list.clear();
    
    std::list<uint32_t>::const_iterator it;
    for(it=m_readout_hit.begin();it!=m_readout_hit.end();++it)
    {
        TGC_BYTESTREAM_READOUTHIT roh;
        fromBS32(*it,roh);
        roh_list.push_back(roh);
    }
    
    return roh_list;
}

std::list<TGC_BYTESTREAM_READOUTTRIPLETSTRIP> 
TgcCalibData::readoutTripletStrip() const
{
    std::list<TGC_BYTESTREAM_READOUTTRIPLETSTRIP> tp_list;
    tp_list.clear();
    
    std::list<uint32_t>::const_iterator it;
    for(it=m_readout_triplet_strip.begin();it!=m_readout_triplet_strip.end();++it)
    {
        TGC_BYTESTREAM_READOUTTRIPLETSTRIP tp;
        fromBS32(*it,tp);
        tp_list.push_back(tp);
    }
    
    return tp_list;
}

std::list<TGC_BYTESTREAM_READOUTTRACKLET> 
TgcCalibData::readoutTracklet() const
{
    std::list<TGC_BYTESTREAM_READOUTTRACKLET> trk_list;
    trk_list.clear();
    
    std::list<uint32_t>::const_iterator it;
    for(it=m_readout_tracklet.begin();it!=m_readout_tracklet.end();++it)
    {
        TGC_BYTESTREAM_READOUTTRACKLET trk;
        fromBS32(*it,trk);
        trk_list.push_back(trk);
    }
    
    return trk_list;
}
    
std::list<TGC_BYTESTREAM_HIPT>
TgcCalibData::readoutHipt() const
{
    std::list<TGC_BYTESTREAM_HIPT> hpt_list;
    hpt_list.clear();
    
    std::list<uint32_t>::const_iterator it;
    for(it=m_readout_hipt.begin();it!=m_readout_hipt.end();++it)
    {
        TGC_BYTESTREAM_HIPT hpt;
        fromBS32(*it,hpt);
        hpt_list.push_back(hpt);
    }
    
    return hpt_list;
}

std::list<TGC_BYTESTREAM_SL>
TgcCalibData::readoutSL() const
{
    std::list<TGC_BYTESTREAM_SL> sl_list;
    sl_list.clear();
    
    std::list<uint32_t>::const_iterator it;
    for(it=m_readout_sl.begin();it!=m_readout_sl.end();++it)
    {
        TGC_BYTESTREAM_SL sl;
        fromBS32(*it,sl);
        sl_list.push_back(sl);
    }
    
    return sl_list;
}


bool TgcCalibData::checkIntegrity(uint8_t* buffer) const
{
    unsigned int iBs = 0;
    
    TGC_BYTESTREAM_FRAGMENTCOUNT counters[7] = {
        {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 8}, {0, 9}
    };
    
    TGC_BYTESTREAM_FRAGMENTCOUNT counter;
    
    for (unsigned int iCnt = 0; iCnt < 7; iCnt++)
    {
        fromBS32(buffer[iBs*WRDSIZE], counter);
        if (counter.id == (counters[iCnt]).id)
        {
            (counters[iCnt]).count = counter.count;
            iBs++;
        }
    }
            
    return true;
}


int TgcCalibData::dumpWords(uint8_t* buffer,uint16_t& lenght) const 
{
    if ( lenght<this->size() ) return 0;
    unsigned int iBs = 0;
    
    for (unsigned int iCnt = 0; iCnt < 7; iCnt++)
    {
        if ((m_counters[iCnt]).count > 0)
        {
	    charsFromLitEndWord(toBS32(m_counters[iCnt]),buffer+iBs*WRDSIZE);
	    iBs++;
        }
    }
    
    std::list<uint32_t>::const_iterator roh;
    for(roh=m_readout_hit.begin();roh!=m_readout_hit.end();++roh) 
    {
        charsFromLitEndWord(*roh,buffer+iBs*WRDSIZE);
        iBs++;
    }
    std::list<uint32_t>::const_iterator tps;
    for(tps=m_readout_triplet_strip.begin();tps!=m_readout_triplet_strip.end();++tps) 
    {
        charsFromLitEndWord(*tps,buffer+iBs*WRDSIZE);
        iBs++;
    }
    std::list<uint32_t>::const_iterator rot;
    for(rot=m_readout_tracklet.begin();rot!=m_readout_tracklet.end();++rot) 
    {
        charsFromLitEndWord(*rot,buffer+iBs*WRDSIZE);
        iBs++;
    }
    std::list<uint32_t>::const_iterator hpt;
    for(hpt=m_readout_hipt.begin();hpt!=m_readout_hipt.end();++hpt) 
    {
        charsFromLitEndWord(*hpt,buffer+iBs*WRDSIZE);
        iBs++;
    }
    std::list<uint32_t>::const_iterator sl;
    for(sl=m_readout_sl.begin();sl!=m_readout_sl.end();++sl) 
    {
        charsFromLitEndWord(*sl,buffer+iBs*WRDSIZE);
        iBs++;
    }
    
    lenght -= this->size();
    return this->size();
}

std::ostream  &LVL2CALIBRATION::operator<<(std::ostream& stream,const TgcCalibData& data) {

    uint16_t size = data.size();
    if (size==0) return stream;
    
    uint8_t* buffer = new uint8_t[size];
    data.dumpWords(buffer,size);
    
    unsigned int iBs = 0;
    TGC_BYTESTREAM_FRAGMENTCOUNT counter;
    std::vector<TGC_BYTESTREAM_FRAGMENTCOUNT> data_counters;

    data_counters.insert(data_counters.end(),data.counters().begin(),data.counters().end());
    
    for (unsigned int iCnt = 0; iCnt < 7; iCnt++)
    {
	uint32_t counter_word = wordFromLitEndChars(buffer+iBs*WRDSIZE);
        fromBS32(counter_word, counter);
        if (counter.id == (data_counters[iCnt]).id)
        {
	    std::ostringstream tag;
	    switch ((data_counters[iCnt]).id)
	    {
	        case 2 : tag << " READOUT_HIT words"; break;
	        case 3 : tag << " TRACKLET words"; break;
	        case 8 : tag << " HIPT words"; break;
	        case 9 : tag << " SL words"; break;
	    }
	    
	    LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+iBs*WRDSIZE));
            stream << "  " << (data_counters[iCnt]).count << tag.str() << std::endl;
            iBs++;
        }
    }
    
    
    std::list<uint32_t>::const_iterator roh;
    for(roh=data.readout_hit().begin();roh!=data.readout_hit().end();++roh) 
    {
        LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+iBs*WRDSIZE));
        stream << "  READOUT HIT" << std::endl;
	TGC_BYTESTREAM_READOUTHIT ROH;
	fromBS32(wordFromLitEndChars(buffer+iBs*WRDSIZE), ROH);
	iBs++;
    }
    std::list<uint32_t>::const_iterator tps;
    for(tps=data.readout_triplet_strip().begin();tps!=data.readout_triplet_strip().end();++tps) 
    {
        LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+iBs*WRDSIZE));
        stream << "  READOUT TRIPLET STRIP" << std::endl;
	iBs++;
    }
    std::list<uint32_t>::const_iterator rot;
    for(rot=data.readout_tracklet().begin();rot!=data.readout_tracklet().end();++rot) 
    {
        LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+iBs*WRDSIZE));
        stream << "  READOUT TRACKLET" << std::endl;
	iBs++;
    }
    std::list<uint32_t>::const_iterator hpt;
    for(hpt=data.readout_hipt().begin();hpt!=data.readout_hipt().end();++hpt) 
    {
        LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+iBs*WRDSIZE));
        stream << "  READOUT HIPT" << std::endl;
	iBs++;
    }
    std::list<uint32_t>::const_iterator sl;
    for(sl=data.readout_sl().begin();sl!=data.readout_sl().end();++sl) 
    {
        LVL2CALIBRATION::printWord(stream,wordFromLitEndChars(buffer+iBs*WRDSIZE));
        stream << "  READOUT SL" << std::endl;
	iBs++;
    }
    
    
    delete[] buffer;
    
    return stream;
}
