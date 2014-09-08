/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetMomentFillerTool.cxx
 * @author Pier-Olivier DeViveiros < viveiros AT physics.utoronto.ca >
 * @date October, 2009
 * @brief Block filler tool for Jet Moments
 */


#include "JetMomentFillerTool.h"
#include "checkJet.h"
#include "JetEvent/Jet.h"
//#include "JetUtils/JetCaloQualityUtils.h"
#include "JetEvent/JetConstituentIterator.h"
#include "CaloEvent/CaloCluster.h"
#include "AthenaKernel/errorcheck.h"
#include "boost/tokenizer.hpp"
#include "boost/foreach.hpp"


namespace {


double jetOutOfTimeEnergyFraction(const Jet* jet, const double timecut, const bool doOnlyPos) {
  double sumE=0;
  double sumE_OOT=0;
  
  NavigationToken<CaloCell,double> cellToken;
  jet->fillToken(cellToken,double(1.));
  
  if ( cellToken.size() > 0 ) {
    NavigationToken<CaloCell,double>::const_iterator firstCell =  cellToken.begin();
    NavigationToken<CaloCell,double>::const_iterator lastCell  =  cellToken.end();
    for(; firstCell != lastCell; ++firstCell)	{
      // weight is not used
      // double weight = firstCell.getParameter();
      const CaloCell* thisCell = *firstCell;

      double  aCellE = thisCell->e();

      if(doOnlyPos && aCellE<0) continue;

      sumE += aCellE;
      
      // ensure timing information is defined
      bool timeIsDefined(false);
      
      if(thisCell->caloDDE()->is_tile()) {
        if ( (thisCell->provenance()) & 0x8080)  timeIsDefined = true;
      } else { //is_lar	    
        if ( (thisCell->provenance()) & 0x2000)  timeIsDefined = true;
      }

      if ( timeIsDefined ) {
        double time = thisCell->time();
        if(fabs(time)>timecut)  sumE_OOT+=thisCell->e();        
      }
    }
  }    

  return ( (sumE!=0) ? (sumE_OOT/sumE) : 0 );
}


// Calculate the fraction of jet energy from clusters with fabs(time)>timecut
// returns -1 if sum of cluster energy is zero and oot energy fraction otherwise
double jetOutOfTimeEnergyFractionClusters(const Jet* jet, const double timecut, const bool doOnlyPos) {
  if(dynamic_cast<const Jet*>(jet) == NULL) return -1;
  
  double sum_all(0), sum_time(0);

  NavigationToken<CaloCluster,double> clusToken;
  jet->fillToken(clusToken,double(1.));
  
  if ( clusToken.size() > 0 ) {
    // Use the constituent iterator
    JetConstituentIterator firstClus = JetConstituentIterator::first(jet);
    JetConstituentIterator lastClus  = JetConstituentIterator::last(jet);
      
    for(; firstClus != lastClus; ++firstClus) {
	    const CaloCluster* theClus(NULL); theClus = dynamic_cast<const CaloCluster*>(firstClus.get_real_constit());
      if(!theClus) continue;

      double aClusterE    = theClus->hlv(P4SignalState::UNCALIBRATED).e();
      double aClusterTime = theClus->getTime();

      if(doOnlyPos && aClusterE<0) continue;

      sum_all += aClusterE;
      if(fabs(aClusterTime) > timecut) sum_time += aClusterE;
    }  
  }

  double ootEngyFrac = (fabs(sum_all)>0) ? sum_time/sum_all : -1;
  return ootEngyFrac;
}


} // anonymous namespace



namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetMomentFillerTool::JetMomentFillerTool (const std::string& type,
		    const std::string& name,
		    const IInterface* parent)
  : BlockFillerTool<Jet> (type, name, parent),
    m_momentList()
{
  declareProperty
    ("Moments", m_momentList,
     "List of moments to write.  Each entry is in general of the form "
     "VAR=MOM:DOC, where VAR is the D3PD variable name, MOM is the moment "
     "name, and DOC is the documentation string. The :DOC portion may be "
     "omitted.  MOM may be a comma-separated list of moment names; "
     "in this case, we search for the first moment name that exists.  "
     "If the VAR= part is omitted, then the first name in MOM is used "
     "for the D3PD variable name.");
  declareProperty("timingMomentsOnlyPosE", m_timingMomentsOnlyPosE=false);
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetMomentFillerTool::book()
{
  m_momentValues.resize(m_momentList.size());

  for(unsigned int i=0; i<m_momentList.size(); i++) {
    std::string docstring;
    std::string::size_type ipos = m_momentList[i].find(':');
    if (ipos != std::string::npos) {
      docstring = m_momentList[i].substr (ipos+1, std::string::npos);
      m_momentList[i].erase (ipos, std::string::npos);
    }
    std::string varname = m_momentList[i];
    ipos = varname.find ('=');
    if (ipos != std::string::npos) {
      varname.erase (ipos, std::string::npos);
      m_momentList[i].erase (0, ipos+1);
    }
    else {
      ipos = varname.find (',');
      if (ipos != std::string::npos) {
        varname.erase (ipos, std::string::npos);
      }
    }
    CHECK( addVariable (varname,  m_momentValues[i], docstring)  );
  }
 
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode JetMomentFillerTool::fill(const Jet& p)
{
  std::vector<Jet::mkey_t> moment_keys;

  for(unsigned int i=0; i<m_momentList.size(); i++)
    {
      std::string::size_type ipos = m_momentList[i].find (',');
      if (ipos != std::string::npos) {
        if (moment_keys.empty())
          moment_keys = p.getMomentKeys();
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep(" ,");
        tokenizer tokens(m_momentList[i], sep);
        BOOST_FOREACH (const std::string& s, tokens) {
          m_momentList[i] = s;
          if (std::find ( moment_keys.begin(), moment_keys.end(), s) !=
              moment_keys.end())
          {
            break;
          }
        }
      }

      std::string momentNameNow = m_momentList[i];
      double      momentValNow  = p.getMoment(momentNameNow);

      // in the case where the out-of-time fraction moments have not been calculated beforehand, force the calculation
      std::string ootMomentName = "ootFrac";
      if(momentNameNow.find(ootMomentName) != std::string::npos) {
        if(fabs(momentValNow)<1e-50)
          momentValNow = calculateOotFracMoment(&p,momentNameNow);
        else
          if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Out-of-time cluster/cell moemnt already computed: " << momentNameNow <<"\t"<< momentValNow << endreq;
      }
      *m_momentValues[i]  = momentValNow;
    }
  return StatusCode::SUCCESS;
}


/**
 * @brief in the case where the out-of-time fraction moments have not been calculated beforehand, do the calculation
 * @param p The input object, momentName the name of the moment (of the form ootFracClusters5 or ootFracCells5).
 *
 * The format of the name of the moments is in accordance with
 * JetCaloQualityTool.cxx from Reconstruction/Jet/JetMomentTools (from JetMomentTools-00-00-52)
 */
double JetMomentFillerTool::calculateOotFracMoment(const Jet* jet, std::string momentName)
{
  if (!D3PD::checkJet (*jet)) return -999;

  double  momentVal = 0.;

  std::string clusterName = "Clusters";  int clusterNameL = clusterName.length();
  std::string cellName    = "Cells";     int cellNameL    = cellName.length();

  // if calculating cluster moment such as with name ootFracClusters5
  if(momentName.find(clusterName) != std::string::npos) {
    // find the time cut from the name of the moment
    std::string timeCutStr = momentName.substr(momentName.find(clusterName)+clusterNameL);
    double      timeCut    = atoi( timeCutStr.c_str() );

    momentVal = jetOutOfTimeEnergyFractionClusters(jet,timeCut,m_timingMomentsOnlyPosE);

    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Compute out-of-time cluster energy fraction moment: "
                                           << momentName <<" \twith time-cut " << timeCut << " : "<< momentVal << endreq;
  }

  // if calculating cell moment such as with name ootFracCells5
  if(momentName.find(cellName) != std::string::npos) {
    // find the time cut from the name of the moment
    std::string timeCutStr = momentName.substr(momentName.find(cellName)+cellNameL);
    double      timeCut    = atoi( timeCutStr.c_str() );

    momentVal = jetOutOfTimeEnergyFraction(jet,timeCut,m_timingMomentsOnlyPosE);

    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Compute out-of-time cell energy fraction moment: "
                                           << momentName <<" \twith time-cut " << timeCut << " : "<< momentVal << endreq;
  }

  return  momentVal;
}


} // namespace D3PD
