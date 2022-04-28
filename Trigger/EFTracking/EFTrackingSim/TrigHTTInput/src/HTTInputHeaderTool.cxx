/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HTTInputHeaderTool.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"

std::string const HTTInputHeaderTool::m_branchName="HTTEventInputHeader";

HTTInputHeaderTool::HTTInputHeaderTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
  base_class(algname,name,ifc)
{}

StatusCode HTTInputHeaderTool::openFile(std::string const & path)
{
    // close old file
    if (m_infile && m_infile->IsOpen())
    {
        m_infile->Close();
        // I don't think we delete the pointer cause root handles that?
    }

    // open new file
    ATH_MSG_DEBUG ( "Opening file  "  << path << " in " <<  m_rwoption.value() <<" mode" );
    m_infile = TFile::Open(path.c_str(), m_rwoption.value().c_str());

    if (m_infile == nullptr)
    {
        ATH_MSG_FATAL("Could not open input file: " << path);
        return StatusCode::FAILURE;
    }

    if (m_rwoption.value()==std::string("READ") )
    {
    //get the tree
      m_EventTree = (TTree*) m_infile->Get("HTTEventTree");
      if (m_EventTree == nullptr || m_EventTree->GetEntries() == -1 ){
        ATH_MSG_FATAL ("Input file: " << path << " has no entries");
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO ( "Input file: " << path << " has "<< m_EventTree->GetEntries() <<" event entries" );

      if(!m_EventTree->GetListOfBranches()->FindObject(m_branchName.c_str())){
        ATH_MSG_FATAL ("Branch: " << m_branchName << " not found!");
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO ( "Getting branch and set EventHeader" );
      TBranch *branch  = m_EventTree->GetBranch(m_branchName.c_str());
      branch->SetAddress(&m_eventHeader);
    }

    m_event=0;
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTInputHeaderTool::initialize(){
  
  m_eventHeader = new HTTEventInputHeader();

   // open input file
  if( m_rwoption.value()!=std::string("HEADER"))
  {
    if (m_inpath.value().empty())
    {
        ATH_MSG_ERROR("Empty input file list");
        return StatusCode::FAILURE;
    }
    ATH_CHECK(openFile(m_inpath.value().front()));
  }


  if (m_rwoption.value()==std::string("READ") ) {
    ATH_MSG_DEBUG ("Initialized in READ MODE");
  }
  else if (m_rwoption.value()==std::string("RECREATE") || m_rwoption.value()==std::string("HEADER")) {
    ATH_MSG_INFO ("Creating empty brnaches in output file");
    m_EventTree = new TTree("HTTEventTree","data");
    m_EventTree->Branch(m_branchName.c_str(),
            m_branchName.c_str(),// class name
            &m_eventHeader);
  }
  else {
    ATH_MSG_ERROR("RWstatus =" <<m_rwoption.value()<<" not allowed!");
    return StatusCode::FAILURE;
  }

  m_event=0; // in file
  m_totevent=0; // total counter

  return StatusCode::SUCCESS;
}


StatusCode HTTInputHeaderTool::finalize(){
  ATH_MSG_INFO ( "finalize: closing files");
  if (m_rwoption.value()==std::string("RECREATE")){
    m_EventTree->Print();
     // close the output files, but check that it exists (for athenaMP)
    m_infile ->Write();
  }
  if (m_rwoption.value()!=std::string("HEADER"))
    m_infile ->Close();

  delete m_eventHeader;
  return StatusCode::SUCCESS;
}


StatusCode HTTInputHeaderTool::writeData(HTTEventInputHeader* header)  {
  if (m_rwoption.value()==std::string("READ") ){
   ATH_MSG_WARNING ("Asked to write file in READ  mode");
   return StatusCode::SUCCESS;
  }

  if (header != nullptr){
    *m_eventHeader= *header;//copy object
    m_EventTree->Fill();
    ATH_MSG_DEBUG ("Wrote Event "<<m_event <<" in header event "<<m_eventHeader->event() );
    m_event++;
  }
  m_eventHeader->Clear(); //clean the pointer

  return StatusCode::SUCCESS;
}

StatusCode HTTInputHeaderTool::readData(HTTEventInputHeader* header, bool &last)
{
  if (m_rwoption.value()!=std::string("READ") ){
    ATH_MSG_WARNING ("Asked to read file that is not in READ mode");
    return StatusCode::SUCCESS;
  }

  last=false;

  ATH_MSG_DEBUG ("Asked Event "<<m_event <<" in this file; current total is "<<m_totevent);
  if (m_event >= m_EventTree->GetEntries())
    {
        if (++m_file < m_inpath.value().size())
            ATH_CHECK(openFile(m_inpath.value().at(m_file)));
        else {
          last=true;
          return StatusCode::SUCCESS;
        }
    }


  // increase counters
  ATH_MSG_INFO(m_event);
  m_EventTree->GetEntry(m_event++);
  ATH_MSG_DEBUG("Reading event  "<<m_eventHeader->event() );
  *header= *m_eventHeader; //copy object to the external pointer

  m_totevent++;

  return StatusCode::SUCCESS;

}
