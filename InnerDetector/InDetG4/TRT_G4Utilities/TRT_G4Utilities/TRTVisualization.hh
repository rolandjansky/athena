/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTVisualization_hh
#define TRTVisualization_hh

#include "globals.hh"

class G4LogicalVolume;
class G4VisAttributes;


class TRTVisualization
{
  public:
    TRTVisualization();
    ~TRTVisualization();

    void Visualize(G4LogicalVolume*, int);
 
  private:
    void Initialize();

    G4VisAttributes* m_pVisAttributeRed = nullptr;
    G4VisAttributes* m_pVisAttributeGreen = nullptr;
    G4VisAttributes* m_pVisAttributeBlue = nullptr;
    G4VisAttributes* m_pVisAttributeYellow = nullptr;
    G4VisAttributes* m_pVisAttributeMagenta = nullptr;
    G4VisAttributes* m_pVisAttributeCyan = nullptr;
    G4VisAttributes* m_pVisAttributeBlack = nullptr;
};

#endif
