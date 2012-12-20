/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef READOUTTOWERS_H
#define READOUTTOWERS_H

#include <vector>
#include <iostream>
#include <iomanip>
#include "MDTcabling/MRODscheme.h"



class ReadOutTowers
{
    public:
    typedef std::vector<MRODscheme> Towers;

    private:
    unsigned short int m_Subsystem_ID;
    unsigned short int m_Eta_Tower_ID;
    unsigned short int m_Side;
    
    bool m_testbeam;
    
    Towers m_towers;

    bool load(const MRODscheme& mrod);

    void displayComment(std::ostream& stream,chamber_belonging type) const;

    public:
    ReadOutTowers(unsigned short int Subsystem, unsigned short int EtaTower,
                  unsigned short int Side, bool testbeam);
		  
    ReadOutTowers(const ReadOutTowers& read_out_towers);
    ~ReadOutTowers();

    ReadOutTowers operator=(const ReadOutTowers& read_out_towers);

    unsigned short int Subsystem_ID(void) const {return m_Subsystem_ID;}
    unsigned short int Eta_Tower_ID(void) const {return m_Eta_Tower_ID;}
    unsigned short int Side(void)         const {return m_Side;}
    bool testbeam(void)                   const {return m_testbeam;}
    const Towers& towers(void)            const {return m_towers;}
    

    side_belonging side(void)     const {
                                  return static_cast<side_belonging>(m_Side);}
    system_belonging system(void) const;

    ReadOutTowers& operator<<(const MRODscheme& mrod);

    void displayMnemonic(std::ostream&) const;
    void displayCompact(std::ostream&)  const;

    friend std::ostream& operator<<(std::ostream& stream,const ReadOutTowers& towers);

};




#endif
