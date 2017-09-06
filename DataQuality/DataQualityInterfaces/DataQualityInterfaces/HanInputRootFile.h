/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiHanInputRootFile_h
#define dqiHanInputRootFile_h

#include <TDirectory.h>
#include <TKey.h>
#include <TObject.h>

#include <string>
#include <vector>

#include "dqm_core/InputRootFile.h"

namespace dqi {

class HanInputRootFile: public dqm_core::InputRootFile {
public:

  HanInputRootFile( std::string& rootFileName, std::string path = "" );
  ~HanInputRootFile();

  virtual void addListener( const boost::regex& regex, dqm_core::InputListener* listener )
    throw (dqm_core::Exception);

  virtual void addListener( const std::vector<std::string>& names, dqm_core::InputListener* listener )
    throw (dqm_core::Exception);

  virtual void addListener( const std::string& name, dqm_core::InputListener *listener )
    throw (dqm_core::Exception);

  virtual TFile* file() const;

  virtual const TDirectory* getBasedir() const;

protected:

  std::auto_ptr<TFile>  m_rootFile;
  TDirectory*           m_basedir;

  std::vector<std::string> m_histNames;
  bool                     m_histNamesBuilt;

  static void dolsr(const TDirectory* dir, std::vector<std::string>& hists, const TDirectory* topdir = NULL);

};

} // namespace dqi

#endif
