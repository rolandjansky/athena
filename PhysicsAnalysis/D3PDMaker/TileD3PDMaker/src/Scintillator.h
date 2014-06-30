/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   Scintillator.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Scintillator object that stores positions and dimensions.
 *
 * Created in February 2013.
 */

#ifndef SCINTILLATOR_H
#define SCINTILLATOR_H

//=================
class Scintillator{
//=================

    public:
        Scintillator();
        Scintillator(float r,  float phi,  float z,
                     float dr, float dphi, float dz);
        Scintillator(float r,  float phi,  float z,
                     float dr, float dphi, float dz,
                     float eta,float deta);
        ~Scintillator();

        void setPosition(float r, float phi, float z);
        void setDimension(float dr, float dphi, float dz);
        void setEtaDeta(float eta, float deta);

        float r()    const {return m_r;   };
        float dr()   const {return m_dr;  };
        float phi()  const {return m_phi; };
        float dphi() const {return m_dphi;};
        float z()    const {return m_z;   };
        float dz()   const {return m_dz;  };
        float eta()  const {return m_eta; };
        float deta() const {return m_deta;};

    private:
        float m_r;
        float m_phi;
        float m_z;
        float m_dr;
        float m_dphi;
        float m_dz;
        float m_eta;
        float m_deta;

};

#endif