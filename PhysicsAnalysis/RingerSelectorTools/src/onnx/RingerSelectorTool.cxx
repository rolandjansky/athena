/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local includes:
#include "RingerSelectorTools/tools/onnx/RingerSelectorTool.h"
#include "PathResolver/PathResolver.h"
#include <algorithm>
#include "TEnv.h"

using namespace Monitored;


namespace Ringer{

  namespace onnx{


    //==============================================================================
    RingerSelectorTool::RingerSelectorTool():
      asg::AsgMessaging("RingerSelectorTool")
    {;}
    

    //==============================================================================
    StatusCode RingerSelectorTool::read_from( std::string path , AthONNX::IONNXRuntimeSvc *svc )
    {

      std::string configFile = PathResolverFindCalibFile( path );

      ATH_MSG_INFO( "Loading all tunings configs from " << configFile );

      if (configFile.empty()){
         ATH_MSG_ERROR( "Could not locate the config file: " << configFile);
         return StatusCode::FAILURE; 
      }

      // Read configuration file
      TEnv env(configFile.c_str());
   

      // Retrieve all models
      {
        // Retrieve the size
        unsigned size = env.GetValue( "Model__size" , 0 );
        m_useShowerShapes = env.GetValue( "Model__UseShowerShapes", false);
        m_useTrackVariables = env.GetValue( "Model__UseTrackVariables", false);
          
        // Retrieve Et bins
        auto etmin = GetValues<float>( "Model__etmin", env );
        auto etmax  = GetValues<float>( "Model__etmax", env );
        
        // Retrieve Eta bins
        auto etamin = GetValues<float>( "Model__etamin", env );
        auto etamax  = GetValues<float>( "Model__etamax", env );
        
        // Retreive all ONNX model file paths
        auto model_paths = GetPaths( "Model__path", env );
        std::string basepath = configFile.substr( 0, configFile.find_last_of("/"));
        
        // Loop over all models
        for ( unsigned idx = 0; idx < size; ++idx ){
          std::string modelPath = PathResolverFindCalibFile( basepath+"/"+model_paths[idx] );
          //std::string modelPath = basepath+"/"+model_paths[idx];
          ATH_MSG_INFO( "Reading Onnx model from: " << modelPath );
          auto model = Ringer::onnx::Model( modelPath, svc, etmin[idx], etmax[idx], etamin[idx], etamax[idx]) ;
          
          // Compile the model
          model.compile();
          m_models.push_back(model);
        }
      }

      // Retrieve all thresholds
      {
        // Retrieve the size
         unsigned size = env.GetValue( "Threshold__size" , 0);
  
        
           
         auto max_avgmu = GetValues<float>( "Threshold__MaxAverageMu", env );
         
         // Retrieve Et bins
         auto etmin = GetValues<float>( "Threshold__etmin", env );
         auto etmax = GetValues<float>( "Threshold__etmax", env );
         // Retrieve Eta bins
         auto etamin  = GetValues<float>( "Threshold__etamin", env );
         auto etamax  = GetValues<float>( "Threshold__etamax", env );
         // Retrieve slopes
         auto slopes = GetValues<float>( "Threshold__slope", env );
         // Retrieve offsets
         auto offsets = GetValues<float>("Threshold__offset", env );

         for ( unsigned idx = 0; idx <size; ++idx ){
           m_thresholds.push_back( Ringer::onnx::Threshold(
                                     etmin[idx],
                                     etmax[idx],
                                     etamin[idx],
                                     etamax[idx],
                                     slopes[idx],
                                     offsets[idx],
                                     max_avgmu[idx])
                                 );
          }
      }

      return StatusCode::SUCCESS;
    }


    //==============================================================================
    float RingerSelectorTool::predict(const xAOD::TrigRingerRings *ringerShape, const xAOD::TrigElectron *el ,
                                      Monitored::Timer<> &propagate_time, Monitored::Timer<> &preproc_time) const
    {
      float et = ringerShape->emCluster()->et()/Gaudi::Units::GeV;
      float eta = std::min( std::abs(ringerShape->emCluster()->eta()), float(2.5) );
 
      // Find the correct model and predict
      for( auto& model : m_models ){
        
        ATH_MSG_DEBUG("model_Etmin: "<<model.etMin()<<" model_Etmax: "<<model.etMax());
        if(et<=model.etMin() || et > model.etMax()) continue;
        if(eta<=model.etaMin() || eta > model.etaMax()) continue;
        
        ATH_MSG_DEBUG("Event Et: "<< et <<" is between model Et_min: "<< model.etMin() <<" and model Et_max: "<<model.etMax());
        ATH_MSG_DEBUG("Event eta: "<< eta <<" is between model eta_min: "<< model.etaMin() <<" and model eta_max: "<<model.etaMax());
        std::vector< std::vector<float> > inputs;
                    
        preproc_time.start();
        prepare_inputs( ringerShape, el, inputs );
        preproc_time.stop();

        propagate_time.start();
        auto output = model.predict( inputs );       
        propagate_time.stop();
        
        ATH_MSG_DEBUG( "The current model predict with output: " << output );
        return output;
      }

      return -999;
    }



    //==============================================================================
    float RingerSelectorTool::predict(const xAOD::TrigRingerRings *ringerShape , Monitored::Timer<> &propagate_time, 
                                      Monitored::Timer<> &preproc_time ) const
    {
      float et = ringerShape->emCluster()->et()/Gaudi::Units::GeV;
      float eta = std::min( std::abs(ringerShape->emCluster()->eta()), float(2.5));
 
      // Find the correct model and predict
      for( auto& model : m_models ){
        ATH_MSG_DEBUG("model_Etmin: "<<model.etMin()<<" model_Etmax: "<<model.etMax());
        
        if(et<=model.etMin() || et > model.etMax()) continue;
        if(eta<=model.etaMin() || eta > model.etaMax()) continue;
 
        
        ATH_MSG_DEBUG("Event Et: "<< et <<" is between model Et_min: "<< model.etMin() <<" and model Et_max: "<<model.etMax());
        ATH_MSG_DEBUG("Event eta: "<< eta <<" is between model eta_min: "<< model.etaMin() <<" and model eta_max: "<<model.etaMax());
        std::vector< std::vector<float> > inputs;
        
        preproc_time.start();
        prepare_inputs( ringerShape, nullptr, inputs );
        preproc_time.stop();

        propagate_time.start();
        auto output = model.predict( inputs );       
        propagate_time.stop();

        ATH_MSG_DEBUG( "The current model predict with output: " << output );
        return output;
      }

      return -999;
    }



   
    //==============================================================================
    bool RingerSelectorTool::accept( const xAOD::TrigRingerRings *ringerShape, float discriminant, float avgmu ) const 
    {
      float et = ringerShape->emCluster()->et()/Gaudi::Units::GeV;
      float eta = std::min( std::abs(ringerShape->emCluster()->eta()), float(2.5));
    
      ATH_MSG_DEBUG( "Event et = "<< et << ", eta = " << eta );
      for( auto& cutDef : m_thresholds ){
        if ( et <= cutDef.etMin() || et > cutDef.etMax() ) continue;
        if ( eta <= cutDef.etaMin() || eta > cutDef.etaMax() ) continue;
        ATH_MSG_DEBUG( "Output = " << discriminant << " Avgmu = " << avgmu );
        ATH_MSG_DEBUG( "Passed ? " << cutDef.accept( discriminant, avgmu ) );
        return cutDef.accept( discriminant, avgmu );
      }// loop over all thresholds
    
      return false;
    }


    //==============================================================================
    void RingerSelectorTool::prepare_inputs(  const xAOD::TrigRingerRings *ringerShape, 
                                              const xAOD::TrigElectron *el, 
                                              std::vector<std::vector<float>>&inputs ) const
    {
      const std::vector<float> rings = ringerShape->rings();
      std::vector<float> refRings(rings.size());
      refRings.assign(rings.begin(), rings.end());
       
      float energy=0.0;
      for(auto &ring : refRings ) energy+=ring;
      for(auto &ring : refRings ) ring/=std::abs(energy);
      
      inputs.push_back( refRings );


      if ( m_useShowerShapes ){
        std::vector<float> refShowers;
        const xAOD::TrigEMCluster *pClus = ringerShape->emCluster();
        
        float e0 = pClus->energy( CaloSampling::PreSamplerB ) + pClus->energy( CaloSampling::PreSamplerE );
        float e1 = pClus->energy( CaloSampling::EMB1 ) + pClus->energy( CaloSampling::EME1 );
        float e2 = pClus->energy( CaloSampling::EMB2 ) + pClus->energy( CaloSampling::EME2 );
        float e3 = pClus->energy( CaloSampling::EMB3 ) + pClus->energy( CaloSampling::EME3 );
        float eallsamples = e0+e1+e2+e3;
        float f3 = std::abs( eallsamples )>0. ? e3/eallsamples : 0.;

        float eratio = pClus->emaxs1() - pClus->e2tsts1();
        eratio/= (pClus->emaxs1() + pClus->e2tsts1());
        float weta2 = pClus->weta2();
        float wstot = pClus->wstot();
        float reta = pClus->e237() / pClus->e277() ; 


        float etot=0.0;
        for ( float e : pClus->energySample() )  etot+=e;
        
        float f1 = e1/etot;

        refShowers.push_back( reta/1.);
        refShowers.push_back( eratio/1. );
        refShowers.push_back( f1/0.6 );
        refShowers.push_back( f3/0.04 );
        refShowers.push_back( weta2/0.02 );
        refShowers.push_back( wstot/1. );

        inputs.push_back( refShowers );
      }

      if ( m_useTrackVariables && el){
        std::vector<float> refTrack;
        refTrack.push_back( el->etOverPt() );
        refTrack.push_back( el->trkClusDeta() );
        refTrack.push_back( el->trkClusDphi() );
        inputs.push_back( refTrack );
      }

    }


    //==============================================================================
    template <typename T> bool RingerSelectorTool::strtof(const std::string& input, T& f)
    {  
      int diff = 0 ;
      std::string tmp = input;
      std::string::size_type first(0);
      std::string::size_type last(0);
      first = ( input.find('#') ) ;
 
      //if we do not find a comment character "#" we are fine
      if (first == std::string::npos) {
        std::istringstream buffer (tmp);
        buffer>>f;
        return true;
      } 
      
      //if we have found comment character check if it is inlined between two "#"
      last = (input.find('#',first+1) );
      //if nor error
      if (last == std::string::npos) {
        return false;
      }
      //else if between two "#" remove this part 
      diff = last - first ;
      tmp= tmp.erase(first,diff+1);
      std::istringstream buffer (tmp);
      buffer>>f;
      return true;
    }


    //==============================================================================
    template <typename T>  std::vector<T> RingerSelectorTool::GetValues (const std::string& input,  TEnv& env)
    {    
      std::vector<T> CutVector;    
      std::string env_input(env.GetValue(input.c_str(), ""));
      if (!env_input.empty()) {
        std::string::size_type end;
        do {
          end = env_input.find(';');
          T  myValue(0);
          if(strtof(env_input.substr(0,end),myValue)){
            CutVector.push_back(myValue);
          }
          if (end != std::string::npos) {
            env_input= env_input.substr(end+1);
          }
        } while (end != std::string::npos);     
      }
      return CutVector;
    }


    //==============================================================================
    std::vector<std::string> RingerSelectorTool::GetPaths(const std::string& input, TEnv& env)
    {
      std::vector<std::string> CutVector;    
      std::string env_input(env.GetValue(input.c_str(), ""));
      if (!env_input.empty()) {
        std::string::size_type end;
        do {
          end = env_input.find(';');
          CutVector.push_back( env_input.substr(0,end) );
          if (end != std::string::npos) {
            env_input= env_input.substr(end+2);
          }
        } while (end != std::string::npos);     
      }
      return CutVector;
    }




  } // namespace onnx
} // namespace Ringer


