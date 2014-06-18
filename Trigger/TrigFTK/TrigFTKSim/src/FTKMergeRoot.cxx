/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKMergeRoot.h"
#include <fstream>
#include <algorithm>
#include <TFile.h>
#include <TDirectory.h>
#include <TFile.h>
#include "TrigFTKSim/FTKPatternBySector.h"
#include "TrigFTKSim/ftk_dcap.h"
#include "TrigFTKSim/FTKRootFile.h"

/* class FTKMergeRoot
 *
 * Merge patterns stored in ascii or root format
 * Class can also convert text-files or not merged
 * root files.
 *
 * Call DoMerge(int mincoverage) to merge all given input files.
 * Call DoTextExport() to export final pattern bank into ascii-format pattern file.
 * Text to root-file conversion is done by DoTextImport() within DoMerge(),
 * but can be also called manually.
 *
 * If files should be deleted after merging use:
 *   merger.SetDeleteFilesAfterMerging();
 *
 * Set the filename of temporary root file using:
 *   merger.SetTempRootFileName(filename);

 * Set minimum coverage
 *   merger.SetMinCoverage(int);
 *
 * Usage:
 *   FTKMergeRoot merger("outputfile.root");
 *   merger.AddFile("inputfile1.txt");
 *   merger.AddFile("inputfile2.root");
 *   merger.AddFiles(vectoroffilenames);
 *   merger.DoMerge(1);
 * 
 * If desired write out also ascii format:
 *   merger.DoTextExport();
 * 
 * Patterns can also be merged subsequently, e.g.:
 *   merger.AddFile("inputfile2.root");
 *   merger.DoMerge(1);
 *   merger.AddFile("inputfile3.root");
 *   merger.DoMerge(1);
 *   merger.AddFile("inputfile4.root");
 *   merger.DoMerge(2);
 * 
 */

using namespace std;

//================== class FTKMergeRoot =================


FTKMergeRoot::FTKMergeRoot(string OutFile) : FTKLogging("FTKMergeRoot") {
   SetOutputFile(OutFile);
   m_TempRootFile = "";
   m_NSubregions = 0;
   m_DeleteAfterMerging = false;
}


//___________________________________________________________________________________________ //
FTKMergeRoot::~FTKMergeRoot() {
   if ( m_InputFiles.size()>1) {
      Info("~FTKMergeRoot")<<"There are remaining input files. "
				   <<"Merging these files as well into file: "<<m_OutFile<<"\n";
      DoMerge();
   }
}


//___________________________________________________________________________________________ //
void FTKMergeRoot::SetOutputFile(std::string filename) { 
   //! Set filename of output file
   //! Do not use this function after merging patters. This may result on loss of text-input patterns.
   TString out = filename;
   if(!out.EndsWith(".root")) {
      out+=".root";
      Info("SetOutputFile")<<"Filename does not end with '.root'. Appending this necessary suffix and using output file: "
			   <<out<<endl;
   }
   m_OutFile = out.Data();
}


//___________________________________________________________________________________________ //
void FTKMergeRoot::AddFile(string filename){
   m_InputFiles.push_back(filename);
}


//___________________________________________________________________________________________ //
void FTKMergeRoot::AddFiles(vector<string> filenames){
   for ( unsigned int i = 0 ; i < filenames.size() ; i++ ){
      AddFile(filenames[i]);
   }
}


//___________________________________________________________________________________________ //
int FTKMergeRoot::DoMerge(int MinCoverage){
   //! Merge all input files into one root file
   //! Return number of merged files

   //==============================================================
   // first step:
   //    read all input files and append patterns in a big temporary
   //    root file
   //    within the root file, the patterns are ordered by regions
   //    see class FTKThinPatternsRoot
   //==============================================================
   DoTextImport();


   //==============================================================
   // second step:
   //    read all sectors from the temporary file and other
   //    root-format input files, one by one
   //    within each sector, merge the patterns, determine coverage
   //    then write the merged sector to the output file
   //==============================================================

   // open temp file for reading and open output root file
   if ( GetNInput() == 0 ) {   // nothing to do
      Info("DoMerge")<<"========== No input files given. Nothing to do.\n";
      return 0;
   }
   
   // create output file
   TFile* rootOutputFile = (TFile*)FTKRootFile::Instance()->CreateRootFile(m_OutFile.c_str());
   if(!rootOutputFile) {
      Fatal("DoMerge")
	 <<"could not create merged patterns file=\""
	 <<m_OutFile<<"\"\n";
      return 3;
   } 
      
   // looping over input files and adding to FTKRootFileChain
   FTKRootFileChain chain;
   for(int i=0;i<GetNInput();i++) {
      TString inputFileName = GetInput(i);
      if(inputFileName.EndsWith(".root")) { // files MUST end with root because of previous call DoTextImport();
	 chain.AddFile(inputFileName);
      }
   }

   // Sector Reader
   FTKPatternBySectorReader* input = new FTKPatternBySectorReader(chain);
   if(!input->GetNLayers()) { // sanity check
      Warning("DoMerge")<<"number of layers not set, output will be empty. Cannot merge patterns!\n";
      return 2;
   }
   if( input->GetContentType()!=FTKPatternBySectorBase::CONTENT_NOTMERGED ) { // nothing to do
      Info("DoMerge")<<"========== skip this step (input chain already ordered)\n";
   }
   else {  // merge all regions
      Info("DoMerge")<<"========== merge patterns (to root file)\n";
      FTKPatternBySectorWriter* patternWriter = new FTKPatternBySectorWriter(*rootOutputFile);
      patternWriter->AppendMergedPatterns(*input,MinCoverage);
      delete patternWriter; // write to disk
      patternWriter=0;
   }
   
   // cleanup
   if(rootOutputFile) {
      delete rootOutputFile;
      rootOutputFile=0;
   }
   if(input) {
      delete input;
      input=0;
   }

   // delete files if requested
   if ( m_DeleteAfterMerging ) {
      for(int i=0;i<GetNInput();i++) {
	 TString inputFileName = GetInput(i);
	 if(inputFileName.EndsWith(".root") )
	    remove(inputFileName);
      }
   }
   // reset intput files (add one output file as single remaining file).
   m_InputFiles.clear();
   m_InputFiles.push_back(m_OutFile);

   return 0;
}


//___________________________________________________________________________________________ //
void FTKMergeRoot::DoTextImport(){
   //! import text-files into temporary root files
   
   if ( !m_TempRootFile.EndsWith(".root") && m_TempRootFile != "" ) {
      Warning("DoTextImport")<<"Filename for temporary root-file must end with .root.\n";
      m_TempRootFile += ".root";
   }
   else if ( m_TempRootFile == "" ) {
      TString tempout = "textimport." + m_OutFile;
      m_TempRootFile = tempout;
   }

   TFile* tmpFile=0;
   FTKPatternBySectorWriter* patternWriter=0;

   // ============ loop over input files and dump all patterns to tmp file
   for( int i=0 ; i<GetNInput() ; i++) {
      TString inputFileName = GetInput(i);
      if(inputFileName.EndsWith(".root"))
	 continue; // nothing todo. it's already a root-file
      else if ( inputFileName=="") 
	 continue; // no file name specified
      else {  // do Text import
	 if ( !patternWriter ) { 
	    // first ascii file to be read in, then open root-file
	    Info("DoTextImport")<<"========== import text file(s) to file "<< m_TempRootFile<<"\n";
	    tmpFile=(TFile*)FTKRootFile::Instance()->CreateRootFile(m_TempRootFile.Data());
	    if(!tmpFile) {
	       Fatal("DoTextImport")
		  <<"problem to create root file=\""<<m_TempRootFile
		  <<"\" for writing imported patterns\n";
	       return;
	    } 
	    patternWriter = new FTKPatternBySectorWriter(*tmpFile);
	 }
	 
         Info("DoTextImport")
            <<"processing input["<<i<<"] = "<<inputFileName<<"\n";
         // input file is ASCII of compressed ASCII
	 ftk_dcap::istream* in = ftk_dcap::open_for_read(inputFileName.Data());
         if(!in) {
            Error("DoTextImport")<<"failed to open input file \""<<inputFileName
					 <<"\" for reading, skipping\n";
            continue;
         }
	 else {
	    int ret = patternWriter->AppendPatternsFromASCIIstream(*in);
	    delete in;
	    // delete files if requested
	    if ( ret==0 && m_DeleteAfterMerging ) {
	       remove(inputFileName.Data());
	       m_InputFiles[i] = "";
	    }
	    else if ( m_DeleteAfterMerging ){
	       Warning("DoTextImport")<<" There was an error found while importing text files. Do not delete input files."<<endl;
	    }
	 }
      }
   }
   
   // write temporary root file to disk
   if ( patternWriter ) {
      delete patternWriter; // write root-file
      patternWriter=0;
      // add file to list;
      AddFile(m_TempRootFile.Data());
   }
   //clean-up
   if(tmpFile) {
      delete tmpFile;
      tmpFile=0;
   }

}


//___________________________________________________________________________________________ //
void FTKMergeRoot::SetTempRootFileName(std::string filename){
    m_TempRootFile = TString(filename);
}


//___________________________________________________________________________________________ //
void FTKMergeRoot::DoTextExport(TString TextOutFilename, int MinCoverage){
   //! Save output file in compressed ASCII format

   Info("DoTextExport")
      <<"========== save merged patterns (text format) ==========\n";
   
   // Try to merge files, if not yet done so.
   //DoMerge();

   // re-open everything if text-output is requested
   TFile* rootOutputFile = (TFile*)FTKRootFile::Instance()->OpenRootFileReadonly(m_OutFile.c_str());
   if(!rootOutputFile) {
      Error("DoMerge") <<" can not open \""<<m_OutFile<<"\" for reading\n";
      return;
   }

   FTKPatternBySectorReader* input = new FTKPatternBySectorReader(*rootOutputFile);
   if(!input) {
      Fatal("DoTextExport")<<"do not know where to read the pattern from\n";
      return;
   } 
   
   int nSub = m_NSubregions;
   if(m_NSubregions<1) nSub=1;
   for(int iSub=0;iSub<nSub;iSub++) {
      if(m_NSubregions>0) {
	 //	 if ( TextOutFilename.EndsWith() ) {;
	 Info("DoTextExport")<<"Appending subregion and .bz2 to output filename.\n";
	 TextOutFilename += TString::Format("_sub%d.bz2",iSub);
	 //}
      }
      ftk_dcap::ostream* out = ftk_dcap::open_for_write(TextOutFilename.Data());
      if(!out) {
	 Fatal("DoTextExport")<<"could not create file \""<<TextOutFilename<<"\"\n";
	 return;
      } 
      else {
	 Info("DoTextExport")<<"writing to \""<<TextOutFilename<<"\"\n";
	 if(input->GetNLayers()==0) {
	    Warning("DoTextExport")<<"number of layers is zero\n";
	 }
	 input->WritePatternsToASCIIstream(*out,MinCoverage,iSub,nSub);
	 out->flush();
	 delete out;
      }
   }
   // cleanup
   if(input) {
      delete input; //writes output message: 
      input=0;
   }
   if(rootOutputFile) {
      delete rootOutputFile;
      rootOutputFile=0;
   }
   Info("DoTextExport")<<"Wrote patterns to ascii file: " <<TextOutFilename<<endl;
}

   

