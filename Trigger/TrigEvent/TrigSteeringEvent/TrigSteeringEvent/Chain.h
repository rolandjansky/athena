/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
/**********************************************************************************
 * @Project: HLT SteeringEvent
 * @Package: TrigSteeringEvent
 * @class  : Chain
 *
 * @brief  chain of signatures
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * moved from TrigSteering package
 *
 * File and Version Information:
 **********************************************************************************/

#ifndef TRIGSTEERINGEVENT_CHAIN_H
#define TRIGSTEERINGEVENT_CHAIN_H

#include <vector>
#include <stdint.h>
#include <iomanip> 

#include "TrigSteeringEvent/Enums.h"
#include "TrigConfHLTData/HLTChain.h"

#ifdef XAOD_STANDALONE
#include "AsgMessaging/MsgStream.h"
#include "AsgMessaging/MsgStreamMacros.h"
#endif
#ifndef XAOD_STANDALONE
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#endif

//#include "TrigSteeringEvent/GenericResult.h"

#include <iosfwd>

class MsgStream;
class TrigTimer;


namespace HLT {

  static const std::string no_config = "no config";

  // taht is for backward compatibility of TDT, once new TDT will be available it can be removed from steering
  enum ChainStatus { ChainInvalid=0, ConfigOnlyChain, ChainOK };

  /**
     @class Chain
     This class represents one chain of signatures, i.e. one signature for each step.
     The HLT::Chain class also holds information about its status, including prescales,
     pass-throughs etc.

     @author Till Eifert     <Till.Eifert@cern.ch>
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
     @author Tomasz Bold     <Tomasz.Bold@cern.ch>     - U. of California - Irvine
  */
  class Chain
  {

  public:


    //Chain( const std::vector<uint32_t>& serialData ); //!< constructor from serialized data
    
    Chain( uint32_t serialData ); //!< constructor from serialized data

    Chain(const TrigConf::HLTChain* configChain);

    ~Chain(){}


    const TrigConf::HLTChain* getConfigChain() const { return m_configChain; } //!< get underlying ConfigChain


    bool chainPassedRaw() const { return m_passedRaw; }
    bool chainPassed()    const;


    bool isPassedThrough() const { return m_passThrough; }  //!< is chain passed through ?
    bool isPrescaled()     const { return m_prescaled;   }  //!< is chain prescaled ?
    bool isResurrected()   const { return m_resurrected; }  //!< is chain resurrected ?

    // For Lvl Converters

    float prescaleFactor()                  const { return (m_configChain ? m_configChain->prescale() : -1.); } //!< get prescale factor
    float passThroughFactor()               const { return (m_configChain ? m_configChain->pass_through() : -1.); }  //!< get pass-through factor
    unsigned int getChainCounter()          const { return m_chain_counter; }   //!< return the unique identifier of this Chain (uint)
    const std::string&  getChainName()      const { return (m_configChain ? m_configChain->chain_name(): no_config); }    //!< return the Chain name (string)
    const std::string&  getLowerChainName() const { return (m_configChain ? m_configChain->lower_chain_name(): no_config); }    //!< return the Chain name (string)
    int  getEBAfterStep()                   const { return (m_configChain ? m_configChain->EB_after_step() : -1.); }    //!< get EB_after_step  
    bool nextStepAfterEB()                  const { return ((getChainStep()+1) > getEBAfterStep()) && (getEBAfterStep()>0.); } //!< return whether next step requires EB (-1 means no EB called)
    bool isMerged()                         const { return (m_configChain ? (m_configChain->level()=="HLT") : false);}; //!<< return whether is a merged L2+EF chain 

    unsigned int getChainHashId()  const { return (m_configChain ? m_configChain->chain_hash_id() : 0); } //!< return the Chain name's hash ID
    HLT::ErrorCode getErrorCode()  const { return m_errorCode; }          //!< return this Chain's most severe error code (from execution)
    int getChainStep()             const { return m_currentStep; }        //!< return the current step of execution


    /**
       @brief templated print stream method
       so that one can simply write std::cout << myChain << std::endl;
       or with the Gaudi Msg stram msgStram << myChain << ...
    **/
    template <class stream>
    void print(stream& os) const
    {
      os << "Counter = " << std::setw(4) <<getChainCounter()
	 << " success (raw) = " << chainPassedRaw()
	 << " EBstep = " << getEBAfterStep()
	 << " pass-through = " << isPassedThrough()
	 << " prescaled = " << isPrescaled()
	 << " rerun = " << isResurrected()
	 << " lastActiveStep = " << getChainStep()
	 << " name = " << std::setw(35) << getChainName() 
	 << " EC = "<< strErrorCode(getErrorCode()) ;
    }

    ErrorCode serialize( std::vector<uint32_t>& output ) const; //!< serialize this Chain into the given vector of uint's

    ErrorCode deserialize(uint32_t chainWord); //!< deserialize this Chain from given vector of uint's

    ErrorCode setDecisions(bool passedraw, bool passedthrough, bool prescaled, bool resurrected); //!< set bool decisions directly


    static unsigned int inquireChainCounter(uint32_t chainWord); //!< unpack chain counter from the serialized word

    ChainStatus getStatus() const { return m_status; } //!< return Chain status

    bool reset(); //!< restes the bits to the basic state

  protected:


    void setStatus(ChainStatus s) { m_status = s; }

    const TrigConf::HLTChain* m_configChain; //!< underlying Config Chain    
    unsigned int m_chain_counter;       //!< chain counter from configuration (copied here for speed)
    ErrorCode m_errorCode;        //!< most severe  error code of execution   
    bool m_passedRaw;
    bool m_passThrough; //!< Flagged as "passedThrough" for this event ?
    bool m_prescaled;   //!< Flagged as "prescaled" for this event ?
    bool m_resurrected; //!< flag to mar that chain was originally prescalled but is reexecuted
    int  m_currentStep;  //!< Current step of this chain

  private:
    ChainStatus m_status; //!< Chain status, enum {ChainInvalid, ConfigOnlyChain, ChainOK }

  };


/** @brief
    Overload of << operator for both, MsgStream and std::ostream
**/
MsgStream& operator << (MsgStream&, const Chain&);
std::ostream& operator << (std::ostream&, const Chain&);

} // end of namespace


#endif
