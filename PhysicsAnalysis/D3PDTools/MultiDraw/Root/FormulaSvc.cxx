/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.

//
// includes
//

#include <MultiDraw/FormulaSvc.h>

#include <memory>
#include <sstream>
#include <TTree.h>
#include <TTreeFormula.h>
#include <TTreeFormulaManager.h>
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/IWorker.h>
#include <MultiDraw/Formula.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

ClassImp (MD::FormulaSvc)

namespace MD
{
  //
  // legacy implementations
  //

  const std::string FormulaSvc::name = "MultiDraw";



  std::string dbg (const FormulaSvc& /*obj*/, unsigned /*verbosity*/)
  {
    return "FormulaSvc";
  }



  void useFormulas (EL::Job& job)
  {
    if (!job.algsHas (FormulaSvc::name))
      job.algsAdd (new FormulaSvc);
  }



  FormulaSvc *formulas (EL::IWorker *worker)
  {
    RCU_REQUIRE_SOFT (worker != 0);

    FormulaSvc *const svc
      = dynamic_cast<FormulaSvc*>(worker->getAlg (FormulaSvc::name));
    if (svc == 0)
      RCU_THROW_MSG ("Job not configured for FormulaSvc support");
    return svc;
  }



  void FormulaSvc ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);

    for (std::size_t form = 0, end = m_vars.size(); form != end; ++ form)
    {
      RCU_INVARIANT (m_vars[form] != 0);
    }
  }



  FormulaSvc ::
  FormulaSvc ()
    : m_tree (0)
  {
    RCU_NEW_INVARIANT (this);
  }



  FormulaSvc ::
  ~FormulaSvc ()
  {
    RCU_DESTROY_INVARIANT (this);

    for (std::size_t form = 0, end = m_vars.size(); form != end; ++ form)
    {
      delete m_vars[form];
    }
  }



  const Formula *FormulaSvc ::
  addForm (const std::string& formula)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (!formula.empty());

    for (std::size_t form = 0, end = m_vars.size(); form != end; ++ form)
    {
      if (m_vars[form]->formula() == formula)
	return m_vars[form];
    }
    RCU_ASSERT (m_tree != 0);
    std::ostringstream name;
    name << "var" << m_vars.size();
    m_vars.push_back (new Formula (name.str(), formula, m_tree));
    return m_vars.back();
  }



  const char *FormulaSvc ::
  GetName () const
  {
    RCU_READ_INVARIANT (this);
    return name.c_str();
  }



  EL::StatusCode FormulaSvc ::
  changeInput (bool /*firstFile*/)
  {
    RCU_CHANGE_INVARIANT (this);

    m_tree = 0;

    try
    {
      m_tree = wk()->tree();
      m_tree->ResetBit (TTree::kForceRead);

      for (std::size_t form = 0, end = m_vars.size(); form != end; ++ form)
	m_vars[form]->reset (m_tree);
    } catch (...)
    {
      m_tree = 0;
      throw;
    }

    return EL::StatusCode::SUCCESS;
  }



  EL::StatusCode FormulaSvc ::
  execute ()
  {
    RCU_CHANGE_INVARIANT (this);
    if (wk()->treeEntry() != wk()->tree()->GetReadEvent())
      wk()->tree()->LoadTree (wk()->treeEntry());
    RCU_ASSERT (wk()->treeEntry() == wk()->tree()->GetReadEvent());
    return EL::StatusCode::SUCCESS;
  }
}
