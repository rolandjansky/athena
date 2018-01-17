/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_DICT_H
#define DATAMODELATHENAPOOL_DICT_H

#include <vector>
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "DataModelAthenaPool/ElementLink_p2.h"
#include "DataModelAthenaPool/ElementLink_p3.h"
#include "DataModelAthenaPool/ElementLinkContNames_p2.h"
#include "DataModelAthenaPool/DataLink_p1.h"
#include "DataModelAthenaPool/DataLink_p2.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include "DataModelAthenaPool/VectorElementLink_p1.h"
#include "DataModelAthenaPool/Navigable_p1.h"
#include "DataModelAthenaPool/Navigable_p2.h"
#include "DataModelAthenaPool/NavigableVector_p1.h"
#include "DataModelAthenaPool/NavigableVector_p2.h"
#include "DataModelAthenaPool/DataVectorConvert.h"
#include "DataModelAthenaPool/CLHEPConverters.h"
#include "DataModelAthenaPool/PackedContainerConverter.h"
#include "DataModelAthenaPool/AthenaBarCode_p1.h"


namespace {
   struct DMAPdict {
     // template instantiations
     ElementLinkStr_p1 		dummyLS1;
     ElementLinkInt_p1 		dummyLI1;
     ElementLinkStr_p2 		dummyLS2;
     ElementLinkInt_p2 		dummyLI2;
     ElementLinkStrVector_p1 	dummyLSV1;
     ElementLinkIntVector_p1 	dummyLIV1;
     DataLinkVector_p1		dummyDLVp1;
     DataLinkVector_p2		dummyDLVp2;

     ElementLinkIntVector_p1::ElementRef m_elmref_p1_int;
     ElementLinkStrVector_p1::ElementRef m_elmref_p1_str;

     Navigable_p1<unsigned int, void>	navi1;
     Navigable_p1<unsigned int, float>	navi2;
     Navigable_p1<unsigned int, double>	navi3;
     Navigable_p1<std::string, void>	navs1;
     Navigable_p1<std::string, float>	navs2;
     Navigable_p1<std::string, double>	navs3;


     NavigableVector_p1<unsigned int, void>	navvi1;
     NavigableVector_p1<unsigned int, float>	navvi2;
     NavigableVector_p1<unsigned int, double>	navvi3;
     NavigableVector_p1<std::string, void>	navvs1;
     NavigableVector_p1<std::string, float>	navvs2;
     NavigableVector_p1<std::string, double>	navvs3;

     // STL containers
     std::vector<ElementLinkInt_p1> m_elLinkInts_p1;
     std::vector<ElementLinkInt_p2> m_elLinkInts_p2;
     std::vector<ElementLinkIntVector_p1> m_elLinkIntVectors_p1;
     std::vector<ElementLinkStr_p1> m_elLinkStr_p1;
     std::vector<ElementLinkStr_p2> m_elLinkStr_p2;

     AthenaBarCode_p1 m_abcp1;
   };

   
   struct ELp3_dict {
     ElementLinkInt_p3 		dummyLI3;
     ElementLinkStr_p3 		dummyLS3;

     std::vector<ElementLinkInt_p3> m_elLinkInt_p3;
     std::vector<ElementLinkStr_p3> m_elLinkStr_p3;

     Navigable_p2<unsigned int, void>	navi21;
     Navigable_p2<unsigned int, float>	navi22;
     Navigable_p2<unsigned int, double>	navi23;
     Navigable_p2<std::string, void>	navs21;
     Navigable_p2<std::string, float>	navs22;
     Navigable_p2<std::string, double>	navs23;

     NavigableVector_p2<unsigned int, void>	navvi1;
     NavigableVector_p2<unsigned int, float>	navvi2;
     NavigableVector_p2<unsigned int, double>	navvi3;
     NavigableVector_p2<std::string, void>	navvs1;
     NavigableVector_p2<std::string, float>	navvs2;
     NavigableVector_p2<std::string, double>	navvs3;

     ElementLinkVector_p1<unsigned int>::ElementRef  elvp1_ui_er;
   };
}

#endif
