//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <PATInterfaces/SystematicVariation.h>

#include <cmath>
#include <sstream>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace CP
{
  bool operator < (const SystematicVariation& a, const SystematicVariation& b)
  {
    return a.name() < b.name();
  }



  bool operator == (const SystematicVariation& a, const SystematicVariation& b)
  {
    return a.name() == b.name();
  }



  void SystematicVariation :: 
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  SystematicVariation :: 
  SystematicVariation ()
  {
    RCU_NEW_INVARIANT (this);
  }



  SystematicVariation ::
  SystematicVariation (const std::string& val_name)
    : m_name (val_name)
  {
    RCU_NEW_INVARIANT (this);
  }



  SystematicVariation ::
  SystematicVariation (const std::string& val_basename,
		       const std::string& val_subvariation)
    : m_name (val_basename + "__" + val_subvariation)
  {
    RCU_NEW_INVARIANT (this);
  }



  SystematicVariation ::
  SystematicVariation (const std::string& val_basename,
		       float val_parameter)
  {
    std::string separator = "up";
    if (val_parameter < 0)
    {
      separator = "down";
      val_parameter *= -1;
    }
    std::ostringstream str;
    str << rint (val_parameter * 10000);
    std::string number = str.str();

    while (number.size() < 5)
      number = "0" + number;

    /// rationale: in case the parameter is ~0 we initialize this to
    ///   an empty systematic, since we are essentially not requesting
    ///   anything
    if (number != "00000")
    {
      m_name = val_basename + "__" + number.substr (0, number.size()-4)
	+ separator + number.substr (number.size()-4);
      while (m_name[m_name.size()-1] == '0')
	m_name = m_name.substr (0, m_name.size()-1);
    }

    RCU_NEW_INVARIANT (this);
  }



  SystematicVariation ::
  SystematicVariation (const std::string& val_basename, CONTINUOUS_ARG)
    : m_name (val_basename + "__continuous")
  {
    RCU_NEW_INVARIANT (this);
  }



  SystematicVariation ::
  SystematicVariation (const std::set<SystematicVariation>& systematics,
		       std::string val_basename)
  {
    RCU_NEW_INVARIANT (this);

    for (std::set<SystematicVariation>::const_iterator
	   sys = systematics.begin(),
	   end = systematics.end(); sys != end; ++ sys)
    {
      if (sys->basename() == val_basename)
      {
	*this = *sys;
	return;
      }
    }
  }



  bool SystematicVariation ::
  empty () const
  {
    RCU_READ_INVARIANT (this);
    return m_name.empty();
  }



  const std::string& SystematicVariation ::
  name () const
  {
    RCU_READ_INVARIANT (this);
    return m_name;
  }



  std::string SystematicVariation ::
  basename () const
  {
    RCU_READ_INVARIANT (this);

    std::string::size_type split = m_name.rfind ("__");
    if (split != std::string::npos)
      return m_name.substr (0, split);
    else
      return m_name;
  }



  std::string SystematicVariation ::
  subvariation () const
  {
    RCU_READ_INVARIANT (this);

    std::string::size_type split = m_name.rfind ("__");
    if (split != std::string::npos)
      return m_name.substr (split + 2);
    else
      return "";
  }



  float SystematicVariation ::
  parameter () const
  {
    RCU_READ_INVARIANT (this);

    std::string subvariation = this->subvariation();

    std::string::size_type split = subvariation.rfind ("up");
    if (split != std::string::npos)
    {
      std::istringstream str (subvariation.substr (0, split) + "." + subvariation.substr (split + 2));
      float result;
      if (str >> result)
	return result;
      else
	return 0;
    }

    split = subvariation.rfind ("down");
    if (split != std::string::npos)
    {
      std::istringstream str (subvariation.substr (0, split) + "." + subvariation.substr (split + 4));
      float result;
      if (str >> result)
	return -result;
      else
	return 0;
    }

    return 0;
  }
}
