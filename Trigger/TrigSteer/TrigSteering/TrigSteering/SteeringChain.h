/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigSteering_SteeringChain_h
#define TrigSteering_SteeringChain_h


#include <vector>
#include <stdint.h>
#include "TrigSteeringEvent/Chain.h"

#include "TrigSteering/StreamTag.h"

class PartialEventBuildingInfo;
class ScoutingInfo;

namespace HLT {

  class AlgoConfig;
  class ISequenceProvider;
  class Signature;
  class IScalerSvc;
  class IScaler;

  /**
   * @brief Chains which can be executed.
   * There are extra members and methods which allow this chain to bexecuted within context of the steering.
   * This nothing new bu the surgery made on HLT::Chain to split it into 2 
   * classes (simple one for analysis), complex for execution
   * The SteeringChain class contains everything it needs to go from step to step, ie each step
   * contains a Signature object which in turn knows what Sequences to run in order
   * to generate the required TEs.
   *
   * @author Tomasz Bold <Tomasz.Bold@cern.ch> - UC Irvine, AGH Krakow - (the split from Chain)
   *
   **/

  class SteeringChain : public Chain {
  public:



    SteeringChain( const TrigConf::HLTChain* configChain,
		   ISequenceProvider* seqs, AlgoConfig* config,
		   IScalerSvc* scalerSvc ); //!< Standard constructor from the configuration


    virtual ~SteeringChain(); //!< destructor

    SteeringChain& operator=(const SteeringChain&); //!< assignment operator


    /** @brief
	- if chain already inactive, stop
	- try to satisfy signature of current step
	  by calling the execute method of the Signature object
	- if success: increase current step
	- if (last_step || no_success): deactivate this chain and return false;
	- else : return true
    */
    HLT::ErrorCode executeStep(); //!< try to go to next step by satisfying the signature of this step

     /** @brief execute the ROB request preparation for this step
     - If the chain is still active in the current step (signature) the prepare ROB request method of this signature is called
     - in case of an error, the chain gets deactivated
     */
    HLT::ErrorCode prepareStepRobRequests(); //!< execute the ROB request preparation for this step


    virtual bool resetChain(); //!< Is called for every new event, resets everything to default (including all underlying objects)
    bool resetROBRequestPreparation(); // called at begin of each event, calls resetROBRequestPreparation for all signatures


    void setPassThroughState(); //!< using the pass-through factor and the configured ScalingSvc, this sets passed-through to true or false
    void setPrescaleState();    //!< using the prescaling factor and the configured ScalingSvc, this sets prescaled to true or false

    void setResurrectedState();    //!< using the rerun (resurrection) factor and the configured ScalingSvc, this sets the isResurrected to true or false

    void setPassedRaw()   { m_passedRaw = true;   }  //!< Set it when chain passed selction by algorithms
    void setPassThrough() { m_passThrough = true; }  //!< Set passThrough true for this event
    void setPrescaled()   { m_prescaled = true;   }  //!< Set prescaled true for this event
    void setResurrected() { m_resurrected = true; }  //!< Set resurrected true for this event
    
    void setActive()        { /*if ( m_signatures.size() > 0 )*/ m_active = true;  }  //!< set chain active
    void setDeactive()      { m_active = false; }                                 //!< set chain deactive

    /** @brief chain to be run in first pass
     * It is placed here as it has to be reused in few places (i.e. monitoring)
     */
    bool runInFirstPass() const; 
    /** @brief chain to be run in second pass
     * chan is to be rerun
     */
    bool runInSecondPass() const; 


    const std::vector<const Signature*> getSignatures() const; //!< return const Signatures (getter for monitoring)
    const std::vector<StreamTag>& getStreamTags() const { return m_streamTags; } //!< return const StreamTags

    bool isActive() const { return m_active; }   //!< is chain active ?
    bool isValid()  const;                       //!< is this a valid Chain ? checks this Chain and all underlying Signatures    

    void setPrescaleMaster(SteeringChain* master);
    std::string& prescaleGroup();

    const PartialEventBuildingInfo* getPEBInfo() const {return m_calibrationROBs; }
    const ScoutingInfo* getScoutingInfo() const {return m_scouting; }
  private:

    bool fillFromConfigChain( ISequenceProvider* seqs, IScalerSvc* scalerSvc );

    bool canAddSignature(unsigned int pos); //!< can we add a signature into this Chain ?

    AlgoConfig* m_config; //!< HLT config object, holding common variables

    IScaler* m_preScaler, *m_passScaler;  //!< engines to determine prescale/passThrough status

    std::vector<Signature*> m_signatures; //!< Signature objects for each step:

    std::vector<StreamTag> m_streamTags;  //!< all StreamTags belonging to this Chain

    bool m_active;         //!< Status of chain: active/deactive

    TrigTimer* m_timer;    //!< timer object
    
    PartialEventBuildingInfo* m_calibrationROBs;
    ScoutingInfo* m_scouting;
     
    SteeringChain *m_prescaleMaster; //!< chain of which the prescaling needs to be synchronize
    std::string m_prescaleGroup;     //!< name of prescaling group (null string if none)
    bool m_prescalingWasRun;         //!< flag signifying that the prescaling was run in this event already and should not be repeated
  };
}

#endif 
