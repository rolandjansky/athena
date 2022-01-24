/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : LumiBlockRangeContainerConverter
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 *      Marjorie Shapiro <mdshapiro@lbl.gov>
 **********************************************************************************/

#ifndef LUMICALC_LUMIBLOCKRANGECONTAINERCONVERTER_H
#define LUMICALC_LUMIBLOCKRANGECONTAINERCONVERTER_H

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

  LumiBlockRangeContainerConverter (const LumiBlockRangeContainerConverter&) = delete;
  LumiBlockRangeContainerConverter& operator= (const LumiBlockRangeContainerConverter&) = delete;

  xAOD::LumiBlockRangeContainer* GetLumiBlockRangeContainer(const Root::TGoodRunsList& grl, std::map<TString,TString>& metadata, TString& version) ;
  xAOD::LumiBlockRangeContainer* GetLumiBlockRangeContainer(const Root::TGoodRunsList& grl) const;
  xAOD::LumiBlockRangeContainer* GetLumiBlockRangeContainer(const char* xmlfile);
  xAOD::LumiBlockRangeContainer* GetLumiBlockRangeContainerFromString( const TString& xmlstring );

  void CreateXMLFile(const Root::TGoodRunsList& grl, const TString& xmlfilename="", const TString& prefix="" );
  void CreateXMLFile(const Root::TGRLCollection& grlcollection, const char* xmlfilename);
  void CreateXMLFile(const xAOD::LumiBlockRangeContainer& lbc, const grlmetadatamap& metadata = grlmetadatamap(), const char* version="", 
                     const TString& xmlfilename="", const TString& prefix="" );
  void CreateXMLFiles(const Root::TGRLCollection& grlcollection, const char* prefix);

  const TString GetXMLString(const Root::TGoodRunsList& grl);
  const TString GetXMLString(const Root::TGRLCollection& grlcollection);
  const TString GetXMLString(const xAOD::LumiBlockRangeContainer& lbc, const grlmetadatamap& metadata = grlmetadatamap(), const char* version="" );
  const std::vector<TString> GetXMLStrings(const Root::TGRLCollection& grlcollection);

  Root::TGoodRunsList* GetGRLObject( const xAOD::LumiBlockRangeContainer& lbc, const grlmetadatamap& metadata = grlmetadatamap(), const char* version="" ) const;
  Root::TGoodRunsList* GetGRLObject( const char* xmlfile );
  Root::TGoodRunsList* GetGRLObjectFromString( const TString& xmlstring );

  Root::TGRLCollection* GetGRLCollection( const char* xmlfile );
  Root::TGRLCollection* GetGRLCollectionFromString( const TString& xmlstring );

  const TString GetSuggestedName( const xAOD::LumiBlockRangeContainer& lbc ) const ;

 private:

  Root::TGoodRunsListReader* m_reader;   
  Root::TGoodRunsListWriter* m_writer;

};

#endif

