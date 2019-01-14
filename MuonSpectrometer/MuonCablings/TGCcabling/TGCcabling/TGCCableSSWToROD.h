/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLESSWTOROD_HH
#define TGCCABLESSWTOROD_HH
 
#include <string>
#include "TGCcabling/TGCCable.h"
#include "TGCcabling/TGCId.h"
#include "TGCcabling/TGCDatabaseSLBToROD.h" 
#include "TGCcabling/TGCModuleSSW.h"
#include "TGCcabling/TGCModuleROD.h"

namespace LVL1TGCCabling8
{
  
class TGCCableSSWToROD : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableSSWToROD (std::string filename);

  virtual ~TGCCableSSWToROD (void);
  
  virtual TGCModuleMap* getModule (const TGCModuleId* moduleId) const;

private:
  TGCCableSSWToROD (void) {}
  virtual TGCModuleMap* getModuleIn (const TGCModuleId* rod) const;
  virtual TGCModuleMap* getModuleOut (const TGCModuleId* ssw) const;
  TGCDatabase* m_database;

  // hidden assignment operator and copy constructor
  TGCCableSSWToROD & operator=(const TGCCableSSWToROD &right);
  TGCCableSSWToROD(const TGCCableSSWToROD&);

};
  
} // end of namespace
 
#endif
