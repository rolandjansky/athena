/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTVisualization_hh
#define TRTVisualization_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"
#include "CxxUtils/checker_macros.h"

class G4LogicalVolume;
class G4VisAttributes;


class ATLAS_NOT_THREAD_SAFE TRTVisualization // static variable and thread unsafe exit are used.
{
  public:
    ~TRTVisualization();

    static TRTVisualization* GetPointer()
    {
      if (!s_pVisualization)
        s_pVisualization = new TRTVisualization;
      return s_pVisualization;
    }

    void Visualize(G4LogicalVolume*, int);
 
    MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

  private:
    TRTVisualization();

    void Initialize();

    G4VisAttributes* m_pVisAttributeRed;
    G4VisAttributes* m_pVisAttributeGreen;
    G4VisAttributes* m_pVisAttributeBlue;
    G4VisAttributes* m_pVisAttributeYellow;
    G4VisAttributes* m_pVisAttributeMagenta;
    G4VisAttributes* m_pVisAttributeCyan;
    G4VisAttributes* m_pVisAttributeBlack;

    static TRTVisualization* s_pVisualization;

    mutable Athena::MsgStreamMember m_msg ATLAS_THREAD_SAFE;

};

#endif
