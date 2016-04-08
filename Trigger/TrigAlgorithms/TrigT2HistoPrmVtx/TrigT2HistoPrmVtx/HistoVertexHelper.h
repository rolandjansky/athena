/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     HistoVertexHelper.h
// PACKAGE:  Trigger/TrigHypothesis/TrigT2HistoPrmVtx
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro@ge.infn.it
// 
// ************************************************

#ifndef TRIGT2HISTOPRMVTX_HISTOVERTEXHELPER
#define TRIGT2HISTOPRMVTX_HISTOVERTEXHELPER


/**
 * @brief Class for histogramming management.
 *
 * @author Andrea Coccaro <Andrea.Coccaro@ge.infn.it>
 *
 * This is the base class for histogramming Z coordinate of tracks and finding primary vertex candidates.
 */


class HistoVertexHelper
{

 public:

  HistoVertexHelper();
  HistoVertexHelper(int, int, int);
  HistoVertexHelper(const HistoVertexHelper&);
  ~HistoVertexHelper();

  HistoVertexHelper& operator=(const HistoVertexHelper&);

  /** @brief To fill the bin, with unitary weight, corresponding to a certain longitudinal impact parameter, watchin out for underflow and overflow. */
  bool fill(float);
  /** @brief To get the total number of bins of the histogram. */
  int getNBin() {return m_N;}; 
  /** @brief To get the total number of entries of a certain bin. */
  int getN(int);
  /** @brief To get the bin given a certain Z coordinate. */
  int getBin(float);
  /** @brief To get the Z coordinate given a certain bin. */
  float getZ(float);
  /** @brief To retrieve the number of entries in a given interval. */
  int getIntegral(int, int);
  /** @brief To reset the histogram entries in all its range. */
  bool reset();
  /** @brief To reset the histogram entries in the range passed to the function. */
  bool reset(int, int);

 private:

  int* m_zCoord;

  /** @brief Total number of bins (default is 8000 bins).*/
  int m_N;
  /** @brief Lower histogram bound .*/
  int m_min;
  /** @brief Upper histogram bound .*/
  int m_max;

};

#endif
