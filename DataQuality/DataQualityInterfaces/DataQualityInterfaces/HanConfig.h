/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiHanConfig_h
#define dqiHanConfig_h

#include <string>
#include <set>
#include <map>
#include <TList.h>
#include <TMap.h>

#include <TObject.h>

#include "DataQualityInterfaces/MiniConfigTreeNode.h"
#include "DataQualityInterfaces/HanConfigAssessor.h"
#include "DataQualityInterfaces/HanConfigCompAlg.h"

#ifndef __CINT__
#include <boost/shared_ptr.hpp>
#endif

class TDirectory;
class TFile;
class TKey;

namespace dqm_core {
  class Input;
  class Output;
  class Region;
}


namespace dqi {

class MiniConfig;
class HanConfigAssessor;
class HanConfigGroup;
class HanOutput;

class HanConfig : public TObject {
public:

  HanConfig();
  virtual ~HanConfig();
  
  
  virtual void AssembleAndSave( std::string infileName, std::string outfileName );
  
  virtual void BuildMonitors( std::string configName, dqm_core::Input& input, HanOutput& output );
#ifndef __CINT__
  virtual boost::shared_ptr<dqm_core::Region> BuildMonitorsNewRoot( std::string configName, dqm_core::Input& input, dqm_core::Output& output );
#endif
  virtual void BuildConfigOutput( std::string configName, TFile* inputFile, std::string path,
                                  std::map<std::string,TSeqCollection*>* outputMap, TSeqCollection *outputList );
  
  virtual TObject* GetReference( std::string& groupName, std::string& name );
  virtual const HanConfigAssessor* GetAssessor( std::string& groupName, std::string& name ) const;

  virtual void GetRegexList( std::set<std::string>& regexlist );

protected:

  typedef std::map<std::string,TDirectory*> DirMap_t;


  class RefVisitor : public MiniConfigTreeNode::Visitor {
  public:
    RefVisitor( TFile* outfile_, HanConfig::DirMap_t& directories_, TMap* refsourcedata );
    virtual void Visit( const MiniConfigTreeNode* node ) const;
  protected:
    TFile* outfile;
    HanConfig::DirMap_t& directories;
    TMap* m_refsourcedata;
  };
  
  class AssessmentVisitorBase : public MiniConfigTreeNode::Visitor {
  public:
    AssessmentVisitorBase( HanConfigGroup* root_, const MiniConfig& algConfig_,
                           const MiniConfig& thrConfig_, const MiniConfig& refConfig_,
                           TFile* outfile_, HanConfig::DirMap_t& directories_,
			   TMap* refsourcedata_ );
  
  protected:
  
    void GetAlgorithmConfiguration( HanConfigAssessor* dqpar, const std::string& algID,
                                    std::string assessorName = "" ) const;
    
    HanConfigGroup* root;
    const MiniConfig& algConfig;
    const MiniConfig& thrConfig;
    const MiniConfig& refConfig;
    TFile* outfile;
    HanConfig::DirMap_t& directories;
    TMap* m_refsourcedata;
  };
  
  
  class RegionVisitor : public AssessmentVisitorBase {
  public:
    RegionVisitor( HanConfigGroup* root_, const MiniConfig& algConfig_,
                   const MiniConfig& thrConfig_, const MiniConfig& refConfig_,
                   HanConfig::DirMap_t& directories_ );
    virtual void Visit( const MiniConfigTreeNode* node ) const;
  };
  
  
  class AssessmentVisitor : public AssessmentVisitorBase {
  public:
    AssessmentVisitor( HanConfigGroup* root_, const MiniConfig& algConfig_,
                       const MiniConfig& thrConfig_, const MiniConfig& refConfig_,
                       TFile* outfile_, HanConfig::DirMap_t& directories_,
		       TMap* refsourcedata_ );
    virtual void Visit( const MiniConfigTreeNode* node ) const;
  };

#ifndef __CINT__
  class RegexVisitor : public HanConfigAssessor::Visitor {
  public:
    RegexVisitor( std::set<std::string>& regexes_ );
    virtual boost::shared_ptr<dqm_core::Node>
      Visit( const HanConfigAssessor* node, boost::shared_ptr<dqm_core::Region> ) const;
  protected:
    std::set<std::string>& regexes;
  };  
  
  class ConfigVisitor : public HanConfigAssessor::Visitor {
  public:
    ConfigVisitor( TFile* file_, dqm_core::Output* output_ );
    virtual boost::shared_ptr<dqm_core::Node>
      Visit( const HanConfigAssessor* node, boost::shared_ptr<dqm_core::Region> dqParent ) const;
  protected:
    TFile* file;
    dqm_core::Output* output;
  };
#endif
  
  class CompAlgVisitor : public MiniConfigTreeNode::Visitor {
  public:
    CompAlgVisitor( TFile* outfile_ , const MiniConfig& compAlgConfig_);
    virtual void Visit( const MiniConfigTreeNode* node ) const;
  protected:
    TFile* outfile;
    const MiniConfig& compAlgConfig;
  };
  
  class MetadataVisitor : public MiniConfigTreeNode::Visitor {
  public:
    MetadataVisitor( TFile* outfile_ , const MiniConfig& metadataConfig_);
    virtual void Visit( const MiniConfigTreeNode* node ) const;
  protected:
    TFile* outfile;
    const MiniConfig& metadataConfig;
  };
  

  bool Initialize( std::string configName );
  
  
  TFile*             config;
#ifndef __CINT__
  boost::shared_ptr<dqm_core::Region>  dqRoot;
#endif
  HanConfigGroup*    top_level;
  TSeqCollection*    metadata;
  
//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef( HanConfig, 0 ) // Creates a Han configuration from a MiniConfig
///\endcond

private:

  static TKey* GetObjKey( TDirectory* dir, std::string path );
  static TDirectory* ChangeInputDir( TDirectory* dir, std::string path );
  static TDirectory* ChangeOutputDir( TFile* file, std::string path, DirMap_t& directories );

};

} // namespace dqi

#endif
