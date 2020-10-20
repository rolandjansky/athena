/*
  Copyright (C) 2002-20120 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HGTD_IdDictSvc.h
// Tool to generate HGTD part of InnerDetector IdDict on-the-fly
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_HGTD_IDDICTSVC_H
#define INDETTRACKINGGEOMETRY_HGTD_IDDICTSVC_H

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
#include <InDetTrackingGeometryXML/XMLGeoTemplates.h>
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

using namespace xercesc;

static const InterfaceID IID_HGTD_IdDictSvc("HGTD_IdDictSvc", 1, 0);

class HGTD_IdDictSvc : public AthService, public InDet::XMLUtils {
public:
    HGTD_IdDictSvc(const std::string& name, ISvcLocator* svc);
    virtual ~HGTD_IdDictSvc();

    // Interface ID for python stuff to work
    static const InterfaceID& interfaceID() { return IID_HGTD_IdDictSvc; }
    StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    // AlgTool initialize and finalize methods
    StatusCode initialize();
    StatusCode finalize();

private:
    // ID Dict generation
    void writeDictionary(std::string filename);
    void openDictFile(std::ofstream& file,std::string filename) const;
    void closeDictFile(std::ofstream& file) const;
    void writeHgtdDict(std::ofstream& file);

    // Init stage to be done after initialization!
    void retrieveTools();

    // Data members
    std::string m_dictionaryFileName;

    bool m_doHGTD, m_createDict;
};

#endif //INDETTRACKINGGEOMETRY_HGTD_IDDICTSVC_H
