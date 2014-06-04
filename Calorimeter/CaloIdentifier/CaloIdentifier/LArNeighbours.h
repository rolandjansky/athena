/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* date of creation : 28/07/2003 */

#ifndef LAR_NEIGHBOURS_H
#define LAR_NEIGHBOURS_H


namespace LArNeighbours {
  enum neighbourOption { prevInPhi   		= 0x0001, 
			 nextInPhi   		= 0x0002,
			 prevInEta   		= 0x0004, 
			 nextInEta   		= 0x0008, 
			 faces2D     		= prevInPhi|nextInPhi|prevInEta|nextInEta, 
			 corners2D   		= 0x0010, 
			 all2D       		= corners2D|faces2D,
			 prevInSamp  		= 0x0020,
			 nextInSamp  		= 0x0040,
			 upAndDown   		= prevInSamp|nextInSamp,
			 prevSubDet  		= 0x0080,
			 nextSubDet  		= 0x0100, 
			 all3D                  = all2D|prevInSamp|nextInSamp|upAndDown|prevSubDet|nextSubDet,
			 corners3D   		= 0x0200, 
			 all3DwithCorners       = all3D|corners3D,
			 prevSuperCalo          = 0x0400, 
			 nextSuperCalo          = 0x0800, 
			 super3D                = all3D|prevSuperCalo|nextSuperCalo};
}

#endif //LAR_NEIGHBOURS_H

