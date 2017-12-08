/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityUtils/MonitoringFile.h"

#include <iostream>
#include <ostream>
#include <iomanip>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <TFile.h>
#include <TH1.h>
#include <TKey.h>
#include <TString.h>
#include <string>

namespace dqutils {
    
TFile *source = 0;
TFile *target = 0;
int padding = 0;
    
// define helper methods here rather then in this central MonitoringFile.h beast
int updateHists(std::string inFileName, std::string inStem, std::string outFileName = "", std::string outStem = "");
bool makeDirectories(std::string dirName);
bool makeDir(std::string dirName);
void Copy(std::string inDir, std::string outDir, std::string inHist = "", std::string outHist = "");
void CopyHist(std::string inDir, std::string outDir, std::string inHist, std::string outHist);

//main method here
void MonitoringFile::VxMon_move( std::string inFilename, bool isIncremental)
{
  std::string stream_ref = "physics_MinBias";
  int stream_check = -1;
  stream_check = inFilename.find(stream_ref);
  
  if(!isIncremental && stream_check >= 0){
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
    if (f == 0 || ! f->IsOpen()) {
        delete f;
        return;
    }
    if(f->GetSize()<1000.) {
        delete f;
        return; 
    } 
    
    bool dirExists = false;
    TString run_dir;
    TString lb_dir;
    int writeEOS = 0;
    int writeLOCAL = -1;
    bool IsFirstTime = true;
    std::string AthenaTAG;
    
    const char *AtlasVersion = getenv("AtlasVersion");
    if (!AtlasVersion){
    	delete f;
    	return;
    }
    AthenaTAG = AtlasVersion;
    std::string out_inStem;
    std::string out_EOS;
    std::string out_LOCAL;
    std::string out_outStem;

    TIter run_keys ( f->GetListOfKeys() );
    TIter lb_keys( f->GetListOfKeys() );
    TKey* key_run(0);
    TKey* key_lb(0);
    while ((key_run = dynamic_cast<TKey*>( run_keys() )) !=0 ) {
        TObject* obj_run = key_run->ReadObj();
        TDirectory* tdir_run = dynamic_cast<TDirectory*>( obj_run );
        if (tdir_run !=0 ) {
            std::string tdir_run_name( tdir_run->GetName() );
            if (tdir_run_name.find("run")!= std::string::npos ) {
                run_dir=tdir_run_name;
		out_EOS = "root://eosatlas//eos/atlas/atlascerngroupdisk/perf-idtracking/VertexMonitoring/VxMon_"+ run_dir + "_" + AthenaTAG + ".root";				    
		out_LOCAL = "VxMon_" + run_dir + "_" + AthenaTAG + ".root";
                
                lb_keys = tdir_run->GetListOfKeys();
                while ((key_lb = dynamic_cast<TKey*>( lb_keys() )) !=0 ) {
	            TObject* obj_lb = key_lb->ReadObj();
                    TDirectory* tdir_lb = dynamic_cast<TDirectory*>( obj_lb );
		    //cout << "tdir_lb " << tdir_lb << endl;
                    if (tdir_lb !=0 ) {
                        std::string tdir_lb_name( tdir_lb->GetName() );
			//cout << "tdir_lb_name " << tdir_lb_name << endl;
                        if (tdir_lb_name.find("lb")!= std::string::npos ) {
                            lb_dir=tdir_lb_name;
            
                            dirExists = f->GetDirectory(run_dir+"/"+lb_dir+"/InDetGlobal/PrimaryVertexMultiplicity");
                            if(dirExists){
				out_inStem = run_dir+"/"+lb_dir+"/InDetGlobal/PrimaryVertexMultiplicity/nVx";
                                out_outStem = "nVx_"+lb_dir;
				if (writeEOS==0){
				    writeEOS = updateHists(inFilename.c_str(),out_inStem,out_EOS,out_outStem);
				    if(writeEOS!=0 && IsFirstTime) writeLOCAL = updateHists(inFilename.c_str(),out_inStem,out_LOCAL,out_outStem);
				}
                                else{
                                    writeLOCAL = updateHists(inFilename.c_str(),out_inStem,out_LOCAL,out_outStem);
                                }
				
                                if(writeEOS != 0 && !IsFirstTime && writeLOCAL != 0){
				    delete f;
				    return;
                                }; 
                                if(writeEOS != 0 && writeLOCAL != 0){
				    delete f;
                                    return;
                                };   
                                IsFirstTime = false;
                            }
                    	}
		    }
                    else
                    {   	
                    	delete obj_lb;
                    }
                }
                
                if(writeEOS!=0 && writeLOCAL==0){
                    int return_code = system("xrdcp VxMon_" + run_dir + "_" + AthenaTAG + ".root root://eosatlas//eos/atlas/atlascerngroupdisk/perf-idtracking/VertexMonitoring/VxMon_" + run_dir + "_" + AthenaTAG + ".root");
                    if(return_code==0)remove(out_LOCAL.c_str());
		    else{
		    	delete f;
		    	return;
		    }
                }
            }
        }
        else
        {
            delete obj_run;
        }
    }

    f->Close();
    delete f;
  }
    return;
}
    
    //----------------------------------------
    // main macro
    //----------------------------------------
    int updateHists(std::string inFileName, std::string inStem, std::string outFileName, std::string outStem)
    {
        //open original file
        source = TFile::Open(inFileName.c_str());
        if (!source) {
            //std::cout << "Couldn't open input file, " << inFileName << std::endl;
            return 1;
        }//else std::cout << "opening input file, " << inFileName << std::endl;
        
        //find out whether inStem is a histogram or a directory
        bool isDir = true;
        std::string path = inStem;
        std::string hist;
        
        if (inStem[inStem.size() - 1] != '/') {
            std::string::size_type lastSlash = inStem.find_last_of('/');
            hist = inStem.substr(lastSlash + 1, inStem.size() - lastSlash - 1);
            path =  inStem.substr(0, lastSlash + 1);
            isDir = (source->FindObjectAny(hist.c_str()))->InheritsFrom("TDirectory");
            if (isDir) {
                path = inStem;
                hist.clear();
            }
        }
        
        if (path[path.size() - 1] == '/')
            path.resize(path.size() - 1);
        if (!source->GetDirectory(path.c_str())) {
            //std::cout << "path " << path << " directory doesn't exist in input file" << std::endl;
            return 1;
        }
        
        //open target file
        std::cout.fill(' ');
        if (outFileName.empty()) {
            outFileName = std::string(inFileName, 0, inFileName.find(".root"));
            outFileName += "_trimmed.root";
        }

        target = TFile::Open(outFileName.c_str(), "update");
        if (!target) {
            //std::cout << "coundn't open output file " << outFileName << std::endl;
            return 1;
        }//else std::cout << "opening output file " << outFileName << std::endl;
        
        if (outStem.empty()) outStem = inStem;
        std::string targetPath = outStem;
        std::string targetHist;
        if (!isDir) {
            std::string::size_type lastSlash = outStem.find_last_of('/');
            targetPath = outStem.substr(0, lastSlash + 1);
            targetHist = outStem.substr(lastSlash + 1, outStem.size() - lastSlash - 1);
        }
    
        if (targetPath[targetPath.size() - 1]  == '/')
            targetPath.resize(targetPath.size() - 1);      
        
	target->cd();
        if (!makeDirectories(targetPath)) {
            //std::cout << "couldn't create outStem directories in output" << std::endl;
            return 1;
        }      
        //copy relevant objects
	if(!target->IsWritable()) return 1;
        Copy(path, targetPath, hist, targetHist);
 
        delete target;
        delete source;
        return 0;
    }
    
    bool makeDirectories(std::string dirName)
    {
        bool success = true;
        
        if (!dirName.empty()) {
            
            std::string::size_type firstSlash = dirName.find('/');
            if (firstSlash == std::string::npos) {
                success = makeDir(dirName);
            } else {
                std::string subdir(dirName, 0, firstSlash);
                if (!subdir.empty())
                    success = makeDir(subdir);
                
                std::string newSubdir(dirName, firstSlash + 1, dirName.size() - firstSlash);
                success = makeDirectories(newSubdir);
            }
        }
        return success;
    }
    
    bool makeDir(std::string dirName)
    {
        padding += 3;
        std::cout << std::setw(padding) << " ";
        
        if (!gDirectory->FindKey(dirName.c_str())) {
            gDirectory->mkdir(dirName.c_str());
//            std::cout << "makeDir=" << dirName << std::endl;
        } else
//            std::cout << "object " << dirName << " already exists in directory " << gDirectory->GetName() << std::endl;
        
        padding -= 3;
        return gDirectory->cd(dirName.c_str());
    }
    
    void Copy(std::string inDir, std::string outDir, std::string inHist, std::string outHist)
    {
        padding += 3;
        
        if (!inHist.empty()) {
            CopyHist(inDir, outDir, inHist, outHist);
            
        } else {
            TDirectory *sourceDir = source->GetDirectory(inDir.c_str());
            TDirectory *targetDir = target->GetDirectory(outDir.c_str());
            
            TIter nextKey(sourceDir->GetListOfKeys());
            
            TKey *key;
            while ((key = (TKey*)nextKey())) {
                std::string keyName  = key->GetName();
                std::string className(key->GetClassName());
                
                if (className == "TDirectoryFile") {
                    std::string newInDir = inDir;
                    newInDir += '/';
                    newInDir += keyName;
                    
                    std::string newOutDir = outDir;
                    newOutDir += '/';
                    newOutDir += keyName;
                    
                    if (! targetDir->FindKey(keyName.c_str())) {
                        /*
                        std::cout << std::setw(padding) << " ";
                        std::cout << "creating Dir " << newOutDir << std::endl;
                        */
                        targetDir->mkdir(keyName.c_str());
                    }
                    /*
                    std::cout << std::setw(padding) << " ";
                    std::cout << "moving to " << newInDir << ", a " << className << std::endl;
                    */
                    Copy(newInDir, newOutDir);
                    
                } else {
                    CopyHist(inDir, outDir, keyName, keyName);
                }
            }
        }
        
        padding -= 3;
    }
    
    void CopyHist(std::string inDir, std::string outDir, std::string inHist, std::string outHist)
    {
        TDirectory *sourceDir = source->GetDirectory(inDir.c_str());
        TDirectory *targetDir = target->GetDirectory(outDir.c_str());
        
        targetDir->cd();
        TKey *key = sourceDir->FindKey(inHist.c_str());
        TObject *object = key->ReadObj();
        bool permission = true;
        
        if (targetDir->FindKey(outHist.c_str())) permission = false;
        if (permission) object->Write(outHist.c_str(), TObject::kOverwrite);
	
    }

}
