/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <dqm_algorithms/tools/DumpConfig.h>

using namespace std;

dqm_algorithms::tools::DumpConfig::DumpConfig(std::string ParameterName,dqm_core::test::DummyAlgorithmConfig & config, std::string algorithmname, std::string histogramname,  std::string reffilename,std::string refhistogramname, float weight, std::string regionname){
_ParameterName=ParameterName;
 _config=config;
 _algorithmname=algorithmname;
 _histogramname=histogramname;
 _weight=weight;
 _refhistogramname=refhistogramname;
 _reffilename=reffilename;
 _regionname=regionname;
 
}

dqm_algorithms::tools::DumpConfig::~DumpConfig(){
}

void
dqm_algorithms::tools::DumpConfig::DumpRegion() {
 _myfile<<"<obj class=\"DQRegion\" id=\""+_regionname+"\">\n";
 _myfile<<"<attr name=\"InputDataSource\" type=\"string\">\"\"</attr>\n";
 _myfile<<"<attr name=\"Weight\" type=\"float\">1</attr>\n";
 _myfile<<"<attr name=\"Action\" type=\"string\">\"\"</attr>\n";
 _myfile<<"<rel name=\"Algorithm\">\"DQAlgorithm\" \"SimpleSummary\"</rel>\n";
 _myfile<<"<rel name=\"AlgorithmParameters\" num=\"0\"></rel>\n";
 _myfile<<"<rel name=\"References\" num=\"0\"></rel>\n";
 _myfile<<"<rel name=\"GreenThresholds\" num=\"0\"></rel>\n";
 _myfile<<"<rel name=\"RedThresholds\" num=\"0\"></rel>\n";
 _myfile<<"<rel name=\"DQRegions\" num=\"0\"></rel>\n";
 _myfile<<"<rel name=\"DQParameters\" num=\"1\">\n";
 _myfile<<"   \"DQParameter\"   \""+_ParameterName+"\"\n";
 _myfile<<"</rel>\n";
_myfile<<"</obj>\n\n";

}

void
dqm_algorithms::tools::DumpConfig::DumpAgent() {  
 _myfile<<"<obj class=\"DQAgent\" id=\""+_regionname+"\">\n";
 _myfile<<"<attr name=\"Parameters\" type=\"string\">\"-n "+_regionname+" -p ${TDAQ_PARTITION}\"</attr>\n";
 _myfile<<"<attr name=\"RestartParameters\" type=\"string\">\"-n "+_regionname+" -p ${TDAQ_PARTITION}\"</attr>\n";
 _myfile<<"<attr name=\"ControlledByOnline\" type=\"bool\">1</attr>\n";
 _myfile<<"<attr name=\"IfDies\" type=\"enum\">\"Restart\"</attr>\n";
 _myfile<<"<attr name=\"IfFailed\" type=\"enum\">\"Restart\"</attr>\n";
 _myfile<<"<attr name=\"StartAt\" type=\"enum\">\"Boot\"</attr>\n";
 _myfile<<"<attr name=\"StopAt\" type=\"enum\">\"Shutdown\"</attr>\n";
 _myfile<<"<attr name=\"InitTimeout\" type=\"u32\">0</attr>\n";
 _myfile<<"<attr name=\"StartIn\" type=\"string\">\"\"</attr>\n";
 _myfile<<"<attr name=\"InputDevice\" type=\"string\">\"\"</attr>\n";
 _myfile<<"<attr name=\"Logging\" type=\"bool\">1</attr>\n";
 _myfile<<"<rel name=\"InitializationDependsFrom\" num=\"0\">\n";
 _myfile<<"</rel>\n";
 _myfile<<"<rel name=\"ShutdownDependsFrom\" num=\"0\"></rel>\n";
 _myfile<<"<rel name=\"Program\">\"Binary\" \"dqmf_agent\"</rel>\n";
 _myfile<<"<rel name=\"ExplicitTag\">\"\" \"\"</rel>\n";
 _myfile<<"<rel name=\"Uses\" num=\"0\"></rel>\n";
 _myfile<<"<rel name=\"ProcessEnvironment\" num=\"0\">\n";
 _myfile<<"</rel>\n";
 _myfile<<"<rel name=\"RunsOn\">\"\" \"\"</rel>\n";
 _myfile<<"<rel name=\"DQRegions\" num=\"1\">\n";
 _myfile<<"    \"DQRegion\" \""+_regionname+"\"\n";
 _myfile<<"</rel>\n";
_myfile<<"</obj>\n";


}


void 
dqm_algorithms::tools::DumpConfig::WriteThresholdFromMap(std::map<std::string,double> object,std::string _ParameterName,std::string Name){
  int objsize=object.size();
  char line[500];
sprintf(line, "<rel name=\"%sThresholds\" num=\"%d\">\n", Name.c_str(),objsize);
 _myfile<<line;
 std::map<std::string,double>::const_iterator iter;
 
 for (iter=object.begin();iter!=object.end();iter++){ 
   if (Name == "Red"){
     red_id.push_back(Name+"Thresh_"+_ParameterName+"_"+iter->first);
   }else {
     green_id.push_back(Name+"Thresh_"+_ParameterName+"_"+iter->first);
  }
   
   _myfile <<"     \"DQThreshold\"  \""+Name+"Thresh_"+_ParameterName+"_"+iter->first+"\"\n";
   
 }
 _myfile<<"</rel>\n";
 
}

void
dqm_algorithms::tools::DumpConfig::DumpThresholds(){
  
  std::map<std::string,double>::iterator iter;
  int count=0;
  char line[500];
  std::map<std::string,double> thresh;
  std::vector<std::string> id;
  for (int i=0; i<2;++i){
    if (i== 0 ) {
      thresh=gthresh;
      id=green_id;
    }else {
	thresh=rthresh; 
        id=red_id;
    }
    
    for (iter=thresh.begin();iter!=thresh.end();iter++){ 
      _myfile<<"<obj class=\"DQThreshold\" id=\""+id[count]+"\">\n";
      _myfile<<"  <attr name=\"Name\" type=\"string\">\""+iter->first+"\"</attr>\n";
      sprintf(line, "  <attr name=\"Value\" type=\"double\">%4.2f</attr>\n</obj>\n\n",iter->second);
      _myfile<<line;
      ++count;
    }
    count=0;
  }
  
}

void
dqm_algorithms::tools::DumpConfig::DumpParams(){
  
  std::map<std::string,double>::const_iterator iter;
  char pline[500];
  int count=0;
  
  for (iter=params.begin();iter!=params.end();iter++){
    _myfile<<"<obj class=\"DQAlgorithmParameter\" id=\""+param_id[count]+"\">\n";
    _myfile<<"  <attr name=\"Name\" type=\"string\">\""+iter->first+"\"</attr>\n";
    sprintf(pline, "  <attr name=\"Value\" type=\"double\" num=\"1\">%4.2f</attr>\n</obj>\n\n",iter->second);
    _myfile<<pline;
    ++count;
  }
  
}



void
dqm_algorithms::tools::DumpConfig::DumpOnlineConfig(std::string filename, bool dumpAgent) {
  
  _myfile.open(filename.c_str());
  //open file
  int length;
  char * buffer;

  ifstream is;
  is.open ("../share/oks-xml-header.txt", ios::in );

  // get length of file:
  is.seekg (0, ios::end);
  length = is.tellg();
  is.seekg (0, ios::beg);

  // allocate memory:
  buffer = new char [length];

  // read data as a block:
  is.read (buffer,length);
  is.close();

  _myfile.write (buffer,length);

  delete[] buffer;  

 if (_regionname != ""){
    DumpRegion();
    if (dumpAgent) {
	DumpAgent();
    }
 } 

  _myfile << "<obj class=\"DQParameter\" id=\""+_ParameterName+"\">\n";
  _myfile <<"<attr name=\"InputDataSource\" type=\"string\">\"Histogramming.PT."+_histogramname+"\"</attr>\n";
  
  char weightline [500];
  sprintf(weightline, "<attr name=\"Weight\" type=\"float\">%f</attr>\n", _weight);
  _myfile<<weightline;
  _myfile <<"<rel name=\"Algorithm\">  \"DQAlgorithm\"  \""+_algorithmname+"\"</rel>\n";
  _myfile <<"<attr name=\"Action\" type=\"string\">\"\"</attr>\n";
  
  // Parameters....
  params=_config.getParameters();
  int paramsize=params.size();
  
  char paramsline [500];
  sprintf(paramsline, "<rel name=\"AlgorithmParameters\" num=\"%d\">\n", paramsize);
_myfile <<paramsline;
 
 std::map<std::string,double>::const_iterator iter;
 for (iter=params.begin();iter!=params.end();iter++){
   _myfile <<"     \"DQAlgorithmParameter\" \"Params_"+_ParameterName+"_"+iter->first+"\"\n";
   param_id.push_back("Params_"+_ParameterName+"_"+iter->first);
 }
 _myfile<<"</rel>\n";
 
 // Reference histogram
 if (_refhistogramname == "" && _reffilename=="") {
   _myfile<<"<rel name=\"References\" num=\"0\"> </rel>\n";
 }else {
   _myfile<<"<rel name=\"References\" num=\"1\">\"DQReference\"  \"ref_"+_ParameterName+"\"</rel>\n";
 }
 
 //Green Threshold............
 gthresh=_config.getGreenThresholds();
 WriteThresholdFromMap(gthresh,_ParameterName,"Green");
 
 //Red Threshold............
 rthresh=_config.getRedThresholds();
 DumpConfig::WriteThresholdFromMap(rthresh,_ParameterName,"Red");
 
 _myfile<<"</obj>\n\n\n";

//References
 if (_refhistogramname != "" && _reffilename != "") {
 _myfile<<"<obj class=\"DQReference\" id=\"ref_"+_ParameterName+"\">\n";
 _myfile<<"<attr name=\"Reference\" type=\"string\">\""+_reffilename+":"+_refhistogramname+"\"</attr>\n";
 _myfile<<"<attr name=\"ExternalConditionName\" type=\"string\">\"\"</attr>\n";
 _myfile<<"<attr name=\"ExternalConditionValue\" type=\"double\">0</attr>\n";
 _myfile<<"</obj>\n\n";
}
 DumpThresholds();
 
 DumpParams();

 
  _myfile<<"</oks-data>";

 _myfile.close();

}

void
dqm_algorithms::tools::DumpConfig::DumpOfflineConfig(std::string filename) {

  _myfile.open(filename.c_str());

  _myfile.close();

}
