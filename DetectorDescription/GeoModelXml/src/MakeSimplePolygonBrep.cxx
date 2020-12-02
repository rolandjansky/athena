/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "GeoModelXml/OutputDirector.h"

#include "GeoModelXml/shape/MakeSimplePolygonBrep.h"
#include <xercesc/dom/DOM.hpp>
#include "GeoModelKernel/RCBase.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "xercesc/util/XMLString.hpp"
#include "GeoModelXml/GmxUtil.h"
using namespace xercesc;

MakeSimplePolygonBrep::MakeSimplePolygonBrep() {}

const RCBase * MakeSimplePolygonBrep::make(const xercesc::DOMElement *element, GmxUtil &gmxUtil) const {
const int nParams = 3; 
char const *parName[nParams] = {"xpoints", "ypoints", "zhalflength"};
double z;
std::vector <double> x;
std::vector <double> y;

char *toRelease;

    toRelease = XMLString::transcode(element->getAttribute(XMLString::transcode(parName[0])));
    std::string xPoints(toRelease);
    XMLString::release(&toRelease);

    toRelease = XMLString::transcode(element->getAttribute(XMLString::transcode(parName[1])));
    std::string yPoints(toRelease);
    XMLString::release(&toRelease);

    toRelease = XMLString::transcode(element->getAttribute(XMLString::transcode(parName[2])));
    z = gmxUtil.evaluate(toRelease);
    XMLString::release(&toRelease);

    GeoSimplePolygonBrep * poly = new GeoSimplePolygonBrep(z);

    std::istringstream xSS(xPoints);
    while (!xSS.eof()) {
        std::string upToNextComma;
        getline( xSS, upToNextComma, ';' );
        x.push_back(gmxUtil.evaluate(upToNextComma.c_str()));
    }

    std::istringstream ySS(yPoints);
    while (!ySS.eof()) {
        std::string upToNextComma;
        getline( ySS, upToNextComma, ';' );
        y.push_back(gmxUtil.evaluate(upToNextComma.c_str()));
    }

    int nx = x.size();
    int ny = y.size();
  
    if (nx < 3 || ny < 3 || nx != ny) {
        OUTPUT_STREAM;
        msglog << MSG::ERROR << "MakeSimplePolygonBrep: Bad xpoints and/or ypoints\n";
        throw std::runtime_error(std::string("MakeSimplePolygonBrep: Unequal number of x and y points, or less than 3\n\n") +
         "xpoints was:\n" + xPoints + "\nypoints was:\n" + yPoints + "\n\n");
    }

    for (int i = 0; i < nx; ++i) {
        poly->addVertex(x[i], y[i]);
    }

    return poly;

}
