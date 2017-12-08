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
        : deadtimeStart(-9999)
        , readtimeStart(-9999)
        , currentState(READY)
        , vmmTime(0)
        , readtimeWindow(0)
        , deadtimeWindow(0)
        , produceDeadDigits(false)
        , channelType(0)
        , readoutTick(1)
        , mode_neighborOn(false)
        , mainThreshold(1)
        , neighborThreshold(1)
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
        : deadtimeStart(-9999)
        , readtimeStart(-9999)
        , digitsIn(inputDigits)
        , currentState(READY)
        , vmmTime(time)
        , readtimeWindow(readWindow)
        , deadtimeWindow(deadWindow)
        , produceDeadDigits(readDeadDigits)
        , channelType(typeOfChannel)
        , readoutTick(1)
        , mode_neighborOn(false)
        , mainThreshold(1)
        , neighborThreshold(1)
    {
        readVMMConfig();
        
    }
    // //**********************************************************************

    bool tick() // advance the vmmTime, buffer digits, return live signal
    {
        ATH_MSG_VERBOSE("sTgcVMMSim::tick( ): currentState = " << currentState);
        if(digitBuffer.size() > 0) {
            digitBuffer.clear(); // clear the buffer from the last tick
            ATH_MSG_VERBOSE("Buffer cleared");
        }

        vmmTime += readoutTick;                              // Advance the vmmTime by the readout clock tick
        ATH_MSG_VERBOSE("sTgcVMMSim::tick( ): advance vmmTime " << (vmmTime - readoutTick) << " to " << vmmTime);
        if((vmmTime - readoutTick) > digitsIn.back().time()) // This tick could not contain any digits
            return false;                                    // No more digits exist beyond this vmmTime: kill this VMM
        else {
            for(std::vector<sTgcDigit>::iterator it_digit = digitsIn.begin(); it_digit != digitsIn.end(); ++it_digit) {
                ATH_MSG_VERBOSE("Digit time difference : " << (it_digit->time() - vmmTime) );
                if((abs((it_digit->time() - vmmTime)) <= readoutTick) && (it_digit->time() <= vmmTime) && (it_digit->charge() >= neighborThreshold || channelType != 1)) {
                    ATH_MSG_VERBOSE("Buffering Digit at time : " << it_digit->time() << " charge: " << it_digit->charge());
                    digitBuffer.push_back(
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
        ATH_MSG_VERBOSE("sTgcVMMSim::tock(): currentState = " << currentState << " vmmTime = " << vmmTime);
        bool thresh_trig = false;
        if(currentState == DEAD && vmmTime > (deadtimeStart + deadtimeWindow)) { // VMM is dead and ready for reset
            currentState = READY;
        }
        if(currentState == READY) { // VMM is live and looking for an over Threshold digit
            for(std::vector<sTgcDigit>::iterator it_digit = digitBuffer.begin(); it_digit != digitBuffer.end();
                ++it_digit) {
                ATH_MSG_VERBOSE("Examining Digit at time : " << it_digit->time() << " charge: " << it_digit->charge());
                if(it_digit->charge() >= mainThreshold || channelType != 1) {
                    ATH_MSG_VERBOSE("Begin VMM READ at time : " << it_digit->time() << " charge: " << it_digit->charge());
                    beginRead(vmmTime); // If a digit crosses threshold, begin readout window at the beginning of the tick
                    thresh_trig = true;
                }
            }
        }
        if(currentState == READ) { // VMM is in read state either from over threshold digit or NeighborOn signal
            if(vmmTime - readtimeStart >= readtimeWindow) { // If the readout window has elapsed, send the channel dead.
                                                            // This should nominally only happen to NeighborOn triggered
                                                            // channels
                currentState = DEAD;
                deadtimeStart = vmmTime;
            }
        }
        return thresh_trig;
    }
    //**********************************************************************
    sTgcDigit* flush() // Flush digit in this tick from buffer if it is in a READ state
    {
        ATH_MSG_VERBOSE("sTgcVMMSim::flush(): currentState = " << currentState);
        if(currentState == READ && digitBuffer.size() > 0) {
            digitOut = &digitBuffer[0];
            currentState = DEAD;
            deadtimeStart = digitOut->time();
            return digitOut;
        } else if(currentState == DEAD && produceDeadDigits && digitBuffer.size() > 0) {
            digitOut = &digitBuffer[0];
            digitOut->set_isDead(true);
            return digitOut;
        } else
            return nullptr;
    }
    //**********************************************************************
    int getState() // Getter for the VMM State
    {
        return currentState;
    }

    //**********************************************************************

    void neighborTrigger() // Trigger this channel from a neighbor.  tock() returning true should cause this to be
                           // called from the main program
    {
        if(mode_neighborOn)
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
        ATH_MSG_VERBOSE(digitsIn.size() << " digits pased to the VMM");
        for(unsigned int i = 0; i<digitsIn.size(); i++){
            ATH_MSG_VERBOSE("Digit In: " << i+1 );
            ATH_MSG_VERBOSE(" digitTime = " << digitsIn[i].time()) ;
            ATH_MSG_VERBOSE(" charge = "    << digitsIn[i].charge()) ;
        }
        ATH_MSG_VERBOSE(mainThreshold << " mainThreshold");
        ATH_MSG_VERBOSE("VMMTime initalized to " << vmmTime);
        ATH_MSG_VERBOSE("Backing up VMM clock by one Half Readout Tick to ensure captureof first digit.");
        setTime(vmmTime - (readoutTick/2.0));
        
        return;
    }

    enum vmmState { READY = 0, READ, DEAD };

private: // data
    // internal variables
    float deadtimeStart;
    float readtimeStart;
    std::vector<sTgcDigit> digitsIn;
    std::vector<sTgcDigit> digitBuffer;
    sTgcDigit* digitOut;
    vmmState currentState;

    // parameters given in constructor
    float vmmTime;
    float readtimeWindow;
    float deadtimeWindow;
    bool produceDeadDigits;
    int channelType;

    // parameters read from config
    float readoutTick;
    bool mode_neighborOn;
    float mainThreshold;
    float neighborThreshold;
    
    //**********************************************************************
    // Functions

    bool beginRead(float triggerTime) // Trigger the beginning of the read window
    {
        if(currentState == DEAD || currentState == READ) // read window will not start
            return false;
        else // Read window will start
        {
            currentState = READ;           // A digit in the buffer triggers a readout
            readtimeStart = (triggerTime); // Set the readout window going at the beginning of this tick
            return true;
        }
    }
    //**********************************************************************
    void setState(vmmState _state) // Setter for the VMM State
    {
        currentState = _state;
    }
    //**********************************************************************
    void setTime(float _time) // Setter for the VMM Time
    {
        vmmTime = _time;
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
                mode_neighborOn = (bool)value;
                ATH_MSG_DEBUG("m_deadtimeON = " << (bool)value);
                continue;
            }
            if(var.compare("mainThreshold") == 0) {
                mainThreshold = value;
                ATH_MSG_DEBUG("mainThreshold = " << value);
                continue;
            }
            if(var.compare("neighborThreshold") == 0) {
                neighborThreshold = value;
                ATH_MSG_DEBUG("neighborThreshold = " << value);
                continue;
            }
            if(var.compare("readoutTick") == 0) {
                readoutTick = value;
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
