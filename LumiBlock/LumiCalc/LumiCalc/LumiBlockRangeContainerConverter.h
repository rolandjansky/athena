/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : LumiBlockRangeContainerConverter
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 *      Marjorie Shapiro <mdshapiro@lbl.gov>
 **********************************************************************************/

#ifndef __LumiBlockRangeContainerConverter__
#define __LumiBlockRangeContainerConverter__

#include <map>
#include <vector>
#include "TString.h"

namespace Root {
  class TGoodRunsList;
  class TGRLCollection;
  class TGoodRunsListReader;
  class TGoodRunsListWriter;
}

#include "xAODLuminosity/LumiBlockRangeContainer.h"
#include "xAODLuminosity/LumiBlockRangeAuxContainer.h"

typedef std::map<TString,TString> grlmetadatamap;

class LumiBlockRangeContainerConverter {
   
 public:

  LumiBlockRangeContainerConverter();      
  virtual ~LumiBlockRangeContainerConverter();

  xAOD::LumiBlockRangeContainer* GetLumiBlockRangeContainer(const Root::TGoodRunsList& grl, std::map<TString,TString>& metadata, TString& version) ;
  xAOD::LumiBlockRangeContainer* GetLumiBlockRangeContainer(const Root::TGoodRunsList& grl) const;
  xAOD::LumiBlockRangeContainer* GetLumiBlockRangeContainer(const char* xmlfile) const;
  xAOD::LumiBlockRangeContainer* GetLumiBlockRangeContainerFromString( const TString& xmlstring ) const;

  void CreateXMLFile(const Root::TGoodRunsList& grl, const TString& xmlfilename="", const TString& prefix="" ) const;
  void CreateXMLFile(const Root::TGRLCollection& grlcollection, const char* xmlfilename) const;
  void CreateXMLFile(const xAOD::LumiBlockRangeContainer& lbc, const grlmetadatamap& metadata = grlmetadatamap(), const char* version="", 
                     const TString& xmlfilename="", const TString& prefix="" ) const;
  void CreateXMLFiles(const Root::TGRLCollection& grlcollection, const char* prefix) const;

  const TString GetXMLString(const Root::TGoodRunsList& grl) const;
  const TString GetXMLString(const Root::TGRLCollection& grlcollection) const;
  const TString GetXMLString(const xAOD::LumiBlockRangeContainer& lbc, const grlmetadatamap& metadata = grlmetadatamap(), const char* version="" ) const;
  const std::vector<TString> GetXMLStrings(const Root::TGRLCollection& grlcollection) const;

  Root::TGoodRunsList* GetGRLObject( const xAOD::LumiBlockRangeContainer& lbc, const grlmetadatamap& metadata = grlmetadatamap(), const char* version="" ) const;
  Root::TGoodRunsList* GetGRLObject( const char* xmlfile ) const;
  Root::TGoodRunsList* GetGRLObjectFromString( const TString& xmlstring ) const;

  Root::TGRLCollection* GetGRLCollection( const char* xmlfile ) const;
  Root::TGRLCollection* GetGRLCollectionFromString( const TString& xmlstring ) const;

  const TString GetSuggestedName( const xAOD::LumiBlockRangeContainer& lbc ) const ;

 private:

  Root::TGoodRunsListReader* m_reader;   
  Root::TGoodRunsListWriter* m_writer;

};

#endif

