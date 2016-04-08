/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerRoot/src/RootD3PDSvc.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2009
 * @brief Service to create a Root-based D3PD tree.
 */


#include "RootD3PDSvc.h"
#include "RootD3PD.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/make_unique.h"
#include "TTree.h"
#include "TROOT.h"
#include "TFile.h"
#include "TCollection.h"
#include <cstdlib>


namespace D3PD {


/**
 * @brief Constructor.
 * @param name Service name.
 * @param svcloc Service locator.
 */
RootD3PDSvc::RootD3PDSvc (const std::string& name,
                          ISvcLocator* svcloc)
  : base_class (name, svcloc),
    m_histSvc ("THistSvc", name)
{
  // See comments on cleanup().
  std::atexit (cleanup);

  declareProperty ("HistSvc",        m_histSvc);
  declareProperty ("DoBranchRef",    m_doBranchRef = true);
  declareProperty ("MasterTree",     m_masterTree = "CollectionTree");
  declareProperty ("IndexMajor",     m_indexMajor = "RunNumber");
  declareProperty ("IndexMinor",     m_indexMinor = "EventNumber");
  declareProperty ("BasketSize",     m_basketSize = 32768);
  declareProperty ("EntryOffsetLen", m_entryOffsetLen = 512);
  declareProperty ("AutoFlush",      m_autoFlush = -1,
                   "Value to set for ROOT's AutoFlush parameter. "
                   "(Tells how often the tree baskets will be flushed.) "
                   "0 disables flushing. "
                   "-1 (default) makes no changes to what THistSvc did. "
                   "Any other negative number gives the number of bytes "
                   "after which to flush. "
                   "A positive number gives the number of entries after which "
                   "to flush.");
  declareProperty ("AllowedNames",   m_allowedNames,
                   "Variable names allowed in the output D3PDs. An empty list "
                   "means that all variable names are allowed. Regular expressions "
                   "are allowed.");
  declareProperty ("VetoedNames",    m_vetoedNames,
                   "Variable names that are not allowed to end up in the "
                   "created D3PDs. Regular expressions are allowed.");
}


/**
 * @brief Make sure all files are closed before exiting, to prevent crashes.
 *
 * Root files are normally closed by THistSvc::finalize().
 *
 * However, if we get an error during initialization, then the program
 * will exit without running the finalize() methods.  In that case,
 * the root files will get closed when global destructors are run.
 * But by that time, some of the root objects needed to perform
 * the close may have already been deleted, leading to a crash.
 *
 * To avoid this, we register this method with atexit();
 * we just loop through all existing root files and try to close them all.
 */
void RootD3PDSvc::cleanup ()
{
  // Sometimes gDirectory is invalid at this point...
  gDirectory = gROOT;
  TIter it (gROOT->GetListOfFiles());
  while (TObject* o = it.Next()) {
    if (TFile* f = dynamic_cast<TFile*> (o))
      f->Close();
  }
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode RootD3PDSvc::initialize()
{
  CHECK( AthService::initialize() );
  CHECK( m_histSvc.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi finalize method.
 */
StatusCode RootD3PDSvc::finalize()
{
  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi stop method.
 */
StatusCode RootD3PDSvc::stop()
{
  // Run through all the trees we've made.
  for (size_t i = 0; i < m_d3pds.size(); i++) {
    RootD3PD* d3pd = m_d3pds[i].get();

    // Make an index if requested.
    if (!m_indexMajor.empty())
      d3pd->tree()->BuildIndex (m_indexMajor.c_str(), m_indexMinor.c_str());

    // Was there a master tree specified?
    if (!d3pd->master().empty()) {
      // Yes --- try to find it
      TDirectory* dir = d3pd->tree()->GetDirectory();
      TTree* master =
        dynamic_cast<TTree*> (dir->Get (d3pd->master().c_str()));
      if (!master && d3pd->tree()->GetEntries() > 0)
        CHECK( m_histSvc->getTree (d3pd->master(), master) );
      if (master) {
        // Make an index for the master if needed.
        if (!master->GetTreeIndex())
          // AANTupleStream will leave branch addresses in the master
          // tree pointing at dead objects.
          master->ResetBranchAddresses();

          master->BuildIndex (m_indexMajor.c_str(), m_indexMinor.c_str());

        // Make this tree a friend of the master.
        master->AddFriend (d3pd->tree());
      }
    }

    {
      TDirectory::TContext ctx (gDirectory, d3pd->tree()->GetDirectory());
      d3pd->tree()->Write();
    }
  }

  // Get rid of the RootD3PD wrappers.
  // (Doesn't delete the root trees themselves.)
  m_d3pds.clear();

  return StatusCode::SUCCESS;
}


/**
 * @brief Create a new D3PD tree.
 * @param name The name of the new tree.
 *             If the name contains a slash, it is interpreted
 *             as STREAM/NAME.  If the stream name is of the form
 *             `pool:FILE', then this is a request to put the new tree
 *             in the given pool file.  If the the stream name otherwise
 *             contains a colon,
 *             then the part of the name before the colon is the
 *             `parent' stream name; this is used to locate the
 *             master tree.
 * @param d3pd[out] The created tree.
 */
StatusCode RootD3PDSvc::make (const std::string& name, ID3PD* & d3pd)
{
  std::string tname = name;
  std::string::size_type ipos = name.rfind ('/');
  std::string master = m_masterTree;
  std::string poolfile;
  if (ipos != std::string::npos) {
    tname = name.substr (ipos+1);
    if (!master.empty()) {
      std::string sname = name.substr (0, ipos+1);
      std::string::size_type jpos = sname.find (':');
      if (sname.substr (0, jpos) == "pool" ||
          sname.substr (0, jpos) == "/pool")
      {
        poolfile = sname.substr (jpos+1, std::string::npos);
        if (!poolfile.empty() && poolfile[poolfile.size()-1] == '/')
          poolfile.erase (poolfile.size()-1);
      }
      else {
        if (jpos != std::string::npos)
          sname = sname.substr (0, jpos) + sname[sname.size()-1];
        master = sname + master;
      }
    }
  }
  TTree* tree = new TTree (tname.c_str(), tname.c_str());

  if (m_doBranchRef)
    tree->BranchRef();
  if (m_autoFlush != -1)
    tree->SetAutoFlush (m_autoFlush);
  auto rd3pd = CxxUtils::make_unique<RootD3PD>
    (tree, master,
     m_allowedNames, m_vetoedNames,
     m_basketSize, m_entryOffsetLen);

  if (!poolfile.empty())
    rd3pd->setPoolFile (poolfile);
  else
    CHECK( m_histSvc->regTree (name, tree) );

  d3pd = rd3pd.get();
  m_d3pds.push_back (std::move (rd3pd));
  return StatusCode::SUCCESS;
}


} // namespace D3PD
