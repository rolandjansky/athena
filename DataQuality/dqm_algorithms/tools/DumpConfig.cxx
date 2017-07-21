/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <dqm_algorithms/tools/DumpConfig.h>

using namespace std;

dqm_algorithms::tools::DumpConfig::DumpConfig(std::string ParameterName,dqm_core::test::DummyAlgorithmConfig & config, std::string algorithmname, std::string histogramname,  std::string reffilename,std::string refhistogramname, float weight, std::string regionname){
m_ParameterName=ParameterName;
 m_config=config;
 m_algorithmname=algorithmname;
 m_histogramname=histogramname;
 m_weight=weight;
 m_refhistogramname=refhistogramname;
 m_reffilename=reffilename;
 m_regionname=regionname;
 
}

dqm_algorithms::tools::DumpConfig::~DumpConfig(){
}

void
dqm_algorithms::tools::DumpConfig::DumpRegion() {
 m_myfile<<"<obj class=\"DQRegion\" id=\""+m_regionname+"\">\n";
 m_myfile<<"<attr name=\"InputDataSource\" type=\"string\">\"\"</attr>\n";
 m_myfile<<"<attr name=\"Weight\" type=\"float\">1</attr>\n";
 m_myfile<<"<attr name=\"Action\" type=\"string\">\"\"</attr>\n";
 m_myfile<<"<rel name=\"Algorithm\">\"DQAlgorithm\" \"SimpleSummary\"</rel>\n";
 m_myfile<<"<rel name=\"AlgorithmParameters\" num=\"0\"></rel>\n";
 m_myfile<<"<rel name=\"References\" num=\"0\"></rel>\n";
 m_myfile<<"<rel name=\"GreenThresholds\" num=\"0\"></rel>\n";
 m_myfile<<"<rel name=\"RedThresholds\" num=\"0\"></rel>\n";
 m_myfile<<"<rel name=\"DQRegions\" num=\"0\"></rel>\n";
 m_myfile<<"<rel name=\"DQParameters\" num=\"1\">\n";
 m_myfile<<"   \"DQParameter\"   \""+m_ParameterName+"\"\n";
 m_myfile<<"</rel>\n";
m_myfile<<"</obj>\n\n";

}

void
dqm_algorithms::tools::DumpConfig::DumpAgent() {  
 m_myfile<<"<obj class=\"DQAgent\" id=\""+m_regionname+"\">\n";
 m_myfile<<"<attr name=\"Parameters\" type=\"string\">\"-n "+m_regionname+" -p ${TDAQ_PARTITION}\"</attr>\n";
 m_myfile<<"<attr name=\"RestartParameters\" type=\"string\">\"-n "+m_regionname+" -p ${TDAQ_PARTITION}\"</attr>\n";
 m_myfile<<"<attr name=\"ControlledByOnline\" type=\"bool\">1</attr>\n";
 m_myfile<<"<attr name=\"IfDies\" type=\"enum\">\"Restart\"</attr>\n";
 m_myfile<<"<attr name=\"IfFailed\" type=\"enum\">\"Restart\"</attr>\n";
 m_myfile<<"<attr name=\"StartAt\" type=\"enum\">\"Boot\"</attr>\n";
 m_myfile<<"<attr name=\"StopAt\" type=\"enum\">\"Shutdown\"</attr>\n";
 m_myfile<<"<attr name=\"InitTimeout\" type=\"u32\">0</attr>\n";
 m_myfile<<"<attr name=\"StartIn\" type=\"string\">\"\"</attr>\n";
 m_myfile<<"<attr name=\"InputDevice\" type=\"string\">\"\"</attr>\n";
 m_myfile<<"<attr name=\"Logging\" type=\"bool\">1</attr>\n";
 m_myfile<<"<rel name=\"InitializationDependsFrom\" num=\"0\">\n";
 m_myfile<<"</rel>\n";
 m_myfile<<"<rel name=\"ShutdownDependsFrom\" num=\"0\"></rel>\n";
 m_myfile<<"<rel name=\"Program\">\"Binary\" \"dqmf_agent\"</rel>\n";
 m_myfile<<"<rel name=\"ExplicitTag\">\"\" \"\"</rel>\n";
 m_myfile<<"<rel name=\"Uses\" num=\"0\"></rel>\n";
 m_myfile<<"<rel name=\"ProcessEnvironment\" num=\"0\">\n";
 m_myfile<<"</rel>\n";
 m_myfile<<"<rel name=\"RunsOn\">\"\" \"\"</rel>\n";
 m_myfile<<"<rel name=\"DQRegions\" num=\"1\">\n";
 m_myfile<<"    \"DQRegion\" \""+m_regionname+"\"\n";
 m_myfile<<"</rel>\n";
m_myfile<<"</obj>\n";


}


void 
dqm_algorithms::tools::DumpConfig::WriteThresholdFromMap(std::map<std::string,double> object,std::string ParameterName,std::string Name){
  int objsize=object.size();
  char line[500];
sprintf(line, "<rel name=\"%sThresholds\" num=\"%d\">\n", Name.c_str(),objsize);
 m_myfile<<line;
 std::map<std::string,double>::const_iterator iter;
 
 for (iter=object.begin();iter!=object.end();iter++){ 
   if (Name == "Red"){
     m_red_id.push_back(Name+"Thresh_"+ParameterName+"_"+iter->first);
   }else {
     m_green_id.push_back(Name+"Thresh_"+ParameterName+"_"+iter->first);
  }
   
   m_myfile <<"     \"DQThreshold\"  \""+Name+"Thresh_"+ParameterName+"_"+iter->first+"\"\n";
   
 }
 m_myfile<<"</rel>\n";
 
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
      thresh=m_gthresh;
      id=m_green_id;
    }else {
	thresh=m_rthresh; 
        id=m_red_id;
    }
    
    for (iter=thresh.begin();iter!=thresh.end();iter++){ 
      m_myfile<<"<obj class=\"DQThreshold\" id=\""+id[count]+"\">\n";
      m_myfile<<"  <attr name=\"Name\" type=\"string\">\""+iter->first+"\"</attr>\n";
      sprintf(line, "  <attr name=\"Value\" type=\"double\">%4.2f</attr>\n</obj>\n\n",iter->second);
      m_myfile<<line;
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
  
  for (iter=m_params.begin();iter!=m_params.end();iter++){
    m_myfile<<"<obj class=\"DQAlgorithmParameter\" id=\""+m_param_id[count]+"\">\n";
    m_myfile<<"  <attr name=\"Name\" type=\"string\">\""+iter->first+"\"</attr>\n";
    sprintf(pline, "  <attr name=\"Value\" type=\"double\" num=\"1\">%4.2f</attr>\n</obj>\n\n",iter->second);
    m_myfile<<pline;
    ++count;
  }
  
}



void
dqm_algorithms::tools::DumpConfig::DumpOnlineConfig(std::string filename, bool dumpAgent) {
  
  m_myfile.open(filename.c_str());
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

  m_myfile.write (buffer,length);

  delete[] buffer;  

 if (m_regionname != ""){
    DumpRegion();
    if (dumpAgent) {
	DumpAgent();
    }
 } 

  m_myfile << "<obj class=\"DQParameter\" id=\""+m_ParameterName+"\">\n";
  m_myfile <<"<attr name=\"InputDataSource\" type=\"string\">\"Histogramming.PT."+m_histogramname+"\"</attr>\n";
  
  char weightline [500];
  sprintf(weightline, "<attr name=\"Weight\" type=\"float\">%f</attr>\n", m_weight);
  m_myfile<<weightline;
  m_myfile <<"<rel name=\"Algorithm\">  \"DQAlgorithm\"  \""+m_algorithmname+"\"</rel>\n";
  m_myfile <<"<attr name=\"Action\" type=\"string\">\"\"</attr>\n";
  
  // Parameters....
  m_params=m_config.getParameters();
  int paramsize=m_params.size();
  
  char paramsline [500];
  sprintf(paramsline, "<rel name=\"AlgorithmParameters\" num=\"%d\">\n", paramsize);
m_myfile <<paramsline;
 
 std::map<std::string,double>::const_iterator iter;
 for (iter=m_params.begin();iter!=m_params.end();iter++){
   m_myfile <<"     \"DQAlgorithmParameter\" \"Params_"+m_ParameterName+"_"+iter->first+"\"\n";
   m_param_id.push_back("Params_"+m_ParameterName+"_"+iter->first);
 }
 m_myfile<<"</rel>\n";
 
 // Reference histogram
 if (m_refhistogramname == "" && m_reffilename=="") {
   m_myfile<<"<rel name=\"References\" num=\"0\"> </rel>\n";
 }else {
   m_myfile<<"<rel name=\"References\" num=\"1\">\"DQReference\"  \"ref_"+m_ParameterName+"\"</rel>\n";
 }
 
 //Green Threshold............
 m_gthresh=m_config.getGreenThresholds();
 WriteThresholdFromMap(m_gthresh,m_ParameterName,"Green");
 
 //Red Threshold............
 m_rthresh=m_config.getRedThresholds();
 DumpConfig::WriteThresholdFromMap(m_rthresh,m_ParameterName,"Red");
 
 m_myfile<<"</obj>\n\n\n";

//References
 if (m_refhistogramname != "" && m_reffilename != "") {
 m_myfile<<"<obj class=\"DQReference\" id=\"ref_"+m_ParameterName+"\">\n";
 m_myfile<<"<attr name=\"Reference\" type=\"string\">\""+m_reffilename+":"+m_refhistogramname+"\"</attr>\n";
 m_myfile<<"<attr name=\"ExternalConditionName\" type=\"string\">\"\"</attr>\n";
 m_myfile<<"<attr name=\"ExternalConditionValue\" type=\"double\">0</attr>\n";
 m_myfile<<"</obj>\n\n";
}
 DumpThresholds();
 
 DumpParams();

 
  m_myfile<<"</oks-data>";

 m_myfile.close();

}

void
dqm_algorithms::tools::DumpConfig::DumpOfflineConfig(std::string filename) {

  m_myfile.open(filename.c_str());

  m_myfile.close();

}
