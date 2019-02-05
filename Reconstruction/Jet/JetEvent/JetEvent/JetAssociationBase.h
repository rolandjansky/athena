/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETASSOCIATIONBASE_H
#define JETEVENT_JETASSOCIATIONBASE_H

#include "Navigation/INavigable.h"
#include "JetEvent/JetKeyDescriptor.h"

namespace Analysis {
  class TrackAssociationCnv_p1;
  class PhotonAssociationCnv_p1;
  class MuonAssociationCnv_p1;
  class ElectronAssociationCnv_p1;
  class ISvxAssociationCnv_p1;
}

// RS HERE
class JetAssociationBase : public virtual INavigable
{
  // make the templated converter base friend
  template <class PERS>
    friend class JetConverterBase;
  
 public:

  typedef JetKeyDescriptor::key_t   name_t;
  typedef name_t                    NameType; // FIXME backward compatibility

  virtual ~JetAssociationBase() = default;
  
  void setName(const name_t& name);
  const name_t& name() const;
  size_t keyIndex() const { return m_keyIndex; }
  
  virtual JetAssociationBase* clone() const = 0;// { return 0; };
  
  // RS
  JetAssociationBase();
  
  //  virtual void fillToken(INavigationToken&) const { };
  
  //  virtual void fillToken(INavigationToken&, const boost::any&) const { };
  
protected:
  friend class Analysis::TrackAssociationCnv_p1;
  friend class Analysis::PhotonAssociationCnv_p1;
  friend class Analysis::MuonAssociationCnv_p1;
  friend class Analysis::ElectronAssociationCnv_p1;
  friend class Analysis::ISvxAssociationCnv_p1;
  
  // JetAssociationBase();
  
  size_t            m_keyIndex;
  JetKeyDescriptorInstance* m_jetKeyDescr;

};
#endif
