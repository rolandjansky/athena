///////////////////////// -*- C++ -*- /////////////////////////////
// METSignificance.cxx
// Implementation file for class METSignificance
// Author: P.Francavilla<francav@cern.ch>
///////////////////////////////////////////////////////////////////

// METUtilities includes
#include "METUtilities/METSignificance.h"

// MET EDM
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETComposition.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"

// Jet EDM
#include "xAODJet/JetAttributes.h"

// Tracking EDM
#include "xAODTracking/TrackParticle.h"

// Shallow copy
#include "xAODCore/ShallowCopy.h"

// Muon EDM
#include "xAODMuon/MuonContainer.h"

// Tools
#include "JetResolution/JERTool.h"
#include "JetResolution/IJERTool.h"
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
#include "ElectronPhotonFourMomentumCorrection/IEgammaCalibrationAndSmearingTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/ITauSmearingTool.h"

#define CHECK_TOOL_RETRIEVE( TOOLHANDLE )         \
  ATH_MSG_VERBOSE("Try to retrieve " << #TOOLHANDLE);       \
  if( TOOLHANDLE.retrieve().isFailure()) {          \
    ATH_MSG_ERROR("Failed to retrieve tool " << #TOOLHANDLE); \
    return StatusCode::FAILURE;             \
  }

#ifdef XAOD_STANDALONE
template <typename T>
static void DeleteTool(ToolHandle<T>& handle)
{
  if (handle.empty())
    return;

  std::string name = handle.name();
  if (asg::ToolStore::contains<T>(name)) {
    auto tool = asg::ToolStore::get<T>(name);
    delete tool;
  }
}
#endif

namespace met {
    
    using std::vector;
    
    using xAOD::MissingET;
    using xAOD::MissingETContainer;
    using xAOD::MissingETAssociation;
    using xAOD::MissingETAssociationMap;
    using xAOD::MissingETAuxContainer;
    using xAOD::MissingETComposition;
    //
    using xAOD::IParticle;
    using xAOD::IParticleContainer;
    //
    using xAOD::JetContainer;
    using xAOD::JetConstituentVector;
    //
    using xAOD::TrackParticle;
    
    typedef ElementLink<xAOD::IParticleContainer> iplink_t;

    
    static const SG::AuxElement::ConstAccessor<float> acc_varX("varX");
    static const SG::AuxElement::ConstAccessor<float> acc_varY("varY");
    static const SG::AuxElement::ConstAccessor<float> acc_covXY("covXY");
    
    static const SG::AuxElement::Decorator< std::vector<iplink_t > > dec_constitObjLinks("ConstitObjectLinks");
    
    const static MissingETBase::Types::bitmask_t invisSource = 0x100000; // doesn't overlap with any other
    
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
    
    // Constructors
    ////////////////
    METSignificance::METSignificance(const std::string& name) :
      AsgTool(name),
      m_jerTool(""),
      m_muonCalibrationAndSmearingTool(""),
      m_egammaCalibTool(""),
      m_tauSmearingTool("")
    {
        //
        // Property declaration
        //
        //declareProperty("TrackSelectorTool",  m_trkseltool                               );
    }
    
    // Destructor
    ///////////////
    METSignificance::~METSignificance()
    {
#ifdef XAOD_STANDALONE
      DeleteTool(m_jerTool);
      DeleteTool(m_muonCalibrationAndSmearingTool);
      DeleteTool(m_egammaCalibTool);
      DeleteTool(m_tauSmearingTool);
#endif
    }
    
    // Athena algtool's Hooks
    ////////////////////////////
    StatusCode METSignificance::initialize()
    {
        ATH_MSG_INFO ("Initializing " << name() << "...");


	ATH_MSG_INFO("Set up JER tools");
	std::string toolName;
	std::string jetcoll = "AntiKt4EMTopoJets";
	toolName = "JERTool_" + jetcoll;
	JERTool* jerTool = new JERTool(toolName);
	ATH_CHECK( jerTool->setProperty("PlotFileName", "JetResolution/Prerec2015_xCalib_2012JER_ReducedTo9NP_Plots_v2.root") );
	ATH_CHECK( jerTool->setProperty("CollectionName", jetcoll) );
	ATH_CHECK( jerTool->initialize() );
	m_jerTool = jerTool;

	ATH_MSG_INFO("Set up MuonCalibrationAndSmearing tools");
	toolName = "MuonCalibrationAndSmearingTool";
	CP::MuonCalibrationAndSmearingTool* muonCalibrationAndSmearingTool = new CP::MuonCalibrationAndSmearingTool(toolName);
	//   MuonCalibrationAndSmearingTool->msg().setLevel( MSG::DEBUG);
	ATH_CHECK( muonCalibrationAndSmearingTool->initialize() );
	m_muonCalibrationAndSmearingTool = muonCalibrationAndSmearingTool;
	
	CP::EgammaCalibrationAndSmearingTool* egammaCalibTool = new CP::EgammaCalibrationAndSmearingTool("EgammaCalibrationAndSmearingTool");
	ATH_MSG_DEBUG( "Initialising EgcalibTool " );
	//ATH_CHECK( egammaCalibTool->setProperty("ESModel", "es2015c_summer") ); //used for analysis using only 2015 data processed with 20.7
	ATH_CHECK( egammaCalibTool->setProperty("ESModel", "es2016PRE") ); //used for analysis using only 2015 data processed with 20.7 (default)
	ATH_CHECK( egammaCalibTool->setProperty("decorrelationModel", "1NP_v1") );
	//if (isAtlfast()) ATH_CHECK( egammaCalibTool->setProperty("useAFII", true) );
	
	ATH_CHECK( egammaCalibTool->initialize() );
	m_egammaCalibTool = egammaCalibTool;
	
	//TauAnalysisTools::TauSmearingTool* tauSmearingTool = new TauAnalysisTools::TauSmearingTool("TauSmearingTool");
	//ATH_CHECK( tauSmearingTool->initialize() );
	//m_tauSmearingTool = tauSmearingTool;


	// retrieve
	CHECK_TOOL_RETRIEVE( m_jerTool );
	CHECK_TOOL_RETRIEVE( m_muonCalibrationAndSmearingTool );
	CHECK_TOOL_RETRIEVE( m_egammaCalibTool );
	CHECK_TOOL_RETRIEVE( m_tauSmearingTool );

        return StatusCode::SUCCESS;
    }
    
    StatusCode METSignificance::finalize()
    {
        ATH_MSG_INFO ("Finalizing " << name() << "...");
        
        return StatusCode::SUCCESS;
    }
    
    
    // **** Rebuild generic MET term ****
    
    
    
    StatusCode METSignificance::varianceMET(xAOD::MissingETContainer* metCont)
    {
        float varx=0;
        float vary=0;
        float covxy=0;
        for(const auto& met : *metCont) {
            if(MissingETBase::Source::isTotalTerm(met->source())) continue;
            if(met->source()==invisSource) continue;
	    // Soft term collection
            if(MissingETBase::Source::isSoftTerm(met->source())){

	      continue;
	    }
            ATH_MSG_VERBOSE("Add MET term " << met->name() );
            for(const auto& el : dec_constitObjLinks(*met)) {
                const IParticle* obj(*el);
		float pt_reso=0.0;
		if(obj->type()==xAOD::Type::Muon){
		  const xAOD::Muon* muon(static_cast<const xAOD::Muon*>(obj)); 

		  int dettype=-1;
		  if(muon->muonType()==0){//Combined
		    dettype=3;//CB
		  }
		  if(muon->muonType()==1){//MuonStandAlone
		    dettype=1;//MS
		  }
		  if(muon->muonType()>1){//Segment, Calo, Silicon
		    dettype=2;//ID
		  }

		  xAOD::Muon my_muon(*muon);

		  //m_muonCalibrationAndSmearingTool->expectedResolution(dettype,muon,!isData());
		  pt_reso=m_muonCalibrationAndSmearingTool->expectedResolution(dettype,my_muon,true);
		}else if(obj->type()==xAOD::Type::Jet){
		  const xAOD::Jet* jet(static_cast<const xAOD::Jet*>(obj));
		  //m_jerTool->getRelResolutionData(jet);
		  pt_reso=m_jerTool->getRelResolutionMC(jet);
		    
		}else if(obj->type()==xAOD::Type::Electron){
		  const xAOD::Electron* ele(static_cast<const xAOD::Electron*>(obj)); 
		  const auto cl_etaCalo = xAOD::get_eta_calo(*(ele->caloCluster()), ele->author());
		  pt_reso=m_egammaCalibTool->resolution(ele->e(),ele->caloCluster()->eta(),cl_etaCalo,PATCore::ParticleType::Electron);
		}else if(obj->type()==xAOD::Type::Photon){
		  const xAOD::Photon* pho(static_cast<const xAOD::Photon*>(obj)); 
		  const auto cl_etaCalo = xAOD::get_eta_calo(*(pho->caloCluster()), pho->author());
		  pt_reso=m_egammaCalibTool->resolution(pho->e(),pho->caloCluster()->eta(),cl_etaCalo,PATCore::ParticleType::Photon);
		}else if(obj->type()==xAOD::Type::Tau){
		  //const xAOD::Tau* tau(*static_cast<const xAOD::Tau*>(obj)); 
		}
		std::cout << "resolution: " << pt_reso << std::endl;
                //const xAOD::Jet* jet(*el);
                //xAOD::Jet* jet(*el)
                if(acc_varX.isAvailable(*obj) && acc_varY.isAvailable(*obj) && acc_covXY.isAvailable(*obj)) {
                    ATH_MSG_VERBOSE("Add object with vars " << acc_varX(*obj)<<","<<acc_varY(*obj)<<","<<acc_covXY(*obj));
                    varx+=acc_varX(*obj);
                    vary+=acc_varY(*obj);
                    covxy+=acc_covXY(*obj);
                    //met->add(acc_varX(*el),acc_varY(*el),acc_covXY(*el));
                }
            }
        }
        return StatusCode::SUCCESS;
    }

    
    
    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////
    // Protected methods:
    ///////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////
    
} //> end namespace met
