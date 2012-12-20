/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MRODSCHEME_H
#define MRODSCHEME_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>

#if (__GNUC__) && (__GNUC__ > 2) 
               // put your gcc 3.2 specific code her
	       #include <sstream>
	       typedef std::ostringstream __osstream;
#else
               // put your gcc 2.95 specific code here
	       #include <strstream>
	       typedef ostrstream __osstream;
#endif


#include "MDTcabling/CSMid.h"

class MDTGeometry;

enum system_belonging  {Barrel,Endcap,SystemUndefined};
enum side_belonging    {C_side,A_side,SideUndefined};
enum chamber_belonging {Long,Short,ChamberUndefined};

class MRODscheme
{
    public:
    typedef std::vector< CSMid > CSMvec;

    private:
    unsigned short int m_Subsystem_ID;
    unsigned short int m_ROD_ID;
    unsigned short int m_MROD_crate;

    system_belonging  m_system;
    side_belonging    m_side;
    chamber_belonging m_chamber;

    CSMvec m_links;

    void define_mrod_belonging(void);


    public:
    MRODscheme(unsigned short int subsystem, 
               unsigned short int rod, 
               unsigned short int crate, 
               CSMid* links);

    MRODscheme(unsigned short int subsystem, 
               unsigned short int rod, 
               unsigned short int crate,
               unsigned short int compact1,unsigned short int compact2,
               unsigned short int compact3,unsigned short int compact4,
               unsigned short int compact5,unsigned short int compact6,
               unsigned short int compact7,unsigned short int compact8);

    MRODscheme(unsigned short int subsystem, 
               unsigned short int rod, 
               unsigned short int crate,
               std::string mnemonic1,std::string mnemonic2,
               std::string mnemonic3,std::string mnemonic4,
               std::string mnemonic5,std::string mnemonic6,
               std::string mnemonic7,std::string mnemonic8);

    MRODscheme(const MRODscheme& mrod);

    ~MRODscheme();

    MRODscheme operator=(const MRODscheme& mrod);

    bool operator==(const MRODscheme& mrod) const;
    bool operator!=(const MRODscheme& mrod) const;
    bool operator% (const MRODscheme& mrod) const;
    bool operator< (const MRODscheme& mrod) const;

    bool isSameSystem (const MRODscheme& mrod) const 
        {return (m_system == mrod.system());}
    bool isSameSide   (const MRODscheme& mrod) const
        {return (m_side == mrod.side());}
    bool isSameChamber(const MRODscheme& mrod) const
        {return (m_chamber == mrod.chamber());}

    bool isEmpty (void) const;

    unsigned int Subsystem_ID(void) const {return m_Subsystem_ID;}
    unsigned int ROD_ID(void)       const {return m_ROD_ID;}
    unsigned int MROD_crate(void)   const {return m_MROD_crate;}
    const CSMvec& links(void)       const {return m_links;}

    system_belonging  system(void)  const {return m_system;}
    side_belonging    side(void)    const {return m_side;}
    chamber_belonging chamber(void) const {return m_chamber;}

    unsigned short int Side(void)   const 
                              {return static_cast<unsigned short int>(m_side);}

    std::string CSMsequence_mnemonic(void) const;
    std::string CSMsequence_compact (void) const;

    friend std::ostream& operator<<(std::ostream& stream,const MRODscheme& mrod);

    friend class MDTGeometry;

    private:
    CSMvec& csm(void) {return m_links;}
};


#endif
