/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetAttributeHisto.h"
#define toGeV 1/1000.

namespace jet {
  template<typename T>
  using Accessor = SG::AuxElement::Accessor< T >;

  class HistoFiller {
  public:
    virtual ~HistoFiller() {}
    virtual void fill(const xAOD::Jet &, float /*weight*/ ){};
    // allows to avoid invalid attribute
    virtual bool isValid(const xAOD::Jet &){return false;}
  };

  // ------------------
  template<typename ATYPE, typename HTYPE>
  struct AccessorAndHisto {
    AccessorAndHisto(const std::string & attname, HTYPE* h, bool gev) : m_accessor(attname), m_h(h), m_toGeV1(gev ? toGeV :1) {}

  protected:
    Accessor< ATYPE > m_accessor;
    HTYPE* m_h;
    const float m_toGeV1;
  };

  template<typename ATYPE1,typename ATYPE2, typename HTYPE>
  struct AccessorAndHisto2 : public AccessorAndHisto<ATYPE1, HTYPE> {
    AccessorAndHisto2(const std::string & att1,const std::string & att2  , HTYPE* h, bool gev1, bool gev2) : AccessorAndHisto<ATYPE1,HTYPE>(att1,h, gev1), m_accessor2(att2), m_toGeV2(gev2 ? toGeV : 1) {}
  protected:
    Accessor< ATYPE2 > m_accessor2;
    const float m_toGeV2;
  };
  // ------------------

// shotcuts to have more compact code
#define scale1 this->m_toGeV1
#define scale2 this->m_toGeV2

  // ------------------
  // 1D hist fillers
  template<typename T>
  struct AttFiller : public HistoFiller, public AccessorAndHisto<T, TH1F> {
    AttFiller(const std::string & attname, TH1F* h, bool gev1)  : AccessorAndHisto<T, TH1F>(attname, h, gev1) {}

    virtual void fill(const xAOD::Jet & j, float weight){this->m_h->Fill( this->m_accessor(j)*scale1, weight ); };
    virtual bool isValid(const xAOD::Jet &j){return this->m_accessor.isAvailable(j);}
  };


  template<typename T>
  struct VecAttFiller : public HistoFiller, public AccessorAndHisto<std::vector<T>, TH1F> {
    VecAttFiller(const std::string & attname, TH1F* h, bool gev1)  : AccessorAndHisto<std::vector<T>, TH1F>(attname, h, gev1) {}

    virtual void fill(const xAOD::Jet & j, float weight){
      const std::vector<T> & vec = this->m_accessor( j);
      for(const T& v : vec ) this->m_h->Fill( v *scale1, weight );
    }
    virtual bool isValid(const xAOD::Jet & j){return this->m_accessor.isAvailable(j);}

  };

  template<typename T>
  struct VecAttIndexFiller : public HistoFiller, public AccessorAndHisto<std::vector<T>, TH1F> {
    VecAttIndexFiller(const std::string & attname, TH1F* h, size_t index, bool gev1) : AccessorAndHisto<std::vector<T>,TH1F>(attname,h, gev1), m_index(index) {}

    virtual void fill(const xAOD::Jet & j, float weight){
      const std::vector<T> & vec = this->m_accessor( j);
      if( vec.size() > m_index) this->m_h->Fill( vec[m_index]*scale1, weight );
    }
    virtual bool isValid(const xAOD::Jet & j){return this->m_accessor.isAvailable(j);}

  protected:
    size_t m_index;
  };


  // ------------------
  // 2D hist fillers
  template<typename T,typename HTYPE>
  struct AttvsAttFiller : public HistoFiller, public AccessorAndHisto2<T,T, HTYPE> {
    AttvsAttFiller(const std::string & att1,const std::string & att2  , HTYPE* h, bool gev1, bool gev2) : AccessorAndHisto2<T,T, HTYPE>(att1,att2,h, gev1, gev2) {}

    virtual void fill(const xAOD::Jet & j, float weight){this->m_h->Fill( this->m_accessor(j)*scale1, this->m_accessor2(j)*scale2, weight ); };
    virtual bool isValid(const xAOD::Jet &j){return (this->m_accessor.isAvailable(j))&&(this->m_accessor2.isAvailable(j));}

  };

  template<typename T,typename HTYPE>
  struct AttvsVecAttIndexFiller : public HistoFiller, public AccessorAndHisto2<std::vector<T>,T, HTYPE> {
    AttvsVecAttIndexFiller(const std::string & att1,const std::string & att2  , HTYPE* h, size_t index , bool gev1, bool gev2, bool swapAxis=false) : AccessorAndHisto2<std::vector<T>,T, HTYPE>(att1,att2,h, gev1, gev2) , m_index(index), m_swap(swapAxis){}

    virtual void fill(const xAOD::Jet & j, float weight){
      const std::vector<T> & vec = this->m_accessor( j);
      if( vec.size() > m_index) {
        if( m_swap) this->m_h->Fill( this->m_accessor2(j)*scale2, vec[m_index]*scale1, weight ) ;
        else        this->m_h->Fill( vec[m_index] *scale1, this->m_accessor2(j)*scale2, weight) ;
      }
    }

    virtual bool isValid(const xAOD::Jet &j){return (this->m_accessor.isAvailable(j))&&(this->m_accessor2.isAvailable(j));}

  protected:
    size_t m_index;
    bool m_swap;
  };


#undef scale1
#undef scale2

  // ------------------
  // ------------------
  struct AttTypes {
    enum Supported {
      Unknown=0,
      Int=1,
      Float=2,
      VectFloat=3,
      VectInt=4,

      Float_Float=22,
      Float_VectFloat=23,
      VectFloat_Float=32,
    };
    static Supported fromString(const std::string &n){
      static std::map< std::string , Supported > m( { {"int",Int}, {"float",Float}, {"vector<float>",VectFloat}, {"vector<int>",VectInt},  } );
      if ( m.find( n ) != m.end() ) return m[n];
      return Unknown;
    }
    static Supported fromString(const std::string &n1, const std::string &n2){
      Supported t1 = fromString(n1);
      Supported t2 = fromString(n2);
      switch( (Supported) (10*t1+t2) ) {
      case Float_Float:
        return Float_Float;
      case Float_VectFloat:
        return Float_VectFloat;
      case VectFloat_Float:
        return VectFloat_Float;
      default:
        break;
      }
      return Unknown;
    }

  };


}



// ****************************************************
// ****************************************************


JetAttributeHisto::JetAttributeHisto(const std::string &t) : JetHistoBase(t)
                                                           , m_histoDef(this)
                                                           , m_attTypes()
                                                           , m_selectedIndex(-1)
                                                           , m_doTProfile(false)
                                                           , m_histoFiller(0)
{
  declareProperty("HistoDef", m_histoDef);
  declareProperty("SelectIndex", m_selectedIndex);
  declareProperty("DoTProfile", m_doTProfile);
  declareProperty("AttributeTypes", m_attTypes);
  declareProperty("AttributeNames", m_attNames);
  declareProperty("AttributeInGeV", m_attGeV);

}

JetAttributeHisto::~JetAttributeHisto(){
  if(m_histoFiller) delete m_histoFiller;
}
StatusCode JetAttributeHisto::initialize() {
  CHECK( m_histoDef.retrieve() );
  CHECK( JetHistoBase::initialize() );
  if(m_attNames.empty() ) {
    ATH_MSG_ERROR("Differing no attribute name specified  ");
    return StatusCode::FAILURE;
  }

  if( m_attNames.size() != m_attTypes.size() ){
    ATH_MSG_ERROR("Differing number of attributes names and types  ");
    return StatusCode::FAILURE;
  }

  m_attGeV.resize(m_attNames.size(), false); // re-size it to 2 entry with false by default. No effect if already size 2

  return StatusCode::SUCCESS;
}


using namespace jet;
int JetAttributeHisto::buildHistos(){
  //std::string attname = m_histoDef->attributeName();

  if( m_attTypes.size() == 1 ){
    bool gev1 = m_attGeV[0];
    AttTypes::Supported typ = AttTypes::fromString(m_attTypes[0]);
    switch( typ ){
    case AttTypes::Int:
      m_histoFiller = new AttFiller<int>(m_attNames[0],
                                         bookHisto( m_histoDef->buildTH1F() ), gev1 );
      break;
    case AttTypes::Float :
      m_histoFiller = new AttFiller<float>(m_attNames[0],
                                           bookHisto( m_histoDef->buildTH1F() ), gev1 );
      break;
    case AttTypes::VectFloat :
      {
        if(m_selectedIndex==-1)
          m_histoFiller =      new VecAttFiller<float>(m_attNames[0],
                                                       bookHisto( m_histoDef->buildTH1F() ), gev1 );
        else
          m_histoFiller = new VecAttIndexFiller<float>(m_attNames[0],
                                                       bookHisto( m_histoDef->buildTH1F() ) , m_selectedIndex , gev1
                                                        );
      }
      break;
    case AttTypes::VectInt :
      {
        if(m_selectedIndex==-1)
          m_histoFiller =      new VecAttFiller<int>(m_attNames[0],
                                                       bookHisto( m_histoDef->buildTH1F() ), gev1 );
        else
          m_histoFiller = new VecAttIndexFiller<int>(m_attNames[0],
                                                       bookHisto( m_histoDef->buildTH1F() ) , m_selectedIndex , gev1
                                                        );
      }
      break;
    default:
      {
        ATH_MSG_ERROR("Do not support attribute type "<< m_attTypes[0] << "  "<< typ);
        return 1;
      }
    } // switch
  }// attType.size()==1

  else if( m_attTypes.size() == 2 ){
    std::string att1 = m_attTypes[0];
    std::string att2 = m_attTypes[1];
    std::string n1 = m_attNames[0];
    std::string n2 = m_attNames[1];
    bool gev1 = m_attGeV[0];
    bool gev2 = m_attGeV[1];

    AttTypes::Supported typ = AttTypes::fromString(att1,att2);

    switch( typ ){
    case AttTypes::Float_Float:
      {
        if(m_doTProfile)
          m_histoFiller = new AttvsAttFiller<float, TProfile>(n1, n2,
                                                              bookHisto( m_histoDef->buildTProfile() ), gev1, gev2 );
        else
          m_histoFiller =     new AttvsAttFiller<float, TH2F>(n1, n2,
                                                              bookHisto( m_histoDef->buildTH2F() ),gev1, gev2 );
      }
      break;
    case AttTypes::VectFloat_Float:
    case AttTypes::Float_VectFloat:
      {
        if(m_selectedIndex==-1) { ATH_MSG_ERROR("Must select an index >-1 for vector<float> vs float attribute histogramming"); return 1;}
        bool swapAxis = (typ != AttTypes::VectFloat_Float);
        if(swapAxis) att1.swap(att2);
        if(m_doTProfile)
          m_histoFiller = new AttvsVecAttIndexFiller<float, TProfile>(n1, n2,
                                                                      bookHisto( m_histoDef->buildTProfile() ),
                                                                      m_selectedIndex, gev1, gev2, swapAxis);

        else
          m_histoFiller = new AttvsVecAttIndexFiller<float, TH2F>(n1, n2,
                                                                  bookHisto( m_histoDef->buildTH2F() ),
                                                                  m_selectedIndex, gev1, gev2, swapAxis);
      }
      break;
    default:
      {
        ATH_MSG_ERROR("Do not support attribute types "<< m_attTypes[0] << " with "<< m_attTypes[1]);
        return 1;
      }
    } // switch
  } //size()==2
  if(m_histoFiller==NULL) {
    ATH_MSG_ERROR("Do not support attribute types __ ");
    return 1;
  }

  return 0;
}

int JetAttributeHisto::fillHistosFromJet(const xAOD::Jet &j, float weight){
  m_histoFiller->fill(j, weight);
  return 0;
}

int JetAttributeHisto::fillHistosFromContainer(const xAOD::JetContainer & cont, float weight){
  if (cont.empty() ) return 0;
  const xAOD::Jet * j0 = cont[0];
  if ( !m_histoFiller->isValid(*j0) ){
    return 0;
  }
  return JetHistoBase::fillHistosFromContainer(cont, weight);
}


void JetAttributeHisto::renameAndRegister(TH1* h, const std::string & subD, Interval_t i){

  // if(m_selectedIndex >-1 ) {
  //   TString s="_"; s+= m_selectedIndex;
  //   h->SetName( h->GetName()+s);
  // }
  JetHistoBase::renameAndRegister(h,subD,i);
  if(m_selectedIndex >-1 ) {
    TString s=" (at index "; s+= m_selectedIndex; s+=")";
    h->SetTitle( h->GetTitle()+s);
  }
}
