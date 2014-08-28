/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoCommon/Exception.h"

#include <iostream>

using namespace std;
using namespace TXC;

L1TopoMenu::L1TopoMenu() {}

L1TopoMenu::L1TopoMenu(const std::string & name, const std::string & version) :
   m_name(name),
   m_version(version)
{}  

// L1TopoMenu::L1TopoMenu(const L1TopoMenu & o) :
//    m_name(o.m_name),
//    m_version(o.m_version),
//    m_algos(o.m_algos),
//    m_topoconfig(o.m_topoconfig),
//    m_outputlist(o.m_outputlist)
// {}


L1TopoMenu::L1TopoMenu(L1TopoMenu&& o) :
   m_name( std::move(o.m_name) ),
   m_version( std::move(o.m_version) ),
   m_algos( std::move(o.m_algos) ),
   m_topoconfig( std::move(o.m_topoconfig) ),
   m_outputlist( std::move(o.m_outputlist) )
{}


L1TopoMenu::~L1TopoMenu() {}

L1TopoMenu& 
L1TopoMenu::operator=(L1TopoMenu&& o) {
   m_name = std::move(o.m_name);
   m_version = std::move(o.m_version);
   m_algos = std::move(o.m_algos);
   m_topoconfig = std::move(o.m_topoconfig);
   m_outputlist = std::move(o.m_outputlist);
   return *this;
}

void
L1TopoMenu::addAlgorithm(const TXC::L1TopoConfigAlg &alg) {
   m_algos.push_back(alg);
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

