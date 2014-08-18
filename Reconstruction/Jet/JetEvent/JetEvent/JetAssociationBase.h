/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETASSOCIATIONBASE_H
#define JETEVENT_JETASSOCIATIONBASE_H

#include "Navigation/INavigable.h"
#include "JetEvent/JetKeyDescriptor.h"

// RS HERE
class JetAssociationBase : public virtual INavigable
{
  // make the templated converter base friend
  template <class PERS>
    friend class JetConverterBase;
  
 public:

  typedef JetKeyDescriptor::key_t   name_t;
  typedef name_t                    NameType; // FIXME backward compatibility

  virtual ~JetAssociationBase();
  
  void setName(const name_t& name);
  const name_t& name() const;
  
  virtual JetAssociationBase* clone() const = 0;// { return 0; };
  
  // RS
  JetAssociationBase();
  
  //  virtual void fillToken(INavigationToken&) const { };
  
  //  virtual void fillToken(INavigationToken&, const boost::any&) const { };
  
protected:
  
  // JetAssociationBase();
  
  size_t            m_keyIndex;
  JetKeyDescriptorInstance* m_jetKeyDescr;

};
#endif
