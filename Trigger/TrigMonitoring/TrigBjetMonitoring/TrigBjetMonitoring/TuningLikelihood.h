/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *****************************************************
//
// NAME:     TuningLikelihood.h
// PACKAGE:  Trigger/TrigMonitoring/TrigBjetMonitoring
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// *****************************************************

#ifndef TRIGBJETMONITORING_TUNINGLIKELIHOOD
#define TRIGBJETMONITORING_TUNINGLIKELIHOOD



/**
 * @brief Private class for likelihood templates.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 *
 * This is the private class used to store likelihood calibrations which are set through the following python files:
  * - python/TrigBjetFexTuningEFID.py: calibration of EF distributions.
 */


class TuningLikelihood {

 public:


  TuningLikelihood();   

  ~TuningLikelihood();


 private:


};


#endif
