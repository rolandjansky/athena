/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 Authors: John Alison (johnda@hep.upenn.edu)

 Date: 22 Aug 2008

 Description: This algorithm loops over the Inner Detector elements 
   and prints thier local frames to a text file (IDLocalFrames.txt)

*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include <iostream>
#include <fstream>
#include "CLHEP/Geometry/Transform3D.h"
//#include "CLHEP/Matrix/Vector.h"

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"


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
class GetDetectorLocalFrames:public AthAlgorithm {

 public:
    GetDetectorLocalFrames (const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

 private:
    
    /** Functions to write the data */
    void writePixelFames();
    void writeSCTFrames();
    void writeTRTFrames();
    void writeTransForm(const HepGeom::Transform3D& transform);
    void writeVector(std::string name, const Amg::Vector3D& vector);

    /** Name of the Output file */
    std::string m_outputFileName;
    std::ofstream m_outputFile;
    
    /** Pixel Data */
    
    /** SCT Data */
    
    /** TRT Data */
    int m_trt_barrel_ec;
    int m_trt_layer_or_wheel;
    int m_trt_phi_module;
    int m_trt_straw_layer;
    int m_trt_straw;
    
    /** Tools */
    const PixelID *m_PixelHelper;
    const InDetDD::PixelDetectorManager *m_pixelDetectorManager;
    
    const SCT_ID *m_SCTHelper;
    const InDetDD::SCT_DetectorManager *m_SCTDetectorManager;

    const TRT_ID *m_TRTHelper;
    const InDetDD::TRT_DetectorManager *m_TRTDetectorManager;
    
    
    
    
};
