/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFO_TAGINFO_H
# define EVENTINFO_TAGINFO_H 1
/**
 * @file TagInfo.h
 *
 * @brief This class contains the list of currently valid tags for
 *  detector description - GeoModel and IOV/Conditions.
 *  Tags are given as name/value pairs.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: TagInfo.h,v 1.4 2007-03-19 19:41:49 gemmeren Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/StatusCode.h"
#include <vector>
#include <string>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class MsgStream;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 **  @class TagInfo
 **
 **  @brief This class contains the list of currently valid tags for detector
 **  description - GeoModel and IOV/Conditions.
 **  Tags are given as name/value pairs.
 **
 **  The set of tags corresponding to the input event are also made
 **  available separately.
 **
 **/
class TagInfo {
public:

    /// @name typedefs:
    //@{
    typedef     std::pair<std::string,std::string>  NameTagPair;
    typedef     std::vector<NameTagPair>            NameTagPairVec;
    //@}

    /// \name structors
    //@{
    TagInfo();
    virtual ~TagInfo();
    //@}

    /// \name Tag information accessors
    //@{

    /// Find tag by its name - for current tags
    void                findTag       (const std::string& name, std::string& tag) const;

    /// Fill vector with all current tags
    void                getTags       (NameTagPairVec& pairs) const; 

    /// Find tag by its name - for input tags
    void                findInputTag  (const std::string& name, std::string& tag) const;

    /// Fill vector with all current tags
    void                getInputTags  (NameTagPairVec& pairs) const; 

    /// The tag of the TagInfo object
    std::string         tagInfoTag    () const;
    //@}
    
    /// Less than comparision needed to create e.g. set<TagInfo>
    bool                operator<(const TagInfo& rhs) const;

    /// \name Tag information set methods
    //@{

    /// addTag for current tags - returns failure if tag name exists
    /// and override == false
    StatusCode          addTag        (const NameTagPair& pair, bool override=false);
    
    /// addInputTag for input tags - returns failure if tag name
    /// exists and override == false
    StatusCode          addInputTag   (const NameTagPair& pair, bool override=false);
    
    /// set the tag for the TagInfo object itself
    void                setTagInfoTag (const std::string& tag);
    //@}

    /// Printout method:
    void                printTags     (MsgStream& log) const;
    

private:

    /// Return an iterator for a tag found
    NameTagPairVec::iterator       findTag      (const std::string& name,
						 NameTagPairVec& tags);
    /// Return an iterator for a tag found
    NameTagPairVec::const_iterator findTag      (const std::string& name,
						 const NameTagPairVec& tags) const;

    NameTagPairVec      m_tags;
    NameTagPairVec      m_inputTags;
    std::string         m_myTag;
};

#ifndef CLIDSVC_CLASSDEF_H
# include "SGTools/CLASS_DEF.h"
#endif
CLASS_DEF( TagInfo , 61780915 , 1 )

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // EVENTINFO_TAGINFO_H
