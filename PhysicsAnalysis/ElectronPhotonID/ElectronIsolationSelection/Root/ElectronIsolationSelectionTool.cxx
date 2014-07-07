/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronIsolationSelectionTool.cxx 605168 2014-07-07 14:12:31Z morrisj $

// Local include(s):
#include "ElectronIsolationSelection/ElectronIsolationSelectionTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <cmath>

namespace CP {

   ElectronIsolationSelectionTool::ElectronIsolationSelectionTool( const std::string& name )
      : asg::AsgTool( name ),
      m_inputFile("EisoTool2012_data12_Zee.root"),
      m_sgKeyEventInfo("EventInfo"),
      m_sgKeyVertex("PrimaryVertices"),
      m_eiso(0),
      m_showerDepthTool( "CP::ShowerDepthTool/ShowerDepthTool" ),
      m_accept( "ElectronIsolationSelectionTool" ){

       
       declareProperty( "inputFile", m_inputFile = "EisoTool2012_data12_Zee.root" );
       declareProperty( "SGKeyEventInfo", m_sgKeyEventInfo = "EventInfo" );
       declareProperty( "SGKeyVertex"   , m_sgKeyVertex = "PrimaryVertices" );
       declareProperty( "ShowerDepthTool", m_showerDepthTool );
   }

   StatusCode ElectronIsolationSelectionTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising..." );
            
      TString inputFile = this->findInputFile_ExpectBetterSolutionFromASG();
      m_eiso = new EisoTool2012(inputFile);
      
      // Retrieve the tools:
      ATH_CHECK( m_showerDepthTool.retrieve() );      

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   const Root::TAccept& ElectronIsolationSelectionTool::getTAccept() const {

      return m_accept;
   }

   const Root::TAccept&
   ElectronIsolationSelectionTool::accept( const xAOD::IParticle* p ) const {

     
     ATH_MSG_INFO(" Please do not use this method");
      // Reset the result:
      m_accept.clear();

      // Check if this is a muon:
      if( p->type() != xAOD::Type::Electron ) {
         ATH_MSG_ERROR( "accept(...) Function received a non-electron" );
         return m_accept;
      }
      
      return m_accept;
   }

   const Root::TAccept& ElectronIsolationSelectionTool::acceptEtcone( const xAOD::Electron& ele , const float& coneSize, const unsigned int& Efficiency_required ) const
   {
     bool debug(false);
      // Reset the result:
      m_accept.clear();
      m_accept.addCut( "Isolation" , "Isolation cut provided by ElectronIsolationSelectionTool" ); 
      
      if( this->validEtEtaRange( ele ) ){

        unsigned int pass = m_eiso->Etcone( coneSize ,
                                            Efficiency_required , 
                                            this->getNPV() ,
                                            this->getElectronClusterEnergy( ele ),
                                            this->getElectronTrackEta( ele ),
                                            this->getElectronClusterEta( ele ),
                                            this->getElectronEtaS2( ele ),
                                            this->getElectronEtap( ele , this->isMC() ),
                                            this->getElectronEtcone( ele , coneSize ),
                                            this->isMC()
                                          );
        if( pass == 1 ){
          m_accept.setCutResult( "Isolation", true );
        }          
        
        if( debug ){
        
          float corEtcone = m_eiso->EtconeCorrected(  this->getNPV(),
                                                      this->getElectronClusterEnergy( ele ),
                                                      this->getElectronClusterEta( ele ),
                                                      this->getElectronEtaS2( ele ),
                                                      this->getElectronEtap( ele , this->isMC() ),
                                                      this->getElectronEtcone( ele , coneSize ),
                                                      coneSize
                                                    );
          
          float cutVal = m_eiso->getCutValueEtcone( coneSize ,
                                                    Efficiency_required,
                                                    this->getElectronClusterEnergy( ele ),
                                                    this->getElectronTrackEta( ele ),
                                                    this->getElectronClusterEta( ele )                                               
                                                  );   
          
          ATH_MSG_INFO(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ");
          ATH_MSG_INFO("  Isolation Debug Test " );
          ATH_MSG_INFO("    cone , eff , NPV = "<<coneSize<<" , "<<Efficiency_required<<" , "<<this->getNPV() );
          ATH_MSG_INFO("    elec Cluster Energy , track Eta , cluster Eta = "<<this->getElectronClusterEnergy( ele )<<" , "<<this->getElectronTrackEta( ele )<<" , "<<this->getElectronClusterEta( ele ));
          ATH_MSG_INFO("    EtaS2 , Etap = "<<this->getElectronEtaS2(ele )<<" , "<<this->getElectronEtap( ele , this->isMC() ) );
          ATH_MSG_INFO("    Etcone = "<<this->getElectronEtcone(ele , 20 ) );
          ATH_MSG_INFO("    Electron Et , eta= "<<this->getElectronClusterEnergy(ele)/cosh(this->getElectronTrackEta(ele))<<" , "<<this->getElectronClusterEta(ele));
          ATH_MSG_INFO("    Corrected Etcone , CutVal = "<<corEtcone/1000.<<" < "<<cutVal);
          ATH_MSG_INFO("    Pass ?? "<<pass);
          ATH_MSG_INFO("  ");
        } // debug
        
      } // Valid Et eta range
      
      // Return the result:
      return m_accept;
   }
   
   
    const Root::TAccept& ElectronIsolationSelectionTool::acceptPtcone( const xAOD::Electron& ele , const float& coneSize, const unsigned int& Efficiency_required ) const
    {
      // Reset the result:
      m_accept.clear();
      m_accept.addCut( "Isolation" , "Isolation cut provided by ElectronIsolationSelectionTool" );       
      
      if( this->validEtEtaRange( ele ) ){
      
        unsigned int pass = m_eiso->Ptcone( coneSize, 
                                            Efficiency_required,
                                            this->getElectronClusterEnergy( ele ),
                                            this->getElectronTrackEta( ele ),
                                            this->getElectronClusterEta( ele ),
                                            this->getElectronPtcone( ele , coneSize )
                                    );
        
        if( pass == 1 ){
          m_accept.setCutResult( "Isolation", true );
        }
      
      } // valid Et Eta range
      
      // Return the result:
      return m_accept;           
    }
    
    const Root::TAccept& ElectronIsolationSelectionTool::acceptTopocone( const xAOD::Electron& ele , const float& coneSize, const unsigned int& Efficiency_required ) const
    {
      // Reset the result:
      m_accept.clear();
      m_accept.addCut( "Isolation" , "Isolation cut provided by ElectronIsolationSelectionTool" ); 
      
      ATH_MSG_INFO(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
      ATH_MSG_INFO(" Topoetcone isolatio does not work at present. ");
      ATH_MSG_INFO(" There is no xAOD access to the variable ED_Median" );
      ATH_MSG_INFO(" Unable to apply corrections to topoetcone "<<coneSize);
      ATH_MSG_INFO("    Efficiency requested = "<<Efficiency_required);
      ATH_MSG_INFO("    Electron pT , eta = "<<ele.pt()<<" , "<<ele.eta());
      ATH_MSG_INFO(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
      
      // Return the result:
      return m_accept;        
    }
   
    bool ElectronIsolationSelectionTool:: validEtEtaRange( const xAOD::Electron& ele ) const
    {
      if( this->getElectronClusterEnergy(ele) / cosh( this->getElectronTrackEta(ele) ) > 10000.0 ){
        float feta = fabs( this->getElectronClusterEta(ele) );
        if( feta < 2.47 ){
          if( !(feta > 1.37 && feta < 1.52) ){
            return true;
          }
        }
      }
      return false;
    }
    
    bool ElectronIsolationSelectionTool::isMC() const
    {
      const xAOD::EventInfo* eventInfo = 0;
      ATH_CHECK( evtStore()->retrieve( eventInfo, m_sgKeyEventInfo ) );
      return eventInfo->eventTypeBitmask() > 1 ? true : false;
    }
    
    float ElectronIsolationSelectionTool::getElectronClusterEnergy( const xAOD::Electron& ele ) const
    {
      return ele.caloCluster()->e();
    }
    
    float ElectronIsolationSelectionTool::getElectronClusterEta( const xAOD::Electron& ele ) const
    {
      return ele.caloCluster()->eta();
    }
    
    float ElectronIsolationSelectionTool::getElectronTrackEta( const xAOD::Electron& ele ) const
    {
       return ele.trackParticle()->eta();     
    }
    
    float ElectronIsolationSelectionTool::getElectronEtap( const xAOD::Electron& ele , const bool& isMC ) const
    {
      float value(0.0);
      value = m_showerDepthTool->getCaloPointingEta( ele.caloCluster()->etaBE(1) ,
                                                     ele.caloCluster()->etaBE(2) ,
                                                     ele.phi() ,
                                                     !isMC );
      return value;      
    }
    
    float ElectronIsolationSelectionTool::getElectronEtaS2( const xAOD::Electron& ele ) const
    {
      return ele.caloCluster()->etaBE(2);
    }
    
    float ElectronIsolationSelectionTool::getElectronEtcone( const xAOD::Electron& ele , const float& coneSize ) const
    {
      float value(0.0);
      if( coneSize == 0.2 || coneSize == 20 ){
        ele.isolationValue(value,xAOD::EgammaParameters::etcone20  );
      }
      if( coneSize == 0.3 || coneSize == 30 ){
        ele.isolationValue(value,xAOD::EgammaParameters::etcone30  );
      }      
      if( coneSize == 0.4 || coneSize == 40 ){
        ele.isolationValue(value,xAOD::EgammaParameters::etcone40  );
      }         
      return value;
    }
        
    float ElectronIsolationSelectionTool::getElectronPtcone( const xAOD::Electron& ele , const float& coneSize ) const
    {
      float value(0.0);
      if( coneSize == 0.2 || coneSize == 20 ){
        ele.isolationValue(value,xAOD::EgammaParameters::ptcone20  );
      }
      if( coneSize == 0.3 || coneSize == 30 ){
        ele.isolationValue(value,xAOD::EgammaParameters::ptcone30  );
      }      
      if( coneSize == 0.4 || coneSize == 40 ){
        ele.isolationValue(value,xAOD::EgammaParameters::ptcone40  );
      }             
      return value;      
    }
    
            
    float ElectronIsolationSelectionTool::getElectronTopocone( const xAOD::Electron& ele , const float& coneSize ) const
    {
      float value(0.0);
      if( coneSize == 0.2 || coneSize == 20 ){
        ele.isolationValue(value,xAOD::EgammaParameters::topoetcone20  );
      }
      if( coneSize == 0.3 || coneSize == 30 ){
        ele.isolationValue(value,xAOD::EgammaParameters::topoetcone30  );
      }      
      if( coneSize == 0.4 || coneSize == 40 ){
        ele.isolationValue(value,xAOD::EgammaParameters::topoetcone40  );
      }       
      return value;      
    }
    
    
    int ElectronIsolationSelectionTool::getNPV() const
    {
      int npv(0);
      const xAOD::VertexContainer* vtx = 0;
      ATH_CHECK( evtStore()->retrieve( vtx, m_sgKeyVertex ) );
      
      xAOD::VertexContainer::const_iterator itr = vtx->begin();
      xAOD::VertexContainer::const_iterator itr_end = vtx->end();
      for( ; itr != itr_end; ++itr ) {     
        if( (*itr)->vertexType() == 1 || (*itr)->vertexType() == 3 ){
          if( (*itr)->nTrackParticles() >= 2 ){
            npv++;
          }
        }
      }   
      return npv;
    }
        
    TString ElectronIsolationSelectionTool::findInputFile_ExpectBetterSolutionFromASG()
    {
        
      std::string s_path;
      //now setting the correct path for the file. Borrowed from Fede (MuonMomentumCorrections)
      char *rootCoreArea = getenv("ROOTCOREBIN");
      char *testArea = getenv("TestArea");

      // ROOTCore: Data folder
      if(rootCoreArea != NULL){
        s_path = std::string(rootCoreArea) + "/data/egammaAnalysisUtils/";
      }
      // Athena: InstallArea
      else if ( testArea != NULL ){
        s_path = std::string(testArea) + "/InstallArea/share/";
      }
      
      TString fileName = s_path + m_inputFile;
      return fileName; 
    }

} // namespace CP
