/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_BSelectionParser_H
#define DERIVATIONFRAMEWORK_BSelectionParser_H

#include <assert.h>
#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include <map>
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

namespace DerivationFramework {

  /** selection expression parser
   */
  class BSelectionParser {
    public:
      /** constructor
       */
      BSelectionParser(std::string expr, std::string offset = "");

      /** evaluate
       */
      bool eval(ServiceHandle<StoreGateSvc>& evtStore);
      
      /** destructor
       */
      ~BSelectionParser();
      
    private:
      /** replaces outermost matching braces with placeholder string
       *  and stores the replacements in a map
       */
      void replaceBraces(std::string& str, std::map<std::string, std::string>& tokenMap);
      
      /** replaces brances tokens back with the expressions
       */
      void putBackBraces(std::string& str, std::map<std::string, std::string>& tokenMap);
      
      /** enums
       */
      enum node_type { FINAL, OR, AND };  //!< type os the node operation
      enum comparison_type { EQ, GT };    //!< operation "==" or ">"
      
      /** private attributes
       */
      std::string m_expr;
      node_type   m_nodeType;
      std::string m_collection;
      std::string m_flag;
      comparison_type m_compType;
      int m_count;
      std::vector<BSelectionParser*> m_daughters;
      
  };


}

#endif
