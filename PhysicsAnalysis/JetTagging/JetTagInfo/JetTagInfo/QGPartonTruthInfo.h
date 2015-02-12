/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFO_QGPARTONTRUTHINFO_H
#define JETTAGINFO_QGPARTONTRUTHINFO_H
#include "JetTagInfo/BaseTagInfo.h"
#include <string>
#include <map>

namespace Analysis
{

   /** Class QGPartonTruthInfo:
   Class to store more truth information about a JetTag (e.g. jet truth labeling)
   */
class QGPartonTruthInfo : public BaseTagInfo
{
public:
    /** default constructor */
    QGPartonTruthInfo();

    /** constructor with info type */
    QGPartonTruthInfo(TagInfoType x);
    
    /** copy destructor */
    QGPartonTruthInfo(const QGPartonTruthInfo& QGPartonTruthInfo);
    
    /** assigenment operator */
    QGPartonTruthInfo &operator= (const QGPartonTruthInfo& rhs);

    /** default destructor */
    virtual ~QGPartonTruthInfo();
    
    /** clone method */
    virtual QGPartonTruthInfo* clone() const;
    
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
    void deltaRToParton(double dR);
    double deltaRToParton() const;

private:
    std::string m_jetTruthLabel;
    double m_distanceToParton;

}; // End class

inline QGPartonTruthInfo* QGPartonTruthInfo::clone() const
{
    return new QGPartonTruthInfo(*this);
}

inline const std::string& QGPartonTruthInfo::jetTruthLabel() const
{
    return m_jetTruthLabel;
}

inline void QGPartonTruthInfo::setJetTruthLabel(const std::string& jetTruthLabel)
{
    m_jetTruthLabel = jetTruthLabel;
}

} // End namespace
#endif

