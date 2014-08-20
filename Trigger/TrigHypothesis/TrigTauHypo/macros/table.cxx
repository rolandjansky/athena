/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Cuts{
  string item;
  float l2CaloEtMin;
  float l2EMRadiusMax;
  int l2NtrkMin;
  int l2NtrkMax;
  float l2LeadtPtMin;
  float l2SumPtRatioMax;;
  float l2SimpleEtFlowMin;

  int efNTrackMin     ;   
  int efNTrackMax        ;
  float efEtCalibMin       ;
  float efEMRadiusMax_1P   ;
  float efEMRadiusMax_MP   ;
  float efPtMaxTrackMin_1P ;
  float efPtMaxTrackMin_MP ;


  void reset(){
    item = "";
  l2CaloEtMin = 0;
  l2EMRadiusMax = 0;
  l2NtrkMin = 0;
  l2NtrkMax = 0;
  l2LeadtPtMin = 0;
  l2SumPtRatioMax = 0;;
  l2SimpleEtFlowMin = 0;

  efNTrackMin     = 0 ;   
  efNTrackMax       = 0  ;
  efEtCalibMin      = 0  ;
  efEMRadiusMax_1P   = 0 ;
  efEMRadiusMax_MP    = 0;
  efPtMaxTrackMin_1P  = 0;
  efPtMaxTrackMin_MP  = 0;

  };

  void printL2() const
  {
    //out << " Calo Et &  EMRad & Ntr & Leading Pt & SumPtRatio & Et Flow \\\\ \n";
    printf("%s\t & %6.1f & %6.4f & [ %d,%d ]  & %3.2f & %5.1f \\\\ \n",
	   item.c_str() , l2CaloEtMin/1000., l2EMRadiusMax, l2NtrkMin, l2NtrkMax, //l2LeadtPtMin,
	   l2SumPtRatioMax, l2SimpleEtFlowMin);
  };
  void printEF() const
  {
    //cout << " EMRad (1P/MP) &  Ntr & Leading Pt & SumPtRatio & Et \\\\ \n";
    printf("%s\t &  %5.3f /%5.3f & [%d,%d] &\t%3.1f /%3.1f \t& %5.1f \\\\ \n",
	   item.c_str() , efEMRadiusMax_1P, efEMRadiusMax_MP, efNTrackMin, efNTrackMax,
	   efPtMaxTrackMin_1P, efPtMaxTrackMin_MP, efEtCalibMin);
  };


  void readFile(const string & filename)
  {
    ifstream myfile (filename.c_str());
    string line;
    if (myfile.is_open())
      {
	while (! myfile.eof() )
	  {
	    getline (myfile,line);
	    //cout << line << "\n";
	    
	  //---------- item -------------------
	    if( line.find("T2CaloTauHypo_tau") !=string::npos &&  line.find("class") !=string::npos){

	      if( line.find("_") !=string::npos ) line.replace(0,line.find("_")+1,"");
	      if( line.find(" ") !=string::npos ) line.replace(line.find(" "),string::npos,"");
	      item = line;
	      cout << "==================== "<< item << " \n";
	    }

	  //---------- L2Calo -------------------
	  if( line.find("EMRadius") !=string::npos &&  line.find("self.EMRadiusMax") ==string::npos)
	    {
	      line.replace(0,line.find("=")+1,"");  l2EMRadiusMax=atof(line.c_str());
	      
	    } else if( line.find("RawEtNar") !=string::npos){
	    line.replace(0,line.find("=")+1,"");  l2CaloEtMin=atof(line.c_str());
	    
	    // --------- L2 tracking --------------------
	  }else if( line.find("self.NTrkMin") !=string::npos) {
	    line.replace(0,line.find("=")+1,"");  l2NtrkMin=atoi(line.c_str());
	    
	  }else  if( line.find("self.NTrkMax") !=string::npos) {
	    line.replace(0,line.find("=")+1,"");  l2NtrkMax=atoi(line.c_str());
	    
	  }else if( line.find("self.sumPtRatio") !=string::npos) {
	    line.replace(0,line.find("=")+1,"");  l2SumPtRatioMax=atof(line.c_str());

	  }else if( line.find("self.MinLeadingTrackPt") !=string::npos) {
	    line.replace(0,line.find("=")+1,"");  l2LeadtPtMin=atof(line.c_str());
	    
	    
	  } else if( line.find("MinSimpleEtFlow") !=string::npos){
	    line.replace(0,line.find("=")+1,"");  l2SimpleEtFlowMin=atof(line.c_str());
	    
	    
	    
	    // --------- EF  --------------------
	    
	  } else if( line.find("self.NTrackMin") !=string::npos) {
	    line.replace(0,line.find("=")+1,"");  efNTrackMin=atoi(line.c_str());

	  } else if( line.find("self.NTrackMax") !=string::npos) {
	    line.replace(0,line.find("=")+1,"");  efNTrackMax=atoi(line.c_str());
	    
	  } else if( line.find("self.EtCalibMin") !=string::npos) {
	    line.replace(0,line.find("=")+1,"");  efEtCalibMin=atof(line.c_str());
	  
	  } else if( line.find("self.EMRadiusMax_1P") !=string::npos) {
	    line.replace(0,line.find("=")+1,"");  efEMRadiusMax_1P=atof(line.c_str());
      
	  } else if( line.find("self.EMRadiusMax_MP") !=string::npos) {
	    line.replace(0,line.find("=")+1,"");  efEMRadiusMax_MP=atof(line.c_str());
  
	  } else if( line.find("self.PtMaxTrackMin_1P") !=string::npos) {
	    line.replace(0,line.find("=")+1,"");  efPtMaxTrackMin_1P=atof(line.c_str());


	  } else if( line.find("self.PtMaxTrackMin_MP") !=string::npos) {
	    line.replace(0,line.find("=")+1,"");  efPtMaxTrackMin_MP=atof(line.c_str());
	    
	  }
	  
	  
	  }
	myfile.close();
      }
    
    else cout << "Unable to open file " << filename << "\n" ; 
    
  };


  
};


int main()
{
  const int nfiles = 8*3;
  string filenames[nfiles] = {
"TrigTauHypoConfig_tau12_loose.py",
"TrigTauHypoConfig_tau16i_loose.py",
"TrigTauHypoConfig_tau20i_loose.py",
"TrigTauHypoConfig_tau29i_loose.py",
"TrigTauHypoConfig_tau38_loose.py",
"TrigTauHypoConfig_tau50_loose.py",
"TrigTauHypoConfig_tau84_loose.py",
"TrigTauHypoConfig_tau125_loose.py",


"TrigTauHypoConfig_tau16i_medium.py",
"TrigTauHypoConfig_tau20i_medium.py",
"TrigTauHypoConfig_tau29i_medium.py",
"TrigTauHypoConfig_tau38_medium.py",
"TrigTauHypoConfig_tau50_medium.py",
"TrigTauHypoConfig_tau84_medium.py",
"TrigTauHypoConfig_tau125_medium.py",

"TrigTauHypoConfig_tau16i_tight.py",
"TrigTauHypoConfig_tau20i_tight.py",
"TrigTauHypoConfig_tau29i_tight.py",
"TrigTauHypoConfig_tau38_tight.py",
"TrigTauHypoConfig_tau50_tight.py",
"TrigTauHypoConfig_tau84_tight.py",
"TrigTauHypoConfig_tau125_tight.py"
  };

  Cuts cuts[nfiles];
  for(unsigned int ifile = 0; ifile < nfiles; ++ifile)
    {
      cuts[ifile].reset();
      cuts[ifile].readFile(filenames[ifile]);
    }

  cout << " &  Calo Et &  EMRad & Ntr & Leading Pt & SumPtRatio & Et Flow \\\\ \n";
  for(unsigned int ifile = 0; ifile < nfiles; ++ifile)
    cuts[ifile].printL2();
  
  cout << "  &  EMRad (1P/MP) &  Ntr & Leading Pt(1P/MP) &  Et \\\\ \n";
  for(unsigned int ifile = 0; ifile < nfiles; ++ifile)
    cuts[ifile].printEF();
}




