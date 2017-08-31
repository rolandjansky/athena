/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 Authors: John Alison (johnda@hep.upenn.edu)

 Date: 22 Aug 2008

 Description: This algorithm loops over the Inner Detector elements 
   and prints thier global positions to a text file (IDgeometryTextFile.txt)

*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include <iostream>
#include <fstream>

namespace InDetDD {
  class TRT_DetectorManager; 
  class SCT_DetectorManager; 
  class PixelDetectorManager; 
}

class TRT_ID;
class SCT_ID;
class PixelID;
class Identifier;

/////////////////////////////////////////////////////////////////////////////
class GetDetectorPositions:public AthAlgorithm {

 public:
    GetDetectorPositions (const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

 private:
    
    /** Functions to get the data */
    void writePixelPositions();
    void writeSCTPositions();
    void writeTRTPositions();

    void writeSimpleTRTPositions();
    void writeDetailedTRTPositions(bool doDoubleSampling=false);
    void writeTRTPositions(Identifier const& trtId);
    void writeTwoTRTPositions(Identifier const& trtId);

    /** Name of the Output file */
    unsigned int m_detailLevel;
    bool         m_doTRT;
    std::string m_outputFileName;
    std::ofstream m_outputFile;
    
    /** Tools */
    const PixelID *m_PixelHelper;
    const InDetDD::PixelDetectorManager *m_pixelDetectorManager;
    
    const SCT_ID *m_SCTHelper;
    const InDetDD::SCT_DetectorManager *m_SCTDetectorManager;

    const TRT_ID *m_TRTHelper;
    const InDetDD::TRT_DetectorManager *m_TRTDetectorManager;
    
    
    
    
};
