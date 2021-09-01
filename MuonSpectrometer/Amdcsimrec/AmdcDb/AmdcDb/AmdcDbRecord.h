/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCDB_AMDCDBRECORD_H
#define AMDCDB_AMDCDBRECORD_H

#include "RDBAccessSvc/IRDBRecord.h"

#include <string>
#include <vector>
#include <map>

  /**
   @class AmdcDbRecord

   This class is an implementation of the class IRDBRecord
   
  @author samusog@cern.ch
  
  */

class AmdcDbRecord : public IRDBRecord{
public:
    AmdcDbRecord(long UniversalId,const std::string& NameOfTheRecord);
    virtual ~AmdcDbRecord();

public:
///////////////////////////////////

//Functions 
//  of IRDBRecord Not implemented
  virtual bool        isFieldNull(const std::string& fieldName) const;
  virtual int         getInt     (const std::string& fieldName, unsigned int index) const;
  virtual long        getLong    (const std::string& fieldName, unsigned int index) const;
  virtual double      getDouble  (const std::string& fieldName, unsigned int index) const;
  virtual float       getFloat   (const std::string& fieldName, unsigned int index) const;
  virtual std::string getString  (const std::string& fieldName, unsigned int index) const;


// Get
   virtual std::string               getNameOfTheRecord() const;
   virtual long                      getUniversalId() const;
   virtual std::vector< std::string> getVariableList() const;
   virtual std::string               getVariableComment(const std::string& NameToFind) const;
   virtual int                       getVariableType(const std::string& NameToFind) const;
   virtual std::vector< std::string> getAllData() const;
   virtual int                       ThereIsABLOB() const;
   
// Get
   virtual int         getInt            (const std::string& NameToFind) const;
   virtual double      getDouble         (const std::string& NameToFind) const;
   virtual float       getFloat          (const std::string& NameToFind) const;

   virtual std::string getString         (const std::string& NameToFind) const;
   virtual long        getLong           (const std::string& NameToFind) const;

   virtual int         getDoublePrecision(const std::string& NameToFind) const;
   virtual std::string getBlob           (const std::string& NameToFind) const;

// Set
   virtual void        addInt(std::string NameToSet,std::string Comment,int ValueToSet);
   virtual void        addDouble(std::string NameToSet,std::string Comment,double ValueToSet, int precision);
   virtual void        addString(std::string NameToSet,std::string Comment,std::string ValueToSet);
   virtual void        addLong(std::string NameToSet,std::string Comment,long ValueToSet);
   virtual void        addBlob(std::string NameToSet,std::string Comment,std::string ValueToSet);

// Compare
   virtual int doWeMatchForThisVariable(
                                const std::string&  NameToFind,
                                const AmdcDbRecord* pAmdcDbRecord
			       ) const;
   virtual void doIMatch(
                 int            WarningLevel,
                 int&           Kwarn,
                 int&           Kchck,
		 std::ofstream& OutFile,
                 const AmdcDbRecord*  pAmdcDbRecord
		) const;

protected:
///////////////////////////////////

   std::string m_NameOfTheRecord ; 
   long        m_UniversalId     ;
   int         m_ThereIsABLOB    ;

   std::vector< std::string> m_VectorOfVariableNames        ; //!< Vector of Variable name 
   std::vector< std::string> m_VectorOfVariableNamesComment ; //!< Vector of Variable comment 
   std::vector< int>         m_VectorOfVariableNamesType    ; //!< Vector of Variable type

   std::map< std::string, int> m_MapOfint ; //!< Map of int

   std::map< std::string, double> m_MapOfdouble       ; //!< Map of double
   std::map< std::string, int> m_MapOfdoublePrecision ; //!< Map of precision of the double

   std::map< std::string, std::string> m_MapOfstring ; //!< Map of string

   std::map< std::string, long> m_MapOflong ; //!< Map of long

   std::map< std::string, std::string> m_MapOfBlob ; //!< Map of Blob

};

#endif
