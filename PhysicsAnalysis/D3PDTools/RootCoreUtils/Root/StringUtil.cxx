//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <RootCoreUtils/StringUtil.h>

#include <TRegexp.h>
#include <TString.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace RCU
{
  std::string substitute (const std::string& str, const std::string& pattern,
			  const std::string& with)
  {
    RCU_REQUIRE (!pattern.empty());

    std::string result = str;
    std::string::size_type pos;
    while ((pos = result.find (pattern)) != std::string::npos)
      result = result.substr (0, pos) + with + result.substr (pos + pattern.size());
    return result;
  }



  bool match_expr (const TRegexp& expr, const std::string& str)
  {
    Ssiz_t len = 0;
    TString mystr (str);
    return expr.Index (mystr, &len) == 0 && len == mystr.Length();
  }



  TString glob_to_regexp (const std::string& glob)
  {
    TString result;

    for (std::string::const_iterator iter = glob.begin(),
	   end = glob.end(); iter != end; ++ iter)
    {
      if (*iter == '*')
      {
	result += ".*";
      } else if (*iter == '?')
      {
	result += ".";
      } else if (*iter == '^' || *iter == '$' || *iter == '+' || *iter == '.')
      {
	result += '\\';
	result += *iter;
      } else
      {
	result += *iter;
      }
    }
    return result;
  }
}
