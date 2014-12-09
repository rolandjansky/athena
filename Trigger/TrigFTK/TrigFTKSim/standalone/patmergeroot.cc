/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Utility program for the ATLAS FTK [patmergeroot]
 *
 *  This program orders patterns by sector and hit numbers
 *  identical patterns are merged and their coverage is added
 *
 *  The resulting patterns are ordered by coverage within each sector
 *  If requested, another output is produced, where all
 *  patterns from all sectors are ordered by coverage.
 *
 *  Author: Stefan Schmitt, DESY
 *  Date: 2013/12/02
 *
 * Step 1: concatenate input files (NINPUTS) to one big root file (TMP_FILE)
 *    This step is performed if:
 *      NINPUTS and TMP_FILE are given
 * Step 2: merge patterns from TMP_FILE write result to OUT_ROOT
 *    This step is performed if
 *      TMP_FILE and OUT_ROOT are given
 * Step 3: write output as text file, orederd by coverage
 *    This step is performed if
 *      OUT_ROOT and OUT_FILE are given
 *
 * compression (on the fly) is handled automatically by ftk_dcap.h
 *
 *  Sample steering file:

# sample steering file for patmergeroot
####### general options with FTKSteering,FTKLogging
#
#  level 0: suppress FATAL,ERROR,WARNING,INFO,PROGRESS (no printout at all)
#  level 1: suppress ERROR,WARNING,INFO,PROGRESS
#  level 2: suppress WARNING,INFO,PROGRESS
#  level 3: suppress INFO,PROGRESS
#  level 4: suppress PROGRESS (default)
#  level 5: print everything
LOGGING_PRINT_LEVEL 4
#
#  level 0: always continue
#  level 1: stop if FATAL
#  level 2: stop if FATAL,ERROR (default)
#  level 3: stop if FATAL,ERROR,WARNING
LOGGING_ABORT_LEVEL 2
#
####### options specific to patmergeroot
#
# cut on coverage, applied to MERGED_ROOTFILE and OUT_FILE
MINCOVERAGE 2
#
# temporary file, with all patterns, prior to merging
#  if no file is given, construct name by appending .textImport.root
#  to OUT_FILE
#TEXTIMPORT_ROOTFILE patterns_tmp.root
#
# output file (root or compressed text)
#  if the file does not end with .root, then the output is text format.
#  In this case, an additional temporary file is produced,
#  unless there is only one input file which is already merged.
#  The temporary file name is constructed from the output file name 
#  by appending .root 
OUT_FILE patterns.patt.bz2
#
# compression algortith (see Root software, Compression.h)
COMPRESSION_TYPE Global 
#COMPRESSION_TYPE ZLIB 
#COMPRESSION_TYPE LZMA 
#COMPRESSION_TYPE Old 
COMPRESSION_LEVEL 1
#
# do not split output into sub-regions
#
NSUB 0
#
# input files: various input formats can be mixed
NINPUTS 3
# first file (text)
pattern1.patt
# second file (compressed text)
pattern2.patt.bz2
# third file (root format)
pattern3.root

 */

#include <iostream>
#include <unistd.h>
#include <TString.h>
#include <TFile.h>

#include "TrigFTKSim/FTKSteering.h"
#include "TrigFTKSim/FTKPatternBySector.h"
#include "TrigFTKSim/FTKRootFile.h"
#include "TrigFTKSim/ftk_dcap.h"
#include "TrigFTKSim/FTKMergeRoot.h"
#include <Compression.h>

using namespace std;

#define OUT_FILE "OUT_FILE"
#define TEXTIMPORT_ROOTFILE "TEXTIMPORT_ROOTFILE"
#define MERGED_ROOTFILE "MERGED_ROOTFILE"
#define NINPUTS "NINPUTS"
#define MINCOVERAGE "MINCOVERAGE"
#define NSUB "NSUB"
#define UNSPLIT_SECTORS "UNSPLIT_SECTORS" 
#define COMPRESSION_TYPE "COMPRESSION_TYPE" 
#define COMPRESSION_LEVEL "COMPRESSION_LEVEL" 

class PatMergeSteering : public FTKSteering {
public:
   static PatMergeSteering *Instance(void) {
      if(!fSteering) {
         fSteering=new PatMergeSteering();
         fSteering->AddStringPar(TEXTIMPORT_ROOTFILE);
         fSteering->AddStringPar(OUT_FILE);
         fSteering->AddStringPar(NINPUTS,0);
         fSteering->AddIntPar(MINCOVERAGE);
         fSteering->AddIntPar(NSUB);
         fSteering->AddIntPar(UNSPLIT_SECTORS,1,1);
         fSteering->AddStringPar(COMPRESSION_TYPE,1,"Global");
         fSteering->AddIntPar(COMPRESSION_LEVEL,1,1);
         // the VMEM_LIMIT parameter is not used but is tolerated for
         //    backwards compatibility
         fSteering->AddIntPar("VMEM_LIMIT");
         // the DELETE_FILES_AFTER_USE parameter is not used but
         //    is tolerated for backwards compatibility
         fSteering->AddIntPar("DELETE_FILES_AFTER_USE");
      }
      return fSteering;
   }
   const char *GetOutputName(void) const {
      return *(*this)[OUT_FILE];
   }
   const char *GetTextImportName(void) const {
      return *(*this)[TEXTIMPORT_ROOTFILE];
   }
   int GetNInput(void) const {
      return (*this)[NINPUTS]->GetActualSize();
   }
   const char *GetInput(int i) const {
      return (*(*this)[NINPUTS])[i];
   }
   bool HaveTextImportName(void) const {
      const char *outName=GetTextImportName();
      return outName && outName[0];
   }
   bool HaveOutputName(void) const {
      const char *outName=GetOutputName();
      return outName && outName[0];
   }
   int GetMinCoverage(void) const {
      return (*this)[MINCOVERAGE][0];
   }
   int GetNSub(void) const {
      return (*this)[NSUB][0];
   }
   int GetUnsplitSectors(void) const {
      return (*this)[UNSPLIT_SECTORS][0];
   }
   int GetCompressionLevel(void) const {
      return (*this)[COMPRESSION_LEVEL][0];
   }
   const char *GetCompressionType(void) const {
      return *(*this)[COMPRESSION_TYPE];
   }
   int GetCompression(void) const {
      TString type(GetCompressionType());
      int level=GetCompressionLevel();
      if((level>=0)&&(level<=9)) {
         if(type.EqualTo("ZLIB"))
            return ROOT::CompressionSettings(ROOT::kZLIB, level);
         if(type.EqualTo("LZMA"))
            return ROOT::CompressionSettings(ROOT::kLZMA, level);
         if(type.EqualTo("Old"))
            return ROOT::CompressionSettings(ROOT::kOldCompressionAlgo, level);
         if(type.EqualTo("Global"))
            return ROOT::CompressionSettings(ROOT::kUseGlobalSetting, level);
      }
      return -1;
   }
private:
   static PatMergeSteering *fSteering;
};

PatMergeSteering *PatMergeSteering::fSteering=0;


// ____________________________________________________________________ //
int main(int argc, char const *argv[]) {

   // ============ read steering ==================
   int error=0;
   istream *steering= &cin;
   FTKLogging logging("main");
   if(argc>1) {
      steering=ftk_dcap::open_for_read(argv[1]);
      if(!steering) {
         logging.Fatal("parse")<<"failed to open steering file \""
                               <<argv[1]<<"\"\n";
         error=3;
	 return error;
      }
   }
   if(PatMergeSteering::Instance()->Parse(*steering)<0) {
      logging.Error("parse")<<"errors found in steering\n";
      error=2;
      return error;
   }
   if(steering!= &cin) delete steering;


   // =================== do merging ======================= //

   TString outName = PatMergeSteering::Instance()->GetOutputName();
   const int nInput = PatMergeSteering::Instance()->GetNInput();
   const int mincov = PatMergeSteering::Instance()->GetMinCoverage();
   const int compression =
      PatMergeSteering::Instance()->GetCompression();
   if(compression<0) {
      logging.Error("steering")
         <<"undefined compression algorithm type="
         <<PatMergeSteering::Instance()->GetCompressionType()
         <<" level="
         <<PatMergeSteering::Instance()->GetCompressionLevel()
         <<"\n";
   }

   FTKMergeRoot merger(outName.Data());
   // set text-import name if available
   if ( PatMergeSteering::Instance()->HaveTextImportName() ) 
      merger.SetTempRootFileName(PatMergeSteering::Instance()->GetTextImportName());
   // add files to merger
   for(int i=0;i<nInput;i++) 
      merger.AddFile(PatMergeSteering::Instance()->GetInput(i));
   // do merging
   merger.SetNSubregions(PatMergeSteering::Instance()->GetNSub());
   error += merger.DoMerge(mincov,compression);
   // write out in addition ascii-file if output file did not ended with '.root'
   if ( !outName.EndsWith(".root") ) merger.DoTextExport(outName.Data());
   //done
   return error;
   
   // ====================================================== //

   
   /*
   if((PatMergeSteering::Instance()->GetUnsplitSectors()!=1)&&
      (PatMergeSteering::Instance()->GetNSub()>0)) {
      logging.Error("check")
         <<"for NSUB="<<PatMergeSteering::Instance()->GetNSub()
         <<" the parameter "<<UNSPLIT_SECTORS<<" must be set to 1"
         <<" but is set to "
         <<PatMergeSteering::Instance()->GetUnsplitSectors()<<"\n";
   }

   // count input files
   int nInputText=0,nInputRoot=0;
   if(nInput) {
      for(int i=0;i<nInput;i++) {
         TString inputFileName=PatMergeSteering::Instance()->GetInput(i);
         if(inputFileName.EndsWith(".root")) {
            nInputRoot++;
         } else {
            nInputText++;
         }
      }
   }
   
   // determine names of output files and work-flow
   bool doRootOut=false;
   bool doTextOut=false;
   bool doImport=nInputText>0;
   bool canImport=false;
   TString rootOutputName; 
   TString textOutputName; 
   TString textImportName;
   if(PatMergeSteering::Instance()->HaveOutputName()) {
      TString outName=PatMergeSteering::Instance()->GetOutputName();
      if(!outName.EndsWith(".root")) {
         textOutputName=outName;
         rootOutputName=outName+".root";
         doTextOut=true;
      } else {
         rootOutputName=outName;
         doRootOut=true;
      }
      textImportName=outName+".textImport.root";
      canImport=true;
   }
   if(PatMergeSteering::Instance()->HaveTextImportName()) {
      textImportName=PatMergeSteering::Instance()->GetTextImportName();
      canImport=true;
   }
   if(doImport && !canImport) {
      logging.Error("check")
         <<"request to import "<<nInputText
         <<" text file(s), but neither OUT_FILE nor "
         <<"TEXTIMPORT_ROOTFILE are specified\n";
   }
   if((nInputRoot>0) && !(doTextOut || doRootOut)) {
      logging.Error("check")
         <<"request to merge "<<nInputRoot
         <<" root file(s), but OUT_FILE is not specified\n";
   }
   if((doTextOut || doRootOut) && (nInputRoot==0) &&
      (!(doImport && canImport))) {
      logging.Error("check")
         <<"request to write output to "
         <<PatMergeSteering::Instance()->GetOutputName()
         <<" but no input file names are given\n";
   }
   
   if(doImport && canImport) {
      logging.Info("workplan")
         <<"STEP 1: import "<<nInputText
         <<" text "<<((nInputText==1)?"file" : "files")
         <<" to "<<textImportName<<"\n";
   }
   int nInputMerge=nInputRoot;
   if(doImport && canImport) nInputMerge +=1;
   if(doRootOut && (nInputMerge>0)) {
      logging.Info("workplan")
         <<"STEP 2: merge "<<nInputMerge
         <<" root "<<((nInputMerge==1)?"file":"files")
         <<" to "<<rootOutputName<<"\n";
   }
   if(doTextOut && (nInputMerge>0)) {
      logging.Info("workplan")
         <<"STEP 2: merge "<<nInputMerge
         <<" root files to "<<rootOutputName<<" [if required]\n";
      if(PatMergeSteering::Instance()->GetNSub()>0) {
         logging.Info("workplan")
            <<"STEP 3: export in text format to "<<textOutputName
            <<" (split into "<<PatMergeSteering::Instance()->GetNSub()
            <<" subregions)\n";
      } else {
         logging.Info("workplan")
            <<"STEP 3: export in text format to "<<textOutputName<<"\n";
      }
   }

   //==============================================================
   //
   // first step:
   //
   //    read all input files and append patterns in a big temporary
   //    root file
   //    within the root file, the patterns are ordered by regions
   //    see class FTKThinPatternsRoot
   //
   //==============================================================

   TDirectory *tmpFile=0;
   FTKPatternBySectorWriter *patternWriter=0;
   if(doImport && canImport) {
      logging.Info("step1")
         <<"========== import text file(s)\n";
      tmpFile=FTKRootFile::Instance()->CreateRootFile(textImportName);
      if(!tmpFile) {
         logging.Fatal("step1")
            <<"problem to create root file=\""<<textImportName
            <<"\" for writing imported patterns\n";
         error=3;
      } else {
         patternWriter=new FTKPatternBySectorWriter(*tmpFile);
      }
   }

   // ============ loop over input files and dump all patterns to tmp file
   if(patternWriter) {
      for(int i=0;i<nInput;i++) {
         TString inputFileName=PatMergeSteering::Instance()->GetInput(i);
         if(inputFileName.EndsWith(".root")) continue;
         logging.Info("step1")
            <<"processing input["<<i<<"] = "<<inputFileName<<"\n";

         // input file is ASCII of compressed ASCII
         ftk_dcap::istream *in=ftk_dcap::open_for_read
            (inputFileName.Data());
         if(!in) {
            logging.Error("step1")
               <<"failed to open input file \""<<inputFileName
               <<"\" for reading, skipping\n";
            continue;
         }
         patternWriter->AppendPatternsFromASCIIstream(*in);
         delete in;
      }
      // write temporary root file to disk
      delete patternWriter;
      patternWriter=0;
   }
   if(tmpFile) {
      delete tmpFile;
      tmpFile=0;
   }



   //==============================================================
   //
   // second step:
   //
   //    read all sectors from the temporary file and other
   //    root-format input files, one by one
   //    within each sector, merge the patterns, determine coverage
   //    then write the merged sector to the output file
   //
   //==============================================================

   // open temp file for reading and open output root file 
   FTKPatternBySectorReader *input=0;
   if((doRootOut || doTextOut) && (nInputMerge>0)) {
      FTKRootFileChain chain;
      if(doImport && canImport) {
         chain.AddFile(textImportName);
      }
      for(int i=0;i<nInput;i++) {
         TString inputFileName=PatMergeSteering::Instance()->GetInput(i);
         if(inputFileName.EndsWith(".root")) {
            chain.AddFile(inputFileName);
         }
      }
      input=new FTKPatternBySectorReader(chain);
   }
   TDirectory *rootOutputFile=0;
   if(input && (!doRootOut) && (input->GetContentType()!=
                                FTKPatternBySectorBase::CONTENT_NOTMERGED)) {
      logging.Info("step2")
         <<"========== skip this step (input chain already ordered)\n";
   }
   if(input && (doRootOut || (input->GetContentType()==
                              FTKPatternBySectorBase::CONTENT_NOTMERGED))) {
      logging.Info("step2")
         <<"========== merge patterns (to root file)\n";
      rootOutputFile=FTKRootFile::Instance()->CreateRootFile(rootOutputName);
      FTKPatternBySectorWriter *patternWriter=0;
      if(!rootOutputFile) {
         logging.Fatal("step2")
            <<"could not create merged patterns file=\""
            <<rootOutputName<<"\"\n";
         error=3;
      } else {
         patternWriter=new FTKPatternBySectorWriter(*rootOutputFile);
      }

      // merge all regions regions
      int min_coverage=PatMergeSteering::Instance()->GetMinCoverage();
      if(rootOutputFile) {
         if(!input->GetNLayers()) {
            logging.Warning("step2")
               <<"number of layers not set, output will be empty\n";
            error=1;
         } else {
            patternWriter->AppendMergedPatterns(*input,min_coverage);
         }
      }
      if(patternWriter) {
         delete patternWriter;
         patternWriter=0;
         //    save output file in compressed ASCII format
      }
      if(rootOutputFile) {
         delete rootOutputFile;
         rootOutputFile=0;
      }
      if(input) {
         delete input;
         input=0;
      }

      // open root output file for reading
      if(doTextOut) {
         rootOutputFile=FTKRootFile::Instance()->OpenRootFileReadonly
            (rootOutputName);
         if(!rootOutputFile) {
            logging.Error("step2")
               <<" can not open \""<<rootOutputName<<"\" for reading\n";
         } else {
            input=new FTKPatternBySectorReader(*rootOutputFile);
         }
      }
   }

   //==============================================================
   //
   // third step:
   //
   //    save output file in compressed ASCII format
   //
   //==============================================================
   if(doTextOut) { 
      logging.Info("step3")
         <<"========== save merged patterns (text format) ==========\n";
      if(!input) {
         logging.Fatal("step3")
            <<"do not know where to read the pattern from\n";
      } else {
         int nSub=PatMergeSteering::Instance()->GetNSub();
         if(nSub<1) nSub=1;
         for(int iSub=0;iSub<nSub;iSub++) {
            TString textOutputFileName(textOutputName);
            if(PatMergeSteering::Instance()->GetNSub()>0) {
               textOutputFileName += TString::Format("_sub%d.bz2",iSub);
            }
            ftk_dcap::ostream *out=
               ftk_dcap::open_for_write(std::string(textOutputFileName));
            if(!out) {
               logging.Fatal("step3")
                  <<"could not create file \""<<textOutputFileName<<"\"\n";
               error=3;
            } else {
               logging.Info("step3")
                  <<"writing to \""<<textOutputFileName<<"\"\n";
               if(input->GetNLayers()==0) {
                  logging.Warning("step3")<<"number of layers is zero\n";
                  error=1;
               }
               int min_coverage=PatMergeSteering::Instance()->GetMinCoverage();
               input->WritePatternsToASCIIstream
                  (*out,min_coverage,iSub,nSub);
               out->flush();
               delete out;
            }
         }
         if(input) {
            delete input;
            input=0;
         }
         if(rootOutputFile) {
            delete rootOutputFile;
            rootOutputFile=0;
         }
      }
   }
   return error;
   */
}
