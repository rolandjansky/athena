/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorDescription_IGeometryDBSvc_h
#define DetectorDescription_IGeometryDBSvc_h

#include "GaudiKernel/IInterface.h"
#include <string>
#include "RDBAccessSvc/IRDBAccessSvc.h"


/**
 * @class IGeometryDBSvc
 * Interface class to access geometry database with possibility to override parameters from
 * a text file. Note, that overriding from a text file should not be used for production geometries.
 * Its purpose is for mainly for development and private tests.  
**/

class IGeometryDBSvc: virtual public IInterface
{
public:
  virtual ~IGeometryDBSvc(){}
 
  /// The following methods will first look in the text file if provided and then look in the database.
  virtual double getDouble(IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const=0; 
  virtual int getInt(IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const=0;
  virtual std::string getString(IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const=0; 
  virtual bool testField(IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const=0;
  virtual bool testFieldTxt(IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const=0;
  virtual unsigned int getTableSize(IRDBRecordset_ptr recordSet) const=0;

  /// The following methods look only in the text file and their intention is for use during development 
  /// before the table exists in the database. It is recommended however to use the above methods that take a
  /// IRDBRecordset pointer.
  virtual double getDouble(const std::string & recordSetName, const std::string & name, int index=0) const=0; 
  virtual int getInt(const std::string & recordSetName, const std::string & name, int index=0) const=0; 
  virtual std::string getString(const std::string & recordSetName, const std::string & name, int index=0) const=0; 
  virtual bool testField(const std::string & recordSetName, const std::string & name, int index=0) const=0;
  virtual unsigned int getTableSize(const std::string & recordSetName) const=0; 

  /// Print paramaters read in from text file. If section is supplied only consider the parameters in that section.
  virtual void printParameters(const std::string & section = "") const=0;

  /// Print paramaters from text file which are not used. If section is supplied only consider the parameters in that section.
  virtual void printNotUsed(const std::string & section = "") const=0;

  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface

};

inline const InterfaceID & IGeometryDBSvc::interfaceID(){
  static const InterfaceID IID_IGeometryDBSvc("IGeometryDBSvc",1,0);
  return IID_IGeometryDBSvc;
}

#endif // DetectorDescription_IGeometryDBSvc_h
