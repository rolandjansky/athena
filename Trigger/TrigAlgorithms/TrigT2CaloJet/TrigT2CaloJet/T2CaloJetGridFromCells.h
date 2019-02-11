/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOJET_T2CALOJETGRIDFROMCELLS_H
#define TRIGT2CALOJET_T2CALOJETGRIDFROMCELLS_H

/********************************************************************

NAME:     T2CaloJetGridFromCells.h
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
CREATED:  November, 2005

PURPOSE: Data preparation from cells, uses the
         TrigT2CaloCommon/CommonAccessTool

********************************************************************/

#include "TrigT2CaloJet/T2CaloJetBaseTool.h"
#include "GaudiKernel/AlgTool.h"

//#include "LArRecEvent/LArCellCollection.h"
//#include "TileEvent/TileCellIDC.h"
//#include "LArRecEvent/LArCellIDC.h"
//#include "LArRawUtils/LArTT_Selector.h"
//#include "TrigT2CaloCommon/LArCellCont.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

class TrigT2Jet;
class Trig3Momentum;
class ITrigDataAccess;
class EtaPhiRectangle;

class T2CaloJetGridFromCells : public T2CaloJetBaseTool
{
 public:
  T2CaloJetGridFromCells(const std::string& type, const std::string& name, 
		    const IInterface* parent);
  
  ~T2CaloJetGridFromCells();
  
  virtual StatusCode initialize();
  using T2CaloJetBaseTool::execute;
  virtual StatusCode execute();
  virtual StatusCode finalize();

//  virtual StatusCode execute(TrigT2Jet* jet,double etamin, double etamax, 
//			     double phimin, double phimax);
  virtual StatusCode execute(TrigT2Jet * jet,const IRoiDescriptor& roi  );

  virtual TrigTimer* getTimer(const std::string& name);
  
  StatusCode addAllCells(double etamin, double etamax, 
			 double phimin, double phimax,
			 std::vector<Trig3Momentum>* grid);
  

  StatusCode addLArCells(double etamin, double etamax, 
			 double phimin, double phimax,
			 std::vector<Trig3Momentum>* grid,
			 DETID detectorID, int sampling,
			 bool prepare);

  StatusCode addTileCells(double etamin, double etamax, 
			 double phimin, double phimax,
			 std::vector<Trig3Momentum>* grid,
			 DETID detectorID, int sampling,
			 bool prepare);
 //! set the regions to be skipped
 void setForbiddenRegions(const std::vector< EtaPhiRectangle > &regions) { m_forbiddenRegions = regions; } ;
 private:
  bool isInVetoRegion(const double &eta, const double &phi);
 private:
  Trig3Momentum* m_gridElement;


  // New way with TrigDataAccess doing BS conversion
  // iterators to LArCells. Note the different type from the above
  LArTT_Selector<LArCellCont>::const_iterator m_iBegin;
  LArTT_Selector<LArCellCont>::const_iterator m_iEnd;
  LArTT_Selector<LArCellCont>::const_iterator m_it;
  
  TileCellCollection::const_iterator m_itBegin;
  TileCellCollection::const_iterator m_itEnd;
  TileCellCollection::const_iterator m_itt;



  //int ncell_roi;
  //int nnegcell_roi;
  //double eem_roi;
  //double etot_roi;

  bool m_applyNoiseCut;
  double m_noiseCutValue;
  // cell based jet cleaning:
  bool m_doHECcellQualityCleaning;
  int m_cellQualityThresholdHEC;

  ToolHandle<LArCablingLegacyService> m_cablingSvc;
  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;
  std::vector< EtaPhiRectangle > m_forbiddenRegions; //!< regions to be skipped

 protected:

  ITrigTimerSvc* m_timerSvc;
  /** Timer Items for the main algorithms. */
  std::map<std::string, TrigTimer*> m_timers;

  bool m_doTiming;
};

#endif // TRIGT2CALOJET_T2CALOJETGRIDFROMCELLS
