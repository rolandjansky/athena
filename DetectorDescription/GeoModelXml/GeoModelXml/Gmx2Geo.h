/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_GMX2GEO_H
#define GEO_MODEL_XML_GMX2GEO_H

/**
 *  @brief Create a branch of a GeoModel Tree.
 *
 * @author Nigel Hessey, Nikhef
@section GeoModelXmlOverview Class Overview
  The GeoModelXml package contains the following classes:

  - Gmx2Geo : Create an instance of a Gmx2Geo class to do all the work
 * First parameter is the name of the file containing the xml
 * Second parameter is the position in the ATLAS GeoModel tree at which your geometry is to be inserted
 * Third parameter gives a set of call back routines for dealing with sensitive and alignable volumes.
 *
 */

//
//    Gmx2Geo is the main class needed to set-up a GeoModel graph branch using 
//    xml (in a .gmx file).
//    The user instantiates one of these, giving the name of the file and where in the 
//    GeoModel graph the volumes are to be added, and a GmxInterface object. The constructor does the rest,
//    adding solids and materials etc. according to the .gmx file.
//
//    Use:
//        #include "GeoModelXml/Gmx2Geo.h"
//        #include "GeoModelXml/GmxInterface.h"
//            ...create your world volume and do all the Athena setup...
//            ... get the filename with the xml in:
//              std::string xmlfilename; some athena stuff to get filename from the job options...
//            ...create your envelope GeoPhysVol...
//            GeoPhysVol *sct(...);
//            GmxInterface gmxInterface;
//
//            Gmx2Geo gmx2Geo(xmlfilename, sct, gmxInterface);   
//
#include <string>
#include "GeoModelXml/GmxUtil.h"
#include "GeoModelXml/GmxInterface.h"

class GeoPhysVol;

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMLSParser.hpp>

class Evaluator;

class Gmx2Geo {
public:
    Gmx2Geo(const std::string gmxFile, GeoPhysVol *addHere, GmxInterface &gmxInterface, unsigned int flags = 0);
private:
// Disallow copying
    Gmx2Geo(const Gmx2Geo &right);
    Gmx2Geo & operator=(const Gmx2Geo &right);

    int doDefines(xercesc::DOMDocument *doc, Evaluator &eval);
    int doPositionIndex(xercesc::DOMDocument *doc, GmxUtil &gmxUtil);
    int doReadoutGeometry(xercesc::DOMDocument *doc, GmxUtil &gmxUtil);
    void addParam(xercesc::DOMNode *node, std::map<std::string, std::string> &params);
};
#endif // GMX2GEO_H
