/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT
 * @Package: TrigMissingETMuon
 * @Class  : TrigMissingETMuon
 *
 * @brief  Algorithm that add muon contribution to the TrigMissingET object using muons
 *
 * @author Hongtao Yang     <Hongtao.Yang@cern.ch>     - UW Madsion, USA
 *
 * File and Version Information:
 * $Id: TrigMissingETMuon.h,v 0.01 2012-03-05 09:00:20 yanght Exp $
 **********************************************************************************/



#ifndef TrigMissingETMuon_MissingETMuon_H
#define TrigMissingETMuon_MissingETMuon_H


#include "TrigInterfaces/AllTEAlgo.h"
#include <vector>

#include "xAODTrigMissingET/TrigMissingET.h"

namespace HLT {
  class TriggerElement;
}

// class xAOD::TrigMissingET;


  /**
      $class TrigMissingETMuon
      This class add MET object with muon components.
      Please note that the muon contributions are NOT subtracted from total MET.
      It is supposed to be done in the hypo algorithm.
      No calorimeter energy deposit correction is performed for the
      muon, because it is assumed that the muon Pt does not include
      a correction for calorimeter energy.

      The first defined trigger element must be the TrigMissingET object

      @author Hongtao Yang     <Hongtao.Yang@cern.ch>
  */
class TrigMissingETMuon : public HLT::AllTEAlgo
{
 public:

  TrigMissingETMuon(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi Algorithm constructor

  HLT::ErrorCode hltInitialize() { return HLT::OK; } //!< hlt initialize, doing nothing here
  HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize, doing nothing here

  virtual HLT::ErrorCode hltBeginRun();

  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input, unsigned int output);
  HLT::ErrorCode makeOutputTE(std::vector<std::vector<HLT::TriggerElement*> >& input, unsigned int type_out ) ;

  HLT::ErrorCode hltEndEvent() { m_useCachedResult = false; m_met = 0; m_cachedTE=0; return HLT::OK; };
 
  // virtual bool reset() {m_useCachedResult = false; m_met = 0; m_cachedTE=0; AllTEAlgo::reset(); return true;}
 
 private:

  std::string m_featureLabel; //!< label for the MET feature in the HLT Navigation

  StoreGateSvc* m_StoreGate; //!< pointer to StoreGate

  // the following are 32-bit unsigned values:
  unsigned int m_current_run_id;  //!< run number
  unsigned int m_current_lbk_id;  //!< luminosity block
  unsigned int m_current_evt_id;  //!< event number
  unsigned int m_current_bcg_id;  //!< bunch crossing

  bool m_useCachedResult;  //!< internal caching: true when the hltExecute will run in cached mode

  float m_mu_ex; //!< Monitoring: Muon ETx [GeV]
  float m_mu_ey; //!< Monitoring: Muon ETy [GeV]
  float m_mu_set;   //!< Monitoring: Muon Et [GeV]

  float m_mu_ex_log; //!< Monitoring: Muon ETx [GeV] log-scale
  float m_mu_ey_log; //!< Monitoring: Muon ETy [GeV] log-scale
  float m_mu_set_log; //!< Monitoring: Muon Et [GeV] log-scale

  xAOD::TrigMissingET* m_met;    //!< internal caching: mnissing E_T feature of the first execution
  HLT::TriggerElement* m_cachedTE; //!< internal caching: output TE from the first exectution

  /** definition of the meaning for the component flag bits
   ** (see the constructor)
   **/

  unsigned short m_maskErrMuon;      //!< Unexpected no. of input muons
  unsigned short m_maskCompErrors;   //!< Error detected

  float m_muonptcut;
/*   unsigned m_maskGlobErrors;            //!< Error detected */



};


#endif
