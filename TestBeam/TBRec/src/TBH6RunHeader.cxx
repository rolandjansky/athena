/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class TBH6RunHeader // make header for TBH6 Root Tree
//
// author: A.Minaenko
// date:  28/07/2006

#include "TBRec/TBH6RunHeader.h"

ClassImp(TBH6RunHeader)

TBH6RunHeader::TBH6RunHeader(const int nRun):
  m_nRun(nRun),
  m_beamMomentum(0),
  m_xCryo(0),
  m_yTable(0),
  m_zCalo(0),
  m_nCell(0),
  m_cellID(),
  m_cellNoiseRMS(),
  m_cellEta(),
  m_cellPhi(),
  m_cellX(),
  m_cellY(),
  m_cellZ(),
  m_cellR(),
  m_celldEta(),
  m_celldPhi(),
  m_celldZ(),
  m_celldR(),
  m_cellVolume(),
  m_celliEta(),
  m_celliPhi(),
  m_cellRegion(),
  m_cellCalo(),
  m_cellSampling()
{}

TBH6RunHeader::~TBH6RunHeader()
{ }

int TBH6RunHeader::runNum() const {return m_nRun;}
float TBH6RunHeader::beamMomentum() const {return m_beamMomentum;}
float TBH6RunHeader::xCryo() const {return m_xCryo;}
float TBH6RunHeader::yTable() const {return m_yTable;}
float TBH6RunHeader::zCalo() const {return m_zCalo;}
int TBH6RunHeader::cellNum() const {return m_nCell;}
int const * TBH6RunHeader::cellID() const {return m_cellID;}
float const * TBH6RunHeader::cellNoiseRMS() const {return m_cellNoiseRMS;}
float const * TBH6RunHeader::cellEta() const {return m_cellEta;}
float const * TBH6RunHeader::cellPhi() const {return m_cellPhi;}
float const * TBH6RunHeader::cellX() const {return m_cellX;}
float const * TBH6RunHeader::cellY() const {return m_cellY;}
float const * TBH6RunHeader::cellZ() const {return m_cellZ;}
float const * TBH6RunHeader::cellR() const {return m_cellR;}
float const * TBH6RunHeader::celldEta() const {return m_celldEta;}
float const * TBH6RunHeader::celldPhi() const {return m_celldPhi;}
float const * TBH6RunHeader::celldZ() const {return m_celldZ;}
float const * TBH6RunHeader::celldR() const {return m_celldR;}
float const * TBH6RunHeader::cellVolume() const {return m_cellVolume;}
int const * TBH6RunHeader::celliEta() const {return m_celliEta;}
int const * TBH6RunHeader::celliPhi() const {return m_celliPhi;}
int const * TBH6RunHeader::cellRegion() const {return m_cellRegion;}
int const * TBH6RunHeader::cellCalo() const {return m_cellCalo;}
int const * TBH6RunHeader::cellSampling() const {return m_cellSampling;}
void TBH6RunHeader::SetRunNum(int run){m_nRun=run;}
void TBH6RunHeader::SetBeamMomentum(float mom){m_beamMomentum=mom;}
void TBH6RunHeader::SetXCryo(float x){m_xCryo=x;}
void TBH6RunHeader::SetYTable(float y){m_yTable=y;}
void TBH6RunHeader::SetZCalo(float z){m_zCalo=z;}
void TBH6RunHeader::SetCellNum(int nCell){m_nCell=nCell;}
void TBH6RunHeader::SetCellID(const int i, const int id){m_cellID[i]=id;}
void TBH6RunHeader::SetCellNoiseRMS(const int i, const float rms)
{m_cellNoiseRMS[i]=rms;}
void TBH6RunHeader::SetCellEta(const int i, const float eta){m_cellEta[i]=eta;}
void TBH6RunHeader::SetCellPhi(const int i, const float phi){m_cellPhi[i]=phi;}
void TBH6RunHeader::SetCellX(const int i, const float x){m_cellX[i]=x;}
void TBH6RunHeader::SetCellY(const int i, const float y){m_cellY[i]=y;}
void TBH6RunHeader::SetCellZ(const int i, const float z){m_cellZ[i]=z;}
void TBH6RunHeader::SetCellR(const int i, const float r){m_cellR[i]=r;}
void TBH6RunHeader::SetCelldEta(const int i, const float deta)
{m_celldEta[i]=deta;}
void TBH6RunHeader::SetCelldPhi(const int i, const float dphi)
{m_celldPhi[i]=dphi;}
void TBH6RunHeader::SetCelldZ(const int i, const float dz){m_celldZ[i]=dz;}
void TBH6RunHeader::SetCelldR(const int i, const float dr){m_celldR[i]=dr;}
void TBH6RunHeader::SetCellVolume(const int i, const float vol)
{m_cellVolume[i]=vol;}
void TBH6RunHeader::SetCelliEta(const int i, const int ieta)
{m_celliEta[i]=ieta;}
void TBH6RunHeader::SetCelliPhi(const int i, const int iphi)
{m_celliPhi[i]=iphi;}
void TBH6RunHeader::SetCellRegion(const int i, const int reg)
{m_cellRegion[i]=reg;}
void TBH6RunHeader::SetCellCalo(const int i, const int calo)
{m_cellCalo[i]=calo;}
void TBH6RunHeader::SetCellSampling(const int i, const int samp)
{m_cellSampling[i]=samp;}
