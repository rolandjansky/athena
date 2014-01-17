/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_StrawMap.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRT_STRAWMAP_H
#define INDETTRT_STRAWMAP_H

#include "TRT_DetectorPerformance/ITRT_StrawMap.h"
#include "AthenaBaseComps/AthAlgTool.h"

//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/ServiceHandle.h" 

/*

#include "StoreGate/StoreGateSvc.h"

#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "TRT_ConditionsTools/TRTStrawNeighbourTool.h"
*/

#include "Identifier/Identifier.h"

class TRT_ID;
namespace InDetDD { class TRT_DetectorManager; }
class ITRT_StrawNeighbourSvc;


namespace InDet 
{

  /** @class TRT_StrawMap 

  This tool provides a numbering scheme that is common to all algorithms 
  in TRT_DetectorPerformance package.
  For any other user, TRTStrawNeighbourTool is a recommended and default option.
  Once TRTStrawNeighbour tool is fully debugged and available for the endcap, 
  this tool can only serve as an interface.
      
  @author  Sasa Fratina <sasa.fratina@cern.ch>
  */  

  class TRT_StrawMap : virtual public ITRT_StrawMap, public AthAlgTool
    {
    public:
	
      TRT_StrawMap(const std::string&,const std::string&,const IInterface*);
      virtual ~TRT_StrawMap ();
      
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      /** return board number in range 0-28; 
          0-8 for barrel (1S, 1L, 2S, 2L1, 2L2, 3S1, 3S2, 3L1, 3L2) 
          9-28 for endcap (from smaller |z| to larger |z|) */
      int board(Identifier id);
      /** will return the board number of the last identifier that was set */
      int board(); 
      /** return board number based on a straw number */
      int strawToBoard(int straw); 
      /** takes as an input chip number that follows 0-343 convention described below */
      int chipToBoard(int chip); 

      /** return chip number in range 0-343; 
	  0-103 for barrel (same order as in hardware) 
	  103-343 for endcap (from smaller |z| to larger |z|, always 12 per board, 0-11 order same as in hardware) */
      int chip(Identifier id); 
      /** will return the chip number of the last identifier set */
      int chip(); 
      /** need side and phi information due to reversed numbering in some endcap phi stacks */ 
      int strawToChip(int straw, int side, int phi);
 
    private:
	
      int strawToChip(int straw, int reversed); 

    public:
      /** return straw number in range 0-5481; 
	  0-1641 for barrel (same order as in hardware) 
	  1642-5481 for endcap (from smaller |z| to larger |z|, same order as in hardware) */
      int straw(Identifier id); 
      int straw();
	  
      /** set private members appropriately: m_id, m_straw (index), m_reverse */
      void setNewIdentifier(Identifier id, int *index = 0);

      inline int nBarrelStraws() { return m_nBarrelStraws; }
      inline int nAllStraws() { return m_nAllStraws; }
      inline int nBarrelChips() { return m_nBarrelChips; }

      /** prints out comparison with TRTStrawNeighbour tool (to debug the later) - very long output file! */
      void compareWithTRTStrawNeighbour();       
      
      static const InterfaceID& interfaceID(){
      	static const InterfaceID id("TRT_StrawMap",1,0);
      	return id;
      }
      
      virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ){
      	if ( riid == interfaceID() )  {
      	  *ppvIf = this;
      	  addRef();
      	  return StatusCode::SUCCESS;
      	}
      	return AthAlgTool::queryInterface( riid, ppvIf );
      }
      
      
    private:
      
      const InDetDD::TRT_DetectorManager *m_TRTDetMgr;
      const TRT_ID *m_TRTHelper;

      Identifier m_id;
      int m_straw;
      int m_reverse; // needed for phi sectors 0-7 and 16-23 that have reversed chip order in the endcap

      int m_strawToPadMap[5482]; // 5482: number of all straws in one phi stack, one side 

      int m_nBarrelStraws;// = 1642;
      int m_nAllStraws;// = 5482;
      int m_nBarrelChips;// = 104;

      /** member variables for algorithm properties: */
      // int/double/bool  m_propertyName;

      ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourTool;
      
    }; 
	
  //	int TRT_StrawMap::s_nBarrelStraws = 1642; 
  //	int TRT_StrawMap::s_nAllStraws = 5482; 
  //	int TRT_StrawMap::s_nBarrelChips = 104; 
	
} // end of namespace

#endif 
