///////////////////////////////////////////////////////////////////
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
// Tool to fill templates from XML files
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_GMXREADERSVC_H
#define INDETTRACKINGGEOMETRY_GMXREADERSVC_H

// Gaudi
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
// STD
#include <fstream>
#include <vector>
// XML library
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
// Trk tools library
#include <InDetTrackingGeometryXML/XMLUtils.h>
#include <InDetTrackingGeometryXML/GMXGeoTemplates.h>

using namespace xercesc;


namespace InDet {
  static const InterfaceID IID_GMXReaderSvc("InDetGMXReaderSvc", 1, 0);

  class GMXReaderSvc : public AthService, public XMLUtils {
  public:
    GMXReaderSvc(const std::string& name, ISvcLocator* svc);
    virtual ~GMXReaderSvc();

    // Interface ID for python stuff to work
    static const InterfaceID& interfaceID() { return IID_GMXReaderSvc; }
    StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    // AlgTool initialize and finalize methods
    StatusCode initialize();
    StatusCode finalize();
    
  private:
    
    void writeDictionary(std::string filename);
    void openDictFile(std::ofstream& file,std::string filename) const;
    void closeDictFile(std::ofstream& file) const;
    
    void getInputGMX(std::string& gmxInput);
    void writeGMXDictionary(std::ofstream& file);

    void getVariables(DOMDocument *doc);
    std::vector<double> returnVector(char* val);
    double returnDouble(char* val);

    void CreateBarrelLayers(std::vector<double> Barrel_NumberOfEta, std::vector<double> Barrel_NumberOfPhi);
    void CreateEndcapDiscs(double Endcap_Wheels, std::vector<double> Endcap_NumberOfPhi);
    void LinkSensors( std::vector<SensorClass*> sensors, std::map < std::string, std::vector <int> > barrel_indices );
    
    // Data members
    std::string m_dictionaryFileName;
    std::string m_gmxInputName;

    std::vector< SensorClass* > m_tmp_BarrelSensors;
    std::vector< SensorClass* > m_tmp_EndcapSensors;
    std::vector< LayerDiscClass*> m_tmp_BarrelLayers; 
    std::vector< LayerDiscClass*> m_tmp_EndcapDiscs;

    bool m_createDict;
    
    bool m_addBCL;

    bool m_useDb;

    bool m_doHGTD;

    std::string m_pathToGMXFile;
  };
}

#endif //INDETTRACKINGGEOMETRY_XMLREADERSVC

