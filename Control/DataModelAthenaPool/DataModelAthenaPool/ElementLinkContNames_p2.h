/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATAMODELATHENAPOOL_ELEMENTLINKCONTAINERNAMES_P2_H
#define DATAMODELATHENAPOOL_ELEMENTLINKCONTAINERNAMES_P2_H

/** @file ElementLinkContNames_p2.h
 *  @brief This file contains the class definition for the ElementLinkContNames_p2 class.
 *  @author R.D.Schaffer@cern.ch
 **/

#include <string>
#include <vector>

/** @class ElementLinkContNames_p2<INDEX>
 *  @brief This class is used to hold the container names for element
 *  links. It can converter container names between string names and
 *  integer indices. 
 **/
 
class ElementLinkContNames_p2 {
public:

    ElementLinkContNames_p2() {}

    /// ElementLink container names
    std::vector<std::string> m_names;


};

#endif // DATAMODELATHENAPOOL_ELEMENTLINKCONTAINERNAMES_P2_H


