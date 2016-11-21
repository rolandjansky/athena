/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <vector>
#include <cmath>

#include "GaudiKernel/MsgStream.h"

// Configuration interface includes:
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/L1DataDef.h"

#include "TrigConfigSvc/Verifyer.h"

using namespace std;

namespace TrigConf {

   const unsigned int Verifyer::MUON_CABLE_MASK  = 0x0003ffff;
   const unsigned int Verifyer::CP1_CABLE_MASK   = 0x00ffffff;
   const unsigned int Verifyer::CP2_CABLE_MASK   = 0x00ffffff;
   const unsigned int Verifyer::JEP1_CABLE_MASK  = 0xffffffff;
   const unsigned int Verifyer::JEP2_CABLE_MASK  = 0x0000ffff;
   const unsigned int Verifyer::JEP3_CABLE_MASK  = 0x00000fff;

   /**
    * Function checking if the trigger menu can be used in the simulation. A return value of true means the
    * trigger menu failed the test, a return value of false means the trigger menu passed the test.
    */
   bool Verifyer::verify( const Menu& menu, MsgStream& log ) {

      log << MSG::DEBUG << "---> Verifying the validity of the trigger menu..." 
          << endmsg;

      if( verifyThCount( menu, log ) ) {
         log << MSG::ERROR << "Verifyer::verifyThCount() failed" << endmsg;
         return true;
      }

      //if( verifyThLayout( menu, log ) ) {
      //  log << MSG::ERROR << "Verifyer::verifyThLayout() failed" << endmsg;
      //  return true;
      //}

      log << MSG::DEBUG << "---> Trigger menu found to be valid" << endmsg;
      return false;

   }

   bool Verifyer::verifyThCount( const Menu& menu, MsgStream& log ) {

      L1DataDef def;

      log << MSG::VERBOSE << "  ---> Checking trigger threshold counts" << endmsg;

      //
      // Count all the thresholds in the configuration:
      //
      unsigned int muon_th_count = 0;
      unsigned int em_th_count = 0;
      unsigned int tau_th_count = 0;
      unsigned int emtau_th_count = 0;
      unsigned int jet_th_count = 0;
      unsigned int jetenergy_th_count = 0;
      unsigned int jb_th_count = 0;
      unsigned int jf_th_count = 0;
      unsigned int energy_th_count = 0;
      unsigned int missen_th_count = 0;
      unsigned int rndm_th_count = 0;
      unsigned int pclk_th_count = 0;
      unsigned int bgrp_th_count = 0;
      unsigned int nim_th_count = 0;
      unsigned int mbts_th_count = 0;
      unsigned int mbtssi_th_count = 0;
      unsigned int zdc_th_count = 0;
      unsigned int trt_th_count = 0;
      unsigned int bcm_th_count = 0;
      unsigned int bcmcmb_th_count = 0;
      unsigned int lucid_th_count = 0;
      unsigned int calreq_th_count = 0;
      unsigned int zb_th_count = 0;
      unsigned int xs_th_count = 0;

      vector<unsigned int> count(L1DataDef::UNDEF);
      for(auto& c: count) c=0;

      for ( TriggerThreshold* thr : menu.thresholdVector() ) {

         count[ thr->ttype() ] ++ ;
         
         if( thr->type() == def.muonType() ) {
            muon_th_count++;
         } else if( thr->type() == def.emType() ) {
            em_th_count++;
            emtau_th_count++;
         } else if( thr->type() == def.tauType() ) {
            tau_th_count++;
            emtau_th_count++;
         } else if( thr->type() == def.jetType() ) {
            jet_th_count++;
         } else if( thr->type() == def.jeType() ) {
            jetenergy_th_count++;
         } else if( thr->type() == def.jbType() ) {
            jb_th_count++;
         } else if( thr->type() == def.jfType() ) {
            jf_th_count++;
         } else if( thr->type() == def.teType() ) {
            energy_th_count++;
         } else if( thr->type() == def.xeType() ) {
            missen_th_count++;
         } else if ( thr->type() == def.rndmType()) {
            rndm_th_count ++;
         } else if ( thr->type() == def.pclkType()) {
            pclk_th_count ++;
         } else if ( thr->type() == def.bgrpType()) {
            bgrp_th_count ++;
         } else if ( thr->type() == def.nimType()) {
            nim_th_count ++;
         } else if ( thr->type() == def.mbtsType()) {
            mbts_th_count ++;
         } else if ( thr->type() == def.mbtssiType()) {
            mbtssi_th_count ++;
         } else if ( thr->type() == def.trtType()) {
            trt_th_count ++;
         } else if ( thr->type() == def.zdcType()) {
            zdc_th_count ++;
         } else if ( thr->type() == def.bcmType()) {
            bcm_th_count ++;
         } else if ( thr->type() == def.bcmcmbType()) {
            bcmcmb_th_count ++;
         } else if ( thr->type() == def.lucidType()) {
            lucid_th_count ++;
         } else if ( thr->type() == def.calreqType()) {
            calreq_th_count ++;
         } else if ( thr->type() == def.zbType()) {
            zb_th_count ++;
         } else if ( thr->type() == def.xsType()) {
            xs_th_count ++;
         }

      }

      //
      // Check if the thresholds can fit on the cables:
      //
      if( muon_th_count > def.max_MUON_Threshold_Number() ) {

         log << MSG::ERROR << "Too many MUON thresholds defined in the configuration" << endmsg;
         return true;

      } else if( muon_th_count != def.max_MUON_Threshold_Number() ) {

         log << MSG::WARNING << "    ---> Less than 6 MUON thresholds defined, TGC simulation will override!" << endmsg;

      } else {

         log << MSG::VERBOSE << "    ---> MUON threshold count passed the check" << endmsg;

      }

      if( em_th_count > def.max_EM_Threshold_Number() ) {

         log << MSG::ERROR << "Too many EM thresholds defined in the configuration" << endmsg;
         return true;

      } else {

         log << MSG::VERBOSE << "    ---> EM threshold count passed the check" << endmsg;

      }

      if( tau_th_count > def.max_TAU_Threshold_Number() ) {

         log << MSG::ERROR << "Too many TAU thresholds defined in the configuration" << endmsg;
         return true;

      } else {

         log << MSG::VERBOSE << "    ---> TAU threshold count passed the check" << endmsg;

      }

      if( jet_th_count > def.max_JET_Threshold_Number() ) {

         log << MSG::ERROR << "Too many JET thresholds defined in the configuration" << endmsg;
         return true;

      } else {

         log << MSG::VERBOSE << "    ---> JET threshold count passed the check" << endmsg;

      }

      if( jetenergy_th_count > def.max_JE_Threshold_Number() ) {

         log << MSG::ERROR << "Too many JE thresholds defined in the configuration" << endmsg;
         return true;

      } else {

         log << MSG::VERBOSE << "    ---> JE threshold count passed the check" << endmsg;

      }

      if( jb_th_count > def.max_JB_Threshold_Number() ) {

         log << MSG::ERROR << "Too many JB thresholds defined in the configuration" << endmsg;
         return true;

      } else {

         log << MSG::VERBOSE << "    ---> JB threshold count passed the check" << endmsg;

      }

      if( jf_th_count > def.max_JF_Threshold_Number() ) {

         log << MSG::ERROR << "Too many JF thresholds defined in the configuration" << endmsg;
         return true;

      } else {

         log << MSG::VERBOSE << "    ---> JF threshold count passed the check" << endmsg;

      }

      if( energy_th_count > def.max_TE_Threshold_Number() ) {

         log << MSG::ERROR << "Too many TE thresholds defined in the configuration" << endmsg;
         return true;

      } else {

         log << MSG::VERBOSE << "    ---> TE threshold count passed the check" << endmsg;

      }

      if( missen_th_count > def.max_XE_Threshold_Number() ) {
         log << MSG::ERROR << 
            "Too many XE thresholds defined in the configuration" << endmsg;
         return true;
      } else {
         log << MSG::VERBOSE << 
            "    ---> XE threshold count passed the check" << endmsg;
      }

      if(rndm_th_count > def.max_RNDM_Threshold_Number()) {
         log << MSG::ERROR << 
            "Too many RNDM thresholds defined in the configuration" << endmsg;
         return true;
      } else {
         log << MSG::VERBOSE << 
            "    ---> RNDM threshold count passed the check" << endmsg;
      }
      if(pclk_th_count > def.max_PCLK_Threshold_Number()) {
         log << MSG::ERROR << 
            "Too many PCLK thresholds defined in the configuration" << endmsg;
         return true;
      } else {
         log << MSG::VERBOSE << 
            "    ---> PCLK threshold count passed the check" << endmsg;
      }
      if(bgrp_th_count > def.max_BGRP_Threshold_Number()) {
         log << MSG::ERROR << 
            "Too many BGRP thresholds defined in the configuration" << endmsg;
         return true;
      } else {
         log << MSG::VERBOSE << 
            "    ---> BGRP threshold count passed the check" << endmsg;
      }


      if( count[L1DataDef::TOPO] > L1DataDef::typeConfig( L1DataDef::TOPO ).max ) {
         log << MSG::ERROR << 
            "Too many TOPO thresholds defined in the configuration" << endmsg;
         return true;
      } else {
         log << MSG::VERBOSE << 
            "TOPO threshold count passed the check" << endmsg;
      }





      //===========================================================================
      // NOW INTERNAL TRIGGERS ARE NOT DEFINED IN THE THRESHOLD LIST
      // THEY ONLY APPEAR AS A STRING INSIDE ITERM LOGIC
      //===========================================================================
      //     if (rndm_th_count != def.max_RNDM_Threshold_Number()) {
      //       log << MSG::WARNING << 
      // 	"Only " << rndm_th_count << " RNDM thresholds are defined! " <<
      // 	" maximum allowed is " << def.max_RNDM_Threshold_Number() << endmsg;
      //     }
      //     if (pclk_th_count != def.max_PCLK_Threshold_Number()) {
      //       log << MSG::WARNING << 
      // 	"Only " << pclk_th_count << " PCLK thresholds are defined! " <<
      // 	" maximum allowed is " << def.max_PCLK_Threshold_Number() << endmsg;
      //     }
      //     if (bgrp_th_count != def.max_BGRP_Threshold_Number()) {
      //       log << MSG::WARNING << 
      // 	"Only " << bgrp_th_count << " BGRP thresholds are defined! " <<
      // 	" maximum allowed is " << def.max_BGRP_Threshold_Number() << endmsg;
      //     }
      //===========================================================================

      log << MSG::VERBOSE << "  ---> Trigger threshold counts passed the tests" << endmsg;
      return false;

   }

   bool Verifyer::verifyThLayout( const Menu& menu, MsgStream& log ) {

      L1DataDef def;
      //
      // Now check that the layout of the thresholds on the cables is possible or not:
      //
      log << MSG::VERBOSE << "  ---> Checking layout of trigger thresholds onto cables" << endmsg;

      unsigned int muon_cable = !MUON_CABLE_MASK;
      unsigned int cp1_cable  = !CP1_CABLE_MASK;
      unsigned int cp2_cable  = !CP2_CABLE_MASK;
      unsigned int jep1_cable = !JEP1_CABLE_MASK;
      unsigned int jep2_cable = !JEP2_CABLE_MASK;
      unsigned int jep3_cable = !JEP3_CABLE_MASK;

      for ( vector< TriggerThreshold* >::const_iterator it = menu.thresholdVector().begin();
            it != menu.thresholdVector().end(); ++it ) {
         if ( (*it)->type()==L1DataDef::rndmType() ||
              (*it)->type()==L1DataDef::pclkType() ||
              (*it)->type()==L1DataDef::bgrpType()) {
            // Internal trigger, so there's no input cable
            continue;
         }

         if( ( *it )->cableName() == "MU" ) {

            if( ( *it )->type() != def.muonType() ) {
               log << MSG::ERROR << "    ---> A(n) " << ( *it )->type() << " threshold can't be put on the MU cable!" << endmsg;
               return true;
            }
            if( muon_cable & createMask( ( *it )->cableStart(), ( *it )->cableEnd() ) ) {
               log << MSG::ERROR << "    ---> The bits from " << ( *it )->cableStart() << " to " << ( *it )->cableEnd() << " on cable MU "
                   << "are already partly or totally taken" << endmsg;
               log << MSG::ERROR << "         Threshold " << ( *it )->name() << " can't be placed" << endmsg;
               return true;
            }
            muon_cable |= createMask( ( *it )->cableStart(), ( *it )->cableEnd() );
            log << MSG::VERBOSE << "    ---> Trigger threshold " << ( *it )->name() << " on cable MU verifyed" << endmsg;


         } else if( ( *it )->cableName() == "CP1" ) {

            if( ( *it )->type() != def.emType() ) {
               log << MSG::ERROR << "    ---> A(n) " << ( *it )->type() << " threshold can't be put on the CP1 cable!" << endmsg;
               return true;
            }
            if( cp1_cable & createMask( ( *it )->cableStart(), ( *it )->cableEnd() ) ) {
               log << MSG::ERROR << "    ---> The bits from " << ( *it )->cableStart() << " to " << ( *it )->cableEnd() << " on cable CP1 "
                   << "are already partly or totally taken" << endmsg;
               log << MSG::ERROR << "         Threshold " << ( *it )->name() << " can't be placed" << endmsg;
               return true;
            }
            cp1_cable |= createMask( ( *it )->cableStart(), ( *it )->cableEnd() );
            log << MSG::VERBOSE << "    ---> Trigger threshold " << ( *it )->name() << " on cable CP1 verifyed" << endmsg;


         } else if( ( *it )->cableName() == "CP2" ) {

            if( ! ( ( ( *it )->type() == def.emType() ) || ( ( *it )->type() == def.tauType() ) ) ) {
               log << MSG::ERROR << "  ---> A(n) " << ( *it )->type() << " threshold can't be put on the CP2 cable!" << endmsg;
               return true;
            }
            if( cp2_cable & createMask( ( *it )->cableStart(), ( *it )->cableEnd() ) ) {
               log << MSG::ERROR << "    ---> The bits from " << ( *it )->cableStart() << " to " << ( *it )->cableEnd() << " on cable CP2 "
                   << "are already partly or totally taken" << endmsg;
               log << MSG::ERROR << "         Threshold " << ( *it )->name() << " can't be placed" << endmsg;
               return true;
            }
            cp2_cable |= createMask( ( *it )->cableStart(), ( *it )->cableEnd() );
            log << MSG::VERBOSE << "    ---> Trigger threshold " << ( *it )->name() << " on cable CP2 verifyed" << endmsg;

         } else if( ( *it )->cableName() == "JEP1" ) {

            if( ! ( ( ( *it )->type() == def.jetType() ) || ( ( *it )->type() == def.jeType() ) ) ) {
               log << MSG::ERROR << "    ---> A(n) " << ( *it )->type() << " threshold can't be put on the JEP1 cable!" << endmsg;
               return true;
            }
            if( jep1_cable & createMask( ( *it )->cableStart(), ( *it )->cableEnd() ) ) {
               log << MSG::ERROR << "    ---> The bits from " << ( *it )->cableStart() << " to " << ( *it )->cableEnd() << " on cable JEP1 "
                   << "are already partly or totally taken" << endmsg;
               log << MSG::ERROR << "         Threshold " << ( *it )->name() << " can't be placed" << endmsg;
               return true;
            }
            jep1_cable |= createMask( ( *it )->cableStart(), ( *it )->cableEnd() );
            log << MSG::VERBOSE << "    ---> Trigger threshold " << ( *it )->name() << " on cable JEP1 verifyed" << endmsg;

         } else if( ( *it )->cableName() == "JEP2" ) {

            if( ! ( ( *it )->type()==def.jbType() || (*it)->type()==def.jfType()) ) {
               log << MSG::ERROR << "    ---> A(n) " << ( *it )->type() << " threshold can't be put on the JEP2 cable!" << endmsg;
               return true;
            }
            if( jep2_cable & createMask( ( *it )->cableStart(), ( *it )->cableEnd() ) ) {
               log << MSG::ERROR << "    ---> The bits from " << ( *it )->cableStart() << " to " << ( *it )->cableEnd() << " on cable JEP2 "
                   << "are already partly or totally taken" << endmsg;
               log << MSG::ERROR << "         Threshold " << ( *it )->name() << " can't be placed" << endmsg;
               return true;
            }
            jep2_cable |= createMask( ( *it )->cableStart(), ( *it )->cableEnd() );
            log << MSG::VERBOSE << "    ---> Trigger threshold " << ( *it )->name() << " on cable JEP2 verifyed" << endmsg;

         } else if( ( *it )->cableName() == "JEP3" ) {

            if( ! ( ( ( *it )->type() == def.teType() ) || ( ( *it )->type() == def.xeType() ) ) ) {
               log << MSG::ERROR << "    ---> A(n) " << ( *it )->type() << " threshold can't be put on the JEP3 cable!" << endmsg;
               return true;
            }


            if( jep3_cable & createMask( ( *it )->cableStart(), ( *it )->cableEnd() ) ) {
               log << MSG::ERROR << "    ---> The bits from " << ( *it )->cableStart() << " to " << ( *it )->cableEnd() << " on cable JEP3 "
                   << "are already partly or totally taken" << endmsg;
               log << MSG::ERROR << "         Threshold " << ( *it )->name() << " can't be placed" << endmsg;
               return true;
            }
            jep3_cable |= createMask( ( *it )->cableStart(), ( *it )->cableEnd() );
            log << MSG::VERBOSE << "    ---> Trigger threshold " << ( *it )->name() << " on cable JEP3 verifyed" << endmsg;

         } else if( ( *it )->cableName() == "NIM" ) {

            if( ! ( (*it)->type() == def.nimType() ||
                    (*it)->type()==def.mbtsType() ||
                    (*it)->type()==def.mbtssiType() ||
                    (*it)->type()==def.zdcType() ||
                    (*it)->type()==def.trtType() ||
                    (*it)->type()==def.bcmType() ||
                    (*it)->type()==def.bcmcmbType() || 
                    (*it)->type()==def.lucidType())) {
               log << MSG::ERROR << "    ---> A(n) " << (*it)->type() 
                   << " threshold can't be put on the NIM cable!" << endmsg; // This check seems tied to an outdated cabling...
               return true;
            }
            log << MSG::VERBOSE << "    ---> Trigger threshold " 
                << ( *it )->name() << " on cable NIM verifyed" << endmsg;

         } else if( ( *it )->cableName() == "MBTS" ) {
         } else {
	
            log << MSG::ERROR << "     ---> There is no cable named: " << ( *it )->cableName() << endmsg;
            log << MSG::ERROR << "          Threshold " << ( *it )->name() << " can't be placed" << endmsg;
            return true;

         }

      }

      log << MSG::VERBOSE << "  ---> Trigger thresholds layout passed the tests" << endmsg;
      return false;

   }

   unsigned int Verifyer::createMask( int startbit, int stopbit ) {

      unsigned int result = 0;

      for( int i = 0; i < ( abs( startbit - stopbit ) + 1 ); ++i ) {

         result <<= 1;
         result |= 0x1;

      }

      result <<= min( startbit, stopbit );

      return result;

   }

   template< class T > T Verifyer::max( T i, T j ) {

      return i > j ? i : j;

   }

   template< class T > T Verifyer::min( T i, T j ) {

      return i > j ? j : i;

   }

} // namespace TrigConf
