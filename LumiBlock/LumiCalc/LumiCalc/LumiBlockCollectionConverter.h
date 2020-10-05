/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : LumiBlockCollectionConverter                                                       *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 **********************************************************************************/

#ifndef LUMICALC_LUMIBLOCKCOLLECTIONCONVERTER_H
#define LUMICALC_LUMIBLOCKCOLLECTIONCONVERTER_H

#include <map>
#include <vector>
#include "TString.h"

namespace Root {
  class TGoodRunsList;
  class TGRLCollection;
  class TGoodRunsListReader;
  class TGoodRunsListWriter;
}

class LumiBlockCollection;

typedef std::map<TString,TString> grlmetadatamap;

class LumiBlockCollectionConverter {
   
 public:

  LumiBlockCollectionConverter();      
  virtual ~LumiBlockCollectionConverter();

  LumiBlockCollectionConverter (const LumiBlockCollectionConverter&) = delete;
  LumiBlockCollectionConverter& operator= (const LumiBlockCollectionConverter&) = delete;

  LumiBlockCollection* GetLumiBlockCollection(const Root::TGoodRunsList& grl, std::map<TString,TString>& metadata, TString& version) ;
  LumiBlockCollection* GetLumiBlockCollection(const Root::TGoodRunsList& grl) const;
  LumiBlockCollection* GetLumiBlockCollection(const char* xmlfile) const;
  LumiBlockCollection* GetLumiBlockCollectionFromString( const TString& xmlstring ) const;

  void CreateXMLFile(const Root::TGoodRunsList& grl, const TString& xmlfilename="", const TString& prefix="" ) const;
  void CreateXMLFile(const Root::TGRLCollection& grlcollection, const char* xmlfilename) const;
  void CreateXMLFile(const LumiBlockCollection& lbc, const grlmetadatamap& metadata = grlmetadatamap(), const char* version="", 
                     const TString& xmlfilename="", const TString& prefix="" ) const;
  void CreateXMLFiles(const Root::TGRLCollection& grlcollection, const char* prefix) const;

  const TString GetXMLString(const Root::TGoodRunsList& grl) const;
  const TString GetXMLString(const Root::TGRLCollection& grlcollection) const;
  const TString GetXMLString(const LumiBlockCollection& lbc, const grlmetadatamap& metadata = grlmetadatamap(), const char* version="" ) const;
  const std::vector<TString> GetXMLStrings(const Root::TGRLCollection& grlcollection) const;

  Root::TGoodRunsList* GetGRLObject( const LumiBlockCollection& lbc, const grlmetadatamap& metadata = grlmetadatamap(), const char* version="" ) const;
  Root::TGoodRunsList* GetGRLObject( const char* xmlfile ) const;
  Root::TGoodRunsList* GetGRLObjectFromString( const TString& xmlstring ) const;

  Root::TGRLCollection* GetGRLCollection( const char* xmlfile ) const;
  Root::TGRLCollection* GetGRLCollectionFromString( const TString& xmlstring ) const;

  const TString GetSuggestedName( const LumiBlockCollection& lbc ) const ;

 private:

  Root::TGoodRunsListReader* m_reader;   
  Root::TGoodRunsListWriter* m_writer;

};

#endif

