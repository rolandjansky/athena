/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************
// Name: L1CaloPprPhos4ShapeCollection.h
// Package: TrigT1CaloCalibConditions
//
// Author: Taylor Childers (childers@mail.cern.ch)
// 
// This class holds a collection of PpmPhos4SignalShapes.
//
// For a description of the PpmPhos4SignalShape please
// see PpmPhos4SignalShape.h
//
// Each PpmPhos4SignalShape is organized into a std::map 
// according to COOL ID. Use one of the Fill functions
// as described here to fill the shapes for the channel id.
//
// 1. Fill(coolId,ns_step,adc): ns_step should be the nanosecond
//    step in the phos4 scan where the nanosecond step runs
//    from 0 to the number of 25 nanosecond time slices read
//    out times 25 nanoseconds, i.e. in 15+1 readout mode
//    ns_step would run from 0 to 25*15. adc is just the value
//    of the ADC read out for that step. This is done for
//    appropriate COOL channel ID that was specified using
//    coolId.
// 2. Fill(coolId,timeSlice,rodHeaderStep,adc): timeSlice is the
//    time slice of the ADC value to be stored. rodHeaderStep
//    is the step number from the DSS board, which is read out
//    via the ROD Headers. adc is just the value
//    of the ADC read out for that step. This is done for
//    appropriate COOL channel ID that was specified using
//    coolId.
// 3. Fill(coolId,rodHeaderStep,adc): rodHeaderStep is again the  
//    step number from the DSS board, which is read out via the  
//    ROD Headers. This time adc is a vector of the ADC values
//    with each entry being a different time slice. This is done for
//    appropriate COOL channel ID that was specified using
//    coolId.
//
// In the Finalize step of the algorithm you should call the
// Finalize() function. This function in turn calls the
// Finalize() function for each of the PpmPhos4SignalShape
// objects, which perform a pedestal correction to the signal
// shapes.
//
// The pedestal correction uses the first 2 time slices, which
// are assumed to be at the pedestal. This typically means you 
// need to be in 15+1 mode with the peak at 7. So if you are
// analyzing in 5+1 mode the processed histo will not be
// correct.
//
// The Get()-er functions are self explanitory I hope.
// 
// *********************************************************
#ifndef L1CALOPPRPHOS4SHAPECOLLECTION_H
#define L1CALOPPRPHOS4SHAPECOLLECTION_H

#include <map>
#include <string>
#include <sstream>
#include <fstream>

#include "TrigT1CaloCalibTools/L1CaloPprPhos4Shape.h"

#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CxxUtils/checker_macros.h"

class TTree;
class TFile;
class TProfile;
class TH1F;
class TH2F;
class TCanvas;

class MsgStreamMember;
class L1CaloCoolChannelId;

class L1CaloPprPhos4ShapeCollection{
public:
   L1CaloPprPhos4ShapeCollection();
   ~L1CaloPprPhos4ShapeCollection();
   
   L1CaloPprPhos4Shape* GetSignalShape(const L1CaloCoolChannelId& coolId);
   
   void SetMinimumSignalHeight(const unsigned int min){m_minSignalHeight=min;};
   unsigned int GetMinimumSignalHeight(void) const {return m_minSignalHeight;};
   
   void SetTimingRegime(const std::string regime){m_timingRegime = regime;};
   std::string GetTimingRegime(void) const {return m_timingRegime;};
   
   void SetHistogramTool(ToolHandle<LVL1::TrigT1CaloLWHistogramTool>& histTool){m_histTool = &histTool;};
   
   StatusCode SetCurrentFullDelayData(const L1CaloCoolChannelId& coolId,unsigned int delay);
   StatusCode SetPedValue(const L1CaloCoolChannelId& coolId,unsigned int value);
   StatusCode SetL1aFadcSlice(const L1CaloCoolChannelId& coolId,unsigned int slice);
   StatusCode SetValidChannel(const L1CaloCoolChannelId& coolId,bool validity);
   StatusCode SetChannelEnabled(const L1CaloCoolChannelId& coolId,bool enabled);
   
   bool SetChannelEtaPhiLayer(const L1CaloCoolChannelId& coolId,const float eta,const float phi,const L1CaloPprPhos4Shape::CaloLayer layer);
   
   typedef std::map<unsigned int,L1CaloPprPhos4Shape*> ShapesMap_t;
   typedef ShapesMap_t::iterator ShapesMap_itr;
   ShapesMap_itr begin(void){return m_signalShapes->begin();};
   ShapesMap_itr end(void){return m_signalShapes->end();};
   
   unsigned int size(void){return m_signalShapes->size();};
   
   StatusCode Fill(const L1CaloCoolChannelId& coolId,const unsigned int ns_step,const unsigned int adc);
   StatusCode Fill(const L1CaloCoolChannelId& coolId,const unsigned int timeSlice,const unsigned int rodHeaderStep,const int adc);
   StatusCode Fill(const L1CaloCoolChannelId& coolId,const unsigned int rodHeaderStep,const std::vector<int> adc);
   
   StatusCode Finalize ATLAS_NOT_REENTRANT(void);
   
   TProfile* GetRawSignalShape(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetRawSignalShape();};
   unsigned int GetRawMaxPeakBin(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetRawMaxPeakBin();};
   float GetRawMaxPeakValue(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetRawMaxPeakValue();};
   unsigned int GetRawMaxFullDelayData(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetRawMaxFullDelayData();};
   unsigned int GetRawFitPeakBin(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetRawFitPeakBin();};
   float GetRawFitPeakValue(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetRawFitPeakValue();};
   unsigned int GetRawFitFullDelayData(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetRawFitFullDelayData();};
   
   TH1F* GetProcessedSignalShape(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetProcessedSignalShape();};
   unsigned int GetProcessedMaxPeakBin(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetProcessedMaxPeakBin();};
   float GetProcessedMaxPeakValue(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetProcessedMaxPeakValue();};
   unsigned int GetProcessedMaxFullDelayData(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetProcessedMaxFullDelayData();};
   unsigned int GetProcessedFitPeakBin(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetProcessedFitPeakBin();};
   float GetProcessedFitPeakValue(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetProcessedFitPeakValue();};
   unsigned int GetProcessedFitFullDelayData(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetProcessedFitFullDelayData();};
   
   unsigned int GetErrorCode(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->GetErrorCode();};
   
   bool IsProcessed(const L1CaloCoolChannelId& coolId){return GetMapIterator(coolId)->second->IsProcessed();};
   
   void SetRunNumber(const unsigned int runNumber){m_runNumber=runNumber;std::stringstream ss;ss.width(8);ss.fill('0');ss<<m_runNumber;ss>>m_runNumberStr;};
   unsigned int GetRunNumber(void){return m_runNumber;};
   
   void SetOutputDirectoryName(std::string name){m_outputFileDirName = name;};
   std::string GetOutputDirectoryName(void){return m_outputFileDirName;};
   
   void SetTileRun(bool value = true){m_isTileRun = value;};
   bool IsTileRun(void) const {return m_isTileRun;};
   
   void SetLArRun(bool value = true){m_isLArRun = value;};
   bool IsLArRun(void) const {return m_isLArRun;};
   
   
private:
   // don't want C++ implementing these automatically
   L1CaloPprPhos4ShapeCollection(const L1CaloPprPhos4ShapeCollection& rhs);
   void operator=(const L1CaloPprPhos4ShapeCollection& rhs);
   
   bool m_isTileRun;
   bool m_isLArRun;
   
   Athena::MsgStreamMember* m_log;
   
   unsigned int m_minSignalHeight;
   std::string m_timingRegime;
   
   ToolHandle<LVL1::TrigT1CaloLWHistogramTool>* m_histTool;
   
   ShapesMap_t* m_signalShapes;
   ShapesMap_itr m_signalShapesItr;
   
   ShapesMap_itr GetMapIterator(const L1CaloCoolChannelId& coolId);
   bool CoolIdExists(const L1CaloCoolChannelId& coolId) const;
   std::string CoolIdToString(const L1CaloCoolChannelId& coolId) const;
   std::string GetPadTitle(const unsigned int coolId) const;
   TH2F* SaveEtaPhiHisto(TH2F_LW *histo,const float min = -3.,const float max = 3., const unsigned int contourBins = 12) const;
   
   bool IsTileChannel(L1CaloPprPhos4Shape const * const shape) const;
   bool IsLArChannel(L1CaloPprPhos4Shape const * const shape) const;
   
   void MergeErrorAndShapes(std::string& error, std::string& shapes) const;
   
   void gzipFile(std::string& filename) const;
   void ps2pdf(std::string& filename) const;
   void psmerge(std::string& file1, std::string& file2, std::string& outputfile) const;
   void rm(std::string& file) const;
   void mv(std::string& file1, std::string& file2) const;
   
   std::string m_outputFileDirName;
   std::string m_outputFileName;
   std::string m_outputFileNamePostfix;
   std::string m_dbFileName_rawMax;
   std::string m_dbFileName_rawFit;
   std::string m_dbFileName_processedMax;
   std::string m_dbFileName_processedFit;
   std::string m_outDBFileNamePostfix;
   
   std::string m_outRawCanvasName;
   std::string m_outRawCanvasNamePostfix;
   std::string m_outProcessedCanvasName;
   std::string m_outProcessedCanvasNamePostfix;
   
   std::string m_outputSummaryPlotsFileName;
   std::string m_outputSummaryPlotsPostfix;
   std::string m_outputSummaryCanvasPostfix;
   
   
   // TTree variables and names
   std::string m_coolIdName;
   unsigned int m_coolId;
   
   std::string m_isEnabledName;
   bool m_isEnabled;
   
   std::string m_isValidName;
   bool m_isValid;
   
   std::string m_rawSignalShapeName;
   TProfile* m_rawSignalShape;
   std::string m_rawMaxPeakBinName;
   unsigned int m_rawMaxPeakBin;
   std::string m_rawMaxPeakValueName;
   float m_rawMaxPeakValue;
   std::string m_rawMaxFullDelayDataName;
   unsigned int m_rawMaxFullDelayData;
   std::string m_rawFitPeakBinName;
   unsigned int m_rawFitPeakBin;
   std::string m_rawFitPeakValueName;
   float m_rawFitPeakValue;
   std::string m_rawFitFullDelayDataName;
   unsigned int m_rawFitFullDelayData;
   std::string m_rawFitPerformedName;
   bool m_rawFitPerformed;
   
   std::string m_rawFitName;
   TF1* m_rawFit;
   
   std::string m_pedestalMeanName;
   float m_pedestalMean;
   std::string m_pedestalSigmaName;
   float m_pedestalSigma;
   
   std::string m_processedSignalShapeName;
   TH1F* m_processedSignalShape;
   std::string m_processedMaxPeakBinName;
   unsigned int m_processedMaxPeakBin;
   std::string m_processedMaxPeakValueName;
   float m_processedMaxPeakValue;
   std::string m_processedMaxFullDelayDataName;
   unsigned int m_processedMaxFullDelayData;
   std::string m_processedFitPeakBinName;
   unsigned int m_processedFitPeakBin;
   std::string m_processedFitPeakValueName;
   float m_processedFitPeakValue;
   std::string m_processedFitFullDelayDataName;
   unsigned int m_processedFitFullDelayData;
   std::string m_processedFitPerformedName;
   bool m_processedFitPerformed;
   
   std::string m_processedFitName;
   TF1* m_processedFit;
   
   std::string m_isProcessedName;
   bool m_isProcessed;
   
   std::string m_risingSlopeMeanName;
   float m_risingSlopeMean;
   std::string m_risingSlopeSigmaName;
   float m_risingSlopeSigma;
   
   std::string m_currentFullDelayDataName;
   unsigned int m_currentFullDelayData;
   
   std::string m_errorCodeName;
   unsigned int m_errorCode;
   
   std::string m_runNumberName;
   unsigned int m_runNumber;
   std::string m_runNumberStr;
   
   static const float PI;
};


#endif

