/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelXml/GmxUtil.h"

#include "GeoModelXml/OutputDirector.h"

#ifndef GEOMODEL_STANDALONE_GMX
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#endif

#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelXml/GmxInterface.h"

#include <cstdlib>
#include <sstream>
#include <cctype>
#include <limits>

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
    geoItemRegistry.enregister("simplepolygonbrep", (Element2GeoItem *) &tagHandler.simplepolygonbrep);
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
    static string lastGoodExpression("none"); // It is helpful for debugging to get some idea of where we had got to...
    string strExpression(expression);

    bool isWhiteSpace = true;
    for(unsigned int i = 0; i < strExpression.length(); ++i){
        if(!std::isspace(strExpression[i])) {
            isWhiteSpace = false;
            break;
        }
    }
    if (isWhiteSpace) { // Catch a common error early and give best message possible
	OUTPUT_STREAM;
        msglog << MSG::FATAL << "GeoModelXml Error processing Evaluator expression: empty expression. Exiting program.\n" << 
               endmsg;
        throw runtime_error(string("evaluate: empty or white space expression. Last good expression was " + lastGoodExpression));

    }
//
//    Process any []s. Contents are evaluated to in integer, then the [...] are replaced by 
//    string representation of that integer. Idea from Joakim Gradin. Re-coded by Nigel.
//    Allows array elements to be accessed as array_[col-expression]_[row-expression]. 
//
    string noBrackets = debracket(strExpression);
//
//   And evaluate the result
//
    double result = eval.evaluate(noBrackets.c_str());
    if (eval.status() != Evaluator::OK) {
	OUTPUT_STREAM;
        msglog << MSG::FATAL << "GeoModelXml Error processing Evaluator expression. Error name <" <<
         eval.error_name() << ">" << endl << "Message: <";
        eval.print_error();
        msglog << ">. Original expression <" << expression << ">; Expression after de-bracketing:\n";
        msglog << noBrackets << endl;
        msglog << string(eval.error_position(), '-') << '^' << '\n';
        msglog << "Exiting program.\n" << endmsg;
        throw runtime_error(string("evaluate: invalid expression. Last good expression was <" + lastGoodExpression + ">"));
    }
    lastGoodExpression = strExpression;
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
	OUTPUT_STREAM;
        msglog << MSG::ERROR << "debracket: unpaired opening [; expression was:\n    " << expression << endmsg; 
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
#ifndef GEOMODEL_STANDALONE_GMX
    StoreGateSvc *pDetStore = 0;
    ISvcLocator *svcLocator = Gaudi::svcLocator();

    OUTPUT_STREAM;

    StatusCode sc = svcLocator->service("DetectorStore", pDetStore);
    if (sc.isFailure()) {
            msglog << MSG::ERROR << "GmxUtil::makeAssemblyLV: Unable to access Detector Store" << endmsg;
    }
    else {
        DataHandle<StoredMaterialManager> theMaterialManager;
        sc = pDetStore->retrieve(theMaterialManager, "MATERIALS");
        if(sc.isFailure()) {
                msglog << MSG::ERROR << "GmxUtil::makeAssemblyLV: Unable to access Material Manager" << endmsg;
        }
        else {
            const GeoMaterial *assembly_material = theMaterialManager->getMaterial("special::HyperUranium");
            GeoBox *box = new GeoBox(1., 1., 1.); // Simplest shape; it is irrelevant
            GeoLogVol *lv = new GeoLogVol(string("AssemblyLV"), box, assembly_material);
            return lv;
        }
    }
    return 0;
#else
    const GeoMaterial *assembly_material = new GeoMaterial("special::HyperUranium", 1.e-20);
    GeoElement *vacuum = new GeoElement("vacuum", "Mt", 1, 1);
    assembly_material->add(vacuum, 1.0);
    assembly_material->lock();
    GeoBox *box = new GeoBox(1., 1., 1.); // Simplest shape; it is irrelevant
    GeoLogVol *lv = new GeoLogVol(string("AssemblyLV"), box, assembly_material);
    return lv;
#endif
}
