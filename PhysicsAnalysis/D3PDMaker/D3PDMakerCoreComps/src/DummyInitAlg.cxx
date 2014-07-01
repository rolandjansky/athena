/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DummyInitAlg.cxx 486128 2012-03-02 08:54:20Z krasznaa $
/**
 * @file D3PDMakerCoreComps/src/DummyInitAlg.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Work around initialization ordering problems.
 */


#include "DummyInitAlg.h"
#include "AthenaKernel/errorcheck.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TError.h"


namespace {


class DirectoryRestore
  : public TDirectory::TContext
{
public:
  DirectoryRestore();
  ~DirectoryRestore();

private:
  TFile* m_file;
  int m_ge;
};


DirectoryRestore::DirectoryRestore()
  : TDirectory::TContext (0),
    m_file (gFile),
    m_ge (gErrorIgnoreLevel)
{
}


DirectoryRestore::~DirectoryRestore()
{
  gFile = m_file;
  gErrorIgnoreLevel = m_ge;
}


}


namespace D3PD {


DummyInitAlg::DummyInitAlg (const std::string& name,
                            ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_histSvc ("THistSvc", name)
{
  declareProperty ("HistSvc",        m_histSvc);
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode DummyInitAlg::initialize()
{
  CHECK( AthAlgorithm::initialize() );

  // THistSvc has a bug whereby if it opens output files,
  // then it leaves the root directory pointing at the last output
  // file when initialize returns.  This means that other root
  // objects that are meant to be transient can end up in this file.
  // Worse, we can get a double-delete, as these transient objects
  // will then be deleted when the file is closed.
  //
  // We do this here in the hope that we're the first to call THistSvc.
  // Maybe we should also bash the current directory back to TROOT?
  //
  // This can be removed once this is fixed in THistSvc.
  // https://savannah.cern.ch/bugs/index.php?60162
  {
    DirectoryRestore save;
    CHECK( m_histSvc.retrieve() );
  }

  CHECK( detStore().retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi @c execute method.
 */
StatusCode DummyInitAlg::execute()
{
  return StatusCode::SUCCESS;
}


} // namespace D3PD
