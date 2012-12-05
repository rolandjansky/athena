/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * This macro allows to pick all histograms analyzed by one Algorithm from several han output files and compiles an overview webpage.
 *
 * Execute the function like this from the command line:
 *
 * echo AlgorithmName outputfile.html input1.root input2.root... | root -b CompileAlgorithmOverview.C+
 *
 * This macro assumes that all inputfiles contain the same histograms for the given algorithm.
 * If this is not the case it will work anyway, but the labeling in the output table migth be screwed up.
 */

#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <algorithm>

#include "TList.h"
#include "TFile.h"
#include "TKey.h"
#include "TDirectory.h"
#include "TClass.h"
#include "TH1F.h"
#include "TCanvas.h"

using namespace std;

typedef map<string, vector<double>* > overviewMapT;

map<string, overviewMapT*> overviewMap;

typedef map<string,string> histoMapT;

//A map which contains a map of histos an results for each run (nested maps)
map<string,histoMapT*> runMap;
//A map which stores the base url for each run
map<string,string> linkBaseMap;
map<string,string> imgBaseMap;

//two global variables to keep track of the current run in all subroutines
string checkAlgName;
string runNumber;

//Set to true if you want a stdout output for each histogram
bool detailedOutput = false;

//Some summary histograms
TH1F *hChisquare, *hAsym, *hSym;

string GetPathInFile(const char * fullPath)
//takes a full path (containing path of file in filesysteme and path in file) and returns only the path in file.
{
  string pathInFile(fullPath);
  int filePathEnd = pathInFile.find_last_of(":");
  pathInFile.erase(0, filePathEnd+1);
  return pathInFile;
}

TKey *GetHanValueKey(TKey *hanValueKey)
//This funktion returns the TKey holding the (alpha)numerical value of the parameter whose key is passed on to the function
{
  TDirectory *hanValueDir = dynamic_cast<TDirectory*>(hanValueKey->ReadObj());
  if (hanValueDir == 0)
    return 0;
  TKey *resultValueKey = dynamic_cast<TKey*>(hanValueDir->GetListOfKeys()->At(0));
  return resultValueKey;
}

void printHanConfigs(TDirectory *configDir)
{
  if (detailedOutput)
    cout << "Parameters:" << endl;
  //Loop over all config parameters
  TList *configKeyList = configDir->GetListOfKeys();
  for (int i = 0; i < configKeyList->GetEntries(); i++)
  {
    TKey *configNameKey = dynamic_cast<TKey*>(configKeyList->At(i));
    string configName = configNameKey->GetName();
    //print their name and value
    if (detailedOutput)
      cout << "\t" << configName << "\t";
    TKey *configValueKey = GetHanValueKey(configNameKey);
    if (detailedOutput)
      cout << configValueKey->GetName() << endl;
  }
  return;
}

void printHanLimits(TDirectory *limitDir)
{
  if (detailedOutput)
    cout << "Limits:" << endl;
  
  //Just to be sure we didn't get a null pointer
  if (limitDir == 0)
  {
    cout << "Empty Dir given" << endl;
    return;
  }
  
  //Loop over all limit variables
  TList *limitKeyList = limitDir->GetListOfKeys();
  for (int i = 0; i < limitKeyList->GetEntries(); i++)
  {
    TKey *limitNameKey = dynamic_cast<TKey*>(limitKeyList->At(i));
    string limitName = limitNameKey->GetName();
    if (detailedOutput)
      cout << "\t" << limitName << endl;
    TDirectory *limitValueDir = dynamic_cast<TDirectory*>(limitNameKey->ReadObj());
    //now get the numerical values for red and yellow thresholds
    TKey *greenKey = limitValueDir->FindKey("Green");
    TKey *yellowKey = limitValueDir->FindKey("Red");
    TKey *greenValueKey = GetHanValueKey(greenKey);
    TKey *yellowValueKey = GetHanValueKey(yellowKey);
    //and print them
    if (detailedOutput)
      cout << "\t\tGreen\t" << greenValueKey->GetName() << endl;
    if (detailedOutput)
      cout << "\t\tRed\t" << yellowValueKey->GetName() << endl;
  }
  return;
}


string printResults(TDirectory *resultsDir, string histoName)
{
  if (detailedOutput)
    cout << "Results:" << endl;
  string returnString = "";
  
  //Loop over all entries in the results dir
  TList *resultsKeyList = resultsDir->GetListOfKeys();
  for (int i = 0; i < resultsKeyList->GetEntries(); i++)
  {
    //Get name of the reported value
    TKey *resultNameKey = dynamic_cast<TKey*>(resultsKeyList->At(i));
    string resultName = resultNameKey->GetName();
    //(once again: we don't care for references)
    if(resultName == "Reference")
      continue;
    if((*overviewMap[histoName]).find(resultName) == (*overviewMap[histoName]).end())
      (*overviewMap[histoName])[resultName] = new vector<double>();
    if (detailedOutput)
      cout << "\t" << resultName << "\t" << GetHanValueKey(resultNameKey)->GetName() << endl;
    
    if (resultName == "Max_rel_asym_deviation")
      hAsym->Fill(atof(GetHanValueKey(resultNameKey)->GetName()));
    if (resultName == "Max_rel_sym_deviation")
      hSym->Fill(atof(GetHanValueKey(resultNameKey)->GetName()));
    if (resultName == "Chisquare_ndf")
      hChisquare->Fill(atof(GetHanValueKey(resultNameKey)->GetName()));
    //if the result entry is the overall histo status we want to report this up
    if(resultName == "Status")
      returnString = GetHanValueKey(resultNameKey)->GetName();
    (*(*overviewMap[histoName])[resultName]).push_back(atof(GetHanValueKey(resultNameKey)->GetName()));
  }
  if (returnString == "Undefined")
    returnString = "Blue";
  return returnString;
}

int handleHisto(TDirectory *hanDir, TKey *histoKey)
{
  //Lets see if we can find a config and a results dir in the han directory (should be the case)
  TDirectory *configDir = dynamic_cast<TDirectory*>(hanDir->FindKey("Config")->ReadObj());
  TDirectory *resultsDir = dynamic_cast<TDirectory*>(hanDir->FindKey("Results")->ReadObj());
  if ((configDir == 0) || (resultsDir == 0))
  {
    cout << "Could not read Config and Results directories for histogram " << histoKey->GetName() << endl;
    return -5;
  }
  
    //now let's see which algorithm has done its work here
  TDirectory *dirBuff = dynamic_cast<TDirectory*>(configDir->FindKey("name")->ReadObj());
  if (dirBuff == 0)
  {
    cout << "Could not read name directory for histogram " << histoKey->GetName() << endl;
    return -5;
  }
  string algName = dirBuff->GetListOfKeys()->At(0)->GetName();
  
  if (algName == checkAlgName)
  //it's the alg we are interested in
  {
    //Get the name of the histo and its path in the file
    string histoName = GetPathInFile(hanDir->GetPath()).c_str();
    histoName.erase(histoName.length()-1, histoName.length()); //configdir = histoname + _ 
    
    if(overviewMap.find(histoName) == overviewMap.end())
      overviewMap[histoName] = new overviewMapT();
    
    if (detailedOutput)
      cout << "\nHistogram: " << histoName.c_str() << " was checked with " << algName.c_str() << endl;
    //Get directory pointers for parameter, limits and results separately and deal with them accordingly

    TDirectory *paramDir = dynamic_cast<TDirectory*>(configDir->FindKey("algPars")->ReadObj());
    if (detailedOutput)
      printHanConfigs(paramDir);
    
    if (detailedOutput)
    {
      TDirectory *limitsDir = dynamic_cast<TDirectory*>(configDir->FindKey("algLimits")->ReadObj());
      printHanLimits(limitsDir);
    }
    
    
    
    string status = printResults(resultsDir, histoName);
    
    //put the name of the histo and its status in the right map
    (*runMap[runNumber])[histoName] = status;
  }
  return 0;
}

void HandleLeaf(TDirectory *leafDir, TKey *leafKey)
{
  TObject *objBuff = leafKey->ReadObj();
  //if leaf object is not a histogram we don't care for it.
  if ((objBuff->IsA()->InheritsFrom("TH1")) != kTRUE)
    //is not a histogram
    return;
  //if leaf is a reference histogram we are not interested
  string leafName(leafKey->GetName());
  if (leafName == "Reference")
    //is a reference histogram
    return;
  //check whether there is a dir containing han information for this histo
  string hanDirName = leafKey->GetName();
  hanDirName.append("_");
  TKey *hanDirKey = leafDir->FindKey(hanDirName.c_str());
  if (hanDirKey == 0)
    //no matching config dir could be founf
    return;
  
  TDirectory *hanDir = dynamic_cast<TDirectory*>(hanDirKey->ReadObj());

  //just to be sure... should never happen
  if (hanDir == 0)
  {
    cout << "something really wrong happened: hanDir is not a TDriectory!" << endl;
    exit(-1);
  }
  
  //now: there is a valid histo: do it!
  handleHisto(hanDir, leafKey);
}

void BrowseDirectoryStructure(TDirectory* indir, int indentlevel = 0)
//browse recursivly through a directory structure
//note: indentlevel is not used here but allows to keep track of the recursion level
{
  TList *keyList = indir->GetListOfKeys();
  indentlevel++;
  //for all elements in a directory
  for (int i = 0; i < keyList->GetEntries(); i++)
  {
    //check if they are directories themselves
    TKey *keyBuff = dynamic_cast<TKey*>(keyList->At(i));
    TDirectory *workDir = dynamic_cast<TDirectory*>(keyBuff->ReadObj());
    
    //if yes: go down recurcion
    if (workDir != 0)
    {
      BrowseDirectoryStructure(workDir, indentlevel);
    }
    //if not: we found a leaf. Deal with it!
    else
    {
      HandleLeaf(indir, keyBuff);
    }
  }
  return;
}  

void PrintSummary(string outfilename)
{
  //open file for writing
  ofstream outfile;
  outfile.open(outfilename.c_str());
  
  if (!outfile.is_open())
  {
    cout << "Could not open file " << outfile << endl;
    return;
  }
  
  cout << "Writing output to: " << outfilename << endl;

  //build basic html structure
  outfile << "<html>\n<head>\n<title>Summary of histograms checked with algorithm \"" << checkAlgName << "\"</title>\n</head>\n<body>\n";
  outfile << "<table cellpadding=\"4\" border=\"2\">\n";
  
  //now we need some iterators to loop over our nested maps
  map<string,histoMapT*>::iterator it = runMap.begin();
  map<string,string>::iterator itt;
  
  //here comes the header row
  outfile << "<tr>\n<th>Run No.</th>\n";
  
  //it just takes the first run here
  for (itt = (it->second)->begin(); itt != (it->second)->end(); ++itt)
  {
    //loop over the histograms analyzed and print their names. It is assumed here, that all files contain the same histograms here!
    outfile << "<th>" << itt->first << "</th>\n";
  }
  
  outfile << "</tr>\n";
  
  //now for the data
  for (it = runMap.begin(); it != runMap.end(); ++it)
  //loop over all runs and print run no
  {
    outfile << "<tr>\n<td>";
    cout << it->first << endl;
    outfile << it->first << "</td>\n";
    for (itt = (it->second)->begin(); itt != (it->second)->end(); ++itt)
    //now loop over all histos and display histogram image and status in the table
    {
      string link = linkBaseMap[it->first] + itt->first;
      string img = imgBaseMap[it->first] + itt->first;
      string imgUrl = img + ".png";
      cout << itt->first << "\t" << itt->second << endl;
      outfile << "<td bgcolor=\"" << itt->second << "\"><a href=\"" << link << "\">" << "<img src=\"" << imgUrl << "\" width=\"200\"></a></td>\n";
    }
    outfile << "</tr>\n";
  }
  //done
  outfile << "</table>\n</body>\n</html>\n";
  outfile.close();
  return;
}
  
void changeDirectory(string Path)
{
  string headdir = Path.substr(0, Path.find_first_of("/"));
//   cout << "cd to subdir " << headdir << endl;
  if (!gDirectory->cd(headdir.c_str()))
  {
//     cout << "subdir " << headdir << " not found. Created." << endl;
    gDirectory->mkdir(headdir.c_str());
    gDirectory->cd(headdir.c_str());
  }
  Path.erase(0, Path.find_first_of("/")+1);
//   cout << "remaining path " << Path.c_str() << endl;
  if (Path.empty())
  {
    return;
  } 
  else
  {
    changeDirectory(Path);
  }
}
  
void handleOverviewMap(string outfile)
{
  outfile += ".root";
  TFile* rootFile = new TFile(outfile.c_str(), "RECREATE");
  
  map<string,overviewMapT*>::iterator it;
  map<string,vector<double>* >::iterator itt;
  vector<double>::iterator ittt;
  vector<double> *vBuff = 0;
  TH1 *hBuff = 0;
  
  for(it = overviewMap.begin(); it != overviewMap.end(); it++)
  {
    string histoName = it->first;
    //puts(histoName.c_str());
    for (itt = it->second->begin(); itt != it->second->end(); itt++)
    {
      string paramName = itt->first;
      //puts(paramName.c_str());
      vBuff = itt->second;
      sort(vBuff->begin(), vBuff->end());
      string hName = histoName.substr(histoName.find_last_of("/")+1, histoName.size()); 
      string hPath = histoName.substr(0, histoName.find_last_of("/")+1);
      gDirectory->cd((outfile+":").c_str());
      hPath.erase(0,1);
      changeDirectory(hPath);
      hName += "_";
      hName += paramName;
      string hTitle = histoName + " " + paramName;
      hBuff = new TH1F(hName.c_str(), hTitle.c_str(), 101, vBuff->front()-((vBuff->back()-vBuff->front())/200), vBuff->back()+((vBuff->back()-vBuff->front())/200));
      for(unsigned int i = 0; i < vBuff->size(); i++)
	hBuff->Fill(vBuff->at(i));
      hBuff->Write();
      delete hBuff;
      hBuff = 0;
    }
  }
  //puts("done");
  delete rootFile;
}
  
void CompileAlgorithmOverview()
{
  //Input via stdin. Looks complicated but is very flexible
  //first param: Algorithm Name to be checked
  cin >> checkAlgName;

  //second param: outfile for html-overview
  string outfile;
  cin >> outfile;

  string filename;
  int filecount = 0;
  
  hChisquare = new TH1F("chisquare", "Quality of Fit Distribution:Chisquare / n.d.f.:", 101, -0.5, 100.5);
  hAsym = new TH1F("asym", "Distribution of asymmetric deviations:relative asymmetric deviations:", 101, -0.005, 1.005);
  hSym = new TH1F("sym", "Distribution of symmetric deviations:relative symmetric deviations:", 101, -0.005, 1.005);
  
  //now loop over all infilenames
  while(cin >> filename) {
    //if there are lowstat lumiblock summaries skip them
    if (filename.find("lowStat") < filename.length())
      continue;
    
    cout << "Opening file " << filename.c_str() << endl;
    
    //Compute Run number from filename
    runNumber = filename;
    runNumber.erase(0, runNumber.find_last_of("/")+1);
    runNumber.erase(runNumber.find_last_of("_"),runNumber.length());
    runNumber.erase(0, runNumber.find_last_of("_")+1);
    
    //generate a new map for the histos and there status for this run number
    runMap[runNumber] = new histoMapT(); 
    
    //Build URL base for webdisplay from filename
    string linkBase = filename;
    linkBase.erase(0,linkBase.find("test"));
//     linkBase.insert(0, "http://atlasdqm.cern.ch:8080/webdisplay/");
    linkBase.erase(linkBase.rfind("run")+3, linkBase.length());
    string imgBase = linkBase;
    linkBase.insert(0, "http://atlasdqm.cern.ch/webdisplay/");
    imgBase.insert(0, "http://atlasdqm.cern.ch/webdisplay/");
//    imgBase.insert(0, "img/webdisplay/");
    
    //...and put it into matching map
    linkBaseMap[runNumber] = linkBase;
    imgBaseMap[runNumber] = imgBase;
    
    filecount++;
    
    //Open file and handle content
    TFile *f = new TFile(filename.c_str());
    BrowseDirectoryStructure(f);
    delete f;
  }
  
  //now sum it up and build webpage
  PrintSummary(outfile);
  
  TCanvas *overviewCanvas = new TCanvas("Overview");
  overviewCanvas->Divide(2,2);
  overviewCanvas->cd(1);
  hChisquare->Draw();
  overviewCanvas->cd(2);
  hAsym->Draw();
  overviewCanvas->cd(3);
  hSym->Draw();
  
  string imagefile = outfile + ".C";
  overviewCanvas->SaveAs(imagefile.c_str());
  
  handleOverviewMap(outfile);
  
  //That's all Folks
  return;
}


