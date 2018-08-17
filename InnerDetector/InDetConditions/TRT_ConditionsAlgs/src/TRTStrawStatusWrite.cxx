////////////////////////////// 
//TRTStrawStatusWrite.cxx  
////////////////////////////

#include "TRT_ConditionsAlgs/TRTStrawStatusWrite.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"

TRTStrawStatusWrite::TRTStrawStatusWrite( const std::string &name, ISvcLocator *pSvcLocator)  :  
  AthAlgorithm( name, pSvcLocator ),
  m_trtStrawStatusIF("TRT_StrawStatusSummarySvc",name),
  m_trtStrawStatus("TRT_StrawStatusSummarySvc",name),
  m_par_statusfile(""), 
  m_par_statusfilepermanent(""),
  m_par_statusfileHT(""),
  m_setup(false)
{ 
  declareProperty("StatusTool",m_trtStrawStatusIF);
  declareProperty("StrawStatusTool",m_trtStrawStatus);
  declareProperty("StatusInputFile",m_par_statusfile);
  declareProperty("StatusInputFilePermanent",m_par_statusfilepermanent);
  declareProperty("StatusInputFileHT",m_par_statusfileHT);
}

TRTStrawStatusWrite::~TRTStrawStatusWrite( )
{ }

StatusCode TRTStrawStatusWrite::initialize()
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TRTStrawStatusWrite initialise" << endmsg;

  if(StatusCode::SUCCESS != m_trtStrawStatusIF.retrieve()){
    msg(MSG::ERROR) << " Can't get TRTStrawstatusTool" << endmsg;
    return StatusCode::FAILURE;
  }


  if( m_trtStrawStatus.retrieve().isFailure() ) {
    msg(MSG::ERROR) << " Can't do a dynamic cast to TRTStrawStatusSummaryTool" << endmsg;
    return StatusCode::FAILURE;
  }

  
  return StatusCode::SUCCESS;
}

StatusCode TRTStrawStatusWrite::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  msg(MSG::INFO) << "TRTStrawStatusWrite::execute write DB tag for files "
      << m_par_statusfile << " " << m_par_statusfilepermanent << " " << m_par_statusfileHT << endmsg;
  if (m_par_statusfile		!="")	sc=m_trtStrawStatus->readFromTextFile(m_par_statusfile);
  if (m_par_statusfilepermanent !="") 	sc=m_trtStrawStatus->readFromTextFile(m_par_statusfilepermanent);
  if (m_par_statusfileHT        !="")   sc=m_trtStrawStatus->readFromTextFile(m_par_statusfileHT);
 
      if(sc!=StatusCode::SUCCESS) {
        msg(MSG::ERROR) << " Could not read TRT StrawStatusSummary objects " << endmsg;
        return StatusCode::FAILURE;
      }
  return sc;

}

StatusCode TRTStrawStatusWrite::finalize()
{
 
  StatusCode sc ;
  msg(MSG::INFO) << " Dump the Straw Status:  " << endmsg;
  sc =   m_trtStrawStatus->writeToTextFile("StrawStatusDump_Writer.txt");
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<" Error writing the file out" <<endmsg;
  }
  return StatusCode::SUCCESS;
}
