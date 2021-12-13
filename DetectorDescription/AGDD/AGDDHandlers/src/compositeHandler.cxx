/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDHandlers/compositeHandler.h"
#include "AGDDHandlers/addmaterialHandler.h"
#include "AGDDHandlers/natomsHandler.h"
#include "AGDDHandlers/fractionmassHandler.h"
#include "AGDDControl/XercesParser.h"
#include "AGDDControl/XMLHandlerStore.h"
#include "AGDDControl/AGDDController.h"
#include "AGDDModel/AGDDMolecule.h"
#include "AGDDModel/AGDDMixture.h"
#include <iostream>

using namespace xercesc;

compositeHandler::compositeHandler(const std::string& s,
                                   AGDDController& c)
  : XMLHandler(s, c)
{
}

void compositeHandler::ElementHandle(AGDDController& c,
                                     xercesc::DOMNode *t)
{
    std::string name=getAttributeAsString(c, t, "name");
    double density=getAttributeAsDouble(c, t, "density");
    StopLoop(true);
	
    addmaterialHandler* addmatHand = dynamic_cast<addmaterialHandler*>
      (c.GetHandlerStore().GetHandler("addmaterial"));
    if (!addmatHand) std::abort();

    natomsHandler* natomsHand = dynamic_cast<natomsHandler*>
      (c.GetHandlerStore().GetHandler("natoms"));
    if (!natomsHand) std::abort();

    fractionmassHandler* fractionsHand = dynamic_cast<fractionmassHandler*>
      (c.GetHandlerStore().GetHandler("fractionmass"));
    if (!fractionsHand) std::abort();

    addmatHand->GetNames();
    natomsHand->GetNatoms();
    fractionsHand->GetFractions();

    DOMNode* child;

    IAGDDParser& parser = *c.GetParser();
    for (child=t->getFirstChild();child!=0;child=child->getNextSibling())
    {
        if (child->getNodeType()==DOMNode::ELEMENT_NODE) {
          parser.elementLoop(c, child);
        }
    }
	
    std::vector<std::string> names = addmatHand->GetNames();
    std::vector<int> natoms = natomsHand->GetNatoms();
    std::vector<double> fractions = fractionsHand->GetFractions();

    if (!natoms.empty())
    {
      // it's a molecule
      AGDDMolecule *m=new AGDDMolecule(c.GetMaterialStore(),name,density);
      for (unsigned int i=0;i<natoms.size();i++)
      {
        m->AddElement(c.GetMaterialStore(), names[i]);
        m->NAtoms(natoms[i]);
      }
    }
    else if (!fractions.empty())
    {
      // it's a mixture
      AGDDMixture *m=new AGDDMixture(c.GetMaterialStore(),name,density);
      for (unsigned int i=0;i<fractions.size();i++)
      {
        m->AddMaterial(c.GetMaterialStore(), names[i]);
        m->Fraction(fractions[i]);
      }
    }
    else
      std::cout<<"Something wrong: "<<name<<" neither molecule nor mixture!!!"<<std::endl;
}
