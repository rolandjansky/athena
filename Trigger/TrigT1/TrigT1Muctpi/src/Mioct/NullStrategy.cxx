/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: NullStrategy.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// Local include(s):
#include "NullStrategy.h"
#include "MioctSectorSet.h"
#include "MultiplicityLogic.h"

//***********************************************************************
//
//       Version : $Revision: 362102 $
//
//   Description :
//
//        Author : $Author: krasznaa $
//          Date : $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
//
//
//
//***********************************************************************

namespace LVL1MUCTPI {

   NullStrategy::NullStrategy( const MultiplicityLogic* logic )
      : OverlapStrategy( "No-Overlap-Consideration" ),
        m_errorLogger("NullStrategy"), m_multiplicityLogic( logic ) {

   }

   NullStrategy::~NullStrategy() {

   }

   PtMultiplicitySet
   NullStrategy::calculateMultiplicity( const MioctSectorSet& sectorData,
                                        const MioctID& ) const {

      //
      // Blindly add the multiplicity of each candidate to the total multiplicity
      // of this octant:
      //
      PtMultiplicitySet result;
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Barrel1.getPtCand1(),
                                                            sectorData.Barrel1.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Barrel1.getPtCand2(),
                                                            sectorData.Barrel1.getSectorID(),
                                                            false );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Barrel2.getPtCand1(),
                                                            sectorData.Barrel2.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Barrel2.getPtCand2(),
                                                            sectorData.Barrel2.getSectorID(),
                                                            false );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Barrel3.getPtCand1(),
                                                            sectorData.Barrel3.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Barrel3.getPtCand2(),
                                                            sectorData.Barrel3.getSectorID(),
                                                            false );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Barrel4.getPtCand1(),
                                                            sectorData.Barrel4.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Barrel4.getPtCand2(),
                                                            sectorData.Barrel4.getSectorID(),
                                                            false );

      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Endcap1.getPtCand1(),
                                                            sectorData.Endcap1.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Endcap1.getPtCand2(),
                                                            sectorData.Endcap1.getSectorID(),
                                                            false );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Endcap2.getPtCand1(),
                                                            sectorData.Endcap2.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Endcap2.getPtCand2(),
                                                            sectorData.Endcap2.getSectorID(),
                                                            false );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Endcap3.getPtCand1(),
                                                            sectorData.Endcap3.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Endcap3.getPtCand2(),
                                                            sectorData.Endcap3.getSectorID(),
                                                            false );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Endcap4.getPtCand1(),
                                                            sectorData.Endcap4.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Endcap4.getPtCand2(),
                                                            sectorData.Endcap4.getSectorID(),
                                                            false );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Endcap5.getPtCand1(),
                                                            sectorData.Endcap5.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Endcap5.getPtCand2(),
                                                            sectorData.Endcap5.getSectorID(),
                                                            false );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Endcap6.getPtCand1(),
                                                            sectorData.Endcap6.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Endcap6.getPtCand2(),
                                                            sectorData.Endcap6.getSectorID(),
                                                            false );

      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Forward1.getPtCand1(),
                                                            sectorData.Forward1.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Forward1.getPtCand2(),
                                                            sectorData.Forward1.getSectorID(),
                                                            false );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Forward2.getPtCand1(),
                                                            sectorData.Forward2.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Forward2.getPtCand2(),
                                                            sectorData.Forward2.getSectorID(),
                                                            false );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Forward3.getPtCand1(),
                                                            sectorData.Forward3.getSectorID(),
                                                            true );
      result += m_multiplicityLogic->calculateMultiplicity( sectorData.Forward3.getPtCand2(),
                                                            sectorData.Forward3.getSectorID(),
                                                            false );

      return result;
   }

} // namespace LVL1MUCTPI
