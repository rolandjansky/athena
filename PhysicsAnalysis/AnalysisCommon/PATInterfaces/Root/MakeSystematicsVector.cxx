/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <PATInterfaces/MakeSystematicsVector.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/StringUtil.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <TRandom3.h>
#include <TRegexp.h>
#include <cstdint>
#include <map>
#include <memory>

//
// method implementations
//

namespace CP
{
  namespace
  {
    /// \brief make a hash of the given string
    ///
    /// I added this because there is no guarantee that different
    /// versions of std::hash actually produce the same hash.
    ///
    /// I stole this from
    /// http://stackoverflow.com/questions/114085/fast-string-hashing-algorithm-with-low-collision-rates-with-32-bit-integer
    ///
    /// \par Guarantee
    ///   no-fail
    uint32_t hash_string (const std::string& str)
    {
      uint32_t hash = 0;

      for (char ch : str)
      {
    	hash += ch;
    	hash += (hash << 10);
    	hash ^= (hash >> 6);
      }

      hash += (hash << 3);
      hash ^= (hash >> 11);
      hash += (hash << 15);

      return hash;
    }
  }



  void MakeSystematicsVector ::
  testInvariant () const
  {
    //RCU_INVARIANT (this != nullptr);
    RCU_INVARIANT (!m_config.empty());
  }



  MakeSystematicsVector ::
  MakeSystematicsVector ()
    : m_config (1)
  {
    RCU_NEW_INVARIANT (this);
  }



  const std::vector<SystematicSet>& MakeSystematicsVector ::
  result (const std::string& label) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (!m_result.empty(), "calculate() has been called");
    auto iter = m_result.find (label);
    if (iter == m_result.end())
      RCU_THROW_MSG ("unknown systematics group: " + label);
    return iter->second;
  }



  void MakeSystematicsVector ::
  calc (const SystematicSet& sysList)
  {
    RCU_CHANGE_INVARIANT (this);

    auto baseSys = calcBaseSys (sysList);

    std::map<std::string,std::vector<SystematicSet>> myresult;
    myresult[m_useForNominal].push_back (SystematicSet ());
    for (std::size_t group = 0; group != m_config.size(); ++ group)
    {
      const auto& config = m_config[group];

      // note: this is not just a short-cut, but also makes sure that
      // we have an entry for each label, even if there are no
      // systematics for the label
      auto& subresult = myresult[config.label];

      // this skips groups that don't match any requested systematics,
      // which is mainly important for toy systematics as you wouldn't
      // want to generate a bunch of empty systematics
      if (baseSys[group].empty())
	continue;

      if (config.toys == 0)
      {
	for (auto sys : baseSys[group])
	{
	  RCU_ASSERT (!sys.second.empty());
	  RCU_ASSERT (!sys.second.front().isToyEnsemble());
	  if (sys.second.front().isContinuousEnsemble())
	  {
	    // for continuous systematics
	    subresult.push_back(CP::SystematicSet());
	    subresult.back().insert (CP::SystematicVariation (sys.first, config.sigma));
	    subresult.push_back(CP::SystematicSet());
	    subresult.back().insert (CP::SystematicVariation (sys.first, -config.sigma));
	  } else if (sys.second.front().isEnsemble())
	  {
	    // we must have added a new kind of ensemble after I wrote
	    // this code
	    RCU_THROW_MSG ("unsupported ensemble systematic: " + sys.first);
	  } else
	  {
	    // otherwise just add all of them flat
	    for (auto mysys : sys.second)
	    {
	      subresult.push_back(CP::SystematicSet());
	      subresult.back().insert(mysys);
	    }
	  }
	}
      } else
      {
	std::vector<CP::SystematicSet> toys (config.toys);

	for (auto sys : baseSys[group])
	{
	  RCU_ASSERT (!sys.second.empty());
	  RCU_ASSERT (sys.second.front().isEnsemble());

	  if (sys.second.front().isContinuousEnsemble())
	  {
	    std::unique_ptr<TRandom3> random (new TRandom3);
	    random->SetSeed (hash_string (sys.first));

	    for (auto& toy : toys)
	      toy.insert (CP::SystematicVariation (sys.first, random->Gaus (0, config.sigma)));
	  } else if (sys.second.front().isToyEnsemble())
	  {
	    for (unsigned toy = 0; toy != config.toys; ++ toy)
	      toys[toy].insert (CP::SystematicVariation::makeToyVariation (sys.first, toy + 1, config.sigma));
	  } else
	  {
	    // we must have added a new kind of ensemble after I
	    // wrote this code
	    RCU_THROW_MSG ("unsupported ensemble systematic for toys: " + sys.first);
	  }
	}
	for (auto& toy : toys)
	  subresult.push_back (std::move (toy));
      }
    }

    m_result = myresult;
  }



  void MakeSystematicsVector ::
  addGroup (const std::string& val_label)
  {
    RCU_CHANGE_INVARIANT (this);
    GroupConfig config;
    config.label = val_label;
    m_config.push_back (config);
  }



  void MakeSystematicsVector ::
  setPattern (const std::string& val_pattern)
  {
    RCU_CHANGE_INVARIANT (this);
    m_config.back().pattern = val_pattern;
  }



  void MakeSystematicsVector ::
  setSigma (float val_sigma)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (val_sigma > 0);
    m_config.back().sigma = val_sigma;
  }



  void MakeSystematicsVector ::
  setToys (unsigned val_toys)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (val_toys > 0);
    m_config.back().toys = val_toys;
  }



  void MakeSystematicsVector ::
  useForNominal ()
  {
    RCU_CHANGE_INVARIANT (this);
    m_useForNominal = m_config.back().label;
  }



  std::vector<std::map<std::string,std::vector<SystematicVariation>>>
  MakeSystematicsVector ::
  calcBaseSys (const SystematicSet& sysList)
  {
    std::map<std::string,std::vector<SystematicVariation> > basesys;
    for (auto sys : sysList)
    {
      basesys[sys.basename()].push_back (sys);
    }
    std::vector<std::map<std::string,std::vector<SystematicVariation> >>
      basesysList (m_config.size());
    for (auto sys : basesys)
    {
      // extract the ensemble if we have one
      SystematicVariation ensemble;
      for (auto mysys : sys.second)
      {
	if (mysys.isEnsemble())
	{
	  if (!ensemble.empty())
	    RCU_THROW_MSG ("inconsistent ensembles requested: " + ensemble.name() + " " + mysys.name());
	  ensemble = mysys;
	}
      }

      // setting this beyond the valid groups in case none matches
      std::size_t group = m_config.size();
      for (std::size_t iter = 0; iter != m_config.size(); ++ iter)
      {
	if (m_config[iter].pattern.empty())
	{
	  // only use empty patterns if no previous pattern already took this
	  if (group == m_config.size())
	  {
	    if (m_config[iter].toys > 0)
	    {
	      if (ensemble.isToyEnsemble())
		group = iter;
	    } else
	    {
	      if (!ensemble.isToyEnsemble())
		group = iter;
	    }
	  }
	} else if (RCU::match_expr (boost::regex (m_config[iter].pattern.c_str()), sys.first))
	{
	  if (m_config[iter].toys > 0 && ensemble.empty())
	    RCU_THROW_MSG ("toys only supported for ensemble systematics");
	  group = iter;
	}
      }
      if (group == m_config.size())
	RCU_THROW_MSG ("no systematics group for systematic: " + sys.first);

      if (!ensemble.empty())
      {
	basesysList[group][sys.first].push_back (ensemble);
      } else
      {
	basesysList[group][sys.first] = std::move (sys.second);
      }
    }
    return basesysList;
  }
}
