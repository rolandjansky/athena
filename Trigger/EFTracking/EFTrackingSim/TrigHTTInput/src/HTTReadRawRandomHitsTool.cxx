/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTInput/HTTReadRawRandomHitsTool.h"
#include "TrigHTTObjects/HTTEventInputHeader.h"


 HTTReadRawRandomHitsTool::HTTReadRawRandomHitsTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
   base_class(algname,name,ifc) {}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTTReadRawRandomHitsTool::initialize(){

   // open input file
  ATH_MSG_INFO ( "Opening input file: "  << m_inpath.value() );
  m_infile = TFile::Open(m_inpath.value().c_str(),"READ");
  if (m_infile == nullptr){
    ATH_MSG_FATAL ("Could not open input file: " << m_inpath.value() );
    return StatusCode::FAILURE;
  }

  //get the tree
  m_EventTree = (TTree*) m_infile->Get("HTTEventTree");
  if (m_EventTree == nullptr || m_EventTree->GetEntries() == -1 ){
    ATH_MSG_FATAL ("Input file: " << m_inpath << " has no entries");
    return StatusCode::FAILURE;
  }
  m_nEntries = m_EventTree->GetEntries();

  std::string branchName="HTTEventInputHeader";
  if(!m_EventTree->GetListOfBranches()->FindObject(branchName.c_str())){
    ATH_MSG_FATAL ("Branch: " << branchName << " not found!");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO ( "Getting branch and set EventHeader" );
  TBranch *branch  = m_EventTree->GetBranch(branchName.c_str());
  branch->Print();

  m_eventHeader = new HTTEventInputHeader();
  branch->SetAddress(&m_eventHeader);

  return StatusCode::SUCCESS;
}



StatusCode HTTReadRawRandomHitsTool::readData(HTTEventInputHeader* header, bool &last)
{
  return readData(header, last, false); // by default with this tool don't reset data
}

StatusCode HTTReadRawRandomHitsTool::readData(HTTEventInputHeader* header, bool &last, bool doReset)
{

  last = false;
  //unsigned entry = static_cast<unsigned>(CLHEP::RandFlat::shoot() * m_nEntries);
  m_EventTree->GetEntry(m_entry++);

  // Truth Info
  HTTOptionalEventInfo optional;
  int maxbarcode = 0; // to scale up barcodes!

  // --- Copy old data
  if (doReset)
    {
      header->reset(); //reset things!
    }
  else // not resetting, start by copying over truth information from old header
    {
      for (auto truthtrack : header->optional().getTruthTracks())
        {
	        if (truthtrack.getBarcode() > maxbarcode) maxbarcode = truthtrack.getBarcode();
      	  optional.addTruthTrack(truthtrack);
        }

      // now we got the max bar code, copy the offline tracks
      for (auto offlinetrack : header->optional().getOfflineTracks()) 
	      { 
      	  optional.addOfflineTrack(offlinetrack);
      	}
    }

  // --- Copy new data
  for (auto truthtrack : m_eventHeader->optional().getTruthTracks())
    {
      truthtrack.setBarcode(truthtrack.getBarcode() + maxbarcode);
      optional.addTruthTrack(truthtrack);
    }
  for (auto offlinetrack : m_eventHeader->optional().getOfflineTracks()) 
    {
      offlinetrack.setBarcode(offlinetrack.getBarcode() + maxbarcode);
      optional.addOfflineTrack(offlinetrack);
    }

  header->setOptional(optional);

  // copy Hits
  for (auto rawhit : m_eventHeader->hits())
    {
      if (rawhit.getBarcode() >= 0)
        {
	        HTTMultiTruth origtruth = rawhit.getTruth();
      	  HTTMultiTruth mt;
      	  HTTMultiTruth::Barcode uniquecode(rawhit.getEventIndex(),rawhit.getBarcode()+maxbarcode);
      	  mt.maximize(uniquecode, rawhit.getBarcodePt());
      	  rawhit.setBarcode(rawhit.getBarcode() + maxbarcode);
      	  rawhit.setTruth(mt);
        }
      header->addHit(rawhit);
    }

  return StatusCode::SUCCESS;
}


StatusCode HTTReadRawRandomHitsTool::writeData(HTTEventInputHeader* /*header*/)  {  
  ATH_MSG_FATAL("This tool is being forced to write things. But it is only designed to read things. Don't worry, everything is fine");
  return StatusCode::FAILURE;// this tool is not designed to write things
}


StatusCode HTTReadRawRandomHitsTool::finalize(){
  delete m_eventHeader;
  return StatusCode::SUCCESS;
}
