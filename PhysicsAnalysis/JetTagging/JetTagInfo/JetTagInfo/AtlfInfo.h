/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_ATLFINFO_H
#define JETTAGINFO_ATLFINFO_H
#include "JetTagInfo/BaseTagInfo.h"

namespace Analysis { 

  /**
  @class AtlfInfo
  Basic class to hold Atlfast Info
  */

class AtlfInfo : public BaseTagInfo { 
public:
    /** default constructor */
    AtlfInfo();

    /** constructor with info type */
    AtlfInfo(TagInfoType x);
    
    /** copy destructor */
    AtlfInfo(const AtlfInfo& AtlfInfo);
    
    /** assigenment operator */
    AtlfInfo &operator= (const AtlfInfo& rhs);

    /** default destructor */
    virtual ~AtlfInfo();
    virtual AtlfInfo* clone() const;

    inline void isBTagged(bool b)              { m_isBTagged = b; }
    inline bool isBTagged() const              { return m_isBTagged; }
    inline void isTauTagged(bool b)            { m_isTauTagged = b; }
    inline bool isTauTagged() const            { return m_isTauTagged; }
    inline void isTau1PTagged(bool b)          { m_isTau1PTagged = b; }
    inline bool isTau1PTagged() const          { return m_isTau1PTagged; }
    inline void isTau3PTagged(bool b)          { m_isTau3PTagged = b; }
    inline bool isTau3PTagged() const          { return m_isTau3PTagged; }
    inline void LightHypoCalFactor(float c)    { m_calFactLight = c; }
    inline float LightHypoCalFactor() const    { return m_calFactLight; }
    inline void TauHypoCalFactor(float c)      { m_calFactTau = c; }
    inline float TauHypoCalFactor() const      { return m_calFactTau; }
    inline void Tau1P3PHypoCalFactor(float c)  { m_calFactTau1P3P = c; }
    inline float Tau1P3PHypoCalFactor() const  { return m_calFactTau1P3P; }
    inline void BHypoCalFactor(float c)        { m_calFactB = c; }
    inline float BHypoCalFactor() const        { return m_calFactB; }

    /** min deltaR to b,c quarks/tau lepton: */
    void deltaRMinTo(const std::string&, float);
    float deltaRMinTo(const std::string&) const;
    
private:
    bool m_isBTagged; 
    bool m_isTauTagged; 
    bool m_isTau1PTagged; 
    bool m_isTau3PTagged; 
    float m_calFactLight;
    float m_calFactTau;
    float m_calFactTau1P3P;
    float m_calFactB;
    float m_distanceToBQuark;
    float m_distanceToCQuark;
    float m_distanceToTLepton;
}; // End class

inline Analysis::AtlfInfo* Analysis::AtlfInfo::clone() const { 
    return new AtlfInfo(*this);
}

} // End namespace
#endif
