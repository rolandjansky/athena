/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_DICT_H
#define JETEVENT_DICT_H

#include "JetEvent/JetCollection.h"
#include "JetEvent/JetSamplingCollection.h"
#include "JetEvent/JetAssociationBase.h"
#include "JetEvent/JetINav4MomAssociation.h"
#include "JetEvent/JetTagInfoBase.h"
#include "JetEvent/JetKeyDescriptor.h"
#include "JetEvent/JetKeyDescriptorCollection.h"
#include "JetEvent/JetMomentMap.h"
#include "JetEvent/JetMomentMapCollection.h"
#include "DataModel/DataVector.h"
#include "JetEvent/JetConstituentIterator.h"

struct temp
{
  Navigable<JetCollection,double> m_Jnav;
  ElementLink<JetCollection> m_Jlink;
  std::pair<ElementLink<JetCollection>,double> m_Jpair;
  std::pair<int,double> m_pairid;
  std::vector<const JetAssociationBase*> m_assocVec;
  std::vector<const JetTagInfoBase*>     m_tagInfoVec;
  std::vector<Analysis::ITagInfo*>       m_dum;
  JetKeyDescriptorCollection             m_dumJetKeyDescriptorCollection;
  std::vector<JetKeyDescriptor>          m_dumVJetKeyDescriptor;
  DataVector<JetKeyDescriptor>           m_dumDJetKeyDescriptor;
  JetMomentMapCollection                 m_dumJetMomentMapCollection;
  std::vector<JetMomentMap>              m_dumVJetMomentMap;
  DataVector<JetMomentMap>               m_dumDJetMomentMap;
};

#endif
