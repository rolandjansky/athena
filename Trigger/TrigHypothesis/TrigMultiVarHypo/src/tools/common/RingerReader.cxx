/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include "TTree.h"
#include <iostream>
#include "TParameter.h"
#include "PathResolver/PathResolver.h"

// ringer file reader
#include "TrigMultiVarHypo/tools/common/RingerReader.h"
// interfaces
#include "TrigMultiVarHypo/tools/procedures/IModel.h"
#include "TrigMultiVarHypo/tools/procedures/IThresholds.h"
// models
#include "TrigMultiVarHypo/tools/procedures/MultiLayerPerceptron.h"
#include "TrigMultiVarHypo/tools/procedures/Thresholds.h"
#include "GaudiKernel/StatusCode.h"

using namespace Ringer;

RingerReader::RingerReader(std::string  name):
  asg::AsgMessaging(name),
  m_name(name)
{
  m_etaBins=nullptr;
  m_etBins=nullptr;
  m_thresholds=nullptr;
  m_weights=nullptr;
  m_bias=nullptr;
  m_conv_kernel=nullptr;
  m_conv_nodes=nullptr;
  m_conv_kernel_i=nullptr;
  m_conv_kernel_j=nullptr;


  // current version
  //m_version = 2; //  Used for v6 and v8 tuning for Run 2
  m_version = 3; // Should be used for future in Run 3
  m_useCaloRings=true;
  m_useTileCal=true;
  m_useTrack=false;
  m_useShowerShape=false;

}


// ========================================================================

RingerReader::~RingerReader()
{;}

// ========================================================================
template <class T>
void RingerReader::InitBranch(TTree* fChain, std::string branch_name, T* param){
  bool message=true;
  std::string bname = branch_name;
  if (fChain->GetAlias(bname.c_str()))
     bname = std::string(fChain->GetAlias(bname.c_str()));

  if (!fChain->FindBranch(bname.c_str()) && message) {
    ATH_MSG_WARNING( " unknown branch " << bname );
    return; 
  }
  fChain->SetBranchStatus(bname.c_str(), 1.);
  fChain->SetBranchAddress(bname.c_str(), param);
}
// ========================================================================


bool RingerReader::retrieve( std::string &calibPath, std::vector< std::shared_ptr<IModel> > &discriminators )
{
  m_etaBins=nullptr;
  m_etBins=nullptr;
  m_muBins=nullptr;
  m_weights=nullptr;
  m_bias=nullptr;
  m_nodes=nullptr; 
  m_tfnames=nullptr;
  m_conv_kernel=nullptr;
  m_conv_bias=nullptr;;
  m_conv_nodes=nullptr;
  m_conv_kernel_i=nullptr;
  m_conv_kernel_j=nullptr;
  m_conv_input_i=nullptr;
  m_conv_input_j=nullptr;
  m_conv_tfnames=nullptr;
  m_useConvLayer=nullptr; 
  discriminators.clear();

  ATH_MSG_INFO( "Checking discriminators CalibPath: "<< calibPath );

  auto fullpath = PathResolverFindCalibFile(calibPath);
	TFile file(fullpath.c_str(),"READ");
  auto version = ((TParameter<int>*)file.Get("__version__"))->GetVal();
  TTree *t = (TTree*)file.Get("tuning/discriminators");

  if(version == 2){
	  // Link all branches
    InitBranch(t, "weights", &m_weights);
    InitBranch(t, "bias"   , &m_bias   );
    InitBranch(t, "nodes"  , &m_nodes  );
    InitBranch(t, "etaBin" , &m_etaBins );
    InitBranch(t, "etBin"  , &m_etBins  );
   
    for(Long64_t d=0; d<t->GetEntries();++d){
      t->GetEntry(d);
      // Create the discriminator object
      try{
        std::vector<std::string> tfnames;
        for(unsigned l=0; l<m_nodes->size(); l++) tfnames.push_back("tanh"); // This is default for this version
	  	  discriminators.push_back( std::make_shared<MultiLayerPerceptron>(*m_nodes,*m_weights,*m_bias,tfnames,m_etBins->at(0),
                                                                          m_etBins->at(1),m_etaBins->at(0),m_etaBins->at(1), -999., 999.) ); 
        ATH_MSG_INFO( "Added new discriminator into the list." );
	    }catch(std::bad_alloc &){
        ATH_MSG_ERROR("Can not alloc cutDefs on memory." );
        return false;
      }
    }	// Loop over ttree events	
 
    // retrieve metadata
    m_useCaloRings=true;


  
  }else if ( version == m_version ){ // version 3 (development version for Run 3)

    // Dense configuration
    InitBranch(t, "dense_weights", &m_weights );
    InitBranch(t, "dense_bias"   , &m_bias    );
    InitBranch(t, "dense_nodes"  , &m_nodes   );
    InitBranch(t, "dense_tfnames", &m_tfnames );

    
    // Need to attach the convolutional layer?
    InitBranch(t, "useConvLayer"   , &m_useConvLayer    );

    if(m_useConvLayer->at(0)){
      InitBranch(t, "conv_kernel_i" , &m_conv_kernel_i  );
      InitBranch(t, "conv_kernel_j" , &m_conv_kernel_j  );
      InitBranch(t, "conv_input_i"  , &m_conv_input_i   );
      InitBranch(t, "conv_input_j"  , &m_conv_input_j   );
      InitBranch(t, "conv_tfnames"  , &m_conv_tfnames   );
      InitBranch(t, "conv_nodes"    , &m_conv_nodes     );
      InitBranch(t, "conv_bias"     , &m_conv_bias      );
      InitBranch(t, "conv_kernel"   , &m_conv_kernel    );
      InitBranch(t, "conv_frame"    , &m_conv_frame     );
    }
      
    // Model params
    InitBranch(t, "etaBin" , &m_etaBins );
    InitBranch(t, "etBin"  , &m_etBins  );
    InitBranch(t, "muBin"  , &m_muBins  );

    for(Long64_t d=0; d<t->GetEntries();++d){
      
      t->GetEntry(d);

      // Create the discriminator object
      try{

        if(m_useConvLayer->at(0)){
          ATH_MSG_INFO( "Alloc Convolutional Neural Object into the stack..." );
          /* To be include in the future
          discriminators.push_back(new ConvMultiLayerPerceptron( 
                                    // Dense layers
                                    *m_nodes, *m_weights, *m_bias, *m_tfnames,
                                    // Convolutional layers
                                    *m_conv_frame,
                                    m_conv_input_i->at(0), m_conv_input_j->at(0), *m_conv_nodes, *m_conv_kernel_i, 
                                    *m_conv_kernel_j, *m_conv_kernel, *m_conv_bias ,*m_conv_tfnames,
                                    // Model params
                                    m_etBins->at(0),m_etBins->at(1),m_etaBins->at(0),m_etaBins->at(1), m_muBins->at(0), m_muBins->at(1)
                                    ) 
                                  ); 
          */
        }else{
	  	    discriminators.push_back(std::make_shared< MultiLayerPerceptron >(
                                      // Dense layer
                                      *m_nodes,*m_weights,*m_bias,*m_tfnames,
                                      // Model params
                                      m_etBins->at(0),m_etBins->at(1),m_etaBins->at(0),m_etaBins->at(1), m_muBins->at(0), m_muBins->at(1) 
                                      ) 
                                    );



        }
        
        ATH_MSG_INFO( "Added new discriminator into the list." );
	    }catch(std::bad_alloc &){
        ATH_MSG_ERROR(  "Can not alloc cutDefs on memory." );
        return false;
      }

    }	// Loop over ttree events	
 
    // retrieve metadata
    m_useTrack        = ((TParameter<bool>*)file.Get("metadata/UseTrack"))->GetVal();
    m_useCaloRings    = ((TParameter<bool>*)file.Get("metadata/UseCaloRings"))->GetVal();
    m_useShowerShape  = ((TParameter<bool>*)file.Get("metadata/UseShowerShape"))->GetVal();
    m_useTileCal      = ((TParameter<bool>*)file.Get("metadata/UseTileCal"))->GetVal();

  }
  else{
    ATH_MSG_WARNING("version not supported" );
    return false;
  }

  ATH_MSG_INFO( "Config file version                         : " << version );
  ATH_MSG_INFO( "RemoveOutputTansigTF?                       : " << (m_removeOutputTansigTF?"Yes":"No") );
  ATH_MSG_INFO( "Total of discriminators retrievied is       : " << discriminators.size() );
  ATH_MSG_INFO( "UseCaloRings?                               : " << m_useCaloRings );
  ATH_MSG_INFO( "UseShowerShape?                             : " << m_useShowerShape );
  ATH_MSG_INFO( "UseTrack?                                   : " << m_useTrack );
  ATH_MSG_INFO( "UseTileCal?                                 : " << m_useTileCal );
  file.Close();
  
  return true;
}

// ========================================================================



bool RingerReader::retrieve( std::string &calibPath, std::vector< std::shared_ptr<IThresholds> > &cutDefs )
{
  m_etaBins=nullptr;
  m_etBins=nullptr;
  m_muBins=nullptr;
  m_thresholds=nullptr;

  cutDefs.clear();
  ATH_MSG_INFO("Checking thresholds CalibPath: "<< calibPath );
 	auto fullpath = PathResolverFindCalibFile(calibPath);
	TFile file(fullpath.c_str(),"READ");
  auto version = ((TParameter<int>*)file.Get("__version__"))->GetVal();
  TTree *t = (TTree*)file.Get("tuning/thresholds");
	
  if(version == 2){  
    InitBranch(t, "thresholds", &m_thresholds);
    InitBranch(t, "etBin"     , &m_etBins  );
    InitBranch(t, "etaBin"    , &m_etaBins );
	  
	  for(Long64_t d=0; d<t->GetEntries();++d){
	    t->GetEntry(d);
      // Create the discriminator object
      try{
        cutDefs.push_back( std::make_shared< Thresholds >(*m_thresholds,m_etaBins->at(0),
                                          m_etaBins->at(1), 
                                          m_etBins->at(0),
                                          m_etBins->at(1),
                                          -999. , 
                                          999.));
      }catch(std::bad_alloc &){
        ATH_MSG_ERROR( "Can not alloc cutDefs on memory." );
        return false;
      }
	  }	// Loop over ttree events	
    
    // retrieve metadata

    m_lumiCut = ((TParameter<int>*)file.Get("metadata/LumiCut"))->GetVal();
    m_doPileupCorrection = ((TParameter<bool>*)file.Get("metadata/DoPileupCorrection"))->GetVal();
    m_removeOutputTansigTF = ((TParameter<bool>*)file.Get("metadata/UseNoActivationFunctionInTheLastLayer"))->GetVal();
  
  
  }else if ( version == m_version ){ // version 3 (current)

    InitBranch(t, "thresholds", &m_thresholds);
    InitBranch(t, "etBin"     , &m_etBins  );
    InitBranch(t, "etaBin"    , &m_etaBins );
    InitBranch(t, "muBin"     , &m_muBins );
	  
    for(Long64_t d=0; d<t->GetEntries();++d){
      t->GetEntry(d);
      // Create the discriminator object
      try{
        cutDefs.push_back(std::make_shared< Thresholds >(  *m_thresholds,
                                           m_etaBins->at(0),
                                           m_etaBins->at(1), 
                                           m_etBins->at(0),
                                           m_etBins->at(1),
                                           m_muBins->at(0), 
                                           m_muBins->at(1)) );
 
	    }catch(std::bad_alloc &){
        ATH_MSG_ERROR( "Can not alloc cutDefs on memory." );
        return false;
      }
    }	// Loop over ttree events	
 
    // retrieve metadata
    m_lumiCut               = ((TParameter<int>*)file.Get("metadata/LumiCut"))->GetVal();
    m_doPileupCorrection    = ((TParameter<bool>*)file.Get("metadata/DoPileupCorrection"))->GetVal();
    m_removeOutputTansigTF  = ((TParameter<bool>*)file.Get("metadata/RemoveOutputTansigTF"))->GetVal();
  
  }else{
    ATH_MSG_WARNING("version not supported" );
    return false;
  }
  
  file.Close(); 
  ATH_MSG_INFO( "Config file version                         : " << version );
  ATH_MSG_INFO( "Total of cutDefs retrievied is              : " << cutDefs.size() );
  ATH_MSG_INFO( "Using pileup correction                     : " << (m_doPileupCorrection?"Yes":"No") );
  ATH_MSG_INFO( "Using lumi cut                              : " << (m_lumiCut) );
  return true;
}

    






