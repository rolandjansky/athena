/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include <iostream>
#include <time.h>

#include "Regexer.h"
#include "PathFinder.h"



int main(int argc, char* argv[]){

  //switch arguments:
  //possible flags:
  //-f ouputfile
  //-i list input
  //-o listoutput
  //-h help
  //--incl include pattern
  //--excl exclude pattern
  
  //create holders for complete job specification
  bool cleanCommand = true;
  std::string inputFilePath = "";
  std::string outputFilePath = "";
  std::string inputListPath = "";
  std::string outputListPath = "";
  std::string includeExpression = "";
  std::string excludeExpression = "";
  std::string runNumber = "XXXXXX";
  //loop through arguments
  for(int i=1; i<argc; i++){
   
    //read argument to string
    std::string arg = argv[i];

    //check for help flag and skip to validation (which will fail) and so on to usage instructions
    if(arg=="-h"){ cleanCommand = false; break; }
    
    //check for output filename
    else if(arg=="-f"){ 
      //check there is another argument following this
      if(argc<i+2){ cleanCommand = false; break; }
      //set output path
      outputFilePath=argv[i+1];
      //increment counter so we don't parse next argument again
      i++;
    }
   
    //check for include expression
    else if(arg=="--incl"){
      //check there is another argument following this
      if(argc<i+2){ cleanCommand = false; break; }
      includeExpression=argv[i+1];
      //increment counter so we don't parse next argument again
      i++;
    }

    //check for exclude expression
    else if(arg=="--excl"){
      //check there is another argument following this
      if(argc<i+2){ cleanCommand = false; break; }
      excludeExpression=argv[i+1];
      //increment counter so we don't parse next argument again
      i++;
    }

    //else this must be the inputfile
    else{
      inputFilePath=arg;
    }

  }

  //validate/complete arguments
  if(inputFilePath==""){ cleanCommand=false; }
  if(outputFilePath==""){
    //fetch date
    time_t systime = time(NULL);
    tm* loctime = localtime(&systime);
    char datestring[8];
    sprintf(datestring, "%04d%02d%02d", loctime->tm_year+1900, loctime->tm_mon+1, loctime->tm_mday);
    //fetch username
    std::string username = getenv("USER");
    //fetch runnumber from input file (eight digits enclosed by dots, strip leading zeros)
    if(inputFilePath!=""){
      Regexer* exer = new Regexer();
      runNumber = exer->ExtractRunNumber(inputFilePath);
      delete exer;
    }

    //issue warning if no runnumber was found
    if(runNumber==""){
      std::cout<<"WARNING: couldn't detect run number in input filename. Is this a standalone test?"<<std::endl;
      runNumber="000000";
    }
    
    //build outputfilepath
    outputFilePath = "HLT_HLTEgamma_Tier0Mon_R"+runNumber+"_"+username+"_"+datestring+".root";
  }
 
  //set expressions to default if none were given (both need to be empty for default to be applied)
  if(cleanCommand && includeExpression=="" && excludeExpression==""){
    std::cout<<"INFO: setting include/exclude expressions to defaults."<<std::endl;
    includeExpression="(EgammaMon|EgammaSigTE.*(PassedChain|Offline))";
    excludeExpression="EgammaMon.*(Efficiency|Rates)";
  }


  //issue usage instructions if command is ill-defined and bail out
  if(!cleanCommand){
    std::cout<<std::endl<<"TrigEgammaMonitoring histogram extractor"<<std::endl<<std::endl;
    std::cout<<"USAGE: extractor.exe [-(h|f) <option>] [--(incl|excl) <expression>] <input file>"<<std::endl;
    std::cout<<std::endl<<"Basic options:"<<std::endl;
    std::cout<<"  -h ... print this screen"<<std::endl;
    std::cout<<"  -f <output file> ... write output to file specified (overrides default filename)"<<std::endl;
    //std::cout<<"  -i <input list file> ... copy all histograms listed in file"<<std::endl;
    //std::cout<<"  -o <output list file> ... write histogram list to file"<<std::endl;
    std::cout<<std::endl<<"Histogram path matching:"<<std::endl;
    std::cout<<"  --incl <regex> ... include expression: histograms are only copied if this matches their full path."<<std::endl;
    std::cout<<"  --excl <regex> ... exclude expression: same as above, only matches are excluded."<<std::endl;
    std::cout<<std::endl;
    exit(-1);
  }

  //print current job info
  std::cout<<"RUNNING extraction:"<<std::endl;
  std::cout<<" - inputFile: "<<inputFilePath<<std::endl;
  std::cout<<" - outputFile: "<<outputFilePath<<std::endl;
  //if(inputListPath!=""){ std::cout<<" - reading histogram list from: "<<inputListPath<<std::endl; }
  //if(outputListPath!=""){ std::cout<<" - writing histogram list to: "<<outputListPath<<std::endl; }
  std::cout<<" - includeExpression: "<<includeExpression<<std::endl;
  std::cout<<" - excludeExpression: "<<excludeExpression<<std::endl;

  
  PathFinder* finder = new PathFinder(inputFilePath, runNumber, includeExpression, excludeExpression);
  finder->WalkDirectory();
  finder->FilterList();
  finder->CopyHistograms(outputFilePath);
  exit(0);
}
