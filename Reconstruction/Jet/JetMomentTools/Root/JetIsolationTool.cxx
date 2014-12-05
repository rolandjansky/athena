///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetIsolationTool.cxx 
// Implementation file for class JetIsolationTool
/////////////////////////////////////////////////////////////////// 

// JetMomentTools includes
#include "JetMomentTools/JetIsolationTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "JetUtils/JetDistances.h"
#include <sstream>

/////////////////////////////////////////////////////////////////// 
// Define some isolation calculators.  
/////////////////////////////////////////////////////////////////// 
namespace jet {

  //////////////////////////////////////////////////////
  /// \namespace JetIsolation
  /// \brief Helper classes to calculate various isolation quantities
  ///
  /// There are many ways of calculating isolation variables. 3 aspect can be considered :
  ///  1) The isolation zone (i.e which constituents not part of the jets are considered). In most of the cases this is simply a cone which radius depends on the jet.
  ///  2) The kinematics computed from the contistuents in the isolation zone (sum pt, full P, ...)
  ///  3) The signal sate at which constituents have to be chosen (i.e. LC scale or EM scale in the cluster case).
  ///
  ///  To avoid to copy-paste similar code for the many possible combinations,
  ///  these 3 aspects are encapsulated in different classes :
  ///   1) IsolationAreaBase and derived classes
  ///   2) IsolationResult : which holds the 4-vector and has helpers to return the desired variables.
  ///   3) KinematicGetter classes which hides the actual access to kinematics of constituents
  ///
  /// All these pieces are then combined in a IsolationCalculatorT<IsolationAreaBase, KinematicGetter> class (inheriting IsolationCalculator)
  /// which is able to build the IsolationResult for the concrete IsolationAreaBase and KinematicGetter it receives as template paramters.
  ///
  /// Finally the JetIsolationTool instantiates as many IsolationCalculator as requested from its jobOption property.
  ///

  namespace JetIsolation {

    typedef xAOD::IParticle::FourMom_t FourMom_t;

    /// IsolationCalculator : base class for isolation calculations
    class IsolationCalculator {
    public:

      /// Define the availble isolation variables
      enum Kinematics {
        SumPt, Par, Perp, P
      };

      /// names for isolation variables
      static const std::string s_kname[4];

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

      
      virtual std::string baseName() const {return "";}
      virtual IsolationCalculator * clone(const xAOD::Jet* ) const {return NULL;}
      virtual void copyFrom( const IsolationCalculator* o, const xAOD::Jet*){
        m_attNames = o->m_attNames;
        m_kinematics = o->m_kinematics;
      }
      
        
      /// compute the isolation momentum from jet and the constituents in nearbyConstit (these ones are supposed to be outside the jet).
      virtual IsolationResult jetIsolation(const xAOD::Jet* , std::vector<jet::ParticlePosition> & ) const {return IsolationResult();}
      virtual void setIsolationAttributes(xAOD::Jet* jet, std::vector<jet::ParticlePosition> & nearbyConstit) const {

        IsolationResult result = jetIsolation(jet,nearbyConstit);
        for(size_t i = 0; i< m_kinematics.size(); i++){
          switch( m_kinematics[i]) {
          case Perp: jet->setAttribute<float>( m_attNames[i], result.isoPerp(jet->p4().Vect()) ); break;
          case SumPt: jet->setAttribute<float>( m_attNames[i], result.isoSumPt() ); break;
          case Par: jet->setAttribute<float>( m_attNames[i], result.isoPar(jet->p4().Vect()) ); break;
          case P: jet->setAttribute<float>( m_attNames[i], result.isoP().Vect().Mag() ); break;
          }
        }

      }
      

      
      bool scheduleKinematicCalculation(std::string kname){
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
      std::vector< Kinematics > m_kinematics; /// kinematics isolation vars to be computed
      std::vector< std::string > m_attNames;  /// kinematics isolation vars names
      //std::string m_baseName;    
    };

  const std::string IsolationCalculator::s_kname[4] =  {"SumPt", "Par", "Perp", "P"};

    
    struct IParticleKinematicGetter {
      const FourMom_t& getP(jet::ParticlePosition& p) const { return p.particle()->p4();}
      double getPt(jet::ParticlePosition& p) const { return p.particle()->pt();}
    };

    struct EMClusterKinematicGetter {
      const FourMom_t& getP(jet::ParticlePosition& p) const { 
        const xAOD::CaloCluster * cl = static_cast<const xAOD::CaloCluster*>(p.particle() );
        return cl->p4(xAOD::CaloCluster::UNCALIBRATED);
      }
      double getPt(jet::ParticlePosition& p) const { 
        const xAOD::CaloCluster * cl = static_cast<const xAOD::CaloCluster*>(p.particle() );        
        return cl->p4(xAOD::CaloCluster::UNCALIBRATED).Pt();
      }
    };

    
    template<typename ISOCRITERIA, typename KINEMATICGETTER = IParticleKinematicGetter>
    class IsolationCalculatorT : public IsolationCalculator {
    public:

      IsolationCalculatorT(double param=0.) : m_iso(param) { }

      virtual IsolationResult jetIsolation(const xAOD::Jet* jet, std::vector<jet::ParticlePosition> &nearbyConstit) const {
        IsolationResult result;
        result.m_isoSumPt = 0;
        int contributed = 0;
        for(size_t i=0 ; i<nearbyConstit.size(); i++){
          if( m_iso.inIsolationArea(jet, nearbyConstit[i]) ) {
            result.m_isoP +=    m_kine.getP( nearbyConstit[i] ) ;
            result.m_isoSumPt += m_kine.getPt(nearbyConstit[i]);
            contributed++;
          }    
        }
        //std::cout << "  "<< momentName()<< " contributed : "<< contributed<< "  tot :"<< nearbyConstit.size() << " deltaRmax "<< m_deltaRmax  << std::endl;
        return result;
      }
      
      virtual std::string baseName() const {return m_iso.name();}

      
      
      virtual IsolationCalculator * clone(const xAOD::Jet* j) const {
        IsolationCalculator* iso;
        if( j->getInputType() == xAOD::JetInput::EMTopo) {
          auto* isoT = new IsolationCalculatorT<ISOCRITERIA, EMClusterKinematicGetter >();
          isoT->m_iso = m_iso;
          isoT->m_iso.setup(j);
          iso = isoT;
        } else  {
          auto* isoT= new IsolationCalculatorT();
          isoT->m_iso = m_iso;
          isoT->m_iso.setup(j);
          isoT->m_kine = m_kine;
          iso = isoT;
        }
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
      IsolationAreaBase(double p, const std::string &n) : m_parameter(p), m_name(n){}
      
      bool inIsolationArea(const xAOD::Jet* j, jet::ParticlePosition& part) const {

        double dr2 = JetDistances::deltaR(j->eta(),j->phi(), part.x(), part.y());
        return dr2< m_deltaRmax2;
      }
      
      std::string name() const {
        std::ostringstream oss; oss << m_name << int(10*m_parameter);
        return oss.str();        
      }

      double m_parameter;
      std::string m_name;
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
    ISOAREA( IsoFixedArea, sqrt(jetRadius*jetRadius+param/M_PI) , )  ;

    // For Iso6To8 we need to redefine inIsolationArea
    ISOAREA( Iso6To8, 0.8 ,  bool inIsolationArea(const xAOD::Jet* j, jet::ParticlePosition& constit)const ;  )  ;
    bool Iso6To8::inIsolationArea(const xAOD::Jet* j, jet::ParticlePosition& constit) const {
      double dr2 = JetDistances::deltaR2(j->eta(), j->phi(), constit.x(), constit.y());
      return ( (dr2<0.8*0.8) && (dr2>0.6*0.6) );
    }

 
    IsolationCalculator *createCalulator(std::string n, double parameter){

      if( n == "IsoKR" )        return new  IsolationCalculatorT<IsoKR>( parameter);      
      if( n == "IsoDelta" )     return new  IsolationCalculatorT<IsoDelta>( parameter);      
      if( n == "IsoFixedCone" ) return new  IsolationCalculatorT<IsoFixedCone>( parameter);      
      if( n == "IsoFixedArea" ) return new  IsolationCalculatorT<IsoFixedArea>( parameter);      
      if( n == "Iso6To8" )      return new  IsolationCalculatorT<Iso6To8>( parameter);

      return NULL;
    }


  } // namespace JetIsolation

/// helper 
  void colonSplit(const std::string &in, std::string &s1, std::string &s2, std::string &s3){
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



// Constructors
////////////////
JetIsolationTool::JetIsolationTool( const std::string& name) 
  : JetModifierBase(name),
    m_deltaRmax(1.2),
    m_rejectNegE(true)
{

  //
  // Property declaration
  // 

  declareProperty( "IsolationCalculations", m_isolationCodes);

  declareProperty( "ConstituentContainer", m_inputPseudoJets = "",  "The Constituents from which isolation is computed (will be retrieve automatically if blank)" );
  
//  declareProperty("RejectNegE", m_rejectNegE= true, "Ignore cluster with E<0");

  
  
}

// Destructor
///////////////
JetIsolationTool::~JetIsolationTool()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode JetIsolationTool::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  // a temporary map of isolation calculator
  std::map<std::string, IsolationCalculator*> calcMap;

  size_t nmom = m_isolationCodes.size();
  /// decode each isolation calculations as entered by properties
  for(size_t i=0;i <nmom; i++){
    std::string isocriteria, param_s, kinematic;
    jet::colonSplit( m_isolationCodes[i], isocriteria, param_s, kinematic);
    std::string calcId = isocriteria+param_s;
    IsolationCalculator* & isoC= calcMap[calcId];
    if( isoC == NULL) {
      isoC = createCalulator( isocriteria, std::stod( param_s)/10. );
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

  /// Fill the iso calculator vector from the map
  for( auto & pair : calcMap ){ 
    m_isoCalculators.push_back( pair.second ); 
    ATH_MSG_DEBUG("Will use iso calculation : "<< pair.second->baseName() );
    pair.second->dump();
  }


  return StatusCode::SUCCESS;
}


int JetIsolationTool::modify(xAOD::JetContainer& jets) const {

  ATH_MSG_DEBUG("modify container...");
  if(jets.empty() ) return 0;

  const jet::ParticleFastMap * inputMap = retrieveInputMap();
  if(inputMap == NULL) {
    ATH_MSG_ERROR("Can't retrieve constituents container. No isolation calculations");
    return 1;
  }

  ATH_MSG_DEBUG("modify : retrieved map");

  // adapt the calculators to these jets (radius, input type, etc...)
  // Since we're in a const method, we must create a copy of the calculators we own.
  // We use the 1st jet, assuming all others in the container are similar.
  std::vector<IsolationCalculator*> calculators; // the adapted calculators.
  for( const IsolationCalculator * calc : m_isoCalculators ){
    IsolationCalculator * cloned = calc->clone( jets[0] );
    calculators.push_back( cloned );
  }

  // Loop over jets in this collection.
  for( xAOD::Jet* jet: jets ){

    jet::ParticlePosition jetPos(jet);
    std::vector<jet::ParticlePosition> all_nearbyC = inputMap->pointsInDr( jetPos , m_deltaRmax);
  
    // restrict to nearby particles which are NOT jet constituents
    //  (for the expected num of constituents per jet (~30), doing a naive loop is
    //    faster than using set objects)
    std::vector<jet::ParticlePosition> nearbyC;
    nearbyC.reserve( 30 ); // educated guess.

    for( size_t i=0;i<all_nearbyC.size(); i++){
      jet::ParticlePosition &c= all_nearbyC[i];
      if( jet::isConstituent( c.particle(), jet ) ) continue;
      nearbyC.push_back( c ); // the constituent does not belong to jet
    }
    
    ATH_MSG_DEBUG( " Found nearbyC constit. All : "<< all_nearbyC.size() << "  nearbyC :"<< nearbyC.size() << " in jet "<< jet->numConstituents() << "  total "<< inputMap->size() );

    // loop over calculators, calculate isolation given the close-by particles not part of the jet.
    for( IsolationCalculator * calc : calculators ){
      calc->setIsolationAttributes( jet, nearbyC );
    }
    
  }

  // clear calculators :
  for( IsolationCalculator * calc : calculators ){
    delete calc;
  }

  ATH_MSG_DEBUG("done");
  return 0;
}


const jet::ParticleFastMap * JetIsolationTool::retrieveInputMap() const {

  ATH_MSG_DEBUG("retrieveInputMap ");

  // Check if the map is in SG
  std::string mapName = m_inputPseudoJets+"FastMap";
  if( evtStore()->contains<jet::ParticleFastMap>(mapName) ){
    const jet::ParticleFastMap * inputMap = 0;  
    StatusCode sc = evtStore()->retrieve( inputMap, mapName) ;
    if(sc.isFailure()) return NULL; // should never happen
    return inputMap;
  }

  ATH_MSG_DEBUG("Building the map ! ");

  // else build the map !
  jet::ParticleFastMap * inputMap = new jet::ParticleFastMap();
  inputMap->init(m_deltaRmax);
  // For now very simple and limited.
  const xAOD::IParticleContainer* inputConstits=0;
  StatusCode sc= evtStore()->retrieve( inputConstits, m_inputPseudoJets) ;
  if(sc.isFailure()) {
    ATH_MSG_ERROR("No input constituents "<< m_inputPseudoJets);
    return NULL;
  }
  
  for( const xAOD::IParticle* p : *inputConstits ){
    if( m_rejectNegE && ( p->e()<0) ) continue;
    jet::ParticlePosition pos( p );
    inputMap->insert( pos );
  }

  ATH_MSG_DEBUG("Built  map size= "<< inputMap->size());

  sc= evtStore()->record( inputMap, mapName) ;
  if(sc.isFailure()) { ATH_MSG_WARNING( "Can't record fatmap "<< mapName); }
  return inputMap;
}





StatusCode JetIsolationTool::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "...");
  for( size_t i=0;i<m_isoCalculators.size(); i++){
    delete m_isoCalculators[i];
  }
  return StatusCode::SUCCESS;
}

