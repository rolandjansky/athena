/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : ZdcRawChannel.h
// Author   : Peter Steinberg
// Created  : March 2009
//
// DESCRIPTION:
//    A ZdcRawChannel is the data class containing energy/time information
//
// HISTORY:
//
// ***************************************************************************

#ifndef ZDCEVENT_ZDCRAWCHANNEL_H
#define ZDCEVENT_ZDCRAWCHANNEL_H

#include "ZdcEvent/ZdcRawData.h"
#include "CLIDSvc/CLASS_DEF.h"

class ZdcRawChannel : public ZdcRawData
{
public:

    /* Constructors */

    ZdcRawChannel() {
      m_size = 0;
    }

    ZdcRawChannel(const Identifier& id): ZdcRawData(id)
    //This will hold high/low gain combinations of up to 4 different methods
    //of reconstruction
    {
        m_size = 8;
    	m_energy.resize(8,0.);
    	m_time.resize(8,0.);
    	m_chi.resize(8,0.);
    }

    /* Destructor */

    virtual ~ZdcRawChannel() { }

    /* Inline access methods */



    std::string whoami   (void) const { return "ZdcRawChannel"; }
    void        print    (void) const{};
    // Convertion operator to a std::string
    // Can be used in a cast operation : (std::string) ZdcRawChannel
    operator std::string() const {return "ZdcRawChannel::string()";};

private:

    std::vector<float>  m_energy;
    std::vector<float>  m_time;
    std::vector<float>  m_chi;

    unsigned int m_size;

 public:

    void   setSize(unsigned int i) {
    	m_size = i;
    	m_energy.resize(i,0.);
    	m_time.resize(i,0.);
    	m_chi.resize(i,0.);

    }


    void   setEnergy(int i, float e) {m_energy[i] = e;}
    void   setTime  (int i, float t)  {m_time[i] = t;}
    void   setChi   (int i, float c)  {m_chi[i] = c;}

	unsigned int getSize () const {return m_energy.size();}

    float getEnergy(int i) const {return m_energy[i];}
    float getTime(int i)   const {return m_time[i];}
    float getChi(int i)    const {return m_chi[i];}

};

CLASS_DEF(ZdcRawChannel,92926131,0)


#endif  //ZDCEVENT_ZDCDIGITS_H
