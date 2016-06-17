/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/GmxUtil.h"
#ifndef STANDALONE_GMX
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#endif
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelXml/GmxInterface.h"

#include <iostream>
#include <cstdlib>
#include <sstream>

using namespace std;

GmxUtil::GmxUtil(GmxInterface &gmxInterface) {
    m_gmxInterface = &gmxInterface;
//
//    Initialise the CLHEP::Evaluator
//
    eval.setStdMath();
//
//    Fetch the material special::HyperUranium and make any shape with it to make a special logvol.
//    When Geo2G4 finds any physvol using a logvol with material special::HyperUranium, it creates a G4Assembly
//    instead of a G4Volume.
//
    m_assemblyLV = makeAssemblyLV();
//
//   Register tag handlers that produce a vector of items to add to the tree.
//
    processorRegistry.enregister("addbranch", &tagHandler.addbranch);
    processorRegistry.enregister("logvol", &tagHandler.logvol);
    processorRegistry.enregister("logvolref", &tagHandler.logvolref);
    processorRegistry.enregister("assembly", &tagHandler.assembly);
    processorRegistry.enregister("assemblyref", &tagHandler.assemblyref);
    processorRegistry.enregister("transform", &tagHandler.transform);
    processorRegistry.enregister("multicopy", &tagHandler.multicopy);
    processorRegistry.enregister("index", &tagHandler.index);
//
//   Register tag handlers that produce GeoNodes. Only useful for those tags which
//   can appear in a long list in any order. So certainly all shapes; maybe others.
//
    geoItemRegistry.enregister("box", (Element2GeoItem *) &tagHandler.box);
    geoItemRegistry.enregister("cons", (Element2GeoItem *) &tagHandler.cons);
    geoItemRegistry.enregister("generictrap", (Element2GeoItem *) &tagHandler.generictrap);
    geoItemRegistry.enregister("para", (Element2GeoItem *) &tagHandler.para);
    geoItemRegistry.enregister("pcon", (Element2GeoItem *) &tagHandler.pcon);
    geoItemRegistry.enregister("pgon", (Element2GeoItem *) &tagHandler.pgon);
    geoItemRegistry.enregister("trap", (Element2GeoItem *) &tagHandler.trap);
    geoItemRegistry.enregister("trd", (Element2GeoItem *) &tagHandler.trd);
    geoItemRegistry.enregister("tube", (Element2GeoItem *) &tagHandler.tube);
    geoItemRegistry.enregister("tubs", (Element2GeoItem *) &tagHandler.tubs);
    geoItemRegistry.enregister("intersection", (Element2GeoItem *) &tagHandler.intersection);
    geoItemRegistry.enregister("union", (Element2GeoItem *) &tagHandler.onion); // union is reserved
    geoItemRegistry.enregister("subtraction", (Element2GeoItem *) &tagHandler.subtraction);
    geoItemRegistry.enregister("shaperef", (Element2GeoItem *) &tagHandler.shaperef);
    geoItemRegistry.enregister("transformation", (Element2GeoItem *) &tagHandler.transformation);
    geoItemRegistry.enregister("transformationref", (Element2GeoItem *) &tagHandler.transformationref);
}

GmxUtil::~GmxUtil() {
}

GmxInterface * GmxUtil::gmxInterface() {
    return m_gmxInterface;
}

double GmxUtil::evaluate(char const *expression) {
//
//    Process any []s. Contents are evaluated to in integer, then the [...] are replaced by 
//    string representation of that integer. Idea from Joakim Gradin. Re-coded by Nigel.
//    Allows array elements to be accessed as array_[col-expression]_[row-expression]. 
//
    string strExpression(expression);
    string noBrackets = debracket(strExpression);
//
//   And evaluate the result
//
    double result = eval.evaluate(noBrackets.c_str());
    if (eval.status() != HepTool::Evaluator::OK) {
        cerr << "GeoModelXml Error processing CLHEP Evaluator expression. Error name " <<
         eval.error_name() << endl << "Message: ";
        eval.print_error();
        cerr << "original expression" << expression << "; Expression after de-bracketing:\n";
        cerr << noBrackets << endl;
        cerr << string(eval.error_position(), '-') << '^' << '\n';
        cerr << "Exiting program.\n";
        exit(999); // Should do better...
    }
    return result;
}

std::string GmxUtil::debracket(std::string expression) {
//
//    Get the last brackets contents and evaluate it
//
    size_t lastOpen = expression.find_last_of('[');
    if (lastOpen == string::npos) { // None found
        return expression; // Done
    }
    size_t nextClose = expression.find_first_of(']', lastOpen);
    if (nextClose == string::npos) {
        cerr << "debracket: unpaired opening [; expression was:\n    " << expression << endl; 
        return expression;
    }
    string toEvaluate = expression.substr(lastOpen + 1, nextClose - lastOpen - 1);
//
//    Evaluate it
//
    stringstream result;
    result << (int) eval.evaluate(toEvaluate.c_str());
//
//    Replace [sub-expr] with result
//
    string newExpression = expression.substr(0, lastOpen) + result.str() + 
                           expression.substr(nextClose + 1, string::npos);

    return debracket(newExpression);
}

GeoLogVol * GmxUtil::makeAssemblyLV() {
#ifndef STANDALONE_GMX
    StoreGateSvc *pDetStore = 0;
    IMessageSvc *msgSvc = 0;
    ISvcLocator *svcLocator = Gaudi::svcLocator();

    StatusCode sc = svcLocator->service("MessageSvc", msgSvc);

    if(sc.isFailure()) {
        std::cerr << "GmxUtil::makeAssemblyLV(): unable to access Message Service\n";
    }
    else {
        MsgStream log(msgSvc,"EPVolumeBuilder");
        sc=svcLocator->service("DetectorStore", pDetStore);
        if(sc.isFailure()) {
            log << MSG::ERROR << "GmxUtil::makeAssemblyLV: Unable to access Detector Store" << endreq;
        }
        else {
            DataHandle<StoredMaterialManager> theMaterialManager;
            sc = pDetStore->retrieve(theMaterialManager, "MATERIALS");
            if(sc.isFailure()) {
                log << MSG::ERROR << "GmxUtil::makeAssemblyLV: Unable to access Material Manager" << endreq;
            }
            else {
                GeoMaterial *assembly_material = theMaterialManager->getMaterial("special::HyperUranium");
                GeoBox *box = new GeoBox(1., 1., 1.); // Simplest shape; it is irrelevant
                GeoLogVol *lv = new GeoLogVol(string("AssemblyLV"), box, assembly_material);
                return lv;
            }
        }
    }
    return 0;
#else
                GeoMaterial *assembly_material = new GeoMaterial("special::HyperUranium", 1.e-20);
                GeoElement *vacuum = new GeoElement("vacuum", "Mt", 1, 1);
                ether->add(vacuum, 1.0);
                ether->lock();
                GeoBox *box = new GeoBox(1., 1., 1.); // Simplest shape; it is irrelevant
                GeoLogVol *lv = new GeoLogVol(string("AssemblyLV"), box, assembly_material);
                return lv;
#endif
}
