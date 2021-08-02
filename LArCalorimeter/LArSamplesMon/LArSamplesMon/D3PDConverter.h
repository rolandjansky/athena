/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::D3PDConverter
   @brief storage of the time histories of all the cells
 */

#ifndef LArSamples_D3PDConverter_H
#define LArSamples_D3PDConverter_H

#include "LArSamplesMon/CaloD3PDClass.h"
#include "LArSamplesMon/Interface.h"
#include "TString.h"

#include <map>

namespace LArSamples {

  class D3PDConverter : public CaloD3PDClass
  {

    public:

      /** @brief Constructor  */
      D3PDConverter(TTree& tree, const TString& templateFile, const TString& translatorFile)
       : CaloD3PDClass(&tree) { initMapping(templateFile, translatorFile); }
       
      virtual ~D3PDConverter() { }

      bool makeSamplesTuple(const TString& outputFileName);

      bool initMapping(const TString& templateFile, const TString& translatorFile);
      
      std::map<unsigned int, unsigned int> m_id2hash;
      Interface* m_template;
  };
}

#endif
