/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiCalc/LumiBlockCollectionConverter.h"
#include "LumiBlockData/LumiBlockCollection.h"
#include "LumiBlockData/LB_IOVRange.h"

#include "GoodRunsLists/TLumiBlockRange.h"
#include "GoodRunsLists/TGoodRun.h"
#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TGRLCollection.h"
#include "GoodRunsLists/TGoodRunsListReader.h"
#include "GoodRunsLists/TGoodRunsListWriter.h"

#include "TROOT.h"

#include <stdlib.h>

LumiBlockCollectionConverter::LumiBlockCollectionConverter()
 : m_reader(new Root::TGoodRunsListReader())
 , m_writer(new Root::TGoodRunsListWriter())
{
}


LumiBlockCollectionConverter::~LumiBlockCollectionConverter()
{
  if (m_reader!=0) { delete m_reader; m_reader=0; }
  if (m_writer!=0) { delete m_writer; m_writer=0; }
}


LumiBlockCollection* 
LumiBlockCollectionConverter::GetLumiBlockCollection(const Root::TGoodRunsList& grl, std::map<TString,TString>& metadata, TString& version) 
{
  metadata = grl.GetMetaData();
  version = grl.GetVersion();
  return this->GetLumiBlockCollection( grl );
}


LumiBlockCollection* 
LumiBlockCollectionConverter::GetLumiBlockCollection(const Root::TGoodRunsList& grl) const 
{
  LumiBlockCollection* iovc = new LumiBlockCollection();

  Root::TGoodRun goodrun;
  std::map<Int_t, Root::TGoodRun>::const_iterator it;
  std::vector< Root::TLumiBlockRange >::const_iterator itlbr;
  int RunNumber(-1), LumiBlockStart(-1), LumiBlockEnd(-1);

  for(it = grl.begin(); it != grl.end(); ++it) {
    RunNumber = it->first;
    goodrun = it->second;
    for(itlbr = goodrun.begin(); itlbr != goodrun.end(); ++itlbr) {
      LumiBlockStart = itlbr->Begin();
      LumiBlockEnd = itlbr->End();
      iovc->push_back(new LB_IOVRange(IOVTime(RunNumber, LumiBlockStart), IOVTime(RunNumber,LumiBlockEnd)));
    }
  }
  iovc->sort(LumiBlockCollection::SortIOVRangeByStart());

  return iovc;
}


LumiBlockCollection* 
LumiBlockCollectionConverter::GetLumiBlockCollection(const char* xmlfile)
{
  Root::TGoodRunsList* pgrl = this->GetGRLObject( xmlfile );
  LumiBlockCollection* iovc = this->GetLumiBlockCollection( *pgrl );
  delete pgrl;
  return iovc;
}


LumiBlockCollection* 
LumiBlockCollectionConverter::GetLumiBlockCollectionFromString( const TString& xmlstring )
{
  m_reader->SetXMLString( xmlstring );
  (void) m_reader->Interpret();
  return this->GetLumiBlockCollection( m_reader->GetMergedGoodRunsList() );
}


void 
LumiBlockCollectionConverter::CreateXMLFile(const Root::TGRLCollection& grlcollection, const char* xmlfilename)
{
  m_writer->SetGRLCollection( grlcollection ) ;
  m_writer->SetFilename( xmlfilename ) ;
  m_writer->WriteXMLFile() ;
}


void
LumiBlockCollectionConverter::CreateXMLFile(const Root::TGoodRunsList& grl, const TString& xmlfilename, const TString& prefix)
{
  m_writer->SetGoodRunsList( grl ) ;
  TString xmlfile = ( xmlfilename.IsNull() ? prefix + grl.GetSuggestedName() + ".xml" : xmlfilename );
  m_writer->SetFilename( xmlfile.Data() ) ;
  m_writer->WriteXMLFile() ;
}


void 
LumiBlockCollectionConverter::CreateXMLFile(const LumiBlockCollection& lbc, const grlmetadatamap& metadata, const char* version, 
                                            const TString& xmlfilename, const TString& prefix )
{
  Root::TGoodRunsList* pgrl = this->GetGRLObject(lbc,metadata,version);
  m_writer->SetGoodRunsList( *pgrl ) ;
  TString xmlfile = ( xmlfilename.IsNull() ? prefix+pgrl->GetSuggestedName()+".xml" : xmlfilename );
  m_writer->SetFilename( xmlfile.Data() ) ;
  m_writer->WriteXMLFile() ;  
  delete pgrl;
}


void
LumiBlockCollectionConverter::CreateXMLFiles(const Root::TGRLCollection& grlcollection, const char* prefix)
{
  m_writer->SetGRLCollection( grlcollection ) ;
  m_writer->SetPrefix( prefix ) ;
  m_writer->WriteXMLFiles() ;
}


const TString 
LumiBlockCollectionConverter::GetXMLString(const Root::TGoodRunsList& grl)
{
  m_writer->SetGoodRunsList( grl ) ;
  return m_writer->GetXMLString() ;
}


const TString
LumiBlockCollectionConverter::GetXMLString(const Root::TGRLCollection& grlcollection)
{
  m_writer->SetGRLCollection( grlcollection ) ;
  return m_writer->GetXMLString() ;
}


const std::vector<TString> 
LumiBlockCollectionConverter::GetXMLStrings(const Root::TGRLCollection& grlcollection)
{
  m_writer->SetGRLCollection( grlcollection ) ;
  return m_writer->GetXMLStrings() ;
}


const TString 
LumiBlockCollectionConverter::GetXMLString( const LumiBlockCollection& lbc, const std::map<TString,TString>& metadata, const char* version )
{
  Root::TGoodRunsList* pgrl = this->GetGRLObject(lbc,metadata,version);
  m_writer->SetGoodRunsList( *pgrl ) ;
  TString xmlstring = m_writer->GetXMLString() ;
  delete pgrl;
  return xmlstring;
}


Root::TGoodRunsList* 
LumiBlockCollectionConverter::GetGRLObject( const LumiBlockCollection& lbc, const std::map<TString,TString>& metadata, const char* version ) const
{
  Root::TGoodRunsList* grl = new Root::TGoodRunsList();
  grl->SetVersion(version);
  grl->SetMetaData(metadata);

  if (!lbc.empty()) {
    Root::TGoodRun goodrun;
    int prev_runnbr(-1), cur_runnbr(-1), cur_lbstart(-1), cur_lbstop(-1);
  
    for( LumiBlockCollection::const_iterator it=lbc.begin(); it != lbc.end(); ++it ) {
      const LB_IOVRange* iovr = (*it);
      cur_runnbr = iovr->start().run();
      cur_lbstart = iovr->start().event();
      cur_lbstop = iovr->stop().event();
      // store previous run and reset
      if (prev_runnbr!=cur_runnbr) {
        if (prev_runnbr>=0) { (*grl)[prev_runnbr]=goodrun; }
        goodrun.clear();
        goodrun.SetRunNumber(cur_runnbr);
      }
      // store lumiblock range
      goodrun.push_back( Root::TLumiBlockRange(cur_lbstart,cur_lbstop) ) ;
      prev_runnbr = cur_runnbr;
    } // loop over lbs
    (*grl)[prev_runnbr]=goodrun; // also store last goodrun
  } // lbc !empty


  grl->Compress();
  return grl;
}


Root::TGoodRunsList* 
LumiBlockCollectionConverter::GetGRLObject( const char* xmlfile )
{
  m_reader->SetXMLFile( xmlfile );
  (void) m_reader->Interpret();
  return (new Root::TGoodRunsList( m_reader->GetMergedGoodRunsList() )); 
}


Root::TGoodRunsList* 
LumiBlockCollectionConverter::GetGRLObjectFromString( const TString& xmlstring )
{
  m_reader->SetXMLString( xmlstring );
  (void) m_reader->Interpret();
  return (new Root::TGoodRunsList( m_reader->GetMergedGoodRunsList() ));
}


Root::TGRLCollection*
LumiBlockCollectionConverter::GetGRLCollection( const char* xmlfile )
{
  m_reader->SetXMLFile( xmlfile );
  (void) m_reader->Interpret();
  return (new Root::TGRLCollection( m_reader->GetMergedGRLCollection() ));
} 


Root::TGRLCollection*
LumiBlockCollectionConverter::GetGRLCollectionFromString( const TString& xmlstring )
{
  m_reader->SetXMLString( xmlstring );
  (void) m_reader->Interpret();
  return (new Root::TGRLCollection( m_reader->GetMergedGRLCollection() ));
}


const TString
LumiBlockCollectionConverter::GetSuggestedName( const LumiBlockCollection& lbc ) const
{
  if (lbc.empty()) return "grl_empty";

  Int_t beginrun(-1), endrun(-1), beginlb(-1), endlb(-1);

  LumiBlockCollection::const_iterator itb=lbc.begin();
  LumiBlockCollection::const_reverse_iterator ite=lbc.rbegin();

  beginrun = (*itb)->start().run();
  beginlb  = (*itb)->start().event();
  endrun = (*ite)->stop().run();
  endlb  = (*ite)->stop().event();

  return Form("grl_%d.%d-%d.%d",beginrun,beginlb,endrun,endlb);
}

