/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeometryDBSvc_h
#define GeometryDBSvc_h

#include "GeometryDBSvc/IGeometryDBSvc.h"

#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"

#include <vector>
#include <string>

class TextFileDBReader;
class IRDBRecordset;

/**
 * @class GeometryDBSvc
 * Concrete class for interface to geometry database with possibility to 
 * override entries from a text file.
**/

class GeometryDBSvc:  public AthService, virtual public IGeometryDBSvc
{
public:
  GeometryDBSvc(const std::string& name, ISvcLocator* sl);
  virtual ~GeometryDBSvc();

  virtual StatusCode initialize();          //!< Service init
  virtual StatusCode finalize();            //!< Service finalize
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  static const InterfaceID & interfaceID();

  virtual double getDouble(IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const; 
  virtual int getInt(IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const;
  virtual std::string getString(IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const; 
  virtual bool testField(IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const;
  virtual bool testFieldTxt(IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const;
  virtual unsigned int getTableSize(IRDBRecordset_ptr recordSet) const;

  virtual double getDouble(const std::string & recordSetName, const std::string & name, int index=0) const; 
  virtual int getInt(const std::string & recordSetName, const std::string & name, int index=0) const; 
  virtual std::string getString(const std::string & recordSetName, const std::string & name, int index=0) const; 
  virtual bool testField(const std::string & recordSetName, const std::string & name, int index=0) const;
  virtual unsigned int getTableSize(const std::string & recordSetName) const; 

  /// Print paramaters read in from text file. If section is supplied only consider the parameters in that section.
  virtual void printParameters(const std::string & section = "") const;

  /// Print paramaters from text file which are not used. If section is supplied only consider the parameters in that section.
  virtual void printNotUsed(const std::string & section = "") const;


private:

  void setParameterFileName(const std::string & filename);
  std::string parameterKey(const std::string & recordSetName, const std::string & name, int index) const;
  bool getValue(const std::string & recordSetName, const std::string & name, int index, std::string & var) const;
  bool getValue(const std::string & recordSetName, const std::string & name, int index, double & var) const;
  bool getValue(const std::string & recordSetName, const std::string & name, int index,  int & var) const;
  bool getTableSizeFromTextFile(const std::string & recordSetName, int & var) const;

  // Pointer to text file interface
  TextFileDBReader * m_textParameters;

  // Properties
  std::string m_textFileName;
  bool m_printParameters;
  bool m_printNotUsed;
  std::vector<std::string> m_sections;

  // Cache of last lookup.
  mutable std::string m_lastLookupKey;
  mutable std::string m_lastLookupValue;
  mutable bool m_lastLookupResult;

};

inline const InterfaceID & GeometryDBSvc::interfaceID(){
  return IGeometryDBSvc::interfaceID();
}

#endif                           
