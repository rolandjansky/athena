// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TgcCoinHierarchyCoinDataAssociationTool.h 343690 2011-02-03 14:44:47Z kanno $
#ifndef TRIGMUOND3PDMAKER_TGCCOINHIERARCHYCOINDATAASSOCIATIONTOOL_H
#define TRIGMUOND3PDMAKER_TGCCOINHIERARCHYCOINDATAASSOCIATIONTOOL_H

// D3PD include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace Muon{
   class TgcCoinData;
}

namespace Trigger{
   class TgcCoinHierarchy;
}

namespace D3PD {

   /**
    *  @short Tool "associating" TgcCoinHierarchy with TgcCoinData
    *
    *         TgcCoinHierarchy holds pointers of TgcCoinData (and TgcPrepData)
    *         which formed coincidence. This tool simply return the pointer
    *         according to the type of coincidence. The type to be returned
    *         can be set by the properties AssocSL, AssocHpt, AssocStrip (see below)
    *
    * @author Takayuki Kanno <takayuki.kanno@cern.ch>
    *
    * $Revision: 343690 $
    * $Date: 2011-02-03 15:44:47 +0100 (Thu, 03 Feb 2011) $
    */
   class TgcCoinHierarchyCoinDataAssociationTool:
      public SingleAssociationTool< Trigger::TgcCoinHierarchy, Muon::TgcCoinData> {

   public:
      /// Regular AlgTool constructor
      TgcCoinHierarchyCoinDataAssociationTool( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent );

      /// Regular AlgTool initializer
      virtual StatusCode initialize();

      /// Function performing the association
      virtual const Muon::TgcCoinData* get( const Trigger::TgcCoinHierarchy& hierarchy );

   private:
      typedef SingleAssociationTool< Trigger::TgcCoinHierarchy, Muon::TgcCoinData> BASE;

      /*
       * Flags to set target
       * |    Property               | Target to be stored    |
       * |AssocSL|AssocHpt|AssocStrip|                        |
       * +-------+--------+----------+--------------------------+
       * | true  | any    |  any     | SL                     |
       * | false | true   | T or F   | Hpt Strip or Wire      |
       * | false | false  | T or F   | Tracklet Strip or Wire |
       */
      bool m_assocSL;
      bool m_assocHpt;
      bool m_assocStrip;
      Muon::TgcCoinData::CoinDataType m_TargetType;


   }; // class TgcCoinHierarchyCoinDataAssociationTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TGCCOINHIERARCHYCOINDATAASSOCIATIONTOOL_H
