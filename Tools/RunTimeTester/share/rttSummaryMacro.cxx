#include <fstream>
#include <vector>
#include <string>
#include "TH1.h"
#include "TCanvas.h"
#include "TPostScript.h"
#include "TPaveText.h"
#include <cstdlib>
#include <iostream>
#include <stdlib.h>


void printInt(int i){std::cout<<i<<" ";}

class Group{
public:
  Group(std::ifstream& f, int nRelease,
	const std::vector<std::string>& releases,
	const std::vector<std::string>& times) {

    // constructor reads in the number of jobs, succesful jobs
    // tests, succesful tests, for a job group as a function of
    // release, then make superimposed (all, succcess) hists.

    int iRelease;
    int thing;
    f>>m_group;

    for(iRelease=0; iRelease<nRelease; ++iRelease){
      f>>thing;
      std::cout<<"debug "<<thing<<std::endl;
      m_jobs.push_back(thing);
    }
    
    for(iRelease=0; iRelease<nRelease; ++iRelease){
      f>>thing;
      m_jSucc.push_back(thing);
    }

    for(iRelease=0; iRelease<nRelease; ++iRelease){
      f>>thing;
      m_tests.push_back(thing);
    }
    
    for(iRelease=0; iRelease<nRelease; ++iRelease){
      f>>thing;
      m_tSucc.push_back(thing);
    }


    m_jobAllHist  = 0;
    m_jobSucHist  = 0;
    m_testAllHist = 0;
    m_testSucHist = 0;
    makeHists(releases, times);
  }

  void dump(int gNum){
    
    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<gNum<<std::endl;
    std::cout<<m_group<<std::endl;
    
    std::cout<<std::endl<<"jobs"<<std::endl;
    std::for_each(m_jobs.begin(),   m_jobs.end(), printInt);

    std::cout<<std::endl<<"jSucc"<<std::endl;
    std::for_each(m_jSucc.begin(),  m_jSucc.end(), printInt);

    std::cout<<std::endl<<"tests"<<std::endl;
    std::for_each(m_tests.begin(),  m_tests.end(), printInt);

    std::cout<<std::endl<<"tSucc"<<std::endl;
    std::for_each(m_tSucc.begin(), m_tSucc.end(), printInt);
    
    std::cout<<std::endl;
    
    }
  TH1F* jobAllHist() {return m_jobAllHist;}
  TH1F* jobSucHist() {return m_jobSucHist;}
  TH1F* testAllHist(){return m_testAllHist;}
  TH1F* testSucHist(){return m_testSucHist;}

  /*
    Dont know why this destructor causes a crash
  ~Group(){
    std::cout<<"Group destructor: pointers to hists"<<std::endl;
    std::cout<<m_jobAllHist<<std::endl;
    std::cout<<m_jobSucHist<<std::endl;
    std::cout<<m_testAllHist<<std::endl;
    std::cout<<m_testSucHist<<std::endl;
    delete m_jobAllHist;
    std::cout<<"del0"<<std::endl;
    delete m_jobSucHist;
    std::cout<<"del1"<<std::endl;
    delete m_testAllHist;
    std::cout<<"del2"<<std::endl;
    delete m_testSucHist;
    std::cout<<"del3"<<std::endl;
  }
  */
  TPaveText* textBox(){return m_textBox;}
private:
  std::string      m_group;
  std::vector<int> m_jobs;
  std::vector<int> m_jSucc;
  std::vector<int> m_tests;
  std::vector<int> m_tSucc;
  TH1F* m_jobAllHist;
  TH1F* m_jobSucHist;
  TH1F* m_testAllHist;
  TH1F* m_testSucHist;
  
  TPaveText* m_textBox;

  void makeHists(std::vector<std::string> releases,
		 std::vector<std::string> times){
    std::cout<<"m_group1 "<<m_group<<std::endl;
    std::string label;



    label = "All Jobs "+m_group;
    m_jobAllHist = new TH1F(label.c_str(), m_group.c_str(), 7, 0., 7.);
    
    label = "Successful Jobs "+m_group;
    m_jobSucHist = new TH1F(label.c_str(), m_group.c_str(), 7, 0., 7.);
    
    label = "All Tests "+m_group;
    m_testAllHist = new TH1F(label.c_str(), m_group.c_str(), 7, 0., 7.);
    
    label = "Successful Tests "+m_group;
    m_testSucHist = new TH1F(label.c_str(), m_group.c_str(), 7, 0., 7.);
    

    
    const char* relLabels[7] = {" ", " ", " ", " ", " ", " ", " "};

    m_textBox = new TPaveText(0, 0, 1, 1);
			      
    std::vector<std::string>::const_iterator releaseIter = releases.begin();
    std::vector<std::string>::const_iterator timeIter    = times.begin();
    std::string text;
    for(; releaseIter != releases.end(); ++releaseIter, ++timeIter){
      text = "";
      text += *releaseIter;
      text += ": ";
      text += *timeIter;
      m_textBox -> AddText(text.c_str());
    }
    m_textBox ->SetTextSize(0.07);

    releaseIter = releases.begin();
    int ind = 0;
    // the x-axis labels are formed from the release name and the time 
    // the RTT was run
    for(; releaseIter != releases.end(); ++releaseIter){
      relLabels[ind++]=(*releaseIter).c_str();
      if (ind == 7) break;
    }
    
    m_jobAllHist->SetBit(TH1::kCanRebin);
    m_jobSucHist->SetBit(TH1::kCanRebin);
    m_testAllHist->SetBit(TH1::kCanRebin);
    m_testSucHist->SetBit(TH1::kCanRebin);

    int bin = 0;
    std::vector<int>::const_iterator iter;
    for(iter = m_jobs.begin(); iter != m_jobs.end(); ++iter){
      m_jobAllHist->Fill(relLabels[bin++], double(*iter));
    }
    
    bin = 0;
    for(iter = m_jSucc.begin(); iter != m_jSucc.end(); ++iter){
      m_jobSucHist->Fill(relLabels[bin++], double(*iter));
    }

    bin = 0;
    for(iter = m_tests.begin(); iter != m_tests.end(); ++iter){
      m_testAllHist->Fill(relLabels[bin++], double(*iter));
     }

    bin = 0;
    for(iter = m_tSucc.begin(); iter != m_tSucc.end(); ++iter){
      m_testSucHist->Fill(relLabels[bin++], double(*iter));
     }
  }
};

/*  
class PrintGroup{
public:
  PrintGroup(){}
  void operator()(Group g){}
};
*/    

class PrintGroup{
public:
  PrintGroup(){
    m_gNum = 0;
  }
  void operator()(Group g){
    g.dump(m_gNum++);
  }
private:
  int m_gNum;
};

class Plot{
public:
  Plot(){} // root seems to want this.....

  Plot(std::string fn){

    int maxPadSide   = 2;

    m_title      = fn.c_str();
    m_pad        = 0;
    m_lot        = 0;
    m_fn         = fn;
    m_maxPad     = maxPadSide*maxPadSide;
    m_maxPadSide = maxPadSide;
    m_can        = new TCanvas(m_title, m_title, 1);
    m_can        -> Divide(maxPadSide, maxPadSide);
    m_can->Print((m_fn+"[").c_str());

  }

  void plot(TH1* all, TH1* succ, TPaveText* text){


    if(++m_pad == m_maxPad){

      std::cout<<"wrting canvas with current pad = "<<m_pad<<std::endl;
      m_can->Print( (m_fn+"(").c_str() );
      m_can->Clear();
      m_can-> Divide(m_maxPadSide, m_maxPadSide);
      m_pad = 1;
    }
    
    m_can->cd(m_pad);
    all->SetFillColor(1);
    all->SetStats(kFALSE);
    all->Draw();
    succ->SetFillColor(3);
    succ->SetStats(kFALSE);
    succ->Draw("same");

    // draw the text box giving the date and time for each release
    // to th last pad
    if (++m_pad == m_maxPad){
      m_can->cd(m_pad);
      text->Draw();
    }
    --m_pad;

  }
  void close(){
    if (m_pad != m_maxPad){
      std::cout<<"writing last canvas with current pad = "<<m_pad<<std::endl;
      m_can->Print( (m_fn+"(").c_str() );
    }
    m_can->Print( (m_fn+"]").c_str());
  }

  void operator()(Group& g){}

private:
  TCanvas* m_can;
  int m_maxPad;
  int m_maxPadSide;
  int m_pad;
  int m_lot;
  const char* m_title;
  std::string m_fn;
};

class PlotJobs: public Plot{
  // function object to plot all jobs superimposed by succesful jobs
public:
  PlotJobs(std::string fn): Plot(fn+ ":Jobs.ps"){}
  
  void operator()(Group& g){
    this->plot(g.jobAllHist(), g.jobSucHist(), g.textBox());
  }
};

class PlotTests: public Plot{
  // function object to plot all tests superimposed by succesful tests
public:
  PlotTests(std::string fn): Plot(fn+":Tests.ps"){}
  void operator()(Group& g){
    this->plot(g.testAllHist(), g.testSucHist(), g.textBox());
  }
};


int doit(std::string fileName){

  const char* fn = fileName.c_str();
  std::cout<<"Summary File = "<<fn<<std::endl;
  std::ifstream from(fn);
  if (!from) {
    std::cout<<"cannot open "<<fn<<std::endl;
    return 1;
  }

  int nRelease;
  from >> nRelease;


  std::vector<std::string> releases;
  for (int release =0; release<nRelease; ++release){
    std::string rel;
    from >> rel;
    std::cout<<"release in = "<<rel<<std::endl;
    releases.push_back(rel);
  }

  std::vector<std::string> times;
  for (int release =0; release<nRelease; ++release){
    std::string time;
    from >> time;
    std::cout<<"release time = "<<time<<std::endl;
    times.push_back(time);
  }

  int nGroups;
  from>>nGroups;
  std::vector<Group> groups;
  for(int iGroup = 0; iGroup<nGroups; ++iGroup)
    {groups.push_back(Group(from, nRelease, releases, times));}
  
  from.close();
  
  std::for_each(groups.begin(), groups.end(), PrintGroup());  
  
  PlotJobs plotJobs(fileName);
  plotJobs = std::for_each(groups.begin(), groups.end(), plotJobs);
  plotJobs.close();

  PlotTests plotTests(fileName);
  plotTests = std::for_each(groups.begin(), groups.end(), plotTests);
  plotTests.close();
  
  
  
  return 0;
}
int rttSummaryMacro(){
  // read in a list of files to process.
  // each file contains information for a number of releases.
  // for the information is only available for nightlies.
  // The information is written out by the RTT for each run, then
  // collected and massages by RTTSumFromXML.py

  std::cout<<"hello1"<<std::endl;
  const char* fn = "./rttSummaryList.txt";
  std::ifstream listIn(fn);
  std::vector<std::string> summaryFiles;
  std::string summaryFile;
  while (listIn>>summaryFile) summaryFiles.push_back(summaryFile);
 

  if (summaryFiles.size() == 0){
    std::cout<<"No summary files found"<<std::endl;
    return 0;
  }
  std::for_each(summaryFiles.begin(), summaryFiles.end(), doit);
  std::cout<<"hello2"<<std::endl;
  
  return 0;
}
/*
int rttSummaryMacro(){
  std::cout<<"hello"<<std::endl;
  return 0;
}
*/


