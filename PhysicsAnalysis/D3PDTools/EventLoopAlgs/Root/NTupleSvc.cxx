/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoopAlgs/NTupleSvc.h>

#include <fstream>
#include <memory>
#include <TBranchElement.h>
#include <TFile.h>
#include <TLeaf.h>
#include <TRegexp.h>
#include <TTree.h>
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <EventLoopAlgs/WhiteBoardSvc.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

ClassImp(EL::NTupleSvc)

namespace EL
{
  NTupleSvc *getNTupleSvc (IWorker *worker, const std::string& outputStream,
			   const std::string& treeName)
  {
    RCU_REQUIRE_SOFT (worker != 0);
    RCU_REQUIRE_SOFT (!outputStream.empty());
    NTupleSvc *result = dynamic_cast<NTupleSvc*>
      (worker->getAlg ("NTupleSvc_" + outputStream + "_" + treeName));
    if (result == 0)
    {
      result = dynamic_cast<NTupleSvc*>
	(worker->getAlg ("NTupleSvc_" + outputStream));
    }
    RCU_ASSERT2_SOFT (result != 0, "output algorithm not found");
    return result;
  }



  void NTupleSvc ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  NTupleSvc ::
  NTupleSvc (const std::string& val_outputName)
    : m_file (0), m_tree (0), m_initialized (false),
      m_whiteboard (0)
  {
    RCU_REQUIRE_SOFT (!val_outputName.empty());

    m_outputName = val_outputName;

    RCU_NEW_INVARIANT (this);
  }



  NTupleSvc ::
  ~NTupleSvc ()
  {
    RCU_DESTROY_INVARIANT (this);
  }



  void NTupleSvc ::
  copyBranch (const std::string& name)
  {
    RCU_CHANGE_INVARIANT (this);
    m_copyBranches.insert (name);
  }



  void NTupleSvc ::
  copyBranchList (const std::string& fileName)
  {
    // no invariant used

    std::ifstream file (fileName.c_str());
    std::string line;
    while (getline (file, line))
    {
      while (!line.empty() && isspace (line[0]))
	line = line.substr (1);
      while (!line.empty() && isspace (line[line.size()-1]))
	line = line.substr (0, line.size()-1);
      if (!line.empty() && line[0] != '#')
	copyBranch (line);
    }
  }



  void NTupleSvc ::
  addWhiteFloat (const std::string& varName)
  {
    RCU_CHANGE_INVARIANT (this);
    m_whiteFloat.insert (varName);
  }



  void NTupleSvc ::
  addWhiteArray (const std::string& varName)
  {
    RCU_CHANGE_INVARIANT (this);
    m_whiteArray.insert (varName);
  }



  TTree *NTupleSvc ::
  tree () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE2_SOFT (m_tree != 0, "initialize() has been called");
    return m_tree;
  }



  bool NTupleSvc ::
  getFilterPassed () const
  {
    RCU_READ_INVARIANT (this);
    return m_taken;
  }



  void NTupleSvc ::
  setFilterPassed (bool passed)
  {
    RCU_CHANGE_INVARIANT (this);
    if (passed)
      m_taken = true;
  }



  const std::string& NTupleSvc ::
  treeName () const
  {
    RCU_READ_INVARIANT (this);
    return m_treeName;
  }



  void NTupleSvc ::
  treeName (const std::string& val_treeName)
  {
    RCU_CHANGE_INVARIANT (this);
    m_treeName = val_treeName;
  }



  const char *NTupleSvc ::
  GetName () const
  {
    RCU_READ_INVARIANT (this);
    return m_outputName.c_str();
  }



  StatusCode NTupleSvc ::
  setupJob (Job& job)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE2_SOFT (job.outputHas (m_outputName), ("output stream " + m_outputName + " not configured.  try:\n  EL::OutputStream output (\"" + m_outputName + "\");\n  job.addOutput (output);").c_str());
    return EL::StatusCode::SUCCESS;
  }



  EL::StatusCode NTupleSvc :: changeInput (bool /*firstFile*/)
  {
    RCU_CHANGE_INVARIANT (this);

    m_connected = false;
    return EL::StatusCode::SUCCESS;
  }



  EL::StatusCode NTupleSvc :: initialize ()
  {
    RCU_CHANGE_INVARIANT (this);

    m_file = wk()->getOutputFile (m_outputName);
    std::string name = m_treeName;
    if (name.empty()) name = wk()->tree()->GetName();
    TDirectory *curDir = gDirectory;
    try
    {
      m_file->cd ();
      m_tree = new TTree (name.c_str(), m_tree_title.c_str());
      curDir->cd ();
    } catch (...)
    {
      curDir->cd ();
    }

    // Turn off auto saving:
    m_tree->SetAutoSave(10000);
    // Flush the tree contents after collecting 30 MB in memory:
    m_tree->SetAutoFlush( -30000000 );
    TTree::SetBranchStyle(1);

    return EL::StatusCode::SUCCESS;
  }



  EL::StatusCode NTupleSvc :: execute ()
  {
    RCU_CHANGE_INVARIANT (this);

    m_taken = false;
    return EL::StatusCode::SUCCESS;
  }



  EL::StatusCode NTupleSvc :: postExecute ()
  {
    RCU_CHANGE_INVARIANT (this);

    initBranches ();

    if (!m_taken)
      return EL::StatusCode::SUCCESS;

    copyInput ();

    for (whiteInfoMIter iter = m_whiteInfo.begin(),
	   end = m_whiteInfo.end(); iter != end; ++ iter)
    {
      if (iter->array)
      {
	std::size_t size;
	const float *values;
	m_whiteboard->getArray (iter->name, size, values);
	std::vector<float>(values,values+size).swap (iter->buffer);
      } else
	iter->buffer[0] = m_whiteboard->getFloat (iter->name);
    }

    m_tree->Fill ();
    return EL::StatusCode::SUCCESS;
  }



  bool NTupleSvc ::
  hasName (const std::string& name) const
  {
    RCU_READ_INVARIANT (this);
    return
      name == "NTupleSvc_" + m_outputName ||
      name == "NTupleSvc_" + m_outputName + "_" + m_treeName;
  }



  void NTupleSvc ::
  initBranches ()
  {
    if (m_initialized == false)
    {
      m_initialized = true;

      std::set<std::string> branchList;

      findBranches (branchList);
      initOutput (branchList);

      for (m_whiteFloatIter iter = m_whiteFloat.begin(),
	     end = m_whiteFloat.end(); iter != end; ++ iter)
      {
	WhiteInfo info;
	info.name = *iter;
	info.array = false;
	m_whiteInfo.push_back (info);
      }
      for (m_whiteArrayIter iter = m_whiteArray.begin(),
	     end = m_whiteArray.end(); iter != end; ++ iter)
      {
	WhiteInfo info;
	info.name = *iter;
	info.array = true;
	m_whiteInfo.push_back (info);
      }
      for (whiteInfoMIter iter = m_whiteInfo.begin(),
	     end = m_whiteInfo.end(); iter != end; ++ iter)
      {
	if (iter->array)
	{
	  iter->pointer = &iter->buffer;
	  m_tree->Branch (iter->name.c_str(), &iter->pointer);
	} else
	{
	  iter->buffer.resize (1);
	  m_tree->Branch (iter->name.c_str(), &iter->buffer[0], (iter->name + "/F").c_str());
	}
      }
      if (!m_whiteInfo.empty())
	m_whiteboard = getWhiteBoardSvc (wk());
    }
  }


  void NTupleSvc ::
  findBranches (std::set<std::string>& branchList)
  {
    for (m_copyBranchesIter iter = m_copyBranches.begin(),
	   end = m_copyBranches.end(); iter != end; ++ iter)
    {
      std::size_t count = 0;
      TRegexp pattern (*iter);
      TObject *object = 0;
 
      for (TIter branchIter (wk()->tree()->GetListOfBranches());
	   (object = branchIter());)
      {
	TString str (object->GetName());
	Ssiz_t len = 0;

	if (pattern.Index (str, &len) == 0 && len == str.Length())
	{
	  branchList.insert (str.Data());
	  ++ count;
	}
      }
      if (count == 0)
	RCU_THROW_MSG ("could not find any branch that matches pattern \"" + *iter + "\"");
    }
  }



  void NTupleSvc ::
  initOutput (const std::string& branchName)
  {
    // rationale: this ensures that I am not copying branches twice
    for (copyInfoMIter branch = m_copyInfo.begin(),
	   end = m_copyInfo.end(); branch != end; ++ branch)
    {
      if (branch->name == branchName)
	return;
    }

    CopyInfo info;
    info.name = branchName;
    info.source = wk()->tree()->FindBranch (branchName.c_str());
    if (info.source == 0)
      RCU_THROW_MSG ("could not find input branch: " + branchName);

    const char *className = info.source->GetClassName();
    if (strlen (className) > 0)
    {
      info.target = m_tree->Branch (branchName.c_str(), className,
				    static_cast<void*>(0));
    } else
    {
      static std::map<std::string,std::string> types;
      if (types.empty())
      {
	types["Char_t"] = "B";
	types["UChar_t"] = "b";
	types["Short_t"] = "S";
	types["UShort_t"] = "s";
	types["Int_t"] = "I";
	types["UInt_t"] = "i";
	types["Float_t"] = "F";
	types["Double_t"] = "D";
	types["Long64_t"] = "L";
	types["ULong64_t"] = "l";
	types["Bool_t"] = "O";
      }

      std::string leaves;
      TObject *object;
      for (TIter iter = info.source->GetListOfLeaves(); (object = iter()); )
      {
	TLeaf *myleaf = dynamic_cast<TLeaf*>(object);
	if (myleaf == 0)
	  RCU_THROW_MSG ("found non-leaf object in leaf list");

	std::string typeName = myleaf->GetTypeName();
	std::map<std::string,std::string>::const_iterator type
	  = types.find (typeName);
	if (type == types.end())
	  RCU_THROW_MSG ("unknown leaf type " + typeName);

	if (!leaves.empty())
	  leaves += ":";
	leaves = leaves + myleaf->GetTitle() + "/" + type->second;
      }
      if (leaves.empty())
	RCU_THROW_MSG ("failed to scan leaves of branch " + branchName);

      for (std::string::size_type pos = 0;
	   (pos = leaves.find ("[", pos)) != std::string::npos; )
      {
	++ pos;
	std::string::size_type pos2 = leaves.find ("]", pos);
	if (pos2 == std::string::npos)
	  RCU_THROW_MSG ("failed to scan leaf dimensions for " + leaves);
	std::string dim = leaves.substr (pos, pos2 - pos);
	if (!(dim[0] >= '0' && dim[0] <= '9'))
	  initOutput (dim);
      }
      info.target = m_tree->Branch (branchName.c_str(), static_cast<void*>(0),
				    leaves.c_str());
    }
    m_copyInfo.push_back (info);
  }



  void NTupleSvc ::
  initOutput (const std::set<std::string>& branchList)
  {
    for (std::set<std::string>::const_iterator branch = branchList.begin(),
	   end = branchList.end(); branch != end; ++ branch)
      initOutput (*branch);
  }



  void NTupleSvc ::
  copyInput ()
  {
    if (!m_connected)
    {
      for (copyInfoMIter info = m_copyInfo.begin(),
	     end = m_copyInfo.end(); info != end; ++ info)
      {
	info->source = dynamic_cast<TBranch*>
	  (wk()->tree()->GetBranch (info->name.c_str()));
	RCU_ASSERT2_SOFT (info->source != 0, ("source branch " + info->name + " not found for copying").c_str());
      }
      m_connected = true;
    }

    for (copyInfoMIter info = m_copyInfo.begin(),
	   end = m_copyInfo.end(); info != end; ++ info)
    {
      void *address = info->source->GetAddress();
      if (address == 0)
      {
	std::size_t size = sizeof (void*);
	TObject *object;
	for (TIter iter = info->source->GetListOfLeaves();
	     (object = iter());)
	{
	  TLeaf *myleaf = dynamic_cast<TLeaf*>(object);
	  RCU_ASSERT (myleaf != 0);
	  Int_t countval;
	  TLeaf *countleaf = myleaf->GetLeafCounter (countval);
	  if (countleaf)
	    countval = countleaf->GetMaximum();
	  if (countval < 0)
	    RCU_THROW_MSG (std::string ("could not determine size of leaf ") + myleaf->GetName() + " in branch " + info->name);
	  if (countval == 0)
	    countval = 1;
	  const std::size_t mysize
	    = myleaf->GetOffset() + myleaf->GetLenType() * countval;
	  if (size < mysize)
	    size = mysize;
	}
	info->buffer.resize (size, 0);
	info->source->SetStatus (1);
	info->source->SetAddress (address = &info->buffer[0]);
      }
      RCU_ASSERT (address != 0);
      info->target->SetAddress (address);
      info->source->GetEntry (wk()->treeEntry(), 1);
    }
  }
}
