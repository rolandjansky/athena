/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOJET_T2CALOJETGRIDFROMFEBHEADER_H
#define TRIGT2CALOJET_T2CALOJETGRIDFROMFEBHEADER_H

/********************************************************************

NAME:     T2CaloJetGridFromFEBHeader.h
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
          Ignacio Aracena <ignacio.aracena@cern.ch>
CREATED:  November, 2005

PURPOSE: Data preparation from FEB Headers (Ex/Ey)

********************************************************************/

#include "TrigT2CaloJet/T2CaloJetBaseTool.h"
#include "GaudiKernel/AlgTool.h"
//#include "LArRecEvent/LArFebEnergyIDC.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/MsgStream.h"

class TrigT2Jet;
class Trig3Momentum;
class ITrigDataAccess;

class T2CaloJetGridFromFEBHeader : public T2CaloJetBaseTool
{
 public:

  T2CaloJetGridFromFEBHeader(const std::string& type, const std::string& name, 
		    const IInterface* parent);
  
  ~T2CaloJetGridFromFEBHeader();
  
  virtual StatusCode initialize();
  using T2CaloJetBaseTool::execute;
  virtual StatusCode execute();
  virtual StatusCode finalize();

  virtual TrigTimer* getTimer(const std::string& name);

  virtual StatusCode execute(TrigT2Jet* jet,double etamin, double etamax, double phimin, double phimax);
  
  StatusCode addFebEnergies(double etamin, double etamax,
                         double phimin, double phimax,
                         std::vector<Trig3Momentum>* grid);

  StatusCode addLArFebEnergy(double etamin, double etamax,
                          double phimin, double phimax,
                          std::vector<Trig3Momentum>* grid,
                          DETID detectorID, int sampling,
                          bool prepare);

  StatusCode addTileCells(double etamin, double etamax,
                          double phimin, double phimax,
                          std::vector<Trig3Momentum>* grid,
                          DETID detectorID, int sampling,
                          bool prepare);
 private:
  //   ITrigDataAccess *m_data;
   Trig3Momentum* m_gridElement;
   std::vector<int>* m_usedfeb;
   std::map<std::string, TrigTimer*> m_timers;

protected:
   ITrigTimerSvc* m_timerSvc;
   bool m_doTiming;

  // New way with TrigDataAccess doing BS conversion
  // iterators to LArFEBEnergy. Note the different type from the above
  LArFebEnergyCollection::const_iterator m_iFebBegin;
  LArFebEnergyCollection::const_iterator m_iFebEnd;
  LArFebEnergyCollection::const_iterator m_Febit;

  TileCellCollection::const_iterator m_itBegin;
  TileCellCollection::const_iterator m_itEnd;
  TileCellCollection::const_iterator m_itt;

};

#endif // TRIGT2CALOJET_T2CALOJETGRIDFROMFEBHEADER
