/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <PATInterfaces/SystematicVariation.h>

#include <cmath>
#include <boost/regex.hpp>
#include <sstream>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

bool operator < (const CP::SystematicVariation& a, const CP::SystematicVariation& b)
{
  return a.name() < b.name();
}


bool operator == (const CP::SystematicVariation& a, const CP::SystematicVariation& b)
{
  return a.name() == b.name();
}

//
// method implementations
//

namespace CP
{
  namespace
  {
    /// \brief pack the given value into a string
    /// \sa unpackUnsigned
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
    std::string packUnsigned (unsigned value)
    {
      std::ostringstream result;
      result << value;
      return result.str();
    }


    /// \brief unpack the value from a string
    /// \sa packUnsigned
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   parse errors
    unsigned unpackUnsigned (const std::string& value)
    {
      std::istringstream str (value);
      unsigned result = 0;
      if (!(str >> result))
	RCU_THROW_MSG ("failed to parse \"" + value + "\" into an unsigned");
      char c;
      if (str >> c)
	RCU_THROW_MSG ("failed to parse \"" + value + "\" into an unsigned");
      return result;
    }



    /// \brief pack the given value into a string
    /// \sa unpackFloat
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
    std::string packFloat (float value, const std::string& plus,
			   const std::string& minus)
    {
      std::string separator = plus;
      if (value < 0)
      {
	separator = minus;
	value *= -1;
      }
      RCU_ASSERT (!separator.empty());

      std::ostringstream str;
      str << rint (value * 10000);
      std::string number = str.str();

      while (number.size() < 5)
	number = "0" + number;
      return number.substr (0, number.size() - 4) + separator + number.substr (number.size() - 4);
    }



    /// \brief helper function for \ref unpackFloat
    ///
    /// this function unpacks one sign of the float
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   parse errors
    bool unpackFloatHelper (const std::string& value,
			    const std::string& separator,
			    float& result)
    {
      if (!separator.empty())
      {
	std::string::size_type split = value.rfind (separator);
	if (split != std::string::npos)
	{
	  std::istringstream str (value.substr (0, split) + "." + value.substr (split + separator.size()));
	  if (!(str >> result))
	    RCU_THROW_MSG ("failed to parse into a float: " + value);
	  char c;
	  if (str >> c)
	    RCU_THROW_MSG ("failed to parse into a float: " + value);
	  return true;
	}
      }
      return false;
    }



    /// \brief unpack the value from a string
    /// \sa packFloat
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   parse errors
    float unpackFloat (const std::string& value, const std::string& plus,
		       const std::string& minus)
    {
      float result = 0;
      if (unpackFloatHelper (value, plus, result))
	return result;
      if (unpackFloatHelper (value, minus, result))
      {
	result *= -1;
	return result;
      }
      RCU_THROW_MSG ("failed to parse into a float: " + value);
      return 0; //compiler dummy
    }
  }



  void SystematicVariation ::
  testInvariant () const
  {
    //RCU_INVARIANT (this != 0);
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



  #ifndef __CINT__
  SystematicVariation ::
  SystematicVariation (const std::string& val_basename, CONTINUOUS_ARG)
    : m_name (val_basename + "__continuous")
  {
    RCU_NEW_INVARIANT (this);
  }
  #endif



  SystematicVariation SystematicVariation ::
  makeToyVariation (const std::string& basename,
		    unsigned toyIndex, float toyScale)
  {
    RCU_REQUIRE (toyIndex > 0);
    RCU_REQUIRE (toyScale > 0);
    return SystematicVariation
      (basename, "toy_" + packUnsigned (toyIndex) + "_" + packFloat (toyScale, "scale", ""));
  }



  SystematicVariation SystematicVariation ::
  makeContinuousEnsemble (const std::string& basename)
  {
    return SystematicVariation (basename, CONTINUOUS);
  }



  SystematicVariation SystematicVariation ::
  makeToyEnsemble (const std::string& basename)
  {
    return SystematicVariation (basename, "toy_ensemble");
  }



  SystematicVariation SystematicVariation ::
  makeContinuous (const std::string& basename)
  {
    return SystematicVariation (basename + "__continuous");
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



  std::pair<unsigned,float> SystematicVariation ::
  getToyVariation () const
  {
    // no invariant used

    std::string variation = subvariation();

    const std::size_t prefixSize = 4;
    const auto split1 = variation.find ("toy_");
    if (split1 != 0)
      RCU_THROW_MSG ("not a toy variation: " + name());
    const auto split2 = variation.find ("_", split1 + prefixSize);
    if (split2 == std::string::npos)
      RCU_THROW_MSG ("not a toy variation: " + name());

    const auto sub1 = variation.substr (prefixSize, split2 - prefixSize);
    const auto sub2 = variation.substr (split2 + 1);
    return std::make_pair (unpackUnsigned (sub1), unpackFloat (sub2, "scale", ""));
  }



  bool SystematicVariation ::
  isToyEnsemble () const
  {
    // no invariant used
    return subvariation() == "toy_ensemble";
  }



  bool SystematicVariation ::
  isContinuousEnsemble () const
  {
    // no invariant used
    return subvariation() == "continuous";
  }



  bool SystematicVariation ::
  isEnsemble () const
  {
    // no invariant used
    return isToyEnsemble() || isContinuousEnsemble();
  }



  bool SystematicVariation ::
  ensembleContains (const SystematicVariation& sys) const
  {
    if (basename() != sys.basename())
      return false;
    if (isContinuousEnsemble() && sys.isContinuousVariation())
      return true;
    if (isToyEnsemble() && sys.isToyVariation())
      return true;
    return false;
  }



  bool SystematicVariation ::
  isToyVariation () const
  {
    static const boost::regex pattern (".*__toy_[0-9]+_[0-9]+((scale)[0-9]{1,4})?");
    return regex_match (m_name, pattern);
  }



  bool SystematicVariation ::
  isContinuousVariation () const
  {
    static const boost::regex pattern (".*__[0-9]+((up)|(down))([0-9]{1,4})?");
    return regex_match (m_name, pattern);
  }



  std::ostream& operator << (std::ostream& str, const CP::SystematicVariation& obj)
  {
    return str << obj.name();
  }
}
