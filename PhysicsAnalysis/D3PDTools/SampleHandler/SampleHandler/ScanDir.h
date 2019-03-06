/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef SAMPLE_HANDLER__SCAN_DIR_H
#define SAMPLE_HANDLER__SCAN_DIR_H


#include <SampleHandler/Global.h>

#include <RootCoreUtils/StringUtil.h>
#include <map>
#include <vector>

namespace SH
{
  /// \brief the class used for scanning local directories and file
  /// servers for samples
  ///
  /// Originally these was a series of stand-alone function calls, but
  /// people kept asking for more and more options, making it unwieldy
  /// to call and to maintain.  Instead we now have a single class
  /// containing all the possible parameters, which makes it easier to
  /// configure and extend.
  ///
  /// The member functions all return *this, so that usage like this
  /// is possible:
  /// \code
  ///   ScanDir()
  ///     .filePattern ("*.root*")
  ///     .scan (sh, "/data");
  /// \endcode

  struct ScanDir
  {
    //
    // public interface
    //

    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    ScanDir ();


    /// \brief scan the given directory and put the created samples
    ///   into the sample handler
    /// \return *this
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory III\n
    ///   i/o errors\n
    ///   duplicate samples
  public:
    const ScanDir& scan (SampleHandler& sh, const std::string& dir) const;


    /// \brief scan the given directory in EOS and put the created
    ///   samples into the sample handler
    /// \return *this
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory III\n
    ///   i/o errors\n
    ///   duplicate samples
  public:
    const ScanDir& scanEOS (SampleHandler& sh, const std::string& eosDir) const;


    /// \copydoc scan
  public:
    const ScanDir& scan (SampleHandler& sh, DiskList& list) const;


    /// \brief the index of the file hierarchy at which we gather the
    /// sample name.
    ///
    /// this is positive when it starts counting from the top, and
    /// negative when it starts from the back, i.e. -1 uses the file
    /// name, 0 denotes the directory inside the top level directory
  public:
    ScanDir& sampleDepth (int val_sampleDepth);

    /// \brief the index of the file hierarchy at which we gather the
    /// sample name.
    ///
    /// this differs from \ref sampleDepth in that negative numbers
    /// count up in the directory hierarchy from the top of where we
    /// scan, while \ref sampleDepth starts counting from the back if
    /// the number is negative.
  public:
    ScanDir& absSampleDepth (int val_absSampleDepth);

    /// \brief a single sample name into which all found files should
    /// be placed.  if set, this overrides all other naming methods.
  public:
    ScanDir& sampleName (const std::string& val_sampleName);

    /// \brief the minimum depth for files to make it into the sample
  public:
    ScanDir& minDepth (std::size_t val_minDepth);

    /// \brief the maximum depth for files to make it into the sample
  public:
    ScanDir& maxDepth (std::size_t val_maxDepth);

    /// \brief the pattern for files to be accepted
    /// \sa filePatternRegex
  public:
    ScanDir& filePattern (const std::string& val_filePattern);

    /// \brief the regular expression for files to be accepted
    /// \sa filePattern
  public:
    ScanDir& fileRegex (const std::string& val_fileRegex);

    /// \brief the pattern for samples to be accepted
  public:
    ScanDir& samplePattern (const std::string& val_samplePattern);

    /// \brief the pattern for the postfix to be stripped from the
    /// sampleName
  public:
    ScanDir& samplePostfix (const std::string& val_samplePostfix);

    /// \brief rename any sample matching pattern to name
  public:
    ScanDir& sampleRename (const std::string& pattern, const std::string& name);

    /// \brief attach an extra name component to the sample based on a
    /// second component of the path
    /// \pre val_relSampleDepth != 0
  public:
    ScanDir& extraNameComponent (int val_relSampleDepth);



    //
    // private interface
    //

    /// \brief if this is negative it is the depth at which we take
    /// the sample name, counting from the end
  private:
    int m_relSampleDepth;

    /// \brief if \ref m_relSampleDepth is not negative, it is the
    /// depth at which we take the sample name, counting from the
    /// first directory scanned
  private:
    int m_absSampleDepth;

    /// \brief the value set by \ref sampleName
  private:
    std::string m_sampleName;

    /// \brief the value set by \ref minDepth
  private:
    std::size_t m_minDepth;

    /// \brief the value set by \ref maxDepth
  private:
    std::size_t m_maxDepth;

    /// \brief the value set by \ref filePattern, converted to a
    ///   regular expression
  private:
    boost::regex m_filePattern;

    /// \brief the value set by \ref samplePattern, converted to a
    ///   regular expression
  private:
    boost::regex m_samplePattern;

    /// \brief the value set by \ref samplePostfix, converted to a
    ///   regular expression
  private:
    boost::regex m_samplePostfix;

    /// \brief whether \ref samplePostfix has been set to the empty
    /// string
  private:
    bool m_samplePostfixEmpty;

    /// \brief the list of entries from \ref sampleRename
  private:
    typedef std::vector<std::pair<boost::regex,std::string> >::const_iterator SampleRenameIter;
    std::vector<std::pair<boost::regex,std::string> > m_sampleRename;

    /// \brief the depth set with \ref extraNameComponent, or 0 otherwise
  private:
    int m_extraNameComponent;


    /// \brief perform the recursive scanning of the directory tree
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory III\n
    ///   i/o errors
  private:
    void recurse (std::map<std::string,SamplePtr>& samples, DiskList& list,
		  const std::vector<std::string>& hierarchy) const;


    /// \brief add the given file to the sample based on the
    ///   hierarchy, creating the sample if necessary
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  private:
    void addSampleFile (std::map<std::string,SamplePtr>& samples,
			const std::vector<std::string>& hierarchy,
			const std::string& path) const;


    /// \brief find the path component at the given depth
    /// \return the path componenent, or NULL if it doesn't exist
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  private:
    std::string
    findPathComponent (const std::vector<std::string>& hierarchy,
		       int absSampleDepth,
		       int relSampleDepth) const;
  };
}

#endif
