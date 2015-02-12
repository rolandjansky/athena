/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_SOFTLEPTONTRUTHINFO_H
#define JETTAGINFO_SOFTLEPTONTRUTHINFO_H
#include "JetTagInfo/BaseTagInfo.h"
#include "JetTagInfo/SLTrueInfo.h"
#include <vector>

namespace Analysis
{

   /** Class SoftLeptonTruthInfo:
   Class to store truth information about lepton in jet
   */
class SoftLeptonTruthInfo : public BaseTagInfo
{
public:
    /** default constructor */
    SoftLeptonTruthInfo();

    /** constructor with info type */
    SoftLeptonTruthInfo(TagInfoType x);
    
    /** copy destructor */
    SoftLeptonTruthInfo(const SoftLeptonTruthInfo& SoftLeptonTruthInfo);
    
    /** assigenment operator */
    SoftLeptonTruthInfo &operator= (const SoftLeptonTruthInfo& rhs);

    /** default destructor */
    virtual ~SoftLeptonTruthInfo();
    
    /** clone method */
    virtual SoftLeptonTruthInfo* clone() const;


    /** Number of truth lepton info objects stored */
    inline int numSLTrueInfo() const { return m_sltrueinfo.size();}
    /** Insert a new track info object */
    inline void addSLTrueInfo(const SLTrueInfo& i) { m_sltrueinfo.push_back(i); }
    /** Get a track info object */
    inline const SLTrueInfo& getSLTrueInfo(uint i) const { 
      return m_sltrueinfo.at(i);
    }

private:

    std::vector<SLTrueInfo> m_sltrueinfo;

}; // End class

inline SoftLeptonTruthInfo* SoftLeptonTruthInfo::clone() const {
  return new SoftLeptonTruthInfo(*this);
}


} // End namespace
#endif

