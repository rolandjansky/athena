/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

//protect
#include <SampleHandler/TagList.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

namespace SH
{
  std::string dbg (const TagList& obj, unsigned /*verbosity*/)
  {
    std::string result;

    for (TagList::iterator tag = obj.begin(), end = obj.end();
	 tag != end; ++ tag)
    {
      if (!result.empty())
	result += ",";
      result += *tag;
    };
    return "(" + result + ")";
  }



  void TagList ::
  testInvariant () const
  {
    for (TagsIter tag = m_tags.begin(),
	   end = m_tags.end(); tag != end; ++ tag)
    {
      RCU_INVARIANT (!tag->empty());
      //RCU_INVARIANT (tag->find (':') == std::string::npos);
    };
  }



  TagList ::
  TagList ()
  {
    RCU_NEW_INVARIANT (this);
  }



  TagList ::
  TagList (const std::string& tags, char separator)
  {
    std::string::const_iterator pos1, pos2 = tags.begin(), end = tags.end();

    while (pos2 != end)
    {
      for (pos1 = pos2; pos2 != end && *pos2 != separator; ++ pos2) {};

      if (pos2 != pos1)
	m_tags.insert (std::string (pos1, pos2));
      if (pos2 != end)
	++ pos2;
    };

    RCU_NEW_INVARIANT (this);
  }



  bool TagList ::
  has (const std::string& tag) const
  {
    RCU_READ_INVARIANT (this);
    RCU_REQUIRE_SOFT (!tag.empty());
    return m_tags.find (tag) != m_tags.end();
  }



  TagList::iterator TagList ::
  begin () const
  {
    RCU_READ_INVARIANT (this);
    return m_tags.begin();
  }



  TagList::iterator TagList ::
  end () const
  {
    RCU_READ_INVARIANT (this);
    return m_tags.end();
  }



  std::string TagList ::
  join (const char separator) const
  {
    RCU_READ_INVARIANT (this);

    std::string result;
    for (TagsIter tag = m_tags.begin(), end = m_tags.end();
	 tag != end; ++ tag)
    {
      if (tag->find (separator) != std::string::npos)
	RCU_THROW_MSG (std::string ("can't use separator ") + separator + " it is part of tag " + *tag);
      if (!result.empty())
	result += separator;
      result += *tag;
    };
    return result;
  }



  void TagList ::
  add (const std::string& tag)
  {
    RCU_CHANGE_INVARIANT (this);

    m_tags.insert (tag);
  }
}
