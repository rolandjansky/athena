/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

class LArRoIMap;
class LArOnOffIdMapping;
class CaloDetDescrManager;

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
  void initialize ( const LArRoIMap& roiMap,
                    const LArOnOffIdMapping& onOffMap,
                    const CaloDetDescrManager& man,
       const std::vector<const CaloCellCorrection*>*  pCorr, unsigned int poolMaxSize =190000);

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
                      const std::vector<const CaloCellCorrection*>& vCorr,
                      const EventContext& ctx) const;

 // cells in a FEB stored in a vector
 typedef std::vector<CellInfo> CELL_VEC ; 

 // all FEBs in a map. 
 typedef std::map<unsigned int, CELL_VEC  > CELL_MAP;

 CELL_MAP  m_cellMap; 	

 size_t m_poolMaxSize;
 IMessageSvc * m_msgSvc ;
 
 const LArOnlineID* m_onlineID;

 float m_ethreshold ; // common threshold in MeV

};

#endif 
