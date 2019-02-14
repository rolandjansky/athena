/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTools/MessagePrinterErrorCollect.h>

#include <regex>

//
// method implementations
//

#ifdef XAOD_STANDALONE

namespace asg
{
  void MessagePrinterErrorCollect ::
  print (MSG::Level reglvl, const std::string& /*name*/,
         const std::string& text)
  {
    if (reglvl >= MSG::ERROR)
    {
      m_errorMessages.push_back (text);
    }
  }



  bool MessagePrinterErrorCollect ::
  empty () const noexcept
  {
    return m_errorMessages.empty();
  }



  bool MessagePrinterErrorCollect ::
  matchesRegex (const std::string& pattern) const
  {
    std::regex regex (pattern);
    for (auto& msg : m_errorMessages)
    {
      if (std::regex_search (msg, regex))
        return true;
    }
    return false;
  }



  std::string MessagePrinterErrorCollect ::
  asString (const std::string& prefix) const
  {
    std::string result;
    for (auto& msg : m_errorMessages)
    {
      if (!result.empty())
        result += "\n";
      result += prefix + msg;
    }
    return result;
  }
}

#endif
