#include "TString.h"
#include "TH1F.h"
#include "TFile.h"
#include "TSystem.h"

#include <vector>
#include <time.h>
#include <fstream>
#include <string>

#include "CompareFilesKolmogorov.C"

#define REF_ARRAY_SIZE 8

const TString RefBasePath = "/afs/cern.ch/user/d/dferreir/public/references_new_dataset";

int TestIt(const TString &FilePath, const TString &RefPath, const TString &ResultPath);

int getdir (string dir, vector<string> &files)
{
  void *dirp = gSystem->OpenDirectory(dir.c_str());
  if (dirp == NULL) {
    std::cout << "Error opening " << dir << std::endl;
    return -1;
  }
  const char *entry = gSystem->GetDirEntry(dirp);
  while (entry != 0) {
    string newEntry = dir;
    newEntry += "/";
    newEntry += entry;
    
    ifstream inf(newEntry.c_str(), ios::binary);
    char onebyte;
    inf.read(&onebyte, 1);
    if (!inf.eof()) {
      files.push_back(newEntry);
    }
    inf.close();

    entry = gSystem->GetDirEntry(dirp);
  }
  return 0;
}

int CKolmogorov(TString ResultPath = TString("results.txt"))
{
  // Comment out! For debugging purposes.
  //gDebug = 1;
  
  unsigned int k;
  
  TFile *f = 0;
  std::vector<std::string> files;
  getdir(".", files);
  for (k = 0; k < files.size(); ++k) {
    if ( (files[k].find("Validation") != std::string::npos) && (files[k].find(".root") != std::string::npos) ){
      std::cout << "Using file " << files[k] << std::endl;
      f = TFile::Open(files[k].c_str());
      if (f) {
        if (f->IsZombie()) {
	  std::cout << "Error opening file " << files[k] << std::endl;
	  delete f;
	  f = 0;
	}
      }
      if (f) {
        delete f;
        break;
      }
    }
  }
  
  std::string cwd = gSystem->WorkingDirectory();
  std::string cwd_dd = cwd.substr(0, cwd.find_last_of("/\\"));
  std::string job_option_dir = cwd_dd.substr(cwd_dd.find_last_of("/\\") + 1, std::string::npos);
  
  TString RefPath = RefBasePath;
  RefPath += "/";
  RefPath += job_option_dir;
  RefPath += "/";
  RefPath += "reference.root";
  
  std::cout << "Using reference file " << RefPath << std::endl;
  
  return TestIt(TString(files[k]), RefPath, ResultPath);
}

int TestIt(const TString &FilePath, const TString &RefPath, const TString &ResultPath) {

  int ret = 0;
  std::ofstream results((const char *) ResultPath, std::ios::trunc | std::ios::out);

  // Get time information
  time_t t = time(NULL);

  results << "Results of Kolmogorov Test" << endl;
  results << "Time: " << ctime(&t) << endl;
  results << endl; 

  // Now calculate the kolmogorov tests
  CompareFilesKolmogorov(FilePath, RefPath, ret, results, true);

  if (ret != 0)
    {
      results << ret << " tests failed (below 90%)." << endl;
    }
  else
    {
      results << "Results OK!" << endl;
    }

  results << "-- END OF FILE --" << endl;
  results.close();

  std::cout << "End of Kolmogorov test." << std::endl;

  // TODO: Returns the number of failures! Could return 0 or 1, for success or failure!
  return ret;
}

