// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     IAlgToolEFCalo.h
// PACKAGE:  TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   C. Santamarina (based on M.P. Casado IAlgToolCalo.h)
//
// ********************************************************************

#ifndef TRIGCALOREC_IALGTOOLEFCALO_H
#define TRIGCALOREC_IALGTOOLEFCALO_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ToolHandle.h" 
#include "GaudiKernel/ServiceHandle.h" 
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "CaloEvent/CaloCell.h"

#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/IRoiDescriptor.h"

class CaloCellContainer;

static const InterfaceID IID_IAlgToolEFCalo("IAlgToolEFCalo",1,0);

class IAlgToolEFCalo: public virtual IAlgTool, public AthAlgTool {

 public:

  enum DETID_EF {
    EFTTEM=0, EFTTHEC, EFTILE, EFFCALEM, EFFCALHAD, EFFULLCALO, MAXNEFCALO
  };  

  
public:
  IAlgToolEFCalo(const std::string & type, const std::string & name, const IInterface* parent) : AthAlgTool(type,name,parent){
    declareInterface<IAlgToolEFCalo>(this);
  }
  
  virtual ~IAlgToolEFCalo() { }
  static const InterfaceID& interfaceID();
  inline virtual StatusCode execute(CaloCellContainer &/*pCaloCellContainer*/, 
				    const IRoiDescriptor& /* roi */ ) {return StatusCode::FAILURE;}

  inline virtual StatusCode execute(CaloCellContainer &/*pCaloCellContainer*/ ) {return StatusCode::FAILURE;}
  inline virtual std::vector<TrigTimer*> getTimers() const { return m_timer;}

  inline bool CellInTheRoi(CaloCell* thecell, const IRoiDescriptor& roi ) { 
    /// just a straight copy for the time being
    if (thecell->eta()<roi.etaMinus()) return false;
    if (thecell->eta()>roi.etaPlus()) return false;
    /// normal rois
    
    if (roi.phiPlus() < roi.phiMinus())
      { if (! (thecell->phi()>roi.phiMinus() || thecell->phi()<roi.phiPlus())) return false; else return true; }
    else
      { if (! (thecell->phi()>roi.phiMinus() && thecell->phi()<roi.phiPlus())) return false; else return true; }

    /// these can never be called since the roi wraps to -pi to pi 
    ///    if (phimin<-M_PI && !(thecell->phi()<phimax || thecell->phi()>phimin+2*M_PI)) return false;
    ///    if (phimax>+M_PI && !(thecell->phi()>phimin || thecell->phi()<phimax-2*M_PI)) return false;
    return true;
  }
  
  uint32_t report_error() { return m_error; }
  


  /// obsolete methods DO NOT USE !!!
  inline virtual StatusCode execute(CaloCellContainer &/*pCaloCellContainer*/, double /*etamin*/, double /*etamax*/,
				    double /*phimin*/, double /*phimax*/) {return StatusCode::FAILURE;}

  inline bool CellInTheRoi(CaloCell* thecell, double etamin, double etamax, double phimin, double phimax ) {
    if (thecell->eta()<etamin) return false;
    if (thecell->eta()>etamax) return false;
    if (phimin>-M_PI && phimax<M_PI && (thecell->phi()<phimin || thecell->phi()>phimax)) return false;
    if (phimin<-M_PI && !(thecell->phi()<phimax || thecell->phi()>phimin+2*M_PI)) return false;
    if (phimax>+M_PI && !(thecell->phi()>phimin || thecell->phi()<phimax-2*M_PI)) return false;
    return true;
  }


protected:

  // Timing
  ITrigTimerSvc* m_timersvc;
  /** Pointers to timers */
  std::vector<TrigTimer*> m_timer;
  uint32_t m_error;
  
private:
};

inline const InterfaceID& IAlgToolEFCalo::interfaceID() { 
           return IID_IAlgToolEFCalo; 
} 


#endif
