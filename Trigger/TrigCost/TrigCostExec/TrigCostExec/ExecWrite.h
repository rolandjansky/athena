/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_EXECWRITE_H
#define ANP_EXECWRITE_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : ExecWrite
 * @Author : Doug Schaefer
 *
 * @Brief  : 
 *
 * Class for writing count summary tables
 *
 **********************************************************************************/

// C/C++
#include <map>
#include <string>

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/Registry.h"

// Local
#include "TrigCostExec/ExecData.h"
#include "TrigCostExec/ExecBase.h"

class TDirectory;
class TH1;
class TH2;

namespace Anp
{

  namespace Exec {

    void WriteROSTable(HtmlNode &rnode, const ExecClient &alg, const Summary &sum, const double scale, const std::string type);
    void WriteDETTable(HtmlNode &anode, const ExecClient &alg, const Summary &sum, const double scale);
    void WriteALGTable(HtmlNode &anode, const ExecClient &ros, const Summary &sum, const double scale);
    void WriteALGBYCHNTable(HtmlNode &anode, const ExecClient &ros, const Summary &sum, const double scale, const double timeout, const unsigned level);
    void WriteCHNBYROSTable(HtmlNode &anode, const ExecClient &ros, const Exec::Summary &sum, const double scale, const std::string type);
  }

}
#endif
