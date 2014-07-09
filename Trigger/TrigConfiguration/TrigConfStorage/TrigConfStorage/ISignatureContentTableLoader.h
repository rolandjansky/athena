/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_ISignatureContentTableLoader
#define TrigConf_ISignatureContentTableLoader

#include "TrigConfStorage/ILoader.h"

namespace TrigConf {
   class SignatureContentTable;

   /**
    * @brief Interface for loaders of the HLT signatures
    */
   class ISignatureContentTableLoader : virtual public ILoader {
   public:

      /**@brief destructor*/       
      virtual ~ISignatureContentTableLoader(){};

      /**
       * @brief Load the HLT signature definitions from the
       * configuration source
       *
       * @param signatureContent reference to the SignatureContentTable
       * object to be filled
       * 
       * @returns @c true if the loading was successful, @c false
       * otherwise
       */
      virtual bool load( SignatureContentTable& data ) = 0;
   };
}

#endif
