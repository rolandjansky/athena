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

  TuningLikelihood(const int*, const float*, const float*, const int);    //EN
  //  TuningLikelihood(const float*, const float*, const float*, const int);
  ~TuningLikelihood();

  float getBLikelihoodValue(int index) {return m_bLikelihood[index];};
  float getULikelihoodValue(int index) {return m_uLikelihood[index];};

  /*
  const float* getMinXLikelihood() {return m_minXLikelihood;};
  const float* getMaxXLikelihood() {return m_maxXLikelihood;};
  const float* getNumXLikelihood() {return m_numXLikelihood;};

  const float* getMinYLikelihood() {return m_minYLikelihood;};
  const float* getMaxYLikelihood() {return m_maxYLikelihood;};
  const float* getNumYLikelihood() {return m_numYLikelihood;};

  const float* getMinZLikelihood() {return m_minZLikelihood;};
  const float* getMaxZLikelihood() {return m_maxZLikelihood;};
  const float* getNumZLikelihood() {return m_numZLikelihood;};
  */

  const int* getMinXLikelihood() {return m_minXLikelihood;};
  const int* getMaxXLikelihood() {return m_maxXLikelihood;};
  const int* getNumXLikelihood() {return m_numXLikelihood;};

  const int* getMinYLikelihood() {return m_minYLikelihood;};
  const int* getMaxYLikelihood() {return m_maxYLikelihood;};
  const int* getNumYLikelihood() {return m_numYLikelihood;};

  const int* getMinZLikelihood() {return m_minZLikelihood;};
  const int* getMaxZLikelihood() {return m_maxZLikelihood;};
  const int* getNumZLikelihood() {return m_numZLikelihood;};

 private:

  bool is1D, is2D, is3D;

  //  const float *m_numXLikelihood, *m_minXLikelihood, *m_maxXLikelihood;
  //  const float *m_numYLikelihood, *m_minYLikelihood, *m_maxYLikelihood;
  //  const float *m_numZLikelihood, *m_minZLikelihood, *m_maxZLikelihood;

  const int *m_numXLikelihood, *m_minXLikelihood, *m_maxXLikelihood;
  const int *m_numYLikelihood, *m_minYLikelihood, *m_maxYLikelihood;
  const int *m_numZLikelihood, *m_minZLikelihood, *m_maxZLikelihood;

  const float *m_bLikelihood, *m_uLikelihood;
  const int m_dimensionLikelihood;

};


#endif
