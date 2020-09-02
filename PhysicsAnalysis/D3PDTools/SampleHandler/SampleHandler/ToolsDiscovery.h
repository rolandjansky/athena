/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER_TOOLS_DISCOVERY_H
#define SAMPLE_HANDLER_TOOLS_DISCOVERY_H

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


/// This module defines utility functions used for scanning the local
/// data storage.  The interface provided in this module is intended
/// for the general user.  The module is considered to be in the
/// pre-alpha stage.



//protect
#include <SampleHandler/Global.h>

#include <string>
#include <vector>

class TChain;

namespace SH
{
  /// effects: scan the given directory and add all subdirectories as
  ///   samples that contain root files.  if prefix is provided, use
  ///   that instead of dir when constructing the file names.
  /// guarantee: basic, only some samples might be added
  /// failures: low level errors IV
  /// failures: directory not found
  /// failures: duplicate samples
  /// rationale: the prefix option is for the Tier 3 prototype, where
  ///   you scan the local directory, but then access the files
  ///   through xrootd
  [[deprecated("please use new ScanDir() mechanism instead")]]
  void scanDir (SampleHandler& sh, DiskList& list,
		const std::string& pattern = "*.root*",
		const std::string& samplePattern = "*",
		const std::string& samplePostfix = "");
  [[deprecated("please use new ScanDir() mechanism instead")]]
  void scanDir (SampleHandler& sh, const std::string& dir);
  [[deprecated("please use new ScanDir() mechanism instead")]]
  void scanDir (SampleHandler& sh, const std::string& dir,
		const std::string& prefix);


  /// effects: scan the given directory tree and turn it into a single
  ///   sample of the given name
  /// guarantee: strong
  /// failures: out of memory III
  [[deprecated("please use new ScanDir() mechanism instead")]]
  void scanSingleDir (SampleHandler& sh, const std::string& name,
		      DiskList& list, const std::string& pattern = "*.root*");


  /// effects: scan the given directory tree and make a separate
  ///   sample for each file (using the file name before any "." as
  ///   the sample name).
  /// guarantee: basic, only some samples might be added
  /// failures: out of memory III
  /// failures: i/o errors
  /// failures: duplicate samples
  [[deprecated("please use new ScanDir() mechanism instead")]]
  void scanFiles (SampleHandler& sh, DiskList& list,
		  const std::string& pattern = "*.root*");


  /// effects: create a sample with the given name from the given
  ///   TChain object
  /// guarantee: strong
  /// failures: out of memory II
  Sample *makeFromTChain (const std::string& name, const TChain& chain);


  /// effects: make a list from DQ2 using the given pattern
  /// guarantee: basic, may add partially
  /// failures: out of memory III
  /// failures: dq2-ls errors
  [[deprecated("please use scanRucio instead")]]
  void scanDQ2 (SampleHandler& sh, const std::string& pattern);


  /// \brief make a list of grid datasets using the given pattern
  ///
  /// This will first look for dataset containers, and if none are
  /// found, it will instead look for datasets.  If those are also not
  /// found it will produce an error.
  ///
  /// For patterns that don't contain a wild-card (i.e. that can only
  /// match one dataset/container) it instead adds the dataset
  /// directly without querying rucio.  This is a performance
  /// optimization based on the assumption that you probably checked
  /// the dataset names beforehand, and that if you made a mistake it
  /// will pop up at a later stage anyways.  If you want to query
  /// rucio even in those cases set alwaysQuery to true.
  ///
  /// \par Guarantee
  ///   basic, may add some datasets
  /// \par Failures
  ///   no pattern match\n
  ///   rucio failures\n
  ///   out of memory III
  void scanRucio (SampleHandler& sh, const std::string& pattern,
		  bool alwaysQuery = false);


  /// effects: add a grid dataset for dataset ds
  /// guarantee: strong
  /// failures: out of memory II
  /// requires: ds.find ("*") == std::string::npos
  void addGrid (SampleHandler& sh, const std::string& ds);


  /// effects: add a combined grid dataset with name dsName for dataset list dsList
  /// guarantee: strong
  /// failures: out of memory II
  /// requires: for each dataset ds: ds.find ("*") == std::string::npos
  void addGridCombined (SampleHandler& sh, const std::string& dsName,
                        const std::vector<std::string>& dsList);


  /// effects: add a combined grid dataset with name dsName for dataset list file dsFile
  /// guarantee: strong
  /// failures: out of memory II
  /// requires: for each dataset ds: ds.find ("*") == std::string::npos
  void addGridCombinedFromFile (SampleHandler& sh, const std::string& dsName,
                                const std::string& dsFile);


  /// effects: update all grid samples in the sample handler that are
  ///   located on the given disk to be opened directly.  for that it
  ///   replaces the from part of the file with the to part.  if
  ///   allow_partial is set, it allows for partial datasets (meaning
  ///   you may lose some files that are not there).
  /// guarantee: basic, may convert only some samples
  /// failures: out of memory III
  /// failures: dq2-ls errors
  /// failures: from not part of file name
  void makeGridDirect (SampleHandler& sh, const std::string& disk,
		       const std::string& from, const std::string& to,
		       bool allow_partial);


  /// effects: scan for trees in the given sample (or sample handler),
  ///   and create a separate sample for each tree.  if pattern is
  ///   specified it is applied to the allowed tree names.
  /// guarantee: strong
  /// failures: out of memory III
  /// failures: i/o errors
  void scanForTrees (SampleHandler& sh, const Sample& sample,
		     const std::string& pattern = ".*");
  void scanForTrees (SampleHandler& sh, const std::string& pattern = ".*");


  /// effects: read a file list from a text file
  /// guarantee: strong
  /// failures: out of memory III
  /// failures: i/o errors
  void readFileList (SampleHandler& sh, const std::string& name,
		     const std::string& file);
}

#endif
