/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORACOOL_CORACOOLTYPES_H
#define CORACOOL_CORACOOLTYPES_H
// CoraCoolTypes.h - definition of shared pointer types for CoraCool classes
// Richard Hawkings, started 10/2006

#include<boost/shared_ptr.hpp>

class CoraCoolDatabase;
typedef boost::shared_ptr<CoraCoolDatabase> CoraCoolDatabasePtr;

class CoraCoolFolder;
typedef boost::shared_ptr<CoraCoolFolder> CoraCoolFolderPtr;

class CoraCoolObject;
typedef boost::shared_ptr<CoraCoolObject> CoraCoolObjectPtr;

class CoraCoolObjectIter;
typedef boost::shared_ptr<CoraCoolObjectIter> CoraCoolObjectIterPtr;

#endif // CORACOOL_CORACOOLTYPES_H
