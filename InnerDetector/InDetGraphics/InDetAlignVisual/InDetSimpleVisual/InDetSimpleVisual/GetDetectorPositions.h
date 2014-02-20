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
    std::string m_outputFileName;
    std::ofstream outputFile;
    
    /** Pixel Data */
    int m_pix_barrel_ec;
    int m_pix_layer_disk;
    int m_pix_phi_module;
    int m_pix_eta_module;
    float m_pix_x;
    float m_pix_y;
    float m_pix_z;

    /** SCT Data */
    int m_sct_barrel_ec;
    int m_sct_layer_disk;
    int m_sct_phi_module;
    int m_sct_eta_module;
    float m_sct_x;
    float m_sct_y;
    float m_sct_z;

    /** TRT Data */
    int m_trt_barrel_ec;
    int m_trt_layer_or_wheel;
    int m_trt_phi_module;
    int m_trt_straw_layer;
    int m_trt_straw;
    float m_trt_x;
    float m_trt_y;
    float m_trt_z;

    /** Tools */
    const PixelID *m_PixelHelper;
    const InDetDD::PixelDetectorManager *m_pixelDetectorManager;
    
    const SCT_ID *m_SCTHelper;
    const InDetDD::SCT_DetectorManager *m_SCTDetectorManager;

    const TRT_ID *m_TRTHelper;
    const InDetDD::TRT_DetectorManager *m_TRTDetectorManager;
    
    
    
    
};
