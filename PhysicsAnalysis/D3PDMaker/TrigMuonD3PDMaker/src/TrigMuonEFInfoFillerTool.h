// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoFillerTool.h 332700 2010-11-17 13:41:01Z kanno $
#ifndef TRIGMUOND3PDMAKER_TRIGMUONEFINFOFILLERTOOL_H
#define TRIGMUOND3PDMAKER_TRIGMUONEFINFOFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFInfo.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool filling information about TrigMuonEFInfo objects
    *
    *         This tool only needs to save the information that is stored directly
    *         in the TrigMuonEFInfo objects. The information stored in the objects
    *         pointed to by this object is saved using other tools.
    *
    * @author Takayuki Kanno
    *
    * $Revision: 332700 $
    * $Date: 2010-11-17 14:41:01 +0100 (Wed, 17 Nov 2010) $
    */
   class TrigMuonEFInfoFillerTool : public BlockFillerTool< TrigMuonEFInfo > {

   public:
      /// Regular AlgTool constructor
      TrigMuonEFInfoFillerTool( const std::string& type, const std::string& name,
                                const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TrigMuonEFInfo& muon );

   private:
      int* m_roINum;
      int* m_nSegments;
      int* m_nMdtHits;
      int* m_nRpcHits;
      int* m_nTgcHits;
      int* m_nCscHits;
      float* m_etaPreviousLevel;
      float* m_phiPreviousLevel;

   }; // class TrigMuonEFInfoFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TRIGMUONEFINFOFILLERTOOL_H
