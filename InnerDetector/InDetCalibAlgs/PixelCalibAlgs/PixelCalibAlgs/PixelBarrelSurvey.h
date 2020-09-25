/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "PixelCalibAlgs/PixelBarrelSurveyUtils.h"
#include "PixelCalibAlgs/PixelConvert.h"
#include "StoreGate/ReadCondHandleKey.h"
#include <vector>
#include <string>


class PixelID;
class IToolSvc;
class AlignableTransform;
class DetCondCFloat;


/////////////////////////////////////////////////////////////////////////////

class PixelBarrelSurvey:public AthAlgorithm {

 public:
  PixelBarrelSurvey (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  std::string m_mappingDataFile;
  std::string m_surveyDataFile;
  std::string m_tag;
  bool m_oldOutputFormat;
  std::string m_outputDataFile;
  std::string m_outputDistoFile;
  std::vector<PixelBarrelSurveyUtils::StaveStruct> m_staves;
  const PixelID *m_pixelID;     // the ID helper
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
  AlignableTransform *m_transforms;
  DetCondCFloat *m_distorsions;
  PixelConvert::Map m_map;
};
 
