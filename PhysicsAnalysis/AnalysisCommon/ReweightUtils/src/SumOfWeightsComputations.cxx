/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReweightUtils includes
#include "ReweightUtils/SumOfWeightsComputations.h"
#include "ReweightUtils/WeightToolBase.h"

#include "TString.h"

// linux i/o includes
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>


using std::string;

//**********************************************************************

SumOfWeightsTool::SumOfWeightsTool(const std::string& name, ISvcLocator* pSvcLocator ) : 
  ::AthAnalysisAlgorithm( name, pSvcLocator ), 
  m_ofname("sumOfWeights.txt"),
  m_dumpTxtFile(0),
  m_maxMuBins(50),
  m_CBKcontainerName("SumOfWeights"),
  m_CBKnamePrefix("SumOfWeights_") {
  declareProperty( "WeightTools", m_weightTools);
  declareProperty( "OutputTxtFile", m_ofname);
  declareProperty( "DumpTxtFile", m_dumpTxtFile);
  declareProperty( "maxMuBins", m_maxMuBins);
  declareProperty( "CutBookKeeperContainterName", m_CBKcontainerName);
  declareProperty( "CutBookKeeperNamePrefix", m_CBKnamePrefix);
}

//**********************************************************************

SumOfWeightsTool::~SumOfWeightsTool() { }

//**********************************************************************

StatusCode SumOfWeightsTool::initialize() {
  StatusCode sc;
  ATH_MSG_INFO("Initializing " << name() << "...");
  ATH_MSG_INFO("Retrieving tools...");
  sc = m_weightTools.retrieve();
  if ( ! sc.isSuccess() ) {
    ATH_MSG_ERROR("Tool retrieval failed.");
    return sc;
  }
  ATH_MSG_INFO("Tool retrieval completed.");
  unsigned int ntool = m_weightTools.size();
  ATH_MSG_INFO("  Tool count: " << ntool);
  for ( unsigned int itool=0; itool<ntool; ++itool ) {
    ATH_MSG_INFO("    " << m_weightTools[itool]->name());
    m_weightTools[itool]->print();
    // Get the tool's message property:
    const WeightToolBase* tool = dynamic_cast< const WeightToolBase* >( m_weightTools[ itool ].operator->() );
    if( ! tool ) {
       ATH_MSG_ERROR( "The received tool is not an WeightToolBase?!?" );
       return StatusCode::FAILURE;
    }
  }

  //Initialize the different sumOfWeights (last entry will contain the unweighted counts)
  for (unsigned int iT=0; iT<=ntool; iT++) {
    //Will store the total sumOfWeights corresponding to each of the weight computation requested
    m_TotalSumOfWeights.push_back(0.);
    m_TotalSumOfSquaredWeights.push_back(0.);
    //Will store the SumOfWeights in 50 bins of mu (i.e. averageIntPerXing) for each of the weight computation requested
    std::vector<float> tmpVec;
    for (unsigned int iM=0; iM<m_maxMuBins; iM++)
      tmpVec.push_back(0.);
    m_SumOfWeightsPerMu.push_back(tmpVec);
    m_SumOfSquaredWeightsPerMu.push_back(tmpVec);
  }

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode SumOfWeightsTool::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << ", will loop over WeightTools...");

  const xAOD::EventInfo* eventInfo;
  StatusCode sc = evtStore()->retrieve(eventInfo);
  if (! sc.isSuccess() ) {
    ATH_MSG_ERROR( " Event info not found in event store.");
    return sc;
  }

  //Getting the pile-up mu
  unsigned int mu = eventInfo->averageInteractionsPerCrossing();
  if (mu>m_maxMuBins)
    mu=m_maxMuBins-1;

  // Loop over weight tools.
  string line = "---------------------------------------------------";
  ATH_MSG_DEBUG(line);
  unsigned int itool=0;
  for ( ToolHandleArray<IWeightTool>::const_iterator itoo=m_weightTools.begin(); itoo!=m_weightTools.end(); ++itoo ) {
    ToolHandle<IWeightTool> htool = *itoo;
    //Increase the total sumOfWeights for this weight computation
    float weight = htool->getWeight();
    m_TotalSumOfWeights[itool] += weight;
    m_TotalSumOfSquaredWeights[itool] += weight*weight;
    ATH_MSG_DEBUG("   got weight = " << weight << " for " << htool->name());
    //Increasing the sumOfWeights in bins of mu for this weight computation
    m_SumOfWeightsPerMu[itool][mu] += weight;
    m_SumOfSquaredWeightsPerMu[itool][mu] += weight*weight;

    itool++;
  }

  //Last entry contains the unweighted counts
  m_TotalSumOfWeights[itool] += 1;
  m_SumOfWeightsPerMu[itool][mu] += 1;
  m_TotalSumOfSquaredWeights[itool] += 1;
  m_SumOfSquaredWeightsPerMu[itool][mu] += 1;



  //Will initialize CutBookKeepers to store the information in (D)xAOD
  //Expected that SumOfWeightsComputations is the only one allowed to record *ebc
  if(! outputMetaStore()->contains<xAOD::CutBookkeeperContainer>(m_CBKcontainerName) ) {
    ATH_MSG_DEBUG( "xAOD::CutBookkeeperContainer " << m_CBKcontainerName << " does not exist in output MetaData store. Creating it." );
    //Create and register the container
    xAOD::CutBookkeeperContainer* ebc = new xAOD::CutBookkeeperContainer();
    // Take care of the peculiarities of the new xAOD EDM, i.e., create the needed AuxStore
    xAOD::CutBookkeeperAuxContainer* auxCont = new xAOD::CutBookkeeperAuxContainer();
    ebc->setStore( auxCont ); //gives it a new associated aux container
    //Create all the needed CutBookkeepers
    itool=0;
    for ( ToolHandleArray<IWeightTool>::const_iterator itoo=m_weightTools.begin(); itoo!=m_weightTools.end(); ++itoo ) {
      //Getting the tool name (to set the SumOfWeights name)
      std::string toolname = m_CBKnamePrefix + (*itoo)->name();
      toolname.replace(toolname.find("ToolSvc."), std::string("ToolSvc.").length(), "");

      //Create a CutBookkeeper for each sumOfWeights
      xAOD::CutBookkeeper* eb = new xAOD::CutBookkeeper();
      m_ebcList.push_back(eb);
      ebc->push_back(eb);
      eb->setName(toolname);
      eb->setDescription("Number of unskimmed weighted events using " + toolname);

      //Creating a CutBookkeeper for each SumOfWeights split per bin of averageIntPerXing
      std::vector<xAOD::CutBookkeeper*> tmpVec;
      m_ebcListPerMu.push_back( tmpVec );
      if (m_maxMuBins>1)
       for (unsigned int iM=0; iM<m_maxMuBins; iM++) {
	xAOD::CutBookkeeper* eb_mu = new xAOD::CutBookkeeper();
	m_ebcListPerMu[itool].push_back(eb_mu);
	ebc->push_back(eb_mu);
	eb_mu->setName(TString::Format("%s_mu_%u",toolname.c_str(),iM).Data());
	eb_mu->setDescription("Number of unskimmed weighted events using " + toolname + " split in bins of averageIntPerXing");
       }

      itool++;
    }
    // Record the container
    ATH_CHECK( outputMetaStore()->record(ebc, m_CBKcontainerName) );
    ATH_MSG_VERBOSE("Recorded xAOD::CutBookkeeperContainer " << m_CBKcontainerName);
    // Record the aux container
    ATH_CHECK( outputMetaStore()->record( auxCont, m_CBKcontainerName+"Aux." ) );
    ATH_MSG_VERBOSE( "Recorded xAOD::CutBookkeeperContainer " << m_CBKcontainerName << "Aux." );
  }//end of creating of cut book keepers

  return StatusCode::SUCCESS;
}


StatusCode SumOfWeightsTool::dumpIntoTxtFile() {
  /// opening file handle to the ASCII output file
  int m_ofd=-1;
  ATH_MSG_INFO("dumping data into file [" << m_ofname << "]...");
  if (m_ofname.empty()) {
    ATH_MSG_ERROR("cannot dump data into an empty file name!");
    return StatusCode::FAILURE;
  }
  m_ofd = open(m_ofname.c_str(), 
	       O_WRONLY | O_CREAT | O_TRUNC,
	       S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (m_ofd < 0) {
    ATH_MSG_ERROR("problem opening file [" << m_ofname << "] with write permissions.");
    return StatusCode::FAILURE;
  }

  char* buf = 0;
  int buf_sz = asprintf(&buf,
			".%s = %u , .%s = %u\n",
			"nWeights",
			(unsigned int)m_TotalSumOfWeights.size(),
			"nMuBins",
			m_maxMuBins);
  write(m_ofd, buf, buf_sz);
  free(buf);
		
  //Looping over weight computations and storing all results for one computation on a single line
  int itool=0;
  for ( ToolHandleArray<IWeightTool>::const_iterator itoo=m_weightTools.begin(); itoo!=m_weightTools.end(); ++itoo ) {
    ToolHandle<IWeightTool> htool = *itoo;

    std::stringstream bufv;
    //Starting the line with the weight name
    std::string toolname = htool->name();
    toolname.replace(toolname.find("ToolSvc."), std::string("ToolSvc.").length(), "");
    bufv << toolname;
    bufv << ", ";
    //Storing the total sumOfWeights
    bufv << m_TotalSumOfWeights[itool] ;
    bufv << ", ";
    bufv << m_TotalSumOfSquaredWeights[itool] ;
    bufv << ", ";
    //Storing the 'per-mu' sumOfWeights
    std::vector<float> tmpVec = m_SumOfWeightsPerMu[itool];
    std::vector<float> tmpVecSquared = m_SumOfSquaredWeightsPerMu[itool];
    for (unsigned int iM=0; iM<m_maxMuBins; iM++) {
      bufv << tmpVec[iM];
      bufv << ", ";
      bufv << tmpVecSquared[iM];
      bufv << ", ";
    }

    buf = 0;
    buf_sz = asprintf(&buf,
		      "%s\n",
		      bufv.str().c_str());
    write(m_ofd, buf, buf_sz);
    free(buf);

    itool++;
  }//end of loop over weights computations

  //Last entry contains the unweighted counts
  std::stringstream bufv;
  bufv << "Unweighted, ";
  //Storing the total count
  bufv << m_TotalSumOfWeights[itool] ;
  bufv << ", ";
  bufv << m_TotalSumOfSquaredWeights[itool] ;
  bufv << ", ";
  //Storing the 'per-mu' counts
  std::vector<float> tmpVec = m_SumOfWeightsPerMu[itool];
  std::vector<float> tmpVecSquared = m_SumOfSquaredWeightsPerMu[itool];
  for (unsigned int iM=0; iM<m_maxMuBins; iM++) {
    bufv << tmpVec[iM];
    bufv << ", ";
    bufv << tmpVecSquared[iM];
    bufv << ", ";
  }
  buf = 0;
  buf_sz = asprintf(&buf,
		    "%s\n",
		    bufv.str().c_str());
  write(m_ofd, buf, buf_sz);
  free(buf);


  //Closing the text file
  if (m_ofd > 0) {
    fflush(NULL);
    if (close(m_ofd) < 0) {
      ATH_MSG_WARNING("problem while closing [" << m_ofname << "]");
    }
  }

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode SumOfWeightsTool::endRun() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  //Dumping the different SumOfWeights into a text file
  if (m_dumpTxtFile) ATH_CHECK(dumpIntoTxtFile());

  //----------------------------------------------------------------
  //Trying to store the same information into CutBookKeeper objects
  //----------------------------------------------------------------
  //Looping over weight computations and storing all results for one computation on a single line
  unsigned int ntool = m_weightTools.size();
  for ( unsigned int itool=0; itool<ntool; itool++) {
    xAOD::CutBookkeeper* eb = m_ebcList[itool];
    eb->setNAcceptedEvents(m_TotalSumOfWeights[ntool]);
    eb->setSumOfEventWeights(m_TotalSumOfWeights[itool]);
    eb->setSumOfEventWeightsSquared(m_TotalSumOfSquaredWeights[itool]);

    //Storing the SumOfWeights split per bin of averageIntPerXing
    std::vector<float> tmpVec = m_SumOfWeightsPerMu[itool];
    std::vector<float> tmpVecSquared = m_SumOfSquaredWeightsPerMu[itool];
    if (m_maxMuBins>1)
     for (unsigned int iM=0; iM<m_maxMuBins; iM++) {
      //Create a CutBookkeeper for each sumOfWeights
      xAOD::CutBookkeeper* eb_mu = m_ebcListPerMu[itool][iM];
      eb_mu->setNAcceptedEvents(m_SumOfWeightsPerMu[ntool][iM]);
      eb_mu->setSumOfEventWeights(tmpVec[iM]);
      eb_mu->setSumOfEventWeightsSquared(tmpVecSquared[iM]);
     }
  }//end of loop over weights computations

  return StatusCode::SUCCESS;
}


