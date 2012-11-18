/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaConditions/EMInsituDatabaseEntry.h"
#include <iostream>
#include "egammaConditions/EMClusterErrorsMatrix.h"

//ClassImp(EMInsituDatabaseEntry)

/** Default Constructor*/
EMInsituDatabaseEntry::EMInsituDatabaseEntry()
{
	clear();
}


/** Constructor Defining DatabaseID*/
EMInsituDatabaseEntry::EMInsituDatabaseEntry(std::string dbaseID)
{
	clear();
	setDatabaseID(dbaseID);
}

/** Default Destructor*/
EMInsituDatabaseEntry::~EMInsituDatabaseEntry()
{
}

/** Copy Constructor*/
EMInsituDatabaseEntry::EMInsituDatabaseEntry(const EMInsituDatabaseEntry& ob)
{
// 	m_ID		= ob.m_ID;
	m_DatabaseID	= ob.m_DatabaseID;
	m_vecString	= ob.m_vecString;
	m_vecFloat_Data	= ob.m_vecFloat_Data;
	m_vecInt_Data	= ob.m_vecInt_Data;
	m_vecFloat_Info	= ob.m_vecFloat_Info;
	m_vecInt_Info	= ob.m_vecInt_Info;
}

EMInsituDatabaseEntry& EMInsituDatabaseEntry::operator = (const EMInsituDatabaseEntry& ob)
{
  //	m_ID		= ob.m_ID;
	m_DatabaseID	= ob.m_DatabaseID;
	m_vecString	= ob.m_vecString;
	m_vecFloat_Data	= ob.m_vecFloat_Data;
	m_vecInt_Data	= ob.m_vecInt_Data;
	m_vecFloat_Info	= ob.m_vecFloat_Info;
	m_vecInt_Info	= ob.m_vecInt_Info;
        return *this;
}

/** Fill this EMInsituDatabaseEntry from given APEfficiencyMatrix*/
bool EMInsituDatabaseEntry::setMatrix(const EMClusterErrorsMatrix &apClusterErrorsMatrix)
{
  //std::cerr << "setMatrix called"<< std::endl;
  if (apClusterErrorsMatrix.m_matrix.size()==0) {
    return false;
  }
  clear();
  // Writing ID of EfficiencyMatrix
  m_vecInt_Info.push_back(EMInsituDatabase_EMClusterErrorsMatrixID);
//   m_ID = EMInsituDatabase_EMClusterErrorsMatrixID;
  // Writing Special Tag of EMClusterErrors
  m_vecInt_Info.push_back(0);	// is this like the version number or something?
  // Writing Information of EMClusterErrors
  m_vecInt_Info.push_back((int)apClusterErrorsMatrix.m_dimensions);
  for(unsigned i=0; i<apClusterErrorsMatrix.m_dimensions; i++) {
    std::vector<double> axisBinning = apClusterErrorsMatrix.m_axis.at(i).getBinningInformation();
    m_vecInt_Info.push_back((int)axisBinning.size());
    m_vecString.push_back(apClusterErrorsMatrix.m_axis.at(i).getName());
    for (unsigned int j=0; j < axisBinning.size(); j++) {
      m_vecFloat_Info.push_back(axisBinning.at(j));
    }
  }

  const int matrixSize = (int)apClusterErrorsMatrix.m_matrix.size();
  m_vecInt_Info.push_back(matrixSize);
    
  // push back the number of floats to store per matrix entry
  const int vectSize = (matrixSize) ? apClusterErrorsMatrix.m_matrix.at(0).size() : 0;
  m_vecInt_Info.push_back(vectSize);
	
  for (int i=0; i<matrixSize; i++) {
    const EMClusterErrorsEntry vect = apClusterErrorsMatrix.m_matrix.at(i);
    if ((int)vect.size() != vectSize) {
      std::cerr << "The input APEMClusterErrorsMatrix doesn't have entries with all having the same number of floats" << std::endl; // shoudl this go to a message stream instead? The macros didn't work.
      clear();
      return false;
    }
    for (int j=0; j<vectSize; j++) {
      m_vecFloat_Data.push_back(vect.at(j));
    }
  }
  m_vecString.push_back(apClusterErrorsMatrix.getTextDescription());
  return true;
}

/** Fill EMClusterErrors from this EMClusterErrorsDatabaseEntry object*/
bool EMInsituDatabaseEntry::getMatrix(EMClusterErrorsMatrix &apClusterErrorsMatrix) const
{
  apClusterErrorsMatrix.clear();
    
  if (m_vecInt_Info.size()<3) {
    return false;
  }
  // Reading and testing ID of entry
  if (m_vecInt_Info.at(0)!=EMInsituDatabase_EMClusterErrorsMatrixID) {
    return false;
  }
  //  m_ID =m_vecInt_Info.at(0);
  // Reading Tag of entry
  int nTag=m_vecInt_Info.at(1);
  if (nTag==-1) {
    return false;
  }
	
  apClusterErrorsMatrix.m_dimensions = m_vecInt_Info.at(2);

  // m_vecInt_info(3) is next, so store the index:
  unsigned int m=3;

  // m_vecFloat_Info(0) is next, so store the index
  unsigned int n=0;

  // m_vecString(0) is next, so store the index;
  unsigned int s=0;

  //   std::vector<APMatrixAxis> axes;
  std::vector<double> axisBinning;
  unsigned int ib=1;
  for (unsigned int i=0; i<apClusterErrorsMatrix.m_dimensions; i++) {
    axisBinning.clear();
    for (int j=0; j<m_vecInt_Info.at(m); j++) {
      axisBinning.push_back(m_vecFloat_Info.at(n++));
    }
    apClusterErrorsMatrix.m_axis.push_back(EMAPMatrixAxis(m_vecString.at(s++),axisBinning));
    apClusterErrorsMatrix.m_base.push_back(ib);
    ib+=m_vecInt_Info.at(m++);
  }
    
  //     for (unsigned int i=0; i<apClusterErrorsMatrix.m_dimensions; i++) {
  //       m_axis.push_back(axes.at(i));
  //     }
    
  const int matrixSize = m_vecInt_Info.at(m++);
  const int vectSize =  m_vecInt_Info.at(m);

  //std::cerr << "matrixSize = " << matrixSize << ", vectSize = " << vectSize << std::endl;

  // the data index;
  unsigned int di=0;

  // std::cerr << "matrix size before is " << apClusterErrorsMatrix.m_matrix.size() << std::endl;
  for (int i=0; i< matrixSize; i++) {
    EMClusterErrorsEntry entry;
    for (int j= 0; j < vectSize; j++) {
      //std::cerr << "reading value: " << m_vecFloat_Data.at(di) << std::endl;
      entry.push_back(m_vecFloat_Data.at(di++));
    }
    apClusterErrorsMatrix.m_matrix.push_back(entry);
  }
  // std::cerr << "matrix size is " << apClusterErrorsMatrix.m_matrix.size() << std::endl;
  apClusterErrorsMatrix.setTextDescription(m_vecString.at(s));
  return true;
}


/** Setting DatabaseID */
void	EMInsituDatabaseEntry::setDatabaseID(std::string dbaseID)
{
	m_DatabaseID	= dbaseID;
}


/** Getting DatabaseID */
std::string	EMInsituDatabaseEntry::getDatabaseID() const
{
	return m_DatabaseID;
}


/** clear all information*/
void	EMInsituDatabaseEntry::clear()
{
// 	m_ID		= -1;
	m_DatabaseID	= "EMDatabaseEntry";
	m_vecString.clear();
	m_vecFloat_Data.clear();
	m_vecInt_Data.clear();
	m_vecFloat_Info.clear();
	m_vecInt_Info.clear();
}

EMInsituDatabaseEntry*	EMInsituDatabaseEntry::clone() const
{
	return new EMInsituDatabaseEntry(*this);
}

void	EMInsituDatabaseEntry::getBasicInformation(std::string &dbaseID, unsigned int &nSize_Strings, unsigned int &nSize_FloatInfo, unsigned int &nSize_IntInfo)
{
	dbaseID 		= m_DatabaseID;
	nSize_Strings		= m_vecString.size();
	nSize_FloatInfo		= m_vecFloat_Info.size();
	nSize_IntInfo		= m_vecInt_Info.size();
}

void EMInsituDatabaseEntry::debugPrint() const
{
  std::cout << "*********** Dumping contents of EMInsituDatabaseEntry **********" << std::endl;
  std::cout << "  Database ID: " << m_DatabaseID << std::endl;

  std::cout << "  vecString of size " << m_vecString.size() << ":\n    ";
  for (unsigned int i = 0; i < m_vecString.size(); i++) {
    if (i!= 0) std::cout << ", ";
    std::cout << m_vecString.at(i);
  }
  std::cout << std::endl;

  std::cout << "  vecFloat_Data of size " << m_vecFloat_Data.size() << ":\n    ";
  for (unsigned int i = 0; i < m_vecFloat_Data.size(); i++) {
    if (i!= 0) std::cout << ", ";
    std::cout << m_vecFloat_Data.at(i);
  }
  std::cout << std::endl;

  std::cout << "  vecInt_Data of size " << m_vecInt_Data.size() << ":\n    ";
  for (unsigned int i = 0; i < m_vecInt_Data.size(); i++) {
    if (i!= 0) std::cout << ", ";
    std::cout << m_vecInt_Data.at(i);
  }
  std::cout << std::endl;

  std::cout << "  vecFloat_Info of size " << m_vecFloat_Info.size() << ":\n    ";
  for (unsigned int i = 0; i < m_vecFloat_Info.size(); i++) {
    if (i!= 0) std::cout << ", ";
    std::cout << m_vecFloat_Info.at(i);
  }
  std::cout << std::endl;

  std::cout << "  vecInt_Info of size " << m_vecInt_Info.size() << ":\n    ";
  for (unsigned int i = 0; i < m_vecInt_Info.size(); i++) {
    if (i!= 0) std::cout << ", ";
    std::cout << m_vecInt_Info.at(i);
  }
  std::cout << std::endl;
}
