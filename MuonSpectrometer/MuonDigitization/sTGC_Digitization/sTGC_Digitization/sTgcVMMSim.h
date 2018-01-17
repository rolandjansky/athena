/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* This class is a simulation of the sTGC VMM behavior.  It is intended to be used as a module controlling
 * the readout of digits from each readout channel.  It should take a vector of digits for the whole time
 * window of the event being digitized on that channel, the initial time to begin the processing, and the
 * deadtime and readtime windows for the VMM as parameters in its constructior.  Other parameters pertaining
 * to the VMM behavior are controlled in a config file.  This object is designed to be used in an array
 * with other instances of itself so that neighboring strips can turn each other into the READ state
 * in the neighborON mode.
 *
 * Author: Jesse Heilman (Carleton University, Ottawa, Ontario, Canada)
 * April 2017
 */

#ifndef STGCVMMSim_H
#define STGCVMMSim_H

#include <iosfwd>
#include <inttypes.h>
#include "MuonDigitContainer/MuonDigit.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "PathResolver/PathResolver.h"
#include "GaudiKernel/IToolSvc.h"

class sTgcVMMSim
{

public:
    // functions
    sTgcVMMSim()
        : m_deadtimeStart(-9999)
        , m_readtimeStart(-9999)
        , m_currentState(READY)
        , m_vmmTime(0)
        , m_readtimeWindow(0)
        , m_deadtimeWindow(0)
        , m_produceDeadDigits(false)
        , m_channelType(0)
        , m_readoutTick(1)
        , m_mode_neighborOn(false)
        , m_mainThreshold(1)
        , m_neighborThreshold(1)
    {
        readVMMConfig();
    }

    //**********************************************************************
    sTgcVMMSim(const std::vector<sTgcDigit> inputDigits,
        float time,
        float deadWindow,
        float readWindow,
        bool readDeadDigits,
        int typeOfChannel)
        : m_deadtimeStart(-9999)
        , m_readtimeStart(-9999)
        , m_digitsIn(inputDigits)
        , m_currentState(READY)
        , m_vmmTime(time)
        , m_readtimeWindow(readWindow)
        , m_deadtimeWindow(deadWindow)
        , m_produceDeadDigits(readDeadDigits)
        , m_channelType(typeOfChannel)
        , m_readoutTick(1)
        , m_mode_neighborOn(false)
        , m_mainThreshold(1)
        , m_neighborThreshold(1)
    {
        readVMMConfig();
        
    }
    // //**********************************************************************

    bool tick() // advance the m_vmmTime, buffer digits, return live signal
    {
        ATH_MSG_VERBOSE("sTgcVMMSim::tick( ): currentState = " << m_currentState);
        if(m_digitBuffer.size() > 0) {
            m_digitBuffer.clear(); // clear the buffer from the last tick
            ATH_MSG_VERBOSE("Buffer cleared");
        }

        m_vmmTime += m_readoutTick;                              // Advance the m_vmmTime by the readout clock tick
        ATH_MSG_VERBOSE("sTgcVMMSim::tick( ): advance vmmTime " << (m_vmmTime - m_readoutTick) << " to " << m_vmmTime);
        if((m_vmmTime - m_readoutTick) > m_digitsIn.back().time()) // This tick could not contain any digits
            return false;                                    // No more digits exist beyond this m_vmmTime: kill this VMM
        else {
            for(std::vector<sTgcDigit>::iterator it_digit = m_digitsIn.begin(); it_digit != m_digitsIn.end(); ++it_digit) {
                ATH_MSG_VERBOSE("Digit time difference : " << (it_digit->time() - m_vmmTime) );
                if((abs((it_digit->time() - m_vmmTime)) <= m_readoutTick) && (it_digit->time() <= m_vmmTime) && (it_digit->charge() >= m_neighborThreshold || m_channelType != 1)) {
                    ATH_MSG_VERBOSE("Buffering Digit at time : " << it_digit->time() << " charge: " << it_digit->charge());
                    m_digitBuffer.push_back(
                        *it_digit); // buffer digits if they fall inside the tick window and would pass
                                    // the lowest threshold
                }
            }
            return true; // There are digits possible in the future: keep this VMM live
        }
    }
    //**********************************************************************
    bool tock() // Check Deadtime and Update Readout States.  Returns true if a Threshold crossind digit is present
    {
        ATH_MSG_VERBOSE("sTgcVMMSim::tock(): currentState = " << m_currentState << " vmmTime = " << m_vmmTime);
        bool thresh_trig = false;
        if(m_currentState == DEAD && m_vmmTime > (m_deadtimeStart + m_deadtimeWindow)) { // VMM is dead and ready for reset
            m_currentState = READY;
        }
        if(m_currentState == READY) { // VMM is live and looking for an over Threshold digit
            for(std::vector<sTgcDigit>::iterator it_digit = m_digitBuffer.begin(); it_digit != m_digitBuffer.end();
                ++it_digit) {
                ATH_MSG_VERBOSE("Examining Digit at time : " << it_digit->time() << " charge: " << it_digit->charge());
                if(it_digit->charge() >= m_mainThreshold || m_channelType != 1) {
                    ATH_MSG_VERBOSE("Begin VMM READ at time : " << it_digit->time() << " charge: " << it_digit->charge());
                    beginRead(m_vmmTime); // If a digit crosses threshold, begin readout window at the beginning of the tick
                    thresh_trig = true;
                }
            }
        }
        if(m_currentState == READ) { // VMM is in read state either from over threshold digit or NeighborOn signal
            if(m_vmmTime - m_readtimeStart >= m_readtimeWindow) { // If the readout window has elapsed, send the channel dead.
                                                            // This should nominally only happen to NeighborOn triggered
                                                            // channels
                m_currentState = DEAD;
                m_deadtimeStart = m_vmmTime;
            }
        }
        return thresh_trig;
    }
    //**********************************************************************
    sTgcDigit* flush() // Flush digit in this tick from buffer if it is in a READ state
    {
        ATH_MSG_VERBOSE("sTgcVMMSim::flush(): currentState = " << m_currentState);
        if(m_currentState == READ && m_digitBuffer.size() > 0) {
            m_digitOut = &m_digitBuffer[0];
            m_currentState = DEAD;
            m_deadtimeStart = m_digitOut->time();
            return m_digitOut;
        } else if(m_currentState == DEAD && m_produceDeadDigits && m_digitBuffer.size() > 0) {
            m_digitOut = &m_digitBuffer[0];
            m_digitOut->set_isDead(true);
            return m_digitOut;
        } else
            return nullptr;
    }
    //**********************************************************************
    int getState() // Getter for the VMM State
    {
        return m_currentState;
    }

    //**********************************************************************

    void neighborTrigger() // Trigger this channel from a neighbor.  tock() returning true should cause this to be
                           // called from the main program
    {
        if(m_mode_neighborOn)
            setState(READ);
        else
            ATH_MSG_VERBOSE("Neighbor trigger failed: neighborON mode is turned off");
    }

    // Declaring the Message method for further use
    MsgStream& msg(const MSG::Level lvl) const
    {
        return m_msg << lvl;
    }
    bool msgLvl(const MSG::Level lvl) const
    {
        return m_msg.get().level() <= lvl;
    }
    void setMessageLevel(const MSG::Level lvl) const
    {
        m_msg.get().setLevel(lvl);
        
        return;
    }
    void initialReport() {
        ATH_MSG_VERBOSE(m_digitsIn.size() << " digits pased to the VMM");
        for(unsigned int i = 0; i<m_digitsIn.size(); i++){
            ATH_MSG_VERBOSE("Digit In: " << i+1 );
            ATH_MSG_VERBOSE(" digitTime = " << m_digitsIn[i].time()) ;
            ATH_MSG_VERBOSE(" charge = "    << m_digitsIn[i].charge()) ;
        }
        ATH_MSG_VERBOSE(m_mainThreshold << " mainThreshold");
        ATH_MSG_VERBOSE("VMMTime initalized to " << m_vmmTime);
        ATH_MSG_VERBOSE("Backing up VMM clock by one Half Readout Tick to ensure captureof first digit.");
        setTime(m_vmmTime - (m_readoutTick/2.0));
        
        return;
    }

    enum vmmState { READY = 0, READ, DEAD };

private: // data
    // internal variables
    float m_deadtimeStart;
    float m_readtimeStart;
    std::vector<sTgcDigit> m_digitsIn;
    std::vector<sTgcDigit> m_digitBuffer;
    sTgcDigit* m_digitOut;
    vmmState m_currentState;

    // parameters given in constructor
    float m_vmmTime;
    float m_readtimeWindow;
    float m_deadtimeWindow;
    bool m_produceDeadDigits;
    int m_channelType;

    // parameters read from config
    float m_readoutTick;
    bool m_mode_neighborOn;
    float m_mainThreshold;
    float m_neighborThreshold;
    
    //**********************************************************************
    // Functions

    bool beginRead(float triggerTime) // Trigger the beginning of the read window
    {
        if(m_currentState == DEAD || m_currentState == READ) // read window will not start
            return false;
        else // Read window will start
        {
            m_currentState = READ;           // A digit in the buffer triggers a readout
            m_readtimeStart = (triggerTime); // Set the readout window going at the beginning of this tick
            return true;
        }
    }
    //**********************************************************************
    void setState(vmmState state) // Setter for the VMM State
    {
        m_currentState = state;
    }
    //**********************************************************************
    void setTime(float time) // Setter for the VMM Time
    {
        m_vmmTime = time;
    }
    //**********************************************************************
    void readVMMConfig() // Read config parameters in from the VMM config file
    {
        const char* const fileName = "sTGC_Digitization_VMM.config"; // Located in the share folder of the package
        std::string fileWithPath = PathResolver::find_file(fileName, "DATAPATH");

        ATH_MSG_DEBUG("Reading VMM config file");

        std::ifstream ifs;
        if(fileWithPath != "") {
            ifs.open(fileWithPath.c_str(), std::ios::in);
        } else {
            ATH_MSG_FATAL("readDeadtimeConfig(): Could not find file " << fileName);
            exit(-1);
        }

        if(ifs.bad()) {
            ATH_MSG_FATAL("readDeadtimeConfig(): Could not open file " << fileName);
            exit(-1);
        }

        std::string var;
        float value;

        while(ifs.good()) {
            ifs >> var >> value;
            if(var.compare("neighborON") == 0) {
                m_mode_neighborOn = (bool)value;
                ATH_MSG_DEBUG("m_deadtimeON = " << (bool)value);
                continue;
            }
            if(var.compare("mainThreshold") == 0) {
                m_mainThreshold = value;
                ATH_MSG_DEBUG("mainThreshold = " << value);
                continue;
            }
            if(var.compare("neighborThreshold") == 0) {
                m_neighborThreshold = value;
                ATH_MSG_DEBUG("neighborThreshold = " << value);
                continue;
            }
            if(var.compare("readoutTick") == 0) {
                m_readoutTick = value;
                ATH_MSG_DEBUG("readoutTick = " << value);
                continue;
            }
            ATH_MSG_WARNING("Unknown value encountered reading VMM.config");
        }

        ifs.close();
    }

protected:
    // Declaring private message stream member.
    mutable Athena::MsgStreamMember m_msg = Athena::MsgStreamMember("sTgcVMMSim");
};

#endif
