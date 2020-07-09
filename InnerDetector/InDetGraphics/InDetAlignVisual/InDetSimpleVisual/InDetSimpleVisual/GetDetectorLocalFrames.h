/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** 
 Authors: John Alison (johnda@hep.upenn.edu)

 Date: 22 Aug 2008

 Description: This algorithm loops over the Inner Detector elements 
   and prints thier local frames to a text file (IDLocalFrames.txt)

*/

#include "AthenaBaseComps/AthAlgorithm.h"

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "CLHEP/Geometry/Transform3D.h"

#include <string>
#include <iostream>
#include <fstream>

namespace InDetDD {
  class TRT_DetectorManager; 
}

class TRT_ID;
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
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
    
    /** SCT Data */
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
    
    /** TRT Data */
    int m_trt_barrel_ec;
    int m_trt_layer_or_wheel;
    int m_trt_phi_module;
    int m_trt_straw_layer;
    int m_trt_straw;
    
    /** Tools */
    const TRT_ID *m_TRTHelper;
    const InDetDD::TRT_DetectorManager *m_TRTDetectorManager;
    
    /** Counter */
    int m_eventCount{-1};
    
};
