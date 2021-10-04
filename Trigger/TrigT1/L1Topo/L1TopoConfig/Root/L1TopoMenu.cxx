/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoCommon/Exception.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace TXC;

L1TopoMenu::L1TopoMenu() {}

L1TopoMenu::L1TopoMenu(const std::string & name, const std::string & version) :
   m_name(name),
   m_version(version),
   m_smk(0)
{}  



L1TopoMenu::~L1TopoMenu() {}

void
L1TopoMenu::addAlgorithm(TXC::L1TopoConfigAlg &&alg) {
   m_algos.emplace_back(std::move(alg));
}

const L1TopoConfigAlg &
L1TopoMenu::algorithm(const std::string& name) const {
   for(const L1TopoConfigAlg & alg: m_algos)
      if(alg.name() == name) return alg;
   TCS_EXCEPTION("L1TopoMenu: algorithm of name " << name << " doesn't exist");
}


TCS::StatusCode
L1TopoMenu::setTriggerList() {

   for(const OutputListElement & out: m_outputlist.getOutputList() ) {
      
      const L1TopoConfigAlg & alg = algorithm( out.algoname() );
      
      if(! alg.isDecAlg()) {
         TCS_EXCEPTION("L1TopoMenu: output list contains an algorithm " << out.algoname() << " that is not an decision algorithm");
      }

      unsigned int bit = out.firstbit();
      for (const string& outputname : alg.getOutputNames() ) {
         m_outputlist.addTriggerLine( TriggerLine(outputname, out.algoname(),
                                                  out.algoid(), out.module(), out.fpga(), out.clock(),
                                                  bit++) );
      }
   }

   m_outputlist.sort();

   return TCS::StatusCode::SUCCESS;
}

// print menu
void TXC::L1TopoMenu::print() const {

   cout << "==========================================" << endl
        << "Menu Name   : " << getName() << endl
        << "Menu Version: " << getVersion() << endl
        << m_topoconfig << endl;

   for(const TXC::L1TopoConfigAlg & alg: getL1TopoConfigAlgs())
      cout << alg << endl;

   cout << m_outputlist << endl;

}
