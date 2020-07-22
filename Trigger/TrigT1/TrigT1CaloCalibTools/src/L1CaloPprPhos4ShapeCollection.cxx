/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibTools/L1CaloPprPhos4ShapeCollection.h"
#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

// Get the message service and StoreGate
#include "AthenaKernel/MsgStreamMember.h"

#include <sstream>
#include <unistd.h>


// ROOT includes
#include "TTree.h"
#include "TFile.h"
#include "TProfile.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"

#include "LWHists/TH2F_LW.h"


const float L1CaloPprPhos4ShapeCollection::PI = 3.14159265;

L1CaloPprPhos4ShapeCollection::L1CaloPprPhos4ShapeCollection(void):
   m_minSignalHeight(30),
   m_timingRegime("Calib1"),
   m_histTool(0)
{
   // turn off directory ownership in ROOT
   TH1::AddDirectory(0);

   m_isTileRun                   =  false;
   m_isLArRun                    =  false;

   m_signalShapes = new ShapesMap_t;
   m_signalShapes->clear();
   m_signalShapesItr             =  m_signalShapes->end();

   m_outputFileDirName           =  ".";
   m_outputFileName              =  "shapeMakerData";
   m_outputFileNamePostfix       =  "root";

   m_dbFileName_rawMax           =  "rawMax";
   m_dbFileName_rawFit           =  "rawFit";
   m_dbFileName_processedMax     =  "processedMax";
   m_dbFileName_processedFit     =  "processedFit";
   m_outDBFileNamePostfix        =  "txt";

   m_outRawCanvasName            =  "rawShapes";
   m_outRawCanvasNamePostfix     =  "ps";
   m_outProcessedCanvasName      =  "processedShapes";
   m_outProcessedCanvasNamePostfix = "ps";

   m_outputSummaryPlotsFileName  =  "summary";
   m_outputSummaryPlotsPostfix   =  "root";
   m_outputSummaryCanvasPostfix  =  "ps";

   m_coolIdName                  =  "coolId";
   m_coolId                      =  0;

   m_isEnabledName               =  "isEnabled";
   m_isEnabled                   =  false;

   m_isValidName                 =  "isValid";
   m_isValid                     =  false;

   m_rawSignalShapeName          =  "rawSignalShape";
   m_rawSignalShape              =  0;
   m_rawMaxPeakBinName           =  "rawMaxPeakBin";
   m_rawMaxPeakBin               =  0;
   m_rawMaxPeakValueName         =  "rawMaxPeakValue";
   m_rawMaxPeakValue             =  0.0;
   m_rawMaxFullDelayDataName     =  "rawMaxFullDelayData";
   m_rawMaxFullDelayData         =  0;
   m_rawFitPeakBinName           =  "rawFitPeakBin";
   m_rawFitPeakBin               =  0;
   m_rawFitPeakValueName         =  "rawFitPeakValue";
   m_rawFitPeakValue             =  0.0;
   m_rawFitFullDelayDataName     =  "rawFitFullDelayData";
   m_rawFitFullDelayData         =  0;
   m_rawFitPerformedName         =  "rawFitPerformed";
   m_rawFitPerformed             =  false;

   m_rawFitName                  =  "rawFit";
   m_rawFit                      =  0;

   m_pedestalMeanName            =  "pedestalMean";
   m_pedestalMean                =  0.0;
   m_pedestalSigmaName           =  "pedestalSigma";
   m_pedestalSigma               =  0.0;

   m_processedSignalShapeName    =  "processedSignalShape";
   m_processedSignalShape        =  0;
   m_processedMaxPeakBinName     =  "processedMaxPeakBin";
   m_processedMaxPeakBin         =  0;
   m_processedMaxPeakValueName   =  "processedMaxPeakValue";
   m_processedMaxPeakValue       =  0.0;
   m_processedMaxFullDelayDataName =  "processedMaxFullDelayData";
   m_processedMaxFullDelayData   =  0;
   m_processedFitPeakBinName     =  "processedFitPeakBin";
   m_processedFitPeakBin         =  0;
   m_processedFitPeakValueName   =  "processedFitPeakValue";
   m_processedFitPeakValue       =  0.0;
   m_processedFitFullDelayDataName =  "processedFitFullDelayData";
   m_processedFitFullDelayData   =  0;
   m_processedFitPerformedName   =  "processedFitPerformed";
   m_processedFitPerformed       =  false;

   m_processedFitName            =  "processedFit";
   m_processedFit                =  0;

   m_isProcessedName             =  "isProcessed";
   m_isProcessed                 =  false;

   m_risingSlopeMeanName         =  "risingSlopeMean";
   m_risingSlopeMean             =  0.0;
   m_risingSlopeSigmaName        =  "risingSlopeSigma";
   m_risingSlopeSigma            =  0.0;

   m_currentFullDelayDataName    =  "currentFullDelayData";
   m_currentFullDelayData        =  0;

   m_errorCodeName               =  "errorCode";
   m_errorCode                   =  0;

   m_runNumberName               =  "runNumber";
   m_runNumber                   =  0;
   m_runNumberStr                =  "";

   m_log = new Athena::MsgStreamMember(Athena::Options::Eager, "L1CaloPprPhos4ShapeCollection");
}

L1CaloPprPhos4ShapeCollection::~L1CaloPprPhos4ShapeCollection()
{
   for (auto& P : *m_signalShapes) delete P.second;
   delete m_signalShapes;
   delete m_log;
}

L1CaloPprPhos4Shape* L1CaloPprPhos4ShapeCollection::GetSignalShape(const L1CaloCoolChannelId& coolId) {

   if ( coolId.id() == m_signalShapesItr->first) {
      return m_signalShapesItr->second;
   }

   m_signalShapesItr = m_signalShapes->find(coolId.id());

   if ( m_signalShapesItr == m_signalShapes->end()) {
      *m_log << MSG::WARNING << "[GetSignalShape] Did not find requested COOL ID, 0x" << std::hex << coolId.id() << std::dec << ", in signalShapes." << endmsg;
      return (L1CaloPprPhos4Shape*)0;
   }

   return m_signalShapesItr->second;
}

L1CaloPprPhos4ShapeCollection::ShapesMap_itr L1CaloPprPhos4ShapeCollection::GetMapIterator(const L1CaloCoolChannelId& coolId) {

   // first check that the current iterator isn't the right one already
   if (m_signalShapesItr != m_signalShapes->end()) {
      if ( coolId.id() == m_signalShapesItr->first) {
         return m_signalShapesItr;
      }
   }

   m_signalShapesItr = m_signalShapes->find(coolId.id());

   // check that the iterator was found
   // if it was not then I create a new one
   // for this COOL ID
   if (m_signalShapesItr == m_signalShapes->end()) {

//       *m_log << MSG::INFO << " Creating coolId: 0x" << std::hex << coolId << std::dec << endmsg;

      // make sure cool ID exists first
      if (!CoolIdExists(coolId)) {
         *m_log << MSG::WARNING << "[GetMapIterator] channel 0x" << CoolIdToString(coolId) << " does not exist and will not be created." << endmsg;
         return m_signalShapes->end();
      }

      L1CaloPprPhos4Shape* temp = new L1CaloPprPhos4Shape(coolId);
      temp->SetMinimumSignalHeight(m_minSignalHeight);

      (*m_signalShapes)[coolId.id()] = temp;
      m_signalShapesItr = m_signalShapes->find(coolId.id());

      if (m_signalShapesItr == m_signalShapes->end()) {
         *m_log << MSG::WARNING << "[GetMapIterator] Something strange going on. Did not find newly created signal shape for cool ID: 0x" << CoolIdToString(coolId) << endmsg;
         return m_signalShapes->end();
      }
   }

   return m_signalShapesItr;

}

StatusCode L1CaloPprPhos4ShapeCollection::Fill(const L1CaloCoolChannelId& coolId, const unsigned int ns_step, const unsigned int adc) {
   ShapesMap_itr itr = GetMapIterator(coolId);
   if (itr == m_signalShapes->end()) {
      *m_log << MSG::WARNING << "[Fill] no iterator returned for 0x" << CoolIdToString(coolId) << endmsg;
      return StatusCode::FAILURE;
   }
   return itr->second->Fill(ns_step, adc);
}


StatusCode L1CaloPprPhos4ShapeCollection::Fill(const L1CaloCoolChannelId& coolId, const unsigned int timeSlice, const unsigned int rodHeaderStep, const int adc) {
   ShapesMap_itr itr = GetMapIterator(coolId);
   if (itr == m_signalShapes->end()) {
      *m_log << MSG::WARNING << "[Fill] no iterator returned for 0x" << CoolIdToString(coolId) << endmsg;
      return StatusCode::FAILURE;
   }
   return itr->second->Fill(timeSlice, rodHeaderStep, adc);
}


StatusCode L1CaloPprPhos4ShapeCollection::Fill(const L1CaloCoolChannelId& coolId, const unsigned int rodHeaderStep, const std::vector<int> adc) {
   ShapesMap_itr itr = GetMapIterator(coolId);
   if (itr == m_signalShapes->end()) {
      *m_log << MSG::WARNING << "[Fill] no iterator returned for 0x" << CoolIdToString(coolId) << endmsg;
      return StatusCode::FAILURE;
   }
   return itr->second->Fill(rodHeaderStep, adc);
}

StatusCode L1CaloPprPhos4ShapeCollection::Finalize ATLAS_NOT_REENTRANT(void) {

   // want to store the profiles and histograms for each cool id into
   // a TTree, which will be output to a file.

   *m_log << MSG::INFO << "[Finalize] In Finalize()" << endmsg;

   // open the output file
   std::string filename;
   if (m_runNumberStr.empty()) {
      filename = (m_outputFileDirName + '/' + m_outputFileName + '.' + m_outputFileNamePostfix);
   }
   else {
      filename = (m_outputFileDirName + '/' + m_outputFileName + '_' + m_runNumberStr + '.' + m_outputFileNamePostfix);
   }

   *m_log << MSG::INFO << "[Finalize] Opening output file: " << filename << endmsg;
   TFile* outputFile = new TFile(filename.c_str(), "RECREATE");
   if (!outputFile->IsOpen()) {
      *m_log << MSG::ERROR << "[Finalize]: Could not open file for TTree output: " << filename << endmsg;

      delete outputFile; outputFile = 0;
      return StatusCode::FAILURE;
   }


   *m_log << MSG::INFO << "[Finalize] Create TTree" << endmsg;

   TTree* tree = new TTree("phos4signalShapes", "phos4signalShapes");

   tree->Branch(m_coolIdName.c_str(), &m_coolId, std::string(m_coolIdName + "/i").c_str());

   tree->Branch(m_isEnabledName.c_str(), &m_isEnabled, std::string(m_isEnabledName + "/O").c_str());
   tree->Branch(m_isValidName.c_str(), &m_isValid, std::string(m_isValidName + "/O").c_str());
   tree->Branch(m_isProcessedName.c_str(), &m_isProcessed, std::string(m_isProcessedName + "/O").c_str());

   tree->Branch(m_rawSignalShapeName.c_str(), &m_rawSignalShape);
   tree->Branch(m_rawMaxPeakBinName.c_str(), &m_rawMaxPeakBin, std::string(m_rawMaxPeakBinName + "/i").c_str());
   tree->Branch(m_rawMaxPeakValueName.c_str(), &m_rawMaxPeakValue, std::string(m_rawMaxPeakValueName + "/F").c_str());
   tree->Branch(m_rawMaxFullDelayDataName.c_str(), &m_rawMaxFullDelayData, std::string(m_rawMaxFullDelayDataName + "/i").c_str());
   tree->Branch(m_rawFitPeakBinName.c_str(), &m_rawFitPeakBin, std::string(m_rawFitPeakBinName + "/i").c_str());
   tree->Branch(m_rawFitPeakValueName.c_str(), &m_rawFitPeakValue, std::string(m_rawFitPeakValueName + "/F").c_str());
   tree->Branch(m_rawFitFullDelayDataName.c_str(), &m_rawFitFullDelayData, std::string(m_rawFitFullDelayDataName + "/i").c_str());
   tree->Branch(m_rawFitPerformedName.c_str(), &m_rawFitPerformed, std::string(m_rawFitPerformedName + "/O").c_str());

   tree->Branch(m_rawFitName.c_str(), &m_rawFit);

   tree->Branch(m_pedestalMeanName.c_str(), &m_pedestalMean, std::string(m_pedestalMeanName + "/F").c_str());
   tree->Branch(m_pedestalSigmaName.c_str(), &m_pedestalSigma, std::string(m_pedestalSigmaName + "/F").c_str());

   tree->Branch(m_processedSignalShapeName.c_str(), &m_processedSignalShape);
   tree->Branch(m_processedMaxPeakBinName.c_str(), &m_processedMaxPeakBin, std::string(m_processedMaxPeakBinName + "/i").c_str());
   tree->Branch(m_processedMaxPeakValueName.c_str(), &m_processedMaxPeakValue, std::string(m_processedMaxPeakValueName + "/F").c_str());
   tree->Branch(m_processedMaxFullDelayDataName.c_str(), &m_processedMaxFullDelayData, std::string(m_processedMaxFullDelayDataName + "/i").c_str());
   tree->Branch(m_processedFitPeakBinName.c_str(), &m_processedFitPeakBin, std::string(m_processedFitPeakBinName + "/i").c_str());
   tree->Branch(m_processedFitPeakValueName.c_str(), &m_processedFitPeakValue, std::string(m_processedFitPeakValueName + "/F").c_str());
   tree->Branch(m_processedFitFullDelayDataName.c_str(), &m_processedFitFullDelayData, std::string(m_processedFitFullDelayDataName + "/i").c_str());
   tree->Branch(m_processedFitPerformedName.c_str(), &m_processedFitPerformed, std::string(m_processedFitPerformedName + "/O").c_str());

   tree->Branch(m_processedFitName.c_str(), &m_processedFit);

   tree->Branch(m_risingSlopeMeanName.c_str(), &m_risingSlopeMean, std::string(m_risingSlopeMeanName + "/F").c_str());
   tree->Branch(m_risingSlopeSigmaName.c_str(), &m_risingSlopeSigma, std::string(m_risingSlopeSigmaName + "/F").c_str());

   tree->Branch(m_errorCodeName.c_str(), &m_errorCode, std::string(m_errorCodeName + "/i").c_str());

   tree->Branch(m_currentFullDelayDataName.c_str(), &m_currentFullDelayData, std::string(m_currentFullDelayDataName + "/i").c_str());

   *m_log << MSG::INFO << "[Finalize] Looping over shapes for processing." << endmsg;

   // open a file to output each of the new FullDelayData values for the database
   std::string dbFilename_rawMax;
   std::string dbFilename_rawFit;
   std::string dbFilename_processedMax;
   std::string dbFilename_processedFit;
   if (m_runNumberStr.empty()) {
      dbFilename_rawMax = (m_outputFileDirName + '/' + m_dbFileName_rawMax + '.' + m_outDBFileNamePostfix);
      dbFilename_rawFit = (m_outputFileDirName + '/' + m_dbFileName_rawFit + '.' + m_outDBFileNamePostfix);
      dbFilename_processedMax = (m_outputFileDirName + '/' + m_dbFileName_processedMax + '.' + m_outDBFileNamePostfix);
      dbFilename_processedFit = (m_outputFileDirName + '/' + m_dbFileName_processedFit + '.' + m_outDBFileNamePostfix);
   }
   else {
      dbFilename_rawMax = (m_outputFileDirName + '/' + m_dbFileName_rawMax + '_' + m_runNumberStr + '.' + m_outDBFileNamePostfix);
      dbFilename_rawFit = (m_outputFileDirName + '/' + m_dbFileName_rawFit + '_' + m_runNumberStr + '.' + m_outDBFileNamePostfix);
      dbFilename_processedMax = (m_outputFileDirName + '/' + m_dbFileName_processedMax + '_' + m_runNumberStr + '.' + m_outDBFileNamePostfix);
      dbFilename_processedFit = (m_outputFileDirName + '/' + m_dbFileName_processedFit + '_' + m_runNumberStr + '.' + m_outDBFileNamePostfix);
   }
   std::ofstream dbFile_rawMax(dbFilename_rawMax.c_str());
   if (!dbFile_rawMax.is_open()) {
      *m_log << MSG::ERROR << "[Finalize]: Failed to open file for database output, " << dbFilename_rawMax << endmsg;
   }
   std::ofstream dbFile_rawFit(dbFilename_rawFit.c_str());
   if (!dbFile_rawFit.is_open()) {
      *m_log << MSG::ERROR << "[Finalize]: Failed to open file for database output, " << dbFilename_rawFit << endmsg;
   }
   std::ofstream dbFile_processedMax(dbFilename_processedMax.c_str());
   if (!dbFile_processedMax.is_open()) {
      *m_log << MSG::ERROR << "[Finalize]: Failed to open file for database output, " << dbFilename_processedMax << endmsg;
   }
   std::ofstream dbFile_processedFit(dbFilename_processedFit.c_str());
   if (!dbFile_processedFit.is_open()) {
      *m_log << MSG::ERROR << "[Finalize]: Failed to open file for database output, " << dbFilename_processedFit << endmsg;
   }

   ////////////////////////////////////////////////////////////////////
   // need a canvas to putput the postscript file containing
   // the raw and processed histograms
   // Adding two more canvases to output the channels
   // with errors which will be saved to a different file
   // and then concatentated to the top of the full list

   TCanvas rawCanvas("rawCanvas", "Raw Signal Shapes", 0, 0, 1024, 768);
   rawCanvas.SetBatch(true);
   // remove any pading space
   rawCanvas.SetMargin(0, 0, 0, 0);
   // divide into two pads (1 for page titles and 1 for plots)
   rawCanvas.Divide(1, 2, 0, 0);
   TVirtualPad *rawTitlePad = rawCanvas.cd(1);
   TVirtualPad *rawPlotPad = rawCanvas.cd(2);
   // remove any pading space from pads
   rawTitlePad->SetMargin(0, 0, 0, 0);
   rawPlotPad->SetMargin(0, 0, 0, 0);
   // resize the pads so that title is at the top
   const float titleBottom = 0.97;
   rawTitlePad->SetPad(0, titleBottom, 1, 1);
   rawPlotPad->SetPad(0, 0, 1, titleBottom);
   // divide bottom pad into 64 pads for plots
   const unsigned short int maxPlotsPerPage = 64;
   const unsigned short int maxPlotsPerPageX = 8;
   const unsigned short int maxPlotsPerPageY = 8;
   rawPlotPad->Divide(maxPlotsPerPageX, maxPlotsPerPageY, 0, 0);
   // Create text label for top pad
   TPaveText *rawPageTitle = new TPaveText(0, 0, 1, 1);
   rawPageTitle->SetFillColor(kWhite);
   rawPageTitle->SetBorderSize(0);

   TCanvas rawErrorCanvas("rawErrorCanvas", "Raw Signal Shapes with Errors", 0, 0, 1024, 768);
   rawErrorCanvas.SetBatch(true);
   // remove any pading space
   rawErrorCanvas.SetMargin(0, 0, 0, 0);
   // divide into two pads (1 for page titles and 1 for plots)
   rawErrorCanvas.Divide(1, 2, 0, 0);
   TVirtualPad *rawErrorTitlePad = rawErrorCanvas.cd(1);
   TVirtualPad *rawErrorPlotPad = rawErrorCanvas.cd(2);
   // remove any pading space from pads
   rawErrorTitlePad->SetMargin(0, 0, 0, 0);
   rawErrorPlotPad->SetMargin(0, 0, 0, 0);
   // resize the pads so that title is at the top
   rawErrorTitlePad->SetPad(0, titleBottom, 1, 1);
   rawErrorPlotPad->SetPad(0, 0, 1, titleBottom);
   // divide bottom pad into 64 pads for plots
   const unsigned short int maxErrorPlotsPerPage = 16;
   const unsigned short int maxErrorPlotsPerPageX = 4;
   const unsigned short int maxErrorPlotsPerPageY = 4;
   rawErrorPlotPad->Divide(maxErrorPlotsPerPageX, maxErrorPlotsPerPageY, 0, 0);
   // Create text label for top pad
   TPaveText *rawErrorPageTitle = new TPaveText(0, 0, 1, 1);
   rawErrorPageTitle->SetFillColor(kWhite);
   rawErrorPageTitle->SetBorderSize(0);

   // Counter for the number of plots in the error canvas
   // this is used to figure out how many plots are being
   // included and when to go to the next page
   unsigned short int nErrorPlots = 0;
   unsigned short int nErrorPlotPages = 0;

   TCanvas processedCanvas("processedCanvas", "Processed Signal Shapes", 0, 0, 1024, 768);
   // remove any pading space
   processedCanvas.SetMargin(0, 0, 0, 0);
   // divide into two pads (1 for page titles and 1 for plots)
   processedCanvas.Divide(1, 2, 0, 0);
   TVirtualPad *processedTitlePad = processedCanvas.cd(1);
   TVirtualPad *processedPlotPad = processedCanvas.cd(2);
   // remove any pading space from pads
   processedTitlePad->SetMargin(0, 0, 0, 0);
   processedPlotPad->SetMargin(0, 0, 0, 0);
   // resize the pads so that title is at the top
   processedTitlePad->SetPad(0, titleBottom, 1, 1);
   processedPlotPad->SetPad(0, 0, 1, titleBottom);
   // divide bottom pad into 64 pads for plots
   processedPlotPad->Divide(maxPlotsPerPageX, maxPlotsPerPageY, 0, 0);
   // Create text label for top pad
   TPaveText *processedPageTitle = new TPaveText(0, 0, 1, 1);
   processedPageTitle->SetFillColor(kWhite);
   processedPageTitle->SetBorderSize(0);

   TCanvas processedErrorCanvas("processedErrorCanvas", "Processed Signal Shapes with Errors", 0, 0, 1024, 768);
   // remove any pading space
   processedErrorCanvas.SetMargin(0, 0, 0, 0);
   // divide into two pads (1 for page titles and 1 for plots)
   processedErrorCanvas.Divide(1, 2, 0, 0);
   TVirtualPad *processedErrorTitlePad = processedErrorCanvas.cd(1);
   TVirtualPad *processedErrorPlotPad = processedErrorCanvas.cd(2);
   // remove any pading space from pads
   processedErrorTitlePad->SetMargin(0, 0, 0, 0);
   processedErrorPlotPad->SetMargin(0, 0, 0, 0);
   // resize the pads so that title is at the top
   processedErrorTitlePad->SetPad(0, titleBottom, 1, 1);
   processedErrorPlotPad->SetPad(0, 0, 1, titleBottom);
   // divide bottom pad into 64 pads for plots
   processedErrorPlotPad->Divide(maxErrorPlotsPerPageX, maxErrorPlotsPerPageY, 0, 0);
   // Create text label for top pad
   TPaveText *processedErrorPageTitle = new TPaveText(0, 0, 1, 1);
   processedErrorPageTitle->SetFillColor(kWhite);
   processedErrorPageTitle->SetBorderSize(0);

   // also want files to store the canvases in
   std::string filenameRawShapes, filenameRawErrorShapes;
   std::string filenameProcessedShapes, filenameProcessedErrorShapes;
   if (m_runNumberStr.empty()) {
      filenameRawShapes = (m_outputFileDirName + '/' + m_outRawCanvasName + '.' + m_outRawCanvasNamePostfix);
      filenameRawErrorShapes = (m_outputFileDirName + '/' + m_outRawCanvasName + "Error." + m_outRawCanvasNamePostfix);
      filenameProcessedShapes = (m_outputFileDirName + '/' + m_outProcessedCanvasName + '.' + m_outProcessedCanvasNamePostfix);
      filenameProcessedErrorShapes = (m_outputFileDirName + '/' + m_outProcessedCanvasName + "Error." + m_outProcessedCanvasNamePostfix);
   }
   else {
      filenameRawShapes = (m_outputFileDirName + '/' + m_outRawCanvasName + '_' + m_runNumberStr + '.' + m_outRawCanvasNamePostfix);
      filenameRawErrorShapes = (m_outputFileDirName + '/' + m_outRawCanvasName + "Error_" + m_runNumberStr + '.' + m_outRawCanvasNamePostfix);
      filenameProcessedShapes = (m_outputFileDirName + '/' + m_outProcessedCanvasName + '_' + m_runNumberStr + '.' + m_outProcessedCanvasNamePostfix);
      filenameProcessedErrorShapes = (m_outputFileDirName + '/' + m_outProcessedCanvasName + "Error_" + m_runNumberStr + '.' + m_outProcessedCanvasNamePostfix);
   }

   //////////////////////////////////////
   // setup summary plots for this run
   //////////////////////////////////////

   // 1D difference between currentFullDelayData and derived
   TH1I *h1f_diff_fullDelayData_rawMax = new TH1I("h1f_diff_fullDelayData_rawMax", "FullDelayData: RawMax - database;#Delta FullDelayData (ns)", 100, -50, 50);
   TH1I *h1f_diff_fullDelayData_rawFit = new TH1I("h1f_diff_fullDelayData_rawFit", "FullDelayData: RawFit - database;#Delta FullDelayData (ns)", 100, -50, 50);
   TH1I *h1f_diff_fullDelayData_processedMax = new TH1I("h1f_diff_fullDelayData_processedMax", "FullDelayData: ProcessedMax - database;#Delta FullDelayData (ns)", 100, -50, 50);
   TH1I *h1f_diff_fullDelayData_processedFit = new TH1I("h1f_diff_fullDelayData_processedFit", "FullDelayData: ProcessedFit - database;#Delta FullDelayData (ns)", 100, -50, 50);

   // 2D difference between currentFullDelayData and derived
   // EM
   TH2F_LW *h2f_diff_fullDelayData_rawMax_em = (*m_histTool)->bookPPMEmEtaVsPhi("h2f_diff_fullDelayData_rawMax_em", "EM FullDelayData: RawMax - database");
   TH2F_LW *h2f_diff_fullDelayData_rawFit_em = (*m_histTool)->bookPPMEmEtaVsPhi("h2f_diff_fullDelayData_rawFit_em", "EM FullDelayData: RawFit - database");
   TH2F_LW *h2f_diff_fullDelayData_processedMax_em = (*m_histTool)->bookPPMEmEtaVsPhi("h2f_diff_fullDelayData_processedMax_em", "EM FullDelayData: ProcessedMax - database");
   TH2F_LW *h2f_diff_fullDelayData_processedFit_em = (*m_histTool)->bookPPMEmEtaVsPhi("h2f_diff_fullDelayData_processedFit_em", "EM FullDelayData: ProcessedFit - database");
   // HAD
   TH2F_LW *h2f_diff_fullDelayData_rawMax_had = (*m_histTool)->bookPPMHadEtaVsPhi("h2f_diff_fullDelayData_rawMax_had", "HAD FullDelayData: RawMax - database");
   TH2F_LW *h2f_diff_fullDelayData_rawFit_had = (*m_histTool)->bookPPMHadEtaVsPhi("h2f_diff_fullDelayData_rawFit_had", "HAD FullDelayData: RawFit - database");
   TH2F_LW *h2f_diff_fullDelayData_processedMax_had = (*m_histTool)->bookPPMHadEtaVsPhi("h2f_diff_fullDelayData_processedMax_had", "HAD FullDelayData: ProcessedMax - database");
   TH2F_LW *h2f_diff_fullDelayData_processedFit_had = (*m_histTool)->bookPPMHadEtaVsPhi("h2f_diff_fullDelayData_processedFit_had", "HAD FullDelayData: ProcessedFit - database");

   // 2D Error Plots (just a place holder to help presentation in the summary plots
   // EM
   TH2F_LW *h2f_errorCode_dummy_em = (*m_histTool)->bookPPMEmEtaVsPhi("h2f_errorCode_dummy_em", "EM Error Code Dummy");
   // HAD
   TH2F_LW *h2f_errorCode_dummy_had = (*m_histTool)->bookPPMHadEtaVsPhi("h2f_errorCode_dummy_had", "HAD Error Code Dummy");

   // 2D ErrorCode
   TH2F_LW *h2f_errorCode_em = (*m_histTool)->bookPPMEmEtaVsPhi("h2f_errorCode_em", "EM Error Code");
   TH2F_LW *h2f_errorCode_had = (*m_histTool)->bookPPMHadEtaVsPhi("h2f_errorCode_had", "HAD Error Code");

   // 2D derived FullDelayData
   // EM
   TH2F_LW *h2f_fullDelayData_rawMax_em = (*m_histTool)->bookPPMEmEtaVsPhi("h2f_fullDelayData_rawMax_em", "EM FullDelayData: RawMax");
   TH2F_LW *h2f_fullDelayData_rawFit_em = (*m_histTool)->bookPPMEmEtaVsPhi("h2f_fullDelayData_rawFit_em", "EM FullDelayData: RawFit");
   TH2F_LW *h2f_fullDelayData_processedMax_em = (*m_histTool)->bookPPMEmEtaVsPhi("h2f_fullDelayData_processedMax_em", "EM FullDelayData: ProcessedMax");
   TH2F_LW *h2f_fullDelayData_processedFit_em = (*m_histTool)->bookPPMEmEtaVsPhi("h2f_fullDelayData_processedFit_em", "EM FullDelayData: ProcessedFit");
   // HAD
   TH2F_LW *h2f_fullDelayData_rawMax_had = (*m_histTool)->bookPPMHadEtaVsPhi("h2f_fullDelayData_rawMax_had", "HAD FullDelayData: RawMax");
   TH2F_LW *h2f_fullDelayData_rawFit_had = (*m_histTool)->bookPPMHadEtaVsPhi("h2f_fullDelayData_rawFit_had", "HAD FullDelayData: RawFit");
   TH2F_LW *h2f_fullDelayData_processedMax_had = (*m_histTool)->bookPPMHadEtaVsPhi("h2f_fullDelayData_processedMax_had", "HAD FullDelayData: ProcessedMax");
   TH2F_LW *h2f_fullDelayData_processedFit_had = (*m_histTool)->bookPPMHadEtaVsPhi("h2f_fullDelayData_processedFit_had", "HAD FullDelayData: ProcessedFit");

   // 2D current Delays
   TH2F_LW *h2f_fullDelayData_em = (*m_histTool)->bookPPMEmEtaVsPhi("h2f_fullDelayData_em", "EM FullDelayData from database");
   TH2F_LW *h2f_fullDelayData_had = (*m_histTool)->bookPPMHadEtaVsPhi("h2f_fullDelayData_had", "HAD FullDelayData from database");


   // get iterators to loop over cool ids
   ShapesMap_itr itr = this->begin();
   ShapesMap_itr itrEnd = this->end();

   StatusCode overallSc;
   for (; itr != itrEnd; ++itr) {

      L1CaloPprPhos4Shape* shape = itr->second;

      // call the finalize function for this cool id
      // so the raw signal is processed.
      StatusCode sc = shape->Finalize();
      if (sc.isFailure()) {
         overallSc = StatusCode::FAILURE;
      }

      // now fill the TTree items
      m_coolId                   = shape->GetCoolId();

      m_isEnabled                = shape->IsEnabled();
      m_isValid                  = shape->IsValidChannel();
      m_isProcessed              = shape->IsProcessed();

      m_rawSignalShape           = shape->GetRawSignalShape();
      m_rawMaxPeakBin            = shape->GetRawMaxPeakBin();
      m_rawMaxPeakValue          = shape->GetRawMaxPeakValue();
      m_rawMaxFullDelayData      = shape->GetRawMaxFullDelayData();
      m_rawFitPeakBin            = shape->GetRawFitPeakBin();
      m_rawFitPeakValue          = shape->GetRawFitPeakValue();
      m_rawFitFullDelayData      = shape->GetRawFitFullDelayData();
      m_rawFitPerformed          = shape->GetRawFitPerformed();

      m_rawFit                   = shape->GetRawFit();

      m_pedestalMean             = shape->GetPedestalMean();
      m_pedestalSigma            = shape->GetPedestalSigma();

      m_processedSignalShape     = shape->GetProcessedSignalShape();
      m_processedMaxPeakBin      = shape->GetProcessedMaxPeakBin();
      m_processedMaxPeakValue    = shape->GetProcessedMaxPeakValue();
      m_processedMaxFullDelayData = shape->GetProcessedMaxFullDelayData();
      m_processedFitPeakBin      = shape->GetProcessedFitPeakBin();
      m_processedFitPeakValue    = shape->GetProcessedFitPeakValue();
      m_processedFitFullDelayData = shape->GetProcessedFitFullDelayData();
      m_processedFitPerformed     = shape->GetProcessedFitPerformed();

      m_processedFit             = shape->GetProcessedFit();

      m_risingSlopeMean          = shape->GetRisingSlopeMean();
      m_risingSlopeSigma         = shape->GetRisingSlopeSigma();

      m_errorCode                = shape->GetErrorCode();

      m_currentFullDelayData     = shape->GetCurrentFullDelayData();

      tree->Fill();

      // output to database text file
      if (dbFile_rawMax.is_open()) {
         dbFile_rawMax << m_timingRegime << ".PprPhos4ScanResults,0x";
         dbFile_rawMax.width(8);
         dbFile_rawMax.fill('0');
         dbFile_rawMax << std::hex << itr->first << std::dec << ",";
         dbFile_rawMax << "ModuleId:unsigned:0,";
         dbFile_rawMax << "ErrorCode:unsigned:" << shape->GetErrorCode() << ",";
         dbFile_rawMax << "Histograms:string:Not yet set,";
         dbFile_rawMax << "FullDelayData:unsigned:" << shape->GetRawMaxFullDelayData() << std::endl;
      }
      if (dbFile_rawFit.is_open()) {
         dbFile_rawFit << m_timingRegime << ".PprPhos4ScanResults,0x";
         dbFile_rawFit.width(8);
         dbFile_rawFit.fill('0');
         dbFile_rawFit << std::hex << itr->first << std::dec << ",";
         dbFile_rawFit << "ModuleId:unsigned:0,";
         dbFile_rawFit << "ErrorCode:unsigned:" << shape->GetErrorCode() << ",";
         dbFile_rawFit << "Histograms:string:Not yet set,";
         dbFile_rawFit << "FullDelayData:unsigned:" << shape->GetRawFitFullDelayData() << std::endl;
      }
      if (dbFile_processedMax.is_open()) {
         dbFile_processedMax << m_timingRegime << ".PprPhos4ScanResults,0x";
         dbFile_processedMax.width(8);
         dbFile_processedMax.fill('0');
         dbFile_processedMax << std::hex << itr->first << std::dec << ",";
         dbFile_processedMax << "ModuleId:unsigned:0,";
         dbFile_processedMax << "ErrorCode:unsigned:" << shape->GetErrorCode() << ",";
         dbFile_processedMax << "Histograms:string:Not yet set,";
         dbFile_processedMax << "FullDelayData:unsigned:" << shape->GetProcessedMaxFullDelayData() << std::endl;
      }
      if (dbFile_processedFit.is_open()) {
         dbFile_processedFit << m_timingRegime << ".PprPhos4ScanResults,0x";
         dbFile_processedFit.width(8);
         dbFile_processedFit.fill('0');
         dbFile_processedFit << std::hex << itr->first << std::dec << ",";
         dbFile_processedFit << "ModuleId:unsigned:0,";
         dbFile_processedFit << "ErrorCode:unsigned:" << shape->GetErrorCode() << ",";
         dbFile_processedFit << "Histograms:string:Not yet set,";
         dbFile_processedFit << "FullDelayData:unsigned:" << shape->GetProcessedFitFullDelayData() << std::endl;
      }

      /////////////////////////////////////////////////
      ////////////////////////////////////////////////
      // put plots in canvas

      const unsigned int digiChanNum =  ((m_coolId >> 8) & 0xf) * 4 + (m_coolId & 0xf);
      // customize how the pads are presented
      TVirtualPad* rawPad = rawPlotPad->GetPad(digiChanNum + 1);
      rawPad->SetRightMargin(0.);
      rawPad->SetTopMargin(0.);
      rawPad->SetBottomMargin(0.);
      rawPad->SetLeftMargin(0.);
      if (!(digiChanNum % 8))
         rawPad->SetLeftMargin(0.1);
      if (digiChanNum >= 56)
         rawPad->SetBottomMargin(0.1);

      m_rawSignalShape->SetMinimum(0);
      m_rawSignalShape->SetMaximum(1030);

      TVirtualPad* procPad = processedPlotPad->GetPad(digiChanNum + 1);
      procPad->SetRightMargin(0.);
      procPad->SetLeftMargin(0.);
      procPad->SetBottomMargin(0.);
      procPad->SetTopMargin(0.);
      if (!(digiChanNum % 8))
         procPad->SetLeftMargin(0.1);
      if (digiChanNum >= 56)
         procPad->SetBottomMargin(0.1);

      m_processedSignalShape->SetMinimum(-30);
      m_processedSignalShape->SetMaximum(1000);

      rawPad->SetFillColor(kWhite);
      procPad->SetFillColor(kWhite);
      // color disabled (but used) channels cyan
      if (!shape->IsEnabled() && shape->IsValidChannel()) {
         rawPad->SetFillColor(kCyan);
         procPad->SetFillColor(kCyan);
      }
      // color enabled but unused channels green
      else if (!shape->IsValidChannel()) {
         rawPad->SetFillColor(kGreen);
         procPad->SetFillColor(kGreen);
      }
      // color channels with error codes red
      else if (shape->GetErrorCode() != 0) {
         rawPad->SetFillColor(kRed);
         procPad->SetFillColor(kRed);

         // if there is no signal, zoom in on the pedestal
         ChanCalibErrorCode raw_errorCode = shape->GetErrorCode();
         if (raw_errorCode.phos4ScanNoSignal()) {
            m_rawSignalShape->SetMaximum(100);
            m_rawSignalShape->SetMinimum(0);
            m_processedSignalShape->SetMaximum(70);
            m_processedSignalShape->SetMinimum(-30);
         }
      }

      rawPlotPad->cd(digiChanNum + 1);
      m_rawSignalShape->GetXaxis()->SetLabelSize(0.07);
      m_rawSignalShape->GetXaxis()->SetLabelOffset(0.01);
      m_rawSignalShape->Draw();
      processedPlotPad->cd(digiChanNum + 1);
      m_processedSignalShape->GetXaxis()->SetLabelSize(0.07);
      m_processedSignalShape->GetXaxis()->SetLabelOffset(0.01);
      m_processedSignalShape->Draw();

      // save canses to file
      if (digiChanNum == maxPlotsPerPage - 1) {
         *m_log << MSG::INFO << "[Finalize] Saving Channel Shapes to file. " << endmsg;
         // set title for the page
         std::string rawTitle = GetPadTitle(m_coolId) + " (raw)";
         std::string processedTitle = GetPadTitle(m_coolId) + " (processed)";

         // draw title on page
         rawTitlePad->cd();
         rawPageTitle->Clear();
         rawPageTitle->AddText(rawTitle.c_str());
         rawPageTitle->Draw();

         // update canvas
         rawCanvas.Update();

         // draw title on page
         processedTitlePad->cd();
         processedPageTitle->Clear();
         processedPageTitle->AddText(processedTitle.c_str());
         processedPageTitle->Draw();

         processedCanvas.Update();

         // if this is the first module in the first crate we open the files.
         if ( ((m_coolId >> 24) & 0xf) == 0 && ((m_coolId >> 16) & 0xf) == 0) {
            rawCanvas.SaveAs( std::string(filenameRawShapes + "(").c_str() );
            processedCanvas.SaveAs( std::string(filenameProcessedShapes + "(").c_str() );

            rawPlotPad->Clear();
            rawPlotPad->SetMargin(0, 0, 0, 0);
            rawPlotPad->Divide(8, 8, 0, 0);
            processedPlotPad->Clear();
            processedPlotPad->SetMargin(0, 0, 0, 0);
            processedPlotPad->Divide(8, 8, 0, 0);

         }
         // if this is the last module in the last crate the we close the files.
         else if (((m_coolId >> 24) & 0xf) == 7 && ((m_coolId >> 16) & 0xf) == 15) {
            rawCanvas.SaveAs( std::string(filenameRawShapes + ")").c_str() );
            processedCanvas.SaveAs( std::string(filenameProcessedShapes + ")").c_str() );
         }
         // all others just add
         else {
            rawCanvas.SaveAs( filenameRawShapes.c_str() );
            processedCanvas.SaveAs( filenameProcessedShapes.c_str() );

            rawPlotPad->Clear();
            rawPlotPad->SetMargin(0, 0, 0, 0);
            rawPlotPad->Divide(8, 8, 0, 0);
            processedPlotPad->Clear();
            processedPlotPad->SetMargin(0, 0, 0, 0);
            processedPlotPad->Divide(8, 8, 0, 0);

         }

      }// end if(digiChanNum)

      // fill the error plot canvases

      // add shapes with errors to the error summary canvases
      if (m_errorCode != 0) {

         ChanCalibErrorCode tempErrorCode(m_errorCode);

         // don't plot saturated signals
         if (!tempErrorCode.phos4ScanSignalSaturated()) {

            // if it's a Tile run and this is a Tile Channel
            if (m_isTileRun && !m_isLArRun && IsTileChannel(shape)) {

               const unsigned short int padIndex = nErrorPlots + 1;

               TVirtualPad *rawErrorPad = rawErrorPlotPad->cd(padIndex);
               TVirtualPad *procErrorPad = processedErrorPlotPad->cd(padIndex);
               rawErrorPad->SetFillColor(kWhite);
               procErrorPad->SetFillColor(kWhite);
               // color disabled (but used) channels cyan
               if (!shape->IsEnabled() && shape->IsValidChannel()) {
                  rawErrorPad->SetFillColor(kCyan);
                  procErrorPad->SetFillColor(kCyan);
               }
               // color enabled but unused channels green
               else if (!shape->IsValidChannel()) {
                  rawErrorPad->SetFillColor(kGreen);
                  procErrorPad->SetFillColor(kGreen);
               }
               // color channels with error codes red
               else {
                  rawErrorPad->SetFillColor(kRed);
                  procErrorPad->SetFillColor(kRed);
               }

               rawErrorPad->cd();
               m_rawSignalShape->Draw();

               procErrorPad->cd();
               m_processedSignalShape->Draw();

               nErrorPlots++;
            }
            // if it's a LAr run and this is a LAr Channel
            else if ( m_isLArRun && !m_isTileRun && IsLArChannel(shape)) {

               const unsigned short int padIndex = nErrorPlots + 1;

               TVirtualPad *rawErrorPad = rawErrorPlotPad->cd(padIndex);
               TVirtualPad *procErrorPad = processedErrorPlotPad->cd(padIndex);
               rawErrorPad->SetFillColor(kWhite);
               procErrorPad->SetFillColor(kWhite);
               // color disabled (but used) channels cyan
               if (!shape->IsEnabled() && shape->IsValidChannel()) {
                  rawErrorPad->SetFillColor(kCyan);
                  procErrorPad->SetFillColor(kCyan);
               }
               // color enabled but unused channels green
               else if (!shape->IsValidChannel()) {
                  rawErrorPad->SetFillColor(kGreen);
                  procErrorPad->SetFillColor(kGreen);
               }
               // color channels with error codes red
               else {
                  rawErrorPad->SetFillColor(kRed);
                  procErrorPad->SetFillColor(kRed);
               }

               rawErrorPad->cd();
               m_rawSignalShape->Draw();

               procErrorPad->cd();
               m_processedSignalShape->Draw();

               nErrorPlots++;
            }
         }
      }// end if(errorCode)

      // Save the output file if the number of plots has reached
      // the maximum
      if (nErrorPlots == maxErrorPlotsPerPage) {

         *m_log << MSG::INFO << "[Finalize] Saving Error Shapes to file. " << endmsg;

         // set title for the page
         std::string rawTitle = "Failed Channels (raw)";
         std::string processedTitle = "Failed Channels (processed)";

         // draw title on page
         rawErrorTitlePad->cd();
         rawErrorPageTitle->Clear();
         rawErrorPageTitle->AddText(rawTitle.c_str());
         rawErrorPageTitle->Draw();

         // update canvas
         rawErrorCanvas.Update();

         // draw title on page
         processedErrorTitlePad->cd();
         processedErrorPageTitle->Clear();
         processedErrorPageTitle->AddText(processedTitle.c_str());
         processedErrorPageTitle->Draw();

         processedErrorCanvas.Update();

         // if this is the first page open a new file
         if ( nErrorPlotPages == 0 ) {
            rawErrorCanvas.SaveAs( std::string(filenameRawErrorShapes + "(").c_str() );
            processedErrorCanvas.SaveAs( std::string(filenameProcessedErrorShapes + "(").c_str() );

            rawErrorPlotPad->Clear();
            rawErrorPlotPad->SetMargin(0, 0, 0, 0);
            rawErrorPlotPad->Divide(maxErrorPlotsPerPageX, maxErrorPlotsPerPageX, 0, 0);
            processedErrorPlotPad->Clear();
            processedErrorPlotPad->SetMargin(0, 0, 0, 0);
            processedErrorPlotPad->Divide(maxErrorPlotsPerPageX, maxErrorPlotsPerPageX, 0, 0);

         }
         // all others just add
         else {
            rawErrorCanvas.SaveAs( filenameRawErrorShapes.c_str() );
            processedErrorCanvas.SaveAs( filenameProcessedErrorShapes.c_str() );

            rawErrorPlotPad->Clear();
            rawErrorPlotPad->SetMargin(0, 0, 0, 0);
            rawErrorPlotPad->Divide(maxErrorPlotsPerPageX, maxErrorPlotsPerPageX, 0, 0);
            processedErrorPlotPad->Clear();
            processedErrorPlotPad->SetMargin(0, 0, 0, 0);
            processedErrorPlotPad->Divide(maxErrorPlotsPerPageX, maxErrorPlotsPerPageX, 0, 0);
         }
         // Note: These files will be closed after the LOOP over the iterators ends
         nErrorPlots = 0;
         nErrorPlotPages++;
      }// end if(nErrorPlots)


      /////////////////////////
      // Fill Summary Plots
      /////////////////////////

      // 1D plots
      const int diffRawMax = (int)m_rawMaxFullDelayData - (int)m_currentFullDelayData;
      const int diffRawFit = (int)m_rawFitFullDelayData - (int)m_currentFullDelayData;
      const int diffProcessedMax = (int)m_processedMaxFullDelayData - (int)m_currentFullDelayData;
      const int diffProcessedFit = (int)m_processedFitFullDelayData - (int)m_currentFullDelayData;
      // only fill if there was no error
      if (m_errorCode == 0) {
         h1f_diff_fullDelayData_rawMax->Fill(diffRawMax);
         h1f_diff_fullDelayData_rawFit->Fill(diffRawFit);
         h1f_diff_fullDelayData_processedMax->Fill(diffProcessedMax);
         h1f_diff_fullDelayData_processedFit->Fill(diffProcessedFit);
      }

      float eta = shape->GetEta();
      float phi = shape->GetPhi();

      // 2D plots
      if (shape->GetLayer() == L1CaloPprPhos4Shape::EM) {

         // error code
         int error = 0;
         if (m_errorCode == 0x12) // no signal
            error = 1;
         else if (m_errorCode == 0x402) // saturated signal
            error = 2;
         else if (m_errorCode == 0x22) // bad signal shape
            error = 3;
         if (error > 0)
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_errorCode_em, eta, phi, error);

         // skip filling if there is an error
         if (error == 0) {
            // diff delay
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_diff_fullDelayData_rawMax_em, eta, phi, diffRawMax);
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_diff_fullDelayData_rawFit_em, eta, phi, diffRawFit);
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_diff_fullDelayData_processedMax_em, eta, phi, diffProcessedMax);
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_diff_fullDelayData_processedFit_em, eta, phi, diffProcessedFit);

            // derived delays
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_fullDelayData_rawMax_em, eta, phi, m_rawMaxFullDelayData);
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_fullDelayData_rawFit_em, eta, phi, m_rawFitFullDelayData);
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_fullDelayData_processedMax_em, eta, phi, m_processedMaxFullDelayData);
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_fullDelayData_processedFit_em, eta, phi, m_processedFitFullDelayData);

            // current delay
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_fullDelayData_em, eta, phi, m_currentFullDelayData);
         }// end if(error)
         // for the diff plots, I want the bins with errors to appear white
         // to do this I falsely fill them with large negative values
         else {
            int artificialDelay = -999;
            // diff delay
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_diff_fullDelayData_rawMax_em, eta, phi, artificialDelay);
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_diff_fullDelayData_rawFit_em, eta, phi, artificialDelay);
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_diff_fullDelayData_processedMax_em, eta, phi, artificialDelay);
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_diff_fullDelayData_processedFit_em, eta, phi, artificialDelay);

            // error plot
            int artificialError = 10;
            (*m_histTool)->fillPPMEmEtaVsPhi(h2f_errorCode_dummy_em, eta, phi, artificialError);
         }
      }
      else if (shape->GetLayer() == L1CaloPprPhos4Shape::HAD) {

         // error code
         int error = 0;
         if (m_errorCode == 0x12) // no signal
            error = 1;
         else if (m_errorCode == 0x402) // saturated signal
            error = 2;
         else if (m_errorCode == 0x22) // bad signal shape
            error = 3;
         if (error > 0)
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_errorCode_had, eta, phi, error);

         // skip filling if there is an error
         if (error == 0) {
            // diff delays
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_diff_fullDelayData_rawMax_had, eta, phi, diffRawMax);
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_diff_fullDelayData_rawFit_had, eta, phi, diffRawFit);
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_diff_fullDelayData_processedMax_had, eta, phi, diffProcessedMax);
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_diff_fullDelayData_processedFit_had, eta, phi, diffProcessedFit);

            // derived delays
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_fullDelayData_rawMax_had, eta, phi, m_rawMaxFullDelayData);
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_fullDelayData_rawFit_had, eta, phi, m_rawFitFullDelayData);
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_fullDelayData_processedMax_had, eta, phi, m_processedMaxFullDelayData);
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_fullDelayData_processedFit_had, eta, phi, m_processedFitFullDelayData);

            // current delay
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_fullDelayData_had, eta, phi, m_currentFullDelayData);
         }// end if(error)
         // for the diff plots, I want the bins with errors to appear white
         // to do this I falsely fill them with large negative values
         else {
            int artificialDelay = -999;

            // diff delays
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_diff_fullDelayData_rawMax_had, eta, phi, artificialDelay);
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_diff_fullDelayData_rawFit_had, eta, phi, artificialDelay);
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_diff_fullDelayData_processedMax_had, eta, phi, artificialDelay);
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_diff_fullDelayData_processedFit_had, eta, phi, artificialDelay);

            // error plot
            int artificialError = 10;
            (*m_histTool)->fillPPMHadEtaVsPhi(h2f_errorCode_dummy_had, eta, phi, artificialError);
         }
      }

   }// end for(itr)


   // Close error plots
   // if there are earlier pages, end the file
   if (nErrorPlotPages > 0) {

      *m_log << MSG::INFO << "[Finalize] Saving Last Error Shapes to file. " << endmsg;

      // set title for the page
      std::string rawTitle = "Failed Channels (raw)";
      std::string processedTitle = "Failed Channels (processed)";

      // draw title on page
      rawErrorTitlePad->cd();
      rawErrorPageTitle->Clear();
      rawErrorPageTitle->AddText(rawTitle.c_str());
      rawErrorPageTitle->Draw();

      // update canvas
      rawErrorCanvas.Update();

      // draw title on page
      processedErrorTitlePad->cd();
      processedErrorPageTitle->Clear();
      processedErrorPageTitle->AddText(processedTitle.c_str());
      processedErrorPageTitle->Draw();

      processedErrorCanvas.Update();

      rawErrorCanvas.SaveAs( std::string(filenameRawErrorShapes + ")").c_str() );
      processedErrorCanvas.SaveAs( std::string(filenameProcessedErrorShapes + ")").c_str() );


      // append Error histograms to the top of the signal shape files
      MergeErrorAndShapes(filenameRawErrorShapes, filenameRawShapes);
      MergeErrorAndShapes(filenameProcessedErrorShapes, filenameProcessedShapes);

   }
   // if there is only one page just save it
   else if (nErrorPlots != 0 ) {

      *m_log << MSG::INFO << "[Finalize] Saving Single Page of Error Shapes to file. " << endmsg;

      // set title for the page
      std::string rawTitle = "Failed Channels (raw)";
      std::string processedTitle = "Failed Channels (processed)";

      // draw title on page
      rawErrorTitlePad->cd();
      rawErrorPageTitle->Clear();
      rawErrorPageTitle->AddText(rawTitle.c_str());
      rawErrorPageTitle->Draw();

      // update canvas
      rawErrorCanvas.Update();

      // draw title on page
      processedErrorTitlePad->cd();
      processedErrorPageTitle->Clear();
      processedErrorPageTitle->AddText(processedTitle.c_str());
      processedErrorPageTitle->Draw();

      processedErrorCanvas.Update();

      rawErrorCanvas.SaveAs( filenameRawErrorShapes.c_str() );
      processedErrorCanvas.SaveAs( filenameProcessedErrorShapes.c_str() );

      // append Error histograms to the top of the signal shape files
      MergeErrorAndShapes(filenameRawErrorShapes, filenameRawShapes);
      MergeErrorAndShapes(filenameProcessedErrorShapes, filenameProcessedShapes);
   }

   //gZip the postscript files
   gzipFile(filenameRawShapes);
   gzipFile(filenameProcessedShapes);

   // close text file
   if (dbFile_rawMax.is_open()) dbFile_rawMax.close();
   if (dbFile_rawFit.is_open()) dbFile_rawFit.close();
   if (dbFile_processedMax.is_open()) dbFile_processedMax.close();
   if (dbFile_processedFit.is_open()) dbFile_processedFit.close();

   *m_log << MSG::INFO << "[Finalize] Writing TTree." << endmsg;

   tree->Write();

   // Also want to store the Run Number
   TTree* runTree = new TTree("runInfo", "runInfo");

   runTree->Branch(m_runNumberName.c_str(), &m_runNumber, std::string(m_runNumberName + "/i").c_str());

   // m_runNumber was set during the run so we can go ahead and fill it.
   runTree->Fill();

   runTree->Write();

   outputFile->Close();

//    delete rawPageTitle;
//    rawPageTitle = 0;
//
//    delete processedPageTitle;
//    processedPageTitle = 0;

   ///////////////////////////
   // Save and Draw summary plots
   ///////////////////////////
   *m_log << MSG::INFO << "Recording Summary Plots." << endmsg;

   std::string summaryRootFile, summaryPsFile;
   if (m_runNumberStr.empty()) {
      summaryRootFile = (m_outputFileDirName + '/' + m_outputSummaryPlotsFileName + '.' + m_outputSummaryPlotsPostfix);
      summaryPsFile = (m_outputFileDirName + '/' + m_outputSummaryPlotsFileName + '.' + m_outputSummaryCanvasPostfix);
   }
   else {
      summaryRootFile = (m_outputFileDirName + '/' + m_outputSummaryPlotsFileName + '_' + m_runNumberStr + '.' + m_outputSummaryPlotsPostfix);
      summaryPsFile = (m_outputFileDirName + '/' + m_outputSummaryPlotsFileName + '_' + m_runNumberStr + '.' + m_outputSummaryCanvasPostfix);
   }

   // Open ROOT file
   TFile *summaryFile = new TFile(summaryRootFile.c_str(), "RECREATE");
   summaryFile->cd();

   // turn off plot titles for summary plots
   gStyle->SetOptTitle(0);

   // Create canvas for postscript file
   TCanvas *summaryCanvas = new TCanvas("summaryCanvas", "Summary Plots", 0, 0, 800, 600);
   summaryCanvas->SetBatch(true);
   summaryCanvas->cd(1)->SetMargin(0, 0, 0, 0);
   // divide into Title pad and content pad
   summaryCanvas->Divide(1, 2, 0, 0);
   TVirtualPad *titlePad = summaryCanvas->cd(1);
   titlePad->SetPad(0, titleBottom, 1, 1);
   TVirtualPad *contentPad = summaryCanvas->cd(2);
   contentPad->SetPad(0, 0, 1, titleBottom);

   TPaveText *titlePave = new TPaveText(0, 0, 1, 1);
   titlePave->SetFillColor(kWhite);
   titlePave->SetBorderSize(kWhite);
   titlePad->cd();
   titlePave->Draw();

   // error code

   contentPad->Clear();
   contentPad->Divide(1, 2, 0, 0);
   contentPad->cd(1)->SetMargin(0.1, 0., 0. , 0.);
   contentPad->cd(2)->SetMargin(0.1, 0., 0.1, 0.);

   contentPad->cd(1);
   SaveEtaPhiHisto(h2f_errorCode_em, 1, 4, 3)->Draw("col");
   contentPad->cd(2);
   SaveEtaPhiHisto(h2f_errorCode_had, 1, 4, 3)->Draw("col");

   titlePave->Clear();
   titlePave->AddText("ErrorCode: EM(top) HAD(bottom) Color Code: no signal(blue), sat signal(green), bad signal(red) ");

   summaryCanvas->Update();
   summaryCanvas->SaveAs( std::string(summaryPsFile + "(").c_str() );

   // current delays

   // EM
   contentPad->Clear();
   contentPad->Divide(2, 2, 0, 0);
   contentPad->cd(1)->SetMargin(0.1, 0. , 0. , 0. );
   contentPad->cd(2)->SetMargin(0. , 0.1, 0. , 0. );
   contentPad->cd(3)->SetMargin(0.1, 0. , 0.1, 0. );
   contentPad->cd(4)->SetMargin(0. , 0.1, 0.1, 0. );

   contentPad->cd(1);
   SaveEtaPhiHisto(h2f_fullDelayData_rawMax_em, 0, 300, 60)->Draw("col");
   contentPad->cd(2);
   SaveEtaPhiHisto(h2f_fullDelayData_rawFit_em, 0, 300, 60)->Draw("colz");
   contentPad->cd(3);
   SaveEtaPhiHisto(h2f_fullDelayData_processedMax_em, 0, 300, 60)->Draw("col");
   contentPad->cd(4);
   SaveEtaPhiHisto(h2f_fullDelayData_processedFit_em, 0, 300, 60)->Draw("colz");

   titlePave->Clear();
   titlePave->AddText("EM Derived FullDelayData: RawMax(top-left) RawFit(top-right) ProcessedMax(bottom-left) ProcessedFit(bottom-right)");

   summaryCanvas->Update();
   summaryCanvas->SaveAs( summaryPsFile.c_str() );

   // HAD
   contentPad->Clear();
   contentPad->Divide(2, 2, 0, 0);
   contentPad->cd(1)->SetMargin(0.1, 0. , 0. , 0. );
   contentPad->cd(2)->SetMargin(0. , 0.1, 0. , 0. );
   contentPad->cd(3)->SetMargin(0.1, 0. , 0.1, 0. );
   contentPad->cd(4)->SetMargin(0. , 0.1, 0.1, 0. );

   contentPad->cd(1);
   SaveEtaPhiHisto(h2f_fullDelayData_rawMax_had, 0, 300, 60)->Draw("col");
   contentPad->cd(2);
   SaveEtaPhiHisto(h2f_fullDelayData_rawFit_had, 0, 300, 60)->Draw("colz");
   contentPad->cd(3);
   SaveEtaPhiHisto(h2f_fullDelayData_processedMax_had, 0, 300, 60)->Draw("col");
   contentPad->cd(4);
   SaveEtaPhiHisto(h2f_fullDelayData_processedFit_had, 0, 300, 60)->Draw("colz");

   titlePave->Clear();
   titlePave->AddText("HAD Derived FullDelayData: RawMax(top-left) RawFit(top-right) ProcessedMax(bottom-left) ProcessedFit(bottom-right)");

   summaryCanvas->Update();
   summaryCanvas->SaveAs( summaryPsFile.c_str() );

   // current delays EM & HAD
   contentPad->Clear();
   contentPad->Divide(1, 2, 0, 0);
   contentPad->cd(1)->SetMargin(0.1, 0.1, 0. , 0.);
   contentPad->cd(2)->SetMargin(0.1, 0.1, 0.1, 0.);

   contentPad->cd(1);
   SaveEtaPhiHisto(h2f_fullDelayData_em, 0, 300, 60)->Draw("colz");
   contentPad->cd(2);
   SaveEtaPhiHisto(h2f_fullDelayData_had, 0, 300, 60)->Draw("colz");

   titlePave->Clear();
   titlePave->AddText("FullDelayData From Database: EM(top) HAD(bottom)");

   summaryCanvas->Update();
   summaryCanvas->SaveAs( summaryPsFile.c_str() );

   // difference of delays

   // EM
   contentPad->Clear();
   contentPad->Divide(2, 2, 0, 0);
   contentPad->cd(1)->SetMargin(0.1, 0. , 0.01, 0. );
   contentPad->cd(2)->SetMargin(0. , 0.1, 0.01, 0. );
   contentPad->cd(3)->SetMargin(0.1, 0. , 0.1, 0. );
   contentPad->cd(4)->SetMargin(0. , 0.1, 0.1, 0. );

   contentPad->cd(1);
   SaveEtaPhiHisto(h2f_errorCode_dummy_em)->Draw("box");
   SaveEtaPhiHisto(h2f_diff_fullDelayData_rawMax_em)->Draw("col same");
   contentPad->cd(2);
   SaveEtaPhiHisto(h2f_errorCode_dummy_em)->Draw("box");
   SaveEtaPhiHisto(h2f_diff_fullDelayData_rawFit_em)->Draw("colz same");
   contentPad->cd(3);
   SaveEtaPhiHisto(h2f_errorCode_dummy_em)->Draw("box");
   SaveEtaPhiHisto(h2f_diff_fullDelayData_processedMax_em)->Draw("col same");
   contentPad->cd(4);
   SaveEtaPhiHisto(h2f_errorCode_dummy_em)->Draw("box");
   SaveEtaPhiHisto(h2f_diff_fullDelayData_processedFit_em)->Draw("colz same");

   titlePave->Clear();
   titlePave->AddText("EM FullDelayData (derived-database): RawMax(top-left) RawFit(top-right) ProcessedMax(bottom-left) ProcessedFit(bottom-right)");

   summaryCanvas->Update();
   summaryCanvas->SaveAs( summaryPsFile.c_str() );

   // HAD
   contentPad->Clear();
   contentPad->Divide(2, 2, 0, 0);
   contentPad->cd(1)->SetMargin(0.1, 0. , 0.01, 0. );
   contentPad->cd(2)->SetMargin(0. , 0.1, 0.01, 0. );
   contentPad->cd(3)->SetMargin(0.1, 0. , 0.1, 0. );
   contentPad->cd(4)->SetMargin(0. , 0.1, 0.1, 0. );

   contentPad->cd(1);
   SaveEtaPhiHisto(h2f_errorCode_dummy_had)->Draw("box");
   SaveEtaPhiHisto(h2f_diff_fullDelayData_rawMax_had)->Draw("col same");
   contentPad->cd(2);
   SaveEtaPhiHisto(h2f_errorCode_dummy_had)->Draw("box");
   SaveEtaPhiHisto(h2f_diff_fullDelayData_rawFit_had)->Draw("colz same");
   contentPad->cd(3);
   SaveEtaPhiHisto(h2f_errorCode_dummy_had)->Draw("box");
   SaveEtaPhiHisto(h2f_diff_fullDelayData_processedMax_had)->Draw("col same");
   contentPad->cd(4);
   SaveEtaPhiHisto(h2f_errorCode_dummy_had)->Draw("box");
   SaveEtaPhiHisto(h2f_diff_fullDelayData_processedFit_had)->Draw("colz same");

   titlePave->Clear();
   titlePave->AddText("HAD FullDelayData (derived-database): RawMax(top-left) RawFit(top-right) ProcessedMax(bottom-left) ProcessedFit(bottom-right)");

   summaryCanvas->Update();
   summaryCanvas->SaveAs( summaryPsFile.c_str() );

   // Save and Draw 1D diff delay histograms
   contentPad->Clear();
   contentPad->Divide(2, 2, 0, 0);
   contentPad->cd(1)->SetMargin(0.1, 0. , 0. , 0. );
   contentPad->cd(2)->SetMargin(0.1, 0. , 0. , 0. );
   contentPad->cd(3)->SetMargin(0.1, 0. , 0.1, 0. );
   contentPad->cd(4)->SetMargin(0.1, 0. , 0.1, 0. );

   // write to ROOT file
   h1f_diff_fullDelayData_rawMax->Write();
   h1f_diff_fullDelayData_rawFit->Write();
   h1f_diff_fullDelayData_processedMax->Write();
   h1f_diff_fullDelayData_processedFit->Write();

   // draw in canvas
   contentPad->cd(1);
   contentPad->cd(1)->SetLogy(1);
   h1f_diff_fullDelayData_rawMax->Draw();
   contentPad->cd(2);
   contentPad->cd(2)->SetLogy(1);
   h1f_diff_fullDelayData_rawFit->Draw();
   contentPad->cd(3);
   contentPad->cd(3)->SetLogy(1);
   h1f_diff_fullDelayData_processedMax->Draw();
   contentPad->cd(4);
   contentPad->cd(4)->SetLogy(1);
   h1f_diff_fullDelayData_processedFit->Draw();

   titlePave->Clear();
   titlePave->AddText("FullDelayData (derived-database): RawMax(top-left) RawFit(top-right) ProcessedMax(bottom-left) ProcessedFit(bottom-right)");

   summaryCanvas->Update();
   summaryCanvas->SaveAs( std::string(summaryPsFile + ")").c_str() );

   // close ROOT file
   summaryFile->Close();


   // copy ps to pdf
   ps2pdf(summaryPsFile);
   sleep(5);
   // gzip ps
   gzipFile(summaryPsFile);


//   delete summaryFile;
//   summaryFile = 0;
//
//   delete h1f_diff_fullDelayData_rawMax;
//   h1f_diff_fullDelayData_rawMax = 0;
//   delete h1f_diff_fullDelayData_rawFit;
//   h1f_diff_fullDelayData_rawFit = 0;
//   delete h1f_diff_fullDelayData_processedMax;
//   h1f_diff_fullDelayData_processedMax = 0;
//   delete h1f_diff_fullDelayData_processedFit;
//   h1f_diff_fullDelayData_processedFit = 0;
//
   // commented out since LW histo deconstructor is private(!!)
//    delete h2f_diff_fullDelayData_rawMax_em;
//    h2f_diff_fullDelayData_rawMax_em = 0;
//    delete h2f_diff_fullDelayData_rawFit_em;
//    h2f_diff_fullDelayData_rawFit_em = 0;
//    delete h2f_diff_fullDelayData_processedMax_em;
//    h2f_diff_fullDelayData_processedMax_em = 0;
//    delete h2f_diff_fullDelayData_processedFit_em;
//    h2f_diff_fullDelayData_processedFit_em = 0;
//
//    delete h2f_diff_fullDelayData_rawMax_had;
//    h2f_diff_fullDelayData_rawMax_had = 0;
//    delete h2f_diff_fullDelayData_rawFit_had;
//    h2f_diff_fullDelayData_rawFit_had = 0;
//    delete h2f_diff_fullDelayData_processedMax_had;
//    h2f_diff_fullDelayData_processedMax_had = 0;
//    delete h2f_diff_fullDelayData_processedFit_had;
//    h2f_diff_fullDelayData_processedFit_had = 0;



   return overallSc;
}

bool L1CaloPprPhos4ShapeCollection::IsTileChannel(L1CaloPprPhos4Shape const * const shape) const {

   const float eta = shape->GetEta();
   const float feta = fabs(eta);
   const L1CaloPprPhos4Shape::CaloLayer layer = shape->GetLayer();

   if (layer == L1CaloPprPhos4Shape::HAD) {
      if ( feta < 1.5 )
         return true;
   }

   return false;

}

bool L1CaloPprPhos4ShapeCollection::IsLArChannel(L1CaloPprPhos4Shape const * const shape) const {

   const float eta = shape->GetEta();
   const float feta = fabs(eta);
   const L1CaloPprPhos4Shape::CaloLayer layer = shape->GetLayer();

   if (layer == L1CaloPprPhos4Shape::HAD) {
      if ( feta > 1.5 && feta < 4.9)
         return true;
   }
   else if (layer == L1CaloPprPhos4Shape::EM) {
      if ( feta < 4.9)
         return true;
   }

   return false;

}


bool L1CaloPprPhos4ShapeCollection::SetChannelEtaPhiLayer(const L1CaloCoolChannelId& coolId, const float eta, const float phi, const L1CaloPprPhos4Shape::CaloLayer layer)
{
   L1CaloPprPhos4Shape *shape = GetSignalShape(coolId);
   if (shape != 0) {
      shape->SetEta(eta);
      shape->SetPhi(phi);
      shape->SetLayer(layer);
      return true;
   }
   return false;
}

StatusCode L1CaloPprPhos4ShapeCollection::SetCurrentFullDelayData(const L1CaloCoolChannelId& coolId, unsigned int delay) {
   ShapesMap_itr itr = GetMapIterator(coolId);
   if (itr == m_signalShapes->end()) {
      *m_log << MSG::WARNING << "[SetCurrentFullDelayData] no iterator returned for 0x" << CoolIdToString(coolId) << endmsg;
      return StatusCode::FAILURE;
   }

   itr->second->SetCurrentFullDelayData(delay);

   return StatusCode::SUCCESS;
}


StatusCode L1CaloPprPhos4ShapeCollection::SetPedValue(const L1CaloCoolChannelId& coolId, unsigned int value) {
   ShapesMap_itr itr = GetMapIterator(coolId);
   if (itr == m_signalShapes->end()) {
      *m_log << MSG::WARNING << "[SetPedValue] no iterator returned for 0x" << CoolIdToString(coolId) << endmsg;
      return StatusCode::FAILURE;
   }

   itr->second->SetPedValue(value);

   return StatusCode::SUCCESS;
}

StatusCode L1CaloPprPhos4ShapeCollection::SetL1aFadcSlice(const L1CaloCoolChannelId& coolId, unsigned int slice) {
   ShapesMap_itr itr = GetMapIterator(coolId);
   if (itr == m_signalShapes->end()) {
      *m_log << MSG::WARNING << "[SetL1aFadcSlice] no iterator returned for 0x" << CoolIdToString(coolId) << endmsg;
      return StatusCode::FAILURE;
   }

   itr->second->SetL1aFadcSlice(slice);

   return StatusCode::SUCCESS;
}

StatusCode L1CaloPprPhos4ShapeCollection::SetValidChannel(const L1CaloCoolChannelId& coolId, bool validity) {
   ShapesMap_itr itr = GetMapIterator(coolId);
   if (itr == m_signalShapes->end()) {
      *m_log << MSG::WARNING << "[SetValidChannel] no iterator returned for 0x" << CoolIdToString(coolId) << endmsg;
      return StatusCode::FAILURE;
   }

   itr->second->SetValidChannel(validity);

   return StatusCode::SUCCESS;
}

StatusCode L1CaloPprPhos4ShapeCollection::SetChannelEnabled(const L1CaloCoolChannelId& coolId, bool enabled) {
   ShapesMap_itr itr = GetMapIterator(coolId);
   if (itr == m_signalShapes->end()) {
      *m_log << MSG::WARNING << "[SetChannelEnabled] no iterator returned for 0x" << CoolIdToString(coolId) << endmsg;
      return StatusCode::FAILURE;
   }

   itr->second->SetEnabled(enabled);

   return StatusCode::SUCCESS;
}


bool L1CaloPprPhos4ShapeCollection::CoolIdExists(const L1CaloCoolChannelId& coolId) const
{
   // must be a PPM
   if (coolId.moduleType() != L1CaloModuleType::Ppm) {
      *m_log << MSG::WARNING << "[CoolIdExists] channel 0x" << CoolIdToString(coolId) << " has L1CaloModuleType is " << coolId.moduleType().toString() << ", but expecting PPM." << endmsg;
      return false;
   }
   // crate must be 0-7
   else if (!(coolId.crate() <= 7)) {
      *m_log << MSG::WARNING << "[CoolIdExists] channel 0x" << CoolIdToString(coolId) << " has crate is " << coolId.crate() << ", but should be 0 - 7." << endmsg;
      return false;
   }
   // module must be 0-15
   else if (!(coolId.module() <= 15)) {
      *m_log << MSG::WARNING << "[CoolIdExists] channel 0x" << CoolIdToString(coolId) << " has module is " << coolId.module() << ", but should be 0 - 15." << endmsg;
      return false;
   }
   // subModule must be 0-15
   else if (!(coolId.subModule() <= 15)) {
      *m_log << MSG::WARNING << "[CoolIdExists] channel 0x" << CoolIdToString(coolId) << " has subModule is " << coolId.subModule() << ", but should be 0 - 15." << endmsg;
      return false;
   }
   // channel must be 0-3
   else if (!(coolId.channel() <= 3)) {
      *m_log << MSG::WARNING << "[CoolIdExists] channel 0x" << CoolIdToString(coolId) << " has channel is " << coolId.channel() << ", but should be 0 - 3." << endmsg;
      return false;
   }

   return true;
}

TH2F* L1CaloPprPhos4ShapeCollection::SaveEtaPhiHisto(TH2F_LW *histo, const float min, const float max, const unsigned int contourBins) const {

   TH2F *temp = histo->getROOTHist();
   temp->SetMinimum(min);
   temp->SetMaximum(max);
   temp->SetContour(contourBins);
   temp->SetFillColor(kGray);
   temp->SetLineColor(kGray);
   temp->Write();
   return temp;
}

std::string L1CaloPprPhos4ShapeCollection::GetPadTitle(const unsigned int coolId) const {

   std::stringstream temp;

   L1CaloCoolChannelId id(coolId);

   temp << "Crate " << id.crate() << " Ppm " << id.module();

   return temp.str();

}


void L1CaloPprPhos4ShapeCollection::gzipFile(std::string& filename) const
{
   if (!gSystem->AccessPathName(filename.c_str())) {
      std::string command = "gzip -f " + filename;
      *m_log << MSG::INFO << "Running System command: " << command << endmsg;
      gSystem->Exec(command.c_str());

   }
}

void L1CaloPprPhos4ShapeCollection::ps2pdf(std::string& filename) const
{
   if (!gSystem->AccessPathName(filename.c_str())) {
      unsigned int index = filename.find("ps");
      std::string temp = filename;
      temp.replace(index, 2, "pdf");
      std::string command = "ps2pdf " + filename;
      command += " ";
      command += temp;
      *m_log << MSG::INFO << "Running System command: " << command << endmsg;
      gSystem->Exec(command.c_str());
   }
}

void L1CaloPprPhos4ShapeCollection::psmerge(std::string& file1, std::string& file2, std::string& outputfile) const
{
   if (!gSystem->AccessPathName(file1.c_str())
         && !gSystem->AccessPathName(file2.c_str())
      )
   {
      std::string command = "psmerge " + file1 + ' ' + file2 + " > " + outputfile;
      *m_log << MSG::INFO << "Running System command: " << command << endmsg;
      gSystem->Exec(command.c_str());
   }
}

void L1CaloPprPhos4ShapeCollection::mv(std::string& file1, std::string& file2) const
{
   if (!gSystem->AccessPathName(file1.c_str()) )
   {
      std::string command = "mv " + file1 + ' ' + file2;
      *m_log << MSG::INFO << "Running System command: " << command << endmsg;
      gSystem->Exec(command.c_str());
   }
}

void L1CaloPprPhos4ShapeCollection::rm(std::string& file) const
{
   if (!gSystem->AccessPathName(file.c_str()) )
   {
      std::string command = "rm " + file;
      *m_log << MSG::INFO << "Running System command: " << command << endmsg;
      gSystem->Exec(command.c_str());
   }
}

void L1CaloPprPhos4ShapeCollection::MergeErrorAndShapes(std::string& error, std::string& shapes) const
{

   // merge the error and shapes files to one file
   std::string tempFilename = "temp.ps";
   psmerge(error, shapes, tempFilename);

   // can remove the error file
   rm(error);

   // can remove the shapes file
   rm(shapes);

   // move the temp filename to the shapes filename
   mv(tempFilename, shapes);

}

std::string L1CaloPprPhos4ShapeCollection::CoolIdToString(const L1CaloCoolChannelId& coolId) const
{
   char temp[200];
   sprintf(temp, "%08x", coolId.id());
   std::string tempStr = temp;
   return tempStr;
}

