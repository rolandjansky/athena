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

#include <MultiDraw/AlgCFlow.h>

#include <memory>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/IWorker.h>
#include <MultiDraw/Formula.h>
#include <MultiDraw/FormulaSvc.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

ClassImp (MD::AlgCFlow)

namespace MD
{
  void AlgCFlow ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
    if (m_hist != 0)
    {
      RCU_INVARIANT (m_hist->GetDirectory() == 0);
      RCU_INVARIANT (m_hist->GetDimension() == 1);
      RCU_INVARIANT (m_formulas.size() == std::size_t (m_hist->GetNbinsX()));
      RCU_INVARIANT (m_formulas.size() == m_values.size());
      RCU_INVARIANT (m_formulas.size() == m_back.size());
      RCU_INVARIANT (m_formulas.size() == m_axis.size());
    }
    if (m_hist2)
    {
      RCU_INVARIANT (m_index.size() == m_formulas.size());
      RCU_INVARIANT (m_formSvc != 0);
    } else
      RCU_INVARIANT (m_index.empty());
  }



  AlgCFlow ::
  AlgCFlow ()
    : m_hist (0), m_hist2 (0), m_formSvc (0)
  {
    RCU_NEW_INVARIANT (this);
  }



  AlgCFlow ::
  AlgCFlow (TH1 *val_hist_swallow)
    : m_hist (0), m_hist2 (0)
  {
    std::auto_ptr<TH1> hist (val_hist_swallow);

    RCU_REQUIRE_SOFT (val_hist_swallow != 0);
    RCU_REQUIRE_SOFT (val_hist_swallow->GetDimension() == 1);

    for (std::size_t form = 0, end = hist->GetNbinsX();
	 form != end; ++ form)
    {
      const std::string label = hist->GetXaxis()->GetBinLabel(form+1);
      const std::size_t split = label.find (':');

      m_axis.push_back (hist->GetXaxis()->GetBinCenter (form+1));
      if (split == std::string::npos)
      {
	m_formulas.push_back (label);
	m_back.push_back (form - 1);
      } else
      {
	m_formulas.push_back (label.substr (0, split));
	const std::string back = label.substr (split+1);
	m_back.push_back (form);

	if (!back.empty())
	{
	  for (std::size_t form2 = 0; form2 != form; ++ form2)
	  {
	    if (m_formulas[form2] == back)
	      m_back.back() = form2;
	  }
	  if (m_back.back() == form)
	    RCU_THROW_MSG ("unknown back formula: " + back);
	}
      }
    }

    m_hist = hist.get();
    m_hist->SetDirectory (0);

    m_values.resize (m_formulas.size());

    hist.release ();
    RCU_NEW_INVARIANT (this);
  }



  EL::StatusCode AlgCFlow ::
  setupJob (EL::Job& job)
  {
    RCU_CHANGE_INVARIANT (this);
    useFormulas (job);
    return EL::StatusCode::SUCCESS;
  }



  EL::StatusCode AlgCFlow ::
  initialize ()
  {
    RCU_CHANGE_INVARIANT (this);

    try
    {
      m_formSvc = formulas (wk());
      for (std::size_t form = 0, end = m_formulas.size(); form != end; ++ form)
      {
	if (!m_formulas[form].empty())
	  m_index.push_back (m_formSvc->addForm (m_formulas[form]));
	else
	  m_index.push_back (0);
      }
      m_hist2 = dynamic_cast<TH1*>(m_hist->Clone ());
      RCU_ASSERT (m_hist2 != 0);
      wk()->addOutput (m_hist2);
    } catch (...)
    {
      m_index.clear ();
      throw;
    }
    return EL::StatusCode::SUCCESS;
  }



  EL::StatusCode AlgCFlow ::
  execute ()
  {
    RCU_CHANGE_INVARIANT (this);

    RCU_ASSERT (m_hist2 != 0);

    int ndim = 0;
    std::size_t size = std::size_t (-1);
    for (std::size_t form = 0; form != m_formulas.size(); ++ form)
    {
      if (m_index[form]) switch (m_index[form]->ndim())
      {
      case -1:
	RCU_THROW_MSG ("formula not valid: " + m_formulas[form]);
	break;
      case 0:
	if (m_index[form]->ndata() > 0)
	  m_values[form] = m_index[form]->value (0);
	else
	  size = 0;
	break;
      case 1:
	ndim = 1;
	if (size > m_index[form]->ndata())
	  size = m_index[form]->ndata();
	break;
      default:
	RCU_THROW_MSG ("unknown formula dimension: " + m_formulas[form]);
      }
    }

    if (ndim == 0 && size > 1)
      size = 1;
    for (std::size_t iter = 0; iter != size; ++ iter)
    {
      for (std::size_t form = 0, end = m_formulas.size(); form != end; ++ form)
      {
	double weight = 1;

	if (m_back[form] < form)
	  weight = m_values[m_back[form]];

	if (weight != 0 && !m_formulas[form].empty())
	{
	  if (m_index[form]->ndim() == 1)
	    weight *= m_index[form]->value (iter);
	  else
	    weight *= m_index[form]->value (0);
	}

	m_values[form] = weight;
	m_hist2->Fill (m_axis[form], m_values[form]);
      }
    }
    return EL::StatusCode::SUCCESS;
  }
}
