/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITRT_StrawMap.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IINDETTRT_STRAWMAP_H
#define IINDETTRT_STRAWMAP_H

#include "GaudiKernel/IAlgTool.h"

//#include "Identifier/Identifier.h"
//class Identifier;


namespace InDet 
{


  static const InterfaceID IID_ITRT_StrawMap("InDet::ITRT_StrawMap", 1, 0);

  class ITRT_StrawMap : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here

// need these???
//      ITRT_StrawMap(const std::string&,const std::string&,const IInterface*);
//      virtual ~ITRT_StrawMap ();

//      virtual StatusCode initialize();
//      virtual StatusCode finalize  ();




    /** return board number in range 0-28; 
        0-8 for barrel (1S, 1L, 2S, 2L1, 2L2, 3S1, 3S2, 3L1, 3L2) 
        9-28 for endcap (from smaller |z| to larger |z|) */
//    virtual int board(Identifier id);
    /** will return the board number of the last identifier that was set */
//    virtual int board();
    /** return board number based on a straw number */
 //   virtual int strawToBoard(int straw);
    /** takes as an input chip number that follows 0-343 convention described below */
 //   virtual int chipToBoard(int chip);

    /** return chip number in range 0-343; 
        0-103 for barrel (same order as in hardware) 
        103-343 for endcap (from smaller |z| to larger |z|, always 12 per board, 0-11 order same as in hardware) */
//    virtual int chip(Identifier id);
    /** will return the chip number of the last identifier set */
//    virtual int chip();
    /** need side and phi information due to reversed numbering in some endcap phi stacks */
  //  virtual int strawToChip(int straw, int side, int phi);

    /** return straw number in range 0-5481; 
        0-1641 for barrel (same order as in hardware) 
        1642-5481 for endcap (from smaller |z| to larger |z|, same order as in hardware) */
//    virtual int straw(Identifier id);
 //   virtual int straw();

    /** set private members appropriately: m_id, m_straw (index), m_reverse */
 //   virtual void setNewIdentifier(Identifier id, int *index = 0);

    /** prints out comparison with TRTStrawNeighbour tool (to debug the later) - very long output file! */
//    virtual void compareWithTRTStrawNeighbour();

  };

  inline const InterfaceID& InDet::ITRT_StrawMap::interfaceID()
    { 
      return IID_ITRT_StrawMap; 
    }

} // end of namespace

#endif 
