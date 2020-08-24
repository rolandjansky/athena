/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//  TopoASCIIReader.cpp

#include "L1TopoCoreSim/TopoASCIIReader.h"

#include <iostream>

using namespace std;

// default constructor
TCS::TopoASCIIReader::TopoASCIIReader(int verbose) :
   m_verbosity(verbose)
{
}

// destructor
TCS::TopoASCIIReader::~TopoASCIIReader() {
  m_fs.close();
}

// load input file
void TCS::TopoASCIIReader::loadInput(const std::string & input) {
   if(input=="") {
      TCS_EXCEPTION("Specify the location of the ascii input file");
   }
  
   cout << "TopoASCIIReader: Opening file " << input << endl;
   m_fs.open(input.c_str());
   m_inputFile = input;
}

void TCS::TopoASCIIReader::validateInput() {
  if(!m_fs) {
     TCS_EXCEPTION("Error opening input file!");
  } else {
     cout << "File " << m_inputFile << " loaded and ready to parse." << endl;
  }
}

bool TCS::TopoASCIIReader::getNextEvent() {
  
   // clear info from previous event
   string type = "default";
   string currentLine;
   m_event->clear();
  
   // run through single event
   while((currentLine != "<end_file>") && (currentLine != "</file>")) {

      // get next data line
      std::getline(m_fs, currentLine);
      if(m_verbosity>0)
         cout << currentLine << endl;

      // process these lines
    
      if(currentLine == "<end_file>" || currentLine == "</file>") return false;
      if(currentLine == "<end_event>" || currentLine == "</event>") break;
      if(currentLine == "<cluster>" || currentLine == "<jet>" || currentLine == "<muon>" || currentLine == "<lateMuon>" || currentLine == "<muonNextBC>" || currentLine == "<tau>" || currentLine == "<met>" || currentLine == "<info>") type = currentLine;
      if(currentLine == "</cluster>" || currentLine == "</jet>" || currentLine == "</muon>" || currentLine == "</lateMuon>" || currentLine == "</muonNextBC>" || currentLine == "</tau>" || currentLine == "</met>" || currentLine == "</info>") { type = ""; continue; }
      if(currentLine == "<begin_file>" || currentLine == "<file>" || currentLine == "<begin_event>" || currentLine == "<event>" || currentLine == "<cluster>" || currentLine == "<jet>" || currentLine == "<muon>" || currentLine == "<lateMuon>" || currentLine == "<muonNextBC>" || currentLine == "<tau>" || currentLine == "<met>" || currentLine == "<info>") continue;
    
      // use stream iterators to copy the stream to a vector as whitespace separated strings
      std::stringstream ss(currentLine);
      std::istream_iterator<std::string> it(ss);
      std::istream_iterator<std::string> end;
      std::vector<std::string> results(it, end);
    
      //std::cout << results.size() << std::endl;
      // skip over whitespace
      if(results.size() == 0) continue;
        
      // convert whitespace separated strings of results into ints
      if(type == "<cluster>") {
         TCS::ClusterTOB cl( TCS::ClusterTOB(atoi(results.at(0).c_str()),atoi(results.at(1).c_str()),atoi(results.at(2).c_str()),atoi(results.at(3).c_str())));
         if(results.size()==6) {
            cl.setEtaDouble( atof(results.at(4).c_str()) );
            cl.setPhiDouble( atof(results.at(5).c_str()) );
         }
         m_event->addCluster(cl);
      } else if(type == "<tau>") {
         TCS::ClusterTOB tau( TCS::ClusterTOB(atoi(results.at(0).c_str()),atoi(results.at(1).c_str()),atoi(results.at(2).c_str()),atoi(results.at(3).c_str())));
         if(results.size()==6) {
            tau.setEtaDouble( atof(results.at(4).c_str()) );
            tau.setPhiDouble( atof(results.at(5).c_str()) );
         }
         m_event->addTau(tau);
      } else if(type == "<jet>") {
         TCS::JetTOB jet( atoi(results.at(0).c_str()),atoi(results.at(1).c_str()),atoi(results.at(2).c_str()),atoi(results.at(3).c_str()) );
         if(results.size()==6) {
            jet.setEtaDouble( atof(results.at(4).c_str()) );
            jet.setPhiDouble( atof(results.at(5).c_str()) );
         }
         m_event->addJet( jet );
      } else if(type == "<muon>") {
         unsigned int et = atoi(results.at(0).c_str());
         int eta = atoi(results.at(1).c_str());
         int phi = atoi(results.at(2).c_str());
         TCS::MuonTOB muon( et, 0, eta, phi );
         if(results.size()==5) {
            muon.setEtaDouble( atof(results.at(3).c_str()) );
            muon.setPhiDouble( atof(results.at(4).c_str()) );
         }
         m_event->addMuon( muon );
      } else if(type == "<lateMuon>") {
         unsigned int et = atoi(results.at(0).c_str());
         int eta = atoi(results.at(1).c_str());
         int phi = atoi(results.at(2).c_str());
         TCS::LateMuonTOB latemuon( et, 0, eta, phi );
         if(results.size()==5) {
            latemuon.setEtaDouble( atof(results.at(3).c_str()) );
            latemuon.setPhiDouble( atof(results.at(4).c_str()) );
         }
         m_event->addLateMuon( latemuon );
      } else if(type == "<muonNextBC>") {
         unsigned int et = atoi(results.at(0).c_str());
         int eta = atoi(results.at(1).c_str());
         int phi = atoi(results.at(2).c_str());
         TCS::MuonNextBCTOB nextbcmuon( et, 0, eta, phi );
         if(results.size()==5) {
            nextbcmuon.setEtaDouble( atof(results.at(3).c_str()) );
            nextbcmuon.setPhiDouble( atof(results.at(4).c_str()) );
         }
         m_event->addMuonNextBC( nextbcmuon );
      } else if(type == "<met>") {
         int ex = atoi(results.at(0).c_str());
         int ey = atoi(results.at(1).c_str());
         int et = atoi(results.at(2).c_str());
         TCS::MetTOB met( ex, ey, et );
         m_event->setMET( met );
      } else if(type == "<info>") {
         uint32_t runNo = atol(results.at(0).c_str());
         uint32_t evtNo = atol(results.at(1).c_str());
         uint32_t lumiB = atol(results.at(2).c_str());
         uint32_t BCID  = atol(results.at(3).c_str());
         m_event->setEventInfo(runNo, evtNo, lumiB, BCID);
      } else {
          TCS_EXCEPTION( "TOB for this event is of unknown type " << type<<": '"<<currentLine<<"'");

      }
   } // end read event
  
   m_nEvents+=1;
  
   return true;
}

void TCS::TopoASCIIReader::printEvent() {
   cout << "Print Event" << endl;
   if(m_event != NULL) {
      cout << *m_event << endl;
   } else {
      TCS_EXCEPTION( "event returns NULL value(s) wehn trying to print!" );
   }
}

void TCS::TopoASCIIReader::printFileSummary() {
  cout << "File summary for" << m_inputFile << endl;
  cout << "Number of Events: " << m_nEvents << endl;
}

void TCS::TopoASCIIReader::reset() {
  cout << "TopoASCIIReader reset" << endl;
  m_fs.close();
  m_inputFile="";
  m_event=NULL;
  m_verbosity=0;
  m_nEvents=0;
}
