/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBH6RUNHEADER_H
#define TBREC_TBH6RUNHEADER_H
#define CELLMAX 1200
// 
// TBH6RunHeader:
// Run header for ROOT Tree of H6 CBT
//

#include "TObject.h"
class TBH6RunHeader: public TObject
//class TBH6RunHeader
{
 public:
  TBH6RunHeader(const int nRun=0);
  ~TBH6RunHeader();
  int runNum() const;
  float beamMomentum() const;
  float xCryo() const;
  float yTable() const;
  float zCalo() const;
  int cellNum() const;
  int const * cellID() const;
  float const * cellNoiseRMS() const;
  float const * cellEta() const;
  float const * cellPhi() const;
  float const * cellX() const;
  float const * cellY() const;
  float const * cellZ() const;
  float const * cellR() const;
  float const * celldEta() const;
  float const * celldPhi() const;
  float const * celldZ() const;
  float const * celldR() const;
  float const * cellVolume() const;
  int const * celliEta() const;
  int const * celliPhi() const;
  int const * cellRegion() const;
  int const * cellCalo() const;
  int const * cellSampling() const;
  void SetRunNum(int run);
  void SetBeamMomentum(float mom);
  void SetXCryo(float x);
  void SetYTable(float y);
  void SetZCalo(float z);
  void SetCellNum(int nCell);
  void SetCellID(const int i, const int id);
  void SetCellNoiseRMS(const int i, const float rms);
  void SetCellEta(const int i, const float eta);
  void SetCellPhi(const int i, const float phi);
  void SetCellX(const int i, const float x);
  void SetCellY(const int i, const float y);
  void SetCellZ(const int i, const float z);
  void SetCellR(const int i, const float r);
  void SetCelldEta(const int i, const float deta);
  void SetCelldPhi(const int i, const float dphi);
  void SetCelldZ(const int i, const float dz);
  void SetCelldR(const int i, const float dr);
  void SetCellVolume(const int i, const float vol);
  void SetCelliEta(const int i, const int ieta);
  void SetCelliPhi(const int i, const int iphi);
  void SetCellRegion(const int i, const int reg);
  void SetCellCalo(const int i, const int calo);
  void SetCellSampling(const int i, const int subc);

 private:

  int m_nRun;
  float m_beamMomentum;
  float m_xCryo;
  float m_yTable;
  float m_zCalo;
  int m_nCell;
  int m_cellID[CELLMAX];
  float m_cellNoiseRMS[CELLMAX];
  float m_cellEta[CELLMAX];
  float m_cellPhi[CELLMAX];
  float m_cellX[CELLMAX];
  float m_cellY[CELLMAX];
  float m_cellZ[CELLMAX];
  float m_cellR[CELLMAX];
  float m_celldEta[CELLMAX];
  float m_celldPhi[CELLMAX];
  float m_celldZ[CELLMAX];
  float m_celldR[CELLMAX];
  float m_cellVolume[CELLMAX];
  int m_celliEta[CELLMAX];
  int m_celliPhi[CELLMAX];
  int m_cellRegion[CELLMAX];
  int m_cellCalo[CELLMAX];
  int m_cellSampling[CELLMAX];

  ClassDef(TBH6RunHeader,1)

};

#endif
