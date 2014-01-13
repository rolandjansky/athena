/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

// COOL
#include "CoolApplication/Application.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/Exception.h"

// Local
#include "TrigCostRate/CoolDB.h"

using namespace std;

//-----------------------------------------------------------------------------
Anp::CoolDB::CoolDB()
  :fCoolId()
{
}

//-----------------------------------------------------------------------------
Anp::CoolDB::~CoolDB()
{
}

//---------------------------------------------------------------------------------------
cool::IDatabaseSvc& Anp::CoolDB::databaseService()
{
  static cool::Application app;
  return app.databaseService();
}

//---------------------------------------------------------------------------------------
bool Anp::CoolDB::IsOpen()
{ 
  return fCoolPtr.use_count()>0 && fCoolPtr->isOpen();
}

//---------------------------------------------------------------------------------------
bool Anp::CoolDB::OpenDB()
{
  if(IsOpen()) return true;
  
  const bool readOnly = true;
  cool::IDatabaseSvc& dbSvc = databaseService();

  try {
    cout << "CoolDB::OpenDB - opening database '" << fCoolId << "'" << endl;
    fCoolPtr = dbSvc.openDatabase(fCoolId, readOnly);   

    return true;
  }
  catch(cool::DatabaseDoesNotExist& e) {
    cout<< "CoolDB::OpenDB - COOL exception caught: " << e.what() << endl
	<< "   could not open database: \"" << fCoolId << "\"" << endl;
    return false;
  }

  return false;
}

//---------------------------------------------------------------------------------------
void Anp::CoolDB::CloseDB()
{
  try {
    if(IsOpen()) {
      cout << "CoolDB::CloseDB - closing database '" << fCoolId << endl;
      fCoolPtr->closeDatabase();
    }
  }
  catch(std::exception& e) {
    cout << "CoolDB::CloseDB - COOL exception caught: " << e.what() << endl
	 << "   could not close COOL database: \"" << fCoolId << "\"" << endl;
  }
  
  return;
}
