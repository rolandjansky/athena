/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: HanConfig.cxx,v 1.30 2009-05-07 14:45:54 ponyisi Exp $
// **********************************************************************

#include "DataQualityInterfaces/HanConfig.h"

#include <string.h>	// strncmp()

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>

#include <TCollection.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>
#include <TKey.h>
#include <TBox.h>
#include <TLine.h>
#include <TROOT.h>

#include "dqm_core/LibraryManager.h"
#include "dqm_core/Parameter.h"
#include "dqm_core/ParameterConfig.h"
#include "dqm_core/Region.h"
#include "dqm_core/RegionConfig.h"
#include "DataQualityInterfaces/HanAlgorithmConfig.h"
#include "DataQualityInterfaces/HanConfigAssessor.h"
#include "DataQualityInterfaces/HanConfigGroup.h"
#include "DataQualityInterfaces/HanConfigAlgPar.h"
#include "DataQualityInterfaces/HanConfigAlgLimit.h"
#include "DataQualityInterfaces/HanConfigMetadata.h"
#include "DataQualityInterfaces/CompositeAlgorithm.h"
#include "DataQualityInterfaces/HanOutput.h"
#include "DataQualityInterfaces/MiniConfig.h"
#include "DataQualityInterfaces/HanUtils.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/pointer_cast.hpp>
#include "DataQualityInterfaces/ConditionsSingleton.h"

ClassImp(dqi::HanConfig)

static TBox box;
static TLine line;

namespace dqi {

// *********************************************************************
// Public Methods
// *********************************************************************

HanConfig::
HanConfig()
  : m_config(0)
  , m_dqRoot()
  , m_top_level(0)
  , m_metadata(0)
{
}


HanConfig::
~HanConfig()
{
  delete m_config;
  /* delete m_dqRoot; */
  delete m_top_level;
  if (m_metadata) m_metadata->Delete();
  delete m_metadata;
}


void
HanConfig::
AssembleAndSave( std::string infileName, std::string outfileName )
{
  TFile* outfile = TFile::Open( outfileName.c_str(), "RECREATE" );
  DirMap_t directories;
  
  // Collect reference histograms and copy to config file
  MiniConfig refconfig;
  refconfig.AddKeyword("reference");
  refconfig.ReadFile(infileName);
  TMap refsourcedata;
  RefVisitor refvisitor( outfile, directories, &refsourcedata );
  refconfig.SendVisitor( refvisitor );
  
  // Collect threshold definitions
  MiniConfig thrconfig;
  thrconfig.AddKeyword("thresholds");
  thrconfig.AddAttributeKeyword("limits");
  thrconfig.ReadFile(infileName);
  
  // Collect algorithm definitions
  MiniConfig algconfig;
  algconfig.AddKeyword("algorithm");
  algconfig.ReadFile(infileName);

  // Collect region definitions
  MiniConfig regconfig;
  regconfig.AddKeyword("output");
  regconfig.ReadFile(infileName);
  
  // Collect histogram definitions
  MiniConfig histconfig;
  histconfig.AddKeyword("dir");
  histconfig.AddAttributeKeyword("hist");
  histconfig.SetAttribKeywordPropagateDown(false);
  histconfig.ReadFile(infileName);
  
  // Collect and write composite-algorithm definitions
  MiniConfig compalgconfig;
  compalgconfig.AddKeyword("compositealgorithm");
  compalgconfig.ReadFile(infileName);
  CompAlgVisitor compalgvisitor(outfile, compalgconfig);
  compalgconfig.SendVisitor(compalgvisitor);
  
  MiniConfig metadataconfig;
  metadataconfig.AddKeyword("metadata");
  metadataconfig.ReadFile(infileName);
  MetadataVisitor metadatavisitor(outfile, metadataconfig);
  metadataconfig.SendVisitor(metadatavisitor);

  // Assemble into a hierarchical configuration
  outfile->cd();
  HanConfigGroup* root = new HanConfigGroup();
  
  RegionVisitor regvisitor( root, algconfig, thrconfig, refconfig, directories );
  regconfig.SendVisitor( regvisitor );
  
  AssessmentVisitor histvisitor( root, algconfig, thrconfig, refconfig, outfile, 
				 directories, &refsourcedata );  
  histconfig.SendVisitor( histvisitor );
  
  outfile->WriteTObject(&refsourcedata, "refsourcedata");
  outfile->cd();
  root->Write();
  outfile->Write();
  outfile->Close();
  delete root;
}

void
HanConfig::
BuildMonitors( std::string configName, dqm_core::Input& input, HanOutput& output )
{
  bool isInitialized = Initialize( configName );
  if( !isInitialized ) {
    return;
  }
  
  m_dqRoot = BuildMonitorsNewRoot( configName, input, output );
  
  output.setConfig( this );
}

boost::shared_ptr<dqm_core::Region>
HanConfig::
BuildMonitorsNewRoot( std::string configName, dqm_core::Input& input, dqm_core::Output& output )
{
  bool isInitialized = Initialize( configName );
  if( !isInitialized ) {
    return boost::shared_ptr<dqm_core::Region>();
  }
  
  std::string algName( m_top_level->GetAlgName() );
  std::string algLibName( m_top_level->GetAlgLibName() );
  if( algLibName != "" ) {
    try {
      dqm_core::LibraryManager::instance().loadLibrary( algLibName );
    }
    catch ( dqm_core::Exception& ex ) {
      //std::cout << "Can't load library " << algLibName << ". Continuing regardless ..." << std::endl;
    }
  }
  dqm_core::RegionConfig regc( algName, 1.0 );
  boost::shared_ptr<dqm_core::Region> retval(dqm_core::Region::createRootRegion( "top_level", input, output, regc ));
  
  ConfigVisitor confvisitor( m_config, &output );
  m_top_level->Accept( confvisitor, retval );
  return retval;
}

void
HanConfig::
BuildConfigOutput( std::string configName, TFile* inputFile, std::string path,
                   HanOutput::DQOutputMap_t* outputMap, TSeqCollection* outputList )
{
  bool isInitialized = Initialize( configName );
  if( !isInitialized ) {
    return;
  }
  
  if( inputFile == 0 ) {
    return;
  }
  
  TDirectory* basedir(0);
  if( path != "" ) {
    std::string pathForSearch = path;
    pathForSearch += "/dummyName";
    basedir = ChangeInputDir( inputFile, pathForSearch );
  }
  
  if( basedir == 0 )
    basedir = inputFile;
  
  TIter mdIter(m_metadata);
  TSeqCollection* mdList = newTList("HanMetadata_");
  HanConfigMetadata* md(0);
  while ((md = dynamic_cast<HanConfigMetadata*>(mdIter()))) {
    mdList->Add( md->GetList(basedir,*outputMap) );
  }
  TSeqCollection* top_level_list = m_top_level->GetList(basedir,*outputMap);
  top_level_list->Add( mdList );
  outputList->Add( top_level_list );  
}


TObject*
HanConfig::
GetReference( std::string& groupName, std::string& name )
{
  /*  if( m_top_level == 0 ) {
    return 0;
  }
  
  HanConfigGroup* parent = m_top_level->GetNode(groupName);
  if( parent == 0 ) {
    parent = m_top_level;
    } */
  
  std::auto_ptr<const HanConfigAssessor> a(GetAssessor( groupName, name ));
  if (!a.get()) return 0;
  std::string refName( a->GetAlgRefName() );
  if( refName != "" ) {
    TKey* key = getObjKey( m_config, refName );
    if( key != 0 ) {
      //TObject* ref = m_config->Get(refName.c_str());
      TObject* ref = key->ReadObj();
      if (ref)
	return ref;
    }
  }
  
  return 0;
}


std::string 
SplitReference(std::string refPath, std::string refName )
{
  //Split comma sepated inputs into individual file names
  std::string delimiter = ",";
  std::vector<std::string> refFileList;
  size_t pos = 0;
  std::string token;
  while ((pos = refPath.find(delimiter)) != std::string::npos) {
    token = refPath.substr(0, pos);
    refFileList.push_back(token);
    refPath.erase(0, pos + delimiter.length());
  }
  refFileList.push_back(refPath);

  //Try to open each file in the list
  for(std::size_t i=0; i<refFileList.size(); i++){
    std::string fileName=refFileList.at(i)+refName;
    size_t first = fileName.find_first_not_of(" ");
    fileName.erase(0, first);
    if (gROOT->GetListOfFiles()->FindObject(fileName.c_str()) ) {
      return fileName;
    } 
    else {
      if(TFile::Open(fileName.c_str())){
	return fileName;
      }
      else{
	std::cerr << "Unable to open " << fileName << ", trying next reference file" << std::endl;
      }
    }
  }
  std::cerr << "Unable to open any reference file, reference will not be included" << std::endl;
  return "";
}

const HanConfigAssessor*
HanConfig::
GetAssessor( std::string& groupName, std::string& name ) const
{
  if( m_top_level == 0 ) {
    return 0;
  }
  
  HanConfigGroup* parent = m_top_level->GetNode(groupName);
  if( parent == 0 ) {
    parent = m_top_level;
  }
  
  const HanConfigAssessor& a = parent->GetAssessor(name);

  return new HanConfigAssessor(a);
} 

void
HanConfig::
GetRegexList(std::set<std::string>& regexlist) 
{
  RegexVisitor rv(regexlist);
  m_top_level->Accept(rv, boost::shared_ptr<dqm_core::Region>());
}

// *********************************************************************
// Protected Methods
// *********************************************************************

HanConfig::RefVisitor::
RefVisitor( TFile* outfile_, HanConfig::DirMap_t& directories_, TMap* refsourcedata_ )
  : m_outfile(outfile_)
  , m_directories(directories_)
  , m_refsourcedata(refsourcedata_)
{
}


void
HanConfig::RefVisitor::
Visit( const MiniConfigTreeNode* node ) const
{
  TObject* obj;
  std::string name = node->GetAttribute("name");
  std::string fileName = node->GetAttribute("file");
  if(node->GetAttribute("location")!=""){
    fileName = SplitReference(node->GetAttribute("location"), fileName);
  }
  std::string refInfo = node->GetAttribute("info");
  if( fileName != "" && name != "" && name != "same_name" ) {
    std::auto_ptr<TFile> infile( TFile::Open(fileName.c_str()) );
    TKey* key = getObjKey( infile.get(), name );
    if( key == 0 ) {
      std::cerr << "HanConfig::RefVisitor::Visit(): Reference not found: \"" << name << "\"\n";
      return;
    }
//     TDirectory* dir = ChangeOutputDir( m_outfile, name, m_directories );
//     dir->cd();
    //sami
    m_outfile->cd();
    obj = key->ReadObj();
    //    obj->Write();
    std::string newHistoName=dqi::ConditionsSingleton::getInstance().getNewRefHistoName();
    //node->SetAttribute("newname",newHistoName,false);//add new name to tree
    //    std::cout<<"Writing \""<<name<<"\" in \""<<fileName<<"\" with new name \""<<newHistoName<<"\""<<std::endl;
    dqi::ConditionsSingleton::getInstance().setNewReferenceName(name,newHistoName);
    obj->Write(newHistoName.c_str());
    delete obj;
    TObjString* fnameostr = new TObjString(fileName.c_str());
    m_refsourcedata->Add(new TObjString(newHistoName.c_str()),
			 fnameostr);
    if (! m_refsourcedata->FindObject(fileName.c_str())) {
      m_refsourcedata->Add(fnameostr, refInfo != "" ? new TObjString(refInfo.c_str())
			   : new TObjString("Reference"));
    }  
  }
}


HanConfig::AssessmentVisitorBase::
AssessmentVisitorBase( HanConfigGroup* root_, const MiniConfig& algConfig_,
                       const MiniConfig& thrConfig_, const MiniConfig& refConfig_,
                       TFile* outfile_, HanConfig::DirMap_t& directories_,
		       TMap* refsourcedata_ )
  : m_root(root_)
  , m_algConfig(algConfig_)
  , m_thrConfig(thrConfig_)
  , m_refConfig(refConfig_)
  , m_outfile(outfile_)
  , m_directories(directories_)
  , m_refsourcedata(refsourcedata_)
{
}

std::shared_ptr<TFile>
HanConfig::AssessmentVisitorBase::
GetROOTFile( std::string& fname ) const
{
  auto it = m_filecache.find(fname);
  if (it != end(m_filecache)) {
    return it->second;
  } else {
    std::shared_ptr<TFile> thisptr(TFile::Open(fname.c_str()));
    if (thisptr.get()) {
      return ( m_filecache[fname] = thisptr );
     } else {
      return thisptr;
    }	  
  }
}

float AttribToFloat(const MiniConfigTreeNode* node, const std::string attrib,
		    const std::string warningString, bool local=false)
{
  std::istringstream valstream;
  if (local) {
    valstream.str(node->GetAttributeLocal(attrib));
  } else {
    valstream.str(node->GetAttribute(attrib));
  }
  float val;
  valstream >> val;
  if (! valstream) {
    std::cerr << warningString << std::endl;
    return 0;
  } else {
    return val;
  }
}

void
HanConfig::AssessmentVisitorBase::
GetAlgorithmConfiguration( HanConfigAssessor* dqpar, const std::string& algID, 
                           std::string assessorName ) const
{
  // bool hasName(false);
  std::set<std::string> algAtt;
  m_algConfig.GetAttributeNames( algID, algAtt );
  std::set<std::string>::const_iterator algAttEnd = algAtt.end();
  for( std::set<std::string>::const_iterator i = algAtt.begin(); i != algAttEnd; ++i ) {
    std::string trail("");
    std::string::size_type pos = (*i).find("|");
    if (pos != std::string::npos) {
      trail = (*i).substr(pos + 1, std::string::npos);
    }
    if( *i == "name" ) {
      // hasName = true;
      std::string algName( m_algConfig.GetStringAttribute(algID,"name") );
      dqpar->SetAlgName( algName );
    }
    else if( *i == "libname" ) {
      std::string algLibName( m_algConfig.GetStringAttribute(algID,"libname") );
      dqpar->SetAlgLibName( algLibName );
    }
    else if( *i == "thresholds" || trail == "thresholds" ) {
      std::string thrID( m_algConfig.GetStringAttribute(algID,*i) );
      std::set<std::string> thrAtt;
      m_thrConfig.GetAttributeNames( thrID, thrAtt );
      std::set<std::string>::const_iterator thrAttEnd = thrAtt.end();
      for( std::set<std::string>::const_iterator t = thrAtt.begin(); t != thrAttEnd; ++t ) {
        std::string thrAttName = *t;
        std::string thrAttVal = m_thrConfig.GetStringAttribute( thrID, thrAttName );
        std::string limName = thrAttVal + std::string("/") + thrAttName;
        HanConfigAlgLimit algLim;
	if (pos != std::string::npos) {
	  algLim.SetName( (*i).substr(0, pos) + std::string("|") + *t );
	} else {
	  algLim.SetName( *t );
	}
        algLim.SetGreen( m_thrConfig.GetFloatAttribute(limName,"warning") );
        algLim.SetRed( m_thrConfig.GetFloatAttribute(limName,"error") );
        dqpar->AddAlgLimit( algLim );
      }
    }
    else if( *i == "reference" ) {
      std::string tmpRefID=m_algConfig.GetStringAttribute(algID,"reference");
      //std::cout<<"Got tmpRefID=\""<<tmpRefID<<"\""<<std::endl;
      dqi::ConditionsSingleton &CS=dqi::ConditionsSingleton::getInstance();
      std::string refCond=CS.getCondition();
      //parses
      std::vector<std::pair<std::string,std::string> > condPairs=CS.getConditionReferencePairs(tmpRefID);
      std::stringstream newRefString;
      // for each condition ...
      for(size_t t=0;t<condPairs.size();t++){
	std::string refID=condPairs.at(t).second;
	std::string cond=condPairs.at(t).first;	
	std::vector<std::string> refIDVec;
	TObjArray *toarray(0);
	bool isMultiRef(false);
	if (refID[0] == '[') {
	  std::string cleanedRefID = refID;
	  boost::algorithm::trim_if(cleanedRefID, boost::is_any_of("[] "));
	  isMultiRef = true;
	  boost::split(refIDVec, cleanedRefID, boost::is_any_of(","));
	  toarray = new TObjArray();
	  toarray->SetOwner(kTRUE);
	} else {
	  refIDVec.push_back(refID);
	}
	std::string newRefId;
	std::string absAlgRefName("");	
	for (const auto thisRefID : refIDVec) {
	  std::string algRefName( m_refConfig.GetStringAttribute(thisRefID,"name") );
	  std::string algRefPath( m_refConfig.GetStringAttribute(thisRefID,"path") );
	  std::string algRefInfo( m_refConfig.GetStringAttribute(thisRefID,"info") );
	  absAlgRefName = "";
	  if( algRefPath != "" ) {
	    absAlgRefName += algRefPath;
	    absAlgRefName += "/";
	  }
	  if( algRefName == "same_name" ) {//sameName alg
	    algRefName = assessorName;
	    absAlgRefName += algRefName;
	    std::string algRefFile( m_refConfig.GetStringAttribute(thisRefID,"file") );
	    if(m_refConfig.GetStringAttribute(thisRefID,"location")!=""){
              algRefFile = SplitReference( m_refConfig.GetStringAttribute(thisRefID,"location"), algRefFile);
            }
	    if( algRefFile != "" ) {
	      std::shared_ptr<TFile> infile = GetROOTFile(algRefFile);
	      if ( ! infile.get() ) {
		std::cerr << "HanConfig::AssessmentVistorBase::GetAlgorithmConfiguration: Reference file " << algRefFile << " not found" << std::endl;
		continue;
	      }
	      TKey* key = getObjKey( infile.get(), absAlgRefName );
	      if( key == 0 ) {
		// Quiet this error ...
		//std::cerr << "HanConfig::AssessmentVisitorBase::GetAlgorithmConfiguration: "
		//<< "Reference not found: \"" << absAlgRefName << "\"\n";
		continue;
	      }
	      // TDirectory* dir = ChangeOutputDir( m_outfile, absAlgRefName, m_directories );
	      //dir->cd();
	      m_outfile->cd(); //we are writing to the / folder of file
	      TObject* obj;
	      obj = key->ReadObj();
	      if (isMultiRef) { 
		TObject* cobj = obj->Clone();
		TH1* hobj = dynamic_cast<TH1*>(cobj);
		if (hobj) {
		  hobj->SetName(thisRefID.c_str());
		}
		toarray->Add(cobj); 
		delete obj;
	      } else {
		std::string algRefUniqueName=algRefFile+":/"+absAlgRefName;
		newRefId=CS.getNewReferenceName(algRefUniqueName,true);
		if(newRefId.empty()){
		  newRefId=CS.getNewRefHistoName();
		  CS.setNewReferenceName(algRefUniqueName,newRefId);
		  auto algRefFileostr = new TObjString(algRefFile.c_str());
		  m_refsourcedata->Add(new TObjString(newRefId.c_str()),
				       algRefFileostr);
		  if (algRefInfo != "") {
		    if (! m_refsourcedata->FindObject(algRefFile.c_str())) {
		      m_refsourcedata->Add(algRefFileostr, 
					   new TObjString(algRefInfo.c_str()));
		    }  
		  }
		}
	      //std::cout<<"Writing algref with algrefname= "<<algRefUniqueName<<", newRefId="<<newRefId<<std::endl;
		obj->Write(newRefId.c_str());//write object with a new reference name
		delete obj;
	      }
	    }
	  } else {
	    // not "same_name" - assign properly
	    newRefId=CS.getNewReferenceName(algRefName,true);
	    if(newRefId.empty()){
	      std::cerr<<"Warning New reference id is empty for refId=\""
		       <<refID<<"\", cond=\""<<cond<<"\", assessorName=\""
		       <<assessorName<<"\", algRefName=\""
		       <<algRefName<<"\""<<std::endl;
	      std::cerr << "AlgRefPath=" << algRefPath << " AlgRefInfo=" << algRefInfo << std::endl;
	    }
	  }
	} 
	    
	if (isMultiRef) {
	  std::string algRefUniqueName=cond+"_multiple:/"+absAlgRefName;
	  newRefId=CS.getNewReferenceName(algRefUniqueName,true);
	  if(newRefId.empty()){
	    newRefId=CS.getNewRefHistoName();
	    CS.setNewReferenceName(algRefUniqueName,newRefId);
	    m_refsourcedata->Add(new TObjString(newRefId.c_str()),
				 new TObjString("Multiple references"));
	  }
	  m_outfile->cd();
	  toarray->Write(newRefId.c_str(), 1);//write object with a new reference name
	  delete toarray;
	}

	/*      if(newRefId.empty()){
	  newRefId=CS.getNewReferenceName(algRefName,true);
	  if(newRefId.empty()){
	    std::cerr<<"Warning New reference id is empty for refId=\""
		     <<refID<<"\", cond=\""<<cond<<"\", assessorName= \""
		     <<assessorName<<"\""<<std::endl;
		     } 
	} */
	
	if(!cond.empty()){
	  newRefString<<cond<<":"<<newRefId<<";";
	}else{
	  newRefString<<newRefId;
	}
      }
      dqpar->SetAlgRefName((newRefString.str()));
			       
//       std::string algRefName( m_refConfig.GetStringAttribute(refID,"name") );
//       std::string algRefPath( m_refConfig.GetStringAttribute(refID,"path") );
//       std::string absAlgRefName("");
//       if( algRefPath != "" ) {
//         absAlgRefName += algRefPath;
//         absAlgRefName += "/";
//       }
//       if( algRefName != "same_name" ) {
//         absAlgRefName += algRefName;
//         dqpar->SetAlgRefName( absAlgRefName );
//       }
//       else {
//         algRefName = assessorName;
//         absAlgRefName += algRefName;
//         std::string algRefFile( m_refConfig.GetStringAttribute(refID,"file") );
//         if( algRefFile != "" ) {
//           std::auto_ptr<TFile> infile( TFile::Open(algRefFile.c_str()) );
//           TKey* key = getObjKey( infile.get(), absAlgRefName );
//           if( key == 0 ) {
//             std::cerr << "HanConfig::AssessmentVisitorBase::GetAlgorithmConfiguration: "
//                       << "Key not found: \"" << absAlgRefName << "\"\n";
//             continue;
//           }
//           TDirectory* dir = ChangeOutputDir( m_outfile, absAlgRefName, m_directories );
//           dir->cd();
//           TObject* obj;
//           obj = key->ReadObj();
// 	  obj->Write();
// 	  delete obj;
//           dqpar->SetAlgRefName( absAlgRefName );
//         }
    }else {
      HanConfigAlgPar algPar;
      algPar.SetName( *i );
      algPar.SetValue( m_algConfig.GetFloatAttribute(algID,*i) );
      dqpar->AddAlgPar( algPar );
    }
  }
}


HanConfig::RegionVisitor::
RegionVisitor( HanConfigGroup* root_, const MiniConfig& algConfig_,
               const MiniConfig& thrConfig_, const MiniConfig& refConfig_,
               HanConfig::DirMap_t& directories_ )
  : AssessmentVisitorBase( root_, algConfig_, thrConfig_, refConfig_, 0, directories_, nullptr )
{
}


void
HanConfig::RegionVisitor::
Visit( const MiniConfigTreeNode* node ) const
{
  const MiniConfigTreeNode* parent = node->GetParent();
  if( parent == 0 )
    return;
  
  const MiniConfigTreeNode* grandparent = parent->GetParent();
  std::auto_ptr<HanConfigGroup> alloc( new HanConfigGroup() );
  HanConfigGroup* reg = (grandparent==0) ? m_root : alloc.get();
  
  std::string regName( node->GetName() );
  reg->SetName( regName );
  reg->SetPathName( node->GetPathName() );
  
  std::string algID( node->GetAttribute("algorithm") );
  if( algID != "" ) {
    GetAlgorithmConfiguration( reg, algID );
  } else {
    std::cerr << "Warning: no summary algorithm specified for " << node->GetPathName() << std::endl
	      << "This is probably not what you want" << std::endl;
  }

  if (node->GetAttributeLocal("weight") != "") {
    std::ostringstream err;
    err << "Weight is not a floating point attribute for " << node->GetPathName() << "; setting to 0";
    reg->SetWeight(AttribToFloat(node, "weight", err.str(), true));
  }

  if( grandparent != 0 ) {
    std::string path( parent->GetPathName() );
    HanConfigGroup* parentReg = m_root->GetNode( path );
    parentReg = (parentReg==0) ? m_root : parentReg;
    if( parentReg != 0 ) {
      parentReg->AddGroup( *reg );
    }
  }
}

HanConfig::RegexVisitor::
RegexVisitor( std::set<std::string>& regexes_ )
  : m_regexes(regexes_)
{
}

boost::shared_ptr<dqm_core::Node> 
HanConfig::RegexVisitor::
Visit( const HanConfigAssessor* node, boost::shared_ptr<dqm_core::Region> ) const
{
  // ignore Groups
  if (dynamic_cast<const HanConfigGroup*>(node) != NULL) {
    return boost::shared_ptr<dqm_core::Parameter>();
  }
  if (node->GetIsRegex()) {
    m_regexes.insert(node->GetName());
  }
  return boost::shared_ptr<dqm_core::Parameter>();
}

HanConfig::AssessmentVisitor::
AssessmentVisitor( HanConfigGroup* root_, const MiniConfig& algConfig_,
                   const MiniConfig& thrConfig_, const MiniConfig& refConfig_,
                   TFile* outfile_, HanConfig::DirMap_t& directories_,
		   TMap* refsourcedata_ )
  : AssessmentVisitorBase( root_, algConfig_, thrConfig_, refConfig_, outfile_, directories_, refsourcedata_ )
{
}

void
HanConfig::AssessmentVisitor::
Visit( const MiniConfigTreeNode* node ) const
{
  std::set<std::string> histAtt;
  node->GetAttributeNames( histAtt );
  std::set<std::string>  defined;
  std::string regID("");
  std::string algID("");
  std::set<std::string>::const_iterator histAttEnd = histAtt.end();
  for( std::set<std::string>::const_iterator h = histAtt.begin(); h != histAttEnd; ++h ) {
    const MiniConfigTreeNode* histNode = node->GetDaughter( *h );
    if( histNode == 0 )
      continue;
    
    algID = histNode->GetAttribute("algorithm");
    regID = histNode->GetAttribute("output");
    
    if( algID == "" ) {
      std::cerr << "No \"algorithm\" defined for " << histNode->GetPathName() << "\n";
      continue;
    }
    
    if( regID == "" )
      regID = "top_level";
    
    std::string strNodeName(histNode->GetName());
    std::string strHistName, strFullHistName;
    std::string::size_type atsign = strNodeName.find("@");
    if (atsign == std::string::npos) {
      strHistName = strNodeName;
      strFullHistName = histNode->GetPathName();
    } else {
      strHistName = strNodeName.substr(0, atsign);
      strFullHistName = histNode->GetPathName();
      strFullHistName = strFullHistName.substr(0, strFullHistName.find("@"));
    }

    if( strHistName == "all_in_dir" )
      continue;
    
    if( defined.find(histNode->GetPathName()) != defined.end() )
      continue;
    
    HanConfigAssessor dqpar;
    dqpar.SetName( histNode->GetPathName() );

    std::string regexflag(histNode->GetAttribute("regex"));
    boost::algorithm::to_lower(regexflag);
    if (regexflag == "1" || regexflag == "true" || regexflag == "yes") {
    	dqpar.SetIsRegex(true);
    }
    
    if (histNode->GetAttribute("weight") != "") {
      std::ostringstream err;
      err << "Weight attribute not a floating point type for " << histNode->GetPathName() << "; setting to zero";
      dqpar.SetWeight(AttribToFloat(histNode, "weight",
				    err.str()));
    }
        
    GetAlgorithmConfiguration( &dqpar, algID, strFullHistName );
   
    std::set<std::string> histAtt2;
    histNode->GetAttributeNames( histAtt2 );
    std::set<std::string>::const_iterator histAttEnd2 = histAtt2.end();
    for( std::set<std::string>::const_iterator h2 = histAtt2.begin(); h2 != histAttEnd2; ++h2 ) {
      if (node->GetDaughter( *h2 )
	  || *h2 == "algorithm" || *h2 == "output" || *h2 == "reference"
	  || *h2 == "weight" || *h2 == "regex") {
	continue;
      }
      HanConfigParMap parMap;
      parMap.SetName(*h2); parMap.SetValue(histNode->GetAttribute(*h2));
      dqpar.AddAnnotation(parMap);
    }

    // Add the histogram name
    HanConfigParMap parMap;
    parMap.SetName("inputname"); parMap.SetValue( strFullHistName );
    dqpar.AddAnnotation(parMap);
       
    HanConfigGroup* dqreg = m_root->GetNode( regID );
    dqreg = (dqreg==0) ? m_root : dqreg;
    if( dqreg != 0 ) {
      dqreg->AddAssessor( dqpar );
    }
    defined.insert( strFullHistName );
  }
  
  for( std::set<std::string>::const_iterator h = histAtt.begin(); h != histAttEnd; ++h ) {
    const MiniConfigTreeNode* histNode = node->GetDaughter( *h );
    if( histNode == 0 )
      continue;
    
    algID = histNode->GetAttribute("algorithm");
    regID = histNode->GetAttribute("output");

    if( algID == "" ) {
      std::cerr << "No \"algorithm\" defined for " << histNode->GetPathName() << "\n";
      continue;
    }
    
    if( regID == "" )
      regID = "top_level";

    std::string strNodeName(histNode->GetName());
    std::string strHistName, strFullHistName, extension;
    std::string::size_type atsign = strNodeName.find("@");
    if (atsign == std::string::npos) {
      strHistName = strNodeName;
      strFullHistName = histNode->GetPathName();
      extension = "";
    } else {
      strHistName = strNodeName.substr(0, atsign);
      extension = strNodeName.substr(atsign, std::string::npos);
      strFullHistName = histNode->GetPathName();
      strFullHistName = strFullHistName.substr(0, strFullHistName.find("@"));
    }

    if( strHistName == "all_in_dir" ) {
	
      std::string regexflag(histNode->GetAttribute("regex"));
      if (histNode->GetAttribute("regex") != "") {
      	std::cerr << "all_in_dir and regex are incompatible; ignoring regex flag for " << histNode->GetPathName()
      						<< "/all_in_dir" << std::endl;
      }

      std::string refID( histNode->GetAttribute("reference") );
      if( refID == "" ) {
        std::cerr << "No \"reference\" defined for " << histNode->GetPathName() << "\n";
        continue;
      }
      std::string refFile( m_refConfig.GetStringAttribute(refID,"file") );
      if( refFile == "" ) {
        std::cerr << "No \"file\" defined for " << histNode->GetPathName() << "\n";
        continue;
      }
      
      std::string refPath( m_refConfig.GetStringAttribute(refID,"path") );
      
      std::string objPath("");
      std::string absObjPath("");
      
      //std::auto_ptr<TFile> infile( TFile::Open(refFile.c_str()) );
      std::shared_ptr<TFile> infile( GetROOTFile(refFile) );
      TDirectory* basedir(0);
      TDirectory* dir(0);
      TKey* key;
      
      if( refPath != "" ) {
        //std::cout << "Searching under path \"" << refPath << "\" for " << "\"" << histNode->GetPathName() << "\"\n";
        absObjPath += refPath;
        absObjPath += "/";
        std::string refPathForSearch = refPath;
        refPathForSearch += "/dummyName";
        basedir = ChangeInputDir( infile.get(), refPathForSearch );
        if( basedir == 0 ) {
          std::cerr << "Cannot find \"" << refPath << "\" in file\n";
          continue;
        }
      }
      
      if( basedir == 0 )
        basedir = infile.get();
      
      std::string allPathName( histNode->GetPathName() );
      std::string::size_type i = allPathName.find_last_of('/');
      if( i != std::string::npos ) {
        objPath = std::string( allPathName, 0, i );
        objPath += "/";
        absObjPath += std::string( allPathName, 0, i );
        absObjPath += "/";
      }
      
      dir = ChangeInputDir( basedir, histNode->GetPathName() );
      if( dir == 0 ) {
        std::cerr << "Cannot find \"" << absObjPath << "\" in file\n";
        continue;
      }
      
      TIter next( dir->GetListOfKeys() );
      std::string objName;
      std::string absObjName;
      // we might have multiple loops through 'all_in_dir', but each should
      // have unique objects
      std::set<std::string>  localdefined;
      while( (key = dynamic_cast<TKey*>(next())) != 0 ) {
        TObject* tmpobj = key->ReadObj();
        TH1* tmph = dynamic_cast<TH1*>(tmpobj);
        TGraph* tmpg = dynamic_cast<TGraph*>(tmpobj);
        if( tmph == 0 && tmpg == 0 )
          continue;
        
        objName = objPath;
        objName += std::string( tmpobj->GetName() );
        absObjName = absObjPath;
        absObjName += std::string( tmpobj->GetName() );
        delete tmpobj;
        
        if( defined.find(objName) != defined.end() ||
	    localdefined.find(objName) != localdefined.end() )
          continue;
        
        TKey* test = getObjKey( infile.get(), absObjName );
        if( test == 0 )
          continue;
        
        HanConfigAssessor dqpar;
        dqpar.SetName( objName + extension );
        GetAlgorithmConfiguration( &dqpar, algID, objName );

	if (histNode->GetAttribute("weight") != "") {
	  std::ostringstream err;
	  err << "Weight attribute not a floating point type for " << objName << "; setting to zero";
	  dqpar.SetWeight(AttribToFloat(histNode, "weight",
					err.str()));
	}

	std::set<std::string> histAtt2;
	histNode->GetAttributeNames( histAtt2 );
	std::set<std::string>::const_iterator histAttEnd2 = histAtt2.end();
	for( std::set<std::string>::const_iterator h2 = histAtt2.begin(); h2 != histAttEnd2; ++h2 ) {
	  if (node->GetDaughter( *h2 )
	      || *h2 == "algorithm" || *h2 == "output" || *h2 == "reference"
	      || *h2 == "weight" || *h2 == "regex") {
	    continue;
	  }
	  HanConfigParMap parMap;
	  parMap.SetName(*h2); parMap.SetValue(histNode->GetAttribute(*h2));
	  dqpar.AddAnnotation(parMap);
	}

	// Add the histogram name
	HanConfigParMap parMap;
	parMap.SetName("inputname"); parMap.SetValue( objName );
	dqpar.AddAnnotation(parMap);

        HanConfigGroup* dqreg = m_root->GetNode( regID );
        dqreg = (dqreg==0) ? m_root : dqreg;
        if( dqreg != 0 ) {
          dqreg->AddAssessor( dqpar );
        }
        localdefined.insert( objName );
      }
      
      continue;
    }
  }
}


HanConfig::ConfigVisitor::
ConfigVisitor( TFile* file_, dqm_core::Output* output_ )
  : m_file(file_),
    m_output(output_)
{
}


boost::shared_ptr<dqm_core::Node>
HanConfig::ConfigVisitor::
Visit( const HanConfigAssessor* node, boost::shared_ptr<dqm_core::Region> dqParent ) const
{
  const HanConfigGroup* gnode = dynamic_cast<const HanConfigGroup*>( node );
  std::string algName( node->GetAlgName() );
  std::string algLibName( node->GetAlgLibName() );
  if( algLibName != "" ) {
    try {
      dqm_core::LibraryManager::instance().loadLibrary( algLibName );
    }
    catch ( dqm_core::Exception& ex ) {
      //std::cout << "Can't load library " << algLibName << ". Continuing regardless ..." << std::endl;
    }
  }
  
  if( gnode != 0 ) {
    dqm_core::RegionConfig regc( algName, node->GetWeight() );
    std::string regName( gnode->GetPathName() );
    boost::shared_ptr<dqm_core::Region> reg(dqParent->addRegion( regName, regc ));
    m_output->addListener(regName, dqParent.get());
    return reg;
  }
  
  std::string inputData( node->GetHistPath() );
  HanAlgorithmConfig* algConfig = new HanAlgorithmConfig( *node, m_file );
  dqm_core::ParameterConfig parc( inputData, algName, node->GetWeight(), 
				  std::shared_ptr<HanAlgorithmConfig>(algConfig), node->GetIsRegex() );
  boost::shared_ptr<dqm_core::Node> par(dqParent->addParameter( node->GetName(), parc ));
  m_output->addListener(node->GetName(), dqParent.get());
  return par;
}


HanConfig::CompAlgVisitor::
CompAlgVisitor(TFile* outfile_, const MiniConfig& compAlgConfig_)
  : m_outfile(outfile_)
    , m_compAlgConfig(compAlgConfig_)
{
}


void
HanConfig::CompAlgVisitor::
Visit( const MiniConfigTreeNode* node ) const
{
  m_outfile->cd();
  std::map<std::string,MiniConfigTreeNode*> daughters = node->GetDaughters();
  std::map<std::string,MiniConfigTreeNode*>::const_iterator nodeiterEnd = daughters.end();  
  for( std::map<std::string,MiniConfigTreeNode*>::const_iterator iter = daughters.begin(); iter != nodeiterEnd; ++iter ) {
    std::string compAlgID = iter->second->GetName();
    std::string compAlgKey = iter->first;
    
    HanConfigCompAlg* compAlg = new HanConfigCompAlg();
    std::set<std::string> compAlgAtt;
    m_compAlgConfig.GetAttributeNames(compAlgID, compAlgAtt);
    std::set<std::string>::const_iterator compAlgAttEnd = compAlgAtt.end();
    //std::cout << "Configuring composite algorithm: " << compAlgID << std::endl;
    compAlg->SetName(compAlgID);
    for( std::set<std::string>::const_iterator i = compAlgAtt.begin(); i != compAlgAttEnd; ++i ) {
      if( *i == "subalgs" ) {
        std::string subAlgs( m_compAlgConfig.GetStringAttribute(compAlgID,"subalgs") );
        std::string::size_type pos = subAlgs.find(',');
        for(int size=subAlgs.size(), sizeOld=-8;
    	    size != sizeOld; 
    	    subAlgs.erase(0, pos+1), pos = subAlgs.find(","),sizeOld=size, size=subAlgs.size()) {
            //std::cout << "  --> adding component algorithm: " <<  subAlgs.substr(0,pos) << std::endl;
            compAlg->AddAlg( subAlgs.substr(0,pos));
          }
      }
      else if( *i == "libnames" ) {
        std::string libs( m_compAlgConfig.GetStringAttribute(compAlgID,"libnames") );
        std::string::size_type pos = libs.find(',');
        for(int size=libs.size(), sizeOld=-8;
    	    size != sizeOld; 
    	    libs.erase(0, pos+1), pos = libs.find(","),sizeOld=size, size=libs.size()) {
            //std::cout << "  --> using library: " <<  libs.substr(0,pos) << std::endl;
            compAlg->AddLib( libs.substr(0,pos));
          }
      }
    }
    compAlg->Write();
  }
}
  
HanConfig::MetadataVisitor::
MetadataVisitor(TFile* outfile_, const MiniConfig& metadataConfig_)
  : m_outfile(outfile_)
    , m_metadataConfig(metadataConfig_)
{
}


void
HanConfig::MetadataVisitor::
Visit( const MiniConfigTreeNode* node ) const
{
  // maybe already existing?
  if (m_outfile->Get("HanMetadata")) {
    m_outfile->cd("HanMetadata");
  } else {
    // try to make it
    TDirectory* mdir = m_outfile->mkdir("HanMetadata");
    if (mdir) {
      mdir->cd();
    } else {
      // all fail
      m_outfile->cd();
    }
  }
    
  std::map<std::string,MiniConfigTreeNode*> daughters = node->GetDaughters();
  std::map<std::string,MiniConfigTreeNode*>::const_iterator nodeiterEnd = daughters.end();  
  for( std::map<std::string,MiniConfigTreeNode*>::const_iterator iter = daughters.begin(); iter != nodeiterEnd; ++iter ) {
    std::string metadataID = iter->second->GetName();

    std::set<std::string> metadataAtt;
    m_metadataConfig.GetAttributeNames(metadataID, metadataAtt);
    HanConfigMetadata* metadata = new HanConfigMetadata();
    metadata->SetName(metadataID);
    for (std::set<std::string>::const_iterator i = metadataAtt.begin(); i != metadataAtt.end(); ++i ) {
      HanConfigParMap parMap;
      parMap.SetName(*i); parMap.SetValue(m_metadataConfig.GetStringAttribute(metadataID, *i));
      metadata->AddKeyVal(parMap);
    }
    metadata->Write();
    delete metadata;
  }
}
   

bool
HanConfig::
Initialize( std::string configName )
{
  if( m_config == 0 || m_top_level == 0 ) {
    
    delete m_config;
    delete m_top_level;
    if (m_metadata) {
      m_metadata->Delete();
    }
    delete m_metadata; m_metadata=newTList("HanMetadata");

    m_config = TFile::Open( configName.c_str(), "READ" );
    if( m_config == 0 ) {
      std::cerr << "HanConfig::Initialize() cannot open file \"" << configName << "\"\n";
      return false;
    }

    TMap* refsourcedata = dynamic_cast<TMap*>(m_config->Get("refsourcedata"));
    if (refsourcedata) {
      ConditionsSingleton::getInstance().setRefSourceMapping(refsourcedata);
    } else {
      std::cerr << "Can't retrieve reference source info" << std::endl;
    }
    
    TIter nextKey( m_config->GetListOfKeys() );
    TKey* compAlgKey(0);
    while( (compAlgKey = dynamic_cast<TKey*>( nextKey() )) != 0 ) {
      TObject* obj = compAlgKey->ReadObj();
      HanConfigCompAlg* compAlg = dynamic_cast<HanConfigCompAlg*>( obj );
      if( compAlg != 0 ) {
        //std::cout << "Instantiating composite algorithm \"" << compAlg->GetName() << "\"...\n";
        new CompositeAlgorithm( *compAlg );
      }
      delete obj;
    }
    
    TKey* key(0);

    m_metadata = newTList("HanMetadata");
    key = m_config->FindKey("HanMetadata");
    if ( key ) {
      TDirectory* mdDir = dynamic_cast<TDirectory*>(key->ReadObj());
      if (mdDir) {
	TIter mdIter(mdDir->GetListOfKeys());
	TKey* mdKey(0);
	while ((mdKey = dynamic_cast<TKey*>(mdIter()))) {
	  HanConfigMetadata* md = dynamic_cast<HanConfigMetadata*>(mdKey->ReadObj());
	  if (md) {
	    m_metadata->Add(md);
	  }
	}
      }
      delete mdDir;
    }

    key = m_config->FindKey("top_level");
    if( key == 0 ) {
      std::cerr << "HanConfig::Initialize() cannot find configuration in file \"" << configName << "\"\n";
      return false;
    }

    m_top_level = dynamic_cast<HanConfigGroup*>( key->ReadObj() );
    if( m_top_level == 0 ) {
      std::cerr << "HanConfig::Initialize() cannot find configuration in file \"" << configName << "\"\n";
      return false;
    }
  }
  
  return true;
}


// *********************************************************************
// Private Methods
// *********************************************************************


TDirectory*
HanConfig::
ChangeInputDir( TDirectory* dir, std::string path )
{
  if( dir == 0 )
    return 0;

  TKey* key(0);

  std::string::size_type i = path.find_first_of('/');
  if( i != std::string::npos ) {
    std::string dName( path, 0, i );
    std::string pName( path, i+1, std::string::npos );
    if( dName != "" ) {
      TDirectory* subDir(0);
      key = dir->FindKey( dName.c_str() );
      if( key != 0 ) {
        subDir = dynamic_cast<TDirectory*>( key->ReadObj() );
      }
      else {
        return 0;
      }
      TDirectory* retval = ChangeInputDir( subDir, pName );
      return retval;
    }
    return ChangeInputDir( dir, pName );
  }
  
  return dir;
}


TDirectory*
HanConfig::
ChangeOutputDir( TFile* file, std::string path, DirMap_t& directories )
{
	if( file == 0 )
		return 0;
	
  std::string::size_type i = path.find_last_of('/');
  if( i != std::string::npos ) {
    std::string subPath( path, 0, i );
    DirMap_t::const_iterator j = directories.find( subPath );
    if( j != directories.end() ) {
      TDirectory* dir = j->second;
      return dir;
    }
    else {
      TDirectory* parDir = ChangeOutputDir( file, subPath, directories );
      std::string dirName;
      std::string::size_type k = subPath.find_last_of('/');
      dirName = (k != std::string::npos) ? std::string( subPath, k+1, std::string::npos ) : subPath;
      TDirectory* dir = parDir->mkdir( dirName.c_str() );
      DirMap_t::value_type dirVal( subPath, dir );
      directories.insert( dirVal );
      return dir;
    }
  }
	
  return file;
}

} // namespace dqi

