/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETGoodnessFillerTool.cxx
 * @author Max Baak <mbaak@cern.ch>
 * @date Nov, 2009
 * @brief Block filler tool for Missing ET derived objects.
 */

#include "MissingETD3PDMaker/MissingETGoodnessFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "MissingETEvent/MissingET.h"
#include "MissingETGoodness/EtmissGoodness.h"
#include "MissingETGoodness/EtmissGoodnessConfig.h"

// reference to MET Goodies map for storing derived quantities
MET::Goodies& D3PD::MissingETGoodnessFillerTool::goodies(MET::Goodies::instance());

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
D3PD::MissingETGoodnessFillerTool::MissingETGoodnessFillerTool (const std::string& type,
							        const std::string& name,
							        const IInterface* parent) 
 : BlockFillerTool<MissingET> (type, name, parent)
 , m_log( msgSvc(),name )
 , m_dummyi(-999)
 , m_dummyd(-999.)
 , m_pbitmask(0)
 , m_issecondbook(false)
 , m_metgness(0)
{
}


/**
 * @ destructor for this block
 */
D3PD::MissingETGoodnessFillerTool::~MissingETGoodnessFillerTool()
{
  m_pintVec.clear();
  m_pdoubleVec.clear();
  m_pvectorIntVec.clear();
  m_pvectorDoubleVec.clear();
  m_pboolVec.clear();
  m_ginameList.clear();
  m_gdnameList.clear();
  if (m_metgness!=0) { delete m_metgness; m_metgness=0; }
}


/**
 * @brief Book variables for this block. Called at first event
 */
StatusCode 
D3PD::MissingETGoodnessFillerTool::book()
{
  // MB : hack, call this function from first event instead
  //      Goodies map not yet filled at first book() call
  if (!m_issecondbook) {
    m_issecondbook = true;
    return StatusCode::SUCCESS;
  }  

  // treat this as initialize
  m_log.setLevel( outputLevel() );

  // Create observables name list from everything available in first event
  m_pintVec.resize(goodies.gInt().size(),0);
  m_pdoubleVec.resize(goodies.gDouble().size(),0);
  m_pvectorDoubleVec.resize(goodies.gVectorDouble().size(),0); 
  m_pvectorIntVec.resize(goodies.gVectorInt().size(),0); 
  m_ginameList.resize(goodies.gInt().size(),TString());
  m_gdnameList.resize(goodies.gDouble().size(),TString());
  m_gvdnameList.resize(goodies.gVectorDouble().size(),TString());
  m_gvinameList.resize(goodies.gVectorInt().size(),TString());

  m_qualifierList = MET::GetEtmissGoodnessQualifierList();
  m_pboolVec.resize(m_qualifierList.size(),0);

  // book the integers
  std::map<TString,int>::const_iterator giitr = goodies.gInt().begin();
  for (int idx=0; giitr!=goodies.gInt().end(); ++giitr, ++idx) { 
    m_log << MSG::DEBUG << name() << " book() : Now booking (i) : " << giitr->first << endreq;
    CHECK( addVariable (giitr->first.Data(),m_pintVec[idx]) );
    m_ginameList[idx] = giitr->first;
  }
  // book the doubles
  std::map<TString,double>::const_iterator gditr = goodies.gDouble().begin();
  for (int idx=0; gditr!=goodies.gDouble().end(); ++gditr, ++idx) {
    m_log << MSG::DEBUG << name() << " book() : Now booking (d) : " << gditr->first << endreq;
    CHECK( addVariable (gditr->first.Data(),m_pdoubleVec[idx]) );
    m_gdnameList[idx] = gditr->first;
  }
  // book the vectors
  std::map<TString,std::vector<double> >::const_iterator gvditr = goodies.gVectorDouble().begin();
  for (int ivx=0; gvditr!=goodies.gVectorDouble().end(); ++gvditr, ++ivx) {
    m_log << MSG::DEBUG << name() << " book() : Now booking (vd) : " << gvditr->first << endreq;
    CHECK( addVariable (gvditr->first.Data(),m_pvectorDoubleVec[ivx]) );
    m_gvdnameList[ivx] = gvditr->first;
  }
  // book the vectors
  std::map<TString,std::vector<int> >::const_iterator gviitr = goodies.gVectorInt().begin();
  for (int ivx=0; gviitr!=goodies.gVectorInt().end(); ++gviitr, ++ivx) {
    m_log << MSG::DEBUG << name() << " book() : Now booking (vi) : " << gviitr->first << endreq;
    CHECK( addVariable (gviitr->first.Data(),m_pvectorIntVec[ivx]) );
    m_gvinameList[ivx] = gviitr->first;
  }

  // met isgood() for various selector qualifiers
  std::vector<TString>::const_iterator qitr = m_qualifierList.begin();
  for (int idx=0; qitr!=m_qualifierList.end(); ++qitr, ++idx) {
    m_log << MSG::DEBUG << name() << " book() : Now booking (b) : " << *qitr << endreq;
    CHECK( addVariable (qitr->Data(),m_pboolVec[idx]) );
  }
  // met goodness selection bitmask
  m_log << MSG::DEBUG << name() << " book() : Now booking (l) : BitMask" << endreq;
  CHECK( addVariable ("BitMask",m_pbitmask) );
  m_metgness = new MET::EtmissGoodness();
 
  return StatusCode::SUCCESS;
}


StatusCode 
D3PD::MissingETGoodnessFillerTool::fill (const MissingET& /*p*/)
{
  
  // call book() before continuing, need to book parameters of interest
  if ( m_issecondbook ) {
    StatusCode sc = this->book();
    if( sc.isFailure() ) {
      m_log << MSG::ERROR << "MissingETD3PDMaker::MissingETGoodnessFillerTool failed to called on goodness" << endreq;
      return sc;
    }//failure
    m_issecondbook=false;
  }

  std::vector<TString>::const_iterator litr;

  // fill the integers
  std::map<TString,int>::iterator giitr;
  litr = m_ginameList.begin();
  for (int idx=0; litr!=m_ginameList.end(); ++litr, ++idx) {
    giitr = goodies.gInt().find(*litr);
    // set dummy value in case name is missing
    if (giitr==goodies.gInt().end()) {
      goodies.gInt()[*litr] = m_dummyi;
      giitr = goodies.gInt().find(*litr);
    }
    m_log << MSG::DEBUG << name() << " fill() : Now filling (i) : " << giitr->first << " = " << giitr->second << endreq;
    *(m_pintVec[idx]) = (giitr->second);
  }

  // fill the doubles
  std::map<TString,double>::iterator gditr;
  litr=m_gdnameList.begin();
  for (int idx=0; litr!=m_gdnameList.end(); ++litr, ++idx) {
    gditr = goodies.gDouble().find(*litr);
    // set dummy value in case name is missing
    if (gditr==goodies.gDouble().end()) { 
      goodies.gDouble()[*litr] = m_dummyd;
      gditr = goodies.gDouble().find(*litr);
    }
    m_log << MSG::DEBUG << name() << " fill() : Now filling (d) : " << gditr->first << " = " << gditr->second << endreq;
    *(m_pdoubleVec[idx]) = (gditr->second);
  }

  // fill the vectors
  std::map<TString,std::vector<double> >::iterator gvditr;
  litr=m_gvdnameList.begin();
  for (int ivx=0; litr!=m_gvdnameList.end(); ++litr, ++ivx) {
    gvditr = goodies.gVectorDouble().find(*litr);
    // set dummy value in case name is missing
    if (gvditr==goodies.gVectorDouble().end()) { 
      goodies.gVectorDouble()[*litr] = m_dummyvD;
      gvditr = goodies.gVectorDouble().find(*litr);
    }
    m_log << MSG::DEBUG << name() << " fill() : Now filling (vd) : " << gvditr->first << " = " << gvditr->second << endreq;
    *(m_pvectorDoubleVec[ivx]) = (gvditr->second);
  }
  // fill the vectors
  std::map<TString,std::vector<int> >::iterator gviitr;
  litr=m_gvinameList.begin();
  for (int ivx=0; litr!=m_gvinameList.end(); ++litr, ++ivx) {
    gviitr = goodies.gVectorInt().find(*litr);
    // set dummy value in case name is missing
    if (gviitr==goodies.gVectorInt().end()) { 
      goodies.gVectorInt()[*litr] = m_dummyvI;
      gviitr = goodies.gVectorInt().find(*litr);
    }
    m_log << MSG::DEBUG << name() << " fill() : Now filling (vi) : " << gviitr->first << " = " << gviitr->second << endreq;
    *(m_pvectorIntVec[ivx]) = (gviitr->second);
  }

  // met isgood() for various selector qualifiers
  Long64_t bitmask(0);
  /*
  std::vector<TString>::const_iterator qitr = m_qualifierList.begin();
  for (int idx=0; qitr!=m_qualifierList.end(); ++qitr, ++idx)
    *(m_pboolVec[idx]) = m_metgness->IsGood(qitr->Data());
  // met selection bitmask
  */
  *m_pbitmask = bitmask; //m_metgness->GetBitMask();

  return StatusCode::SUCCESS;
}


