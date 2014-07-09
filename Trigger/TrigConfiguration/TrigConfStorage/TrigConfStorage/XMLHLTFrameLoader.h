/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//
// NAME:     XMLHLTFrameLoader.h
// PACKAGE:  TrigConfStorage
//
// AUTHORs:  Johannes Haller (CERN) [Johannes.Haller@cern.ch]
//           Andreas Hoecker (CERN) [Andreas.Hocker@cern.ch]
//
// CREATED:  10-Jan.-2006
//
// PURPOSE:
//
//////////////////////////////////////////////////////////////////////

#ifndef TRIGCONFXMLHLTFRAMELOADER
#define TRIGCONFXMLHLTFRAMELOADER

#include "TrigConfStorage/IHLTFrameLoader.h"
#include "TrigConfStorage/XMLLoader.h"
#include <string>
#include <vector>

namespace TrigConf {

   class IStorageMgr;
   class HLTFrame;

   /**@brief TriggerDB loader of the HLT menu*/
   class XMLHLTFrameLoader : virtual public IHLTFrameLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param hltDoc pointer to DOM document containing the hlt menu
       */
      XMLHLTFrameLoader( XMLStorageMgr& sm, value_type* pt )
         : IHLTFrameLoader(), XMLLoader(sm, pt)
      {};

      /**@brief destructor*/
      virtual ~XMLHLTFrameLoader( void ) {};

      virtual bool load( HLTFrame& frame);


   private:
      /**unique identifier for trigger element*/
      int    stringToId  ( std::string );
      std::string stringFromId( int    );

      bool buildHLTChainList   (  HLTFrame& );
      bool buildHLTSequenceList(  HLTFrame& );


      void parseItemLine( std::string theString, std::string theSeparator, std::vector<std::string>& list );

      // unique identifier for trigger element
      // this is unnecessary for DB configuration, where each TE has a DB id
      std::vector<std::pair<int,std::string> > m_idstrTable;

   };
}

#endif
