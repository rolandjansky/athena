/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavStructure/TypelessHolder.h"

HLT::TypelessHolder::TypelessHolder(class_id_type clid, const std::string& label,  sub_index_type sub) 
  : m_clid  (clid),
    m_label (label),
    m_sub   (sub)
{}
