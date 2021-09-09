/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************
// Name: L1CaloPprPhos4Shape.cxx
// Package: TrigT1CaloCalibConditions
//
// Author: Taylor Childers (childers@mail.cern.ch)
// 
// See header for descriptions
//
// *********************************************************

#include "TrigT1CaloCalibTools/L1CaloPprPhos4Shape.h"

L1CaloPprPhos4Shape::L1CaloPprPhos4Shape(const L1CaloCoolChannelId& coolId)
{
   
   
   m_log = new Athena::MsgStreamMember(Athena::Options::Eager,"L1CaloPprPhos4Shape");
   
   m_coolId                         = coolId;
   m_eta                            = 999.;
   m_phi                            = 999.;
   m_layer                          = LAYER_NOT_SET;
   
   m_minSignalHeight                = 30;
   
   m_isValid                        = true;
   m_isEnabled                      = true;
   
   m_currentFullDelayData           = 0;
   m_pedValue                       = 0;
   m_l1aFadcSlice                   = 0;
   
   
   // setup raw signal profile.
   m_raw_signalShape = new TProfile(GetPlotName("rawSignalShape",m_coolId).c_str(),GetPlotTitle("",m_coolId).c_str(),MAX_ADC_TIME_SLICES*NANOSEC_PER_LHC_CLOCK_TICK,0,MAX_ADC_TIME_SLICES*NANOSEC_PER_LHC_CLOCK_TICK);
//    sm_raw_signalShape->GetXaxis()->SetTitle("nanosecond steps");
   
   m_raw_maxPeakValue               = 0.;
   m_raw_maxPeakBin                 = 0;
   m_raw_maxFullDelayData           = 0;
   m_raw_fitPeakValue               = 0.;
   m_raw_fitPeakBin                 = 0;
   m_raw_fitFullDelayData           = 0;
   m_raw_fitPerformed               = true;
   
   m_raw_fit                        = 0;
   
   m_rawPedestalMean                = 0.;
   m_rawPedestalSigma               = 0.;
   
   // I use the first 50 entries, which should all be pedestal level, to
   // correct for the nanosecond depedent PHOS4 correction.
   m_pedCorrectionSum.resize(NANOSEC_PER_LHC_CLOCK_TICK,0);
   m_pedCorrectionSum2.resize(NANOSEC_PER_LHC_CLOCK_TICK,0);
   m_pedCorrectionN.resize(NANOSEC_PER_LHC_CLOCK_TICK,0);
   m_pedCorrectionMean.resize(NANOSEC_PER_LHC_CLOCK_TICK,0.0);
   m_pedCorrectionSigma.resize(NANOSEC_PER_LHC_CLOCK_TICK,0.0);
   
   m_processed_signalShape          = 0;
   m_processed_maxPeakValue         = 0.;
   m_processed_maxPeakBin           = 0;
   m_processed_maxFullDelayData     = 0;
   m_processed_fitPeakValue         = 0.;
   m_processed_fitPeakBin           = 0;
   m_processed_fitFullDelayData     = 0;
   m_processed_fitPerformed         = true;
   
   m_processed_fit                  = 0;
   
   m_risingSlopeMean                = 0.;
   m_risingSlopeSigma               = 0.;
   
   m_processSignalShape = true;
   
}

L1CaloPprPhos4Shape::~L1CaloPprPhos4Shape(void){

}

/// Fill the profile with the ADC values at each nanosecond step
StatusCode L1CaloPprPhos4Shape::Fill(const unsigned int ns_step,const unsigned int adc){
   m_raw_signalShape->Fill(ns_step,adc);
   
   if(ns_step < NANOSEC_PER_LHC_CLOCK_TICK*2){
      const unsigned int phos4step = ns_step % NANOSEC_PER_LHC_CLOCK_TICK;
      m_pedCorrectionSum[phos4step] += adc;
      m_pedCorrectionSum2[phos4step] += adc*adc;
      ++m_pedCorrectionN[phos4step];
   }
   
   return StatusCode::SUCCESS;
}

/// Fill the profile with the ADC values at each nanosecond step
/// Here I can accept the rodHeaderStep and ADC time slice directly
/// and I will calculate the nanosecond step
StatusCode L1CaloPprPhos4Shape::Fill(const unsigned int timeSlice,const unsigned int rodHeaderStep,const int adc){
   const unsigned int phos4step = rodHeaderStep % NANOSEC_PER_LHC_CLOCK_TICK;
   const unsigned int ns_step = timeSlice*NANOSEC_PER_LHC_CLOCK_TICK + phos4step; 
   m_raw_signalShape->Fill(ns_step,adc);
   
   if(ns_step < NANOSEC_PER_LHC_CLOCK_TICK*2){
      m_pedCorrectionSum[phos4step] += adc;
      m_pedCorrectionSum2[phos4step] += adc*adc;
      ++m_pedCorrectionN[phos4step];
   }
   
   return StatusCode::SUCCESS;
}

/// Fill the profile with the ADC values at each nanosecond step
/// Here I can accept the rodHeaderStep and a vector of the ADC
/// values, which should only contain as many entries as there are
/// ADC time slices since I use the vector to determine the number
/// of time slices
StatusCode L1CaloPprPhos4Shape::Fill(const unsigned int rodHeaderStep,const std::vector<int>& adc){
   const unsigned int phos4step = rodHeaderStep % NANOSEC_PER_LHC_CLOCK_TICK;
   
   for(unsigned int timeSlice = 0;timeSlice < adc.size();++timeSlice){
      const unsigned int ns_step = timeSlice*NANOSEC_PER_LHC_CLOCK_TICK + phos4step; 
      m_raw_signalShape->Fill(ns_step,adc[timeSlice]);
   }
   
   m_pedCorrectionSum[phos4step] += adc[0];
   m_pedCorrectionSum2[phos4step] += adc[0]*adc[0];
   ++m_pedCorrectionN[phos4step];
   
   m_pedCorrectionSum[phos4step] += adc[1];
   m_pedCorrectionSum2[phos4step] += adc[1]*adc[1];
   ++m_pedCorrectionN[phos4step];
   
   return StatusCode::SUCCESS;
}

/// In finalize I want the profile to be processed and extract
/// some useful characteristics like the peak value
/// and the peak location
StatusCode L1CaloPprPhos4Shape::Finalize(void){
   
   
   // calculate pedestal with first 50 ns steps
   CalculatePedestal(m_raw_signalShape,m_rawPedestalMean,m_rawPedestalSigma);
   const unsigned int pedestal = (unsigned int)m_rawPedestalMean;
   // use pedestal + minSignalHeight as the cut unless it is lower than 6sigma
   unsigned int signalCut = m_minSignalHeight;
   if(signalCut < (unsigned int)(m_rawPedestalSigma*6.))
      signalCut = (unsigned int)(m_rawPedestalSigma*6.);
   
   // signal cut above pedestal
   signalCut += pedestal;

   bool signalIsPresent = IsSignalPresent(m_raw_signalShape,signalCut);
   
   if(!signalIsPresent || !m_isValid || !m_isEnabled){
      // subtract the pedestal correction from the raw signal
      CorrectRawPedestal();
      
      FillErrorCode(signalIsPresent,false);
      
      return StatusCode::SUCCESS;
   }
   
   // check if the signal is saturated
   const unsigned short int saturationLevel = 1020;
   bool signalIsSaturated = IsSignalSaturated(m_raw_signalShape,saturationLevel);
   // if signal is saturated, don't fit signals
   if(signalIsSaturated){
      m_raw_fitPerformed = false;
      m_processed_fitPerformed = false;
   }
   
   // find the peak value and location for the raw signal
   FindBinWithMaxValue(m_raw_signalShape,m_raw_maxPeakValue,m_raw_maxPeakBin);
   // calculate the fullDelayData for these max values
   m_raw_maxFullDelayData = CalculateFullDelayData(m_raw_maxPeakBin - 1);
   // find the peak value and location using landau/gaus fit
   if(m_raw_fitPerformed){
      m_raw_fit = FitSignalPeak(m_raw_signalShape,m_raw_maxPeakValue,m_raw_maxPeakBin,m_pedValue,m_raw_fitPeakValue,m_raw_fitPeakBin);
      std::string rawFitName = "raw_";
      rawFitName += m_raw_fit->GetName();
      m_raw_fit->SetName(rawFitName.c_str());
      // calculate the fullDelayData for these fitvalues
      m_raw_fitFullDelayData = CalculateFullDelayData(m_raw_fitPeakBin - 1);
   }
   
   // find the peak value and location for the corrected signal
   // and calculate the new FullDataDelay
   if(m_processSignalShape){
      // subtract the pedestal correction from the raw signal
      CorrectRawPedestal();
      
      FindBinWithMaxValue(m_processed_signalShape,m_processed_maxPeakValue,m_processed_maxPeakBin);
      // calculate the fullDelayData for these fitvalues
      m_processed_maxFullDelayData = CalculateFullDelayData(m_processed_maxPeakBin - 1);
      
      
      // find the peak value and location using landau/gaus fit
      if(m_processed_fitPerformed){
         m_processed_fit = FitSignalPeak(m_processed_signalShape,m_processed_maxPeakValue,m_processed_maxPeakBin,0,m_processed_fitPeakValue,m_processed_fitPeakBin);
         
         std::string processedFitName = "processed_";
         processedFitName += m_processed_fit->GetName();
         m_processed_fit->SetName(processedFitName.c_str());
         
         // calculate the fullDelayData for these fitvalues
         m_processed_fitFullDelayData = CalculateFullDelayData(m_processed_fitPeakBin - 1);
      }
   }
   
   // Fill Error Code
   FillErrorCode(signalIsPresent,signalIsSaturated);
   
   return StatusCode::SUCCESS;
}

/// Given a signal profile, this finds the maximum value and it's corresponding bin
void L1CaloPprPhos4Shape::FindBinWithMaxValue(TH1* histo,float& peakValue,unsigned int& peakBin){
   
   peakValue = 0;
   peakBin = 0;
   for(unsigned int bin=1;(int)bin<=histo->GetNbinsX();++bin){
      if(peakValue < histo->GetBinContent(bin)){
         peakValue = histo->GetBinContent(bin);
         peakBin = bin;
      }
   }
}
/// Given a signal profile, this finds the maximum value and it's corresponding bin
TF1* L1CaloPprPhos4Shape::FitSignalPeak(TH1* histo,const float maxPeakValue,const unsigned int maxPeakBin,const unsigned int pedestal,float& fitPeakValue,unsigned int& fitPeakBin){
   
   fitPeakValue = 0.;
   fitPeakBin = 0;
   
   const unsigned int fitRangeWidth = 13;
   const unsigned int minFitRange   = maxPeakBin - fitRangeWidth - 1; // -1 for bin to ns correction
   const unsigned int maxFitRange   = maxPeakBin + fitRangeWidth - 1;
   
   LandGaussFunc *fitFunc = new LandGaussFunc;
   
   std::string fitName = "fit_" + CoolIdToString(m_coolId);
   TF1* fit = new TF1(fitName.c_str(),fitFunc,minFitRange,maxFitRange,5);
   // fix the pedestal value
   fit->FixParameter(4,pedestal);
   
   // constrain the mean position
//    const unsigned int xmax_min = 0, xmax_max = 15*25;
//    fit->SetParLimits(0,xmax_min,xmax_max);
   // set initial mean position to that of the maximum bin
   const unsigned int xmax_init = maxPeakBin-1;
   fit->SetParameter(0,xmax_init);
   
   // constrain the amplitude
//    const unsigned int amp_min = m_minSignalHeight+pedestal, amp_max = 1020;
//    fit->SetParLimits(1,amp_min,amp_max);
   // set initial amplitude
   const float amp_init = (maxPeakValue-pedestal)*exp(0.5);
   fit->SetParameter(1,amp_init);
   
   // constrain the sigma gaus
//    const unsigned int gSigma_min = 1, gSigma_max = 60;
//    fit->SetParLimits(2,gSigma_min,gSigma_max);
   // set initial sigma gaus
   const unsigned int gSigma_init = 25;
   fit->SetParameter(2,gSigma_init);
   
   // constrain the sigma landau
//    const unsigned int lSigma_min = 1, lSigma_max = 60;
//    fit->SetParLimits(3,lSigma_min,lSigma_max);
   // set initial sigma gaus
   const unsigned int lSigma_init = 15;
   fit->SetParameter(3,lSigma_init);
   
   histo->Fit(fit,"qurnob");
   
   
   
//    const float x_max = fit->GetParameter(0);
   const float ampl  = fit->GetParameter(1);
//    const float gSigma = fit->GetParameter(2);
//    const float lSigma = fit->GetParameter(3);
//    const float ped   = fit->GetParameter(4);
   
   fitPeakValue = ampl*exp(-0.5)+pedestal;
   // +0.5 deals with rounding. If fitPeakValue is between x.0 and x.49
   // the bin should not change after adding 0.5, but if it is x.5 to x.9
   // the bin should be assigned to the next bin (so rounding up)
   fitPeakBin = histo->FindBin(fit->GetParameter(0));
   
   delete fitFunc;
   fitFunc = 0;
   
   return fit;
}

void L1CaloPprPhos4Shape::CalculatePedestal(TH1* histo,float& pedMean,float& pedSigma){
   
   if((unsigned int)histo->GetNbinsX() < NANOSEC_PER_LHC_CLOCK_TICK*2){
      *m_log << MSG::WARNING << "[CalculatePedestal] Could not calculate pedestal due to lack of bins in " << histo->GetName() << endmsg;
      return;
   }
   
   float pedSum=0,pedSum2=0;
   unsigned int pedN=0;
   for(unsigned int bin=1;bin<NANOSEC_PER_LHC_CLOCK_TICK*2;++bin){
      pedSum += histo->GetBinContent(bin);
      pedSum2 += histo->GetBinContent(bin)*histo->GetBinContent(bin);
      pedN++;
   }
   // pedN > 0 <=>  NANOSEC_PER_LHC_CLOCK_TICK > 0, which is protected by the above return
   pedMean = pedSum/(float)pedN;
   pedSigma = sqrt( (1.0/(float)pedN)*pedSum2 - pedMean*pedMean);
   
}

void L1CaloPprPhos4Shape::CorrectRawPedestal(void){
   
   // if the pedestal sigma is greater than this value we set
   // the pedestal is flat to false so that the correction is
   // not completed and the raw signal is used instead.
   m_processed_signalShape = new TH1F(GetPlotName("processedSignalShape",m_coolId).c_str(),GetPlotTitle("",m_coolId).c_str(),MAX_ADC_TIME_SLICES*NANOSEC_PER_LHC_CLOCK_TICK,0,MAX_ADC_TIME_SLICES*NANOSEC_PER_LHC_CLOCK_TICK);
   //m_processed_signalShape->GetXaxis()->SetTitle("nanosecond steps");
   
   if(m_rawPedestalSigma > MAX_PEDESTAL_SIGMA){
      m_processSignalShape = false;
   }
   else{
      CalculatePedCorrection();
      
      
      for(unsigned int bin=1;(int)bin<m_raw_signalShape->GetNbinsX();++bin){
         
         unsigned int phos4step = (bin - 1) % NANOSEC_PER_LHC_CLOCK_TICK;
         
         // add the bin content from the raw and pedestal correction
         m_processed_signalShape->SetBinContent(bin,m_raw_signalShape->GetBinContent(bin)-m_pedCorrectionMean[phos4step]);
         
         // calculate the error
         float error = sqrt(m_raw_signalShape->GetBinError(bin)*m_raw_signalShape->GetBinError(bin) + m_pedCorrectionSigma[phos4step]*m_pedCorrectionSigma[phos4step] );
         
         m_processed_signalShape->SetBinError(bin,error);
         
      }
   }
   
}

void L1CaloPprPhos4Shape::CalculatePedCorrection(void){
   for(unsigned int step=0;step<NANOSEC_PER_LHC_CLOCK_TICK;++step){
      m_pedCorrectionMean[step] = (m_pedCorrectionN[step] == 0) ? 0 : (float)m_pedCorrectionSum[step]/(float)m_pedCorrectionN[step];
      m_pedCorrectionSigma[step] = (m_pedCorrectionN[step] == 0) ? 0 : sqrt( (1.0/(float)m_pedCorrectionN[step])* (float)m_pedCorrectionSum2[step] - m_pedCorrectionMean[step]*m_pedCorrectionMean[step] );
   }
   
}

unsigned int L1CaloPprPhos4Shape::CalculateFullDelayData(const int truePeakPosition){
   
   // Stephen Hillier derived this calculation
   
   // integer division to get the course timing delay
   const int courseDelay = (int)(m_currentFullDelayData/NANOSEC_PER_LHC_CLOCK_TICK);
   // base delay
   const int baseDelay   = courseDelay*NANOSEC_PER_LHC_CLOCK_TICK + 9;
   
   const int idealPeakPosition = m_l1aFadcSlice*NANOSEC_PER_LHC_CLOCK_TICK;
   const int dt = idealPeakPosition - truePeakPosition;
   
   const int newFullDelayData = (baseDelay + dt);
   if(newFullDelayData < 0){
      *m_log << MSG::WARNING << "[CalculateFullDelayData]: New Full Delay data was calculated as zero. Should only be greater than zero. Returning 0 for safety. Channel 0x" << CoolIdToString(m_coolId) << ". Peak position passed was: " << truePeakPosition << " and the idealPeakPosition is: " << idealPeakPosition << endmsg;
      return 0;
   }
   
   return (unsigned int)newFullDelayData;
}

void L1CaloPprPhos4Shape::FillErrorCode(bool signalIsPresent,bool signalIsSaturated){
   
   // if the signal is not present set the bad data bit
   if(!signalIsPresent){
      *m_log << MSG::WARNING << "[FillErrorCode] Set bad data bit because signal not present. Channel 0x" << CoolIdToString(m_coolId) << endmsg;
      m_errorCode.badData(true);
      m_errorCode.phos4ScanNoSignal(true);
      
      // fill FullDelayData variables with current database value for safety
      m_raw_maxFullDelayData = m_currentFullDelayData;
      m_raw_fitFullDelayData = m_currentFullDelayData;
      m_processed_maxFullDelayData = m_currentFullDelayData;
      m_processed_fitFullDelayData = m_currentFullDelayData;
      return;
   }
   
   if(signalIsSaturated){
      *m_log << MSG::WARNING << "[FillErrorCode] Set bad data bit because signal is saturated. Channel 0x" << CoolIdToString(m_coolId) << endmsg;
      m_errorCode.badData(true);
      m_errorCode.phos4ScanSignalSaturated(true);
      
      // fill FullDelayData variables with current database value for safety
      m_raw_maxFullDelayData = m_currentFullDelayData;
      m_raw_fitFullDelayData = m_currentFullDelayData;
      m_processed_maxFullDelayData = m_currentFullDelayData;
      m_processed_fitFullDelayData = m_currentFullDelayData;
      
   }
   
   if(m_processSignalShape)
      CheckRisingSlope(m_processed_signalShape,m_processed_maxPeakBin,m_processed_maxPeakValue);
   else
      CheckRisingSlope(m_raw_signalShape,m_raw_maxPeakBin,m_raw_maxPeakValue);
   
   // if no Mean or sigma was calculated this usually indicates a bad signal shape
   if((m_risingSlopeMean < 0.0001 || m_risingSlopeSigma < 0.0001)){
      *m_log << MSG::WARNING << "[FillErrorCode] Set bad data bit because the signal shape is corrupt. Channel 0x" << CoolIdToString(m_coolId) << endmsg;
      m_errorCode.badData(true);
      *m_log << MSG::WARNING << "[FillErrorCode] Set phos4 scan bad signal bit because the signal shape is corrupt. Channel 0x" << CoolIdToString(m_coolId) << endmsg;
      m_errorCode.phos4ScanBadSignal(true);
      
      
      // fill FullDelayData variables with current database value for safety
      m_raw_maxFullDelayData = m_currentFullDelayData;
      m_raw_fitFullDelayData = m_currentFullDelayData;
      m_processed_maxFullDelayData = m_currentFullDelayData;
      m_processed_fitFullDelayData = m_currentFullDelayData;
   }
   else if( m_risingSlopeSigma > 10.0){
      *m_log << MSG::WARNING << "[FillErrorCode] Set bad data because the signal rising edge instantaneous slope has a large sigma. Channel 0x" << CoolIdToString(m_coolId) << endmsg;
      m_errorCode.badData(true);
      *m_log << MSG::WARNING << "[FillErrorCode] Set phos4 scan bad signal bit because the signal rising edge instantaneous slope has a large sigma. Channel 0x" << CoolIdToString(m_coolId) << endmsg;
      m_errorCode.phos4ScanBadSignal(true);
      
      
      // fill FullDelayData variables with current database value for safety
      m_raw_maxFullDelayData = m_currentFullDelayData;
      m_raw_fitFullDelayData = m_currentFullDelayData;
      m_processed_maxFullDelayData = m_currentFullDelayData;
      m_processed_fitFullDelayData = m_currentFullDelayData;
   }
   
   
}

// Verifies that a signal exists in this plot
// tests that the peak is above the passed threshold
bool L1CaloPprPhos4Shape::IsSignalPresent(TH1* histo,const unsigned int threshold) const{
   for(unsigned int bin=1;bin<=(unsigned int)histo->GetNbinsX();++bin){
      if(histo->GetBinContent(bin) > threshold){
         return true;
      }
   }
   return false;
}

// tests for signal saturation
bool L1CaloPprPhos4Shape::IsSignalSaturated(TH1* histo,const unsigned int saturationPoint) const{
   for(unsigned int bin=1;bin<=(unsigned int)histo->GetNbinsX();++bin){
      if(histo->GetBinContent(bin) >= saturationPoint){
         return true;
      }
   }
   return false;
}


// verifies that there are no spikes in the slope
// it should be somewhat linear
void L1CaloPprPhos4Shape::CheckRisingSlope(TH1* histo,unsigned int peakBin,float peakValue){
   
   // want to only look at the slope from 10% to 90%
   const float peakValue10percent = peakValue*0.10;
   const float peakValue90percent = peakValue*0.90;
   unsigned int peakBin10percent = 0;
   unsigned int peakBin90percent = 0;
   for(unsigned int bin=peakBin;bin>0;--bin){
      if(histo->GetBinContent(bin) < peakValue10percent && peakBin10percent == 0){
         peakBin10percent = bin;
         break;
      }
      if(histo->GetBinContent(bin) < peakValue90percent && peakBin90percent == 0){
         peakBin90percent = bin;
      }
      
   }// end for(bin)
   
   //now calculate the mean and sigma instantaneous slope
   float sum=0,sum2=0;
   unsigned int n=0;
   for(unsigned int bin=peakBin10percent+1;bin<=peakBin90percent;++bin){
      float dydx = histo->GetBinContent(bin) - histo->GetBinContent(bin-1);
      // dx = 1 by definition so dy = dydx
      
      sum += dydx;
      sum2 += dydx*dydx;
      n++;
      
   }
   
   m_risingSlopeMean = (n == 0) ? 0 : sum/(float)n;
   m_risingSlopeSigma = (n == 0) ? 0 : sqrt( (1.0/(float)n)*sum2 - m_risingSlopeMean*m_risingSlopeMean);
   
}

std::string L1CaloPprPhos4Shape::GetPlotName(const std::string& base, const L1CaloCoolChannelId& coolId) const{
   std::string tempStr = base + "_" + CoolIdToString(coolId);
   return tempStr;
}

std::string L1CaloPprPhos4Shape::GetPlotTitle(const std::string& base, const L1CaloCoolChannelId& coolId) const{
   std::string tempStr = base + " " + CoolIdToString(coolId);
   return tempStr;
}

std::string L1CaloPprPhos4Shape::CoolIdToString(const L1CaloCoolChannelId& coolId) const{
   char temp[200];
   sprintf(temp,"%08x",coolId.id());
   std::string tempStr = temp;
   return tempStr;
}
