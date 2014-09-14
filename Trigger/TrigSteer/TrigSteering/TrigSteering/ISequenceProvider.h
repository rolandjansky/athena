/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: ISequenceProvidering
 * @Class  : ISequenceProvider
 *
 * @brief  Algorithm to provide sequences to signatures
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: ISequenceProvider.h,v 1.2 2007-07-04 18:38:52 eifert Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_ISequenceProvider_H
#define TRIGSTEERING_ISequenceProvider_H

class StatusCode;

namespace HLT {

  class Sequence;


  /**
     @class ISequenceProvider
     this is an interface class (abstract method) that is used e.g. by the TrigSteer class but also TrigDecisionTool
     to find a Sequence which provides a given output TriggerElement

     @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
  */
  class ISequenceProvider
  {

  public:

    /** @brief
	Method to find the sequence from the global sequence list, which produces
	the given TE type. This is needed by the HLT::Signature class
    **/
    virtual HLT::Sequence* findSeqForOutputTeType(const unsigned int teType) = 0; //!< find HLT Sequence producing given TE
  };
} // end namespace

#endif
