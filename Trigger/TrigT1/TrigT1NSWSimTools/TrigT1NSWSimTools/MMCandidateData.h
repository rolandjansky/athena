/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMCANDIDATEDATA_H
#define MMCANDIDATEDATA_H


// local includes
#include <stdint.h>
#include <ostream>

// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  /**
   *
   *   @short abstract class to interface the data describing the MM trigger candidatei; to be delivered to the trigger sector logic
   *
   * This class contains pure virtual methods to be implemented by the derived classes.
   * These latters implement the offline and online version of this data fragment.
   *
   *  @authors Alessandro Di Mattia <dimattia@cern.ch>, Ourania Sidiropoulou <osidirop@cern.ch>
   *
   *
   */

  class MMCandidateData {

  public:
    MMCandidateData();
    virtual ~MMCandidateData();

    //! methods for retrieving the bare data
    virtual uint32_t id()     const = 0; //!< get the 32 bit word identifing the fragment

    virtual int trig_BCID()   const = 0;

    //! helper  methods decoding the trigger candiate fragment delivered by the FPGA trigger logic for the NSW detector
    virtual float deltaTheta()  const = 0; //!< Delta Theta measurement: defines a prompt track if found within the trigger threshold
    virtual float eta()         const = 0; //!< Eta position of the prompt track candidate
    virtual float etaSigma()    const = 0; //!< Sigma of the Eta position of the prompt track candidate
    virtual float phi()         const = 0; //!< Phi position of the prompt track candidate
    virtual float phiSigma()    const = 0; //!< Sigma of the Phi position of the prompt track candidate

  };  // end of MMCandidateData class

  inline MMCandidateData::MMCandidateData() {}

  inline MMCandidateData::~MMCandidateData() {}

} // namespace NSWL1

#endif
