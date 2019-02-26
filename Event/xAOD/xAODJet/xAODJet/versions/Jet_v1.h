// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_VERSIONS_JET_V1_H
#define XAODJET_VERSIONS_JET_V1_H

#include <map>


// Core include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#ifndef SIMULATIONBASE
#include "xAODBTagging/BTaggingContainer.h"
#endif //SIMULATIONBASE

// Local include(s):
//#include "xAODJet/JetConstituentIterator.h"
#include "xAODJet/JetConstituentVector.h"
#include "xAODJet/JetAttributes.h"
#include "xAODJet/JetTypes.h"
#include "xAODJet/JetContainerInfo.h"

namespace fastjet {
  class PseudoJet;
}

namespace xAOD {
  class FastJetLinkBase; 



  /// Class describing a jet
  ///
  /// Jet is a generic class used to represent any  "types" of hadronic jet within the Atlas Software.
  /// The various "types" of jets include for example jets build from calorimeter cluster or from reconstructed tracks or from truth particle, etc...
  /// The Jet class provides 4 main features :
  ///   - list of constituents. This list is build by the jet finding algorithm and is the physical definition of the jet object. Methods are provided to set and access constituents as IParticle objects.
  ///   - 4-momentum. Accessed through the inherited IParticle interface. A Jet 4-mom is not necessarily the sum of its constituents 4-mom (for ex. if a calibration has been applied).
  ///   - Signal States : Jets can hold several other 4-momentum representing various calibration states (primarily used for calo jets). See also P4SignalState
  ///   - Attributes : depending on its "type"  several other attributes can be added to a jet.
  ///      - numbers, vector of numbers : access through the get/setAttribute functions
  ///      - other IParticles : dedicated functions are provided get/setAssociatedParticles (because of internal I/O details)
  ///
  /// 
  /// @author Pierre-Antoine Delsart <delsart@in2p3.fr>
  /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  ///
  /// $Revision: 795692 $
  /// $Date: 2017-02-06 00:08:21 +0100 (Mon, 06 Feb 2017) $
  ///
  class Jet_v1 : public IParticle {

  public:
    
    typedef JetAttribute::AttributeID AttributeID;
    typedef JetAttribute::AssoParticlesID AssoParticlesID;
    //    typedef P4SignalState::State SignalStateType;

    /// Default constructor
    Jet_v1();
    /// Constructor with the 4-momentum properties of the jet
    //Jet_v1( float pt, float eta, float phi, float m );
    /// Copy constructor
    Jet_v1(const Jet_v1& );

    ~Jet_v1();
    /// @name xAOD::IParticle functions
    /// @{

    Jet_v1& operator=(const Jet_v1& other);

    /// The transverse momentum (\f$p_T\f$) of the particle
    virtual double           pt() const;
    /// The pseudorapidity (\f$\eta\f$) of the particle
    virtual double           eta() const;
    /// The azimuthal angle (\f$\phi\f$) of the particle
    virtual double           phi() const;
    /// The invariant mass of the particle
    virtual double           m() const;
    /// The total energy of the particle
    virtual double           e() const;
    /// The true rapidity (y) of the particle
    virtual double           rapidity() const;

    /// Definition of the 4-momentum type
    typedef IParticle::FourMom_t FourMom_t;

    /// The full 4-momentum of the particle
    virtual FourMom_t p4() const;


    /// The type of the object as a simple enumeration
    virtual Type::ObjectType type() const;




    ////////////////////////////////////////////////////////    
    /// @name Additionnal kinematics for jets.
    /// @{
    ///    

    /// define enum ot follow naming conventions
    typedef JetFourMom_t GenVecFourMom_t;


    /// The full 4-momentum of the particle : internal jet type.
    JetFourMom_t jetP4() const;
    /// The full 4-momentum of the particle : internal jet type (same).
    JetFourMom_t genvecP4() const;

    /// The x-component of the jet's momentum
    float px() const;
    /// The y-component of the jet's momentum
    float py() const;
    /// The z-component of the jet's momentum
    float pz() const;

    /// @}


    ////////////////////////////////////////////////////////    
    /// @name Access signal state
    /// @{
    ///    
    /// access the various signal states of the jets 
    /// For now provide the minimum by returning only FourMomentum.
    /// Access by the enum to the historical states
    JetFourMom_t jetP4(JetScale s) const ;
    JetFourMom_t genvecP4(JetScale s) const ;
    /// Generic access to states
    JetFourMom_t jetP4(const std::string& statename) const ;
    JetFourMom_t genvecP4(const std::string& statename) const ;

    void setJetP4(const JetFourMom_t & p4);
    void setJetP4(JetScale s, const JetFourMom_t & p4);

    void setJetP4(const std::string & sname, const JetFourMom_t & p4);

    //Jet_v1* cloneAtState(const std::string& state="") const ;
    /// @}
    ////////////////////////////////////////////////////////    




    //////////////////////////////////////////////////////// 
    //               Constituents 
    /// @name Jet constituents access 
    ///  
    /// Access to the jet constituents. 
    /// Constiuents are stored internally as IParticle.
    /// getConstituents() returns a special object : JetConstituentVector. The default 4-vectors of the constituents (ex: clusters) 
    /// possibly differ from the 4-vector used in the jet finding procedure (ex: when find jets with non calibrated "EM scale" clusters).
    /// The JetConstituentVector behaves almost exactly as a simple vector<const IParticle*> AND guarantees that the 4-vectors it 
    /// contains correpond to the 4-vector used at jet finding time.
    /// See the JetConstituentVector.h header for more details
    /// 
    /// A direct access method rawConstituent() is provided for experts usage only.
    /// jet finding.
    /// @{
  public:
    /// Add a constituent directly in the ElementLink format
    void addConstituent( const ElementLink< IParticleContainer >& link , float weight=1.0 );
    
    /// Add an IParticle as as constituent.
    /// Warning ! A link to the *original* container of this particle p will be used internally. 
    /// Thus one can not persistify correctly constituents from a container in VIEW mode.
    void addConstituent( const IParticle* p, float weight=1.0);
    
    /// The state at which constituents were when this jet was found
    JetConstitScale getConstituentsSignalState() const ;
    /// Set the state at which constituents were when this jet was found. This function is called by jet builders.
    void setConstituentsSignalState( JetConstitScale t);

    
    /// Return a vector of consituents. The object behaves like vector<const IParticle*>. See JetConstituentVector.h header for more details
    JetConstituentVector  getConstituents() const ;
    
    /// Number of constituents in this jets (this is valid even when reading a file where the constituents have been removed). 
    size_t numConstituents() const;

    /// Direct access to constituents. *WARNING* expert use only.
    const IParticle* rawConstituent(size_t i) const ;

    /// Direct access to constituents. *WARNING* expert use only
    const std::vector< ElementLink< IParticleContainer > >& constituentLinks()const ; 


  protected:
    
    void setNumConstituents(size_t n);

    /// @}
    ////////////////////////////////////////////////////////
    
    


    ////////////////////////////////////////////////
    /// @name Access to auxiliary data
    /// 
    /// Functions to retrieve attributes added to jet objects.
    /// They concern any attributes (float, vector<float>, 4-momentum,...) except IParticles (for the later see associatedParticles() function below).
    /// Access is done by an enum identifier or by string.
    /// They internally translate calls using double or vector<double> (ex: getAttribute<double>() ) in calls  using float. 
    /// - This guarantees values are saved as float (less disk space)
    /// - This avoids potential confusions related to retrieving a number as a float or as a double.
    /// @{
  public:

    /// Retrieve attribute moment by enum
    template<class T>
    bool getAttribute( AttributeID type, T & value ) const ;
    /// Retrieve attribute by string
    template<class T>
    bool getAttribute(const std::string &name, T &value) const;

    /// short form : more convenient and compact. Throws an exception if the moment is missing.
    template<class T>
    T getAttribute(const std::string &name) const ;
    template<class T>
    T getAttribute(AttributeID type) const ;
    


    template<class T>
    void setAttribute(const std::string &name, const T& v) ;
    /// Set  number by enum
    template<class T>
    void setAttribute( AttributeID type, const T& value );
#ifndef SIMULATIONBASE
    /// Access to the associated btagging object
    const BTagging* btagging() const;
    /// Access to the associated btagging object
    const ElementLink< BTaggingContainer >& btaggingLink() const;
    /// Access to btagging objects
    void setBTaggingLink( const ElementLink< BTaggingContainer>& el );
#endif

    /// @}
    ////////////////////////////////////////////////////////    
    /// @name Access object or list of associated objects
    /// @{
    ///
    /// Special functions to store list of objects are provided.
    /// The reason is we want to internally store every object as link (ElementLink).
    /// The setter and getter functions thus perform the conversion automatically from the 
    ///  internal format from/to const Object* or vector<const Object*>
    


    /// get associated objects as a vector<object>
    /// this compact form throws an exception if the object is not existing
    /// If internal links are invalid (ex: because of thinning) they replaced by a null pointer
    template<typename T>
    std::vector<const T*> getAssociatedObjects( const std::string &name) const ;
    template<typename T>
    std::vector<const T*> getAssociatedObjects( AssoParticlesID type) const ;

    /// get associated objects as a vector<object>
    /// returns false if the object is not existing
    /// If internal links are invalid (ex: because of thinning) they replaced by a null pointer
    /// AND the function returns false
    template<typename T>
    bool getAssociatedObjects( const std::string &name, std::vector<const T*>& vec) const ;
    template<typename T>
    bool getAssociatedObjects( AssoParticlesID type, std::vector<const T*>& vec) const ;

    /// set associated objects from a vector of arbitrary object.
    /// T must inherit AuxElement (if T inherits IParticle, objects are stored as IParticle but 
    /// can still be retrieved as their exact type)
    /// returns false if the association failed
    template<typename T>
    void setAssociatedObjects( const std::string &name, const std::vector<const T*>& vec) ;
    template<typename T>
    void setAssociatedObjects( AssoParticlesID type, const std::vector<const T*>& vec)  ;


    /// get a single associated object 
    /// this compact form throws an exception if the object is not existing
    template<typename T>
    const T* getAssociatedObject( const std::string &name) const ;
    template<typename T>
    const T* getAssociatedObject( AssoParticlesID type) const ;

    /// get a single associated object
    /// returns false if the object is not existing
    template<typename T>
    bool getAssociatedObject( const std::string &name, const T*& vec) const ;
    template<typename T>
    bool getAssociatedObject( AssoParticlesID type, const T*& vec) const ;

    /// set a single associated object
    /// T must inherit AuxElement (if T inherits IParticle, objects are stored as IParticle but 
    /// can still be retrieved as their exact type)
    /// returns false if the association failed
    template<typename T>
    void setAssociatedObject( const std::string &name,  const T* vec) ;
    template<typename T>
    void setAssociatedObject( AssoParticlesID type, const T* vec)  ;

    /// @}
    ////////////////////////////////////////////////////////


    
    

    ////////////////////////////////////////////////////////    
    /// @name Metadata like information
    /// @{

    float getSizeParameter() const ;
    JetAlgorithmType::ID getAlgorithmType() const ;
    JetInput::Type getInputType() const ;

    void setSizeParameter(float p)  ;
    void setAlgorithmType(JetAlgorithmType::ID a)  ;
    void setInputType(JetInput::Type t)  ;

    /// @}
    ////////////////////////////////////////////////////////

    
    ////////////////////////////////////////////////////////    
    /// @name Access to internal fastjet object. Reserved to expert usage.
    /// @{
    /// Access to the internal fastjet object. CAN RETURN NULL, see the description of m_pseudoJet
    const fastjet::PseudoJet * getPseudoJet() const ;
    /// Set the fast jet pointer. Expert usage only. This function is templated in order to avoid 
    ///  a dependency of xAODJet on fastjet.
    template<class PSEUDOJET>
    void setPseudoJet(const PSEUDOJET * fj);
    
    
    /// Function making sure that the object is ready for persistification
    // void toPersistent();

    /// workaround some I/O limitations
    void reset();
    /// @}
    ////////////////////////////////////////////////////////

  protected:
    /// Pointer to the fastjet object this jet is build from. WARNING : this pointer is meant to be used in reco flow only.
    ///  It is transient only,  provided only to ease calculation of some jet quantities, and will be invalid for jets read from files.
    FastJetLinkBase * m_fastJetLink;


  }; // class Jet


} // namespace xAOD

#include "xAODJet/versions/Jet_v1.icc"

#endif // XAODJET_VERSIONS_JET_V1_H
