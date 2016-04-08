/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoSerialIdentifier_h
#define GeoSerialIdentifier_h 1
#include "GeoModelKernel/GeoGraphNode.h"

// Like a GeoSerialDenominator.  Identifies volumes,
// including virtual volumes.

class GeoSerialIdentifier : public GeoGraphNode  

{

  public:


      GeoSerialIdentifier(int baseId);

      virtual void exec(GeoNodeAction *action) const;

      inline int getBaseId() const;


  protected:

      virtual ~GeoSerialIdentifier();

  private:

      GeoSerialIdentifier(const GeoSerialIdentifier &right);


      const GeoSerialIdentifier & operator=(const GeoSerialIdentifier &right);

      int  m_baseId;

};

inline int GeoSerialIdentifier::getBaseId() const
{
  return m_baseId;
}





#endif


