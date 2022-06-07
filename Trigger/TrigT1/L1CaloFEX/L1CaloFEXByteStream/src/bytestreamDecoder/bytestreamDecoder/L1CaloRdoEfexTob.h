/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1CALO_RDO_EFEX_EMTAU_H
#define L1CALO_RDO_EFEX_EMTAU_H

#include "bytestreamDecoder/L1CaloRdoFexTob.h"

class L1CaloRdoEfexTob : public L1CaloRdoFexTob
{
public:
   L1CaloRdoEfexTob( int crate, int module, int eta, int phi, int numSlices,
                     TobType tobType, TobSource source, int id = 0, int fibre = 0, int tobSeq = 0 ); 
   
   virtual void infoSpecific( ) const override;
   
   //??bool getOverflow( size_t slice ) const;
   //??bool getOverflow( ) const;
   
   int getClusterEt( size_t slice ) const;
   int getClusterEt( ) const;
   int getIsol( size_t slice ) const;
   int getIsol( ) const;

private:
};

#endif
