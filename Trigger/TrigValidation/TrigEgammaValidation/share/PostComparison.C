#include "TString.h"
#include "TH1F.h"
#include "TFile.h"
#include "TSystem.h"

#include <vector>
#include <time.h>
#include <fstream>
#include <string>

#include "CompareFilesKolmogorov.C"

int TestIt(const TString &FilePath, const TString &RefPath, const TString &ResultPath);

int CKolmogorov(TString ResultPath = TString("results.txt"))
{
  // Comment out! For debugging purposes.
  //gDebug = 1;
  
  std::string file, file_reference;
  std::ifstream file_txt("file.txt");
  file_txt >> file;
  file_txt >> file_reference;
  
  std::cout << "Using reference file " << file_reference << std::endl;
  std::cout << "Using file " << file << std::endl;
  
  return TestIt(TString(file), TString(file_reference), ResultPath);
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

