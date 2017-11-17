/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MDTREGION_H
#define  TRIGL2MUONSA_MDTREGION_H

namespace TrigL2MuonSA {

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

  class MdtRegion
  {
  public:
    MdtRegion() { Clear(); };    
    ~MdtRegion() {};
    
    void Clear()
	{
	  for(int i=0; i<11; i++) {
	    for(int j=0; j<2; j++) {
	      zMin[i][j] = 0;
	      zMax[i][j] = 0;
	      rMin[i][j] = 0;
	      rMax[i][j] = 0;
	      etaMin[i][j] = 0;
	      etaMax[i][j] = 0;
	      phiMin[i][j] = 0;
	      phiMax[i][j] = 0;
	      for(int k=0; k<2; k++) chamberType[i][j][k] = 0;	      
	    }
	  }
	};
      
  public:
      double zMin[11][2];
      double zMax[11][2];
      double rMin[11][2];
      double rMax[11][2];
      double etaMin[11][2];
      double etaMax[11][2];
      double phiMin[11][2];
      double phiMax[11][2];
      int chamberType[11][2][2];
  };

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
}

#endif
