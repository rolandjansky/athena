/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include <SampleHandler/Sample.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/PrintMsg.h>
#include <RootCoreUtils/RootUtils.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaNames.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/SampleGrid.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SamplePtr.h>
#include <TChain.h>
#include <TDSet.h>
#include <TFile.h>
#include <memory>
#include <iostream>

//
// method implementations
//

ClassImp (SH::Sample)

namespace SH
{
  std::string dbg (const Sample& obj, unsigned verbosity)
  {
    std::string result;
    result += "Sample:name=" + obj.name();
    if (dynamic_cast<const SampleGrid*>(&obj))
    {
      if (verbosity % 10 > 0)
      {
	result += "\n " + MetaFields::gridName + "="+obj.meta()->castString(MetaFields::gridName);
        result += "\n " + MetaFields::gridFilter + "="+obj.meta()->castString(MetaFields::gridFilter);
      };
    } else
    {
      if (verbosity % 10 > 0)
	result += ",tags=" + dbg (obj.tags(), verbosity / 10);
      if (verbosity % 10 > 1)
      {
	result += "\n";
	for (std::size_t iter = 0, end = obj.numFiles();
	     iter != end; ++ iter)
	  result += obj.fileName (iter) + "\n";
      };
    };
    return result;
  }



  void Sample ::
  testInvariant () const
  {
    RCU_INVARIANT (m_meta != 0);
 }



  Sample ::
  ~Sample ()
  {
    RCU_DESTROY_INVARIANT (this);

    delete m_meta;
  }



  const std::string& Sample ::
  name () const
  {
    RCU_READ_INVARIANT (this);
    return m_name;
  }



  void Sample ::
  name (std::string val_name)
  {
    RCU_CHANGE_INVARIANT (this);
    if (m_references > 0)
      RCU_THROW_MSG ("Sample already owned by SampleHandler");
    m_meta->setString (MetaNames::sampleName(), val_name);
    m_name = std::move (val_name);
    
  }



  std::size_t Sample ::
  numFiles () const
  {
    RCU_READ_INVARIANT (this);
    return getNumFiles();
  }



  std::string Sample ::
  fileName (const std::size_t index) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE_SOFT (index < numFiles());

    std::string result = getFileName (index);

    RCU_PROVIDE (!result.empty());
    return result;
  }



  SamplePtr Sample ::
  makeLocal () const
  {
    RCU_READ_INVARIANT (this);
    return doMakeLocal ();
  }



  const TagList& Sample ::
  tags () const
  {
    RCU_READ_INVARIANT (this);
    return m_tags;
  }



  void Sample ::
  tags (const TagList& tags)
  {
    RCU_CHANGE_INVARIANT (this);
    m_tags = tags;
  }



  void Sample ::
  addTag (const std::string& tag)
  {
    RCU_CHANGE_INVARIANT (this);
    m_tags.add (tag);
  }



  void Sample ::
  updateLocation (const std::string& from, const std::string& to)
  {
    RCU_CHANGE_INVARIANT (this);
    doUpdateLocation (from, to);
  }



  MetaObject *Sample ::
  meta ()
  {
    RCU_READ_INVARIANT (this);
    return m_meta;
  }



  const MetaObject *Sample ::
  meta () const
  {
    RCU_READ_INVARIANT (this);
    return m_meta;
  }



  std::vector<std::string> Sample ::
  makeFileList () const
  {
    RCU_READ_INVARIANT (this);
    return doMakeFileList ();
  }



  TChain *Sample ::
  makeTChain () const
  {
    // no invariant used

    std::vector<std::string> files = makeFileList ();

    const std::string treeName (meta()->castString (MetaFields::treeName, MetaFields::treeName_default));
    if (treeName.empty())
      RCU_THROW_MSG ("sample " + name() + " does not have a tree name associated");
    std::unique_ptr<TChain> result (new TChain (treeName.c_str()));
    for (std::vector<std::string>::const_iterator file = files.begin(),
	   end = files.end(); file != end; ++ file)
      result->AddFile (file->c_str());
    return result.release();
  }



  TDSet *Sample ::
  makeTDSet () const
  {
    // no invariant used

    std::vector<std::string> files = makeFileList ();

    std::string treeName (meta()->castString (MetaFields::treeName, MetaFields::treeName_default));
    if (treeName.empty())
      RCU_THROW_MSG ("sample " + name() + " does not have a tree name associated");

    std::string dir = "/";
    std::string::size_type split = treeName.rfind ("/");
    if (split != std::string::npos)
    {
      dir += treeName.substr (0, split);
      treeName = treeName.substr (split+1);
    }
    std::unique_ptr<TDSet> result (new TDSet ("TTree", treeName.c_str(), dir.c_str()));
    for (std::vector<std::string>::const_iterator file = files.begin(),
	   end = files.end(); file != end; ++ file)
      result->Add (file->c_str());
    return result.release();
  }



  void Sample ::
  doUpdateLocation (const std::string& /*from*/, const std::string& /*to*/)
  {
    RCU_READ_INVARIANT (this);
  }



  TObject *Sample ::
  readHist (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    return doReadHist (name);
  }



  bool Sample ::
  contains (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    if (m_name == name)
      return true;
    return false;
  }



  void Sample ::
  addSamples (SampleHandler& result)
  {
    RCU_READ_INVARIANT (this);
    doAddSamples (result);
  }



  void Sample ::
  print () const
  {
    RCU_READ_INVARIANT (this);
    std::cout << dbg (*this, 9999) << std::endl;
  }



  void Sample ::
  printContent () const
  {
    // not using invariant
    print ();
  }



  Long64_t Sample ::
  getNumEntries () const
  {
    RCU_READ_INVARIANT (this);

    std::string treeName
      = meta()->castString (MetaFields::treeName, MetaFields::treeName_default);
    Long64_t result = 0;
    std::vector<std::string> fileList = makeFileList();
    for (std::vector<std::string>::const_iterator fileName = fileList.begin(),
	   end = fileList.end(); fileName != end; ++ fileName)
    {
      std::unique_ptr<TFile> file (TFile::Open (fileName->c_str(), "READ"));
      if (file.get() == 0)
	RCU_THROW_MSG ("failed to open file: " + *fileName);
      TTree *tree = dynamic_cast<TTree*>(file->Get (treeName.c_str()));
      if (tree)
	result += tree->GetEntries();
    }
    return result;
  }



  TCollection *Sample ::
  metaDataList ()
  {
    // no invariant used
    return meta();
  }



  const TCollection *Sample ::
  metaDataList () const
  {
    // no invariant used
    return meta();
  }



  void Sample ::
  removeMeta (const std::string& name)
  {
    // no invariant used
    meta()->remove (name);
  }



  void Sample ::
  addReplaceMeta (TNamed *meta_swallow)
  {
    std::unique_ptr<TNamed> mymeta (meta_swallow);

    // no invariant used
    RCU_REQUIRE_SOFT (meta_swallow != 0);

    meta()->addReplace (mymeta.release());
  }



  TObject *Sample ::
  getMeta (const std::string& name)
  {
    // no invariant used
    return meta()->get (name);
  }



  const TObject *Sample ::
  getMeta (const std::string& name) const
  {
    // no invariant used
    return meta()->get (name);
  }



  double Sample ::
  getMetaDouble (const std::string& name, double def_val) const
  {
    // no invariant used
    return meta()->castDouble (name, def_val, MetaObject::CAST_NOCAST_DEFAULT);
  }



  std::string Sample ::
  getMetaString (const std::string& name, const std::string& def_val) const
  {
    // no invariant used
    return meta()->castString (name, def_val, MetaObject::CAST_NOCAST_DEFAULT);
  }



  void Sample ::
  setMetaDouble (const std::string& name, double value)
  {
    // no invariant used
    meta()->setDouble (name, value);
  }



  void Sample ::
  setMetaString (const std::string& name, const std::string& value)
  {
    // no invariant used
    meta()->setString (name, value);
  }



  void Sample ::
  fetchMeta (const Sample& source)
  {
    // no invariant used
    meta()->fetch (*source.meta());
  }



  Sample ::
  Sample (const std::string& name)
    : m_name (name), m_meta (new MetaObject),
      m_references (0)
  {
    m_meta->setString (MetaNames::sampleName(), name);

    RCU_NEW_INVARIANT (this);
  }



  TObject *Sample ::
  doReadHist (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    std::vector<std::string> fileList (makeFileList());
    if (fileList.size() > 1)
      RCU_THROW_MSG ("reading histgrams from samples with multiple files is not (yet) implemented");
    if (fileList.size() == 0)
      return nullptr;
    std::unique_ptr<TFile> file (TFile::Open (fileList[0].c_str(), "READ"));
    if (file == nullptr)
      RCU_THROW_MSG ("could not open file " + fileList[0]);
    TObject *object = file->Get (name.c_str());
    if (object != nullptr)
      RCU::SetDirectory (object, nullptr);
    return object;
  }



  bool Sample ::
  getContains (const std::string& /*name*/) const
  {
    RCU_READ_INVARIANT (this);
    return false;
  }



  void Sample ::
  doAddSamples (SampleHandler& result)
  {
    RCU_READ_INVARIANT (this);
    result.add (this);
  }



  void Sample ::
  alloc () const
  {
    RCU_CHANGE_INVARIANT (this);
    ++ m_references;
  }



  void Sample ::
  release () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2 (m_references > 0, "reference count > 0");

    -- m_references;
    if (m_references == 0)
      delete this;
  }
}
