/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// HGTDSensorSDTool.h, (c) ATLAS Detector software
//////////////////////////////////////////////////////////

#ifndef HGTD_G4_SD_HGTDSENSORSDTOOL_H
#define HGTD_G4_SD_HGTDSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// for the sensitive detector
#include "HGTDSensorSD.h"
#include "CxxUtils/make_unique.h"

// STL headers
#include <string>

class G4VSensitiveDetector;
class HGTD_ID;

class HGTDSensorSDTool : public SensitiveDetectorBase {

public:

    // Constructor
    HGTDSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

    virtual StatusCode initialize() override final;

    // Make me an SD!
    virtual G4VSensitiveDetector* makeSD() override final;

    // Destructor
    virtual ~HGTDSensorSDTool() {}

protected:

    void setupAllSDs(std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration) const;
    bool match(const char *first, const char * second) const;

private:

    // the actual sensitive detector
    std::unique_ptr<HGTDSensorSD> m_hgtdSD;

};

#endif // HGTD_G4_SD_HGTDSENSORSDTOOL_H
