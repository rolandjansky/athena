/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TrkParametersBase/test/ParametersBase_test.cxx
 * @author Shaun Roe
 * @date Jan, 2019
 * @brief Some tests for Trk::ParametersBase 
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_TRKPARAMETERSBASE


#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>
//
#include "../TrkParametersBase/ParametersBase.h"
#include "TrkParametersBase/Charged.h"
#include "TrkParametersBase/Neutral.h"
#include <ostream>
#include <string>
#include <utility>


//Tests run by typing 'ctest' in the build directory (-v for verbose)
//or can run the executable directly, e.g
// ../build/Tracking/TrkEvent/TrkParametersBase/test-bin/ParametersBase_test.exe -l all
//-l gives logging level (a BOOST_TEST option)

//******************************
constexpr unsigned int DIM{5};
using T = Trk::Charged;
//******************************

//truly minimal Surface to use below
namespace Trk{
  class Surface{
  public:
    static constexpr unsigned int id{12345};
    static std::string str() {
      return std::string("TestSurface");
    }
  };
  
  std::ostream & operator<< (std::ostream & out, const Surface &  /*s*/){
    out<<Trk::Surface::str();
    return out;
  }
}
//dont ever do this in real code
class ChargedParametersStub final : public Trk::ParametersBase<DIM,T>{
public:
  //access base class Constructors
  ChargedParametersStub(const AmgVector(DIM)& parameters,
    std::optional<AmgSymMatrix(DIM)> covariance, const T chargeDef):
    Trk::ParametersBase<DIM,T>(parameters, std::move(covariance),chargeDef){
    //nop           
  }
  
  ChargedParametersStub(std::optional<AmgSymMatrix(DIM)> covariance):
    ParametersBase(std::move(covariance)){
   //nop
  }
  
  ChargedParametersStub(const AmgVector(DIM) & parameters,
    std::optional<AmgSymMatrix(DIM)> covariance = std::nullopt):
    ParametersBase(parameters,std::move(covariance)){
    //nop  
  }


  ChargedParametersStub() = default;
  double charge() const final{
    return m_chargeDef.charge();
  }
  Amg::Vector3D position() const override{
    Amg::Vector3D p (1, 1, 1);
    return p;
  }
  Amg::Vector3D momentum() const override{
    Amg::Vector3D p (3, 4, 5); //45 degree, eta ~0.881374
    return p;
  }
  bool hasSurface() const override{
    return false;
  }
  Amg::RotationMatrix3D measurementFrame() const override{
    Amg::RotationMatrix3D m;
    m.setIdentity();
    return m;
  } 

  virtual
  const Trk::Surface& associatedSurface() const override {
    return m_surface;
  }
  Trk::ParametersBase<DIM,T>* clone() const override{
    return new ChargedParametersStub(*this);
  }
  Trk::SurfaceType surfaceType() const override{
    return Trk::SurfaceType::Other;
  }
  Trk::ParametersType type() const override{
    return Trk::ParametersType::AtaSurface;
  }
  
  void updateParametersHelper(const AmgVector(DIM) & p) override{
    m_parameters = p;
  }
  private:
    Trk::Surface m_surface{};
};

BOOST_AUTO_TEST_SUITE(ParametersBaseTest)
  BOOST_AUTO_TEST_CASE(Constructors){
    //constructors are 'tested' just by seeing whether the following
    //compiles for each instantiation, but I put the 'NO_THROW' check around them
    using ChargedParams_t [[maybe_unused]] = Trk::ParametersBase<DIM, T>;
    //ChargedParams_t x; <-constructors are protected, this won't compile
    //Instantiate a derived class using default c'tor . Never do this! 
    //The default c'tor is only for use by POOL
    BOOST_CHECK_NO_THROW(ChargedParametersStub x);
    //preconstruct required parameters
    AmgVector(DIM) parameters;
    std::optional<AmgSymMatrix(DIM)> covariance{};
    T chargeDef;
    BOOST_CHECK_NO_THROW(ChargedParametersStub y(parameters, covariance, chargeDef));
    BOOST_CHECK_NO_THROW(ChargedParametersStub z(covariance));
    //c'tor defaulting covariance
    BOOST_CHECK_NO_THROW(ChargedParametersStub u(parameters));
    //c'tor with defined covariance
    BOOST_CHECK_NO_THROW(ChargedParametersStub v(parameters,covariance));
    //we'll check later whether the following actually did what you think;
    //for now, just check they compile
    //copy c'tor
    ChargedParametersStub v;
    BOOST_CHECK_NO_THROW(ChargedParametersStub w(v));
    //move c'tor?
    BOOST_CHECK_NO_THROW(ChargedParametersStub w(std::move(v)));
  }
  BOOST_AUTO_TEST_CASE(Getters){
    //preconstruct required parameters
    static const double pars[] = {0.5,1,1.5,2.,2.5};
    AmgVector(DIM) parameters (pars);
    AmgSymMatrix(DIM) m;
    m.setIdentity();
    std::optional<AmgSymMatrix(DIM)> covariance{m};
    T chargeDef;
    ChargedParametersStub y(parameters, covariance, chargeDef);
    BOOST_TEST(y.charge() == 1.0);
    BOOST_TEST(y.parameters() == parameters);
    //operator '*' here is dereferencing pointer and getting optional value resp.
    BOOST_TEST(*(y.covariance()) == *covariance);
    BOOST_TEST(y.pT() == 5.);
    constexpr double etaAt45Deg{0.88137358701954305};
    BOOST_TEST(y.eta() == etaAt45Deg);
    BOOST_TEST(y.isCharged() == true);
    const auto expectedLocalPosition=Amg::Vector2D(parameters[Trk::loc1], parameters[Trk::loc2]);
    BOOST_TEST(y.localPosition() == expectedLocalPosition);
    BOOST_TEST(y.charge() == chargeDef.charge());
    Amg::Vector3D expectedPosition (1, 1, 1);
    Amg::Vector3D expectedMomentum (3, 4, 5);
    BOOST_TEST(y.position() == expectedPosition);
    BOOST_TEST(y.momentum() == expectedMomentum);
    BOOST_TEST(y.hasSurface() == false);
    //dummy check on my dummy surface. 
    constexpr unsigned int surfaceId{12345};
    BOOST_TEST(y.associatedSurface().id == surfaceId);
    Amg::RotationMatrix3D expected;
    expected.setIdentity();
    BOOST_TEST((y.measurementFrame() == expected) );
    BOOST_TEST(y.type() == Trk::ParametersType::AtaSurface);
    BOOST_TEST((y.surfaceType() == Trk::SurfaceType::Other));
    //note that the covariance matrix pointer changes for each run,
    //would need a regex test to test properly. For now, just content to
    //print it as a message.
    std::ostringstream os;
    y.dump(os);
    BOOST_TEST_MESSAGE(os.str());
  }
  
  BOOST_AUTO_TEST_CASE(Setters){
    ChargedParametersStub x;
    static const double pars[] = {0.5,1,1.5,2.,2.5};
    AmgVector(DIM) parameters (pars);
    BOOST_CHECK_NO_THROW(x.setParameters(parameters));
    BOOST_TEST(x.parameters() == parameters,"Parameters have been set");
    /** set covariance */
    AmgSymMatrix(DIM) m;
    m.setIdentity();
    std::optional<AmgSymMatrix(DIM)> covariance{m};
    x.setCovariance(*covariance);
    BOOST_TEST(*(x.covariance()) == *covariance,"Covariance has been set");
    //update both
    AmgVector(DIM) otherParameters;
    otherParameters.setZero();
    std::optional<AmgSymMatrix(DIM)> otherCovariance{2.*m};
    x.updateParameters(otherParameters, *otherCovariance);
    const bool test = (x.parameters() == otherParameters) and (*(x.covariance()) == *otherCovariance);
    BOOST_TEST(test, "Updating parameters and covariance");
    x.updateParameters(parameters);
    BOOST_TEST(x.parameters() == parameters,"Just the parameters have been updated");
  }
  
  BOOST_AUTO_TEST_CASE(CopyEqualityAssignment){
    //preconstruct required parameters
    static const double pars[] = {0.5,1,1.5,2.,2.5};
    AmgVector(DIM) parameters (pars);
    AmgSymMatrix(DIM) m;
    m.setIdentity();
    std::optional<AmgSymMatrix(DIM)> covariance{m};
    T chargeDef;
    ChargedParametersStub y(parameters, covariance, chargeDef);
    ChargedParametersStub z(parameters, covariance, chargeDef);
    //does testing equality work?
    BOOST_TEST(y == z,"Equality operator");
    ChargedParametersStub x(y);
    //is a copy constructed object the same as the original?
    BOOST_TEST(x == y,"Copy constructed is same as original");
    //is an assigned object the same as the original?
    ChargedParametersStub u;
    //there's no '!=' operator
    BOOST_TEST(!(u == x),"Two differently constructed objects are not equal");
    u=x;
    BOOST_TEST(u == x,"After assignment, the objects are equal");
    auto *const p = x.clone();
    BOOST_TEST(*p == x,"Cloning an object results in a similar object");
    std::unique_ptr<Trk::ParametersBase<DIM, T>> uniquePtr{x.uniqueClone()};
    BOOST_TEST(*uniquePtr == x,"Cloning to unique_ptr gives identical results");
  }
BOOST_AUTO_TEST_SUITE_END()
