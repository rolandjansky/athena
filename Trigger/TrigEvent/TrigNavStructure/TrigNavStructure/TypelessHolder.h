// Emacs -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVSTRUCTURE_TYPELESSSHOLDER_H
#define TRIGNAVSTRUCTURE_TYPELESSSHOLDER_H
#include "TrigNavStructure/BaseHolder.h"
namespace HLT {
  class TypelessHolder : public BaseHolder {
  public:
    TypelessHolder(class_id_type clid, const std::string& label,  sub_index_type sub);
    virtual class_id_type typeClid() const { return m_clid; }
    virtual const std::string& label() const { return m_label; }
    virtual sub_index_type subTypeIndex() const { return m_sub; }        

  protected:
    class_id_type m_clid;
    std::string m_label;
    sub_index_type m_sub;
  };
} // eof HLT


#endif
