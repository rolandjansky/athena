/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiCalc/LumiBlockRangeContainerConverter.h"
#include "xAODLuminosity/SortLumiBlockRangeByStart.h"

#include "GoodRunsLists/TLumiBlockRange.h"
#include "GoodRunsLists/TGoodRun.h"
#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TGRLCollection.h"
#include "GoodRunsLists/TGoodRunsListReader.h"
#include "GoodRunsLists/TGoodRunsListWriter.h"

#include "TROOT.h"

#include <stdlib.h>

LumiBlockRangeContainerConverter::LumiBlockRangeContainerConverter()
 : m_reader(new Root::TGoodRunsListReader())
 , m_writer(new Root::TGoodRunsListWriter())
{
}


LumiBlockRangeContainerConverter::~LumiBlockRangeContainerConverter()
{
  if (m_reader!=0) { delete m_reader; m_reader=0; }
  if (m_writer!=0) { delete m_writer; m_writer=0; }
}


xAOD::LumiBlockRangeContainer* 
LumiBlockRangeContainerConverter::GetLumiBlockRangeContainer(const Root::TGoodRunsList& grl, std::map<TString,TString>& metadata, TString& version) 
{
  metadata = grl.GetMetaData();
  version = grl.GetVersion();
  return this->GetLumiBlockRangeContainer( grl );
}


xAOD::LumiBlockRangeContainer* 
LumiBlockRangeContainerConverter::GetLumiBlockRangeContainer(const Root::TGoodRunsList& grl) const 
{
  //  xAOD::LumiBlockRangeContainer* iovc = new xAOD::LumiBlockRangeContainer();
  xAOD::LumiBlockRangeContainer* piovComplete = new xAOD::LumiBlockRangeContainer();
  xAOD::LumiBlockRangeAuxContainer* piovCompleteAux = new xAOD::LumiBlockRangeAuxContainer();
  piovComplete->setStore( piovCompleteAux );

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
      xAOD::LumiBlockRange* iovr = new xAOD::LumiBlockRange();
      piovComplete->push_back(iovr);
      iovr->setStartRunNumber(RunNumber);
      iovr->setStartLumiBlockNumber(LumiBlockStart);
      iovr->setStopRunNumber(RunNumber);
      iovr->setStopLumiBlockNumber(LumiBlockEnd);
      iovr->setEventsExpected(0);
      iovr->setEventsSeen(0);
      //      iovc->push_back(new LB_IOVRange(IOVTime(RunNumber, LumiBlockStart), IOVTime(RunNumber,LumiBlockEnd)));
    }
  }
  //  iovc->sort(xAOD::LumiBlockRangeContainer::SortIOVRangeByStart());
  piovComplete->sort(xAOD::SortLumiBlockRangeByStart());
  return piovComplete;
}


xAOD::LumiBlockRangeContainer* 
LumiBlockRangeContainerConverter::GetLumiBlockRangeContainer(const char* xmlfile)
{
  Root::TGoodRunsList* pgrl = this->GetGRLObject( xmlfile );
  xAOD::LumiBlockRangeContainer* iovc = this->GetLumiBlockRangeContainer( *pgrl );
  delete pgrl;
  return iovc;
}


xAOD::LumiBlockRangeContainer* 
LumiBlockRangeContainerConverter::GetLumiBlockRangeContainerFromString( const TString& xmlstring )
{
  m_reader->SetXMLString( xmlstring );
  (void) m_reader->Interpret();
  return this->GetLumiBlockRangeContainer( m_reader->GetMergedGoodRunsList() );
}


void 
LumiBlockRangeContainerConverter::CreateXMLFile(const Root::TGRLCollection& grlcollection, const char* xmlfilename)
{
  m_writer->SetGRLCollection( grlcollection ) ;
  m_writer->SetFilename( xmlfilename ) ;
  m_writer->WriteXMLFile() ;
}


void
LumiBlockRangeContainerConverter::CreateXMLFile(const Root::TGoodRunsList& grl, const TString& xmlfilename, const TString& prefix)
{
  m_writer->SetGoodRunsList( grl ) ;
  TString xmlfile = ( xmlfilename.IsNull() ? prefix + grl.GetSuggestedName() + ".xml" : xmlfilename );
  m_writer->SetFilename( xmlfile.Data() ) ;
  m_writer->WriteXMLFile() ;
}


void 
LumiBlockRangeContainerConverter::CreateXMLFile(const xAOD::LumiBlockRangeContainer& lbc, const grlmetadatamap& metadata, const char* version, 
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
LumiBlockRangeContainerConverter::CreateXMLFiles(const Root::TGRLCollection& grlcollection, const char* prefix)
{
  m_writer->SetGRLCollection( grlcollection ) ;
  m_writer->SetPrefix( prefix ) ;
  m_writer->WriteXMLFiles() ;
}


const TString 
LumiBlockRangeContainerConverter::GetXMLString(const Root::TGoodRunsList& grl)
{
  m_writer->SetGoodRunsList( grl ) ;
  return m_writer->GetXMLString() ;
}


const TString
LumiBlockRangeContainerConverter::GetXMLString(const Root::TGRLCollection& grlcollection)
{
  m_writer->SetGRLCollection( grlcollection ) ;
  return m_writer->GetXMLString() ;
}


const std::vector<TString> 
LumiBlockRangeContainerConverter::GetXMLStrings(const Root::TGRLCollection& grlcollection)
{
  m_writer->SetGRLCollection( grlcollection ) ;
  return m_writer->GetXMLStrings() ;
}


const TString 
LumiBlockRangeContainerConverter::GetXMLString( const xAOD::LumiBlockRangeContainer& lbc, const std::map<TString,TString>& metadata, const char* version )
{
  Root::TGoodRunsList* pgrl = this->GetGRLObject(lbc,metadata,version);
  m_writer->SetGoodRunsList( *pgrl ) ;
  TString xmlstring = m_writer->GetXMLString() ;
  delete pgrl;
  return xmlstring;
}


Root::TGoodRunsList* 
LumiBlockRangeContainerConverter::GetGRLObject( const xAOD::LumiBlockRangeContainer& lbc, const std::map<TString,TString>& metadata, const char* version ) const
{
  Root::TGoodRunsList* grl = new Root::TGoodRunsList();
  grl->SetVersion(version);
  grl->SetMetaData(metadata);

  if (!lbc.empty()) {
    Root::TGoodRun goodrun;
    int prev_runnbr(-1), cur_runnbr(-1), cur_lbstart(-1), cur_lbstop(-1);

    for (const xAOD::LumiBlockRange* lbr : lbc) {
      //    for( xAOD::LumiBlockRangeContainer::const_iterator it=lbc.begin(); it != lbc.end(); ++it ) {
      //      const LB_IOVRange* iovr = (*it);
      cur_runnbr = lbr->startRunNumber();
      cur_lbstart = lbr->startLumiBlockNumber();
      cur_lbstop = lbr->stopLumiBlockNumber();
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
LumiBlockRangeContainerConverter::GetGRLObject( const char* xmlfile )
{
  m_reader->SetXMLFile( xmlfile );
  (void) m_reader->Interpret();
  return (new Root::TGoodRunsList( m_reader->GetMergedGoodRunsList() )); 
}


Root::TGoodRunsList* 
LumiBlockRangeContainerConverter::GetGRLObjectFromString( const TString& xmlstring )
{
  m_reader->SetXMLString( xmlstring );
  (void) m_reader->Interpret();
  return (new Root::TGoodRunsList( m_reader->GetMergedGoodRunsList() ));
}


Root::TGRLCollection*
LumiBlockRangeContainerConverter::GetGRLCollection( const char* xmlfile )
{
  m_reader->SetXMLFile( xmlfile );
  (void) m_reader->Interpret();
  return (new Root::TGRLCollection( m_reader->GetMergedGRLCollection() ));
} 


Root::TGRLCollection*
LumiBlockRangeContainerConverter::GetGRLCollectionFromString( const TString& xmlstring )
{
  m_reader->SetXMLString( xmlstring );
  (void) m_reader->Interpret();
  return (new Root::TGRLCollection( m_reader->GetMergedGRLCollection() ));
}


const TString
LumiBlockRangeContainerConverter::GetSuggestedName( const xAOD::LumiBlockRangeContainer& lbc ) const
{
  if (lbc.empty()) return "grl_empty";

  Int_t beginrun(-1), endrun(-1), beginlb(-1), endlb(-1);

  xAOD::LumiBlockRangeContainer::const_iterator itb=lbc.begin();
  xAOD::LumiBlockRangeContainer::const_reverse_iterator ite=lbc.rbegin();

  //      cur_runnbr = (*i)->startRunNumber();
  //      cur_lbstart = (*i)->startLumiBlockNumber();
  //      cur_lbstop = (*i)->stopLumiBlockNumber();

  beginrun = (*itb)->startRunNumber();
  beginlb  = (*itb)->startLumiBlockNumber();
  endrun = (*ite)->stopRunNumber();
  endlb  = (*ite)->stopLumiBlockNumber();

  return Form("grl_%d.%d-%d.%d",beginrun,beginlb,endrun,endlb);
}

