/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           ITagInfo.h  -  Description
                             -------------------
    begin   : 04-06-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch

    changes : 01-13-2005 (FD) introduce muon case
              01-21-2005 (AW) moved to JetTagging/JetTagInfo
                              changed namespace to Analysis (from BTAG)

 ***************************************************************************/

#ifndef JETTAGINFO_ITAGINFO_H
#define JETTAGINFO_ITAGINFO_H

// This has been moved to JetEvent.
#include "JetEvent/JetTagInfoBase.h"

#if 0
#include<vector>
#include<string>

namespace Analysis
{

  /** string to identify the tagger tool.
  This has been an enum before. To minimize necessary changes in clients I typedef it
  to a string instead of changing the code everywhere.
  */
  typedef std::string TagInfoType;

  /** Class ITagInfo:
  This is the abstract base class for additional information to be put into the JetTag
  class. Users can inherit from this baseclass, extend the interface and datamembers with
  information their taggers need to store and then use it for tagging and pass it into the TagJet.

  @author Andreas.Wildauer@cern.ch
  */
  class ITagInfo
  {
    public:
      virtual ~ITagInfo() {}; //!< destructor
      virtual void setTagLikelihood(const std::vector<double>&) = 0; //!< to set the tag likelihood for one tagger
      virtual std::vector<double> tagLikelihood(void) const = 0;     //!< returns the tag likelihood of one tagger
//       virtual double lhSig(void) const = 0;                          //!< shortcut to retrieve the first element of the tagLikelihood
      virtual void setWeight(double weight) = 0;                     //!< set the weight for one tagger
      virtual double weight() const = 0;                             //!< get the weight of one tagger
      virtual void makeValid() = 0;                                  //!< sets the isValid flag to true, cannot be reversed!
      virtual bool isValid() const = 0;                              //!< returns true if the tag was successfull
      virtual TagInfoType infoType() const = 0;                      //!< returns the infoType of the info objects. This has to be set by the tagger.
      virtual ITagInfo* clone() const = 0;                           //!< a clone method for the proper workings of the copy constructor
  };
}
#endif

#endif
