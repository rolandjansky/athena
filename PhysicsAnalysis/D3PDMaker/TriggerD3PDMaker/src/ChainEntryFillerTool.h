// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ChainEntryFillerTool.h 286304 2010-03-18 17:12:01Z krasznaa $
#ifndef TRIGGERD3PDMAKER_CHAINENTRYFILLERTOOL_H
#define TRIGGERD3PDMAKER_CHAINENTRYFILLERTOOL_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "TriggerMenuNtuple/ChainEntry.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool saving information about the chains from the navigation
    *
    *         This tool is used to save which RoIs belong to a selected set of
    *         trigger chains. The ChainEntry objects are created using the
    *         TriggerMenuAnalysis package before the D3PDMaker code is
    *         run.
    *
    * @author Takanori Kohno
    *
    * $Revision: 286304 $
    * $Date: 2010-03-18 18:12:01 +0100 (Thu, 18 Mar 2010) $
    */
   class ChainEntryFillerTool : public BlockFillerTool< ChainEntry > {

      /// Type of the chain_id variable
      typedef short chainId_t;
      /// Type of the indices written out
      typedef std::vector< int > index_t;

   public:
      /// Regular AlgTool constructor
      ChainEntryFillerTool( const std::string& type, const std::string& name, 
                            const IInterface* parent );

      /// Function creating the D3PD variables
      StatusCode book();
      /// Function filling the D3PD variables
      StatusCode fill( const ChainEntry& x );

   private:
      chainId_t* m_chainId; ///< Variable identifying the chain
      index_t*   m_roiType; ///< Indices of the RoIs belonging to this chain
      index_t*   m_roiIndex; ///< Types of the RoIs belonging to this chain

   }; // class ChainEntryFillerTool

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_CHAINENTRYFILLERTOOL_H
