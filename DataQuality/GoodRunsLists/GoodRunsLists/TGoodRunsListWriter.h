/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************************
 * Class  : TGoodRunsListWriter                                                   *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Max Baak <mbaak@cern.ch> - CERN, Switzerland                              *
 **********************************************************************************/

#ifndef _TGoodRunsListWriter
#define _TGoodRunsListWriter

#include <vector>

#include "TROOT.h"
#include "TList.h"
#include "TObject.h"
#include "TString.h"

#include "GoodRunsLists/TMsgLogger.h"
#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TGRLCollection.h"

typedef unsigned char xmlChar;
typedef struct _xmlTextWriter xmlTextWriter;
typedef xmlTextWriter *xmlTextWriterPtr;

namespace Root {

  class TLumiBlockRange;
  class TGoodRun;
  class TGoodRunsList;
  
  class TGoodRunsListWriter : public TObject {
    
   public:
    
    TGoodRunsListWriter();
    TGoodRunsListWriter( const TGoodRunsList& goodrunslist, const TString& dataCardName );
    ~TGoodRunsListWriter();
    
    Bool_t WriteXMLFile();      
    Bool_t WriteXMLFiles();
    const TString& GetXMLString();
    const std::vector<TString>& GetXMLStrings();
    
    // accessors
    inline const TString& GetFilename() const                 { return m_dataCardName; }
    inline const TGoodRunsList GetMergedGoodRunsList( const Root::BoolOperation& operation = OR ) const  
                                                              { return m_grlvec.GetMergedGoodRunsList(operation); }
    inline const TGRLCollection& GetGRLCollection() const     { return m_grlvec; }

    inline void  SetMergeGoodRunsLists( bool merge=true )     { m_mergegrls=merge; }
    inline void  SetCheckGRLInfo( bool check=true )           { m_grlvec.SetCheckGRLInfo( check ); }
    inline void  SetIndividuals( bool indf=true )             { m_individuals=indf; }

    void SetGRLCollection( const TGRLCollection& grlvec )     { reset(); m_grlvec=grlvec; } 
    void SetGoodRunsList( const TGoodRunsList& goodrunslist ) { reset(); m_grlvec.push_back(goodrunslist); } 
    void SetFilename( const TString& dataCardName )           { m_dataCardName=dataCardName; }
    void SetPrefix( const TString& prefix )                   { m_prefix=prefix; }
    void AddGoodRunsList( const TGoodRunsList& goodrunslist ) { m_grlvec.push_back(goodrunslist); }

   private:
    
    // aux method
    void reset();
    
    Bool_t WriteLumiRangeCollection(xmlTextWriterPtr writer);
    Bool_t WriteNamedLumiRange(xmlTextWriterPtr writer);
    Bool_t WriteMetadata(xmlTextWriterPtr writer);
    Bool_t WriteLumiBlockCollection(xmlTextWriterPtr writer, const TGoodRun& goodrun);
    Bool_t WriteElement(xmlTextWriterPtr writer, const char* name, const char* value=0, 
                                                 const char* atr1=0, const char* val1=0, const char* atr2=0, const char* val2=0);

    xmlChar* ConvertInput(const char *in, const char *encoding);
    
    TGRLCollection m_grlvec;
    TGoodRunsList m_grl;

    std::vector<TString> m_xmlstringVec;
    TString m_xmlstring;
    TString m_dataCardName;
    TString m_prefix;

    TMsgLogger m_logger;

    Bool_t m_mergegrls;
    Bool_t m_individuals;
    
    ClassDef(TGoodRunsListWriter,0)
  };
}

#endif

