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

#include <PATInterfaces/SystematicList.h>

#include <RootCoreUtils/Assert.h>
#include <PATInterfaces/SystematicCode.h>

//
// method implementations
//

namespace CP
{
  systematics_list_type& systematics_list()
  {
    static systematics_list_type result;
    return result;
  }



  systematics_list_type& default_systematics()
  {
    static systematics_list_type result;
    return result;
  }



  std::string
  joinSystematicList (const std::set<SystematicVariation>& systConfig)
  {
    std::string result;

    for (std::set<SystematicVariation>::const_iterator
	   sys = systConfig.begin(),
	 end = systConfig.end(); sys != end; ++ sys)
    {
      if (!result.empty())
	result += "-";
      result += sys->name();
    }
    return result;
  }



  std::set<SystematicVariation>
  splitSystematicList (const std::string& systematics)
  {
    std::set<SystematicVariation> result;

    std::string::size_type split = 0, split2 = 0;
    while ((split = systematics.find ("-", split2)) != std::string::npos)
    {
      result.insert (systematics.substr (split2, split - split2));
      split2 = split + 1;
    }
    result.insert (systematics.substr (split2, split - split2));
    return result;
  }



  SystematicCode filterForAffectingSystematics
  (const std::set<SystematicVariation>& systConfig,
   const std::set<SystematicVariation>& affectingSystematics,
   std::set<SystematicVariation>& filteredSystematics)
  {
    // convert affecting set into base systematics
    std::set<std::string> affectingBaseSystematics;
    for (std::set<SystematicVariation>::const_iterator
	   sys = affectingSystematics.begin(),
	 end = affectingSystematics.end(); sys != end; ++ sys)
      affectingBaseSystematics.insert (sys->basename());

    std::set<SystematicVariation> myFilteredSystematics;
    std::set<std::string> basenames;

    // loop over given systematics to filter
    for (std::set<SystematicVariation>::const_iterator
	   sys = systConfig.begin(),
	 end = systConfig.end(); sys != end; ++ sys)
    {
      // Get the corresponding continuous systematic
      const std::string basename = sys->basename();
      const SystematicVariation
	continuous (basename, SystematicVariation::CONTINUOUS);

      // If neither this sys nor the continuous counterpart are in the
      // global set of systematics, return unsupported. This check
      // doesn't necessarily need to be done here.
      if (systematics_list().find (*sys) == systematics_list().end() &&
	  systematics_list().find (continuous) == systematics_list().end())
	return SystematicCode::Unsupported;
      // This snippet does nothing...
      if (basenames.find (basename) != basenames.end())
	return SystematicCode::Unsupported;
      // If this systematic or its continuous counterpart are
      // in the affecting set, add them to the filtered set.
      if (affectingSystematics.find (sys->name()) !=
	  affectingSystematics.end() ||
	  affectingSystematics.find (continuous) !=
	  affectingSystematics.end())
      {
	myFilteredSystematics.insert (*sys);
      } else
      {
        // If systematic wasn't found in the affecting list,
        // but base systematic does match the affecting base systematics,
        // then it means this subvariation is unsupported.
	if (affectingBaseSystematics.find (basename) !=
	    affectingBaseSystematics.end())
	  return SystematicCode::Unsupported;
      }
    }
    myFilteredSystematics.swap (filteredSystematics);
    return SystematicCode::Ok;
  }
}
