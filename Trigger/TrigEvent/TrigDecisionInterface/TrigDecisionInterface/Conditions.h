/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGDECISIONINTERFACE_CONDITIONS_H
#define TRIGDECISIONINTERFACE_CONDITIONS_H
/**********************************************************************************
 * @Project: TrigDecisionInterface
 * @Package: TrigDecisionInterface
 * @Class  : Conditions
 *
 * @brief Conditions flags
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 * @author Lukas Heinrich  <lukas.heinrich@cern.ch> - New York University
 *
 ***********************************************************************************/

namespace TrigDefs {

  // Conditions Mask elements
  // -- we should do this with enum but there are issues with python
  //    so we'll code it as int for now.
  
  const static unsigned int requireDecision          = 0x1;
  // ... otherwise ignore the decision
  const static unsigned int passedThrough            = 0x1 << 1;
  // ... otherwise ignore the passedThrough requirement
  const static unsigned int allowResurrectedDecision = 0x1 << 2;
  // ... otherwise require that it wasn't resurrected
  //
  const static unsigned int enforceLogicalFlow       = 0x1 << 5;
  // ... otherwise we only care about this trigger level
  const static unsigned int eventAccepted            = 0x1 << 6;
  // ... an event was written out (useful for trigger rates)

  //
  const static unsigned int ignoreIOV = 0x1 << 15;
  const static unsigned int alsoDeactivateTEs = 0x1 << 16;
  const static unsigned int ignoreFDR = 0x1 << 17;
  //
  const static unsigned int Physics = requireDecision | enforceLogicalFlow;
  const static unsigned int fullChain = enforceLogicalFlow;

  // Bit location placeholders for isPassedBit()
  const static unsigned int EF_passedRaw              =0x1;
  const static unsigned int EF_passThrough            =0x1 << 1;
  const static unsigned int EF_prescaled              =0x1 << 2;
  const static unsigned int EF_resurrected            =0x1 << 3;

  const static unsigned int L2_passedRaw              =0x1 << 8;
  const static unsigned int L2_passThrough            =0x1 << 9;
  const static unsigned int L2_prescaled              =0x1 << 10;
  const static unsigned int L2_resurrected            =0x1 << 11;

  const static unsigned int L1_isPassedAfterPrescale  = 0x1 << 16;
  const static unsigned int L1_isPassedBeforePrescale = 0x1 << 17;
  const static unsigned int L1_isPassedAfterVeto      = 0x1 << 18;


  const static unsigned int EF_error                  = 0x1 << 19;
  const static unsigned int L2_error                  = 0x1 << 20;

  const static unsigned int Express_passed            = 0x1 << 20;

}

#endif
