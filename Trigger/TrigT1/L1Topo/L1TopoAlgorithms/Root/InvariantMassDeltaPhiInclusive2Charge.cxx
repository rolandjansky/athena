/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * InvariantMassDeltaPhiInclusive2Charge.cxx
 * Based on V Sorin 2014 implementation of InvariantMassInclusive2. For questions contact atlas-trig-l1topo-algcom@cern.ch.
 *
 * @brief algorithm calculates the sqr of the INVMASS between two lists and applies invmass criteria. For pairs passing the INVMASS cut a further requirement based on DeltaPhi
 * is applied, addressing ATR-19377. Following ATR-19593, an additional cut in the muon charge is required.
 *
**********************************/
//  TO DO size of the input list to be possbly refined 

#include "L1TopoAlgorithms/InvariantMassDeltaPhiInclusive2Charge.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
// Bitwise implementation utils
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
#include "L1TopoSimulationUtils/Kinematics.h"
//
#include "TH1F.h"
#include "TH2F.h"

#include <cmath>

REGISTER_ALG_TCS(InvariantMassDeltaPhiInclusive2Charge)


TCS::InvariantMassDeltaPhiInclusive2Charge::InvariantMassDeltaPhiInclusive2Charge(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 9);
   defineParameter("InputWidth2", 9);
   defineParameter("MaxTob1", 0);
   defineParameter("MaxTob2", 0);
   defineParameter("NumResultBits", 6);
   defineParameter("MinMSqr",   0, 0);
   defineParameter("MaxMSqr", 999, 0);
   defineParameter("MinMSqr",   0, 1);
   defineParameter("MaxMSqr", 999, 1);
   defineParameter("MinMSqr",   0, 2);
   defineParameter("MaxMSqr", 999, 2);
   defineParameter("MinMSqr",   0, 3);
   defineParameter("MaxMSqr", 999, 3);
   defineParameter("MinMSqr",   0, 4);
   defineParameter("MaxMSqr", 999, 4);
   defineParameter("MinMSqr",   0, 5);
   defineParameter("MaxMSqr", 999, 5);
   defineParameter("MinET1",0,0);
   defineParameter("MinET2",0,0);
   defineParameter("MinET1",0,1);
   defineParameter("MinET2",0,1);
   defineParameter("MinET1",0,2);
   defineParameter("MinET2",0,2);
   defineParameter("MinET1",0,3);
   defineParameter("MinET2",0,3);
   defineParameter("MinET1",0,4);
   defineParameter("MinET2",0,4);
   defineParameter("MinET1",0,5);
   defineParameter("MinET2",0,5);
   defineParameter("ApplyEtaCut", 0);
   defineParameter("MinEta1",  0);
   defineParameter("MaxEta1", 31);
   defineParameter("MinEta2",  0);
   defineParameter("MaxEta2", 31);
   defineParameter("MinDeltaPhi",  0, 0);
   defineParameter("MaxDeltaPhi", 31, 0);
   defineParameter("MinDeltaPhi",  0, 1);
   defineParameter("MaxDeltaPhi", 31, 1);
   defineParameter("MinDeltaPhi",  0, 2);
   defineParameter("MaxDeltaPhi", 31, 2);
   defineParameter("MinDeltaPhi",  0, 3);
   defineParameter("MaxDeltaPhi", 31, 3);
   defineParameter("MinDeltaPhi",  0, 4);
   defineParameter("MaxDeltaPhi", 31, 4);
   defineParameter("MinDeltaPhi",  0, 5);
   defineParameter("MaxDeltaPhi", 31, 5);
   //does this need to change? 
   setNumberOutputBits(6);
}

TCS::InvariantMassDeltaPhiInclusive2Charge::~InvariantMassDeltaPhiInclusive2Charge(){}


TCS::StatusCode
TCS::InvariantMassDeltaPhiInclusive2Charge::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_InvMassMin[i] = parameter("MinMSqr", i).value();
      p_InvMassMax[i] = parameter("MaxMSqr", i).value();
      p_DeltaPhiMin[i] = parameter("MinDeltaPhi", i).value();
      p_DeltaPhiMax[i] = parameter("MaxDeltaPhi", i).value();
      p_MinET1[i] = parameter("MinET1",i).value();
      p_MinET2[i] = parameter("MinET2",i).value();
   }

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
    TRG_MSG_INFO("InvMassMin   : " << p_InvMassMin[i]);
    TRG_MSG_INFO("InvMassMax   : " << p_InvMassMax[i]);    
    TRG_MSG_INFO("DeltaPhiMin   : " << p_DeltaPhiMin[i]);
    TRG_MSG_INFO("DeltaPhiMax   : " << p_DeltaPhiMax[i]);
    TRG_MSG_INFO("MinET1          : " << p_MinET1[i]);
    TRG_MSG_INFO("MinET2          : " << p_MinET2[i]);
   }

   p_ApplyEtaCut = parameter("ApplyEtaCut").value();
   p_MinEta1     = parameter("MinEta1"    ).value();
   p_MaxEta1     = parameter("MaxEta1"    ).value();
   p_MinEta2     = parameter("MinEta2"    ).value();
   p_MaxEta2     = parameter("MaxEta2"    ).value();
   TRG_MSG_INFO("ApplyEtaCut : "<<p_ApplyEtaCut );
   TRG_MSG_INFO("MinEta1     : "<<p_MinEta1     );
   TRG_MSG_INFO("MaxEta1     : "<<p_MaxEta1     );
   TRG_MSG_INFO("MinEta2     : "<<p_MinEta2     );
   TRG_MSG_INFO("MaxEta2     : "<<p_MaxEta2     );

   TRG_MSG_INFO("number output : " << numberOutputBits());

   // book histograms
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       std::string hname_accept = "hInvariantMassDeltaPhiInclusive2Charge_accept_bit"+std::to_string(static_cast<int>(i));
       std::string hname_reject = "hInvariantMassDeltaPhiInclusive2Charge_reject_bit"+std::to_string(static_cast<int>(i));
       std::string hname_acceptCharge = "hInvariantMassDeltaPhiInclusive2Charge_acceptCharge_bit"+std::to_string(static_cast<int>(i));
       std::string hname_rejectCharge = "hInvariantMassDeltaPhiInclusive2Charge_rejectCharge_bit"+std::to_string(static_cast<int>(i));
       std::string hname_undefCharge  = "hInvariantMassDeltaPhiInclusive2Charge_undefCharge_bit"+std::to_string(static_cast<int>(i));
       // mass
       bookHist(m_histAcceptM, hname_accept, "INVM vs DPHI", 100, sqrt(p_InvMassMin[i]), sqrt(p_InvMassMax[i]), 100, p_DeltaPhiMin[i], p_DeltaPhiMax[i]);
       bookHist(m_histRejectM, hname_reject, "INVM vs DPHI", 100, sqrt(p_InvMassMin[i]), sqrt(p_InvMassMax[i]), 100, p_DeltaPhiMin[i], p_DeltaPhiMax[i]);
       // eta2 vs. eta1
       bookHist(m_histAcceptEta1Eta2, hname_accept, "ETA vs ETA", 100, p_MinEta1, p_MaxEta1, 100, p_MinEta2, p_MaxEta2);
       bookHist(m_histRejectEta1Eta2, hname_reject, "ETA vs ETA", 100, p_MinEta1, p_MaxEta1, 100, p_MinEta2, p_MaxEta2);
       // eta2 vs. eta1 (charge cut)
       // accept = muons have opposite charge, reject = muons do not have opposite charge, undef = 1 or 2 muons have undefined charge
       bookHist(m_histAcceptChargeEta1Eta2, hname_acceptCharge, "ETA vs ETA", 100, p_MinEta1, p_MaxEta1, 100, p_MinEta2, p_MaxEta2);
       bookHist(m_histRejectChargeEta1Eta2, hname_rejectCharge, "ETA vs ETA", 100, p_MinEta1, p_MaxEta1, 100, p_MinEta2, p_MaxEta2);
       bookHist(m_histUndefChargeEta1Eta2,  hname_undefCharge,  "ETA vs ETA", 100, p_MinEta1, p_MaxEta1, 100, p_MinEta2, p_MaxEta2);
   }
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::InvariantMassDeltaPhiInclusive2Charge::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

   if( input.size() == 2) {
      for( TOBArray::const_iterator tob1 = input[0]->begin();
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {


            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin();
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {
                // Inv Mass calculation
                unsigned int invmass2 = TSU::Kinematics::calcInvMassBW( *tob1, *tob2 );
		// test DeltaPhiMin, DeltaPhiMax                                       
		unsigned int deltaPhi = TSU::Kinematics::calcDeltaPhiBW( *tob1, *tob2 );
                const int eta1 = (*tob1)->eta();
                const int eta2 = (*tob2)->eta();
                const unsigned int aeta1 = std::abs(eta1);
                const unsigned int aeta2 = std::abs(eta2);
                std::string sector1 = (*tob1)->sectorName();
                std::string sector2 = (*tob2)->sectorName();
                int charge1 = (*tob1)->charge();
                int charge2 = (*tob2)->charge();
                // Charge cut ( for TGC muons: 0=negative, 1=positive, as described at ATR-22621 )
                std::string totalCharge = "undef";
                if ( sector1 != "" && sector2 != "" && sector1.at(0) != 'B' && sector2.at(0) != 'B' ) {
                   if ( charge1 + charge2 == 1 ) { totalCharge = "accept"; }
                   else { totalCharge = "reject"; }
                }
		for(unsigned int i=0; i<numberOutputBits(); ++i) {
		   bool accept = false;
                   if( static_cast<parType_t>((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                   if( static_cast<parType_t>((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                   if(p_ApplyEtaCut &&
                      ((aeta1 < p_MinEta1 || aeta1 > p_MaxEta1 ) ||
                       (aeta2 < p_MinEta2 || aeta2 > p_MaxEta2 ) ))  continue;
                   accept = invmass2 >= p_InvMassMin[i] && invmass2 <= p_InvMassMax[i] && deltaPhi >= p_DeltaPhiMin[i] && deltaPhi <= p_DeltaPhiMax[i] && totalCharge != "reject";
                   const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
                   const bool fillReject = fillHistos() and not fillAccept;
                   const bool alreadyFilled = decision.bit(i);
                   if( accept ) {
                       decision.setBit(i, true);
                       output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                   }
                   if(fillAccept and not alreadyFilled) {
		       fillHist2D(m_histAcceptM[i],sqrt(static_cast<float>(invmass2)),static_cast<float>(deltaPhi));
		       fillHist2D(m_histAcceptEta1Eta2[i],eta1, eta2);
                   } else if(fillReject) {
		       fillHist2D(m_histRejectM[i],sqrt(static_cast<float>(invmass2)),static_cast<float>(deltaPhi));
		       fillHist2D(m_histRejectEta1Eta2[i],eta1, eta2);
                   }
                   if(fillHistos() and totalCharge == "accept") {
                       fillHist2D(m_histAcceptChargeEta1Eta2[i], eta1, eta2);
                   } else if (fillHistos() and totalCharge == "reject") {
                       fillHist2D(m_histRejectChargeEta1Eta2[i], eta1, eta2);
                   } else if (fillHistos() and totalCharge == "undef") {
                       fillHist2D(m_histUndefChargeEta1Eta2[i], eta1, eta2);
                   }
                   TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " invmass2 = " << invmass2);
               }
            }
         }
   } else {

      TCS_EXCEPTION("InvariantMassDeltaPhiInclusive2Charge alg must have  2 inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;

}

TCS::StatusCode
TCS::InvariantMassDeltaPhiInclusive2Charge::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{


   if( input.size() == 2) {
      for( TOBArray::const_iterator tob1 = input[0]->begin();
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {
            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin();
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {
                // Inv Mass calculation
                unsigned int invmass2 = TSU::Kinematics::calcInvMass( *tob1, *tob2 );
		// test DeltaPhiMin, DeltaPhiMax                                     
		unsigned int deltaPhi = TSU::Kinematics::calcDeltaPhi( *tob1, *tob2 );
                const int eta1 = (*tob1)->eta();
                const int eta2 = (*tob2)->eta();
                const unsigned int aeta1 = std::abs(eta1);
                const unsigned int aeta2 = std::abs(eta2);
                std::string sector1 = (*tob1)->sectorName();
                std::string sector2 = (*tob2)->sectorName();
                int charge1 = (*tob1)->charge();
                int charge2 = (*tob2)->charge();
                // Charge cut ( for TGC muons: 0=negative, 1=positive, as described at ATR-22621 )
                std::string totalCharge = "undef";
                if ( sector1 != "" && sector2 != "" && sector1.at(0) != 'B' && sector2.at(0) != 'B' ) {
                   if ( charge1 + charge2 == 1 ) { totalCharge = "accept"; }
                   else { totalCharge = "reject"; }
                }
                for(unsigned int i=0; i<numberOutputBits(); ++i) {
                   if( static_cast<parType_t>((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                   if( static_cast<parType_t>((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                   if(p_ApplyEtaCut &&
                      ((aeta1 < p_MinEta1 || aeta1 > p_MaxEta1 ) ||
                       (aeta2 < p_MinEta2 || aeta2 > p_MaxEta2 ) )) continue;
                   bool accept = invmass2 >= p_InvMassMin[i] && invmass2 <= p_InvMassMax[i] && deltaPhi >= p_DeltaPhiMin[i] && deltaPhi <= p_DeltaPhiMax[i] && totalCharge != "reject";
                   const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
                   const bool fillReject = fillHistos() and not fillAccept;
                   const bool alreadyFilled = decision.bit(i);
                   if( accept ) {
                       decision.setBit(i, true);
                       output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
                   }
                   if(fillAccept and not alreadyFilled) {
		       fillHist2D(m_histAcceptM[i],sqrt(static_cast<float>(invmass2)),static_cast<float>(deltaPhi));
		       fillHist2D(m_histAcceptEta1Eta2[i],eta1, eta2);
                   } else if(fillReject) {
		       fillHist2D(m_histRejectM[i],sqrt(static_cast<float>(invmass2)),static_cast<float>(deltaPhi));
		       fillHist2D(m_histRejectEta1Eta2[i],eta1, eta2);
                   }
                   if(fillHistos() and totalCharge == "accept") {
                       fillHist2D(m_histAcceptChargeEta1Eta2[i], eta1, eta2);
                   } else if (fillHistos() and totalCharge == "reject") {
                       fillHist2D(m_histRejectChargeEta1Eta2[i], eta1, eta2);
                   } else if (fillHistos() and totalCharge == "undef") {
                       fillHist2D(m_histUndefChargeEta1Eta2[i], eta1, eta2);
                   }
                  TRG_MSG_DEBUG("Decision " << i << ": " << (accept ?"pass":"fail") << " invmass2 = " << invmass2);
               }
            }
         }
   } else {
      TCS_EXCEPTION("InvariantMassDeltaPhiInclusive2Charge alg must have  2 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
