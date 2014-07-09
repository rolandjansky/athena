/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLHLTFrameLoader
#define TrigConf_XMLHLTFrameLoader

#include "TrigConfStorage/IHLTFrameLoader.h"
#include "TrigConfStorage/XMLLoader.h"

#include <string>
#include <vector>

namespace TrigConf {
   class XMLStorageMgr;
   class HLTFrame;

   /**@brief TriggerDB loader of the HLT menu*/
   class XMLHLTFrameLoader : virtual public IHLTFrameLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param hltDoc pointer to DOM document containing the hlt menu
       */
      XMLHLTFrameLoader( XMLStorageMgr& sm, const ptree::value_type* pt ) :
         XMLLoader("XMLHLTFrameLoader", sm, pt)
      {};

      /**@brief destructor*/
      virtual ~XMLHLTFrameLoader( void ) {};

      virtual bool load( HLTFrame& frame);

   private:
      /**unique identifier for trigger element*/
      int         stringToId  ( const std::string& );
      std::string stringFromId( int );

      void buildHLTChainList   ( HLTFrame& );
      void buildHLTSequenceList( HLTFrame& );

      // unique identifier for trigger element
      // this is unnecessary for DB configuration, where each TE has a DB id
      std::vector<std::pair<int,std::string> > m_idstrTable;

   };
}

#endif
