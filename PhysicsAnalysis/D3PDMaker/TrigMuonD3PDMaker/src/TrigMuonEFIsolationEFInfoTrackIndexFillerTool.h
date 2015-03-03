// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFIsolationEFInfoTrackIndexFillerTool.h 493591 2012-04-02 12:06:17Z kanno $
#ifndef TRIGMUOND3PDMAKER_TRIGMUONEFISOLATIONEFINFOTRACKINDEXFILLERTOOL_H
#define TRIGMUOND3PDMAKER_TRIGMUONEFISOLATIONEFINFOTRACKINDEXFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFIsolation.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"


namespace D3PD {

   /**
    * @short Tool filling the index of TrigMuonEFInfoTrack inside the TrigMuonEFInfo
    *        which is associated to TrigMuonEFIsolation objects
    *
    *         The problem is that the D3PD common tool "IndexAssociation" doesn't simply
    *        work for TrigMuonEFInfoTrack since it is contained in the TrigMuonEFInfo
    *        and doesn't have "GetterTool" for it self. On the other hand, the index is
    *        easily calculatable. This tool provide calculated index and fill it ot D3PD
    *         Note that the index of TrigMuonEFInfo is filled using IndexAssociation.
    *        Remember to activate also filling the index of TrigMuonEFInfo, otherwise
    *        have no meaning...
    *
    * @author Takayuki Kanno
    *
    * $Revision: 493591 $
    * $Date: 2012-04-02 14:06:17 +0200 (Mon, 02 Apr 2012) $
    */
   class TrigMuonEFIsolationEFInfoTrackIndexFillerTool : public BlockFillerTool< TrigMuonEFIsolation > {

   public:
      /// Regular AlgTool constructor
      TrigMuonEFIsolationEFInfoTrackIndexFillerTool( const std::string& type, const std::string& name,
                                                     const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TrigMuonEFIsolation& muon );

   private:

      // Index of the muon in the TrigMuonEFInfoTrack container
      int*    m_index_infotrack;


   }; // class TrigMuonEFIsolationEFInfoTrackIndexFillerTool

} // namespace D3PD

#endif //TRIGMUOND3PDMAKER_TRIGMUONEFISOLATIONEFINFOTRACKINDEXFILLERTOOL_H
