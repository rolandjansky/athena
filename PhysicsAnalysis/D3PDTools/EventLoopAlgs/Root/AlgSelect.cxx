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

#include <EventLoopAlgs/AlgSelect.h>

#include <TH1.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/IWorker.h>
#include <EventLoopAlgs/NTupleSvc.h>
#include <MultiDraw/Formula.h>
#include <MultiDraw/FormulaSvc.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

ClassImp (EL::AlgSelect)

namespace EL
{
  void AlgSelect ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);

    RCU_INVARIANT (!m_outputStream.empty());
    for (std::size_t form = 0, end = m_cuts.size(); form != end; ++ form)
      RCU_INVARIANT (!m_cuts[form].empty());
    if (m_skim != 0)
    {
      RCU_INVARIANT (m_cuts.size() == m_index.size());
      RCU_INVARIANT (m_hist != 0);
      RCU_INVARIANT (m_formSvc != 0);
    }
    if (m_hist != 0)
    {
      RCU_INVARIANT (m_cuts.size() == std::size_t (m_hist->GetNbinsX()));
    }
  }



  AlgSelect ::
  AlgSelect ()
    : m_outputStream ("output"),
      m_hist (0), m_formSvc (0), m_skim (0)
  {
    RCU_NEW_INVARIANT (this);
  }



  AlgSelect ::
  AlgSelect (const std::string& val_outputStream, const std::string& cut)
    : m_outputStream (val_outputStream),
      m_hist (0), m_formSvc (0), m_skim (0)
  {
    RCU_ASSERT (!val_outputStream.empty());

    if (!cut.empty())
      m_cuts.push_back (cut);

    RCU_NEW_INVARIANT (this);
  }



  const std::string& AlgSelect ::
  outputStream () const
  {
    RCU_READ_INVARIANT (this);
    return m_outputStream;
  }



  const std::string& AlgSelect ::
  histName () const
  {
    RCU_READ_INVARIANT (this);
    return m_histName;
  }



  void AlgSelect ::
  histName (const std::string& val_histName)
  {
    RCU_CHANGE_INVARIANT (this);
    m_histName = val_histName;
  }



  void AlgSelect ::
  addCut (const std::string& cut)
  {
    RCU_CHANGE_INVARIANT (this);
    RCU_REQUIRE_SOFT (!cut.empty());
    m_cuts.push_back (cut);
  }



  StatusCode AlgSelect ::
  setupJob (Job& job)
  {
    RCU_CHANGE_INVARIANT (this);
    MD::useFormulas (job);
    return StatusCode::SUCCESS;
  }



  StatusCode AlgSelect ::
  initialize ()
  {
    RCU_CHANGE_INVARIANT (this);

    try
    {
      m_formSvc = MD::formulas (wk());
      if (!m_cuts.empty() && !m_histName.empty())
      {
	m_hist = new TH1D (m_histName.c_str(), 0, m_cuts.size(), 0, m_cuts.size());
	wk()->addOutput (m_hist);
      }
      for (std::size_t form = 0, end = m_cuts.size(); form != end; ++ form)
      {
	m_index.push_back (m_formSvc->addForm (m_cuts[form]));
	m_hist->GetXaxis()->SetBinLabel (1 + form, m_cuts[form].c_str());
      }

      m_skim = getNTupleSvc (wk(), m_outputStream);
    } catch (...)
    {
      m_index.clear ();
      throw;
    }
    return StatusCode::SUCCESS;
  }



  StatusCode AlgSelect ::
  execute ()
  {
    RCU_CHANGE_INVARIANT (this);

    RCU_ASSERT (m_hist != 0);

    double weight = 1;
    for (std::size_t form = 0; form != m_cuts.size() && weight != 0; ++ form)
    {
      double myweight = 1;
      if (m_index[form]) switch (m_index[form]->ndim())
      {
      case -1:
	RCU_THROW_MSG ("formula not valid: " + m_cuts[form]);
	break;
      case 0:
	myweight = m_index[form]->value (0);
	break;
      case 1:
	RCU_THROW_MSG ("formula is an array: " + m_cuts[form]);
	break;
      default:
	RCU_THROW_MSG ("unknown formula dimension: " + m_cuts[form]);
      }

      weight *= myweight;
      if (m_hist && weight != 0)
	m_hist->Fill (form);
    }

    if (weight)
      m_skim->setFilterPassed ();
    return StatusCode::SUCCESS;
  }
}
