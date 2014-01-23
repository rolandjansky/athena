/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// utils.cxx file
///---------------------------------------------------
/// Various useful functions
///
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of B2JpsiV0RootAnalysis
///===================================================

#ifndef utils_CPP
#define utils_CPP

#include<string>
#include<vector>

///===================================================
// class that converts list of strings into a std::vector
struct NameList{
  //convertor
  NameList(std::string name1,     std::string name2 ="", std::string name3 ="",
           std::string name4 ="", std::string name5 ="", std::string name6 ="",
           std::string name7 ="", std::string name8 ="", std::string name9 ="",
           std::string name10="", std::string name11="", std::string name12="") : names(0)
  {
    names.push_back(name1);
    if(name2  != "") names.push_back( name2 );
    if(name3  != "") names.push_back( name3 );
    if(name4  != "") names.push_back( name4 );
    if(name5  != "") names.push_back( name5 );
    if(name6  != "") names.push_back( name6 );
    if(name7  != "") names.push_back( name7 );
    if(name8  != "") names.push_back( name8 );
    if(name9  != "") names.push_back( name9 );
    if(name10 != "") names.push_back( name10 );
    if(name11 != "") names.push_back( name11 );
    if(name12 != "") names.push_back( name12 );
  }

  // search function
  int getIndex(std::string name) {
    std::vector<std::string>::iterator namesItr = names.begin();
    for(int i=0; namesItr!=names.end(); ++namesItr, ++i) {
      if(*namesItr == name) return i;
    }
    // name not found
    return -1;
  }

  // public attribute: std::vector of strings filled by constructor
  std::vector<std::string> names;
};
///===================================================
std::vector<std::string> listFiles(std::string dirPath){

  std::vector<std::string> files;

  void *dir = gSystem->OpenDirectory(dirPath.c_str());
  if (dir==NULL) {
    return files;
  }

//   cout << dirPath << endl;
  const char* file;

  while ((file = gSystem->GetDirEntry(dir))) {
    if (strcmp(file,".")!=0 && strcmp(file,"..")!=0) {
      files.push_back(std::string(file));
    }
  }

  gSystem->FreeDirectory(dir);

  return files;
}
///===================================================
std::vector<std::string> explode(const char* text, char* delims)
{

  std::vector<std::string> tmp;

  char str[10000];
  sprintf(str, "%s", text);

  char *result = NULL;
  result = strtok( str, delims );
  while( result != 0 ) {
    tmp.push_back(std::string(result));
    result = strtok(0, delims);
  }

  return tmp;
}
///===================================================
double getDecayAngle(TLorentzVector lambda, TLorentzVector proton) {

  //calculate normal to the production plane
  TVector3 zHat(0,0,1);
  TVector3 norm = (lambda.Vect().Cross(zHat)).Unit();

  //boost proton into lambda rest frame
  TVector3 boost = -lambda.BoostVector();
  proton.Boost(boost);

  //calculate decay angle measured from the normal
  return norm.Angle(proton.Vect());

}
///===================================================
bool getValue(std::string settings, std::string variableName, double& value) {
  std::string::size_type pos = settings.find(variableName);
  if(pos!=std::string::npos) {
    stringstream ss(settings.substr(pos+variableName.length()));
    ss >> value;
    return true;
  }
  return false;
}
///===================================================
bool getValue(std::string settings, std::string variableName, int& value) {
  std::string::size_type pos = settings.find(variableName);
  if(pos!=std::string::npos) {
    stringstream ss(settings.substr(pos+variableName.length()));
    ss >> value;
    return true;
  }
  return false;
}
///===================================================
bool getValue(std::string settings, std::string variableName, std::string& value) {
  //NOTE string variable must be enclosed in comas, e.g. name="Hist"

  std::string::size_type pos    = settings.find( variableName + "\"" );
  std::string::size_type posEnd = settings.substr( pos + variableName.length() + 1 ).find( "\"" );

  if(pos!=std::string::npos) {
    if(posEnd!=std::string::npos) {
      value = settings.substr(pos+variableName.length() + 1, posEnd);
    }else{
      value = settings.substr(pos+variableName.length() + 1, settings.length() - pos - variableName.length() - 1);
    }
    return true;
  }
  return false;
}
///===================================================
double dAngle(double phi1, double phi2) {
  double dPhi = phi1 - phi2;
  while(dPhi >  M_PI) dPhi =  2*M_PI - dPhi;
  while(dPhi < -M_PI) dPhi = -2*M_PI - dPhi;

  return dPhi;
}
///===================================================
double getEta(double r, double z) {
  double theta = atan2(r,z);
  return -log(tan(theta/2));
}
///===================================================
std::vector<std::string> explode(std::string str, std::string delim) {
  std::string::size_type pos=0;

  std::string tmpStr = str;
  std::vector<std::string> result;

  while(pos!=std::string::npos) {
    // get token
    std::string::size_type pos2 = tmpStr.find(delim);

    if(pos2!=std::string::npos) {
      std::string token = tmpStr.substr(pos, pos2);

      // fill the result
      result.push_back(token);

      // prepare for the next token
      pos = pos2 + delim.length();
      tmpStr = tmpStr.substr(pos,tmpStr.length());

    }else {
      pos = std::string::npos;
      result.push_back(tmpStr);
    }
  }

  return result;
}
///===================================================
///      . .
///====o==V==o========================================

#endif
