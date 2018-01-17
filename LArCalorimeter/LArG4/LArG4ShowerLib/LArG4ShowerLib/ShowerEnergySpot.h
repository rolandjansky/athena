/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Compressed version of energy spots to be used by ShowerLibrary.

*/
#pragma once

#include "G4ThreeVector.hh"

namespace ShowerLib {
    class ShowerEnergySpot
    {

    public:

        // Constructors and destructors
        ShowerEnergySpot(): m_point{0}, m_energy{0},m_time{0}
            {;}
        
                

        ShowerEnergySpot(const G4ThreeVector& P, G4double E, G4double t)
            {
                m_point[0] = P.getX();
                m_point[1] = P.getY();
                m_point[2] = P.getZ();
                m_energy = E;
                m_time = t;
            }
        ~ShowerEnergySpot() 
            { ;}

// Accessor methods
        inline void SetEnergy(const G4double& E) 
            { m_energy = E; }

        inline G4double GetEnergy() const 
            { return m_energy; }

        inline void SetPosition(const G4ThreeVector& P) 
            {
                m_point[0] = P.getX();
                m_point[1] = P.getY();
                m_point[2] = P.getZ();                
            }

        inline G4ThreeVector GetPosition() const 
            { return G4ThreeVector(m_point[0], m_point[1], m_point[2]); }

        inline void SetTime(const G4double t) 
            { m_time = t; }

        inline G4double GetTime() const 
            { return m_time; }
        
        
    private:
        float m_point[3];
        float m_energy;
        float m_time;    
    };

}



    
