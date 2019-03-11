/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: CaloNeighbours.cxx,v 1.2 2006-04-28 12:08:07 menke Exp $
//
// Description: see CaloNeighbours.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "CaloIdentifier/CaloNeighbours.h"
//---------------
// C++ Headers --
//---------------
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>

#include "Identifier/ExpandedIdentifier.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "PathResolver/PathResolver.h"

#define MAX_BUFFER_LEN 1024

//###############################################################################

CaloNeighbourRegion::CaloNeighbourRegion(const std::string name,
                                         const CaloCell_Base_ID *theCaloId) :
  m_type(nextInCalo),
  m_calo_id(0),
  m_hasPhi(false),
  m_iPhiSource(0),
  m_iPhiTarget(0),
  m_hasSide(false),
  m_iSideSource(0),
  m_iSideTarget(0),
  m_isValidMinus(false),
  m_isValidPlus(false)
{
  m_name = name;
  m_calo_id = theCaloId;
}

//###############################################################################

CaloNeighbourRegion::~CaloNeighbourRegion()
{
  std::vector< std::vector<IdentifierHash>* >::iterator nIter = m_neighbours_plus.begin();
  std::vector< std::vector<IdentifierHash>* >::iterator nIterEnd = m_neighbours_plus.end();
  for(; nIter != nIterEnd; nIter++ ) {
    if ( (*nIter) ) {
      delete (*nIter);
      (*nIter) = 0;
    }
  }
  nIter = m_neighbours_minus.begin();
  nIterEnd = m_neighbours_minus.end();
  for(; nIter != nIterEnd; nIter++ ) {
    if ( (*nIter) ) {
      delete (*nIter);
      (*nIter) = 0;
    }
  }
}

//###############################################################################

void CaloNeighbourRegion::setType(const NEIGHBOURTYPE type)
{
  m_type = type;
}

//###############################################################################

void CaloNeighbourRegion::setSourceRange(const Range & theRange)
{
  m_sourceRange = theRange;
  for(unsigned int i=0;i<theRange[SIDE].get_indices();i++) {
    if ( theRange[SIDE].get_value_at(i) < 0 )
      m_isValidMinus = true;
    else
      m_isValidPlus = true;
  }
}

//###############################################################################

void CaloNeighbourRegion::setTargetRange(const Range & theRange)
{
  m_targetRange = theRange;
}

//###############################################################################

void CaloNeighbourRegion::setSide(const int side1, const int side2)
{
  m_hasSide = true;
  m_iSideSource = side1;
  m_iSideTarget = side2;
}

//###############################################################################

void CaloNeighbourRegion::setPhi(const int phi1, const int phi2)
{
  m_hasPhi = true;
  m_iPhiSource = phi1;
  m_iPhiTarget = phi2;

}

//###############################################################################

int CaloNeighbourRegion::getId(ExpandedIdentifier& id, Identifier &rID, const Range &theRange, const int side, const int dphi) const
{
  // modify id for side and phi
  //  ExpandedIdentifier id(oid);
  if ( m_hasSide && side != 1 )
    id[SIDE] = side*id[SIDE];
  if ( m_hasPhi && dphi != 0 ) {
    if ( m_calo_id->is_lar_em(id) || m_calo_id->is_lar_hec(id) )
      id[PHI] = id[PHI]+dphi;
    else if ( m_calo_id->is_lar_fcal(id) )
      id[FCALPHI] = id[FCALPHI]+dphi;
    else 
      id[TILEPHI] = id[TILEPHI]+dphi;
  }

  // first check if id is in the specified range
  if ( !theRange.match(id) ) {
    std::cout << "CaloNeighbours::set_neighbours ERROR: " << (std::string)id << " is not within the Range " << (std::string)theRange << std::endl;
    return 1;
  }

  CaloCell_ID::SUBCALO subCalo;
  if ( m_calo_id->is_lar_em(id) ) {
    subCalo = CaloCell_ID::LAREM;
    rID = m_calo_id->cell_id(subCalo,
			     id[SIDE],
			     id[SAMPL],
			     id[REGION],
			     id[ETA],
			     id[PHI]);
  }
  else if ( m_calo_id->is_lar_hec(id) ) {
    subCalo = CaloCell_ID::LARHEC;
    rID = m_calo_id->cell_id(subCalo,
			     id[SIDE],
			     id[SAMPL],
			     id[REGION],
			     id[ETA],
			     id[PHI]);
  }
  else if ( m_calo_id->is_lar_fcal(id) ) {
    subCalo = CaloCell_ID::LARFCAL;
    rID = m_calo_id->cell_id(subCalo,
			     id[SIDE],
			     id[SAMPL],
			     0,
			     id[FCALETA],
			     id[FCALPHI]);
  }
  else if ( m_calo_id->is_tile(id) ) {
    subCalo = CaloCell_ID::TILE;
    ExpandedIdentifier::element_type sampl = 0;
    if (id.fields() > TILESAMPL)
      sampl = id[TILESAMPL];
    rID = m_calo_id->cell_id(subCalo,
			     id[TILESECTION],
			     id[SIDE],
			     id[TILEPHI],
			     id[TILEETA],
			     sampl);
  }
  else {
    std::cout << "CaloNeighbours::get_id ERROR: " << (std::string)id << " is not in the LAr or Tile calorimeters" << std::endl;
    return 1;
  }
  return 0;
}

//###############################################################################

int CaloNeighbourRegion::setNeighbours(ExpandedIdentifier& id1, 
				       std::vector<ExpandedIdentifier>& id2, 
				       std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>& neighbourMapPlus, 
				       std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>& neighbourMapMinus)
{
  int result = 0;

  // build an Identifier from the ExpandedIdentifier
  int nMaxSides=1;
  int nMaxPhi=1;
  float  rPhi=1;
  if ( m_hasSide )
    nMaxSides = 2;
  if ( m_hasPhi ) {
    nMaxPhi = m_sourceRange[m_iPhiSource].get_indices();
    rPhi = ((float)m_targetRange[m_iPhiTarget].get_indices())/((float)nMaxPhi);
  }

  for (int theSide = 0;theSide<nMaxSides;theSide++) {
    for (int theDPhi = 0;theDPhi<nMaxPhi;theDPhi++) {
      int side = +1;
      int dphi2 = (int)(theDPhi*rPhi+0.05);
      if ( theSide > 0 ) {
	side = -1;
      }
      // only continous region mapping in phi is supported - 
      // therefore theDPhi = delta phi ...
      Identifier myId;
      ExpandedIdentifier myExpID(id1);
      if ( getId(myExpID,myId,m_sourceRange,side,theDPhi) ) {
	return 1;
      }
      // get hash ID's for the source 
      IdentifierHash myHash = m_calo_id->calo_cell_hash (myId);
      // get ID's and hash ID's for the target
      std::vector<ExpandedIdentifier>::const_iterator idIter    = id2.begin();
      std::vector<ExpandedIdentifier>::const_iterator idIterEnd = id2.end();
      std::vector<IdentifierHash> theTargetHashIDs;
      for ( ; idIter != idIterEnd; idIter++ ) {
	Identifier myTargetId;
	ExpandedIdentifier myNExpID(*idIter);
	if ( getId(myNExpID,myTargetId,m_targetRange,side,dphi2) ) {
	  return 1;
	}
	// get hash ID's for the target
	IdentifierHash myTargetHash = m_calo_id->calo_cell_hash (myTargetId);
	theTargetHashIDs.push_back(myTargetHash);
      }
      if ( id1[SIDE]*side > 0 ) {
	neighbourMapPlus[myHash] = theTargetHashIDs;
      }
      else {
	neighbourMapMinus[myHash] = theTargetHashIDs;
      }
    }
  }
  return result;
}

//###############################################################################

void  CaloNeighbourRegion::initializeVectors(std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>& neighbourMapPlus, std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>& neighbourMapMinus)
{
  if ( m_isValidMinus ) {
    std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>::const_iterator first = neighbourMapMinus.begin();
    std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>::const_iterator end = neighbourMapMinus.end();
    std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>::const_reverse_iterator last = neighbourMapMinus.rbegin();
    m_minHashMinus = (*first).first;
    m_maxHashMinus = (*last).first;
    m_neighbours_minus.resize((unsigned int)(m_maxHashMinus-m_minHashMinus+1),0);
    for(;first != end;first++) {
      m_neighbours_minus[(unsigned int)((*first).first-m_minHashMinus)] = new std::vector<IdentifierHash>((*first).second);
    }
  }
  if ( m_isValidPlus ) {
    std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>::const_iterator first = neighbourMapPlus.begin();
    std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>::const_iterator end = neighbourMapPlus.end();
    std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>::const_reverse_iterator last = neighbourMapPlus.rbegin();
    m_minHashPlus = (*first).first;
    m_maxHashPlus = (*last).first;
    m_neighbours_plus.resize((unsigned int)(m_maxHashPlus-m_minHashPlus+1),0);
    for(;first != end;first++) {
      m_neighbours_plus[(unsigned int)((*first).first-m_minHashPlus)] = new std::vector<IdentifierHash>((*first).second);
    }
  }

}

//###############################################################################

int  CaloNeighbourRegion::getNeighbours(const IdentifierHash caloHash, std::vector<IdentifierHash>& neighbourList) const 
{
  int result = 0; 
  
  if ( m_isValidMinus && caloHash >= m_minHashMinus && caloHash <= m_maxHashMinus && m_neighbours_minus[(unsigned int)(caloHash-m_minHashMinus)] ) {
    neighbourList.insert(neighbourList.end(),
			 m_neighbours_minus[(unsigned int)(caloHash-m_minHashMinus)]->begin(),
			 m_neighbours_minus[(unsigned int)(caloHash-m_minHashMinus)]->end());
  }
  else if ( m_isValidPlus && caloHash >= m_minHashPlus && caloHash <= m_maxHashPlus && m_neighbours_plus[(unsigned int)(caloHash-m_minHashPlus)] ) {
    neighbourList.insert(neighbourList.end(),
			 m_neighbours_plus[(unsigned int)(caloHash-m_minHashPlus)]->begin(),
			 m_neighbours_plus[(unsigned int)(caloHash-m_minHashPlus)]->end());
  }

  return result;
}


//###############################################################################

CaloNeighbours::CaloNeighbours() :
  m_calo_id(0)
{ }

//###############################################################################

CaloNeighbours::~CaloNeighbours()
{
  std::vector<CaloNeighbourRegion*>::iterator rIter    = m_next_regions.begin();
  std::vector<CaloNeighbourRegion*>::iterator rIterEnd = m_next_regions.end();
  for(; rIter != rIterEnd; rIter++ ) {
    if ((*rIter)) {
      delete (*rIter);
      (*rIter) = 0;
    }
  }

  rIter    = m_prev_regions.begin();
  rIterEnd = m_prev_regions.end();
  for(; rIter != rIterEnd; rIter++ ) {
    if ((*rIter)) {
      delete (*rIter);
      (*rIter) = 0;
    }
  }
}

//###############################################################################

int CaloNeighbours::initialize(const CaloCell_Base_ID* caloID,
                               std::string filename)
{
  int result =0;
  //  std::cout << " CaloNeighbours::initialize " << std::endl;
  m_calo_id = caloID;
  m_next_regions.resize(0);
  m_prev_regions.resize(0);
  // Find the full path to filename:
  std::string file = PathResolver::find_file (filename, "DATAPATH");
  //    std::cout << "Reading file  " << file << std::endl;
  std::ifstream fin;
  if (file != "") {
    fin.open(file.c_str());
  }
  else {
    std::cout << "CaloNeighbours::initialize ERROR Could not find input file " << filename <<  std::endl;
    return 1;
  }
  if (fin.bad()) {
    std::cout << "CaloNeighbours::initialize ERROR Could not open file " << file << std::endl;
    return 1;
  }

  //
  // Parse the input file
  //

  char aLine[MAX_BUFFER_LEN],dummy;
  std::string sLine;
  std::string cPrevOrNext,cName,cSourceRange,cTargetRange;
  std::string cExpId;
  Range mySourceRange,myTargetRange;
  std::string oArg;
  bool isComment = true;
  bool isNext = false;
  bool isPrev = false;
  const std::string cmdNext("nextSuperCalo");
  const std::string cmdPrev("prevSuperCalo");
  const std::string cmdPhi("calcPhi(");
  const std::string cmdSide("calcSide(");
 
  while( isComment ) {
    sLine.resize( 0 );
    do {  
      fin.getline(aLine,sizeof(aLine)-1);
      sLine = std::string(aLine);
    } while (sLine == "" && !fin.eof());
    isComment = ( sLine.find("#") != std::string::npos );
  }
  do {
    while ( sLine == "" && !fin.eof()) {
      fin.getline(aLine,sizeof(aLine)-1);
      sLine = std::string(aLine);
    }
    std::istringstream header( sLine.c_str() );
    bool hasPhi=false;
    bool hasSide=false;
    int iPhiSource,iPhiTarget;
    int iSideSource,iSideTarget;
    if ( header >> cPrevOrNext >> cName >> cSourceRange >> cTargetRange) {
      mySourceRange.build(cSourceRange);
      myTargetRange.build(cTargetRange);
      while ( header >> oArg ) {
	if (oArg.find(cmdSide) != std::string::npos ) {
	  hasSide = true;
	  oArg.erase(0,cmdSide.size());
	  std::istringstream iside( oArg.c_str() );
	  iside >> iSideSource >> dummy >> iSideTarget;
	}
	else if ( oArg.find(cmdPhi) != std::string::npos ) {
	  hasPhi = true;
	  oArg.erase(0,cmdPhi.size());
	  std::istringstream iphi( oArg.c_str() );
	  iphi >> iPhiSource >> dummy >> iPhiTarget;
	}
      }
      sLine.resize(0);
      bool endOfBlock = false;
      isNext = (cPrevOrNext.find(cmdNext) != std::string::npos);
      isPrev = (cPrevOrNext.find(cmdPrev) != std::string::npos);
      if ( isNext^isPrev ) {
//	std::cout << (isNext?cmdNext:cmdPrev) << " " 
//		  << cName << " " 
//		  << (std::string)mySourceRange << " " 
//		  << (std::string)myTargetRange;
//	if ( hasSide ) {
//	  std::cout << " hasSide(" << iSideSource << "," << iSideTarget << ")";
//	}
//	if ( hasPhi ) {
//	  std::cout << " hasPhi(" << iPhiSource << "," << iPhiTarget << ")";
//	}
//	std::cout << std::endl;
	// create new CaloNeighbourRegion
	CaloNeighbourRegion * myRegion = new CaloNeighbourRegion(cName,m_calo_id);
	if ( isNext )
	  myRegion->setType(nextInCalo);
	else
	  myRegion->setType(prevInCalo);
	if ( hasSide )
	  myRegion->setSide(iSideSource,iSideTarget);
	if ( hasPhi )
	  myRegion->setPhi(iPhiSource,iPhiTarget);
	myRegion->setSourceRange(mySourceRange);
	myRegion->setTargetRange(myTargetRange);
	// create temporary neighbour maps
	std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash> neighbourMapPlus,neighbourMapMinus;

	do {
	  while ( !endOfBlock && sLine == "" && !fin.eof()) {
	    fin.getline(aLine,sizeof(aLine)-1);
	    sLine = std::string(aLine);
	    if ( sLine == "" || fin.eof() )
	      endOfBlock = true;
	  }
	  if (!endOfBlock) {
	    std::istringstream neighbour( sLine.c_str() );
	    ExpandedIdentifier myCell,myNeighbourCell;
	    std::vector<ExpandedIdentifier> myNeighbourCells;
	    if ( neighbour >> cExpId ) {
	      myCell = ExpandedIdentifier(cExpId);
	      // std::cout << (std::string)myCell << ":";
	      while ( neighbour >> cExpId ) {
		myNeighbourCell = ExpandedIdentifier(cExpId);
		// std::cout << " " << (std::string)myNeighbourCell;
		myNeighbourCells.push_back(myNeighbourCell);
	      }
	      // std::cout << std::endl;
	      sLine.resize(0);
	      result = myRegion->setNeighbours(myCell,myNeighbourCells,neighbourMapPlus,neighbourMapMinus);
	      if ( result != 0 ) 
		return result;
	    }
	  }
	} while (!fin.eof() && !endOfBlock);
	myRegion->initializeVectors(neighbourMapPlus,neighbourMapMinus);
	if (isNext) 
	  m_next_regions.push_back(myRegion);
	else
	  m_prev_regions.push_back(myRegion);
      }
      else {
	std::cout << "CaloNeighbours::initialize ERROR Invalid neighbour dat file, exiting ... " << std::endl;
	return 1;
      }
    }
  }while (!fin.eof()) ;
  fin.close();
  
  return 0;
}

//###############################################################################

int CaloNeighbours::get_nextInCalo(const IdentifierHash &id,std::vector<IdentifierHash>& neighbourList) const
{
  return get_neighbours(id,m_next_regions,neighbourList);
}

//###############################################################################

int CaloNeighbours::get_prevInCalo(const IdentifierHash &id,std::vector<IdentifierHash>& neighbourList) const
{
  return get_neighbours(id,m_prev_regions,neighbourList);
}

//###############################################################################

int CaloNeighbours::get_neighbours(const IdentifierHash &id, const std::vector<CaloNeighbourRegion*> &regions, std::vector<IdentifierHash>& neighbourList) const
{
  int result = 0;
  std::vector<CaloNeighbourRegion*>::const_iterator rIter    = regions.begin();
  std::vector<CaloNeighbourRegion*>::const_iterator rIterEnd = regions.end();
  for(; rIter != rIterEnd; rIter++ ) {
    result = (*rIter)->getNeighbours(id,neighbourList);
    if ( result != 0 )
      return result;
  }
  return result;
}

