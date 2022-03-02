/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  LumiBlockCollection* GetLumiBlockCollection(const char* xmlfile);
  LumiBlockCollection* GetLumiBlockCollectionFromString( const TString& xmlstring );

  void CreateXMLFile(const Root::TGoodRunsList& grl, const TString& xmlfilename="", const TString& prefix="" );
  void CreateXMLFile(const Root::TGRLCollection& grlcollection, const char* xmlfilename);
  void CreateXMLFile(const LumiBlockCollection& lbc, const grlmetadatamap& metadata = grlmetadatamap(), const char* version="", 
                     const TString& xmlfilename="", const TString& prefix="" );
  void CreateXMLFiles(const Root::TGRLCollection& grlcollection, const char* prefix);

  const TString GetXMLString(const Root::TGoodRunsList& grl);
  const TString GetXMLString(const Root::TGRLCollection& grlcollection);
  const TString GetXMLString(const LumiBlockCollection& lbc, const grlmetadatamap& metadata = grlmetadatamap(), const char* version="" );
  const std::vector<TString> GetXMLStrings(const Root::TGRLCollection& grlcollection);

  Root::TGoodRunsList* GetGRLObject( const LumiBlockCollection& lbc, const grlmetadatamap& metadata = grlmetadatamap(), const char* version="" ) const;
  Root::TGoodRunsList* GetGRLObject( const char* xmlfile );
  Root::TGoodRunsList* GetGRLObjectFromString( const TString& xmlstring );

  Root::TGRLCollection* GetGRLCollection( const char* xmlfile );
  Root::TGRLCollection* GetGRLCollectionFromString( const TString& xmlstring );

  const TString GetSuggestedName( const LumiBlockCollection& lbc ) const ;

 private:

  Root::TGoodRunsListReader* m_reader;   
  Root::TGoodRunsListWriter* m_writer;

};

#endif

