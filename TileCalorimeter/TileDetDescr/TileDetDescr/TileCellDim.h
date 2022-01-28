/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileCellDim_h
#define TileCellDim_h 1

#include <vector>

// fwd
class MsgStream;

class TileCellDim 
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
  MsgStream * m_log;
};

#endif
