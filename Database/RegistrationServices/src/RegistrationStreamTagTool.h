/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REGISTRATIONSVC_REGSTREAMTAGTOOL_H
# define REGISTRATIONSVC_REGSTREAMTAGTOOL_H

/**
 * @file RegistrationStreamTagTool.h
 *
 * @brief AlgTool which takes references provided by RegStream
 * finds the appropriate CORAL object in storegate and stores
 * them in a POOL collection.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Jack Cranshaw <Jack.Cranshaw@cern.ch>
 *
 * $Id: RegistrationStreamTagTool.h,v 1.4 2008-10-10 14:48:09 cranshaw Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "RegistrationStreamLCGTool.h"

class TagAthenaAttributeList;

/**
 * @class RegistrationStreamTagTool
 *
 * @brief Extension of the TagTool to use a TagAthenaAttributeList 
 * instead of an AthenaAttributeList
 *
 */

class RegistrationStreamTagTool : public RegistrationStreamLCGTool
{
public:

   /// Standard RegistrationStreamTagTool Constructor
   RegistrationStreamTagTool(const std::string& type, const std::string& name, const IInterface* parent);

   /// Working entry point
   /// NOTE that the Tools assume this list will be ordered.
   ///  begin = newest; end = oldest
   ///  duplicate elimination is left to the Tools
   StatusCode fillAtt(std::vector< std::pair<std::string, std::string> >& refs,
                              std::string key);

   /// Initialize RegistrationStreamTagTool
   virtual StatusCode initialize() {return RegistrationStreamLCGTool::initialize();}

   /// Terminate RegistrationStreamTagTool
   virtual StatusCode finalize() {return RegistrationStreamLCGTool::finalize();}

   const CLID& attListID();

protected:

   /// Standard Destructor
   virtual ~RegistrationStreamTagTool();

   /// Dump values from AttributeList for DEBUG
   void dumpTagList(const TagAthenaAttributeList* attributes) const;

   bool m_FragmentByGroup;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // REGISTRATIONSVC_REGSTREAMTAGTOOL_H
