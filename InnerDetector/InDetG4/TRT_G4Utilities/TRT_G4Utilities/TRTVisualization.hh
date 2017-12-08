/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTVisualization_hh
#define TRTVisualization_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

class G4LogicalVolume;
class G4VisAttributes;


class TRTVisualization
{
  public:
    ~TRTVisualization();

    static TRTVisualization* GetPointer()
    {
      if (!pVisualization)
        pVisualization = new TRTVisualization;
      return pVisualization;
    }

    void Visualize(G4LogicalVolume*, int);
 
    MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

  private:
    TRTVisualization();

    void Initialize();

    int printMessages;

    G4VisAttributes* pVisAttributeRed;
    G4VisAttributes* pVisAttributeGreen;
    G4VisAttributes* pVisAttributeBlue;
    G4VisAttributes* pVisAttributeYellow;
    G4VisAttributes* pVisAttributeMagenta;
    G4VisAttributes* pVisAttributeCyan;
    G4VisAttributes* pVisAttributeBlack;

    static TRTVisualization* pVisualization;

    mutable Athena::MsgStreamMember m_msg;

};

#endif
