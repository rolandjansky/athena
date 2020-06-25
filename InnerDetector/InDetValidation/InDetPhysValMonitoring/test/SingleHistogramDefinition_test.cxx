/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPhysValMonitoring/test/SingleHistogramDefinition_test.cxx
 * @author Shaun Roe
 * @date May 2020
 * @brief Some tests for SingleHistogramDefinition class 
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop



#include "../InDetPhysValMonitoring/SingleHistogramDefinition.h"
#include <cmath> //for std::isnan

BOOST_AUTO_TEST_SUITE(SingleHistogramDefinitionTest)
  BOOST_AUTO_TEST_CASE(constructors){
    BOOST_CHECK_NO_THROW(SingleHistogramDefinition());
    const SingleHistogramDefinition emptyDefinition;
    bool allStringsAreEmpty = emptyDefinition.name.empty() and emptyDefinition.histoType.empty()
      and emptyDefinition.title.empty() and emptyDefinition.xTitle.empty() 
      and emptyDefinition.yTitle.empty() and emptyDefinition.zTitle.empty() 
      and emptyDefinition.allTitles.empty() and emptyDefinition.folder.empty();
    bool allBinsAreZero = (emptyDefinition.nBinsX == 0) and (emptyDefinition.nBinsY == 0)
      and (emptyDefinition.nBinsZ == 0);
    auto pairIsInvalid = [](const std::pair<float, float> & axisPair){
      return (std::isnan(axisPair.first) and std::isnan(axisPair.second));
    };
    bool allAxesAreInvalid = pairIsInvalid(emptyDefinition.xAxis) and pairIsInvalid(emptyDefinition.yAxis)
      and pairIsInvalid(emptyDefinition.zAxis);
    BOOST_TEST((allStringsAreEmpty and allBinsAreZero and allAxesAreInvalid));
    //Histogram construction with just x parameters defined
    /**
    SingleHistogramDefinition::SingleHistogramDefinition(Titles_t thename, Titles_t thehistoType,
						     Titles_t thetitle, NBins_t nbinsX, 
						     Var_t xLo, Var_t xHi,
						     Titles_t xName, Titles_t yName,
						     Titles_t thefolder)
		**/
		const std::string name("hName");
		const std::string type("TF");
		const std::string title("hTitle");
		const unsigned int nBinsX{10};
		const float xLo(0.1), xHi(10.1);
		const auto xAxis=std::make_pair(xLo, xHi);
		const std::string xTitle{"xAxis"}, yTitle{"yAxis"};
		const std::string aFolder("folderName");
		//
		auto referenceValuesMatchXConstruction = [=](const auto & h){return h.name == name and h.histoType == type 
		   and h.title == title and h.nBinsX == nBinsX and h.xAxis == xAxis
		   and h.xTitle == xTitle and h.yTitle == yTitle and aFolder == h.folder;
		};
		//
		auto omittedValuesXYAreUndefined = [=](const auto & h) {
		return pairIsInvalid(h.zAxis) and h.nBinsZ == 0;
		};
		auto omittedValuesXAreUndefined = [=](const auto & h) {
		  return  omittedValuesXYAreUndefined(h) and pairIsInvalid(h.yAxis) and h.nBinsY==0;
		};
		//construct the X histo and test
		const SingleHistogramDefinition xHisto(name, type, title, nBinsX, xLo, xHi, xTitle, yTitle, aFolder );
		BOOST_TEST(referenceValuesMatchXConstruction(xHisto));
		BOOST_TEST(omittedValuesXAreUndefined(xHisto));
		//Histogram definition with n bins and limits for yAxis
		const unsigned int nBinsY{10};
		const float yLo(0.2), yHi(10.2);
		const auto yAxis=std::make_pair(yLo, yHi);
    //
		auto referenceValuesMatchXYConstruction = [=] (const auto & h){
		  return referenceValuesMatchXConstruction(h) and h.nBinsY == nBinsY and h.yAxis == yAxis;
		};
		// construct the XY histo and test
		const SingleHistogramDefinition yHisto(name, type, title, nBinsX, nBinsY, xLo, xHi, yLo, yHi, xTitle, yTitle, aFolder );
		BOOST_TEST(referenceValuesMatchXYConstruction(yHisto));
		BOOST_TEST(omittedValuesXYAreUndefined(yHisto));
		//Histogram definition with n bins and limits for zAxis also
		const unsigned int nBinsZ{10};
		const float zLo(0.3), zHi(10.3);
		const auto zAxis=std::make_pair(zLo, zHi);
		const std::string zTitle{"zAxis"};
		auto referenceValuesMatchXYZConstruction = [=] (const auto & h){
		  return referenceValuesMatchXYConstruction(h) and h.nBinsZ == nBinsZ and h.zAxis == zAxis and h.zTitle == zTitle;
		};
		// construct the XYZ histo and test
		const SingleHistogramDefinition zHisto(name, type, title, nBinsX, nBinsY, nBinsZ, xLo, xHi, yLo, yHi, zLo, zHi, xTitle, yTitle, zTitle, aFolder );
		BOOST_TEST(referenceValuesMatchXYZConstruction(zHisto));
		//nothing left undefined in this case
  }
  BOOST_AUTO_TEST_CASE(publicMethodsOnEmptyInstance){
    const SingleHistogramDefinition emptyDefinition;
    //its empty?
    BOOST_TEST(emptyDefinition.empty());
    //whats the string representation of an empty histogram definition?
    //(careful about spaces; perhaps this should be improved!)
    BOOST_TEST(emptyDefinition.str() == "   0 0 0 nan nan nan nan nan nan   ");
    //empty definition should not have valid histo type, or be valid in any sense
    BOOST_TEST(emptyDefinition.validType() == false);
    BOOST_TEST(emptyDefinition.isValid() == false);
    //the stringIndex should be empty
    BOOST_TEST(emptyDefinition.stringIndex().empty() ==  true);
    //the title digest is just the delimiters
    const std::string onlyDelimiters{";;;"};
    BOOST_TEST(emptyDefinition.titleDigest() == onlyDelimiters);
  }
  BOOST_AUTO_TEST_CASE(publicMethodsOnValidInstance){
    const std::string name("hName");
		const std::string type("TH1");
		const std::string title("hTitle");
		const unsigned int nBinsX{10};
		const float xLo(0.1), xHi(10.1);
		const std::string xTitle{"xAxis"}, yTitle{"yAxis"};
		const std::string aFolder("folderName");
		const SingleHistogramDefinition validDefinition(name, type, title, nBinsX, xLo, xHi, xTitle, yTitle, aFolder );
    //its empty?
    BOOST_TEST(validDefinition.empty() == false);
    //whats the string representation of an empty histogram definition?
    //(careful about spaces; perhaps this should be improved!)
    auto quote = [](const std::string & s){return "'" + s + "'";};
    BOOST_TEST(quote(validDefinition.str()) == quote("hName TH1 hTitle 10 0 0 0.100000 10.100000 nan nan nan nan xAxis yAxis "));
    //valid definition should have valid histo type
    BOOST_TEST(validDefinition.validType() == true);
    BOOST_TEST(validDefinition.isValid() == true);
    //the stringIndex should show the full path
    BOOST_TEST(validDefinition.stringIndex() ==  "folderName/hName");
    //the title digest is a delimited concatenation of titles
    BOOST_TEST(validDefinition.titleDigest() == "hTitle;xAxis;yAxis;");
  }
  BOOST_AUTO_TEST_CASE(publicMethodsOnInvalidInstance){
    const std::string name("hName");
		const std::string type("TH1");
		const std::string title("hTitle");
		const unsigned int nBinsX{10};
		const float xLo(0.1), xHi(10.1);
		const std::string xTitle{"xAxis"}, yTitle{"yAxis"};
		const std::string aFolder("folderName");
		const SingleHistogramDefinition badLimits(name, type, title, nBinsX, 10.1, 10.1, xTitle, yTitle, aFolder );
		BOOST_TEST(badLimits.validType() == true);
    BOOST_TEST(badLimits.isValid() == false);
    const SingleHistogramDefinition badType(name, "", title, nBinsX, xLo, xHi, xTitle, yTitle, aFolder );
		BOOST_TEST(badType.validType() == false);
    BOOST_TEST(badType.isValid() == false);
    const SingleHistogramDefinition unnamed("", type, title, nBinsX, xLo, xHi, xTitle, yTitle, aFolder );
    BOOST_TEST(unnamed.validType() == true);
    BOOST_TEST(unnamed.isValid() == false);
  }
BOOST_AUTO_TEST_SUITE_END()

