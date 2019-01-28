/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011 - 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.

//
// includes
//

#include <MultiDraw/Formula.h>

#include <memory>
#include <sstream>
#include <TTree.h>
#include <TTreeFormula.h>
#include <TTreeFormulaManager.h>
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/IWorker.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

namespace MD
{
  void Formula ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
    if (!m_formula.empty())
    {
      RCU_INVARIANT (!m_name.empty());

      if (m_tree != 0)
      {
	RCU_INVARIANT (m_manager != 0);
	if (m_ndata > 0)
	{
	  RCU_INVARIANT (m_cache.size() >= std::size_t (m_ndata));
	  RCU_INVARIANT (m_read.size() >= std::size_t (m_ndata));
	}
      } else
      {
	RCU_INVARIANT (m_form == 0);
	RCU_INVARIANT (m_manager == 0);
      }
    } else
    {
      RCU_INVARIANT (m_name.empty());
      RCU_INVARIANT (m_tree == 0);
      RCU_INVARIANT (m_form == 0);
      RCU_INVARIANT (m_manager == 0);
    }
  }



  Formula ::
  Formula ()
    : m_tree (0), m_form (0), m_manager (0), m_ndim (-1),
      m_entry (-1), m_ndata (0)
  {
    RCU_NEW_INVARIANT (this);
  }



  Formula ::
  Formula (const std::string& name, const std::string& formula, TTree *tree)
    : m_formula (formula), m_name (name),
      m_tree (0), m_form (0), m_manager (0), m_ndim (-1),
      m_entry (-1), m_ndata (0)
  {
    RCU_NEW_INVARIANT (this);

    reset (tree);
  }



  Formula ::
  ~Formula ()
  {
    RCU_DESTROY_INVARIANT (this);

    delete m_form;
    m_form = 0;
  }



  void Formula ::
  reset (TTree *tree)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE (!formula().empty());

    delete m_form;
    m_form = 0;
    m_manager = 0;
    m_ndim = -1;
    m_entry = -1;
    m_ndata = 0;
    m_tree = 0;

    m_tree = tree;
    std::auto_ptr<TTreeFormula> myform
      (m_form = new TTreeFormula (m_name.c_str(), m_formula.c_str(), tree));

    m_form->SetQuickLoad (kTRUE);
    m_manager = new TTreeFormulaManager;
    m_manager->Add (m_form);
    m_manager->Sync ();
    if (m_manager->GetMultiplicity () == -1)
      tree->SetBit (TTree::kForceRead);
    if (m_form->GetNdim() <= 0)
      m_ndim = -1;
    else if (m_form->GetMultiplicity() == 0)
      m_ndim = 0;
    else if (m_manager->GetMultiplicity() == 1 && m_form->GetMultiplicity() == 1)
      m_ndim = 1;
    else if (m_manager->GetMultiplicity() == -1 && m_form->GetMultiplicity() == 1)
      m_ndim = 0;

    myform.release();
  }



  const std::string& Formula ::
  formula () const
  {
    RCU_READ_INVARIANT (this);
    return m_formula;
  }



  bool Formula ::
  valid () const
  {
    RCU_READ_INVARIANT (this);
    return !m_formula.empty() && m_ndim >= 0;
  }



  int Formula ::
  ndim () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE_SOFT (valid ());
    return m_ndim;
  }



  std::size_t Formula ::
  ndata () const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE_SOFT (valid ());

    if (m_ndim < 0)
      RCU_THROW_MSG ("invalid formula: " + m_formula);
    if (m_tree->GetReadEntry() != m_entry)
    {
      m_entry = -1;
      m_ndata = m_manager->GetNdata();
      if (m_ndata > 0)
      {
	if (m_read.size() < std::size_t (m_ndata))
	  m_read.resize (m_ndata);
	for (std::size_t iter = 0, end = m_ndata; iter != end; ++ iter)
	  m_read[iter] = false;

	if (m_cache.size() < std::size_t (m_ndata))
	  m_cache.resize (m_ndata);
	m_cache[0] = m_form->EvalInstance (0);
	m_read[0] = true;
	m_entry = m_tree->GetReadEntry();
      }
    }
    if (m_ndata < 0)
      RCU_THROW_MSG ("failed to read formula: " + m_formula);
    return m_ndata;
  }



  Double_t Formula ::
  value (std::size_t data) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE_SOFT (valid ());
    RCU_REQUIRE_SOFT (data < ndata ());

    if (!m_read[data])
    {
      m_cache[data] = m_form->EvalInstance (data);
      m_read[data] = true;
    }
    return m_cache[data];
  }
}
