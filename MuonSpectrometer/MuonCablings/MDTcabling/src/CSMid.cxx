/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#if (__GNUC__) && (__GNUC__ > 2) 
               // put your gcc 3.2 specific code her
	       #include <sstream>
	       typedef std::stringstream  __sstream;
	       typedef std::ostringstream __osstream;
#else
               // put your gcc 2.95 specific code here
	       #include <strstream>
	       typedef strstream  __sstream;
	       typedef ostrstream __osstream;
#endif



#include <iomanip>
#include <stdlib.h>
#include <cstring>

#include "math.h"

#include "MDTcabling/CSMid.h"


using namespace std;

Amdcsimrec* CSMid::s_amdc = 0;

bool CSMid::isNewSchema = false;

CSMid::CSMid(int StName,int eta, int phi): m_hash_id(0),
                                           m_compact_id(0),
                                           m_mnemonic_id(""),
                                           m_status(true),
					   m_name(StName),
				           m_StationEta(eta),
					   m_StationPhi(phi),
					   m_eta_tower(0),
					   m_etaMin(0.),
					   m_etaMax(0.),
					   m_phiMin(0.),
					   m_phiMax(0.)
{
    memset(m_mdt_chamber,0,2*sizeof(Chamber*));
    memset(m_mezanine_chamber,0,24*sizeof(Chamber*));
    memset(m_mezanine_stationEta,0xff,24*sizeof(int8_t));

    // check for amdb interface
    if (!s_amdc) {no_definition();return;}
    
    // gets the chamber name and check it
    int Jtype = m_name + 1;
    if(s_amdc->GetStationType(Jtype) == "XXX") {no_definition();return;}
    
    // check the ID consistence
    std::string name = s_amdc->GetStationType(Jtype);


    // build the mnemonic chamber code
    char side  = (m_StationEta > 0)?  'A' : 'C';
    int sector = (name.c_str()[2] == 'L' || name.c_str()[2] == 'R' ||
                  name.c_str()[2] == 'M')?
                 (m_StationPhi-1)*2 + 1 : m_StationPhi*2;

    
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream dump;
#else
    // put your gcc 2.95 specific code here
    char mnemonic[8] = {'\0','\0','\0','\0','\0','\0','\0','\0'};
    __osstream dump(mnemonic,8);
#endif   

    // exchange EEL1C05/A05 with EEL2C05/A05 if we are in newSchema
    float eta_index = abs(m_StationEta);
    if(name=="EEL" && sector==5 && this->isNewSchema) {
        if (eta_index==1) eta_index = 2;
	else eta_index = 1;
    }    
    
    dump << setw(3) << name 
         << setw(1) << eta_index 
         << setw(1) << side
         << setw(2) << setfill('0') << sector << ends;

    m_mnemonic_id = dump.str().c_str();

    // build the compact chamber code
    unsigned short int PartitionType = (name[0] == 'B')? 0 : 1;
    unsigned short int StationNumber = m_name;
    unsigned short int PartitionSide = (side == 'A')?  1 : 0;
    unsigned short int StationEta    = abs(m_StationEta);
    unsigned short int StationPhi    = m_StationPhi;
    m_compact_id = PartitionType << 19 |
                   StationNumber << 13 |
                   PartitionSide << 12 |
                   StationEta    << 4  |
                   StationPhi;
}

CSMid::CSMid(uint32_t compact) : m_hash_id(0),
                                 m_compact_id(compact),
                                 m_mnemonic_id(""),
                                 m_status(true),		       
				 m_eta_tower(0),
				 m_etaMin(0.),
                                 m_etaMax(0.),
				 m_phiMin(0.),
				 m_phiMax(0.)
{
    memset(m_mdt_chamber,0,2*sizeof(Chamber*));
    memset(m_mezanine_chamber,0,24*sizeof(Chamber*));
    memset(m_mezanine_stationEta,0xff,24*sizeof(int8_t));
    
    // decode the compact ID
    unsigned short int PartitionType = (compact&0x80000)>>19;
    unsigned short int StationNumber = (compact&0x7E000)>>13;
    unsigned short int PartitionSide = (compact&0x01000)>>12;
    unsigned short int StationEta    = (compact&0x00FF0)>>4;
    unsigned short int StationPhi    = (compact&0x0000F);
    
    // check for amdb interface
    if (!s_amdc) {no_definition();return;}

    // gets the chamber name and check it
    m_name = StationNumber;
    int Jtype = m_name + 1;
    if(s_amdc->GetStationType(Jtype) == "XXX") {no_definition();return;}
    
    // check the ID consistence
    std::string name = s_amdc->GetStationType(Jtype);
    if(name=="EIL" && (StationEta==4||StationEta==5||StationEta==45)) 
    {
        name = "EILM4";
    }
    
    if(name.c_str()[0] == 'B' && PartitionType == 1) {no_definition();return;}
    if(name.c_str()[0] != 'B' && PartitionType == 0) {no_definition();return;}

    // store StationEta and StationPhi values
    // if(stationEta > 8) return;
    m_StationEta = (PartitionSide)? StationEta : -StationEta;

    if(StationPhi < 1 || StationPhi > 8) {no_definition();return;}
    m_StationPhi = StationPhi;

    // build the mnemonic chamber code
    char side  = (PartitionSide)?  'A' : 'C';
    int sector = (name.c_str()[2] == 'L' || name.c_str()[2] == 'R' ||
                  name.c_str()[2] == 'M' )?
                 (StationPhi-1)*2 + 1 : StationPhi*2;


#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream dump;
#else
    // put your gcc 2.95 specific code here
    char mnemonic[9] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0'};
    __osstream dump(mnemonic,9);
#endif 

    // exchange EEL1C05/A05 with EEL2C05/A05 if we are in newSchema
    float eta_index = StationEta;
    if(name=="EEL" && sector==5 && this->isNewSchema) {
        if (eta_index==1) eta_index = 2;
	else eta_index = 1;
    } 
    if (name=="EILM4")
    {
        dump << setw(5) << name  
             << setw(1) << side
             << setw(2) << setfill('0') << sector << ends;
    } else
    {
        int w = (StationEta>9)? 2 : 1;
        dump << setw(3) << name 
	     << setw(w) << eta_index  
             << setw(1) << side
             << setw(2) << setfill('0') << sector << ends;    
    }
    m_mnemonic_id = dump.str();   

}

CSMid::CSMid(std::string mnemonic) : m_hash_id(0),
                                     m_compact_id(0),
                                     m_mnemonic_id(mnemonic),
                                     m_status(true),
				     m_eta_tower(0),
				     m_etaMin(0.),
                                     m_etaMax(0.),
				     m_phiMin(0.),
				     m_phiMax(0.)
{   
    memset(m_mdt_chamber,0,2*sizeof(Chamber*)); 
    memset(m_mezanine_chamber,0,24*sizeof(Chamber*));
    memset(m_mezanine_stationEta,0xff,24*sizeof(int8_t));

    // check for amdb interface
    if (!s_amdc) {no_definition();return;}

    // check the lenght of the mnemonic code
    if(mnemonic.length() != 7 && 
       mnemonic.length() != 8    ) {no_definition();return;}


    // get the Sector info and the StationEta info
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __sstream dump;
#else
    // put your gcc 2.95 specific code here
    char data[8] = {'\0','\0','\0','\0','\0','\0','\0','\0'};
    __sstream dump(data,8);
#endif

    // get the name info plus sector and eta
    std::string name;
    int chamber;
    int sector;
    
    if(mnemonic.length() == 7)
    {
        dump << mnemonic.substr(5,2) << " " << mnemonic[3];
	name = mnemonic.substr(0,3);
    } else
    {
        if( mnemonic.find("EILM4")!=std::string::npos )
	{
	    name = "EIL";
	    dump << mnemonic.substr(6,2) << " " << 45;
	} else
	{
	    name = mnemonic.substr(0,3);
	    dump << mnemonic.substr(6,2) << " " << mnemonic.substr(3,2);
	}
    }

    dump >> sector >> chamber;
    
    // exchange EEL1C05/A05 with EEL2C05/A05 if we are in newSchema
    if(name=="EEL" && sector==5 && this->isNewSchema) {
        if (chamber==1) chamber = 2;
	else chamber = 1;
    } 
    
    // checks value
    if (sector  > 16) {no_definition();return;}


    // get side info and adjust the StationEta value
    char side = (mnemonic.length() == 8)? mnemonic[5] : mnemonic[4];
    if (side != 'A' && side != 'C') {no_definition();return;}
    m_StationEta = (side == 'A')? chamber : -chamber;

    // get the StationName info
    m_name   = s_amdc->GetJtyp(name) - 1;
    
    int Jtype = m_name + 1;
    if(s_amdc->GetStationType(Jtype) == "XXX") {no_definition();return;} 
    // check sector vs name
    if((name[2]=='L' || name[2]=='R' || name[2]=='M') && !(sector%2)) {no_definition();return;}
    if((name[2]!='L' && name[2]!='R' && name[2]!='M') &&  (sector%2)) {no_definition();return;}
    // get the StationPhi value
    m_StationPhi = (name[2] == 'L' || name[2] == 'R' ||
                    name[2] == 'M' )? 
                   (sector-1)/2 +1  :  sector/2;


    // build the compact chamber code
    unsigned short int PartitionType = (name[0] == 'B')? 0 : 1;
    unsigned short int StationNumber = m_name;
    unsigned short int PartitionSide = (side == 'A')?  1 : 0;
    unsigned short int StationEta    = abs(m_StationEta);
    unsigned short int StationPhi    = m_StationPhi;
    m_compact_id = PartitionType << 19 |
                   StationNumber << 13 |
                   PartitionSide << 12 |
                   StationEta    << 4  |
                   StationPhi;
}

CSMid::CSMid(const CSMid& csm)
{
    m_hash_id = csm.hash_id();
    m_compact_id = csm.compact();
    m_mnemonic_id = csm.mnemonic();
    m_name = csm.stationName();
    m_StationEta = csm.stationEta();
    m_StationPhi = csm.stationPhi();
    m_eta_tower = csm.eta_tower();
    
    m_etaMin = csm.etaMin(); 
    m_etaMax = csm.etaMax();
    m_phiMin = csm.phiMin();
    m_phiMax = csm.phiMax();
    
    for(int ch=0;ch<2;++ch)
    {
	m_mdt_chamber[ch] = csm.m_mdt_chamber[ch];
    }
    
    for(int tdc=0;tdc<24;++tdc)
    {
	m_mezanine_chamber[tdc]    = csm.m_mezanine_chamber[tdc];
	m_mezanine_stationEta[tdc] = csm.m_mezanine_stationEta[tdc];
    }

    m_status = (csm)? true : false;
}

CSMid::CSMid()
{
    this->no_definition();
}

CSMid::~CSMid() {}


void
CSMid::no_definition()
{
    m_hash_id     = 0;
    m_compact_id  = 0;
    m_mnemonic_id = "";
    m_name        = 999;
    m_StationEta  = 999;
    m_StationPhi  = 999;
    m_eta_tower   = 0;
    memset(m_mdt_chamber,0,2*sizeof(Chamber*)); 
    memset(m_mezanine_chamber,0,24*sizeof(Chamber*));
    memset(m_mezanine_stationEta,0xff,24*sizeof(int8_t));
    m_status      = false;
}

CSMid
CSMid::operator=(const CSMid& csm)
{
    m_hash_id    = csm.hash_id();
    m_compact_id = csm.compact();
    m_mnemonic_id = csm.mnemonic();
    m_name = csm.stationName();
    m_StationEta = csm.stationEta();
    m_StationPhi = csm.stationPhi();
    m_eta_tower = csm.eta_tower();
    
    m_etaMin = csm.etaMin(); 
    m_etaMax = csm.etaMax();
    m_phiMin = csm.phiMin();
    m_phiMax = csm.phiMax();
    
    //const Chamber* chamber = csm.get_chamber();
    for(int ch=0;ch<2;++ch)
    {
        m_mdt_chamber[ch] = csm.m_mdt_chamber[ch];
    }
    
    //const TDCgeo* Tdc = csm.get_tdc();
    for(int tdc=0;tdc<24;++tdc)
    {
	m_mezanine_chamber[tdc]    = csm.m_mezanine_chamber[tdc];
	m_mezanine_stationEta[tdc] = csm.m_mezanine_stationEta[tdc];
    }

    m_status = (csm)? true : false;

    return *this;
}

bool
CSMid::operator==(const CSMid& csm) const
{
    if(m_name       == csm.stationName()  &&
       m_StationEta == csm.stationEta() &&
       m_StationPhi == csm.stationPhi() ) return true;
    
    if( abs(m_StationEta)>9 )
    {
        int stEta1 = m_StationEta%10;
	int stEta2 = m_StationEta/10;
        
	if(m_name       == csm.stationName()  &&
           (stEta1 == csm.stationEta() || stEta2 == csm.stationEta()) &&
           m_StationPhi == csm.stationPhi() ) return true;
    }
    
    if( abs(csm.stationEta())>9 )
    {
        int chEta1 = csm.stationEta()%10;
	int chEta2 = csm.stationEta()/10;
        
	if(m_name       == csm.stationName()  &&
           (m_StationEta == chEta1 || m_StationEta == chEta2) &&
           m_StationPhi == csm.stationPhi() ) return true;
    }
    
    return false;
}


    
bool 
CSMid::isBarrel() const
{
    if(m_name <= 11 && m_name != 6) return true;
    else return false;
}


bool 
CSMid::isEndcap() const
{
    if(m_name > 11 || m_name == 6) return true;
    else return false;
}
    
bool 
CSMid::isAside () const
{
    if (m_StationEta == 0 || m_StationEta > 0) return true;
    // by definition chambers at eta < 0 are in C-side
    //if (m_StationEta > 0) return true;
    else return false;
}
    
bool 
CSMid::isCside () const
{
    if (m_StationEta < 0) return true;
    else return false;
}
    
bool 
CSMid::isLarge () const
{
    int Jtype = m_name + 1;
    std::string name = s_amdc->GetStationType(Jtype);
    if (name[2] == 'L' || name[2] == 'R' || name[2] == 'M') return true;
    else return false;
}
    
bool 
CSMid::isSmall () const
{
    int Jtype = m_name + 1;
    std::string name = s_amdc->GetStationType(Jtype);
    if (name[2] != 'L' && name[2] != 'R' && name[2] != 'M') return true;
    else return false;
}


void
CSMid::set_chamber(const Chamber* mdt_chamber,int StationEta)
{
    if(!mdt_chamber) return;
    
    for(int tdc=0;tdc<MaxTDC;++tdc)
    {
       if(mdt_chamber->TDC[tdc])
       {
	   m_mezanine_chamber[tdc] = mdt_chamber;
	   m_mezanine_stationEta[tdc] = StationEta;
       }
    }
    
    if (m_mdt_chamber[0]==0) 
    {
        m_mdt_chamber[0] = mdt_chamber;
	setEtaPhiDim();
        return;
    }
    
    if (m_mdt_chamber[1]==0) 
    {
        m_mdt_chamber[1] = mdt_chamber;
	setEtaPhiDim();
        return;
    }
    
    std::cout << "CSMId error: number of chamber " << std::endl;
    exit(1);
}

void
CSMid::set_tower(const int eta_tower)
{
    m_eta_tower = eta_tower;
}

void
CSMid::set_hash(const uint32_t hash_id)
{
    m_hash_id = hash_id;
}

double 
CSMid::zMin() const
{
    double Zmin = 9999999.;
    for(int i=0;i<2;++i)
    {
        if ( const Chamber* cham=m_mdt_chamber[i] )
	{
	    if ( Zmin>cham->Zmin ) Zmin=cham->Zmin;
	}
    }
    return Zmin;
}

double
CSMid::zMax() const
{
    double Zmax = -9999999.;
    for(int i=0;i<2;++i)
    {
        if ( const Chamber* cham=m_mdt_chamber[i] )
	{
	    if ( Zmax<cham->Zmax ) Zmax=cham->Zmax;
	}
    }
    return Zmax;
}

double
CSMid::rMin() const
{
    double Rmin = 99999999.;
    for(int i=0;i<2;++i)
    {
        if ( const Chamber* cham=m_mdt_chamber[i] )
	{
	    if ( Rmin>cham->Rmin ) Rmin=cham->Rmin;
	}
    }
    return Rmin;
}

double
CSMid::rMax() const
{
    double Rmax = 0.;
    for(int i=0;i<2;++i)
    {
        if ( const Chamber* cham=m_mdt_chamber[i] )
	{
	    if ( Rmax<cham->Rmax ) Rmax=cham->Rmax;
	}
    }
    return Rmax;
}

void
CSMid::setEtaPhiDim()
{
    double zMin = this->zMin();
    double zMax = this->zMax();
    double rMin = this->rMin();
    double rMax = this->rMax();
    
    double etaMin1 = computing_eta(zMin,rMin);
    double etaMin2 = computing_eta(zMin,rMax);
    m_etaMin = (etaMin1<etaMin2)? etaMin1 : etaMin2;
    
    double etaMax1 = computing_eta(zMax,rMin);
    double etaMax2 = computing_eta(zMax,rMax);
    m_etaMax = (etaMax1>etaMax2)? etaMax1 : etaMax2;
    
    if(m_etaMin > m_etaMax)
    {
        float tmp = m_etaMax;
	m_etaMax = m_etaMin;
	m_etaMin = tmp;
    }

    m_phiMin = 9999.;
    m_phiMax = -9999.;
    
    for(int i=0;i<2;++i)
    {
        if ( const Chamber* cham=m_mdt_chamber[i] )
	{
	    double min = cham->Phipos - cham->DeltaPhi;
	    double max = cham->Phipos + cham->DeltaPhi;
	    if ( min < m_phiMin ) m_phiMin = min;
	    if ( max > m_phiMax ) m_phiMax = max;
	}
    }
    
    if( m_phiMin < 0. )     m_phiMin += 2*M_PI;
    if( m_phiMax > 2*M_PI ) m_phiMax -= 2*M_PI;
}

double
CSMid::computing_eta(double z, double r) const
{ 
    double Theta  = (z)? atan(r/fabs(z)): asin(1.);
    double Eta    = (z>0.)?  -log(tan(Theta/2.)) : log(tan(Theta/2.));
    return Eta;
}


bool 
CSMid::isOverlapping(double etaMin,double etaMax,double phiMin,
                     double phiMax) const
{
		      
    bool eta = ( (etaMin>=m_etaMin && etaMin<=m_etaMax)||
                 (etaMax>=m_etaMin && etaMax<=m_etaMax)||
		 (etaMin<=m_etaMin && etaMax>=m_etaMax)  )? true : false;
    
    
    bool phi = false;
    if(phiMin<0.) phiMin += 2*M_PI;
    if(phiMax<0.) phiMax += 2*M_PI;
        
    if(m_phiMin > m_phiMax)
    {
	if(phiMax > phiMin)
	{
	    phi = ( (phiMin>=m_phiMin && phiMin<=2.*M_PI) ||
	            (phiMin>=0.       && phiMin<=m_phiMax)||
		    (phiMax>=m_phiMin && phiMax<=2.*M_PI) ||
                    (phiMax>=0.       && phiMax<=m_phiMax) )? true : false;
        }
	else
	{
	    phi = ( (phiMin>=m_phiMin && phiMin<=2.*M_PI)  ||
		    (phiMax<=m_phiMax && phiMax>=0.      ) ||
		    (phiMin>=m_phiMin && phiMax<=m_phiMax) ||
		    (phiMin<=m_phiMin && phiMax>=m_phiMax)   )? true : false;
	}
    } else
    {
        if(phiMax > phiMin)
	{
            phi = ( (phiMin>=m_phiMin && phiMin<=m_phiMax)||
                    (phiMax>=m_phiMin && phiMax<=m_phiMax)||
		    (phiMin<=m_phiMin && phiMax>=m_phiMax)   )? true : false;
        }
	else
	{
	    phi = ( (phiMax>= m_phiMax)                   ||
	            (phiMax>=m_phiMin && phiMax<=m_phiMax)||
	            (phiMin<= m_phiMin)                   ||
		    (phiMin>=m_phiMin && phiMin<=m_phiMax) )? true : false;
	}
    }
    
    /*
    std::cout << " input  : etaMin=" << etaMin << ", etaMax=" << etaMax
              << " .. phiMin=" << phiMin << ", phiMax=" << phiMax << std::endl;
    std::cout << " chamber: etaMin=" << m_etaMin << ", etaMax=" << m_etaMax
              << " .. phiMin=" << m_phiMin << ", phiMax=" << m_phiMax << std::endl;
    std::cout << " resilt : " << ((eta && phi)? "overlap" : "not overlap")
              << std::endl;
    */
    return eta && phi;
}

const Chamber*  
CSMid::get_chamber() const 
{
    if (m_StationEta<9) return m_mdt_chamber[0];
    else
    {
        return get_chamber(m_StationEta%10);
    }
}

ostream& operator<<(ostream& stream, const CSMid& csm)
{
    if(csm)
    { 
        stream << "CSM index:" << endl;
        stream << "  mnemonic code:  " << csm.mnemonic() << endl;
        stream << "  compcat code :  " << hex << csm.compact() << dec << endl;
        stream << "  Station Name = " << csm.name_str()   << endl;
        stream << "  StationEta   = " << csm.stationEta() << endl;
        stream << "  StationPhi   = " << csm.stationPhi() << endl;
    }
    else
    {
	stream << "CSM undefined!" << endl;
    }

    return stream;
}

