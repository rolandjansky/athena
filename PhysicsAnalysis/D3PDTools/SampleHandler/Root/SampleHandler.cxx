/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

//protect
#include <SampleHandler/SampleHandler.h>

#include <iostream>
#include <sstream>
#include <TFile.h>
#include <TSystem.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/CheckRootVersion.h>
#include <RootCoreUtils/StringUtil.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <SampleHandler/DiskListLocal.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/Sample.h>
#include <SampleHandler/SamplePtr.h>

//
// method implementations
//

ClassImp (SH::SampleHandler)

namespace SH
{
  namespace
  {
    /*
    bool isspace (const std::string& str)
    {
      for (std::string::const_iterator iter = str.begin(),
	     end = str.end(); iter != end; ++ iter)
      {
	if (!std::isspace (*iter))
	  return false;
      };
      return true;
    }
    */
  }



  std::string dbg (const SampleHandler& obj, unsigned verbosity)
  {
    std::ostringstream result;
    result << "SampleHandler with " << obj.size() << " files";
    if (verbosity % 10 > 0)
    {
      result << "\n";
      for (SampleHandler::iterator sample = obj.begin(),
	     end = obj.end(); sample != end; ++ sample)
      {
	result << dbg (**sample, verbosity / 10) << "\n";
      };
    };
    return result.str();
  }



  void swap (SampleHandler& a, SampleHandler& b)
  {
    swap (a.m_samples, b.m_samples);
    swap (a.m_named, b.m_named);
  }



  void SampleHandler ::
  testInvariant () const
  {
  }



  SampleHandler ::
  SampleHandler ()
  {
    RCU::check_root_version ();
    RCU_NEW_INVARIANT (this);
  }



  SampleHandler ::
  SampleHandler (const SampleHandler& that)
    : TObject (that), m_samples (that.m_samples), m_named (that.m_named)
  {
    RCU_NEW_INVARIANT (this);
  }



  SampleHandler ::
  ~SampleHandler ()
  {
    RCU_DESTROY_INVARIANT (this);
  }



  SampleHandler& SampleHandler ::
  operator = (const SampleHandler& that)
  {
    // no invariant used
    SampleHandler tmp (that);
    swap (tmp, *this);
    return *this;
  }



  void SampleHandler ::
  add (Sample *sample)
  {
    SamplePtr mysample (sample);

    // invariant not used
    RCU_REQUIRE_SOFT (sample != 0);

    add (mysample);
  }



  void SampleHandler ::
  add (std::unique_ptr<Sample> sample)
  {
    // no invariant used
    add (SamplePtr (std::move (sample)));
  }



  void SampleHandler ::
  add (const SamplePtr& sample)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (!sample.empty());
    RCU_REQUIRE_SOFT (!sample->name().empty());

    if (!sample->name().empty() && m_named.find (sample->name()) != m_named.end())
      RCU_THROW_MSG ("can't add sample of name " + sample->name() + "\na sample with that name already exists\nold sample:\n" + dbg (*m_named.find (sample->name())->second, 9999) + "\nnew sample:\n" + dbg (*sample, 9999));

    try
    {
      m_samples.push_back (sample.get());
      if (!sample->name().empty())
	m_named[sample->name()] = sample;
    } catch (...)
    {
      if (m_samples.back() == sample.get())
	m_samples.pop_back();
    };
  }



  void SampleHandler ::
  add (const SampleHandler& sh)
  {
    // invariant not used
    RCU_REQUIRE_SOFT (this != &sh);

    for (iterator iter = sh.begin(), end2 = sh.end();
	 iter != end2; ++ iter)
    {
      add (*iter);
    };
  }



  void SampleHandler ::
  addWithPrefix (const SampleHandler& sh, const std::string& prefix)
  {
    // invariant not used
    RCU_REQUIRE_SOFT (this != &sh);

    for (iterator iter = sh.begin(), end2 = sh.end();
	 iter != end2; ++ iter)
    {
      std::unique_ptr<Sample> sample (dynamic_cast<Sample*>((*iter)->Clone ()));
      RCU_ASSERT (sample != nullptr);
      sample->name (prefix + (*iter)->name());
      add (sample.release());
    };
  }



  void SampleHandler ::
  remove (const std::string& name)
  {
    // invariant not used
    Sample *sample = get (name);
    if (sample == 0)
      RCU_THROW_MSG ("sample " + name + " not found in SampleHandler");
    remove (sample);
  }



  void SampleHandler ::
  remove (Sample *sample)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (sample != 0);

    NamedMIter nameIter = m_named.find (sample->name());
    if (nameIter == m_named.end())
      RCU_THROW_MSG ("sample " + sample->name() + " not found in SampleHandler");
    if (nameIter->second.get() != sample)
      RCU_THROW_MSG ("different sample of name " + sample->name() + " found in SampleHandler");
    SamplesMIter sampleIter = m_samples.end();
    for (SamplesMIter iter = m_samples.begin(), end2 = m_samples.end();
	 iter != end2; ++ iter)
    {
      if (*iter == sample)
	sampleIter = iter;
    };
    m_samples.erase (sampleIter);
    m_named.erase (nameIter);
  }



  Sample *SampleHandler ::
  get (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);

    NamedIter iter = m_named.find (name);
    if (iter != m_named.end())
      return iter->second.get();
    return 0;
  }



  SampleHandler SampleHandler ::
  find (const std::string& tags) const
  {
    // no invariant used
    return find (TagList (tags, ','));
  }



  SampleHandler SampleHandler ::
  find (const TagList& tags) const
  {
    RCU_READ_INVARIANT (this);

    SampleHandler result;

    for (SamplesIter sample = m_samples.begin(),
	   end = m_samples.end(); sample != end; ++ sample)
    {
      bool use = false;
      for (TagList::iterator iter = tags.begin(),
	     end = tags.end(); !use && iter != end; ++ iter)
	use = (*sample)->tags().has (*iter);
      if (use)
	result.add (*sample);
    };
    return result;
  }



  Sample *SampleHandler ::
  findBySource (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);

    std::vector<Sample*> result;
    for (iterator sample = begin(),
	   end2 = end(); sample != end2; ++ sample)
    {
      if (name == (*sample)->meta()->castString (MetaFields::sourceSample, (*sample)->name()))
	result.push_back ((*sample));
    };
    if (result.size() > 1)
    {
      std::ostringstream message;
      message << "multiple samples have " << name << " as a source:";
      for (std::vector<Sample*>::const_iterator sample = result.begin(),
	     end = result.end(); sample != end; ++ sample)
	message << " " << (*sample)->name();
      RCU_THROW_MSG (message.str());
    };
    if (result.empty())
      return 0;
    return result.front();
  }



  SampleHandler SampleHandler ::
  findByName (const std::string& pattern) const
  {
    RCU_READ_INVARIANT (this);
    SampleHandler result;
    boost::regex expr (pattern);
    for (iterator iter = begin(), end = this->end(); iter != end; ++ iter)
    {
      if (RCU::match_expr (expr, (*iter)->name()))
	result.add (*iter);
    }
    return result;
  }



  void SampleHandler ::
  print () const
  {
    RCU_READ_INVARIANT (this);
    std::cout << dbg (*this, 9999) << std::endl;
  }



  void SampleHandler ::
  printContent () const
  {
    // not using invariant
    print ();
  }



  void SampleHandler ::
  save (const std::string& directory) const
  {
    RCU_READ_INVARIANT (this);

    // rationale: not checking the return status, since this is just a
    //   courtesy directory creation that is Ok to fail.
    gSystem->MakeDirectory (directory.c_str());
    for (iterator iter = this->begin(),
	   end = this->end(); iter != end; ++ iter)
    {
      TFile file ((directory + "/" + (*iter)->name() + ".root").c_str(), "RECREATE");
      (*iter)->Write ("sample");
    };
  }



  void SampleHandler ::
  load (const std::string& directory)
  {
    RCU_CHANGE_INVARIANT (this);

    DiskListLocal mydir (directory);
    while (mydir.next())
    {
      const std::string file = mydir.fileName();

      if (file.size() > 5 &&
	  file.rfind (".root") == file.size() - 5)
      {
	TFile myfile (mydir.path().c_str(), "READ");
	Sample *const sample = dynamic_cast<Sample*>(myfile.Get ("sample"));
	if (sample != 0)
	  add (sample);
      };
    };    
  }



  void SampleHandler ::
  updateLocation (const std::string& from, const std::string& to)
  {
    // no invariant used
    RCU_REQUIRE_SOFT (!from.empty());
    RCU_REQUIRE_SOFT (!to.empty());
    for (iterator sample = begin(),
	   end = this->end(); sample != end; ++ sample)
      (*sample)->updateLocation (from, to);
  }



  void SampleHandler ::
  fetch (const SampleHandler& source)
  {
    // invariant not used

    for (iterator sample = begin(),
	   end2 = end(); sample != end2; ++ sample)
    {
      const std::string name
	= (*sample)->meta()->castString (MetaFields::sourceSample, (*sample)->name());
      const Sample *const mysource = source.get (name);
      if (mysource)
	(*sample)->meta()->fetch (*mysource->meta());
    };
  }



  void SampleHandler ::
  fetchDefaults (const SampleHandler& source)
  {
    // invariant not used

    for (iterator sample = begin(),
	   end2 = end(); sample != end2; ++ sample)
    {
      const std::string name
	= (*sample)->meta()->castString (MetaFields::sourceSample, (*sample)->name());
      const Sample *const mysource = source.get (name);
      if (mysource)
	(*sample)->meta()->fetchDefaults (*mysource->meta());
    };
  }



  bool SampleHandler ::
  check_complete (const SampleHandler& source) const
  {
    // invariant not used

    std::set<std::string> names;
    for (iterator sample = begin(),
	   end2 = end(); sample != end2; ++ sample)
    {
      names.insert ((*sample)->meta()->castString (MetaFields::sourceSample, (*sample)->name()));
    };

    for (iterator sample = source.begin(),
	   end2 = source.end(); sample != end2; ++ sample)
    {
      if (names.find ((*sample)->name()) == names.end())
	return false;
    };
    return true;
  }



  void SampleHandler ::
  setMetaDouble (const std::string& name, double value)
  {
    // no invariant used

    for (iterator sample = begin(),
	   end2 = end(); sample != end2; ++ sample)
    {
      (*sample)->meta()->setDouble (name, value);
    };
  }



  void SampleHandler ::
  setMetaString (const std::string& name, const std::string& value)
  {
    // no invariant used

    for (iterator sample = begin(),
	   end2 = end(); sample != end2; ++ sample)
    {
      (*sample)->meta()->setString (name, value);
    };
  }



  void SampleHandler ::
  setMetaDouble (const std::string& pattern, const std::string& name,
		 double value)
  {
    // no invariant used

    boost::regex mypattern (pattern);

    for (iterator sample = begin(),
	   end2 = end(); sample != end2; ++ sample)
    {
      if (RCU::match_expr (mypattern, (*sample)->name()))
	(*sample)->meta()->setDouble (name, value);
    };
  }



  void SampleHandler ::
  setMetaString (const std::string& pattern, const std::string& name,
		 const std::string& value)
  {
    // no invariant used

    boost::regex mypattern (pattern);

    for (iterator sample = begin(),
	   end2 = end(); sample != end2; ++ sample)
    {
      if (RCU::match_expr (mypattern, (*sample)->name()))
	(*sample)->meta()->setString (name, value);
    };
  }



  SampleHandler::iterator SampleHandler ::
  begin () const
  {
    RCU_READ_INVARIANT (this);
    return m_samples.begin();
  }



  SampleHandler::iterator SampleHandler ::
  end () const
  {
    RCU_READ_INVARIANT (this);
    return m_samples.end();
  }



  std::size_t SampleHandler ::
  size () const
  {
    RCU_READ_INVARIANT (this);
    return m_samples.size();
  }



  Sample *SampleHandler ::
  operator [] (std::size_t index) const
  {
    // no invariant used
    return at (index);
  }



  Sample *SampleHandler ::
  at (std::size_t index) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE_SOFT (index < size());
    return m_samples[index];
  }



  void SampleHandler ::
  Streamer (TBuffer& b)
  {
    if (b.IsReading())
    {
      RCU_CHANGE_INVARIANT (this);
      SampleHandler sh;
      ULong_t count = 0;
      b.ReadULong (count);
      for (ULong_t iter = 0; iter != count; ++ iter)
      {
	Sample *sample = 0;
	b >> sample;
	sh.add (sample);
      }
      swap (*this, sh);
    } else
    {
      RCU_READ_INVARIANT (this);
      ULong_t count = m_samples.size(), count2 = 0;
      b.WriteULong (count);
      for (SamplesIter iter = m_samples.begin(),
	     end = m_samples.end(); iter != end; ++ iter, ++ count2)
      {
	Sample *sample = *iter;
	b << sample;
      }
    };
  }
}
