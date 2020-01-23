/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTesting/UnitTest.h>
#include <EventLoop/OutputStreamData.h>
#include <TH1F.h>
#include <TKey.h>
#include <TTree.h>

//
// unit test
//

namespace EL
{
  namespace Detail
  {
    void fillTree (TTree *tree)
    {
      ASSERT_NE (nullptr, tree);
      for (unsigned iter = 0; iter != 1000; iter += 1)
      {
        tree->Fill ();
      }
    }

    TEST (OutputStreamDataTest, objectAddTests)
    {
      std::string fileName {"OutputStreamDataTest-objectAddTests.root"};
      std::set<std::string> expectedNames;
      std::set<std::string> treeNames;
      {
        OutputStreamData output {fileName, "RECREATE"};

        {
          TH1 *hist = new TH1F ("userHist", "user histogram", 10, 0, 10);
          hist->SetDirectory (output.file());
          expectedNames.insert ("userHist");
        }

        {
          TTree *tree = new TTree ("userTree", "user tree");
          tree->SetDirectory (output.file());
          fillTree (tree);
          expectedNames.insert ("userTree");
          treeNames.insert ("userTree");
        }

        output.addOutput (std::make_unique<TTree> ("directTree", "direct tree"));
        expectedNames.insert ("directTree");
        fillTree (output.getOutputTree ("directTree"));
        treeNames.insert ("directTree");

        output.addOutput (std::make_unique<TH1F> ("directHist", "direct histogram", 10, 0, 10));
        EXPECT_NE (nullptr, output.getOutputHist ("directHist"));
        expectedNames.insert ("directHist");

        output.addOutput (std::make_unique<TNamed> ("directNamed", "direct named"));
        expectedNames.insert ("directNamed");

        output.addClone (TTree ("cloneTree", "clone tree"));
        expectedNames.insert ("cloneTree");
        fillTree (output.getOutputTree ("cloneTree"));
        treeNames.insert ("cloneTree");

        output.addClone (TH1F ("cloneHist", "clone histogram", 10, 0, 10));
        EXPECT_NE (nullptr, output.getOutputHist ("cloneHist"));
        expectedNames.insert ("cloneHist");

        output.addClone (TNamed ("cloneNamed", "clone named"));
        expectedNames.insert ("cloneNamed");

        // just test that saving multiple times doesn't create
        // multiple copies
        output.saveOutput ();
        output.saveOutput ();
        output.close ();
      }

      std::set<std::string> knownNames;
      std::unique_ptr<TFile> file (TFile::Open (fileName.c_str(), "READ"));
      TIter iterator (file->GetListOfKeys());
      TKey *key {};
      while ((key = dynamic_cast<TKey*>(iterator())))
      {
        SCOPED_TRACE(key->GetName());
        EXPECT_EQ (key->GetCycle(), 1);
        EXPECT_TRUE (knownNames.find (key->GetName()) == knownNames.end());
        knownNames.insert (key->GetName());
      }
      for (const std::string& expectedName : expectedNames)
      {
        SCOPED_TRACE (expectedName);
        EXPECT_TRUE (knownNames.find (expectedName) != knownNames.end());
      }
      for (const std::string& treeName : treeNames)
      {
        SCOPED_TRACE (treeName);
        TTree *tree = dynamic_cast<TTree*> (file->Get (treeName.c_str()));
        ASSERT_NE (nullptr, tree);
        EXPECT_EQ (1000, tree->GetEntries());
      }
    }

    TEST (OutputStreamDataTest, directoryTest)
    {
      std::string fileName {"OutputStreamDataTest-directoryTest.root"};

      {
        OutputStreamData output {fileName, "RECREATE"};
        output.addClone (TNamed ("named", "named"));
        output.addClone (TNamed ("dir1/named", "named"));
        output.addClone (TNamed ("dir1/dir2/named", "named"));
        output.saveOutput ();
        output.close ();
      }

      std::unique_ptr<TFile> file (TFile::Open (fileName.c_str(), "READ"));
      TDirectory *dir1 {nullptr}, *dir2 {nullptr};
      EXPECT_NE (nullptr, file->Get ("named"));
      EXPECT_NE (nullptr, file->Get ("dir1/named"));
      EXPECT_NE (nullptr, file->Get ("dir1/dir2/named"));
      ASSERT_NE (nullptr, dir1 = dynamic_cast<TDirectory*>(file->Get ("dir1")));
      EXPECT_NE (nullptr, dir1->Get ("named"));
      ASSERT_NE (nullptr, dir2 = dynamic_cast<TDirectory*>(dir1->Get ("dir2")));
      EXPECT_NE (nullptr, dir2->Get ("named"));
      EXPECT_EQ (dir2, file->Get ("dir1/dir2"));
    }
  }
}

ATLAS_GOOGLE_TEST_MAIN
