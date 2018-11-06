/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHVIDMAPPING_H
#define LARHVIDMAPPING_H

#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include <vector>
#include <map>

class LArElectrodeID;
class LArHVLineID;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class CaloCell_ID;

class LArHVIdMapping {

  friend class LArHVIdMappingAlg; //The conditions alg filling this object

 public:
  LArHVIdMapping()=delete;
  LArHVIdMapping(const CaloCell_ID* caloId, const LArHVLineID* hvId, const LArElectrodeID* elId);

  /** Return the LArHVLineID corresponding to a given LArElectrodeId

   */ 
  const HWIdentifier getLArHVLineID(HWIdentifier& electrodeId) const;

  // Returns a vector<electrodeID> given an input offlineID
  //-------------------------------------------------------
  void getElectrodeInCell(const Identifier& offId, std::vector<HWIdentifier> &elecId ) const;

  /** Returns a vector<HVLineID> given an input offlineID
   */
  void getHVLineInCell( const Identifier& offId, std::vector<HWIdentifier> &hvlineId) const;


  /** Return a vector of LArElectrodeID corresponding to a given LArHVLineID
  */
  const std::vector<HWIdentifier>& getLArElectrodeIDvec(HWIdentifier& hvlineId) const;


  /** returns the *Module* of a given offId
   */
  int getCellModule(const Identifier& offId ) const;
  int getCellEtaSector(const Identifier& offId ) const;
  int getCellPhiSector(const Identifier& offId ) const;
  std::pair<int,int> getCellElectrodeMinMax(const Identifier& offId) const;
  
 private:

  const CaloCell_ID*     m_caloHelper ;
  const LArEM_ID   *     m_emHelper ;
  const LArHEC_ID  *     m_hecHelper ;
  const LArFCAL_ID *     m_fcalHelper ;
  const LArHVLineID *    m_hvlineHelper ;
  const LArElectrodeID*  m_electrodeHelper ;

  std::map<HWIdentifier,HWIdentifier> m_electrode_hvline_map;
  std::map<HWIdentifier,std::vector<HWIdentifier> > m_hvline_electrode_map;
  const std::vector<HWIdentifier> m_invalid;

};



#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArHVIdMapping, 260112632 , 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArHVIdMapping>, 92552832 , 1)

#endif
