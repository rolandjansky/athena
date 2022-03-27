/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * Updates:
 * - 2022 Jan, Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>
 *   Added MsgStream for logging
 */

#ifndef TileCellDim_h
#define TileCellDim_h 1

#include "AthenaBaseComps/AthMessaging.h"
#include <vector>

class TileCellDim
  : public AthMessaging
{
 public:
  TileCellDim(unsigned int nRows);
  ~TileCellDim();

  // Data access
  inline unsigned int getNRows() const { return m_nRows;}
  inline double getVolume() const { return m_volume; }

  double getRMin(unsigned int index) const;
  double getRMax(unsigned int index) const;
  double getZMin(unsigned int index) const;
  double getZMax(unsigned int index) const;
  
  // Filling in the contents
  void addRMin(double rMin);
  void addRMax(double rMax);
  void addZMin(double zMin);
  void addZMax(double zMax);
  void computeVolume();
  double computeRowVolume(int iRow);

  void print() const;
  
 private:
  unsigned int m_nRows;
  std::vector<double> m_rMin;
  std::vector<double> m_rMax;
  std::vector<double> m_zMin;
  std::vector<double> m_zMax;
  double m_volume, m_Radius2HalfLength;
};

#endif
