/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SwitchingFieldManager_H
#define SwitchingFieldManager_H

/** @class SwitchingFieldManager SwitchingFieldManager.h
 *
 *  @brief G4FieldManager that sets tight stepping for muons; disables
 *   magnetic field for other particles
 *
 *  @authors  Evangelos Kourlitis, John Apostolakis 
 *  @date   2021-06-07
 */

#include "G4FieldManager.hh"

class SwitchingFieldManager : public G4FieldManager
{
  public:
    // using G4FieldManager::G4FieldManager;
    
    // Switch the field off when inside Calo
    SwitchingFieldManager(G4Field *field);
   
    ~SwitchingFieldManager();

    // Necessary virtual methods
    void   ConfigureForTrack( const G4Track * ) override;
    G4FieldManager* Clone() const override;

  private:
    G4Field*  m_savedField = nullptr;
    // Calo barrel region 1500mm < r < 3700mm and |z|<1000mm (both EMB and TileCal)
    G4double  m_radiusXYmax = 1900;
    G4double  m_radiusXYmin = 1500;
    G4double  m_Zmax = 2000;
    G4double  m_offset = 0.0; // 'null field area' m_offset mm away of the boudaries given by m_radiusXYmin, m_radiusXYmax and m_Zmax
};
#endif
