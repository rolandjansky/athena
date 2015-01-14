/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLMasterTableLoader
#define TrigConf_XMLMasterTableLoader

#include "TrigConfStorage/IMasterTableLoader.h"
#include "TrigConfStorage/XMLLoader.h"

namespace TrigConf {

   /**@brief TriggerDB loader of the LVL1 trigger configuration*/
   class XMLMasterTableLoader : public IMasterTableLoader, public XMLLoader {
   public:

      /**@brief constructor
       * @param sm reference to storage manager
       * @param node pointer to DOM node
       */
      XMLMasterTableLoader(XMLStorageMgr& sm, const ptree::value_type* pt) :
         IMasterTableLoader(), XMLLoader("XMLMasterTableLoader", sm, pt) {};

      /**@brief destructor*/
      ~XMLMasterTableLoader(){};

      bool load(ThresholdConfig& thrcfg);
      bool load(CTPConfig& ctpc);
      bool load(Muctpi& muctpi);
      bool load(TXC::L1TopoMenu& l1topo);
   };

}

#endif

