/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SAMPLE_HANDLER__GRID_TOOLS_H
#define SAMPLE_HANDLER__GRID_TOOLS_H

//          Copyright Nils Krumnack 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@iastate.edu) for
// bug reports, feature suggestions, praise and complaints.



#include <SampleHandler/Global.h>

#include <AsgTools/MessageCheck.h>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace SH
{
  ANA_MSG_HEADER (msgGridTools)

  /// \brief return whether we have a valid VOMS proxy available
  /// \par Guarantee
  ///   basic
  /// \par Failures
  ///   grid utility failures
  bool checkVomsProxy ();


  /// \brief ensure that we have a valid VOMS proxy available
  ///
  /// First this checks whether we have a valid PROXY, and if not it
  /// sets up a new certificate.
  ///
  /// \par Guarantee
  ///   basic
  /// \par Failures
  ///   grid utility failures\n
  ///   failure to set up new VOMS proxy
  void ensureVomsProxy ();


  /// \brief list the FAX URLs for all the files in the dataset or
  /// dataset container matching the given filter (as glob expression)
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   grid utility failures
  /// \pre !name.empty()
  /// \pre name.find('*') == std::string::npos
  /// \pre !filter.empty()
  [[deprecated("please use rucioDirectAccessGlob instead")]]
  std::vector<std::string>
  faxListFilesGlob (const std::string& name, const std::string& filter);


  /// \brief list the FAX URLs for all the files in the dataset or
  /// dataset container matching the given filter (as regular
  /// expression)
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   grid utility failures
  /// \pre !name.empty()
  /// \pre name.find('*') == std::string::npos
  /// \pre !filter.empty()
  [[deprecated("please use rucioDirectAccessRegex instead")]]
  std::vector<std::string>
  faxListFilesRegex (const std::string& name, const std::string& filter);


  /// \brief list the rucio URLs for all the files in the dataset or
  /// dataset container matching the given filter (as glob expression)
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   grid utility failures
  /// \pre !name.empty()
  /// \pre name.find('*') == std::string::npos
  /// \pre !filter.empty()
  std::vector<std::string>
  rucioDirectAccessGlob (const std::string& name, const std::string& filter);


  /// \brief list the rucio URLs for all the files in the dataset or
  /// dataset container matching the given filter (as regular
  /// expression)
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   grid utility failures
  /// \pre !name.empty()
  /// \pre name.find('*') == std::string::npos
  /// \pre !filter.empty()
  std::vector<std::string>
  rucioDirectAccessRegex (const std::string& name, const std::string& filter);


  /// \brief one entry from the rucio-list-dids command
  struct RucioListDidsEntry
  {
    /// \brief the scope
    std::string scope;

    /// \brief the name
    std::string name;

    /// \brief the type
    std::string type;
  };

  /// \brief run rucio-list-dids for the given dataset
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   grid utility failures
  /// \pre !dataset.empty()
  std::vector<RucioListDidsEntry>
  rucioListDids (const std::string& dataset);



  /// \brief one entry from the rucio-list-file-replicas command
  struct RucioListFileReplicasEntry
  {
    /// \brief the scope
    std::string scope;

    /// \brief the name
    std::string name;

    /// \brief the file size
    std::string filesize;

    /// \brief the checksum
    std::string adler32;

    /// \brief the disk the replica is on
    std::string disk;

    /// \brief the replica
    std::string replica;
  };

  /// \brief run rucio-list-file-replicas for the given dataset
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   grid utility failures
  /// \pre !dataset.empty()
  std::vector<RucioListFileReplicasEntry>
  rucioListFileReplicas (const std::string& dataset);



  /// \brief run rucio-get-metadata for the given list of datasets
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   grid utility failures
  /// \pre !datasets.empty()
  /// \pre !dataset.empty() (for each dataset)
  std::map<std::string,std::unique_ptr<MetaObject> >
  rucioGetMetadata (const std::set<std::string>& datasets);


  /// \brief the result from \ref rucio_download
  struct RucioDownloadResult
  {
    std::string did;
    unsigned totalFiles = 0;
    unsigned downloadedFiles = 0;
    unsigned alreadyLocal = 0;
    unsigned notDownloaded = 0;
  };

  /// \brief run rucio-download
  /// \par Guarantee
  ///   basic
  /// \par Failures
  ///   grid utility failures
  RucioDownloadResult
  rucioDownload (const std::string& location,
                 const std::string& dataset);

  /// \brief run rucio-download with multiple datasets
  /// \par Guarantee
  ///   basic
  /// \par Failures
  ///   grid utility failures
  std::vector<RucioDownloadResult>
  rucioDownloadList (const std::string& location,
                     const std::vector<std::string>& datasets);
}

#endif
