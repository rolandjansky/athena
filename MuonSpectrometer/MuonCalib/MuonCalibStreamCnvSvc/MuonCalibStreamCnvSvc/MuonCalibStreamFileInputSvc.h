/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSTREAMCNVSVC_MUONCALIBSTREAMFILEINPUTSVC_H 
#define MUONCALIBSTREAMCNVSVC_MUONCALIBSTREAMFILEINPUTSVC_H 

//===================================================================
//     MuonCalibStreamFileInputSvc.h
//===================================================================
//
// Description: This class implements the interface MuonCalibStreamInputSvc for 
//              event selector to read the events for Files.
//
//-------------------------------------------------------------------


// Include files.
#include "MuonCalibStreamCnvSvc/MuonCalibStreamInputSvc.h"
#include "MuCalDecode/CalibEvent.h" 
#include "MuCalDecode/CalibUti.h"
#include "MuCalDecode/CalibDataLoader.h" 
#include "MuCalDecode/DataBuffer.h" 
#include "EventStorage/DataReader.h"
#include <fstream>

class MuonCalibStreamFileInputSvc: public MuonCalibStreamInputSvc
{

public:



   // Constructors:
   MuonCalibStreamFileInputSvc(const std::string& name, ISvcLocator* svcloc);

   // Destructor.
   virtual ~MuonCalibStreamFileInputSvc();
    
   // Implementation of the MuonCalibStreamInputSvc interface methods.
   virtual StatusCode initialize();

   virtual const LVL2_MUON_CALIBRATION::CalibEvent* nextEvent();
   virtual const LVL2_MUON_CALIBRATION::CalibEvent* currentEvent() const;

private:
    
   int                                 m_fileEventCounter;    
   int                                 m_totalEventCounter;
   int m_DumpStream;

   DataReader*        m_reader;

   bool m_EoF;

   LVL2_MUON_CALIBRATION::CalibEvent*			       m_re;
   LVL2_MUON_CALIBRATION::CalibDataLoader * m_dataLoader;
   DataBuffer m_dataBuffer;
   std::vector<std::string>                      m_inputFiles;
   std::vector<std::string>::iterator            m_inputFilesIt;

};

#endif  
