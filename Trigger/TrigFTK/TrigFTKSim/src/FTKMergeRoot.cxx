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
#include "TrigFTKSim/FTKSSMap.h"

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


FTKMergeRoot::FTKMergeRoot(string OutFile)
   : FTKLogging("FTKMergeRoot") {
   m_OutFile_basename=OutFile;
   m_TempRootFile = "";
   m_NSubregions = 0;
   m_DeleteAfterMerging = false;
}


//___________________________________________________________________________________________ //
FTKMergeRoot::~FTKMergeRoot() {
   if ( m_InputFiles.size()>1) {
      Info("~FTKMergeRoot")<<"There are remaining input files. "
                           <<"Merging these files as well\n";
      DoMerge();
   }
}

//___________________________________________________________________________________________ //
void FTKMergeRoot::AddFile(string const &filename){
   m_InputFiles.push_back(filename);
}


//___________________________________________________________________________________________ //
void FTKMergeRoot::AddFiles(vector<string> const &filenames){
   for ( unsigned int i = 0 ; i < filenames.size() ; i++ ){
      AddFile(filenames[i]);
   }
}


//___________________________________________________________________________________________ //
int FTKMergeRoot::DoMerge(int MinCoverage,int compression,
                          FTKSSMap *checkMap,int checkTower,int checkHWmodeid) {
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

   // looping over input files and adding to FTKRootFileChain
   FTKRootFileChain chain(120);
   for(int i=0;i<GetNInput();i++) {
      TString inputFileName = GetInput(i);
      if(FTKRootFile::Instance()->IsRootFile(inputFileName)) {
	 chain.AddFile(inputFileName);
      }
   }

   // Sector Reader
   FTKPatternBySectorReader* input = FTKPatternBySectorReader::Factory(chain);
   if(!input->GetNLayers()) { // sanity check
      Warning("DoMerge")<<"number of layers not set, cannot merge patterns!\n";
      delete input;
      return 2;
   }
   if(checkMap && (checkTower>=0) && (checkHWmodeid>=0)) {
      Info("DoMerge")<<"Performing input file consistency check\n";
      input->CheckConsistency(checkMap,checkTower,checkHWmodeid);
   } else {
      Warning("DoMerge")<<"No input file consistency check "
                        <<"\n";
   }
   if((input->GetContentType()!=FTKPatternBySectorBase::CONTENT_NOTMERGED )
      &&(!MinCoverage)&&(chain.GetLength()==1)) { // nothing to do
      Info("DoMerge")
         <<"skip merging step: ordered input chain length=1, MinCoverage==0\n";
   } else {  // merge all regions
   // create output file
      // problems:
      //  (1) output file may not end with ".root"
      //  (2) old output file may be part of the input file list
      bool doRename=false;
      if(m_OutFile_rootname.Length()==0) {
         m_OutFile_rootname=m_OutFile_basename;
         if(!m_OutFile_rootname.EndsWith(".root")) {
            m_OutFile_rootname+=".root";
            Info("FTKMergeRoot")
               <<"Root output Filename does not end with '.root'."
               " changed to: "<<m_OutFile_rootname<<"\n";
         }
      }
      TString outFileName= m_OutFile_rootname;
      bool match=false;
      for(int cycle=0;cycle<1000;cycle++) {
         for(int i=0;i<GetNInput();i++) {
            if(GetInput(i)==m_OutFile_rootname) {
               match=true;
               break;
            }
         }
         if(match) {
            outFileName=TString::Format("%d_",cycle)+outFileName;
            doRename=true;
         }
      } while(match);
      TDirectory *rootOutputFile = FTKRootFile::Instance()
         ->CreateRootFile(outFileName,compression);
      if(!rootOutputFile) {
         Fatal("DoMerge")<<"could not create merged patterns file=\""
            <<rootOutputFile<<"\"\n";
         return 3;
      } 
      Info("DoMerge")<<"merge patterns to root file "<<outFileName<<"\n";
      FTKPatternBySectorWriter* patternWriter =
         FTKPatternBySectorWriter::Factory(*rootOutputFile);
      patternWriter->AppendMergedPatterns(*input,MinCoverage);
      delete patternWriter; // write to disk
      patternWriter=0;

      // keep ssmap, rmap, pmap if available. check consistency btw. maps.
      for(int i=0;i<GetNInput();i++) {
	 TString inputFileName = GetInput(i);
	 //cout<<"Writing ssmaps input: "<<inputFileName<<", to: "<<rootOutputFile->GetName() <<endl;
	 if(FTKRootFile::Instance()->IsRootFile(inputFileName)) {
	    TDirectory *inputFile = FTKRootFile::Instance()->OpenRootFileReadonly(inputFileName);
	    if ( inputFile->GetDirectory(FTKSSMap::GetDCorTSPDirname("TSP") ) ) {
	       FTKSSMap* ssmap = FTKSSMap::GetSSMapFromRootFile(inputFile,"TSP");
	       if ( ssmap->getIsOK() ) {
		  //Info("DoMerge")<<"Writing SSMap to file: "<<rootOutputFile->GetName()<<endl;
		  ssmap->WriteMapToRootFile(rootOutputFile, "TSP");
	       }
	       else 
		  Info("DoMerge")<<"No ssmap found in file: "<<inputFileName<<endl;
	    }
	    else 
	       Info("DoMerge")<<"No ssmap found in file: "<<inputFileName<<endl;
	 }
	 else {
	    Info("DoMerge")<<"No ssmap found, since these can be stored only in root-files."<<endl;
	 }
      }

      // write to disk
      delete rootOutputFile;

      // delete files if requested
      if ( m_DeleteAfterMerging ) {
         for(int i=0;i<GetNInput();i++) {
            TString inputFileName = GetInput(i);
            if(FTKRootFile::Instance()->IsRootFile(inputFileName)) {
               remove(inputFileName);
            }
         }
      }

      if(doRename) {
         rename(outFileName,m_OutFile_rootname);
         Info("DoMerge")<<"rename "<<outFileName<<" to "
                        <<m_OutFile_rootname<<"\n";
      }
      // reset intput files (add one output file as single remaining file).
      m_InputFiles.clear();
      m_InputFiles.push_back(string(m_OutFile_rootname));
   }
//   if(input) { // check not needed (coverity doesn't like it, anyway)
   delete input;
   input=0;
//   }

  return 0;
}


//___________________________________________________________________________________________ //
void FTKMergeRoot::DoTextImport(){
   TFile* tmpFile=0;
   FTKPatternBySectorWriter* patternWriter=0;

   // ============ loop over input files and dump all patterns to tmp file
   for( int i=0 ; i<GetNInput() ; i++) {
      TString inputFileName = GetInput(i);
      if(FTKRootFile::Instance()->IsRootFile(inputFileName))
	 continue; // nothing todo. it's already a root-file
      else if ( inputFileName=="") 
	 continue; // no file name specified
      else {  // do Text import
	 if ( !patternWriter ) { 
	    // first ascii file to be read in, then open root-file
	    Info("DoTextImport")<<"========== import text file(s) to file "<< m_TempRootFile<<"\n";

            //! import text-files into temporary root files
            if ( m_TempRootFile == "" ) {
               TString tempout = "textimport." + m_OutFile_basename;
               m_TempRootFile = tempout;
            }
            if ( !m_TempRootFile.EndsWith(".root") && m_TempRootFile != "" ) {
               Warning("DoTextImport")
                  <<"Filename for temporary root-file must end with .root.\n";
               m_TempRootFile += ".root";
            }

	    tmpFile=(TFile*)FTKRootFile::Instance()->CreateRootFile(m_TempRootFile.Data());
	    if(!tmpFile) {
	       Fatal("DoTextImport")
		  <<"problem to create root file=\""<<m_TempRootFile
		  <<"\" for writing imported patterns\n";
	       return;
	    } 
	    patternWriter = FTKPatternBySectorWriter::Factory(*tmpFile);
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
void FTKMergeRoot::SetTempRootFileName(std::string const &filename){
    m_TempRootFile = TString(filename);
}


//___________________________________________________________________________________________ //
void FTKMergeRoot::DoTextExport(std::string const &TextOutFilename, int /*MinCoverage*/){
   //! Save output file in compressed ASCII format

   Info("DoTextExport")
      <<"========== save merged patterns (text format) ==========\n";
   
   // Try to merge files, if not yet done so.
   //DoMerge();

   // looping over input files and adding to FTKRootFileChain
   FTKRootFileChain chain(120);
   for(int i=0;i<GetNInput();i++) {
      TString inputFileName = GetInput(i);
      if(FTKRootFile::Instance()->IsRootFile(inputFileName)) {
	 chain.AddFile(inputFileName);
      }
   }
   FTKPatternBySectorReader* input = FTKPatternBySectorReader::Factory(chain);
   if(!input) {
      Fatal("DoTextExport")<<"do not know where to read the pattern from\n";
      return;
   }
   if(input->GetContentType()!=FTKPatternBySectorBase::CONTENT_MERGED) {
      Fatal("DoTextExport")
         <<"input chain "<<input->GetSourceName()<<"is not ordered\n";
      delete input;
      return;
   }
   
   int nSub = m_NSubregions;
   vector<TString> nSubFiles;
   if(m_NSubregions<1) nSub=1;
   for(int iSub=0;iSub<nSub;iSub++) {
      TString outputName(TextOutFilename);
      if(m_NSubregions>0) {
	 //	 if ( TextOutFilename.EndsWith() ) {;
	 Info("DoTextExport")<<"Appending subregion and .bz2 to output filename.\n";
	 outputName += TString::Format("_sub%d.bz2",iSub);
	 nSubFiles.push_back(outputName);
	 //}
      }
      ftk_dcap::ostream* out = ftk_dcap::open_for_write(outputName.Data());
      if(!out) {
	 Fatal("DoTextExport")<<"could not create file \""<<outputName<<"\"\n";
	 return;
      } 
      else {
	 Info("DoTextExport")<<"writing to \""<<outputName<<"\"\n";
	 if(input->GetNLayers()==0) {
	    Warning("DoTextExport")<<"number of layers is zero\n";
	 }
	 input->WritePatternsToASCIIstream(*out,iSub,nSub);
	 out->flush();
	 delete out;
      }
   }
   // cleanup
   if(input) {
      delete input; //writes output message: 
      input=0;
   }
   if(m_NSubregions<1) {
      Info("DoTextExport")
         <<"Wrote patterns to ascii file: " <<TextOutFilename<<endl;
   } else {
      Info("DoTextExport")
         <<"Wrote patterns to ascii files:"<<endl;
      for(int iSub=0;iSub<m_NSubregions;iSub++) {
	 Info("DoTextExport")<<"  - "<<nSubFiles[iSub]<<endl;
      }
   }
}

   

