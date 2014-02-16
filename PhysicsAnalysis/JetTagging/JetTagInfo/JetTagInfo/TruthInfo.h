/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_TRUTHINFO_H
#define JETTAGINFO_TRUTHINFO_H
#include "JetTagInfo/BaseTagInfo.h"
#include "EventPrimitives/EventPrimitives.h"
#include <string>
#include <map>

namespace Analysis
{

   /** Class TruthInfo:
   Class to store more truth information about a JetTag (e.g. jet truth labeling)
   */
class TruthInfo : public BaseTagInfo
{
public:
    /** default constructor */
    TruthInfo();

    /** constructor with info type */
    TruthInfo(TagInfoType x);
    
    /** copy destructor */
    TruthInfo(const TruthInfo& TruthInfo);
    
    /** assigenment operator */
    TruthInfo &operator= (const TruthInfo& rhs);

    /** default destructor */
    virtual ~TruthInfo();
    
    /** clone method */
    virtual TruthInfo* clone() const;
    
    /** return the truth label */
    const std::string& jetTruthLabel() const;
    
    /** set the truth label
    The string with which to set the truth label is stored in the JetTruthMatch algtool
    itself. It is passed into the info object by the BTagTool after a successful truth match
    (otherwise it stays at the default value of "N/A"). The truth label string itself
    has to be passed into the JetTruthMatch algtool via jobOptions. For the JetTruthMatch algtool
    instance used by the BTagTool this is set to "b" (and the pdg reference list is set to all b particles).
    */
    void setJetTruthLabel(const std::string& jetTruthLabel);

    /** min deltaR to quarks: */
    void deltaRMinTo(const std::string& key, double dR);
    double deltaRMinTo(const std::string& key) const;

    void BDecayVertex(const Eigen::Vector3d&);
    const Eigen::Vector3d& BDecayVertex() const;
    void BHadronPdg(const int);
    int BHadronPdg() const;

private:
    std::string m_jetTruthLabel;
    double m_distanceToBQuark;
    double m_distanceToCQuark;
    double m_distanceToTLepton;
    Eigen::Vector3d m_BHadronDecayVertex;
    int m_BHadronPdg;
}; // End class

inline void TruthInfo::BDecayVertex(const Eigen::Vector3d& rp) {m_BHadronDecayVertex = rp;}
inline const Eigen::Vector3d& TruthInfo::BDecayVertex() const  {return m_BHadronDecayVertex;}
inline void TruthInfo::BHadronPdg(const int i) {m_BHadronPdg = i;}
inline int TruthInfo::BHadronPdg() const {return m_BHadronPdg;}

inline TruthInfo* TruthInfo::clone() const
{
    return new TruthInfo(*this);
}

inline const std::string& TruthInfo::jetTruthLabel() const
{
    return m_jetTruthLabel;
}

inline void TruthInfo::setJetTruthLabel(const std::string& jetTruthLabel)
{
    m_jetTruthLabel = jetTruthLabel;
}

} // End namespace
#endif

