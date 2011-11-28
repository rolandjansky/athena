/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : ZdcCell.h
// Author   : Peter Steinberg
// Created  : March 2009
//
// DESCRIPTION:
//    A ZdcCell is the data class containing energy/position information for the "pixel" ZDC channels
//    
// HISTORY:
//
// ***************************************************************************

#ifndef ZDCEVENT_ZDCCell_H
#define ZDCEVENT_ZDCCell_H

#include "ZdcEvent/ZdcRawData.h"

class ZdcCell : public ZdcRawData
{
public:
   
    /* Constructors */

    ZdcCell() { }

    ZdcCell(const Identifier& id);

    /* Destructor */

    virtual ~ZdcCell() { }

    /* Inline access methods */

    

    std::string whoami   (void) const { return "ZdcCell"; }
    void        print    (void) const {};
    // Convertion operator to a std::string 
    // Can be used in a cast operation : (std::string) ZdcCell
    operator std::string() const { return "ZdcCell::string()";};

private:

    double   m_energy;
    double   m_time;
    double   m_x;
    double   m_y;

 public:

    void   setEnergy(double e){m_energy = e;}
    double getEnergy() {return m_energy;}
    void   setTime(double t){m_time = t;}
    double getTime() {return m_time;}
    void   setX(double x){m_x = x;}
    double getX() {return m_x;}
    void   setY(double y){m_x = y;}
    double getY() {return m_y;}

};

typedef std::vector<ZdcCell> ZdcCellVec;

#endif  //ZDCEVENT_ZDCDIGITS_H

