/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MVAUtils/BDT.h"

#include "TMVA/Reader.h"
#include "TMVA/MethodBDT.h"

#include <TString.h>
#include <TXMLEngine.h>
#include <TSystem.h>
#include <TFile.h>
#include <TRandom3.h>

#include <vector>
#include <iostream>

using namespace std;

/** 
    A utility to convert xml files from TMVA into root TTrees for this package.

    Usage: convertXmlToRootTree <inFile(xml)> [outFile(root)]

    If outFile is not given, then it is named based on the input name

    @author J Griffith
 */

struct XmlVariableInfo {
  TString expression;
  TString label;
  TString varType;
  TString nodeName;
  float min;
  float max;
};
TString AnalysisType;
unsigned int NClass;


std::vector<XmlVariableInfo>
parseVariables(TXMLEngine *xml, void* node, const TString & nodeName)
{
  std::vector<XmlVariableInfo> result;
  if (!xml || !node) return result;

  // loop over all children inside <Variables> or <Spectators>
  for (XMLNodePointer_t info_node = xml->GetChild(node); info_node != 0;
       info_node = xml->GetNext(info_node))
  {
    XMLAttrPointer_t attr = xml->GetFirstAttr(info_node);
    XmlVariableInfo varInfo;
    // loop over the attributes of each child
    while (attr != 0)
    {
      TString name = xml->GetAttrName(attr);
      if (name == "Expression")
        varInfo.expression = xml->GetAttrValue(attr);
      else if (name == "Label")
        varInfo.label = xml->GetAttrValue(attr);
      else if (name == "Type")
        varInfo.varType = xml->GetAttrValue(attr);
      else if (name == "Min") varInfo.min=TString(xml->GetAttrValue(attr)).Atof();
      else if (name == "Max") varInfo.max=TString(xml->GetAttrValue(attr)).Atof();
      
      attr = xml->GetNextAttr(attr);
    }
    //          ATH_MSG_DEBUG("Expression: " << expression << " Label: " << label << " varType: " << varType);
    varInfo.nodeName = nodeName;
    result.push_back(varInfo);
  }
  return result;
}

std::vector<XmlVariableInfo>
parseXml(const TString & xml_filename)
{
  std::vector<XmlVariableInfo> result;

  TXMLEngine xml;
  XMLDocPointer_t xmldoc = xml.ParseFile(xml_filename);
  if (!xmldoc) {
    std::cerr<<" file not found "  <<xml_filename.Data() << " current directory is:  " <<  gSystem->WorkingDirectory()<<std::endl;
    gSystem->Abort();
  }
  XMLNodePointer_t mainnode = xml.DocGetRootElement(xmldoc);

  // loop to find <Variables> and <Spectators>
  XMLNodePointer_t node = xml.GetChild(mainnode);
  while (node)
  {
    TString nodeName = xml.GetNodeName(node);
    if (nodeName == "Variables" || nodeName == "Spectators") {
      std::vector<XmlVariableInfo> r = parseVariables(&xml, node, nodeName);
      result.insert(result.end(), r.begin(), r.end());
    }
    else if (nodeName == "GeneralInfo" && node) {
      for (XMLNodePointer_t info_node = xml.GetChild(node); info_node != 0;
	   info_node = xml.GetNext(info_node)){
	XMLAttrPointer_t attr = xml.GetFirstAttr(info_node);
	// loop over the attributes of each child
	while (attr != 0) {
          // TString name = xml.GetAttrName(attr);
	  TString value = xml.GetAttrValue(attr);
	  attr = xml.GetNextAttr(attr);
          if (value == "AnalysisType")
            AnalysisType = TString(xml.GetAttrValue(attr));
        }
      }
    }
    else if (nodeName == "Classes") {
      NClass = TString(xml.GetAttr(node,"NClass")).Atoi();
    }
    node = xml.GetNext(node);
  }
  xml.FreeDoc(xmldoc);
  return result;
}

int main(int argc, char** argv){
  TRandom3 rand;
  //float dummyFloat;
  TMVA::Reader *reader = new TMVA::Reader("Silent");

  TString xmlFileName="";
  if(argc>1) xmlFileName=argv[1];
  else return 0;
  int last_slash=xmlFileName.Last('/');
  last_slash = (last_slash<0 ? 0 : last_slash+1);
  TString outFileName=xmlFileName(last_slash, xmlFileName.Length()-last_slash+1);
  outFileName+=".root";
  outFileName.ReplaceAll(".xml.root", ".root");
  if(argc>2) outFileName=argv[2];
  
  std::vector<XmlVariableInfo> variable_infos = parseXml(xmlFileName);
  bool isRegression = ((AnalysisType != "Regression") ? false : true);
  bool isMulti = ((AnalysisType == "Multiclass") ? true : false);
  TString varList;
  vector<float*> vars;
  vector<float> var_avgerage;


  cout << "Boosted Decision Tree for " << AnalysisType << endl;

  for (std::vector<XmlVariableInfo>::const_iterator itvar = variable_infos.begin();
       itvar != variable_infos.end(); ++itvar){

    TString infoType  = (TString(itvar->nodeName).Contains("Variable") ?
                         "variable" : "spectator");
    TString expression = itvar->expression;
    TString varName    = itvar->label;
    TString type       = itvar->varType;

    TString varDefinition(varName);
    if (varName != expression){
      varDefinition += " := " + expression;
    }
    
    float average_value = (itvar->min+itvar->max)/2 ;
    var_avgerage.push_back(average_value);
    vars.push_back(new float(average_value));
    if (infoType == "variable"){
      varList+=varDefinition+",";
      reader->AddVariable(varDefinition, vars.back());
      cout << "Add variable: " << varDefinition << " " << type << endl;
    }
    else if (infoType == "spectator"){
      reader->AddSpectator(varDefinition, vars.back());
      cout << "Add spectator: " << varDefinition << " " << type << endl;
    }
    else // should never happen
      {
	cerr <<"Unknown type from parser "<< infoType.Data()<<endl;
	//throw std::runtime_error("Unknown type from parser");
	//	delete vars.back();
	vars.pop_back();
	return 0;
      }
  }

  reader->BookMVA("BDTG", xmlFileName);

  TMVA::MethodBDT* method_bdt = dynamic_cast<TMVA::MethodBDT*> (reader->FindMVA("BDTG"));
  bool useYesNoLeaf = false;
  bool isGrad = false;
  if(method_bdt->GetOptions().Contains("UseYesNoLeaf=True")) useYesNoLeaf = true;
  if(method_bdt->GetOptions().Contains("BoostType=Grad")) isGrad = true;
  cout << "UseYesNoLeaf? " << useYesNoLeaf << endl;
  cout << "Gradient Boost? " << isGrad << endl;
  MVAUtils::BDT* bdt = new MVAUtils::BDT( method_bdt, isRegression || isGrad, useYesNoLeaf);
  bdt->SetPointers(vars);


  cout << endl << "Testing MVA produced from TMVA::Reader " << endl;

  cout << "MVAUtils::BDT : "
       << (isRegression ? bdt->GetResponse() : isMulti ? bdt->GetMultiResponse(NClass)[NClass-1] : isGrad ? bdt->GetGradBoostMVA(vars) : bdt->GetClassification())
       << " , TMVA::Reader : "
       << (isRegression ? reader->EvaluateRegression(0, "BDTG") : isMulti ? reader->EvaluateMulticlass("BDTG")[NClass-1] : reader->EvaluateMVA("BDTG"))
       << endl;

  for(uint i = 0; i != vars.size(); ++i) *vars[i] = 0;
  cout << "MVAUtils::BDT : "
       << (isRegression ? bdt->GetResponse() : isMulti ? bdt->GetMultiResponse(vars,NClass)[NClass-1] : isGrad ? bdt->GetGradBoostMVA(vars) : bdt->GetClassification())
       << " , TMVA::Reader : "
       << (isRegression ? reader->EvaluateRegression(0, "BDTG") : isMulti ? reader->EvaluateMulticlass("BDTG")[NClass-1] : reader->EvaluateMVA("BDTG"))
       << endl;


  cout << "Writing MVAUtils::BDT in " << outFileName << endl;
  TFile* f = TFile::Open(outFileName, "RECREATE");
  bdt->WriteTree("BDT")->Write();
  TNamed* n  = new TNamed("varList", varList.Data());
  n->Write();
  f->Close();
  delete f;
  delete bdt;
  bdt = nullptr;


  cout << endl << "Reading BDT from root file and testing " << outFileName << endl;

  f = TFile::Open(outFileName, "READ");
  TTree* bdt_tree = dynamic_cast<TTree*> (f->Get("BDT"));
  if(!bdt_tree){
    cerr <<"Could not Retrieve BDT TTree from file , should not happen" <<endl;
    return 0;
  }
  
  bdt = new MVAUtils::BDT(bdt_tree);
  bdt->SetPointers(vars);
  cout << bdt->GetResponse() << endl;
  cout << "MVAUtils::BDT : "
       << (isRegression ? bdt->GetResponse() : isMulti ? bdt->GetMultiResponse(NClass)[NClass-1] : isGrad ? bdt->GetGradBoostMVA(vars) : bdt->GetClassification())
       << " , TMVA::Reader : "
       << (isRegression ? reader->EvaluateRegression(0, "BDTG") : isMulti ? reader->EvaluateMulticlass("BDTG")[NClass-1] : reader->EvaluateMVA("BDTG"))
       << endl;
  for(uint i = 0; i != vars.size(); ++i) *vars[i] = var_avgerage[i]; 
  cout << "MVAUtils::BDT : "
       << (isRegression && !isGrad ? bdt->GetResponse() : isMulti ? bdt->GetMultiResponse(NClass)[NClass-1] : isGrad ? bdt->GetGradBoostMVA(vars) : bdt->GetClassification())
       << " , TMVA::Reader : "
       << (isRegression ? reader->EvaluateRegression(0, "BDTG") : isMulti ? reader->EvaluateMulticlass("BDTG")[NClass-1] : reader->EvaluateMVA("BDTG"))
       << endl;

  cout << "Checking over many random events" << endl;
  int n_events=0;
  for(int i = 0; i != 100; ++i){
    for(uint i = 0; i != vars.size(); ++i) *vars[i] = (1+(rand.Rndm()-0.5)/5)*var_avgerage[i];
    float mva = (isRegression ? bdt->GetResponse() : isMulti ? bdt->GetMultiResponse(vars,NClass)[NClass-1] : isGrad ? bdt->GetGradBoostMVA(vars) : bdt->GetClassification());
    float tmva = (isRegression ? reader->EvaluateRegression(0, "BDTG") : isMulti ? reader->EvaluateMulticlass("BDTG")[NClass-1] : reader->EvaluateMVA("BDTG"));
    if( (tmva-mva)/mva > 0.00001 ){
      cout << "MVAUtils::BDT : " << mva << " , TMVA::Reader : " << tmva << endl;
      n_events++;
    }
  }
  cout << "Found " << n_events << " events in disagreement " << endl;
  cout << endl;
}
