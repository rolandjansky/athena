/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOIDENTIFIER_CALONEIGHBOURS_H
#define CALOIDENTIFIER_CALONEIGHBOURS_H
//-----------------------------------------------------------------------
// File and Version Information:
// 
//$Id: CaloNeighbours.h,v 1.2 2008-12-13 04:57:11 ssnyder Exp $
// 
//
// Description: Initialize and provide access to neighbours accross
//              the calorimeter systems. The neighbours within a 
//              a single system are provided by the individual helper.
//   
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Sven Menke
//
//-----------------------------------------------------------------------
#include <string>
#include <map>
#include <vector>
#include "CaloIdentifier/CaloCell_ID.h"
#include "Identifier/Range.h"
class ExpandedIdentifier;

enum NEIGHBOURTYPE {nextInCalo,prevInCalo};

struct ltIdHash{
  bool operator()(const IdentifierHash& id1, const IdentifierHash& id2) const
  {
    return ((unsigned int)id1) < ((unsigned int)id2);
  }
};


class CaloNeighbourRegion {
 public:

  enum {SIDE = 2, SAMPL = 3, REGION = 4, ETA = 5, PHI = 6, TILESECTION = 1, TILEPHI = 3, TILEETA = 4, TILESAMPL = 5, FCALETA = 4, FCALPHI = 5};
 
  CaloNeighbourRegion(const std::string& name,
                      const CaloCell_Base_ID * theCaloId);

  virtual ~CaloNeighbourRegion();

  void setType(const NEIGHBOURTYPE type);
  void setSourceRange(const Range & theRange);
  void setTargetRange(const Range & theRange);
  void setSide(const int side1, const int side2);
  void setPhi(const int phi1, const int phi2);
  int setNeighbours(ExpandedIdentifier& id1, std::vector<ExpandedIdentifier>& id2, std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>& neighbourMapPlus, std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>& neighbourMapMinus);

  void initializeVectors(std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>& neighbourMapPlus, std::map<IdentifierHash, std::vector<IdentifierHash>, ltIdHash>& neighbourMapMinus);

  int  getNeighbours(const IdentifierHash caloHash, std::vector<IdentifierHash>& neighbourList) const;

  inline NEIGHBOURTYPE getType() const {
    return m_type;
  }

  inline const std::string getName() const {
    return m_name;
  }

 private:
  
  int getId(ExpandedIdentifier& id, Identifier &rID, const Range &theRange, const int side = +1, const int dphi = 0) const;

  std::string m_name;
  NEIGHBOURTYPE m_type;
  const CaloCell_Base_ID * m_calo_id;

  Range m_sourceRange,m_targetRange;

  bool m_hasPhi;
  int  m_iPhiSource,m_iPhiTarget;

  bool m_hasSide;
  int  m_iSideSource,m_iSideTarget;

  bool m_isValidMinus,m_isValidPlus;

  IdentifierHash m_minHashMinus,m_maxHashMinus,m_minHashPlus,m_maxHashPlus;
  std::vector< std::unique_ptr<std::vector<IdentifierHash> > > m_neighbours_plus;
  std::vector< std::unique_ptr<std::vector<IdentifierHash> > > m_neighbours_minus;
};

class CaloNeighbours {

public:

  // default constructor
  CaloNeighbours();

  // default destructor
  virtual ~CaloNeighbours();

  int initialize(const CaloCell_Base_ID* caloID, std::string filename);

  int get_nextInCalo(const IdentifierHash &id,std::vector<IdentifierHash>& neighbourList) const;

  int get_prevInCalo(const IdentifierHash &id,std::vector<IdentifierHash>& neighbourList) const;

  int get_neighbours(const IdentifierHash &id,
                     const std::vector<std::unique_ptr<CaloNeighbourRegion> > &regions,
                     std::vector<IdentifierHash>& neighbourList) const;

 private:
  const CaloCell_Base_ID * m_calo_id;

  std::vector<std::unique_ptr<CaloNeighbourRegion> > m_next_regions;
  std::vector<std::unique_ptr<CaloNeighbourRegion> > m_prev_regions;
};
#endif // CALOIDENTIFIER_CALONEIGHBOURS_H
