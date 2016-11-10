/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETGoodness/METCollisionFilter.h"
//#include "MissingETGoodness/Goodies.h"
#include "PrimaryDPDMaker/CaloTimeFilterTool.h"
#include "PrimaryDPDMaker/MBTSTimeFilterTool.h"
#include "GoodRunsLists/IGoodRunsListSelectorTool.h"
#include "GaudiKernel/SystemOfUnits.h"


using CLHEP::GeV;
using CLHEP::ns;


// MB: reference to MET Goodies map for storing derived quantities
MET::Goodies& METCollisionFilter::s_goodies(MET::Goodies::instance());

METCollisionFilter::METCollisionFilter(const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name, pSvcLocator)
 , m_counter(0)
 , m_caloaccept(0)
 , m_mbtsaccept(0)
 , m_caloclusteraccept(0)
 , m_overlap(0)
 , m_doLBFilter(true)
 , m_doCaloTimeFilter(false)
 , m_doCaloClusterTimeFilter(false)
 , m_doMBTSTimeFilter(false)
 , m_calofilter("CaloTimeFilterTool")
 , m_mbtsfilter("MBTSTimeFilterTool")
 , m_GoodRunsListSelectorTool("GoodRunsListSelectorTool")
{
  declareProperty( "CounterPrescale",  m_prescale = 1000 );
  // the cluster timing can also be used on AODs!
  declareProperty( "DoCaloTimeFilter", m_doCaloTimeFilter = true );
  declareProperty( "DoCaloClusterTimeFilter", m_doCaloClusterTimeFilter = true );
  declareProperty( "DoMBTSTimeFilter", m_doMBTSTimeFilter = true );
  declareProperty( "DoLBFilter", m_doLBFilter = true );

  declareProperty( "Prefix", m_prefix );
  declareProperty( "GoodRunsListArray", m_grlnameVec );
  declareProperty( "BlackRunsListArray", m_brlnameVec );

  declareProperty( "clusterTimeMinEta", m_clusterMinEta = 1.);
  declareProperty( "clusterTimeMinE", m_clusterMinE = 1.*GeV);
  declareProperty( "clusterTimeMean", m_clusterTimeMean = 0.*ns);
  declareProperty( "clusterTimeMeanDelta", m_clusterTimeMeanDelta = 10.*ns);
  declareProperty( "clusterTimeACDelta", m_clusterTimeACDelta = 10.*ns);
}


METCollisionFilter::~METCollisionFilter()
{ 
} 


StatusCode METCollisionFilter::initialize() 
{
  ATH_MSG_DEBUG ("initialize()");

  /// Retrieve the CaloTimeFilter tool using the ToolHandles
  if (m_doCaloTimeFilter) {
    if ( m_calofilter.retrieve().isFailure() ) {
      ATH_MSG_FATAL
        (m_calofilter.propertyName() << ": Failed to retrieve tool "
         << m_calofilter.type());
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG
        (m_calofilter.propertyName() << ": Retrieved tool " 
         << m_calofilter.type());
    }
  }

  /// Retrieve the mbts timefilter tool using the ToolHandles
  if (m_doMBTSTimeFilter) {
    if ( m_mbtsfilter.retrieve().isFailure() ) {
      ATH_MSG_FATAL
        (m_mbtsfilter.propertyName() << ": Failed to retrieve tool "
         << m_mbtsfilter.type());
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG
        (m_mbtsfilter.propertyName() << ": Retrieved tool " 
         << m_mbtsfilter.type());
    }
  }

  /// Retrieve the GoodRunsListSelector tool using the ToolHandles
  if (m_doLBFilter) {
    if ( m_GoodRunsListSelectorTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL 
        (m_GoodRunsListSelectorTool.propertyName() << ": Failed to retrieve tool "
         << m_GoodRunsListSelectorTool.type());
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG
        (m_GoodRunsListSelectorTool.propertyName() << ": Retrieved tool " 
         << m_GoodRunsListSelectorTool.type());
    }
  }

  ATH_MSG_DEBUG ("initialize() successful");

  return StatusCode::SUCCESS;
}


StatusCode 
METCollisionFilter::execute() 
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ("execute()");

  if (m_counter % m_prescale == 0)
    ATH_MSG_VERBOSE ("Now processing event : " << m_counter);
  m_counter++ ;

  if (!m_doCaloTimeFilter && !m_doMBTSTimeFilter && !m_doLBFilter &&
      !m_doCaloClusterTimeFilter)
    return sc;

  //////////////////////////////////////////////

  bool pass(false), passCalo(false), passCaloCluster(false), passMBTS(false), passLB(false);
  double timeDiff(MET::MagicNumber), timeA(MET::MagicNumber), timeC(MET::MagicNumber);
  int countA(MET::MagicNumber), countC(MET::MagicNumber);

  //////////////////////////////////////////////  

  if (m_doCaloTimeFilter) {
    sc = m_calofilter->getTimeDifference(passCalo,timeDiff,timeA,timeC,countA,countC);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING (m_calofilter.propertyName() << ": Failed to execute tool " << m_calofilter.type());
      return StatusCode::FAILURE;
    }
    if (passCalo) {
      ATH_MSG_DEBUG ("Event accepted by CaloTimeFilter.");
      ++m_caloaccept;
    } else 
      ATH_MSG_DEBUG ("Event rejected by CaloTimeFilter.");

    s_goodies.setValue("Calo_pass",     static_cast<int>(passCalo));
    s_goodies.setValue("Calo_timeDiff", timeDiff);
    s_goodies.setValue("Calo_timeA",    timeA);
    s_goodies.setValue("Calo_timeC",    timeC);
    s_goodies.setValue("Calo_countA",   countA);
    s_goodies.setValue("Calo_countC",   countC);
  }

  if (m_doCaloClusterTimeFilter) {
    sc = doClusterTimes(passCaloCluster);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING ("Failed to get cluster times");
      return StatusCode::FAILURE;
    }
    if (passCaloCluster) {
      ATH_MSG_DEBUG ("Event accepted by CaloClusterTimeFilter.");
      ++m_caloclusteraccept;
    } else {
      ATH_MSG_DEBUG ("Event rejected by CaloClusterTimeFilter.");
    }
  }

  //////////////////////////////////////////////  

  if (m_doMBTSTimeFilter) {
    sc = m_mbtsfilter->getTimeDifference(passMBTS,timeDiff,timeA,timeC,countA,countC);
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING (m_mbtsfilter.propertyName() << ": Failed to execute tool " << m_mbtsfilter.type());
      return StatusCode::FAILURE;
    }
    if (passMBTS) { 
      ATH_MSG_DEBUG ("Event accepted by MBTSTimeFilter.");
      ++m_mbtsaccept;
    } else
      ATH_MSG_DEBUG ("Event rejected by MBTSTimeFilter.");

    s_goodies.setValue("MBTS_pass",     static_cast<int>(passMBTS));
    s_goodies.setValue("MBTS_timeDiff", timeDiff);
    s_goodies.setValue("MBTS_timeA",    timeA);
    s_goodies.setValue("MBTS_timeC",    timeC);
    s_goodies.setValue("MBTS_countA",   countA);
    s_goodies.setValue("MBTS_countC",   countC);
  }

  //////////////////////////////////////////////  

  if (m_doLBFilter) {
    passLB = m_GoodRunsListSelectorTool->passThisRunLB(m_grlnameVec,m_brlnameVec);
    s_goodies.setValue(m_prefix+"OkayLB", static_cast<int>(passLB));
  }

  //////////////////////////////////////////////

  if (m_doCaloTimeFilter && m_doMBTSTimeFilter) {
    pass = passCalo && passMBTS;
    ATH_MSG_DEBUG ("Event accepted as collision ? " << pass);
    //this->setFilterPassed (pass); // This skips the execution of following algs for this event
    s_goodies.setValue(m_prefix+"IsCollisionCandidate",static_cast<int>(pass));
    if (pass) ++m_overlap;
  }

  //////////////////////////////////////////////  

  //s_goodies.print();

  ATH_MSG_DEBUG ("execute() successful");

  return sc;
}


StatusCode METCollisionFilter::finalize() 
{
  ATH_MSG_DEBUG ("finalize()");
  if (m_doCaloTimeFilter) ATH_MSG_INFO ("CaloTimeFilter     : Processed " << m_counter << " events, accepted " << m_caloaccept << " events.");
  if (m_doMBTSTimeFilter) ATH_MSG_INFO ("MBTSTimeFilter     : Processed " << m_counter << " events, accepted " << m_mbtsaccept << " events.");

  if (m_doCaloTimeFilter && m_doMBTSTimeFilter)
    ATH_MSG_INFO ("Overlapping accept : " << m_overlap << " events.");

  ATH_MSG_DEBUG ("finalize() successful");

  return StatusCode::SUCCESS;
}

StatusCode
METCollisionFilter::doClusterTimes(bool& passCutForwardBackward)
{
   //.............................................
   ATH_MSG_DEBUG("ClusterTimeFilter execute()");
   
   passCutForwardBackward = true;
   bool passCutUpDown = true;
   double timeDiffForwardBackward = MET::MagicNumber;
   double timeDiffUpDown = MET::MagicNumber;
   double timeA = MET::MagicNumber;
   double timeC = MET::MagicNumber;
   double totalClusterEnergy = MET::MagicNumber;
   double ootClusterEnergy10 = MET::MagicNumber;
   double ootClusterEnergy15 = MET::MagicNumber;
   double ootClusterEnergy20 = MET::MagicNumber;
   double ootClusterEnergy25 = MET::MagicNumber;
   
   const CaloClusterContainer* clusters;   
   if ( evtStore()->retrieve(clusters,"CaloCalTopoCluster").isFailure() ) {
      if (msgLvl(MSG::INFO)) {
	 msg(MSG::INFO)
	    << " Could not get pointer to CaloClusterContainer "
	    << endmsg;
      }
      return StatusCode::SUCCESS;
   }

   // positive eta is side A, negative eta is side C
   double sumMinus = 0.0;
   double sumWeightMinus = 0.0;
   double sumPlus = 0.0;
   double sumWeightPlus = 0.0;
   double sumUp = 0.0;
   double sumWeightUp = 0.0;
   double sumDown = 0.0;
   double sumWeightDown = 0.0;
   double eventTime = MET::MagicNumber;
   double eventTimeWeight = 0.;
   CaloClusterContainer::const_iterator first = clusters->begin();
   CaloClusterContainer::const_iterator last = clusters->end();
   for(; first != last; ++first) {
      const CaloCluster* clus = *first;
      // now that there is really a cluster with energy, reset the values
      // to zero
      if(std::fabs(totalClusterEnergy - MET::MagicNumber)<1e-5 ) {
	 totalClusterEnergy = 0.;
	 ootClusterEnergy10 = 0.;
	 ootClusterEnergy15 = 0.;
	 ootClusterEnergy20 = 0.;
	 ootClusterEnergy25 = 0.;
      }
      totalClusterEnergy += clus->getBasicEnergy();
      // what's the right cut value here?
      if(clus->getBasicEnergy() > m_clusterMinE && clus->getTime() != 0 ) {

	 if( std::fabs(clus->getTime() - m_clusterTimeMean) > m_clusterTimeMeanDelta ) ootClusterEnergy10 += clus->getBasicEnergy();
	 if( std::fabs(clus->getTime() - m_clusterTimeMean) > 15. ) ootClusterEnergy15 += clus->getBasicEnergy();
	 if( std::fabs(clus->getTime() - m_clusterTimeMean) > 20. ) ootClusterEnergy20 += clus->getBasicEnergy();
	 if( std::fabs(clus->getTime() - m_clusterTimeMean) > 25. ) ootClusterEnergy25 += clus->getBasicEnergy();

	 eventTime += std::fabs(clus->getBasicEnergy())* clus->getTime();
	 eventTimeWeight += std::fabs(clus->getBasicEnergy());
	 if(std::fabs(clus->eta()) > m_clusterMinEta) {
	    if(clus->eta() > 0.) {
	       sumPlus += std::fabs(clus->getBasicEnergy()) * clus->getTime();
	       sumWeightPlus += std::fabs(clus->getBasicEnergy());
	    } else {
	       sumMinus += std::fabs(clus->getBasicEnergy()) * clus->getTime();
	       sumWeightMinus += std::fabs(clus->getBasicEnergy());
	    }
	 }
	 if(clus->phi() > 0.) {
	    sumUp += std::fabs(clus->getBasicEnergy()) * clus->getTime();
	    sumWeightUp += std::fabs(clus->getBasicEnergy());
	 } else {
	    sumDown += std::fabs(clus->getBasicEnergy()) * clus->getTime();
	    sumWeightDown += std::fabs(clus->getBasicEnergy());
	 }
      }
   }
   // now average weighted event time
   if(eventTimeWeight > 0.) eventTime /= eventTimeWeight;

   // now forward/backward time diff against beam backgrounds
   if(sumWeightPlus > 0.){   
      sumPlus /= sumWeightPlus;
      timeA = sumPlus;
   }
   if(sumWeightMinus > 0.){   
      sumMinus /= sumWeightMinus;
      timeC = sumMinus;
   }   
   if(sumWeightPlus > 0. and sumWeightMinus > 0.){   
      timeDiffForwardBackward = timeA - timeC;
      if ( std::fabs(timeA - m_clusterTimeMean) > m_clusterTimeMeanDelta or
	   std::fabs(timeC - m_clusterTimeMean) > m_clusterTimeMeanDelta or
	   std::fabs(timeDiffForwardBackward) > m_clusterTimeACDelta ) {
	 passCutForwardBackward = false;
      }
   }   
   // now up/down time diff against cosmics
   if(sumWeightUp > 0.) sumUp /= sumWeightUp;
   if(sumWeightDown > 0.) sumDown /= sumWeightDown;
   if(sumWeightUp > 0. and sumWeightDown > 0.){   
      timeDiffUpDown = sumUp - sumDown;
      if(std::fabs(sumUp - m_clusterTimeMean) > m_clusterTimeMeanDelta or
	 std::fabs(sumDown - m_clusterTimeMean) > m_clusterTimeMeanDelta or
	 std::fabs(timeDiffUpDown) > m_clusterTimeACDelta) {
	 passCutUpDown = false;
      }
   }

   s_goodies.setValue("CalClus_eventTime", eventTime);   
   s_goodies.setValue("CalClusForwBackw_pass", static_cast<int>(passCutForwardBackward));
   s_goodies.setValue("CalClusForwBackw_timeDiff", timeDiffForwardBackward);
   s_goodies.setValue("CalClus_timeA",    timeA);
   s_goodies.setValue("CalClus_timeC",    timeC);
   s_goodies.setValue("CalClusUpDown_pass",     static_cast<int>(passCutUpDown));
   s_goodies.setValue("CalClusUpDown_timeDiff", timeDiffUpDown);

   if(totalClusterEnergy > 0.) s_goodies.setValue("CalClus_InTimeFraction", 1.-ootClusterEnergy10 / totalClusterEnergy);
   else s_goodies.setValue("CalClus_InTimeFraction", 1.);
   s_goodies.setValue("CalClus_OutTimeEnergy",ootClusterEnergy10);

   if(totalClusterEnergy > 0.) s_goodies.setValue("CalClus_ootFraction10", ootClusterEnergy10 / totalClusterEnergy);
   else s_goodies.setValue("CalClus_ootFraction10", 0.);
   if(totalClusterEnergy > 0.) s_goodies.setValue("CalClus_ootFraction15", ootClusterEnergy15 / totalClusterEnergy);
   else s_goodies.setValue("CalClus_ootFraction15", 0.);
   if(totalClusterEnergy > 0.) s_goodies.setValue("CalClus_ootFraction20", ootClusterEnergy20 / totalClusterEnergy);
   else s_goodies.setValue("CalClus_ootFraction20", 0.);
   if(totalClusterEnergy > 0.) s_goodies.setValue("CalClus_ootFraction25", ootClusterEnergy25 / totalClusterEnergy);
   else s_goodies.setValue("CalClus_ootFraction25", 0.);

   s_goodies.setValue("CalClus_ootEnergy10",ootClusterEnergy10);
   s_goodies.setValue("CalClus_ootEnergy15",ootClusterEnergy15);
   s_goodies.setValue("CalClus_ootEnergy20",ootClusterEnergy20);
   s_goodies.setValue("CalClus_ootEnergy25",ootClusterEnergy25);

   return StatusCode::SUCCESS;
}
