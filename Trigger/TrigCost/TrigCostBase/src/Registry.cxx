/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cstring>
#include <fstream>
#include <sstream>

// Local
#include "TrigCostBase/UtilCore.h"
#include "TrigCostBase/Registry.h"

using namespace std;

//----------------------------------------------------------------------------------
void Anp::Registry::Merge(const Registry &rhs)
{
  //
  // Copy rhs into this: replace value if key already exists
  //
  for(unsigned int i = 0; i < rhs.fStr.size(); ++i) Set(rhs.fStr[i].Key(), rhs.fStr[i].Data());
  for(unsigned int i = 0; i < rhs.fDbl.size(); ++i) Set(rhs.fDbl[i].Key(), rhs.fDbl[i].Data());
  for(unsigned int i = 0; i < rhs.fReg.size(); ++i) Set(rhs.fReg[i].Key(), rhs.fReg[i].Data());
}

//----------------------------------------------------------------------------------
void Anp::Registry::Add(const Registry &rhs)
{
  //
  // Merge registry by adding new string values to already existing strings
  //
  for(unsigned int i = 0; i < rhs.fDbl.size(); ++i) { 
    Set(rhs.fDbl[i].Key(), rhs.fDbl[i].Data());
  }

  for(unsigned int i = 0; i < rhs.fStr.size(); ++i) { 
    const string &key = rhs.fStr[i].Key();

    string val;
    if(Get(key, val)) {
      val += rhs.fStr[i].Data();
    }
    else {
      val  = rhs.fStr[i].Data();
    }

    Set(key, val);
  }

  for(unsigned int i = 0; i < rhs.fReg.size(); ++i) { 
    const string &key = rhs.fReg[i].Key();

    Registry val;
    if(Get(key, val)) {
      val.Add(fReg[i].Data());
    }
    else {
      val = rhs.fReg[i].Data();
    }

    Set(key, val);
  }
}

//----------------------------------------------------------------------------------
bool Anp::Registry::KeyExists(const std::string &key) const
{
  if(Anp::KeyExists<std::string>(fStr, key)) return true;
  if(Anp::KeyExists<double>     (fDbl, key)) return true;
  if(Anp::KeyExists<Registry>   (fReg, key)) return true;

  return false;
}

//----------------------------------------------------------------------------------
bool Anp::Registry::RemoveKey(const std::string &key)
{
  if(Anp::RemoveKey<std::string>(fStr, key)) return true;
  if(Anp::RemoveKey<double>     (fDbl, key)) return true;
  if(Anp::RemoveKey<Registry>   (fReg, key)) return true;

  return false;
}

//----------------------------------------------------------------------------------
void Anp::Registry::Clear()
{
  fStr.clear();
  fDbl.clear();
  fReg.clear();
}

//----------------------------------------------------------------------------------
bool Anp::Registry::Get(const std::string &key, std::vector<std::string> &val) const
{
  std::string tmp;
  if(!Anp::GetVal<std::string>(fStr, key, tmp)) return false;
  Anp::StringTok(val, tmp, ", ");
  return true;
}

//----------------------------------------------------------------------------------
bool Anp::Registry::Get(const std::string &key, std::vector<int> &val) const
{
  std::string tmp;
  std::vector<std::string> vec;

  if(!Anp::GetVal<std::string>(fStr, key, tmp)) return false;
  Anp::StringTok(vec, tmp, ", ");

  for(unsigned int i = 0; i < vec.size(); ++i) {
    std::stringstream valS;
    int val_tmp = 0;
    
    valS << vec[i];
    valS >> val_tmp;
    
    if(valS.fail()) {
      std::cerr << "Registry::Get - stringstream operator>> failed for: " << vec[i] << std::endl;
    } else {
      val.push_back(val_tmp);
    }
  }
  
  return true;
}

//----------------------------------------------------------------------------------
bool Anp::Registry::Get(const std::string &key, std::vector<double> &val) const
{
  std::string tmp;
  std::vector<std::string> vec;

  if(!Anp::GetVal<std::string>(fStr, key, tmp)) return false;
  Anp::StringTok(vec, tmp, ", ");

  for(unsigned int i = 0; i < vec.size(); ++i) {
    std::stringstream valS;
    double val_tmp = 0.;
    
    valS << vec[i];
    valS >> val_tmp;
    
    if(valS.fail()) {
      std::cerr << "Registry::Get - stringstream operator>> failed for: " << vec[i] << std::endl;
    } else {
      val.push_back(val_tmp);
    }
  }
  
  return true;
}

//----------------------------------------------------------------------------------
const std::vector<Anp::DataPair<std::string, std::string> >& Anp::Registry::GetStr() const
{
  return fStr;
}

//----------------------------------------------------------------------------------
const std::vector<Anp::DataPair<std::string, double> >&      Anp::Registry::GetDbl() const
{
  return fDbl;
}

//----------------------------------------------------------------------------------
const std::vector<Anp::DataPair<std::string, Anp::Registry> >& Anp::Registry::GetReg() const
{
  return fReg;
}

//----------------------------------------------------------------------------------
namespace Reg
{
  std::ostream& Pad(std::ostream &os, unsigned int margin) {
    for(unsigned int i = 0; i < margin; ++i) {
      os << ' ';
    }
    
    return os;
  }
}

//----------------------------------------------------------------------------------
void Anp::Registry::Print(std::ostream &os,
			  unsigned int margin,
			  const std::string &key) const
{
  Reg::Pad(os, margin) << "Registry::Print" << std::endl;

  Anp::Print<std::string>(fStr, os, "STRING", margin, key);
  Anp::Print<double>     (fDbl, os, "DOUBLE", margin, key);

  for(unsigned int i = 0; i < fReg.size(); ++i) {
    if(!key.empty() && fReg[i].Key().find(key) == std::string::npos) continue;

    Reg::Pad(os, margin) << "\"" << fReg[i].Key() << "\"" << std::endl;
    fReg[i].Data().Print(os, margin+3);
  }  
}

//----------------------------------------------------------------------------------
void Anp::Registry::Read(std::istream &) const
{
}

//----------------------------------------------------------------------------------
void Anp::Registry::PrintConfig(std::ostream &os, const std::string &key) const
{
  bool print_config = false; 
  if(Get(key+"::PrintConfig", print_config) && print_config) {
    os << key << "::PrintConfig" << std::endl;
    Print(os, 3, key);
  } 
}

//----------------------------------------------------------------------------------
void Anp::Registry::WriteXML(const std::string &path, unsigned left_pad) const
{
  //
  // Write self as XML file to "path"
  //
  
  std::ofstream outf(path.c_str());
  if(!outf.is_open()) { 
    cerr << "Registry::WriteXML - failed to open file: " << path << endl;    
    return;
  }
  
  WriteXML(outf, left_pad);
}

//----------------------------------------------------------------------------------
void Anp::Registry::WriteXML(std::ostream &outf, unsigned left_pad) const
{
  string pad1(left_pad+0, ' ');
  string pad2(left_pad+2, ' ');
  
  outf << pad1 << "<Registry>" << endl
       << pad2 << "<AllowNonUniqueKeys>" << !fUniqueKeys << "</AllowNonUniqueKeys>" << endl; 

  outf << pad2 << "<StringVector>" << endl;
  for(unsigned i = 0; i < fStr.size(); ++i) {
    outf << pad2 << "  <DataPair>" << endl 
	 << pad2 << "    <key>"  << fStr.at(i).Key()  << "</key>"  << endl
	 << pad2 << "    <data>" << fStr.at(i).Data() << "</data>" << endl
	 << pad2 << "  </DataPair>" << endl;
  }  
  outf << pad2 << "</StringVector>" << endl;

  outf << pad2 << "<DoubleVector>" << endl;
  for(unsigned i = 0; i < fDbl.size(); ++i) {
    outf << pad2 << "  <DataPair>" << endl 
	 << pad2 << "    <key>"  << fDbl.at(i).Key()  << "</key>"  << endl
	 << pad2 << "    <data>" << fDbl.at(i).Data() << "</data>" << endl
	 << pad2 << "  </DataPair>" << endl;
  }  
  outf << pad2 << "</DoubleVector>" << endl;

  outf << pad2 << "<RegistryVector>" << endl;
  for(unsigned i = 0; i < fReg.size(); ++i) {
    outf << pad2 << "  <DataPair>" << endl 
	 << pad2 << "    <key>"  << fReg.at(i).Key()  << "</key>"  << endl;
    fReg.at(i).Data().WriteXML(outf, left_pad+6);
    outf << pad2 << "  </DataPair>" << endl;
  }
  outf << pad2 << "</RegistryVector>" << endl;

  outf << pad1 << "</Registry>" << endl;
}


//----------------------------------------------------------------------------------
// Help function for reading Registry from XML
//
namespace Anp
{
  void FillReg(Registry &in_reg, Registry &out_reg)
  {
    //
    // Read additional keys
    //
    int ukey = 0;
    if(in_reg.Get("AllowNonUniqueKeys", ukey)) {
      if(ukey == 1) out_reg.AllowNonUniqueKeys();
    }
    
    //
    // Read string registry objects
    //    
    Registry str_reg;
    if(in_reg.Get("StringVector", str_reg)) {

      while(str_reg.KeyExists("DataPair")) {
	//
	// Read registry for one key
	//
	Registry key_reg;
	str_reg.Get("DataPair", key_reg);
	str_reg.RemoveKey("DataPair");
	
	string key, data;

	if(key_reg.Get("key",  key) && 
	   key_reg.Get("data", data)) {
	  out_reg.Set(key, data);
	}
	else {
	  cerr << "FillReg - error: bad DataPair in StringVector" << endl;
	}
      }
    }
  
    //
    // Read double registry objects
    //    
    Registry dbl_reg;
    if(in_reg.Get("DoubleVector", dbl_reg)) {

      while(dbl_reg.KeyExists("DataPair")) {
	//
	// Read registry for one key
	//
	Registry key_reg;
	dbl_reg.Get("DataPair", key_reg);
	dbl_reg.RemoveKey("DataPair");
	
	string key;
	double data = -2.0e100;

	if(key_reg.Get("key",  key) && 
	   key_reg.Get("data", data)) {
	  out_reg.Set(key, data);
	}
	else {
	  cerr << "FillReg - error: bad DataPair in DoubleVector: " << endl;
	  key_reg.Print();
	}
      }
    }
    
    //
    // Read "Registry" registry objects
    //    
    Registry reg_reg;
    if(in_reg.Get("RegistryVector", reg_reg)) {

      while(reg_reg.KeyExists("DataPair")) {
	//
	// Read registry for one key
	//
	Registry key_reg;
	reg_reg.Get("DataPair", key_reg);
	reg_reg.RemoveKey("DataPair");
	
	string key;
	Registry data;

	if(key_reg.Get("key",  key) && 
	   key_reg.Get("Registry", data)) {
	  //
	  // Recursive call to read child registry
	  //
	  Registry val;
	  FillReg(data, val);
	  out_reg.Set(key, val);
	}
	else {
	  cerr << "FillReg - error: bad DataPair in RegistryVector" << endl;
	}
      }
    }
  }
}

//----------------------------------------------------------------------------------
void Anp::Registry::ReadXML(const std::string &path)
{
  //
  // Read registry 
  //
  if(path.empty()) return;

  Registry xml_reg = Anp::ParseXml(path);
  
  string root_element;
  xml_reg.Get("RootElement", root_element);
  
  if(root_element != "Registry") {
    return;
  }
  
  //
  // Merge data into this Registry object
  //
  FillReg(xml_reg, *this);
}

