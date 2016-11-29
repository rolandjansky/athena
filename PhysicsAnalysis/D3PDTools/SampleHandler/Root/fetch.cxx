/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// includes
//

#include <SampleHandler/fetch.h>

#include <sstream>
#include <TPython.h>
#include <TString.h>
#include <TSystem.h>
#include <SampleHandler/MetaDataQuery.h>
#include <SampleHandler/MetaDataSample.h>
#include <SampleHandler/MetaObject.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/PrintMsg.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/Sample.h>
#include <SampleHandler/SampleHandler.h>

//
// method implementations
//

namespace SH
{
  void fetchMetaData (MetaDataQuery& query)
  {
    static bool loaded = false;
    if (!loaded)
    {
      TString path = "$ROOTCOREBIN/python/SampleHandler/SampleHandler_QueryAMI.py";
      gSystem->ExpandPathName (path);
      TPython::LoadMacro (path.Data());
      loaded = true;
    }

    std::ostringstream command;
    command << "SampleHandler_QueryAmi([";
    for (std::size_t iter = 0, end = query.samples.size(); iter != end; ++ iter)
    {
      if (iter != 0)
	command << ", ";
      command << "'" << query.samples[iter].name << "'";
    }
    command << "])";
    MetaDataQuery* myquery = static_cast<MetaDataQuery*>
      ((void*) TPython::Eval (command.str().c_str()));
    query = *myquery;
  }


  void fetchMetaData (SH::SampleHandler& sh, bool override)
  {
    std::vector<SH::Sample*> samples;
    // typedef std::vector<SH::Sample*> SamplesIter;
    MetaDataQuery query;
    for (SH::SampleHandler::iterator sample = sh.begin(),
	   end = sh.end(); sample != end; ++ sample)
    {
      std::string name = (*sample)->meta()->castString (SH::MetaFields::gridName,(*sample)->name());
      query.samples.push_back (MetaDataSample (name));
      samples.push_back (&**sample);
    }
    fetchMetaData (query);

    if (!query.messages.empty())
      RCU_PRINT_MSG (query.messages);
    for (std::size_t iter = 0, end = query.samples.size(); iter != end; ++ iter)
    {
      if (query.samples[iter].unknown)
      {
	RCU_WARN_MSG ("failed to find sample " + query.samples[iter].name);
      } else
      {
	RCU_ASSERT (iter != samples.size());
	SH::Sample *sample = samples[iter];

	if (!override)
	{
	  query.samples[iter].isData = sample->meta()->castDouble (SH::MetaFields::isData, query.samples[iter].isData);
	  query.samples[iter].luminosity = sample->meta()->castDouble (SH::MetaFields::lumi, query.samples[iter].luminosity);
	  query.samples[iter].crossSection = sample->meta()->castDouble (SH::MetaFields::crossSection, query.samples[iter].crossSection);
	  query.samples[iter].nevents = sample->meta()->castDouble (SH::MetaFields::numEvents, query.samples[iter].nevents);
	  query.samples[iter].kfactor = sample->meta()->castDouble (SH::MetaFields::kfactor, query.samples[iter].kfactor);
	  query.samples[iter].filterEfficiency = sample->meta()->castDouble (SH::MetaFields::filterEfficiency, query.samples[iter].filterEfficiency);
	}
	if (query.samples[iter].isData != -1)
	  sample->meta()->setDouble (SH::MetaFields::isData, query.samples[iter].isData);
	if (query.samples[iter].luminosity != -1)
	  sample->meta()->setDouble (SH::MetaFields::lumi, query.samples[iter].luminosity);
	if (query.samples[iter].crossSection != -1)
	  sample->meta()->setDouble (SH::MetaFields::crossSection, query.samples[iter].crossSection);
	if (query.samples[iter].nevents != -1)
	  sample->meta()->setDouble (SH::MetaFields::numEvents, query.samples[iter].nevents);
	if (query.samples[iter].kfactor != -1)
	  sample->meta()->setDouble (SH::MetaFields::kfactor, query.samples[iter].kfactor);
	if (query.samples[iter].filterEfficiency != -1)
	  sample->meta()->setDouble (SH::MetaFields::filterEfficiency, query.samples[iter].filterEfficiency);
      }
    }
  }
}
