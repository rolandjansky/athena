/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class EMInsituDatabaseEntry

    The EMInsituDatabaseEntry is a class which holds the information
    of EMClusterErrrorsMatrixes before writing them to the database or
    after reading them from the database.It is based on
    APInsituDatabas in InsituPerformance, by Matthias Schott.

    @author Jovan Mitrevski (this version), Matthias Schott (original)
**/
/////////////////////////////////////////////////////////////////////////////


#ifndef EMINSITUDATABASEENTRY_H
#define EMINSITUDATABASEENTRY_H

#include <string>
#include <vector>

#include "TROOT.h"

// do I really need a magic number?
#define EMInsituDatabase_EMClusterErrorsMatrixID 1327

// class TDirectory;

class EMClusterErrorsMatrix;

class EMInsituDatabaseEntry
{
 public:
  /** Default Constructor*/
  EMInsituDatabaseEntry();
		
  /** Constructor Defining DatabaseID*/
  EMInsituDatabaseEntry(const std::string& dbaseID);

  /** Default Destructor*/
  ~EMInsituDatabaseEntry();
  /** Copy Constructor*/
  EMInsituDatabaseEntry(const EMInsituDatabaseEntry& ob) = default;
 
  /** = operator */
  EMInsituDatabaseEntry& operator = (const EMInsituDatabaseEntry& ob) = default;

  /** Fill this APEMClusterErrorsEntry from given APEMClusterErrorsMatrix*/
  bool setMatrix(const EMClusterErrorsMatrix &apClusterErrorsMatrix);
  /** Fill APEfficiencyMatrix from this APEMClusterErrorsEntry object*/
  bool getMatrix(EMClusterErrorsMatrix &apClusterErrorsMatrix) const;
 		
  /** Setting DatabaseID */
  void	setDatabaseID(std::string dbaseID);
  /** Getting DatabaseID */
  std::string	getDatabaseID() const;

  int		whichTypeAmI()
  {
    if (!m_vecInt_Info.empty()) return m_vecInt_Info[0];
    return -1;
  }
		
  void	getBasicInformation(std::string &dbaseID, unsigned int &nSize_Strings, unsigned int &nSize_FloatInfo, unsigned int &nSize_IntInfo);
		
  // 		/** clear all information*/
  void	clear();
		
  EMInsituDatabaseEntry* clone() const;

  void debugPrint() const; // dump the contents to stdout for debugging

 protected:
  // TDirectory *fTransient; //! current directory

//   Int_t				m_ID;
  std::string			m_DatabaseID;
  std::vector<std::string>	m_vecString;
  std::vector<Float_t>		m_vecFloat_Data;
  std::vector<Int_t>		m_vecInt_Data;
  std::vector<Float_t>		m_vecFloat_Info;
  std::vector<Int_t>		m_vecInt_Info;

  //  ClassDef(EMInsituDatabaseEntry, 1);
};


// I think we don't need storegate stuff
// #ifndef EMInsituRepresentation_OutsideAthena_h
// #include "CLIDSvc/CLASS_DEF.h"
// CLASS_DEF( Insitu::EMInsituDatabaseEntry , 180555189 , 1 )
// #endif

#endif
