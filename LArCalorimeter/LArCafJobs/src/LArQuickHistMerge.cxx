/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//Standalone compilation:
//g++ -g -o mergetest.exe `root-config --libs --cflags` -L InstallArea/i686-slc5-gcc43-opt/lib/ -l DataQualityUtils LArHistMerge.version2.cxx

#include "TSystem.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TKey.h"
#include "TTree.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "DataQualityUtils/MonitoringFile.h" 

#define MAXSKIPPEDFILES 5
#define MAXSKIPPEDFILESFRACTION 0.1

TFile* openWithRetry(const char* path, const unsigned nTrys=3) {
  TFile* f=NULL;
  unsigned iTry=0;
  unsigned sleepSeconds=30;
  while (!f && iTry<nTrys) {
    f=TFile::Open(path,"READ");
    if (!f || !(f->IsOpen())) {
      ++iTry;
      std::cout << "WARNING: Failed to open file " << path << " on " << iTry << ". attempt." << std::endl;
      if (iTry<nTrys) {
	std::cout << "Will re-try after " << sleepSeconds << " seconds..." << std::endl;
	sleep(sleepSeconds);
	f=NULL;
	sleepSeconds*=2;
      }
      else {//no more attempts
	std::cout << "No more retrys" << std::endl;
      }
    }//end if file not open
  }// end while
  return f;
}

class histCollection {
  
public:
  
  histCollection(bool debug=false) : dbg(debug) {};
  void addDirectory(TDirectory* dir, std::string dirName);
  size_t size() {return m_data.size();};
  void addFile(TFile* in);
  void print();
  void write(TFile* out);

  void addExclusion(const std::string& dir);

private:
  class histPerDir_t {
  public:
    histPerDir_t(const std::string& nameIn, TObject* objIn, TTree* md, bool dbg=false); 
    std::string name;
    TObject* obj;
    void (*mergeMethod)(TObject* a, const TObject* b);    
  };

  struct histDir_t {
    histDir_t(TTree* mdIn): md(mdIn) {};
    std::vector<histPerDir_t> histos;
    TTree* md;
  };


private:
  bool isExcluded(const std::string& dir);
  std::map<std::string,histDir_t> m_data;
  bool dbg;
  std::vector<std::string> m_exclusion;
  
};


void histCollection::addExclusion(const std::string& dir) {
  m_exclusion.push_back(dir);
  return;
}

bool histCollection::isExcluded(const std::string& dir) {

  const size_t firstSlash=dir.find('/',1);
  if (firstSlash==std::string::npos) return false;

  std::vector<std::string>::const_iterator it=m_exclusion.begin();
  std::vector<std::string>::const_iterator it_e=m_exclusion.end();
  for(;it!=it_e;++it) {
    const std::string& excl=*it;
    const size_t s=excl.size();
    //std::cout << "Comparing " << dir << " (index "<< firstSlash <<") with " << excl << std::endl; 
    if (dir.compare(firstSlash,s,excl)==0) {
      return true;
    }
  }
  return false;
}

void histCollection::print() {
  std::map<std::string,histDir_t>::const_iterator it=m_data.begin();
  std::map<std::string,histDir_t>::const_iterator it_e=m_data.end();
  for(;it!=it_e;++it) {
    std::cout << "Dir: " << it->first << std::endl;
    const histDir_t& hd=it->second;
    std::vector<histPerDir_t>::const_iterator it1=hd.histos.begin();
    std::vector<histPerDir_t>::const_iterator it1_e=hd.histos.end();
    for (;it1!=it1_e;++it1) 
      std::cout << "\t" << it1->name << std::endl;
  }

  std::cout << "\nExclusion List: " << std::endl;
  std::vector<std::string>::const_iterator sit=m_exclusion.begin();
  std::vector<std::string>::const_iterator sit_e=m_exclusion.end();
  for(;sit!=sit_e;++sit) 
    std::cout << "\t" << *sit << std::endl;

  return;
}

template<class HIST> 
void defaultMerge(TObject* a, const TObject* b) {
  ((HIST*)a)->Add((HIST*)b);
  return;
}

void weightedAverageTH1(TObject* a, const TObject* b) {
  TH1* a1=(dynamic_cast<TH1*>(a));
  const TH1* b1=dynamic_cast<const TH1*>(b);
  if (!b1 || !a1)
    std::cout << "ERROR in weightedAverageTH1: Object not of type TH1" << std::endl;
  else
    dqutils::MonitoringFile::merge_weightedAverage(*a1,*b1);
  return;
}

void weightedAverageTH2(TObject* a, const TObject* b) {
  TH2* a1=(dynamic_cast<TH2*>(a));
  const TH2* b1=(dynamic_cast<const TH2*>(b));
  if (!b1 || !a1)
    std::cout << "ERROR in weightedAverageTH2: Object not of type TH2" << std::endl;
  else
    dqutils::MonitoringFile::merge_weightedAverage2D(*a1,*b1);
  return;
}

void weightedEff(TObject* a, const TObject* b) {
  TH1* a1=(dynamic_cast<TH1*>(a));
  const TH1* b1=(dynamic_cast<const TH1*>(b));
  if (!b1 || !a1) 
    std::cout << "ERROR in weightedEff: Object not of type TH1" << std::endl;
  else
    dqutils::MonitoringFile::merge_weightedEff(*a1,*b1);
  return;
}

void mergeRMS(TObject* a, const TObject* b) {
  TH1* a1=(dynamic_cast<TH1*>(a));
  const TH1* b1=dynamic_cast<const TH1*>(b);
   if (!b1 || !a1) 
    std::cout << "ERROR in mergeRMS: Object not of type TH1" << std::endl;
  else
    dqutils::MonitoringFile::merge_RMS(*a1,*b1);
  return;
}

void RMSpercentDeviation(TObject* a, const TObject* b) {
  TH1* a1=(dynamic_cast<TH1*>(a));
  const TH1* b1=dynamic_cast<const TH1*>(b);
  if (!b1 || !a1) 
    std::cout << "ERROR in RMSpercentDeviation: Object not of type TH1" << std::endl;
  else
    dqutils::MonitoringFile::merge_RMSpercentDeviation(*a1,*b1);
  return;
}

void perBinEffPerCent(TObject* a, const TObject* b) {
  TH1* a1=(dynamic_cast<TH1*>(a));
  const TH1* b1=dynamic_cast<const TH1*>(b);
  if (!b1 || !a1) 
    std::cout << "ERROR in getBinEffPerCent: Object not of type TH1" << std::endl;
  else
    dqutils::MonitoringFile::merge_perBinEffPerCent(*a1,*b1);
  return;
}

void lowerLB(TObject* a, const TObject* b) {
   TH1* a1=(dynamic_cast<TH1*>(a));
   const TH1* b1=dynamic_cast<const TH1*>(b);
   if (!b1 || !a1) 
     std::cout << "ERROR in lowerLB: Object not of type TH1" << std::endl;
   else
     dqutils::MonitoringFile::merge_lowerLB(*a1,*b1);
   return;
}
    

histCollection::histPerDir_t::histPerDir_t(const std::string& nameIn, TObject* objIn, TTree* md, bool dbg) : 
  name (nameIn),
  obj(objIn),
  mergeMethod(0)
{

  //Some sanity checks:
  if (!objIn) {
    std::cout << "ERROR while adding " << nameIn << ": Histogram pointer is NULL" << std::endl;
    return;
  }

  char howToMerge[256];
  char mdName[256];
  strcpy(howToMerge,"<default>");
  if (!md) {
    std::cout << "ERROR while adding " << nameIn << ": No metadata tree. Use default merging method" << std::endl;
  }
  else {
      md->SetBranchAddress("MergeMethod", howToMerge);
      md->SetBranchAddress("Name", mdName);
      unsigned nEntries = md->GetEntries();
      for (unsigned i=0;i<nEntries;++i) {
	md->GetEntry(i);
	if (name.compare(mdName)==0) break;
      }
  }

  if (dbg) std::cout << "Name:" << mdName << " mergeMethod=" << howToMerge << std::endl;
  if (NULL!=dynamic_cast<TH1*>(obj)) {
    if (!strcmp(howToMerge,"<default>")) 
      mergeMethod=&defaultMerge<TH1>;
    else if (!strcmp(howToMerge,"weightedAverage")) 
      mergeMethod=&weightedAverageTH1;
    else if (!strcmp(howToMerge,"weightedEff")) 
      mergeMethod=&weightedEff;
    else if (!strcmp(howToMerge,"mergeRMS"))
      mergeMethod=&mergeRMS;
    else if (!strcmp(howToMerge,"RMSpercentDeviation")) 
      mergeMethod=&RMSpercentDeviation;
    else if (!strcmp(howToMerge,"perBinEffPerCent"))
      mergeMethod=&perBinEffPerCent;
    else if (!strcmp(howToMerge,"lowerLB")) 
      mergeMethod=&lowerLB;

    else {
      std::cout << "ERROR: Unknown merging method (" << howToMerge << ") for object of type TH1 named " << nameIn << std::endl;
      obj=NULL;
    }
  }// end if TH1
  else if (NULL!=dynamic_cast<TH2*>(obj)) {
     if (!strcmp(howToMerge,"<default>")) 
      mergeMethod=&defaultMerge<TH2>;
    else if (!strcmp(howToMerge,"weightedAverage")) 
      mergeMethod=&weightedAverageTH2;
    else {
      std::cout << "ERROR: Unknown merging method (" << howToMerge << ") for object of type TH2 named " << nameIn << std::endl;
      obj=NULL;
    }
     
  }// end if TH2
  else {
    std::cout << "Object "<< name  << " has unkown type" << std::endl;  
    obj=NULL;
  }
}


void histCollection::addDirectory(TDirectory* dir, std::string dirName) {
  
  TIter next( dir->GetListOfKeys() );
  TKey* key;
  while((key=(TKey*)next()))  {
    const char* name=key->GetName();
    const char* classname=key->GetClassName();
    if (dbg) std::cout << "Found name " << name << ", classname=" << classname << std::endl;

    const std::string newName=dirName+"/"+name;

    if (this->isExcluded(newName)) {
      std::cout << "Path " << newName << " excluded" << std::endl;
      return;
    }

   
    if (!strncmp(classname,"TH1",3) || !strncmp(classname,"TH2",3) || !strncmp(classname,"TProfile",8)) {
      std::map<std::string,histDir_t>::iterator mIt=m_data.find("dirName");
      if (mIt==m_data.end()) { // New top-level directory
	TTree* md=(TTree*)dir->Get("metadata");
	if (!md) std::cout << "ERROR: Did not find metadata tree in directroy " << dirName << std::endl;
	mIt=m_data.insert(std::make_pair(dirName,histDir_t(md))).first;
      }
      histPerDir_t histo(name,key->ReadObj(),mIt->second.md,dbg);
      mIt->second.histos.push_back(histo);
    }
    else if (!strncmp(classname,"TDirectory",10)) {
      TObject* obj = key->ReadObj();
      TDirectory* subdir = dynamic_cast<TDirectory*>( obj );
      if (subdir) {
	this->addDirectory(subdir,newName);
      }
    }
    else if (!strcmp(classname,"TTree") && (!strcmp(name,"metadata"))) {
      //std::cout << "Found Metadata tree" << std::endl;
    }
    else {
      std::cout << "Ignored objects '" << name << "' of type "<< classname << std::endl;
    } 
  }
  return;
}


void histCollection::addFile(TFile* in) {
  std::map<std::string,histDir_t>::const_iterator it=m_data.begin();
  std::map<std::string,histDir_t>::const_iterator it_e=m_data.end();
  for(;it!=it_e;++it) {
    const std::string& dirName=it->first;
    const histDir_t& hd=it->second;
    std::vector<histPerDir_t>::const_iterator it1=hd.histos.begin();
    std::vector<histPerDir_t>::const_iterator it1_e=hd.histos.end();
    for (;it1!=it1_e;++it1) {
      const std::string fullName=dirName+"/"+it1->name;
      if (!it1->obj) {
	//std::cout << "Object " << fullName << " not properly set up. Not merged." << std::endl;
	continue;
      }
      TObject* obj=in->Get(fullName.c_str());
      if (!obj) {
	std::cout << "ERROR: Could not access path " << fullName <<" in file " << in->GetName() << std::endl;
      }
      else
	(*it1->mergeMethod)(it1->obj,obj);
    }//End loop over histograms in directory
  }//End loop over directory names
  return;
}



void histCollection::write(TFile* out) {
  unsigned nWritten=0;
  unsigned nIgnored=0;
  TDirectory* histDir=NULL;
  std::string lastDir;
  std::map<std::string,histDir_t>::const_iterator it=m_data.begin();
  std::map<std::string,histDir_t>::const_iterator it_e=m_data.end();
  for(;it!=it_e;++it) {
    const std::string fulldir=it->first + "/";
    //Create dirs if necessary
    std::string::size_type j=0,j1=0;
    while (j1!=std::string::npos) {
      do {
	j=j1+1;
	j1=fulldir.find('/',j);
	//std::cout << "Inner loop " << fulldir.substr(0,j1) << " << ==? " <<  lastDir.substr(0,j1) << std::endl;
      }
      while(j1!=std::string::npos && !fulldir.compare(0,j1,lastDir,0,j1));
      const std::string currDirAtLevel(fulldir.substr(j,j1-j));
      const std::string currFullDir(fulldir.substr(0,j1));
      const std::string currBaseDir(fulldir.substr(0,j));
      //std::cout << "Working on dir " << fulldir << " [" << currFullDir << " " << currBaseDir << " " << currDirAtLevel << std::endl;
      lastDir=currFullDir;
      out->cd(currBaseDir.c_str());
      gDirectory->mkdir(currDirAtLevel.c_str());
    }//End outer while loop - full directory created 
    //std::cout << "Done creating " << fulldir << std::endl;
    out->cd(fulldir.c_str());
    it->second.md->SetDirectory(histDir);
    it->second.md->Write(0,TObject::kOverwrite);
    std::vector<histPerDir_t>::const_iterator ith=it->second.histos.begin();
    std::vector<histPerDir_t>::const_iterator ith_e=it->second.histos.end();
    for (;ith!=ith_e;++ith) {
      if (ith->obj) {
	ith->obj->Write();
	std::cout << "Writing " <<  ith->name << " to dir " << fulldir << std::endl;
	++nWritten;
      }
      else {
	std::cout << "NOT writing " <<  ith->name << ". Invalid." << std::endl;
	++nIgnored;
      }
    }//End loop over histograms in one directory
  }//End loop over directories;
  std::cout << "Wrote " << nWritten << " histograms to output file.";
  if (nIgnored) 
    std::cout << " Omitting " << nIgnored << " histograms." << std::endl;
  else
    std::cout << std::endl;
    
  return;
}

std::vector<std::string> splitString(const std::string& in, const std::string& delim) {
  std::vector<std::string> retvec;
  size_t pos1=0,pos2=0;
  while (pos2!=std::string::npos) {
    pos2=in.find_first_of(delim,pos1);
    const std::string sub=in.substr(pos1,pos2-pos1);
    if (sub.size())
      retvec.push_back(sub);
    pos1=pos2+1;
  }
  return retvec;
}



bool readFromTextFile(const char* filename, std::vector<std::string>& out) {

  std::ifstream ifs (filename, std::ifstream::in );
  if (!ifs.good()) {
    std::cout << "Failed to open file " << filename << " for reading." << std::endl;
    return false;
  }
  
  while (ifs.good()) {
    std::string line;
    getline(ifs,line);
    //Bit of cleaning and splitting:
    const std::vector<std::string> linetok=splitString(line," ,;\t\n");
    std::vector<std::string>::const_iterator it=linetok.begin();
    std::vector<std::string>::const_iterator it_e=linetok.end();
    for(;it!=it_e;++it) {
      if ((*it)[0]=='#') break; //ingore all characters behind '#'
      out.push_back(*it);
    }
    //if (!line.size()) continue;
    //out.push_back(line);
  }
  ifs.close();
  return true;
}



// =================================================================================================


int main(int argc, char** argv) {

  if (argc<2 || (argc>1 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")))) {
    std::cout << "Syntax:\n" << argv[0] << " [-v ] [-i inputTextFile] [-d directroy] <outfile> <infile1> <infile2> .... " << std::endl;
    std::cout << "  -v verbose " << std::endl;
    return -1;
  }

  bool debug=false;
  std::string outFileName;
  std::vector<std::string> inFileNames;
  std::vector<std::string> baseDirs;

  for(int iArg=1;iArg<argc;++iArg) {
    if (!strcmp(argv[iArg],"-v")) {
      debug=true;
      continue;
    }

    if (!strcmp(argv[iArg],"-d")) {
      ++iArg;
      if (argc<=iArg) {
	std::cout << "ERROR: Expected a parameter (directory name) after '-d'" << std::endl;
	return -1;
      }
      baseDirs.push_back(std::string(argv[iArg]));
      continue;
    }
    

    if (!strcmp(argv[iArg],"-i")) {
      ++iArg;
      if (argc<=iArg) {
	std::cout << "ERROR: Expected text file name after '-i'" << std::endl;
	return -1;
      }
      readFromTextFile(argv[iArg],inFileNames);
      continue;
    }
    if (!outFileName.size())
      outFileName=argv[iArg];
    else
      inFileNames.push_back(std::string(argv[iArg]));
  }//end loop over arguments
  
  
  if (!outFileName.size()) {
    std::cout << "ERROR: No output file name given!" << std::endl;
    return -1;
  }

  if (!gSystem->AccessPathName(outFileName.c_str())) {
    std::cout << "ERROR: Output file " << outFileName << " exists already!" << std::endl;
    return -1;
  }

  if (!inFileNames.size()) {
    std::cout << "WARNING: No input files given! Will produce empty output file" << std::endl;
  }


  std::cout << "Number of input files: " << inFileNames.size() << std::endl;
  std::cout << "Output file: " << outFileName << std::endl;
  
  if (baseDirs.size()==0) {
    baseDirs.push_back("LAr");
    baseDirs.push_back("CaloMonitoring/LArCellMon_NoTrigSel");
    baseDirs.push_back("CaloMonitoring/ClusterMon");
  }
  else {
    //clean superfluos slash characters
    for (size_t i=0;i<baseDirs.size();++i) {
      std::vector<std::string> dirtok=splitString(baseDirs[i],std::string("/"));
      if (dirtok.size()==0) {
	baseDirs[i].clear();
	continue;
      }
      baseDirs[i]=dirtok[0];
      for (size_t j=1;j<dirtok.size();++j) {
	baseDirs[i]+="/"+dirtok[j];
      }//eld loop over dir tokens
    }//end loop over directories
  }


  histCollection listOfHists(debug);
  listOfHists.addExclusion("/CaloMonitoring/LArCellMon_NoTrigSel/Sporadic");

  TFile* in1=NULL;
  std::string runDir("/");
  size_t fileIndex=0;
  unsigned nSkippedFiles=0;

  const size_t nFiles=inFileNames.size(); 

  //Loop to find the first non-empty file
  for (;fileIndex<nFiles && runDir.size()==1;++fileIndex) {
    //in1=TFile::Open(inFileNames[fileIndex].c_str(),"READ");
    in1=openWithRetry(inFileNames[fileIndex].c_str());
    if (!in1) {
      std::cout << "ERROR: Could not open first file " << inFileNames[fileIndex] << "after all attempts" << std::endl;
      if (nSkippedFiles>MAXSKIPPEDFILES) {
	std::cout << "Failed to read " << nSkippedFiles << " input files. Abort job ..." << std::endl;
	return -1;
      }
      if (nSkippedFiles>nFiles*MAXSKIPPEDFILESFRACTION) {
	std::cout << "Failed to read " << 100.0*nSkippedFiles/nFiles << "% of input files. Abort job ..." << std::endl;
	return -1;
      }
      ++nSkippedFiles;
      std::cout << "Continue without this file. Skipped " << nSkippedFiles << " input files so far." << std::endl;
      continue;
    }

    std::cout << "Working on file " << inFileNames[fileIndex] << std::endl;
    //Get run_XXXX directory name
    TIter next(in1->GetListOfKeys() );
    TKey* key;
    while((key=(TKey*)next()))  {
      const char* name=key->GetName();
      if (!strncmp(name,"run_",4)) {
	if (runDir.size()>1) {
	  std::cout << "ERROR More than one run_XXX directory found! Ignoring " <<  name << std::endl;
	}
	else
	  runDir.append(name);
      }
    }
 
    if (runDir.size()==1) {
      std::cout << "WARNING: No run_XXXX directory found. Empty File? Ignored." << std::endl;
      in1->Close();
      continue;
    }

    //Collect histogram directories
    std::vector<std::string>::const_iterator dIt=baseDirs.begin();
    std::vector<std::string>::const_iterator dIt_e=baseDirs.end();
    for (;dIt!=dIt_e;++dIt) {
      std::string dirName=runDir+"/"+(*dIt);
      TDirectory* dir=dynamic_cast<TDirectory*>(in1->Get(dirName.c_str()));
      if (!dir) {
	std::cout << "Did not find directory " << dirName <<"!" << std::endl;
	return -1;
      }
      listOfHists.addDirectory(dir,dirName);
    }

    std::cout << "Number of directories: " << listOfHists.size() << std::endl;

    if (debug) listOfHists.print();

    //std::cout << "Closing first file" << std::endl;
    //in1->Close("R");
    //delete in1;
  }//end loop to find first non-empty file
 
  for (;fileIndex<nFiles;++fileIndex) { // Loop over remaining files
    const char* filename=inFileNames[fileIndex].c_str();
    //TFile* in=TFile::Open(filename,"READ");
    TFile* in=openWithRetry(filename);
    if (!in) {
      std::cout << "ERROR: Could not open file " << filename << "after all attempts" << std::endl;
      if (nSkippedFiles>MAXSKIPPEDFILES) {
	std::cout << "Failed to read " << nSkippedFiles << " input files. Abort job ..." << std::endl;
	return -1;
      }
      if (nSkippedFiles>nFiles*MAXSKIPPEDFILESFRACTION) {
	std::cout << "Failed to read " << 100.0*nSkippedFiles/nFiles << "% of input files. Abort job ..." << std::endl;
	return -1;
      }
      ++nSkippedFiles;
      std::cout << "Continue without this file. Skipped " << nSkippedFiles << " input files so far." << std::endl;
      continue;
    }
    std::cout << "Working on file " << filename << std::endl;
    TObject* dirObj=in->Get(runDir.c_str()+1);
    if (!dirObj) {
      std::cout << "Directory " << runDir << " not found. Ignoring apprently empty file" << std::endl;
    }
    else
      listOfHists.addFile(in);
    in->Close();
    delete in;
  }

  TFile* out=new TFile(outFileName.c_str(),"NEW");
  if (!out || !out->IsOpen()) {
    std::cout << "ERROR: Failed to open output file " << outFileName << " for writing" << std::endl;
    return -1;
  }
  listOfHists.write(out);
  out->Close();
  delete out;

  if (in1 && in1->IsOpen()) {
    in1->Close();
    delete in1;
  }

  if (nSkippedFiles>0) {
    std::cout << "WARNING: Skipped " << nSkippedFiles << " input file(s)." << std::endl;
  }

  return 0;
}
