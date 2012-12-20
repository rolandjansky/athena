/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTcabling/MRODscheme.h"

using namespace std;

MRODscheme::MRODscheme(unsigned short int subsystem, 
                       unsigned short int rod, 
                       unsigned short int crate, 
                       CSMid* links) :
  m_Subsystem_ID(subsystem),m_ROD_ID(rod),m_MROD_crate(crate)
{
    m_links.resize(8);
    m_links.clear();

    m_system  = SystemUndefined;
    m_side    = SideUndefined;
    m_chamber = ChamberUndefined;
    
    bool first = true;

    for (int i=0;i<=8;i++) 
    {
        m_links[i] = *(links+i);
        if(first && m_links[i])
	{
            m_system  = (m_links[i].isBarrel())?  Barrel : Endcap;
            m_side    = (m_links[i].isAside())?   A_side : C_side;
            m_chamber = (m_links[i].isLarge())?   Long   : Short;
	    first = false;
	}
    }

    define_mrod_belonging();
}

MRODscheme::MRODscheme(unsigned short int subsystem, 
                     unsigned short int rod, 
                     unsigned short int crate,
                     unsigned short int compact1,unsigned short int compact2,
                     unsigned short int compact3,unsigned short int compact4,
                     unsigned short int compact5,unsigned short int compact6,
                     unsigned short int compact7,unsigned short int compact8):
  m_Subsystem_ID(subsystem),m_ROD_ID(rod),m_MROD_crate(crate)
{
    m_links.reserve(8);
    m_links.clear();
    
    m_system  = SystemUndefined;
    m_side    = SideUndefined;
    m_chamber = ChamberUndefined;
    
    CSMid link1(compact1); m_links.push_back(link1);
    CSMid link2(compact2); m_links.push_back(link2);
    CSMid link3(compact3); m_links.push_back(link3);
    CSMid link4(compact4); m_links.push_back(link4);
    CSMid link5(compact5); m_links.push_back(link5);
    CSMid link6(compact6); m_links.push_back(link6);
    CSMid link7(compact7); m_links.push_back(link7);
    CSMid link8(compact8); m_links.push_back(link8);

    int i=0;
    while( i < 8 && !m_links[i] ) ++i;
    
    if( i<8 )
    {
        m_system  = (m_links[i].isBarrel())?  Barrel : Endcap;
        m_side    = (m_links[i].isAside())?   A_side : C_side;
        m_chamber = (m_links[i].isLarge())?   Long   : Short;
    }
    
    define_mrod_belonging();
}

MRODscheme::MRODscheme(unsigned short int subsystem, 
                       unsigned short int rod, 
                       unsigned short int crate,
                       std::string mnemonic1,std::string mnemonic2,
                       std::string mnemonic3,std::string mnemonic4,
                       std::string mnemonic5,std::string mnemonic6,
                       std::string mnemonic7,std::string mnemonic8) :

  m_Subsystem_ID(subsystem),m_ROD_ID(rod),m_MROD_crate(crate)
{
    m_links.reserve(8);
    m_links.clear();
    
    m_system  = SystemUndefined;
    m_side    = SideUndefined;
    m_chamber = ChamberUndefined;
    
    CSMid link1(mnemonic1); m_links.push_back(link1);
    CSMid link2(mnemonic2); m_links.push_back(link2);
    CSMid link3(mnemonic3); m_links.push_back(link3);
    CSMid link4(mnemonic4); m_links.push_back(link4);
    CSMid link5(mnemonic5); m_links.push_back(link5);
    CSMid link6(mnemonic6); m_links.push_back(link6);
    CSMid link7(mnemonic7); m_links.push_back(link7);
    CSMid link8(mnemonic8); m_links.push_back(link8);
    
    int i=7;
    while( i>=0 && !m_links[i]) --i;
        
    if( i<8 )
    {
        m_system  = (m_links[i].isBarrel())?  Barrel : Endcap;
        m_side    = (m_links[i].isAside())?   A_side : C_side;
        m_chamber = (m_links[i].isLarge())?   Long   : Short;
    }

    define_mrod_belonging();
}

MRODscheme::MRODscheme(const MRODscheme& mrod)
{
    m_Subsystem_ID = mrod.Subsystem_ID();
    m_ROD_ID       = mrod.ROD_ID();
    m_MROD_crate   = mrod.MROD_crate();

    m_links.reserve(8);
    m_links.clear();
    for (int i=0;i<8;++i) m_links.push_back(mrod.links()[i]);

    m_system  = mrod.system();
    m_side    = mrod.side();
    m_chamber = mrod.chamber();
}

MRODscheme::~MRODscheme() 
{
    m_links.clear();
}

void
MRODscheme::define_mrod_belonging()
{
    if ( (m_system = SystemUndefined) ) return;
    
    for(int i=0;i<8;++i) 
    {
        system_belonging  system  = (m_links[i].isBarrel())?  Barrel : Endcap;
        side_belonging    side    = (m_links[i].isAside())?   A_side : C_side;
        chamber_belonging chamber = (m_links[i].isLarge())?   Long   : Short;

        if(system  != m_system  && m_links[i]) m_system  = SystemUndefined;
        if(side    != m_side    && m_links[i]) m_side    = SideUndefined;
        if(chamber != m_chamber && m_links[i]) m_chamber = ChamberUndefined; 
    }

    if(m_system  == SystemUndefined)
    {
      //cout << "WARNING: MROD contains CSM from different system:" << endl;
      //cout << *this;
    }
    if(m_side    == SideUndefined)
    {
	cout << "WARNING: MROD contains CSM from different side:" << endl;
	cout << *this;
    }
    if(m_chamber == ChamberUndefined)
    {
	cout << "WARNING: MROD contains CSM from different chamber type:" 
             << endl;
	cout << *this;
    }
}

MRODscheme 
MRODscheme::operator=(const MRODscheme& mrod)
{
    m_Subsystem_ID = mrod.Subsystem_ID();
    m_ROD_ID       = mrod.ROD_ID();
    m_MROD_crate   = mrod.MROD_crate();

    m_links.clear();
    for (int i=0;i<8;++i) m_links.push_back(mrod.links()[i]); 

    m_system  = mrod.system();
    m_side    = mrod.side();
    m_chamber = mrod.chamber();

    return *this;
}

bool 
MRODscheme::operator==(const MRODscheme& mrod) const
{
    if(m_ROD_ID  == mrod.ROD_ID() &&
       m_system  == mrod.system() &&
       m_side    == mrod.side()   &&
       m_chamber == mrod.chamber()  ) return true;
    else return false;
}

bool 
MRODscheme::operator!=(const MRODscheme& mrod) const
{
    return !(*this == mrod);
}

bool 
MRODscheme::operator% (const MRODscheme& mrod) const
{
    if(m_system  == mrod.system() &&
       m_side    == mrod.side()   &&
       m_chamber == mrod.chamber()  ) return true;
    else return false;
}


bool 
MRODscheme::operator< (const MRODscheme& mrod) const
{
    if(m_ROD_ID  < mrod.ROD_ID()) return true;
    else return false;
}

bool
MRODscheme::isEmpty() const
{
    for (int i=0;i<8;++i)
    {
        if (m_links[i]) return false;
    }
    
    return true;
}

std::string
MRODscheme::CSMsequence_mnemonic() const
{
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream msg;
#else
    // put your gcc 2.95 specific code here
    char buffer[100];

    for (int i=0;i<100;++i) buffer[i] = '\0';
    __osstream msg (buffer,100);
#endif

    msg << "   " << setw(2) << setfill('0') << hex << this->ROD_ID() << dec
        << " ||";

    for(int i=0;i<8;++i)
    {
        msg << " ";

        CSMid csm = this->m_links[i];
	if(csm) msg << csm.mnemonic();
        else msg << "   -   ";
    }
    msg << " | " << setw(2) << setfill(' ') << this->MROD_crate() << endl
        << ends;

    return std::string(msg.str());
}

std::string
MRODscheme::CSMsequence_compact(void) const
{
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream msg;
#else
    // put your gcc 2.95 specific code here
    char buffer[100];

    for (int i=0;i<100;++i) buffer[i] = '\0';
    __osstream msg (buffer,100);
#endif

    msg << "   " << setw(2) << setfill('0') << hex << this->ROD_ID() << dec
        << " ||";
    for(int i=0;i<8;++i)
    {
        msg << " ";
        CSMid csm = this->m_links[i];
	if(csm) msg << "  " << setw(4) << hex << csm.compact() << dec << " ";
        else msg << "   -   ";
    }
    msg << " | " << setw(2) << setfill(' ') << this->MROD_crate() << endl 
        << ends;

    return std::string(msg.str());
}


ostream& operator<<(ostream& stream, const MRODscheme& mrod)
{
    stream << "SUBSYSTEM " << hex << mrod.Subsystem_ID() << dec << endl;
    stream << "#------------------------------------------------------------------------------" << endl
	   << "#ModID||                   MDT chambers / CSM links                      |MROD" << endl
	   << "#tower||    1   |   2   |   3   |   4   |   5   |   6   |   7   |   8    |crate" << endl
	   << "#------------------------------------------------------------------------------" << endl;

    stream << mrod.CSMsequence_mnemonic();

    return stream;
}
