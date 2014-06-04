/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "Identifier/IdentifierDict.h"
#include "Identifier/IdHelper.h"
#include "AthContainers/DataVector.h"
#include <map>


namespace DetDescrDictionaryDict 
{
    DataVector<Identifier>                    m_idvec;
    std::vector<Identifier>                   m_idvec1;
    std::vector<HWIdentifier>                 m_idvec2;
    std::vector<HWIdentifier>::iterator       m_it1;
    std::vector<HWIdentifier>::const_iterator m_cit1;
    std::map<Identifier,int>                  m_idmapInt;
    std::map<HWIdentifier,float>              m_idmapFloat;
    std::pair<Identifier,int>                 m_idpairInt;
    std::pair<HWIdentifier,float>             m_idpairFloat;
    std::pair < IdentifierHash, int >         m_idhpairInt;
    DataVector<std::pair<IdentifierHash, int> >    m_dvidhpairInt;
    std::vector<std::pair<IdentifierHash, int>* >  m_vidhpairInt;
}
