/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTest/LArReadHadDMCoeffFile.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"


LArReadHadDMCoeffFile::LArReadHadDMCoeffFile(const std::string & name, 
					     ISvcLocator * pSvcLocator) : 
  Algorithm(name,pSvcLocator) {
  declareProperty("HadDMCoeffFileName",m_hadDMCoeffFileName);
  declareProperty("CorrectionKey",m_key="HadDMCoeff");

  m_data = 0;
}


LArReadHadDMCoeffFile::~LArReadHadDMCoeffFile() {}

                            
StatusCode LArReadHadDMCoeffFile::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "=== LArReadHadDMCoeffFile::initialize() ===" << endreq;
  StatusCode sc;
  StoreGateSvc* detStore;
  sc=service("DetectorStore",detStore);
  if (sc.isFailure()) {
     log << MSG::ERROR << "Unable to get the DetectorStore" << endreq;
     return sc;
   }
   initDataFromFile(m_hadDMCoeffFileName);

   int outputLevel = msgSvc()->outputLevel( name() );
   if(outputLevel == MSG::DEBUG) m_data->PrintData();

   sc=detStore->record(m_data,m_key);
   if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to record CaloHadWeight" << endreq;
      return sc;
   }
   sc=detStore->setConst(m_data);
   if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to lock CaloHadWeight" << endreq;
      return sc;
   }
   return StatusCode::SUCCESS;
}


StatusCode  LArReadHadDMCoeffFile::execute() 
{ 
  return StatusCode::SUCCESS;
}

StatusCode  LArReadHadDMCoeffFile::finalize() 
{ 
  return StatusCode::SUCCESS;
}


StatusCode LArReadHadDMCoeffFile::initDataFromFile(std::string hadDMCoeffFileName)
{
   MsgStream log(msgSvc(), name());

   m_data = new CaloHadDMCoeff();

   // Find the full path to filename
   std::string file = PathResolver::find_file (hadDMCoeffFileName, "DATAPATH");
   log << MSG::INFO << "Reading file  " << file << endreq;

   FILE *fin = fopen(file.c_str(),"r");
   if ( fin == NULL) {
      log << MSG::ERROR << "Can't open file " << file << endreq;
      return StatusCode::FAILURE;
   }

   char line[1024], title[128];
   int m_offset = 0;
   int indx_zone = 0, indx_parset = 0;
   int nFrac, nEner, nEta, nPars;
   while(fgets(line,1024,fin)) {
      if(line[0] == '#' || line[0] == '\n' ) continue;
      int _izone, _is_on;
      if(sscanf(line,"%d %d %s\n",&_izone, &_is_on, title)!=3 || indx_zone != _izone) {
         log << MSG::ERROR << "Format error #1." << endreq;
         fclose (fin);
         return StatusCode::FAILURE;         
      }
      CaloHadDMCoeff::HadDMArea dmArea;
      dmArea.m_is_on = _is_on;
      dmArea.m_indx = indx_zone;
      dmArea.m_title = title;

      fgets(line,1024,fin);
      if(sscanf(line,"%d %f %f\n", &nFrac, &dmArea.m_MinFrac, &dmArea.m_MaxFrac)!=3 ||
         nFrac<0 || nFrac>1000)
      {
         log << MSG::ERROR << "Format error #2." << endreq;
         fclose (fin);
         return StatusCode::FAILURE;
      }
      fgets(line,1024,fin);
      if(sscanf(line,"%d %f %f\n", &nEner, &dmArea.m_MinEner, &dmArea.m_MaxEner)!=3 ||
         nEner < 0 || nEner > 1000)
      {
         log << MSG::ERROR << "Format error #3." << endreq;
         fclose (fin);
         return StatusCode::FAILURE;         
      }
      fgets(line,1024,fin);
      if(sscanf(line,"%d %f %f\n", &nEta, &dmArea.m_MinEta, &dmArea.m_MaxEta)!=3 ||
         nEta < 0 || nEta > 1000)
      {
         log << MSG::ERROR << "Format error #4." << endreq;
         fclose (fin);
         return StatusCode::FAILURE;         
      }
      fgets(line,1024,fin);
      if(sscanf(line,"%d \n", &nPars)!=1 || nPars < 0 || nPars> 1000) {
         log << MSG::ERROR << "Format error #5." << endreq;
         fclose (fin);
         return StatusCode::FAILURE;         
      }
      dmArea.m_nPars = nPars;
      
      for(int i_frac=0; i_frac<nFrac; i_frac++){
         int _iparset, _ifrac, _iener, _ieta;
         float _frac, _ener, _eta;
         for(int i_ener=0; i_ener<nEner; i_ener++){
            for(int i_eta=0; i_eta<nEta; i_eta++){
               fgets(line,1024,fin);
               std::string sLine;
               sLine = std::string(line);
               std::istringstream es( sLine.c_str() );

               if ( es >> _iparset >> _ifrac >> _iener >> _ieta >> _frac >> _ener >> _eta) {
                  CaloHadDMCoeff::HadDMCoeff pars;
                  pars.resize(nPars);
                  for(int i=0; i<nPars; i++) {
                     if(!(es >> pars[i]) ) {
                        log << MSG::ERROR << "Format error #6." << endreq;
                        fclose (fin);
                        return StatusCode::FAILURE;         
                     }
                  }
                  if(_ifrac != i_frac || _iener != i_ener || _ieta != i_eta || (int)pars.size() != nPars){
                     log << MSG::ERROR << "Format error #7." << endreq;
                     fclose (fin);
                     return StatusCode::FAILURE;
                  }

                  if(i_frac==0 && i_ener==0) dmArea.m_EtaBins.push_back(_eta);
                  m_data->addHadDMCoeff(pars);
                  pars.clear();
                  indx_parset++;
               } else {
                     log << MSG::ERROR << "Format error #8." << endreq;
               }
            } // i_eta
            if(i_frac == 0) dmArea.m_EnerBins.push_back(_ener);
         } // i_ener
         dmArea.m_FracBins.push_back(_frac);
      }
      dmArea.m_offset = m_offset;
//      std::cout << "indx_zone: " << indx_zone << " m_offset:" << m_offset << std::endl;
      m_data->addHadDMArea(dmArea);
      m_offset += nFrac*nEner*nEta; // ofset for next DM area
      indx_zone++;
   }
   fclose(fin);

  return StatusCode::SUCCESS;
}
