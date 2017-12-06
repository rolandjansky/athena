/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LArRawchannelContainer_H
#define LArRawchannelContainer_H

#include "LArRawEvent/LArRawChannel.h"
#include "AthenaKernel/CLASS_DEF.h" 
#include <vector>

/**
@class LArRawChannelContainer
@brief Container for LArRawChannel (IDC using LArRawChannelCollection)
 * 
 * The LArRawChannel collections are sorted by FEBs
 * @author Kin Yip 
 * @author Hong Ma
 * @author Walter Lampl
 *
 * Modification Feb 2008: use plain std::vector, no sub-structure any more
 */
class LArRawChannelContainer : public std::vector<LArRawChannel>
{

 public:
  /** constructor */
  LArRawChannelContainer() {} ;

  /** desctructor */
  virtual ~LArRawChannelContainer() { };

  inline void add (const LArRawChannel& rc) 
    {this->push_back(rc);}

};

CLASS_DEF(LArRawChannelContainer,2721,0) 

  //The following #define should help in the migration from Identifiable LArRawChannelContainer 
  //to the new version. It's allow frequently changing clients like LArMonTools to have the same
  //code compiling against 13.X.Y and 14.0.0
#define LARRAWCHANNELCONTAINER_IS_NOT_IDENTIFIABLE


#endif
