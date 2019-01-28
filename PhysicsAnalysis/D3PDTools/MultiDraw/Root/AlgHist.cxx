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

#include <MultiDraw/AlgHist.h>

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

ClassImp (MD::AlgHist)

namespace MD
{
  void AlgHist ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
    if (m_hist != 0)
    {
      RCU_INVARIANT (m_hist->GetDirectory() == 0);
      RCU_INVARIANT (m_formulas.size() == m_valnum + m_cut);
      RCU_INVARIANT (m_values.size() == m_valnum + 1);
    }
    switch (m_type)
    {
    case -1:
      RCU_INVARIANT (m_hist == 0);
      break;
    case 0:
      RCU_INVARIANT (m_hist != 0);
      RCU_INVARIANT (m_valnum == 1);
      break;
    case 1:
      RCU_INVARIANT (dynamic_cast<TProfile*>(m_hist) != 0);
      RCU_INVARIANT (m_valnum == 2);
      break;
    case 2:
      RCU_INVARIANT (dynamic_cast<TH2*>(m_hist) != 0);
      RCU_INVARIANT (m_valnum == 2);
      break;
    case 3:
      RCU_INVARIANT (dynamic_cast<TH3*>(m_hist) != 0);
      RCU_INVARIANT (m_valnum == 3);
      break;
    default:
      RCU_INVARIANT0 ("invalid type");
    }
    for (std::size_t form = 0, end = m_formulas.size(); form != end; ++ form)
      RCU_INVARIANT (!m_formulas[form].empty());
    if (m_hist2)
    {
      RCU_INVARIANT (m_index.size() == m_formulas.size());
      RCU_INVARIANT (m_formSvc != 0);
    } else
      RCU_INVARIANT (m_index.empty());
  }



  AlgHist ::
  AlgHist ()
    : m_hist (0), m_type (-1), m_valnum (1), m_cut (false), m_hist2 (0)
  {
    RCU_NEW_INVARIANT (this);
  }



  AlgHist ::
  AlgHist (TH1 *val_hist_swallow,
	  const std::string& val_value0,
	  const std::string& val_value1,
	  const std::string& val_value2,
	  const std::string& val_value3)
    : m_hist (0), m_type (-1), m_valnum (1), m_cut (false), m_hist2 (0)
  {
    std::auto_ptr<TH1> hist (val_hist_swallow);

    RCU_REQUIRE_SOFT (val_hist_swallow != 0);
    RCU_REQUIRE_SOFT (!val_value0.empty());

    m_formulas.push_back (val_value0);
    if (!val_value1.empty())
      m_formulas.push_back (val_value1);
    if (!val_value2.empty())
      m_formulas.push_back (val_value2);
    if (!val_value3.empty())
      m_formulas.push_back (val_value3);

    m_hist = hist.get();
    m_hist->SetDirectory (0);
    if (dynamic_cast<TProfile*>(m_hist) != 0)
    {
      m_type = 1;
      m_valnum = 2;
    } else if (dynamic_cast<TH2*>(m_hist) != 0)
    {
      m_type = 2;
      m_valnum = 2;
    } else if (dynamic_cast<TH3*>(m_hist) != 0)
    {
      m_type = 3;
      m_valnum = 3;
    } else
    {
      m_type = 0;
      m_valnum = 1;
    }
    m_values.resize (m_valnum + 1, 1);
    if (m_valnum == m_formulas.size())
      m_cut = false;
    else if (m_valnum+1 == m_formulas.size())
      m_cut = true;
    else
      RCU_THROW_MSG ("invalid number of formulas");

    hist.release ();
    RCU_NEW_INVARIANT (this);
  }


  AlgHist ::
  ~AlgHist ()
  {
    if (m_hist != nullptr) {
      delete m_hist;
    }
  }

  

  EL::StatusCode AlgHist ::
  setupJob (EL::Job& job)
  {
    RCU_CHANGE_INVARIANT (this);
    useFormulas (job);
    return EL::StatusCode::SUCCESS;
  }



  EL::StatusCode AlgHist ::
  initialize ()
  {
    RCU_CHANGE_INVARIANT (this);

    try
    {
      m_formSvc = formulas (wk());
      for (std::size_t form = 0, end = m_formulas.size(); form != end; ++ form)
	m_index.push_back (m_formSvc->addForm (m_formulas[form]));
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



  EL::StatusCode AlgHist ::
  execute ()
  {
    RCU_CHANGE_INVARIANT (this);

    RCU_ASSERT (m_formulas.size() == m_index.size());
    RCU_ASSERT (m_formulas.size() <= m_values.size());

    if (m_cut && m_index.back()->ndim() == 0 &&
	(m_values.back() = m_index.back()->value (0)) == 0)
      return EL::StatusCode::SUCCESS;

    int ndim = 0;
    std::size_t size = std::size_t (-1);
    for (std::size_t form = 0; form != m_valnum; ++ form)
    {
      switch (m_index[form]->ndim())
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
      if (ndim == 1)
      {
	if (m_cut && m_index.back()->ndim() == 1 &&
	    (m_values.back() = m_index.back()->value (iter)) == 0)
	  continue;

	for (std::size_t form = 0; form != m_valnum; ++ form)
	{
	  if (m_index[form]->ndim() == 1)
	    m_values[form] = m_index[form]->value (iter);
	}
      }

      switch (m_type)
      {
      case 0:
	m_hist2->Fill (m_values[0], m_values[1]);
	break;
      case 1:
	static_cast<TProfile*>(m_hist2)->Fill (m_values[0], m_values[1], m_values[2]);
	break;
      case 2:
	static_cast<TH2*>(m_hist2)->Fill (m_values[0], m_values[1], m_values[2]);
	break;
      case 3:
	static_cast<TH3*>(m_hist2)->Fill (m_values[0], m_values[1], m_values[2], m_values[3]);
	break;
      default:
	RCU_THROW_MSG ("invalid number of values");
      }
    }
    return EL::StatusCode::SUCCESS;
  }
}
