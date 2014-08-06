// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CombLinksFillerTool.h 449429 2011-07-19 10:23:35Z tkohno $
#ifndef TRIGGERD3PDMAKER_COMBLINKSFILLERTOOL_H
#define TRIGGERD3PDMAKER_COMBLINKSFILLERTOOL_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "TriggerMenuNtuple/CombLinks.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool saving information on which feature belongs to a given RoI
    *
    *         This tool is used to save which HLT feature objects belong to
    *         a given RoI. The information is saved as indices, which can be
    *         used to look up the correct feature from the appropriate
    *         ntuple branches.
    *
    * @author Takanori Kohno
    *
    * $Revision: 449429 $
    * $Date: 2011-07-19 12:23:35 +0200 (Tue, 19 Jul 2011) $
    */
   class CombLinksFillerTool : public BlockFillerTool< CombLinks > {

      /// Type of the indices:
      typedef int index_t;

   public:
      /// Regular AlgTool constructor
      CombLinksFillerTool( const std::string& type, const std::string& name, 
                           const IInterface* parent );

      /// Function creating the D3PD variables
      StatusCode book();
      /// Function filling the D3PD variables
      StatusCode fill( const CombLinks& x );
    
   private:
      /// Type of the RoI that we're looking at:
      int m_roiType;

      short* m_type; ///< Type of this particular RoI
      short* m_active; ///< RoI active or not at the end of all selection steps
      short* m_lastStep; ///< Last step that was passed by this RoI
      short* m_TENumber; ///< Location of the TE in the configuration
      short* m_RoINumber; ///< RoI number inside the TE
      std::map< int, index_t* > m_featureIndex; ///< Indices of single objects
      std::map< int, index_t* > m_featureStatus; ///< Statuses of single objects
      std::map< int, std::vector< index_t >* > m_featureIndexVec; ///< Indices of collections
      std::map< int, std::vector< index_t >* > m_featureStatusVec; ///< Statuses of collections

   }; // class CombLinksFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_COMBLINKSFILLERTOOL_H
