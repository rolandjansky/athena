/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DATABASEACCESSTOOL_H_
#define DATABASEACCESSTOOL_H_
#include <string>
//---------------------------------------------------------------------
//
// This class gets data from the detector description database.  If no 
// connection is open we create one.  We drop it when the object goes  
// out of scope.
//
//---------------------------------------------------------------------

class DatabaseAccessTool {

 public:


  // Constructor:
  DatabaseAccessTool();

  // Destructor:
  ~DatabaseAccessTool();

  // Access the data.  The table will be taken from the current tagged ATLAS
  // version.  If a fallback version name is provided then the value will be
  // taken from the fallback name. 
  double getDouble(const std::string & TableName,  const std::string & FallbackVersion, const std::string & ColumnName) const;
  
 private:

  // It is illegal to copy or clone:
  DatabaseAccessTool & operator= (const DatabaseAccessTool & );
  DatabaseAccessTool(const DatabaseAccessTool & );

  // Internals:

  class Clockwork;
  Clockwork *m_cw;

  

};


#endif
