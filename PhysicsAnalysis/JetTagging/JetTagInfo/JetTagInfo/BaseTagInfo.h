/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          BaseTagInfo.h  -  Description
                             -------------------
    begin   : 04-06-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :

 ***************************************************************************/

#ifndef JETTAGINFO_BASETAGINFO_H
#define JETTAGINFO_BASETAGINFO_H
#include "JetEvent/JetTagInfoBase.h"
#include <vector>
#include <sys/types.h>

namespace Analysis
{

  /** Class BasetagInfo:
  Implements methods defined in ITagInfo.

  @author Andreas.Wildauer@cern.ch
  */
class BaseTagInfo : public JetTagInfoBase
{
public:
    /** default constructor */
    BaseTagInfo();

    /** constructor with info type */
    BaseTagInfo(TagInfoType x);

    /** copy destructor */
    BaseTagInfo(const BaseTagInfo& BaseTagInfo);

    /** assigenment operator */
    BaseTagInfo &operator= (const BaseTagInfo& rhs);

    /** default destructor */
    virtual ~BaseTagInfo();

    /** Return and set methods for the likelihood. */
    virtual void                     setTagLikelihood(const std::vector<double>&); //!< to set the tag likelihood for one tagger
    virtual std::vector<double>      tagLikelihood(void) const;                    //!< returns the tag likelihood of one tagger
//     virtual double                   lhSig(void) const;                            //!< shortcut to retrieve the first element of the tagLikelihood
    virtual void                     setWeight(double weight);                     //!< set the weight for one tagger
    virtual double                   weight() const;                               //!< get the weight of one tagger
    virtual void                     makeValid();                            //!< sets the isValid flag to true, cannot be reversed!
    virtual bool                     isValid() const;                              //!< returns true if the tag was successfull
    virtual TagInfoType              infoType(void) const;                         //!< returns the infoType of the info objects. This has to be set by the tagger.
    virtual BaseTagInfo*             clone() const;                                //!< a clone method for the proper workings of the copy constructor

protected:
    bool m_isValid;                      //!< true if the tagger could tag the jet, default is false
    std::vector<double> m_tagLikelihood; //!< vector to hold the taglikelihood (signal plus N background)
    double m_weight;                     //!< weight for this tag
    TagInfoType m_tagJetInfoType;        //!< string to hold the info type (specified by the tag tool)
}
; // End class

inline bool BaseTagInfo::isValid() const
{
  return m_isValid;
}
inline void BaseTagInfo::makeValid()
{
  m_isValid = true;
}
inline TagInfoType BaseTagInfo::infoType(void) const
{
    return m_tagJetInfoType;
}
inline void BaseTagInfo::setTagLikelihood(const std::vector<double>& tagLikelihood)
{
    m_tagLikelihood=tagLikelihood;
}
inline std::vector<double> BaseTagInfo::tagLikelihood(void) const
{
  // in principal the caller has to check if the vector has a length
  // -> no checking for the size as in the case of lhSig(void)
  return m_tagLikelihood;
}
/*inline double BaseTagInfo::lhSig(void) const
{
    if (m_tagLikelihood.size()>0) return m_tagLikelihood[0];
    // cannot return anything when likelihood vector has zero length!!
    else return -1.;
}*/
inline void BaseTagInfo::setWeight(double weight)
{
  m_weight = weight;
}
inline double BaseTagInfo::weight(void) const
{
  return (this->isValid() ? m_weight : -19.75);
}
inline Analysis::BaseTagInfo* Analysis::BaseTagInfo::clone() const
{
    return new BaseTagInfo(*this);
}
} // End namespace
#endif
