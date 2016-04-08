/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBElementID_H
#define TBElementID_H

// every time you add new element here
// you need to update TileDetDescrManager constructor as well
namespace TBElementID {
  enum TBElementID {
	  Unknown                =   0,
          BEAMPIPE1              =   1,
          BEAMPIPE2              =   2,
          SMV                    =   3,  // before the MBPS1 magnet 
          S1                     =   4,
          S2                     =   5,
          S3                     =   6,
          SH                     =   7,
          SNEW                   =   8,
          MBPSID                 =   9,  // MBPS1 magnet   
          IDET                   =  10,
          Pixel                  =  11,
	  SCT                    =  15,
	  CTBTRT                 =  20,
	  TRT                    =  21,
	  CALO                   =  40,
	  LArTBBarrel            =  41,
	  Tile                   =  50,
	  MuonWall               =  51,
	  PhantomBarrel          =  52,
	  CombinedScintillator   =  53,
	  DeadMaterial           =  54,
          LArCrate               =  55,
	  MUON                   =  60,
	  Muon                   =  61,  
	  DUMPIRON               =  62,  
	  MBPL                   =  63,  // MBPL magnet  
	  MBPS2                  =  64,  // MBPS2 magnet 
	  SCDUMP           =  70,
	  SMT              =  71,
	  MUON10x10A       =  72,
	  MUON10x10B       =  73,
	  MUON10x10C       =  74,
	  MUON10x10D       =  75,
	  MUONHODOSELECTOR =  76,
	  MUONHODOPLANE11  =  80,
	  MUONHODOPLANE12  =  81,
	  MUONHODOPLANE13  =  82,
	  MUONHODOPLANE14  =  83,
	  MUONHODOPLANE15  =  84,
	  MUONHODOPLANE16  =  85,
	  MUONHODOPLANE21  =  90,
	  MUONHODOPLANE22  =  91,
	  MUONHODOPLANE23  =  92,
	  MUONHODOPLANE24  =  93,
	  MUONHODOPLANE25  =  94,
	  MUONHODOPLANE26  =  95,
          TotalSize = 100
		    };
}
#endif
