/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <iomanip>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/utsname.h>

#include <stdlib.h>

#include "TrigmuFast/CalibUti.h"

/* Older resolvers do not have gethostbyname2() */
#ifndef HAVE_GETHOSTBYNAME2
#define gethostbyname2(host,family)    gethostbyname( (host) )
#endif

/* Define following even if IPv6 not supported, so we can always allocate an
   adequately-sized buffer, without #ifdefs in the code  */
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN   46    /* max size of IPv6 address string:
                           "xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:"  or
                           "xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:ddd.ddd.ddd.ddd\0"
						    1234567890123456789012345678901234567890123456  */
#endif


void LVL2CALIBRATION::printWord(std::ostream& stream, uint32_t word) {

    stream << "[ " << std::hex << std::setw(8) << std::setfill('0') 
                   << word << std::dec << " ]  |--> ";
}

std::string LVL2CALIBRATION::hexDump(uint8_t* data, uint16_t len)
{
    if (!len || len%4) return "";
    
    std::ostringstream buf;
    uint16_t words = 1;
    
    do {
        uint16_t byte1 = *(data);
	uint16_t byte2 = *(data+1);
	uint16_t byte3 = *(data+2);
	uint16_t byte4 = *(data+3);
	
	// show data in litte-endian format
	buf << std::setfill('0');
        buf << "  word " << std::setw(3) << words << " :  " << std::hex;
	buf << std::setw(2) << (byte4&0xff) << std::setw(2) << (byte3&0xff);
	buf << std::setw(2) << (byte2&0xff) << std::setw(2) << (byte1&0xff);
	
	char car1 = *(data);
	char car2 = *(data+1);
	char car3 = *(data+2);
	char car4 = *(data+3);
	
	// show only the meaningful characters 
	if(car1 < 32 || car1 > 126) car1 = ' ';
	if(car2 < 32 || car2 > 126) car2 = ' ';
	if(car3 < 32 || car3 > 126) car3 = ' ';
	if(car4 < 32 || car4 > 126) car4 = ' ';
	
	// show corresponding chars 
	buf << "   " << std::dec;
	buf << std::setw(1) << car4 << std::setw(1) << car3; 
	buf << std::setw(1) << car2 << std::setw(1) << car1; 
	
	/*
	// show char in big-endian to ease reading
	buf << "   " << std::dec;
	buf << std::setw(1) << car1 << std::setw(1) << car2; 
	buf << std::setw(1) << car3 << std::setw(1) << car4; 
	buf << std::endl;
	*/
	data += 4;

    } while (words++ != len/4);
    
    return buf.str();
}


void LVL2CALIBRATION::copyChars(const uint8_t* data, uint8_t* ptr, uint16_t len)
{
    for(uint16_t i=0;i<len;++i) ptr[i] = data[i]; 
}


uint32_t LVL2CALIBRATION::floatIntoWord(float value) {
    union Data {
        uint32_t word;
	float   value;
    } data;
    
    data.value = value;
    return data.word;
}



float LVL2CALIBRATION::wordIntoFloat(uint32_t word) {
    union Data {
        uint32_t word;
	float   value;
    } data;
    
    data.word = word;
    return data.value;
}


uint32_t LVL2CALIBRATION::wordFromLitEndChars(const uint8_t* ptr)
{
    union Data {
        uint32_t word;
	uint8_t bytes[4];
    } data;
    
    data.bytes[0] = ptr[0];
    data.bytes[1] = ptr[1];
    data.bytes[2] = ptr[2];
    data.bytes[3] = ptr[3];
    
    return data.word;
}


uint32_t LVL2CALIBRATION::wordFromBigEndChars(const uint8_t* ptr)
{
    union Data {
        uint32_t word;
	uint8_t bytes[4];
    } data;
    
    data.bytes[0] = ptr[3];
    data.bytes[1] = ptr[2];
    data.bytes[2] = ptr[1];
    data.bytes[3] = ptr[0];
    
    return data.word;
}

void LVL2CALIBRATION::charsFromLitEndWord(const uint32_t val, uint8_t* ptr)
{
    union Data {
        uint32_t word;
	uint8_t bytes[4];
    } data;
    
    data.word = val;
    
    *ptr     = data.bytes[0];
    *(ptr+1) = data.bytes[1];
    *(ptr+2) = data.bytes[2];
    *(ptr+3) = data.bytes[3];
}

void LVL2CALIBRATION::charsFromBigEndWord(const uint32_t val, uint8_t* ptr)
{
    union Data {
        uint32_t word;
	uint8_t bytes[4];
    } data;
    
    data.word = val;
    
    *ptr     = data.bytes[3];
    *(ptr+1) = data.bytes[2];
    *(ptr+2) = data.bytes[1];
    *(ptr+3) = data.bytes[0];
}



uint32_t LVL2CALIBRATION::bit16IntoWord(const uint16_t bytes1,
                                         const uint16_t bytes2) {

    union Data {
        uint32_t word;
	uint16_t bytes[2];
    } data;
    
    data.bytes[0] = bytes1;
    data.bytes[1] = bytes2;
    
    return data.word;
    
}

void LVL2CALIBRATION::wordIntoBit16(const uint32_t val, uint16_t& bytes1,
                                    uint16_t& bytes2) {

    union Data {
        uint32_t word;
	uint16_t bytes[2];
    } data;
    
    data.word = val;
    
    bytes1 = data.bytes[0];
    bytes2 = data.bytes[1];
}

void LVL2CALIBRATION::IPfromLong(const uint32_t val, char* str)
{
    std::ostringstream buf;
    
    for(int b=3;b>=0;--b) {
        uint32_t address = (val >> 8*b)&0xff;
	char sep = (b==0)? '\0' : '.';
        buf << address << sep;
    }
    
    buf.str().copy(str,16,0);
}

void  LVL2CALIBRATION::IPfromChar(const char* str,uint32_t& val)
{
    val = 0;
    std::string ip = str;
    std::size_t currPos = 0;
    std::size_t nextPos = 0;
    
    for(int b=3;b>=0;--b) {
        nextPos = (b!=0)? ip.find(".") : ip.length();
	
	if (b != 0 && (nextPos == std::string::npos) ) {
	    //err_msg("IPfromChar: bad format of %s",str);
	    val = 0;
	    return;
	}
	
	
	uint32_t add = 0;
	if((add = atoi(ip.substr(currPos,nextPos-currPos).c_str())) > 255) {
	    //err_msg("IPfromChar: bad value < %d > in address %s",add,str);
	    val = 0;
	    return;
	}
		
	uint32_t address = (add << 8*b);
	ip.replace(nextPos,1," ");
	val |= address;
	currPos = nextPos+1;
    }
}

char** LVL2CALIBRATION::local_address(int* addrtype)
{
    struct hostent* hptr;
    struct utsname myname;
    
    if(uname(&myname) < 0) return NULL;
    
    if( (hptr=gethostbyname(myname.nodename)) == NULL) return (NULL);
        
    *addrtype = hptr->h_addrtype;
    return (hptr->h_addr_list);
}

std::string LVL2CALIBRATION::fill_ip()
{
    char **ppr;
    char IP[INET6_ADDRSTRLEN];
    int  addr_type;

    ppr = local_address(&addr_type);
    
    inet_ntop(addr_type,*ppr,IP,sizeof(IP));
    
    return std::string(IP);
}


uint32_t LVL2CALIBRATION::TStoWords()
{
    struct timeval tv1;
    gettimeofday(&tv1, 0);
    
    uint32_t seconds = tv1.tv_sec;
    
    return seconds;
}


static int NameLim[2]         = {0,59};
static int EtaLim[2]          = {-8,8};
static int PhiLim[2]          = {1,8};
static int TgcPhiLim[2]       = {1,48};
static int MultiLayerLim[2]   = {1,2};
static int TubeLayerLim[2]    = {1,4};
static int TubeLim[2]         = {1,127};
static int DoubletRLim[2]     = {1,2};
static int DoubletZLim[2]     = {1,3};
static int DoubletPhiLim[2]   = {1,2};
static int GasGapLim[2]       = {1,2};
static int MeasurePhiLim[2]   = {0,1};
static int StripLim[2]        = {1,127};
static int IsStripLim[2]      = {0,1};
static int ChannelLim[2]      = {1,127};
static int ChamberLayerLim[2] = {1,2};
static int WireLayerLim[2]    = {1,4};


bool LVL2CALIBRATION::checkLim(const int val,int lim[2]) {
    return (val>=lim[0] && val<=lim[1])? true : false;
}


uint32_t LVL2CALIBRATION::MdtIdIntoWord(const int StationName, const int StationEta, 
                                    const int StationPhi, const int MultiLayer,
				    const int TubeLayer, const int Tube) {

    if ( checkLim(StationName,NameLim)      &&
         checkLim(StationEta,EtaLim)        &&
	 checkLim(StationPhi,PhiLim)        &&
	 checkLim(MultiLayer,MultiLayerLim) &&
	 checkLim(TubeLayer,TubeLayerLim)   &&
	 checkLim(Tube,TubeLim)                ) {
	 
    
	uint32_t name    = ( (uint32_t)StationName ) << (32-6);
	uint32_t etaSign = (StationEta<0)? 0x1 : 0x0;
	         etaSign = etaSign << (32-7);
	uint32_t eta     = ( (uint32_t) abs(StationEta) ) << (32-11);
	uint32_t phi     = ( (uint32_t) StationPhi ) << (32-17);
        uint32_t mul     = ( (uint32_t) (MultiLayer - 1) ) << (32-18);
	uint32_t lay     = ( (uint32_t) (TubeLayer - 1) )  << (32-20);
	uint32_t tag     = 0x0 << (32-25);
	uint32_t tub     = ( (uint32_t) (Tube - 1) );
	
	return name | etaSign | eta | phi | mul | lay | tag | tub; 
    } 
    
    return 0x0;			    
}

uint32_t LVL2CALIBRATION::RpcIdIntoWord(const int StationName, const int StationEta, 
                                    const int StationPhi, const int DoubletR,
				    const int DoubletZ, const int DoubletPhi,
				    const int GasGap, const int MeasurePhi,
				    const int Strip) {

    if ( checkLim(StationName,NameLim)      &&
         checkLim(StationEta,EtaLim)        &&
	 checkLim(StationPhi,PhiLim)        &&
	 checkLim(DoubletR,DoubletRLim)     &&
	 checkLim(DoubletZ,DoubletZLim)     &&
	 checkLim(DoubletPhi,DoubletPhiLim) &&
	 checkLim(GasGap,GasGapLim)         &&
	 checkLim(MeasurePhi,MeasurePhiLim) &&
	 checkLim(Strip,StripLim)              ) {
	 
    
	uint32_t name    = ( (uint32_t)StationName ) << (32-6);
	uint32_t etaSign = (StationEta<0)? 0x1 : 0x0;
	         etaSign = etaSign << (32-7);
	uint32_t eta     = ( (uint32_t) abs(StationEta) ) << (32-11);
	uint32_t phi     = ( (uint32_t) StationPhi ) << (32-17);
        uint32_t dr      = ( (uint32_t) (DoubletR - 1) ) << (32-18);
	uint32_t dz      = ( (uint32_t) (DoubletZ - 1) ) << (32-20);
	uint32_t dp      = ( (uint32_t) (DoubletPhi - 1) ) << (32-21);
	uint32_t gap     = ( (uint32_t) (GasGap - 1) ) << (32-22);
	uint32_t mp      = ( (uint32_t) MeasurePhi ) << (32-23);
	uint32_t tag     = 0x1 << (32-25);
	uint32_t str     = ( (uint32_t) (Strip - 1) );
	
	return name | etaSign | eta | phi | dr | dz | dp | gap | mp | tag | str; 
    } 
    return 0x0;		    
}

uint32_t LVL2CALIBRATION::TgcIdIntoWord(const int StationName, const int StationEta, 
                                    const int StationPhi, const int GasGap,
				    const int IsStrip, const int Channel) {

    if ( checkLim(StationName,NameLim)      &&
         checkLim(StationEta,EtaLim)        &&
	 checkLim(StationPhi,TgcPhiLim)     &&
	 checkLim(GasGap,GasGapLim)         &&
	 checkLim(IsStrip,IsStripLim)       &&
	 checkLim(Channel,ChannelLim)          ) {
	 
    
	uint32_t name    = ( (uint32_t)StationName ) << (32-6);
	uint32_t etaSign = (StationEta<0)? 0x1 : 0x0;
	         etaSign = etaSign << (32-7);
	uint32_t eta     = ( (uint32_t) abs(StationEta) ) << (32-11);
	uint32_t phi     = ( (uint32_t) StationPhi ) << (32-17);
	uint32_t gap     = ( (uint32_t) (GasGap - 1) ) << (32-22);
	uint32_t iss     = ( (uint32_t) IsStrip ) << (32-23);
	uint32_t tag     = 0x2 << (32-25);
	uint32_t ch      = ( (uint32_t) (Channel - 1) );
	
	return name | etaSign | eta | phi | gap | iss | tag | ch; 
    } 
    return 0x0;		    
}

uint32_t LVL2CALIBRATION::CscIdIntoWord(const int StationName, const int StationEta, 
                                    const int StationPhi, const int ChamberLayer,
				    const int WireLayer, const int MeasurePhi,
				    const int Strip) {
    if ( checkLim(StationName,NameLim)          &&
         checkLim(StationEta,EtaLim)            &&
	 checkLim(StationPhi,TgcPhiLim)         &&
	 checkLim(ChamberLayer,ChamberLayerLim) &&
	 checkLim(WireLayer,WireLayerLim)       &&
	 checkLim(MeasurePhi,MeasurePhiLim)     &&
	 checkLim(Strip,StripLim)                   ) {
	 
    
	uint32_t name    = ( (uint32_t)StationName ) << (32-6);
	uint32_t etaSign = (StationEta<0)? 0x1 : 0x0;
	         etaSign = etaSign << (32-7);
	uint32_t eta     = ( (uint32_t) abs(StationEta) ) << (32-11);
	uint32_t phi     = ( (uint32_t) StationPhi ) << (32-17);
	uint32_t chl     = ( (uint32_t) (ChamberLayer-1) ) << (32-18);
	uint32_t wrl     = ( (uint32_t) (WireLayer-1) ) << (32-21);
	uint32_t mp      = ( (uint32_t) MeasurePhi ) << (32-23);
	uint32_t tag     = 0x3 << (32-25);
	uint32_t str     = ( (uint32_t) (Strip-1) );
	
	return name | etaSign | eta | phi | chl | wrl | mp | tag | str; 
    } 
    return 0x0;		    
}


void LVL2CALIBRATION::WordIntoMdtId(uint32_t word, int& StationName, 
                                       int& StationEta, int& StationPhi, 
				       int& MultiLayer, int& TubeLayer, 
				       int& Tube) {

    if ( ((word>>(32-25))&0x3) == 0x0 ) {
	
	uint32_t bit7 = 0x7f; 
        uint32_t bit6 = 0x3f;
	uint32_t bit4 = 0xf;
	uint32_t bit2 = 0x3;
	uint32_t bit1 = 0x1;
	
	StationName = (word >> (32-6)) & bit6;
	StationEta  = (word >> (32-11)) & bit4;
	if ( (word>>(32-7))&bit1 ) StationEta = -StationEta;
	StationPhi = (word >> (32-17)) & bit6;
	MultiLayer = ((word >> (32-18)) & bit1) + 1;
	TubeLayer  = ((word >> (32-20)) & bit2) + 1;
	Tube       = (word & bit7) + 1;
	
	return;
    } 
    StationName = 0;
    StationEta  = 0;
    StationPhi  = 0;
    MultiLayer  = 0;
    TubeLayer   = 0;
    Tube        = 0;
    			    
}

void LVL2CALIBRATION::WordIntoRpcId(uint32_t word, int& StationName, 
                                    int& StationEta, int& StationPhi, 
				    int& DoubletR, int& DoubletZ, 
				    int& DoubletPhi, int& GasGap, 
				    int& MeasurePhi, int& Strip) {

    if ( ((word>>(32-25))&0x3) == 0x1 ) {
	 
 	uint32_t bit7 = 0x7f; 
        uint32_t bit6 = 0x3f;
	uint32_t bit4 = 0xf;
	uint32_t bit2 = 0x3;
	uint32_t bit1 = 0x1;
	
	StationName = (word >> (32-6)) & bit6;
	StationEta  = (word >> (32-11)) & bit4;
	if ( (word>>(32-7))&bit1 ) StationEta = -StationEta;
	StationPhi = (word >> (32-17)) & bit6;
        DoubletR   = ((word >> (32-18)) & bit1) + 1;
	DoubletZ   = ((word >> (32-20)) & bit2) + 1;
	DoubletPhi = ((word >> (32-21)) & bit1) + 1;
	GasGap     = ((word >> (32-22)) & bit1) + 1;
	MeasurePhi =  (word >> (32-23)) & bit1;
	Strip      = (word & bit7) + 1;
	
	return;
    }
    
    StationName = 0;
    StationEta  = 0;
    StationPhi  = 0;
    DoubletR    = 0;
    DoubletZ    = 0;
    DoubletPhi  = 0;
    GasGap      = 0;
    MeasurePhi  = 0;
    Strip       = 0;
    	    
}

void LVL2CALIBRATION::WordIntoTgcId(uint32_t word, int& StationName, 
                                    int& StationEta, int& StationPhi, 
				    int& GasGap, int& IsStrip,int& Channel) {

    if ( ((word>>(32-25))&0x3) == 0x2 ) {

 	uint32_t bit7 = 0x7f; 
        uint32_t bit6 = 0x3f;
	uint32_t bit4 = 0xf;
	uint32_t bit2 = 0x3;
	uint32_t bit1 = 0x1;
	
	StationName = (word >> (32-6)) & bit6;
	StationEta  = (word >> (32-11)) & bit4;
	if ( (word>>(32-7))&bit1 ) StationEta = -StationEta;
	StationPhi = (word >> (32-17)) & bit6;
        GasGap     = ((word >> (32-22)) & bit2) + 1;
	IsStrip    = (word >> (32-23)) & bit1;
	Channel    = (word & bit7) + 1;	 
    
        return;
    } 
    
    StationName = 0;
    StationEta  = 0;
    StationPhi  = 0;
    GasGap      = 0;
    IsStrip     = 0;
    Channel     = 0;
}

void LVL2CALIBRATION::WordIntoCscId(uint32_t word, int& StationName, 
				    int& StationEta, int& StationPhi, 
				    int& ChamberLayer, int& WireLayer, 
				    int& MeasurePhi, int& Strip) {

    if ( ((word>>(32-25))&0x3) == 0x3 ) {
	 
  	uint32_t bit7 = 0x7f; 
        uint32_t bit6 = 0x3f;
	uint32_t bit4 = 0xf;
	uint32_t bit2 = 0x3;
	uint32_t bit1 = 0x1;
	
	StationName = (word >> (32-6)) & bit6;
	StationEta  = (word >> (32-11)) & bit4;
	if ( (word>>(32-7))&bit1 ) StationEta = -StationEta;
	StationPhi = (word >> (32-17)) & bit6;
        ChamberLayer = ((word >> (32-18)) & bit1) + 1;
	WireLayer    = ((word >> (32-20)) & bit2) + 1;
	MeasurePhi   = (word >> (32-23)) & bit1;
	Strip        = (word & bit7) + 1;
	
	return;	
    } 
    
    StationName  = 0;
    StationEta   = 0;
    StationPhi   = 0;
    ChamberLayer = 0;
    WireLayer    = 0;
    MeasurePhi   = 0;
    Strip        = 0;   
}


bool LVL2CALIBRATION::CalibDataLoader(std::ifstream& file,
                                      uint8_t* buffer,uint16_t size) {

    for (uint16_t i=0;i<size;++i) buffer[i]='\0';
    
    uint16_t tag,con;

    // read the first word up to when the event tag is found
    do {
        if ( file.eof() ) return false;
        file.read((char*)buffer,4*sizeof(uint8_t));
        uint32_t firstword = wordFromLitEndChars(buffer);
        wordIntoBit16(firstword,con,tag);
    } while(tag!=0xabba);

    // read second word
    file.read((char*)(buffer+4),4*sizeof(uint8_t));
    uint16_t version,datasize;
    wordIntoBit16(wordFromLitEndChars(buffer+4),datasize,version);
    
    file.read((char*)(buffer+8),(datasize-8)*sizeof(uint8_t));
    //std::cout << "Read " << datasize << " bytes" << std::endl;
    //check the lastword to see if the full event is complete
    uint32_t lastword = wordFromLitEndChars(buffer+datasize-4);
    uint16_t closingcon,closingtag;
    wordIntoBit16(lastword,closingtag,closingcon);
    
    if(tag!=closingtag || con!=closingcon) return false;  
    
    return true;
}


bool LVL2CALIBRATION::RpcHitIntoBit16 (const uint16_t bc,const uint16_t ti,
                      const uint16_t ijk,const uint16_t ch,const uint16_t ov,
		      const uint16_t th,uint16_t& data)
{
    data = 0x0;
    if ( (bc&0xfff8)  != 0x0) return false;
    if ( (ti&0xfff8)  != 0x0) return false;
    if ( (ijk&0xfff8) != 0x0) return false;
    if ( (ch&0xffe0)  != 0x0) return false;
    if ( (ov&0xfffc)  != 0x0) return false;
    if ( (th&0xfffc)  != 0x0) return false;

    data |= (bc  << 11);
    data |= (ti  << 8);
    data |= (ijk << 5);

    if (ijk==7) {
        data |= (ov << 2);
        data |= th;
    } else  data |= ch;

    return true;
}



void LVL2CALIBRATION::Bit16IntoRpcHit (const uint16_t data,uint16_t& bc,
                      uint16_t& ti,uint16_t& ijk,uint16_t& ch,uint16_t& ov,
		      uint16_t& th)
{
    bc  = (data >> 11)&0x7;
    ti  = (data >> 8 )&0x7;
    ijk = (data >> 5 )&0x7;

    if (ijk==7) {
        ov = (data >> 2)&0x3;
        th = (data)&0x3;
        ch = 0;
    } else  {
        ch = (data)&0x1f;
        ov = 0;
        th = 0;
    }
}
