/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkCollectionAliasAlg.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKTRKCOLLECTIONALIASALG_H
#define TRKTRKCOLLECTIONALIASALG_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

class AtlasDetectorID;
class Identifier;

namespace Trk 
{

  /** @class TrkCollectionAliasAlg

      This algorithm creates in StoreGate an alias name for
       the StoreGate container key given as property
      
      @author  Martin Siebel <Martin.Siebel@CERN.ch>
  */  

  class TrkCollectionAliasAlg : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       TrkCollectionAliasAlg(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~TrkCollectionAliasAlg();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:
      
      /** member variables for algorithm properties: */

      // The key of the input track collection
      std::string  m_collectionName;

      // The key that should be given to the alias 
      std::string  m_aliasName;
      
    }; 
} // end of namespace

#endif 
