// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <limits>

#include <TPRegexp.h>
#include "TLeaf.h"

//#include "JetAnalysisEDM/ParticleContainer.h"
#include "JetAnalysisEDM/Particle.h"

#include "JetAnalysisEDM/Particle.icc"
#include "JetAnalysisEDM/VarHandleMap.icc"


ClassImp( JetAnalysisEDM::ParticleContainer )

namespace JetAnalysisEDM {
  const ::Long64_t s_ParticleContainer_dummy_master ( (::Long64_t) -1 );

  ParticleContainer::ParticleContainer(   )
      : TObject()
      , m_attributes(this)
      , fFromInput( kFALSE )
      , m_prefix( "" )
      , m_master(&s_ParticleContainer_dummy_master)
      , m_currentEntry( (::Long64_t) -1 )
      , m_inputTree(NULL)
      , m_outputTree(NULL)
      , m_sourceContainer(NULL)
      , m_inputUsesMass(false)
      , m_forcedMass(-1.)
      , m_fastjetClustSeq(NULL)
  {}

   /**
    * This constructor should be used when the object will be used to read
    * variables from an existing ntuple. The object will also be able to
    * output variables, but it will also need to read them from somewhere.
    *
    * @param master Reference to the variable holding the current event number
    * @param prefix Prefix of the variables in the D3PD
    * @param tree  tree to read from. If Null, don't assign the input tree yet. It can be done later.
    */
  ParticleContainer::ParticleContainer( const string_t &prefix, const ::Long64_t& master, TTree *tree  )
      : TObject()
      , m_attributes(this)
      , fFromInput( kFALSE )
      , m_prefix( prefix )
      , m_master(NULL)
      , m_currentEntry( (::Long64_t) -1 )
      , m_inputTree(NULL)
      , m_outputTree(NULL)
      , m_sourceContainer(NULL)
      , m_inputUsesMass(false)
      , m_forcedMass(-1.)
      , m_fastjetClustSeq(NULL)
   {


     setParameters( prefix, &master);
     if(tree) readFrom(tree);
   }

   /**
    * This constructor can be used when the object will only have to output
    * (and temporarily store) new information into an output ntuple. For
    * instance when one wants to create a selected/modified list of information.
    *
    * @param prefix Prefix of the variables in the D3PD
    */
   ParticleContainer::ParticleContainer( const string_t prefix )
      : TObject()
      , m_attributes(this)
      , fFromInput( kFALSE )
      , m_prefix( prefix )
      , m_master(NULL)
      , m_currentEntry( (::Long64_t) -1 )
      , m_inputTree(NULL)
      , m_outputTree(NULL)
      , m_sourceContainer(NULL)
      , m_inputUsesMass(false)
      , m_forcedMass(-1.)
      , m_fastjetClustSeq(NULL)
   {

     setParameters( prefix, 0);
   }

   /**
    * The destructor needs to delete all the allocated objects.
    */
   ParticleContainer::~ParticleContainer() {
     for(size_t i =0 ;i < fProxies.size() ; i++) delete fProxies[i];     
   }


  void ParticleContainer::setParameters(const string_t prefix, const ::Long64_t* master)const {

    setName(prefix);
    m_master = master;
    m_attributes.setParameters(m_prefix, master);
    eta.setName(m_prefix+"eta");pt.setName(m_prefix+"pt");phi.setName(m_prefix+"phi");E.setName(m_prefix+"E");
    eta.setMaster(master);pt.setMaster(master);phi.setMaster(master);E.setMaster(master);

  }
  
  void ParticleContainer::declareKinematicVar(const string_t& _pt, const string_t& _eta, const string_t& _phi, const string_t& _E  ) const {
    eta.setName(m_prefix+_eta);pt.setName(m_prefix+_pt);phi.setName(m_prefix+_phi);E.setName(m_prefix+_E);
  }


  /// get this container name
  string_t ParticleContainer::getName() const {
    size_t s = m_prefix.size();
    if(m_prefix[s-1]=='_') return m_prefix.substr(0,s-1);
    return m_prefix;
  }
  /// set this container name
  void ParticleContainer::setName(const string_t& name) const {
    size_t s = name.size();
    if(name[s-1]=='_') setPrefix(name);
    else setPrefix( name+"_");    
    
  }

   /**
    * @returns The branch name prefix used by the object
    */
  string_t ParticleContainer::getPrefix() const {    
    return m_prefix;
  }

   /**
    * @param prefix The prefix that should be used for the variables
    */
   void ParticleContainer::setPrefix( const string_t prefix ) const {
     m_prefix = prefix;
     m_attributes.setPrefix(prefix);
     eta.setName(prefix+"eta");pt.setName(prefix+"pt");phi.setName(prefix+"phi");E.setName(prefix+"E");

   }

  void ParticleContainer::setMaster(::Long64_t  * master){
    m_master = master;
    m_attributes.setParameters(m_prefix, master);    
    eta.setMaster(master);pt.setMaster(master);phi.setMaster(master);E.setMaster(master);
  }
   /**
    * This function should be called every time a new TFile is opened
    * by your analysis code.
    *
    * @param tree Pointer to the TTree with the variables
    */
  bool ParticleContainer::readFrom( TTree* tree, bool quietMode ) const {
     fFromInput  = kTRUE;
     m_inputTree = tree;
     m_sourceContainer = this;
     
     ::TPRegexp re( m_prefix+"*" );
     
     TObjArray & b = *m_inputTree->GetListOfLeaves();
     int countprefix =0;
     for(int i=0; i< b.GetEntries(); i++){
       TLeaf *br = (TLeaf*) b[i];
       //string_t name = (const char*)
       TString name = br->GetName();
       if( name.BeginsWith(m_prefix) ){
         countprefix++;
         //std::cout << " readFrom  "<< name  << std::endl;
         if( ! re.Match( name ) ) continue;
         string_t var = (const char*) TString(name(m_prefix.size(), name.Length()));
         VarHandleBase * v = m_attributes.buildNewVarFromTypeName(var, br->GetTypeName() );
         //         std::cout << "    readFrom  "<< name << "  v="<< var << std::endl;
         if(v) v->readFrom(tree); // protect agains v==0 (for Int_t xx_n; variables)
       }
     }

     if(countprefix<3){
       if(quietMode) return false; 
       Error("readFrom", "No variables correponding to  %s  in TTree %s", m_prefix.c_str() , m_inputTree->GetName());
       if(!Session::isInteractive()) throw VarNotFound(string_t("ParticleContainer::readFrom can't retrieve ")+m_prefix.c_str());
       return false;
     }

     eta.readFrom(tree);pt.readFrom(tree);phi.readFrom(tree);E.readFrom(tree);
     if(m_forcedMass==-1.) { 
       // then no mass set by user, we absolutely need a 4th component : check it is valid
       if( ! tree->GetBranch( E.getName().c_str() ) ) {
         // try e,  m, M 
         if(  tree->GetBranch( (m_prefix+"e").c_str()) ) {
           E.setName(m_prefix+"e");
         }else if(  tree->GetBranch( (m_prefix+"m").c_str()) ) {
           E.setName(m_prefix+"m");
           m_inputUsesMass = true;
         }else if(  tree->GetBranch( (m_prefix+"M").c_str()) ) {
           E.setName(m_prefix+"M");
           m_inputUsesMass = true;
         } else {
           if(quietMode) return false;
           Error( "readFrom ", " Didn't find any energy/mass variable for container '%s' (tried   E, e, M, m) ... \n"
                  "   --> check spelling of branchname or use declareKinematicVar()/usePtEtaPhiM() followed by readFrom(tree)", m_prefix.c_str());
           if(!Session::isInteractive()) throw VarNotFound(string_t("ParticleContainer::readFrom can't retrieve ")+E.getName());
           return false;
         }
       }
     }     
     return true;
   }

   /**
    * This function can be called to connect the active variables of the object
    * to an output TTree. It can be called multiple times, then the variables
    * will be written to multiple TTrees.
    *
    * @param tree Pointer to the TTree where the variables should be written
    */
   void ParticleContainer::writeTo( TTree* tree ) const {

     // always write out the kinematics
     eta.writeTo(tree);pt.writeTo(tree);phi.writeTo(tree);E.writeTo(tree);
     // Call writeTo(...) on all the variables:
     m_attributes.writeTo(tree);
     m_outputTree = tree;

      return;
   }


   /**
    * This is a convenience function for turning the branches active or
    * inactive conveniently. If the parameter is set to <code>kTRUE</code>
    * then the branches available from the input which match the given
    * pattern are turned active.
    * When it's set to <code>kFALSE</code> then all the variables matching
    * the pattern are turned inactive.
    *
    * @param active Flag behaving as explained above
    * @param pattern Regular expression specifying which branches to modify
    */
   void ParticleContainer::setOutputVar( ::Bool_t active, string_t pattern ) const {
     
     m_attributes.setOutputVar( active, pattern);

     return;
   }

   /**
    * This function can be used to read in all the branches from the input
    * TTree which are set active for writing out. This can simplify writing
    * event selector codes immensely. Remember to set the desired variable
    * active before calling this function.
    */
   void ParticleContainer::readAllActive() const {

      // Check if it makes sense to call this function:
      if( ! fFromInput ) {
         static ::Bool_t wPrinted = kFALSE;
         if( ! wPrinted ) {
            Warning( "readAllActive", "Function only meaningful when used on objects" );
            Warning( "readAllActive", "which are used to read information from a D3PD" );
            wPrinted = kTRUE;
         }
      }
      eta();pt();phi();E();
      m_attributes.readActive();
      return;
   }


   /**
    * This function makes it easier to clear out the object completely.
    * It cleares all the vector variables, and sets the element number
    * variable to 0. Very useful when performing object selection.
    * The option argument is not used at the moment for anything.
    * It's only there because the <code>clear</code> function defined in
    * TObject has this parameter as well.
    *
    * @param option Ignored at the moment
    */
   void ParticleContainer::clear( Option_t* ) {

      // Check if this function can be used on the object:
      if( fFromInput ) {
         Error( "clear", "Objects used for reading a D3PD can't be cleared!" );
         return;
      }

      for(size_t i =0 ;i < fProxies.size() ; i++) delete fProxies[i];  
      fProxies.clear();
      if(m_outputTree) {
        // if no outputTree, the variable below are not used. No need to update
        eta.clear();pt.clear();phi.clear();E.clear();
      }
      m_attributes.clear();
      return;
   }

   /**
    * This function can be used to easily add an 'element' describing one
    * object to an output collection. Comes in very handy when performing
    * object selection.
    *
    * Note that variables which are not available from the input, will be
    * filled with dummy values.
    *
    * @param el The 'element' that should be added to the collection
    */
  bool ParticleContainer::Add(  Particle* el ) {
    return push_back(el);
  }
  bool ParticleContainer::push_back(  Particle* el ) {
    
    // Check if this function can be used on the object:
    if( fFromInput ) {
      Error( "Add", "Objects used for reading a D3PD can't be modified!" );
      return false;
    }
    
    if( el->fContainer !=NULL) {
      Error("Add", "Can't add a particle belonging to container <%s> to <%s>", el->fContainer->getName().c_str(), getName().c_str());
      return false;
    }

    if( m_sourceContainer) {
      const ParticleContainer* oCont = el->getSourceContainer();
      if( oCont  != m_sourceContainer ) {
        string_t oprefix = "no container";
        if( oCont ) oprefix = oCont->getName();
        Error( "Add", "can't add particle from %s into %s", oprefix.c_str(), getName().c_str() );
        return false;
      }
    }
      
    i_add( el);
    m_attributes.copyFrom( el, el->getIndex());
        
    return true;
  }


  Particle* ParticleContainer::addNewFromSource(const Particle* source){
    if( fFromInput ) {
      Error( "addNewFromSource", "Objects used for reading a D3PD can't be modified!" );
        return NULL;
    }

    if( m_sourceContainer) {
      const ParticleContainer* oCont = source->getSourceContainer();
      if( oCont  != m_sourceContainer ) {
        string_t oprefix = "no container";
        if( oCont ) oprefix = oCont->getName();
        Error( "Add", "can't add particle from %s into %s", oprefix.c_str(), getName().c_str() );
        return NULL;
      }
    }
      
    Particle* p = new Particle(0,this);
    //std::cout << "    addNewFromSource  "<< m_master << "  m_master"<< *m_master << "  vs "<< m_currentEntry << std::endl;
    *p = *source; // copy 4 moments and set source
    i_add( p );      
    
    //std::cout << " addNewFromSource  "<< m_sourceContainer << "  fProxies: "<< fProxies.size()<< "  ind="<< p->getIndex() << std::endl;
    if(m_sourceContainer) m_attributes.copyFrom( m_sourceContainer->m_attributes, p->getSourceIndex(), p->getIndex());
    else m_attributes.copyFrom( p, p->getIndex());
    return p;
  }
  
  void ParticleContainer::i_add(Particle *p){
    
    // Check if this container is in sync with the global entry index...
    if( m_master && ( *m_master != m_currentEntry ) ) {
      // ... no, entry has changed. Clear the container and resync.
      m_attributes.buildVars(); // does something only on first call.
      clear(); m_currentEntry = *m_master;
    }

    if(m_outputTree) {
      eta().push_back( p->eta() );
      phi().push_back( p->phi() );
      E().push_back(   p->E() );
      pt().push_back(  p->pt() );
    }
    size_t newindex = fProxies.size();

    // part is our :
    p->fContainer = this;
    p->fIndex = newindex;
    fProxies.push_back( p);

  }



  void ParticleContainer::copyFrom(const ParticleContainer & source, std::vector<int> copyIndices){
    
    std::vector<float> & eta_v = eta();
    std::vector<float> & phi_v = phi();
    std::vector<float> & pt_v = pt();
    std::vector<float> & E_v = E();

    size_t prevSize= size();
    source.loadCurrentEvent();
    for(size_t i=0; i<copyIndices.size(); i++){
      Particle* p = new Particle(0,this);
      *p = *source[copyIndices[i]]; // copy 4 moments and set source
      if(m_outputTree) {
        eta_v.push_back( p->eta() );
        phi_v.push_back( p->phi() );
        E_v.push_back(   p->E() );
        pt_v.push_back(  p->pt() );
      }
      p->fIndex = prevSize+i;
      fProxies.push_back(p);
    }
    m_attributes.copyFrom(source.m_attributes, copyIndices);

  }



   void ParticleContainer::buildContainerFromInput() const {
     m_currentEntry = *m_master;

     const std::vector<float> &v_phi = phi();
     const std::vector<float> &v_pt  = pt();
     const std::vector<float> &v_eta = eta();

     size_t N = v_phi.size();
     //std::cout << "buildContainerFromInput "<<m_currentEntry<< " N="<< N << std::endl;
     if( fProxies.size() < N ) {
       size_t n = fProxies.size();
       fProxies.resize(N);
       for(; n<N;n++) fProxies[n]= new Particle(n,const_cast<ParticleContainer*>(this));
     }

     if(m_forcedMass>-1){
       typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > LorentzVectorM_t;
         
       for(size_t i=0;i<N;i++){

         Particle *p = fProxies[i];
         if(p->m_varMap) p->m_varMap->clear();
         LorentzVectorM_t vec( v_pt[i], v_eta[i], v_phi[i], m_forcedMass);   
         p->i_SetPt( v_pt[i] );
         p->i_SetEta(v_eta[i]);
         p->i_SetPhi(v_phi[i]);
         p->i_SetE(  vec.e() );   // recompute E
       }

     }
     else if(m_inputUsesMass){
       // Read-in particle as PtEtaPhiM4 vectors, then recompute E
       typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > LorentzVectorM_t;
       const std::vector<float> &v_E   = E();             
       for(size_t i=0;i<N;i++){

         Particle *p = fProxies[i];
         if(p->m_varMap) p->m_varMap->clear();
         LorentzVectorM_t vec( v_pt[i], v_eta[i], v_phi[i], v_E[i]);   
         p->i_SetPt( v_pt[i] );
         p->i_SetEta(v_eta[i]);
         p->i_SetPhi(v_phi[i]);
         p->i_SetE(  vec.e() );   // recompute E
       }
     }else {
       const std::vector<float> &v_E   = E();    
       for(size_t i=0;i<N;i++){
         Particle *p = fProxies[i];
         if(p->m_varMap) p->m_varMap->clear();
         p->i_SetPt( v_pt[i]);
         p->i_SetEta(v_eta[i]);
         p->i_SetPhi(v_phi[i]);
         p->i_SetE(  v_E[i]);   
       }

     }
     

     m_attributes.resizeNonInputVar(N);
   }

  Particle* ParticleContainer::at( size_t index ){
    return (*this)[index];
  }

  const Particle* ParticleContainer::at( size_t index ) const{
    return (*this)[index];
  }


  Particle* ParticleContainer::operator[]( size_t index ) {
     if(fFromInput){
       if( *m_master != m_currentEntry ) buildContainerFromInput();    
     }
     return fProxies[ index ];
   }

   /**
    * This operator can be used to get access to one element in the
    * collection. This element can then be passed around between parts
    * of the analysis code easily.
    *
    * This version is useful when only reading the variables.
    *
    * @param index Index of the element inside the collection
    */
   const Particle* ParticleContainer::operator[]( size_t index ) const {
     return const_cast<ParticleContainer*>(this)->operator[](index);
   }


  /// size of the container
  size_t ParticleContainer::size() const {
    //if( pt.fromInput() )return pt().size();
    if(( fFromInput) && m_master)  return pt().size();
    return fProxies.size();
  }

  bool ParticleContainer::setSubsetOfInputContainer(const ParticleContainer* other){
    if(size()>0){Error("setSubsetOfInputContainer", " This container is non empty"); return false;}
    const ParticleContainer * realSource = other;
    while( ! realSource->fFromInput) { 
      realSource = realSource->m_sourceContainer; 
      if(realSource==NULL) {
        Error("setSubsetOfInputContainer", " <%s> can't be a subset : didn't find an input container as source (starting from <%s>) ", getName().c_str(), other->getName().c_str());
        return false;
      }
    }
    m_sourceContainer = realSource;
    fFromInput = false;
    return true;
  }

  void ParticleContainer::declareOutputVarAsIn(const ParticleContainer* p, const string_t & pattern, bool writeOut) const {
    m_attributes.addVarAsIn( p->m_attributes, pattern, writeOut);
  }

  void ParticleContainer::updateKinematics(Particle* p){
    size_t index = p->getIndex();
    pt.uncheckedAccess()[index] = p->pt();  
    eta.uncheckedAccess()[index] = p->eta();  
    phi.uncheckedAccess()[index] = p->phi();  
    E.uncheckedAccess()[index] = p->E();      
  }

  void ParticleContainer::loadCurrentEvent() const {
    if( *m_master != m_currentEntry ) {if (fFromInput) buildContainerFromInput();}
  }

  ParticleContainer::const_iterator ParticleContainer::begin() const {
    loadCurrentEvent();
    return fProxies.begin();
  }
  ParticleContainer::iterator       ParticleContainer::begin()  {
    loadCurrentEvent();
    return fProxies.begin();
  }  
  ParticleContainer::const_iterator ParticleContainer::end() const {
    loadCurrentEvent();
    return fProxies.begin()+size(); // fProxies might be bigger than actual size
  }
  ParticleContainer::iterator       ParticleContainer::end()  {
    loadCurrentEvent();
    return fProxies.begin()+size(); // fProxies might be bigger than actual size
  }


  bool ParticleContainer::fillAssoContainerAtIndex(const ParticleContainer& sourceContainer, ParticleContainer& assoParticles, size_t index, const string_t & indexVariable) const {
    assoParticles.setSubsetOfInputContainer( &sourceContainer );
    std::vector<int> & indInSource = (*m_attributes.getHandle< std::vector<int> >(indexVariable))()[index];
    assoParticles.copyFrom(sourceContainer, indInSource);
    // for(size_t i=0; i< indInSource.size(); i++){
    //   assoParticles.addNewFromSource( sourceContainer[ indInSource[i] ] );
    // }
    return true;
  }
  

  void  ParticleContainer::loadEntry(::Long64_t eventI){
    if(fFromInput){
      if( m_master != &m_currentEntry){
        setMaster( &m_currentEntry);        
      }
      m_currentEntry = eventI;
      buildContainerFromInput();    
    }
    else{
      Error("loadEntry","Trying to loadEntry with a container not associated to an input TTree");
    }
  }

  void ParticleContainer::swap(ParticleContainer& other){
    fProxies.swap(other.fProxies);
    m_attributes.swap(other.m_attributes);
    const ParticleContainer *tmp = m_sourceContainer; m_sourceContainer = other.m_sourceContainer; other.m_sourceContainer = tmp;
    tmp = m_constituentSource; m_constituentSource = other.m_constituentSource; other.m_constituentSource = tmp;
    
    /// swap pointers in VarHandles if valid
    std::vector<float> *vptr, *ovptr;
    vptr= eta.rawPointer(); ovptr = other.eta.rawPointer(); if( bool(vptr) && bool(ovptr) ) ovptr->swap(*vptr);
    vptr= phi.rawPointer(); ovptr = other.phi.rawPointer(); if( bool(vptr) && bool(ovptr) ) ovptr->swap(*vptr);
    vptr= pt.rawPointer(); ovptr = other.pt.rawPointer(); if( bool(vptr) && bool(ovptr) ) ovptr->swap(*vptr);
    vptr= E.rawPointer(); ovptr = other.E.rawPointer(); if( bool(vptr) && bool(ovptr) ) ovptr->swap(*vptr);

  }

} // namespace JetAnalysisEDM
