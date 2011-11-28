/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : ZdcRawData.h
// Author   : Peter Steinberg
// Created  : March 2009
//
// DESCRIPTION:
//    A ZdcRawData is the base class for raw data classes,
//    such as ZdcRdo
//    It has only one member element - HWIdentifier
//
// HISTORY:
//    20 March 2009
//
// BUGS:
//
// ***************************************************************************

#ifndef ZDCEVENT_ZDCRAWDATA_H
#define ZDCEVENT_ZDCRAWDATA_H

#include "Identifier/HWIdentifier.h"

#include <string>
#include <vector>

#include <inttypes.h>

class ZdcRawData
{
public:

    /* Constructor: */
    ZdcRawData(){}
    ZdcRawData(const Identifier& id);

    /* Destructor */
    virtual ~ZdcRawData() {}

    /*  Inline accessor methods: */
    inline Identifier   identify(void)  const   { return m_id;   }

    /*
    Identifier         side_ID (void)  const;
    Identifier         module_ID  (void)  const;
    Identifier         type_ID  (void)  const;
    Identifier         channel_ID (void)  const;
    Identifier         gain_ID (void)  const;
    Identifier         delay_ID (void)  const;
    */

    virtual std::string whoami   (void) const   { return "ZdcRawData"; }
    virtual void        print    (void) const;
    // Convertion operator to a std::string 
    // Can be used in a cast operation : (std::string) ZdcRawData
    virtual operator std::string() const;
  
    static void print_to_stream ( const std::vector<double>& val,
                                  const std::string & label, 
                                  std::ostringstream & text);

    static void print_to_stream ( const std::vector<int>& val,
                                  const std::string & label, 
                                  std::ostringstream & text);
private:

    Identifier m_id; // Hardware (online) ID of the adc
};

#endif  //ZDCEVENT_ZDCRAWDATA_H

