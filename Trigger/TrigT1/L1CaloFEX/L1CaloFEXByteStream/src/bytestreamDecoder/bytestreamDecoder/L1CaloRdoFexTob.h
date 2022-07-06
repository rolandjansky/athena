/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1CALO_RDO_FEX_TOB_H
#define L1CALO_RDO_FEX_TOB_H

#include <string>

#include "bytestreamDecoder/L1CaloRdo.h"

class L1CaloRdoFexTob : public L1CaloRdo
{
public:
   enum TobType { Invalid, EM, Tau, LargeJet, SmallJet, Energy, Muon };
   enum TobSource { EfexTob, EfexXtob, JfexTob, JfexXtob, GfexTob, GfexXtob, Ph1Topo };
   
   L1CaloRdoFexTob( int crate, int module, int eta, int phi, int numSlices,
                    TobType tobType, TobSource source, int id = 0, int fibre = 0, int tobSeq = 0 ); 

   bool sameDatum( const L1CaloRdoFexTob& rhs ) const;
   virtual std::string getType( ) const override;
   
   TobType getTobType( ) const;
   std::string getTobTypeString( ) const;
   int getTobID( ) const;
   bool getIsEM( ) const;
   bool getIsTau( ) const;
   bool getIsJet( ) const;
   bool getIsLargeJet( ) const;
   bool getIsSmallJet( ) const;
   bool getIsEnergy( ) const;
   bool getIsMuon( ) const;
   
   TobSource getTobSource( ) const;
   std::string getTobSourceString( ) const;
   bool getIsTob( ) const;
   bool getIsXtob( ) const;
   bool getIsEfex( ) const;
   bool getIsJfex( ) const;
   bool getIsGfex( ) const;
   bool getIsTopo( ) const;
   
   int getFibre( ) const;
   int getTobSeq( ) const;

private:
   TobType m_tobType;
   TobSource m_source;
   int m_id;
   int m_fibre;
   int m_tobSeq;
};

bool operator<(const L1CaloRdoFexTob& lhs, const L1CaloRdoFexTob& rhs);

#endif
