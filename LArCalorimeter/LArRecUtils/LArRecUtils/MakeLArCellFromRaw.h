/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_MAKELARCELLFROMRAW_H
#define LARCELLREC_MAKELARCELLFROMRAW_H

/** @class MakeLArCellFromRaw
 @brief      Building LArCell objects from LArRawChannel 


     e_threshold:          energy threshold
*/


#include "LArRecEvent/LArCell.h" 
#include "LArIdentifier/LArOnlineID.h" 
#include "CaloUtils/CaloCellCorrection.h"
#include "CaloIdentifier/CaloGain.h" 
#include "AthAllocators/DataPool.h" 

#include <map>
#include <vector>
class CaloDetDescrElement ; 
class LArCablingLegacyService ;

class LArRoI_Map; 

class MakeLArCellFromRaw
{
public:

   struct CellInfo
	{
		Identifier id;
		double eta ; 
		double phi ; 
		double x ; 
		double y ; 
		double z ; 
		bool  fcal ; 
		unsigned int tt; 
		const CaloDetDescrElement* elem;
		double eCorr ; 
	}; 

  /** constructor
	*/ 
  MakeLArCellFromRaw(); 

  /** destructor 
  */ 
  ~MakeLArCellFromRaw(); 


  /** initialize the internal map
  */ 
  void initialize( const LArRoI_Map* map,
       const std::vector<CaloCellCorrection*>*  pCorr, unsigned int poolMaxSize =190000);

  /** access by Identifier 
  */ 
  LArCell* getLArCell( unsigned int feb, unsigned int chan, 
			int e, int t, int q , CaloGain::CaloGain caloGain, unsigned int & ttId) const;

  LArCell* getLArCell( const HWIdentifier& id, int e, int t, int q, 
		       CaloGain::CaloGain g) const ; 

  //FIXME will disappear
  LArCell* getLArCell( unsigned int feb, unsigned int chan, 
			int e, int t, int q , unsigned int & ttId) const;
  //FIXME will disappear
  LArCell* getLArCell( const HWIdentifier& id, int e, int t, int q ) const ; 


  void setThreshold(float t); 
 
private: 

 // Find correction factor for this LArCell 
 double getCorrection(LArCell* cell,
                      const std::vector<CaloCellCorrection*>& vCorr,
                      const EventContext& ctx) const;

 // cells in a FEB stored in a vector
 typedef std::vector<CellInfo> CELL_VEC ; 

 // all FEBs in a map. 
 typedef std::map<unsigned int, CELL_VEC  > CELL_MAP;

 mutable CELL_MAP  m_cellMap; 	

 mutable DataPool<LArCell> * m_dataPool;
 IMessageSvc * m_msgSvc ;
 
 LArCablingLegacyService* m_cablingSvc ;

 const LArOnlineID* m_onlineID;

 float m_ethreshold ; // common threshold in MeV

};

#endif 
