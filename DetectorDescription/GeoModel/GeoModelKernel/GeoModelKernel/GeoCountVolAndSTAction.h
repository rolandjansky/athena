/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoCountVolAndSTAction_h
#define GeoCountVolAndSTAction_h 1

#include "GeoModelKernel/GeoNodeAction.h"

//
// Class: GeoCountVolAndSTAction
// An action to count the number of physical volumes
// and Serial Transformers
//

class GeoCountVolAndSTAction : public GeoNodeAction
{
 public:
  GeoCountVolAndSTAction();
  virtual ~GeoCountVolAndSTAction();

  virtual void handlePhysVol(const GeoPhysVol*);
  virtual void handleFullPhysVol(const GeoFullPhysVol*);
  virtual void handleSerialTransformer(const GeoSerialTransformer*);

  unsigned int getCount() const;

 private:
  GeoCountVolAndSTAction(const GeoCountVolAndSTAction &right);
  const GeoCountVolAndSTAction & operator=(const GeoCountVolAndSTAction &right);

  unsigned int m_count;
};

inline unsigned int GeoCountVolAndSTAction::getCount() const
{
  return m_count;
}

#endif
