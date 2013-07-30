/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*
 * TRT_FillCablingData Types
 * Used in _SR1 and _SR1_ECC
 */
#ifndef TRT_FILLCABLINGDATA_TYPES_H
#define TRT_FILLCABLINGDATA_TYPES_H

typedef struct {
   int SubDet;
   int Phi;
   int RODGroup;
   std::string FEid;
} GlobalCableMap_t;

 typedef std::map<int, std::vector< GlobalCableMap_t *> > GlobalCableMap;

#endif // TRT_FILLCABLINGDATA_TYPES_H
