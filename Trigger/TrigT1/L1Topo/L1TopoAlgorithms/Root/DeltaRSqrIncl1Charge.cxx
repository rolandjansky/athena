/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DeltaRSqrIncl1Charge.cxx
 * Created by Paula Martinez based on Joerg Stelzer / V Sorin.
 *
 * @brief algorithm calculates the R2-distance between objects in one  list and applies dR criteria
 * Events containing a pair of TGC muons with same charge are rejected
 *
 * @param NumberLeading
**********************************/

#include <cmath>
#include "TH1F.h"
#include "TH2F.h"

#include "L1TopoAlgorithms/DeltaRSqrIncl1Charge.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/Kinematics.h"

REGISTER_ALG_TCS(DeltaRSqrIncl1Charge)


// not the best solution but we will move to athena where this comes for free
#define LOG std::cout << "TCS::DeltaRSqrIncl1Charge:     "

TCS::DeltaRSqrIncl1Charge::DeltaRSqrIncl1Charge(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 9);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits", 3);
   defineParameter("RequireOneBarrel", 0);
   defineParameter("MinET1",1);
   defineParameter("MinET2",1);
   defineParameter("DeltaRMin",  0, 0);
   defineParameter("DeltaRMax",  0, 0);
   defineParameter("DeltaRMin",  0, 1);
   defineParameter("DeltaRMax",  0, 1);
   defineParameter("DeltaRMin",  0, 2);
   defineParameter("DeltaRMax",  0, 2);
   setNumberOutputBits(3);
}

TCS::DeltaRSqrIncl1Charge::~DeltaRSqrIncl1Charge(){}


TCS::StatusCode
TCS::DeltaRSqrIncl1Charge::initialize() {
   if(parameter("MaxTob").value() > 0) {
    p_NumberLeading1 = parameter("MaxTob").value();
    p_NumberLeading2 = parameter("MaxTob").value();
   } else {
    p_NumberLeading1 = parameter("InputWidth").value();
    p_NumberLeading2 = parameter("InputWidth").value();
   }
   p_OneBarrel = parameter("RequireOneBarrel").value();

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_DeltaRMin[i] = parameter("DeltaRMin", i).value();
      p_DeltaRMax[i] = parameter("DeltaRMax", i).value();
   }
   p_MinET1 = parameter("MinET1").value();
   p_MinET2 = parameter("MinET2").value();

   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   TRG_MSG_INFO("NumberLeading2 : " << p_NumberLeading2);
   TRG_MSG_INFO("RequireOneBarrel : " << p_OneBarrel);

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
    TRG_MSG_INFO("DeltaRMin   : " << p_DeltaRMin[i]);
    TRG_MSG_INFO("DeltaRMax   : " << p_DeltaRMax[i]);
   }
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);

   TRG_MSG_INFO("number output : " << numberOutputBits());
    
   // book histograms
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       std::string hname_accept = "hDeltaRSqrIncl1Charge_accept_bit"+std::to_string(static_cast<int>(i));
       std::string hname_reject = "hDeltaRSqrIncl1Charge_reject_bit"+std::to_string(static_cast<int>(i));
       // dR
       bookHist(m_histAccept, hname_accept, "DR", 100, std::sqrt(p_DeltaRMin[i]), std::sqrt(p_DeltaRMax[i]));
       bookHist(m_histReject, hname_reject, "DR", 100, std::sqrt(p_DeltaRMin[i]), std::sqrt(p_DeltaRMax[i]));
       // eta2 vs. eta1
       bookHist(m_histAcceptEta1Eta2, hname_accept, "ETA vs ETA", 100, -70, 70, 100, -70, 70);
       bookHist(m_histRejectEta1Eta2, hname_reject, "ETA vs ETA", 100, -70, 70, 100, -70, 70);
   }
   
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DeltaRSqrIncl1Charge::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

   if(input.size() == 1) {
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1) 
          {
              if( parType_t((*tob1)->Et()) <= std::min(p_MinET1,p_MinET2)) continue; // ET cut
              TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
              for( ;
                   tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading2;
                   ++tob2) {
                  if( parType_t((*tob2)->Et()) <= std::min(p_MinET1,p_MinET2)) continue; // ET cut
                  if( (parType_t((*tob1)->Et()) <= std::max(p_MinET1,p_MinET2)) && (parType_t((*tob2)->Et()) <= std::max(p_MinET1,p_MinET2))) continue;
                  // OneBarrel
                  if (p_OneBarrel && parType_t(std::abs((*tob1)->eta())) > 10 && parType_t(std::abs((*tob2)->eta())) > 10 ) continue;
                  // DeltaR2 cuts
                  unsigned int deltaR2 = TSU::Kinematics::calcDeltaR2BW( *tob1, *tob2 );
                  // Charge cut ( for TGC muons: 0=negative, 1=positive, as described at ATR-22621 )
                  // Check the definition of sectorName at MuCTPIL1TopoCandidate.h (for TGC muons: sectorName.at(0)=E or F, for RPC muons: sectorName.at(0)=B)
                  // If no muon sectorName information is available, sectorName = ""
                  std::string sector1 = (*tob1)->sectorName();
                  std::string sector2 = (*tob2)->sectorName();
                  int charge1 = (*tob1)->charge();
                  int charge2 = (*tob2)->charge();
                  int totalCharge = charge1 + charge2;
                  bool acceptCharge = true;
                  if ( sector1 != "" && sector2 != "" && sector1.at(0) != 'B' && sector2.at(0) != 'B' ) {
                     if ( totalCharge == 0 or totalCharge == 2 ) { acceptCharge = false; }
                  }
                  for(unsigned int i=0; i<numberOutputBits(); ++i) {
		    bool accept = false;
		    accept = deltaR2 >= p_DeltaRMin[i] && deltaR2 <= p_DeltaRMax[i] && acceptCharge;
		    const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		    const bool fillReject = fillHistos() and not fillAccept;
		    const bool alreadyFilled = decision.bit(i);
		    if( accept ) {
		      decision.setBit(i, true);  
		      output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
		    }
		    if(fillAccept and not alreadyFilled) {
		      fillHist1D(m_histAccept[i],std::sqrt(static_cast<float>(deltaR2)));
                      fillHist2D(m_histAcceptEta1Eta2[i],(*tob1)->eta(),(*tob2)->eta());
		    } else if(fillReject) {
		      fillHist1D(m_histReject[i],std::sqrt(static_cast<float>(deltaR2)));
                      fillHist2D(m_histRejectEta1Eta2[i],(*tob1)->eta(),(*tob2)->eta());
		    }
		    TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR2 = " << deltaR2);
                  }
              }
          }
   } else {
      TCS_EXCEPTION("DeltaRSqrIncl1Charge alg must have either 1 input, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DeltaRSqrIncl1Charge::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{
    if(input.size() == 1) {
        for( TOBArray::const_iterator tob1 = input[0]->begin(); 
             tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
             ++tob1) 
            {
                if( parType_t((*tob1)->Et()) <= std::min(p_MinET1,p_MinET2)) continue; // ET cut
                TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
                for( ;
                     tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading2;
                     ++tob2) {
                    if( parType_t((*tob2)->Et()) <= std::min(p_MinET1,p_MinET2)) continue; // ET cut
                    if( (parType_t((*tob1)->Et()) <= std::max(p_MinET1,p_MinET2)) && (parType_t((*tob2)->Et()) <= std::max(p_MinET1,p_MinET2))) continue;
                    // OneBarrel
                    if (p_OneBarrel && parType_t(std::abs((*tob1)->eta())) > 10 && parType_t(std::abs((*tob2)->eta())) > 10 ) continue;
                    // DeltaR2 cuts
                    unsigned int deltaR2 = TSU::Kinematics::calcDeltaR2( *tob1, *tob2 );
                    // Charge cut ( for TGC muons: 0=negative, 1=positive, as described at ATR-22621 )
                    // Check the definition of sectorName at MuCTPIL1TopoCandidate.h (for TGC muons: sectorName.at(0)=E or F, for RPC muons: sectorName.at(0)=B)
                    // If no muon sectorName information is available, sectorName = ""
                    std::string sector1 = (*tob1)->sectorName();
                    std::string sector2 = (*tob2)->sectorName();
                    int charge1 = (*tob1)->charge();
                    int charge2 = (*tob2)->charge();
                    int totalCharge = charge1 + charge2;
                    bool acceptCharge = true;
                    if ( sector1 != "" && sector2 != "" && sector1.at(0) != 'B' && sector2.at(0) != 'B' ) {
                       if ( totalCharge == 0 or totalCharge == 2 ) { acceptCharge = false; }
                    }
                    for(unsigned int i=0; i<numberOutputBits(); ++i) {
		      bool accept = false;
		      accept = deltaR2 >= p_DeltaRMin[i] && deltaR2 <= p_DeltaRMax[i] && acceptCharge;
		      const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		      const bool fillReject = fillHistos() and not fillAccept;
		      const bool alreadyFilled = decision.bit(i);
		      if( accept ) {
                        decision.setBit(i, true);  
                        output[i]->push_back( TCS::CompositeTOB(*tob1, *tob2) );
		      }
		      if(fillAccept and not alreadyFilled) {
			fillHist1D(m_histAccept[i],std::sqrt(static_cast<float>(deltaR2)));
                        fillHist2D(m_histAcceptEta1Eta2[i],(*tob1)->eta(),(*tob2)->eta());
		      } else if(fillReject) {
			fillHist1D(m_histReject[i],std::sqrt(static_cast<float>(deltaR2)));
                        fillHist2D(m_histRejectEta1Eta2[i],(*tob1)->eta(),(*tob2)->eta());
		      }
		      TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR2 = " << deltaR2);
                    }
                }
            }
    } else {
      TCS_EXCEPTION("DeltaRSqrIncl1Charge alg must have either 1 input, but got " << input.size());
    }
    return TCS::StatusCode::SUCCESS;
}
