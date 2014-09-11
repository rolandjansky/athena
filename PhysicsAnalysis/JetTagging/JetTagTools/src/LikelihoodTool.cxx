/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
///    Author: A. Wildauer
///    CERN, January 19, 2005
///
/////////////////////////////////////////////////////////////////////////////////////////////

#include "JetTagTools/LikelihoodTool.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include <cmath>

namespace Analysis
{

LikelihoodTool::LikelihoodTool(const std::string& t, const std::string& n, const IInterface* p) :
        AthAlgTool(t,n,p),
        m_histoSvc(0),
        m_allLhVariables(std::vector<std::string>()),
        m_useTheseLhVariables(std::vector<std::string>()),
        m_mapOfAllHistos(std::vector<std::map<std::string, TH1*>* >()),
        m_histoLimitsMap(std::map< std::string, HistoLimits>() ),
        m_lhVariableValues(std::map<std::string, std::vector<double> >()),
        m_likelihoodVector(std::vector<double>())
{
    declareInterface<LikelihoodTool>(this);
    declareProperty("allLhVariables", m_allLhVariables);
    declareProperty("useTheseLhVariables", m_useTheseLhVariables);
}

LikelihoodTool::~LikelihoodTool()
{
    for (std::vector< std::map<std::string, TH1*>* >::iterator itr = m_mapOfAllHistos.begin();
            itr != m_mapOfAllHistos.end(); ++itr)
    {
        delete (*itr);
    }
}

StatusCode LikelihoodTool::initialize()
{
  std::string histoservicename = "THistSvc";
  StatusCode sc = service(histoservicename, m_histoSvc);
  if(sc.isFailure())
    {
      ATH_MSG_FATAL(histoservicename << " not found!");
    }
  return sc;
}

StatusCode LikelihoodTool::finalize()
{
  return StatusCode::SUCCESS;
}

void LikelihoodTool::readInHistosFromFile(const std::string& refFileName)
{
    // the tmpHistoMap belongs to m_mapOfAllHistos and is deleted at the end in the destructor of LikelihoodTool()
    std::map<std::string, TH1*>* tmpHistoMap  = new std::map<std::string, TH1*>();
    for (std::vector<std::string>::iterator itr = m_useTheseLhVariables.begin() ;
            itr != m_useTheseLhVariables.end()  ; ++itr )
    {
        const std::string& histoName = *itr;

        std::string fullPath(refFileName+histoName);

	ATH_MSG_VERBOSE("Retrieving histo: " << "/" << fullPath);
        StatusCode sc = m_histoSvc->regHist("/"+fullPath);
        if(sc.isFailure())
        {
	  ATH_MSG_DEBUG("Could not register histo /" << fullPath);
        }

        TH1* tmphisto(0);
        sc = m_histoSvc->getHist("/"+fullPath,tmphisto);
        if(sc.isFailure())
        {
	  ATH_MSG_DEBUG("Could not get histo /" << fullPath);
        }
	if (tmphisto == 0) ATH_MSG_DEBUG("I COULD NOT RETRIEVE HISTO: " << "/" << fullPath);

        /** Normalize if necessary */
        double norm(tmphisto->Integral());
        if (norm!=1.)
        {
	  ATH_MSG_INFO("Normalizing histo: " << fullPath);
          tmphisto->Scale(1./norm);
        }

        tmpHistoMap->insert(
          std::pair<std::string, TH1*>(
            histoName, tmphisto
            )
        );

        /** Retrieve and set histo limits for this histogram */
        unsigned int bins(tmphisto->GetNbinsX());
        const TAxis* xAxis = tmphisto->GetXaxis();
        double minx(xAxis->GetXmin());
        double maxx(xAxis->GetXmax());

        m_histoLimitsMap[histoName] = HistoLimits(bins, minx, maxx);
    }
    m_mapOfAllHistos.push_back(tmpHistoMap);
}

void LikelihoodTool::setLhVariableValue(const std::string& lhVariable, std::vector<double>& value)
{
    ATH_MSG_DEBUG("Trying to set vector LH variable: " << lhVariable);
    m_lhVariableValues[lhVariable]=value;
}

void LikelihoodTool::setLhVariableValue(const std::string& lhVariable, double value)
{
    std::vector<double> tmpVector;
    tmpVector.push_back(value);
    setLhVariableValue(lhVariable, tmpVector);
}

std::vector<double> LikelihoodTool::calculateLikelihood()
{
/**
Running on all Hbb, Huu, tth, ttbar events we have:

There are 1537821 jets.
There are 680153 b jets. 447493 have a sec vtx (65.793%)
There are 607057 light jets. 17715 have a sec vtx (2.91818%)
*/
    double eb(0.65793);
    double eu(0.0291818);
    bool calcLHForSecVtx(false);
    m_likelihoodVector.clear();
    std::vector<double> probDensityPerEventClassAllVariables;
    probDensityPerEventClassAllVariables.resize(m_mapOfAllHistos.size());
    for (unsigned int i = 0 ; i < m_mapOfAllHistos.size(); ++i)
    {
      probDensityPerEventClassAllVariables[i]=1.;
    }
    /// loop over the different variables
    for (std::vector<std::string>::iterator itr1 = m_useTheseLhVariables.begin() ;
            itr1 != m_useTheseLhVariables.end()  ; ++itr1 )
    {
        std::vector<double>& thisVariable = (*(m_lhVariableValues.find(*itr1))).second;

        // if weight for sec vtx has to be calculated behave differently
        if ((*itr1)=="vtxmassBU")
        {
          calcLHForSecVtx=true;
          if (thisVariable[0] == -1.) // no vertex
          {
	    ATH_MSG_VERBOSE("SecVtxTag had no SecVtx - returning " << (1.-eb)/(2.-eb-eu));
            probDensityPerEventClassAllVariables[0] = (1.-eb)/(2.-eb-eu);
            probDensityPerEventClassAllVariables[1] = (1.-eu)/(2.-eb-eu);
//             mlog << MSG::VERBOSE <<  "Signal LH is: "
//                                  << probDensityPerEventClassAllVariables[0]/(probDensityPerEventClassAllVariables[0]+probDensityPerEventClassAllVariables[1]) 
//                                  << "\tBackground LH is: "
//                                  << probDensityPerEventClassAllVariables[1]/(probDensityPerEventClassAllVariables[0]+probDensityPerEventClassAllVariables[1]) << endreq;
            m_likelihoodVector=probDensityPerEventClassAllVariables;
            return m_likelihoodVector;
          }
        }
        // get HistoLimits for this variable
        const HistoLimits& histoLimits = (*(m_histoLimitsMap.find(*itr1))).second;
	ATH_MSG_VERBOSE("Variable: " << (*itr1) << " appears " << thisVariable.size() << " times. Histolimits: (" << histoLimits.xmin << ", " << histoLimits.xmax << ").");
        // variable might have more than one entry per jet/event
        for (std::vector<double>::iterator varItr  = thisVariable.begin();
                                           varItr != thisVariable.end(); ++varItr )
        {
	  double sum(0.);
	  double value(*varItr);
	  ATH_MSG_VERBOSE("Variable value is: " << value);
	  if (value >= histoLimits.xmax)
            {
	      ATH_MSG_VERBOSE("Variable was too high("<<value<<"), set it to: " << histoLimits.xmax - 0.0001);
              value = histoLimits.xmax - 0.0001;
            } else if (value <= histoLimits.xmin) {
	      ATH_MSG_VERBOSE("Variable was too low("<<value<<"), set it to: " << histoLimits.xmax + 0.0001);
              value = histoLimits.xmin + 0.0001;
            }

            std::vector<double> tmpVector;
            /// loop over the different input histogram sets
            unsigned int inputFilesCounter(1);
            for (std::vector< std::map<std::string, TH1*>* >::iterator
                    itr2 = m_mapOfAllHistos.begin() ;
                    itr2 != m_mapOfAllHistos.end() ;
                    ++itr2 )
            {
	      ATH_MSG_VERBOSE("Inputfile " << inputFilesCounter << ": I try to find histo: " << *itr1);
	      TH1* tmpHisto = (*((*itr2)->find(*itr1))).second;

	      if(tmpHisto == 0) ATH_MSG_VERBOSE("tmpHisto is ZERO!!");
	      // get the bin only once
	      // it will be the same for all input files in m_mapOfAllHistos
	      int bin((tmpHisto->GetXaxis())->FindBin(value));
	      double tmp(tmpHisto->GetBinContent(bin));
	      ATH_MSG_VERBOSE("Value " << value << " is in bin " << bin);
	      ATH_MSG_VERBOSE("The histo height of this bin is: " << tmp);
	      tmpVector.push_back(tmp);
	      sum += tmp;
	      inputFilesCounter++;
            }

	    ATH_MSG_VERBOSE("The sum of the histo heights : " << sum);

            unsigned int classCount(0);
            for (std::vector<double>::iterator itr3 = tmpVector.begin();  itr3 != tmpVector.end(); ++itr3)
            {
                probDensityPerEventClassAllVariables[classCount] *= (*itr3) /sum;
                classCount++;
            }
////// debug output
        }
    }

    // if likelihood for sec vtx has to be calculated behave differently
    if (calcLHForSecVtx)
    {
//         mlog << MSG::VERBOSE <<  "SecVtxTag had SecVtx - returning (without sec vtx taken into account): "
//                              << probDensityPerEventClassAllVariables[0] << ", "
//                              << probDensityPerEventClassAllVariables[1] << endreq;
//         mlog << MSG::VERBOSE <<  "Signal LH would be: "
//                              << probDensityPerEventClassAllVariables[0]/(probDensityPerEventClassAllVariables[0]+probDensityPerEventClassAllVariables[1]) 
//                              << "\tBackground LH would be: "
//                              << probDensityPerEventClassAllVariables[1]/(probDensityPerEventClassAllVariables[0]+probDensityPerEventClassAllVariables[1]) << endreq;
        probDensityPerEventClassAllVariables[0] *= eb;
        probDensityPerEventClassAllVariables[1] *= eu;
	ATH_MSG_VERBOSE("SecVtxTag had SecVtx - returning: " << probDensityPerEventClassAllVariables[0] << ", " << probDensityPerEventClassAllVariables[1]);
	ATH_MSG_VERBOSE("Signal LH would be: " << probDensityPerEventClassAllVariables[0]/(probDensityPerEventClassAllVariables[0]+probDensityPerEventClassAllVariables[1]) << "\tBackground LH would be: " << probDensityPerEventClassAllVariables[1]/(probDensityPerEventClassAllVariables[0]+probDensityPerEventClassAllVariables[1]));
    }
//     for (std::vector<double>::iterator itr4 = probDensityPerEventClassAllVariables.begin();  itr4 != probDensityPerEventClassAllVariables.end(); ++itr4)
//     {
//       mlog << MSG::VERBOSE <<  "Pj(x1,...,xn) = " << *itr4 << endreq;
//     }
    m_likelihoodVector=probDensityPerEventClassAllVariables;
    return m_likelihoodVector;
}

std::vector<double> LikelihoodTool::tagLikelihood()
{
  return m_likelihoodVector;
}

double LikelihoodTool::calculateWeight()
{
    double weight(0.);
/**
Running on all Hbb, Huu, tth, ttbar events we have:

There are 1537821 jets.
There are 680153 b jets. 447493 have a sec vtx (65.793%)
There are 607057 light jets. 17715 have a sec vtx (2.91818%)
*/
    double eb(0.65793);
    double eu(0.0291818);
    double addToWeight(0.); // this is added to the weight in case of secvtx finder

    /// loop over the different variables
    for (std::vector<std::string>::iterator itr1 = m_useTheseLhVariables.begin() ;
            itr1 != m_useTheseLhVariables.end()  ; ++itr1 )
    {
        std::vector<double>& thisVariable = (*(m_lhVariableValues.find(*itr1))).second;

        // if weight for sec vtx has to be calculated behave differently
        if ((*itr1)=="vtxmassBU")
        {
          if (thisVariable[0] == -1.) // no vertex
          {
	    ATH_MSG_VERBOSE("SecVtxTag had no SecVtx - returning " << log((1-eb)/(1-eu)) );
            return log((1-eb)/(1-eu));
          } else
          {
            addToWeight = log(eb/eu);
	    ATH_MSG_VERBOSE("SecVtxTag had SecVtx - adding " << addToWeight << " to final weight");
          }
        }

        // get HistoLimits for this variable
        const HistoLimits& histoLimits = (*(m_histoLimitsMap.find(*itr1))).second;
	ATH_MSG_VERBOSE("Variable: " << (*itr1) << " appears " << thisVariable.size() << " times. Histolimits: (" << histoLimits.xmin << ", " << histoLimits.xmax << ").");
        // variable might have more than one entry per jet/event
        for (std::vector<double>::iterator varItr  = thisVariable.begin();
                                           varItr != thisVariable.end(); ++varItr )
        {
            double value(*varItr);
//             mlog << MSG::VERBOSE <<  "Variable value is: " << value << endreq;
            if (value >= histoLimits.xmax)
            {
	      ATH_MSG_VERBOSE("Variable was too high("<<value<<"), set it to: " << histoLimits.xmax - 0.0001 );
              value = histoLimits.xmax - 0.0001;
            } else if (value <= histoLimits.xmin) {
	      ATH_MSG_VERBOSE("Variable was too low("<<value<<"), set it to: " << histoLimits.xmax + 0.0001);
              value = histoLimits.xmin + 0.0001;
            }

            std::vector<double> tmpVector;
            /// loop over the different input histogram sets
            unsigned int inputFilesCounter(1);
            for (std::vector< std::map<std::string, TH1*>* >::iterator
                    itr2 = m_mapOfAllHistos.begin() ;itr2 != m_mapOfAllHistos.end() ;++itr2 )
            {
//                 mlog << MSG::VERBOSE <<  "Inputfile " << inputFilesCounter << ": I try to find histo: " << *itr1 << endreq;
	      TH1* tmpHisto = (*((*itr2)->find(*itr1))).second;
	      
	      if (tmpHisto == 0) ATH_MSG_VERBOSE("tmpHisto is ZERO!!" );
	      // get the bin only once
	      // it will be the same for all input files in m_mapOfAllHistos
	      int bin((tmpHisto->GetXaxis())->FindBin(value));
	      double tmp(tmpHisto->GetBinContent(bin));
	      //                 mlog << MSG::VERBOSE <<  "Value " << value << " is in bin " << bin << endreq;
	      //                 mlog << MSG::VERBOSE <<  "The histo height of this bin is: " << tmp << endreq;
	      tmpVector.push_back(tmp);
	      inputFilesCounter++;
            }
            weight += log(tmpVector[0]/tmpVector[1]);
//             mlog << MSG::VERBOSE <<  "The sum of the histo heights : " << sum << endreq;
        }
    }
    ATH_MSG_VERBOSE("Returning : " << weight << " + " << addToWeight );
    return (weight + addToWeight);
}

void LikelihoodTool::printStatus()
{
  ATH_MSG_INFO("Currently I hold histos of " <<m_mapOfAllHistos.size() << " input files");
}

}
