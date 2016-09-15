// Hi Emacs ! this is  -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:      TrigFullCaloCellMaker
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR:    B. Brellier, C. Santamarina
 * CREATED:   April 2008
 *
 *********************************************************************/
#ifndef TRIGCALOREC_TRIGFULLCALOCELLMAKER_H
#define TRIGCALOREC_TRIGFULLCALOCELLMAKER_H

#include "TrigInterfaces/AllTEAlgo.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"


class IAlgToolEFCalo;
class ICaloCellMakerTool; 


class TrigFullCaloCellMaker : public HLT::AllTEAlgo {

 public:

  /**  constructor */
  TrigFullCaloCellMaker(const std::string& name, ISvcLocator* pSvcLocator);

  /** destructor */
  ~TrigFullCaloCellMaker();

  /** HLT method to initialize */
  HLT::ErrorCode hltInitialize();

  /** HLT method to finalize */
  HLT::ErrorCode hltFinalize();

  /** HLT method to execute FEX algo on a given TE. 
   input is the Triger Element (TE) from the previous algorithm in
   the configuration chain, the output is the result from the
   TrigCaloCellMaker execution. **/

  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
			      unsigned int type_out);

    /** 
     * @brief This method overwrites the default one (doing nothing) in the algo.h class
     *
     * This is used to reset the internal caching mechanism of this T2MissingET algorithm.
     */
  HLT::ErrorCode hltEndEvent() {  m_useCachedResult = false; m_cachedTE=0; return HLT::OK; }
  //virtual bool reset() {  m_useCachedResult = false; m_cachedTE=0; return true; }

 private:

  /** Calorimeter second sampling eta granularity. */
  static const double m_detas2;

  /** Calorimeter second sampling phi granularity. */
  static const double m_dphis2;

  /** RoI window size in units of EM calo 2nd sampling cells */
  int m_neta, m_nphi;

  /** EF Calorimeter tools for calorimeter data unpacking. */
  ToolHandleArray<IAlgToolEFCalo>     m_tcrAlgTools;

  /** Offline Tool names for CellContainer ordering and checking. */
  std::vector<std::string> m_containerToolNames;

  /** Offline Tool pointers for CellContainer ordering and checking. */
  //ToolHandleArray<ICaloCellMakerTool> m_containerTools; // pointers to tools
  std::vector<ICaloCellMakerTool*> m_containerTools; // pointers to tools

  /** Pointers to timers */
  std::vector<TrigTimer*> m_timer;

  /** Produced Calorimeter Cell Container. */
  CaloCellContainer* m_pCaloCellContainer;

  /** Cell Container size (for monitoring purpouses). */
  double m_CellContainerSize;

  bool m_useCachedResult;  //!< internal caching: true when the hltExecute will run in cached mode

  HLT::TriggerElement* m_cachedTE; //!< internal caching: output TE from the first exectution

  unsigned int m_conversionError;

//public:
//
//  inline CaloCellContainer* GetCellContainer() const {return m_pCaloCellContainer;}

};
#endif
