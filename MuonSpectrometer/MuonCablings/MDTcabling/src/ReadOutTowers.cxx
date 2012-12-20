/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTcabling/ReadOutTowers.h"

using namespace std;

ReadOutTowers::ReadOutTowers(unsigned short int Subsystem, 
                             unsigned short int EtaTower,
                             unsigned short int Side,
			     bool testbeam) : 
                             m_Subsystem_ID(Subsystem),
			     m_Eta_Tower_ID(EtaTower),
			     m_Side(Side),
			     m_testbeam(testbeam)
{
    m_towers.reserve(20);
    m_towers.clear();
}

ReadOutTowers::ReadOutTowers(const ReadOutTowers& read_out_towers) : 
                             m_Subsystem_ID(read_out_towers.Subsystem_ID()),
			     m_Eta_Tower_ID(read_out_towers.Eta_Tower_ID()),
			     m_Side(read_out_towers.side()),
			     m_testbeam(read_out_towers.testbeam())
{
    m_towers.reserve(20);
    m_towers.clear();
    m_towers = read_out_towers.towers();
}

ReadOutTowers::~ReadOutTowers()
{
    m_towers.clear();
}

ReadOutTowers
ReadOutTowers::operator=(const ReadOutTowers& read_out_towers)
{
    m_Subsystem_ID = read_out_towers.Subsystem_ID();
    m_Eta_Tower_ID = read_out_towers.Eta_Tower_ID();
    m_Side = read_out_towers.side();

    m_towers.clear();
    m_towers = read_out_towers.towers();
    
    m_testbeam = read_out_towers.testbeam();

    return *this;    
}

system_belonging
ReadOutTowers::system() const
{
   if(m_Subsystem_ID == 0x61 || m_Subsystem_ID == 0x62) return Barrel;
   else if (m_Subsystem_ID == 0x63 || m_Subsystem_ID == 0x64) return Endcap;
   else return SystemUndefined;
}

bool
ReadOutTowers::load(const MRODscheme& mrod)
{
    if(!m_testbeam)  if ( mrod.side() != this->side() ) return false;

    m_towers.push_back(mrod); 

    return true;
}

ReadOutTowers&
ReadOutTowers::operator<<(const MRODscheme& mrod)
{
    if(m_towers.size() == 20)
    {
      cout << "WARNING: no space for loading the MROD having " << mrod << endl;
        return *this;
    }

    if( !load(mrod) )
    {
	cout << "WARNING: MROD having " << mrod << endl
             << "CANNOT BE LOADED into Towers for SUBSYSTEM " 
             << hex << m_Subsystem_ID << dec << endl;
    }

    return *this;
}

void 
ReadOutTowers::displayMnemonic(ostream& stream) const
{
    char s = (this->side())? 'A' : 'C';
    stream << "Towers " << setw(2) << m_towers.front().ROD_ID()
	   << " - " << setw(2) << m_towers.back().ROD_ID()
	   << " eta " << m_Eta_Tower_ID << " side " << s << " ID " << hex 
	   << m_Subsystem_ID << dec << endl;
    stream << "{" << endl;

    stream << "#------------------------------------------------------------------------------" << endl
	   << "#ModID||                   MDT chambers / CSM links                      |MROD" << endl
	   << "#tower||    1   |   2   |   3   |   4   |   5   |   6   |   7   |   8    |crate" << endl
	   << "#------------------------------------------------------------------------------" << endl;

    
    chamber_belonging type = m_towers[0].chamber();
    this->displayComment(stream,type);

    Towers::const_iterator it = m_towers.begin();
    while (it != m_towers.end())
    {
        if((*it).chamber() != type)
	{
	    type = (*it).chamber();
            this->displayComment(stream,type);
	}
        stream << (*it).CSMsequence_mnemonic();
        ++it;
    }

    stream << "}" << endl;
}

void
ReadOutTowers::displayComment(ostream& stream,chamber_belonging cham_type)const
{
    if (cham_type == Long)
    {
        stream << endl 
               << "#   \"eta " << m_Eta_Tower_ID
	       << "\"  -  Large chambers in sectors 1,3,5,7,9,11,13,15"
               << endl << endl;
    }
    else if (cham_type == Short)
    {
        stream << endl 
               << "#   \"eta " << m_Eta_Tower_ID
	       << "\"  -  Small chambers in sectors 2,4,6,8,10,12,14,16"
               << endl << endl;
    }
}
    
void 
ReadOutTowers::displayCompact(ostream& stream)  const
{
    char s = (this->side())? 'A' : 'C';
    stream << "Towers " << setw(2) << m_towers.front().ROD_ID()
	   << " - " << setw(2) << m_towers.back().ROD_ID()
	   << " eta " << m_Eta_Tower_ID << " side " << s << " ID " << hex
	   << m_Subsystem_ID << dec << endl;
    stream << "{" << endl;

    stream << "#------------------------------------------------------------------------------" << endl
	   << "#ModID||                   MDT chambers / CSM links                      |MROD" << endl
	   << "#tower||    1   |   2   |   3   |   4   |   5   |   6   |   7   |   8    |crate" << endl
	   << "#------------------------------------------------------------------------------" << endl;

    chamber_belonging type = m_towers[0].chamber();
    this->displayComment(stream,type);

    Towers::const_iterator it = m_towers.begin();
    while (it != m_towers.end())
    {        
        if((*it).chamber() != type)
	{
	    type = (*it).chamber();
            this->displayComment(stream,type);
	}
        stream << (*it).CSMsequence_compact();
        ++it;
    }

    stream << "}" << endl;
}


ostream& operator<<(ostream& stream,const ReadOutTowers& read_out_towers)
{
    read_out_towers.displayMnemonic(stream);
    return stream;
}
