/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetIsolationTool.cxx 

#include "JetMomentTools/JetIsolationTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "JetUtils/JetDistances.h"
#include "JetRec/PseudoJetGetterRegistry.h"
#include <sstream>

using std::string;
using fastjet::PseudoJet;

namespace jet {

  namespace JetIsolation {

    typedef TLorentzVector FourMom_t;

    /// IsolationCalculator : base class for isolation calculations
    class IsolationCalculator {
    public:

      /// Define the availble isolation variables
      enum Kinematics {
        SumPt, Par, Perp, P
      };

      /// names for isolation variables
      static const string s_kname[4];

      /// Holds the 4-vector of all constituents contributing to isolation.
      ///  accessors to corresponding isolation variables.
      struct IsolationResult {
        double isoSumPt(){ return m_isoSumPt;}
        double isoPar(const TVector3 &unitV){ return m_isoP.Vect().Dot(unitV);}
        double isoPerp(const TVector3 &unitV){ return m_isoP.Vect().Perp(unitV);}
        FourMom_t & isoP(){ return m_isoP;}
        
        FourMom_t m_isoP ;
        double m_isoSumPt;
        
      };
      
      
      virtual ~IsolationCalculator() {}

      
      virtual string baseName() const {return "";}
      virtual IsolationCalculator * clone(const xAOD::Jet* ) const {return NULL;}
      virtual void copyFrom( const IsolationCalculator* o, const xAOD::Jet*){
        m_attNames = o->m_attNames;
        m_kinematics = o->m_kinematics;
      }
      
        
      /// Compute the isolation momentum from jet and jet inputs.
      /// It is assumed the caller has already removed jet constituents from the input list.
      virtual IsolationResult jetIsolation(const xAOD::Jet*, std::vector<jet::ParticlePosition> & ) const {
        return IsolationResult();
      }
      virtual void
      setIsolationAttributes(xAOD::Jet* jet, std::vector<jet::ParticlePosition>& nearbyConstit) const {
        IsolationResult result = jetIsolation(jet, nearbyConstit);
        for ( size_t i=0; i<m_kinematics.size(); ++i ) {
          switch( m_kinematics[i] ) {
          case Perp  : jet->setAttribute<float>( m_attNames[i], result.isoPerp(jet->p4().Vect()) ); break;
          case SumPt : jet->setAttribute<float>( m_attNames[i], result.isoSumPt() ); break;
          case Par   : jet->setAttribute<float>( m_attNames[i], result.isoPar(jet->p4().Vect()) ); break;
          case P     : jet->setAttribute<float>( m_attNames[i], result.isoP().Vect().Mag() ); break;
          }
        }

      }
      
      bool scheduleKinematicCalculation(string kname){
        for( size_t i=0; i<4 ;i++){  
          if( s_kname[i]==kname) {
            m_kinematics.push_back( (Kinematics) i);
            m_attNames.push_back( baseName()+kname );
            return true;        
          }
        }
        return false;
      }
      

      void dump(){
        std::cout << "Isolation calculator "<< baseName() <<std::endl;
        for( size_t i =0 ; i<m_attNames.size(); i++){
          std::cout << "   - "<< m_attNames[i] << std::endl; 
        }
      }
      
    protected:
      std::vector<Kinematics> m_kinematics; /// kinematics isolation vars to be computed
      std::vector<string> m_attNames;  /// kinematics isolation vars names
    };

  const string IsolationCalculator::s_kname[4] =  {"SumPt", "Par", "Perp", "P"};

    
    struct PseudoJetKinematicGetter {
      const FourMom_t getP(jet::ParticlePosition& p) const {
        return TLorentzVector(p.px(), p.py(), p.pz(), p.e());
      }
      double getPt(jet::ParticlePosition& p) const { return p.pt();}
    };

    template<typename ISOCRITERIA, typename KINEMATICGETTER = PseudoJetKinematicGetter>
    class IsolationCalculatorT : public IsolationCalculator {
    public:

      IsolationCalculatorT(double param=0.) : m_iso(param) { }

      virtual IsolationResult
      jetIsolation(const xAOD::Jet* jet, std::vector<jet::ParticlePosition> &nearbyConstit) const {
        IsolationResult result;
        result.m_isoSumPt = 0;
        int contributed = 0;
        for ( size_t i=0 ; i<nearbyConstit.size(); ++i ) {
          if ( m_iso.inIsolationArea(jet, nearbyConstit[i]) ) {
            result.m_isoP     += m_kine.getP(nearbyConstit[i]);
            result.m_isoSumPt += m_kine.getPt(nearbyConstit[i]);
            ++contributed;
          }    
        }
        return result;
      }

      virtual string baseName() const {return m_iso.name();}

      virtual IsolationCalculator * clone(const xAOD::Jet* j) const {
        IsolationCalculator* iso;
        auto* isoT= new IsolationCalculatorT();
        isoT->m_iso = m_iso;
        isoT->m_iso.setup(j);
        isoT->m_kine = m_kine;
        iso = isoT;
        iso->copyFrom(this,j);
        return iso;
      }

      ISOCRITERIA m_iso;
      KINEMATICGETTER m_kine;
    };
    
    
    /// \class IsolationAreaBase 
    ///  Defines a zone from which constituents will contribute to the isolation of a jet.
    /// In most cases, the zone is simply a cone.
    struct IsolationAreaBase {
      IsolationAreaBase(double p, const string &n) : m_parameter(p), m_name(n){}
      
      bool inIsolationArea(const xAOD::Jet* j, jet::ParticlePosition& part) const {
        double dr2 = JetDistances::deltaR(j->eta(), j->phi(), part.x(), part.y());
        return dr2 < m_deltaRmax2;
      }
      
      string name() const {
        std::ostringstream oss; oss << m_name << int(10*m_parameter);
        return oss.str();        
      }

      double m_parameter;
      string m_name;
      double m_deltaRmax2;
      
    }; 
    
      
    
    // here we define a short cut to declare implementation of IsolationAreaBase classes.
    // In most cases, only a definition of the max deltaR is enough to specify the area, hence these shortcuts. 
    // 1st parameter : class name
    // 2nd parameter : code defining the max deltaR
    // 3rd parameter (optional) : (re)-declaration of additional methods.
    /// See below for example
#define ISOAREA( calcName, deltaRcode , additionalDecl )  struct calcName : public IsolationAreaBase { \
        calcName(double p) : IsolationAreaBase(p, #calcName){}          \
        virtual void setup(const xAOD::Jet* j)  {double jetRadius=j->getSizeParameter();  double param = m_parameter; m_deltaRmax2=deltaRcode ; m_deltaRmax2*=m_deltaRmax2; param=jetRadius*param;}  additionalDecl }



    ISOAREA( IsoKR , jetRadius*param, )  ;
    ISOAREA( IsoDelta, jetRadius+param, )  ;
    ISOAREA( IsoFixedCone, param, )  ; 
    ISOAREA( IsoFixedArea, sqrt(jetRadius*jetRadius+param*M_1_PI) , )  ;

    // For Iso6To8 we need to redefine inIsolationArea
    ISOAREA( Iso6To8, 0.8 ,  bool inIsolationArea(const xAOD::Jet* j, jet::ParticlePosition& constit)const ;  )  ;
    bool Iso6To8::inIsolationArea(const xAOD::Jet* j, jet::ParticlePosition& constit) const {
      double dr2 = JetDistances::deltaR2(j->eta(), j->phi(), constit.x(), constit.y());
      return ( (dr2<0.8*0.8) && (dr2>0.6*0.6) );
    }

 
    IsolationCalculator *createCalulator(string n, double parameter){

      if( n == "IsoKR" )        return new  IsolationCalculatorT<IsoKR>( parameter);      
      if( n == "IsoDelta" )     return new  IsolationCalculatorT<IsoDelta>( parameter);      
      if( n == "IsoFixedCone" ) return new  IsolationCalculatorT<IsoFixedCone>( parameter);      
      if( n == "IsoFixedArea" ) return new  IsolationCalculatorT<IsoFixedArea>( parameter);      
      if( n == "Iso6To8" )      return new  IsolationCalculatorT<Iso6To8>( parameter);

      return NULL;
    }


  } // namespace JetIsolation

  /// helper 
  void colonSplit(const string &in, string &s1, string &s2, string &s3){
    s2=""; s3="";
    std::stringstream str(in);
    std::getline(str, s1, ':' );
    std::getline(str, s2, ':');
    std::getline(str, s3);
  }


  bool isConstituent(const xAOD::IParticle* p,const xAOD::Jet* j){
    // just need raw constituents.
    for(size_t i = 0;i<j->numConstituents();i++){
      if( p == j->rawConstituent(i) ) return true;
    }
    return false;
  }

} // namespace jet 

using namespace jet::JetIsolation;

//**********************************************************************

JetIsolationTool::JetIsolationTool(const string& name) 
: JetModifierBase(name), m_hpjg("") {
  declareProperty( "IsolationCalculations", m_isolationCodes);
  declareProperty("PseudoJetGetter", m_hpjg);
}

//**********************************************************************

JetIsolationTool::~JetIsolationTool() { }

//**********************************************************************

StatusCode JetIsolationTool::initialize() {
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // a temporary map of isolation calculator
  std::map<string, IsolationCalculator*> calcMap;

  size_t nmom = m_isolationCodes.size();
  // decode each isolation calculations as entered by properties
  for ( size_t i=0; i<nmom; ++i ) {
    string isocriteria, param_s, kinematic;
    jet::colonSplit(m_isolationCodes[i], isocriteria, param_s, kinematic);
    string calcId = isocriteria + param_s;
    IsolationCalculator*& isoC = calcMap[calcId];
    if ( isoC == nullptr ) {
      isoC = createCalulator( isocriteria, std::stod(param_s)*0.1 );
    }
    if( isoC == NULL ) {
      ATH_MSG_ERROR(" Unkown isolation criteria "<< isocriteria << "  from "<< m_isolationCodes[i] );
      return StatusCode::FAILURE;
    }
    bool ok = isoC->scheduleKinematicCalculation( kinematic);
    if(!ok) {
      ATH_MSG_ERROR(" Unkown isolation kinematic "<< kinematic << "  from "<< m_isolationCodes[i] );
      return StatusCode::FAILURE;
    }
  }

  // Fill the iso calculator vector from the map
  for ( auto& pair : calcMap ){ 
    m_isoCalculators.push_back(pair.second); 
    ATH_MSG_DEBUG("Will use iso calculation : "<< pair.second->baseName() );
    pair.second->dump();
  }

  ATH_MSG_INFO("Initialized JetIsolationTool " << name());
  if ( m_hpjg.empty() ) {
    ATH_MSG_INFO("  Pseudojet getter will found using JetInput");
  } else {
    ATH_MSG_INFO("  Pseudojet getter: " << m_hpjg->name());
  }
  ATH_MSG_INFO("  Isolation calculations: " << m_isolationCodes);

  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetIsolationTool::modify(xAOD::JetContainer& jets) const {

  ATH_MSG_DEBUG("Modifying jets in container with size " << jets.size());
  if ( jets.empty() ) return 0;

  // Find the pseudojet getter.
  // If one is not supplied, the InputType field for the first jet is used
  // to locate the appropriate tool.
  const IPseudoJetGetter* ppjg = nullptr;
  int iinp = -1;
  if ( m_hpjg.empty() ) {
    iinp = jets[0]->getAttribute<int>("InputType");
    auto iiinp = static_cast<xAOD::JetInput::Type>(iinp);
    string sinp = xAOD::JetInput::typeName(iiinp);
    ATH_MSG_DEBUG("Jet input type is " << sinp);
    ppjg = PseudoJetGetterRegistry::find(sinp);
    if ( ppjg == nullptr ) {
      ATH_MSG_WARNING("Unable to retrieve pseudojet getter for input index/name "
                      << iinp << "/" << sinp);
      return 1;
    }
    ATH_MSG_DEBUG("Found pseudojet getter " << ppjg->name());
  } else {
    ppjg = &*m_hpjg;
  }
  if ( ppjg == nullptr ) {
    ATH_MSG_WARNING("Unable to retrieve pseudojet getter.");
    return 1;
  }
    
  // Fetch the input pseudojets.
  const PseudoJetVector* inputConstits = ppjg->get();
  ATH_MSG_DEBUG("Retrieved input count is " << inputConstits->size());

  // adapt the calculators to these jets (radius, input type, etc...)
  // Since we're in a const method, we must create a copy of the calculators we own.
  // We use the 1st jet, assuming all others in the container are similar.
  std::vector<IsolationCalculator*> calculators; // the adapted calculators.
  for( const IsolationCalculator * calc : m_isoCalculators ){
    IsolationCalculator * cloned = calc->clone( jets[0] );
    calculators.push_back( cloned );
  }

  // Loop over jets in this collection.
  for ( xAOD::Jet* pjet : jets ) {

    // Check this jet has the same inputs.
    int jinp = pjet->getAttribute<int>("InputType");
    if ( m_hpjg.empty() && jinp != iinp ) {
      ATH_MSG_WARNING("Jets have inconsistent inputs: " << iinp << " and " << jinp);
      continue;
    }

    // Create jet position.
    jet::ParticlePosition jetPos(pjet);
  
    // restrict to nearby particles which are NOT jet constituents
    //  (for the expected num of constituents per jet (~30), doing a naive loop is
    //    faster than using set objects)
    std::vector<jet::ParticlePosition> nearbyC;
    nearbyC.reserve( 30 ); // educated guess.

    ATH_MSG_VERBOSE("Jet eta=" << jetPos.x() << ", phi=" << jetPos.y());
    for ( unsigned int ippj=0; ippj<inputConstits->size(); ++ippj ) {
      const PseudoJet* ppj = &(inputConstits->at(ippj));
      const xAOD::IParticle* ppar = nullptr;
      string label = "none";
      if ( ppj->has_user_info<jet::IConstituentUserInfo>() ) {
        const auto& uin = ppj->user_info<jet::IConstituentUserInfo>();
        ppar = uin.particle();
        label = uin.label();
      } else {
        ATH_MSG_WARNING("Pseudojet does not have the expected user info.");
      }
      jet::ParticlePosition pos(ppj);
      string msg = "Skipping";
      bool found = false;
      if ( ppar != nullptr ) {
        for ( unsigned int icon=0; icon<pjet->numConstituents(); ++icon ) {
          if ( ippj == 0 ) ATH_MSG_VERBOSE("  Jet con: " << long(pjet->rawConstituent(icon)));
          if ( pjet->rawConstituent(icon) == ppar ) {
            found = true;
            break;
          }
        }
      }
      if ( ! found ) {
        msg = "Keeping";
        nearbyC.push_back(pos); // the constituent does not belong to jet
      }
      ATH_MSG_VERBOSE("  " << msg << " eta=" << pos.x() << ", phi=" << pos.y()
                      << " @" << long(ppar) << " " << label);
    }
    if ( nearbyC.size() + pjet->numConstituents() != inputConstits->size() ) {
      ATH_MSG_WARNING("Inconsistent number of jet constituents found in jet.");
    }
    
    ATH_MSG_DEBUG( "  # outside jet: "
                   << nearbyC.size() << ", in jet: "<< pjet->numConstituents()
                   << ", total: "<< inputConstits->size() );

    // loop over calculators, calculate isolation given the close-by particles not part of the jet.
    for ( IsolationCalculator* calc : calculators ){
      calc->setIsolationAttributes(pjet, nearbyC);
    }
    
  }

  // clear calculators :
  for ( IsolationCalculator* calc : calculators ) {
    delete calc;
  }

  ATH_MSG_DEBUG("done");
  return 0;
}

//**********************************************************************

StatusCode JetIsolationTool::finalize() {
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");
  for( size_t i=0;i<m_isoCalculators.size(); i++){
    delete m_isoCalculators[i];
  }
  return StatusCode::SUCCESS;
}

