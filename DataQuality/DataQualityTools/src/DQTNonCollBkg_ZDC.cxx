/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTNonCollBkg_ZDC
// PACKAGE:  DataQualityTools
// AUTHORS:  A.Canepa <acanepa@cern.ch>,
//           Marco Leite <leite@cern.ch>
//
// See DQTNonCollBkg_ZDC.h for a description of this class
//
//
// ********************************************************************


#include "ZdcEvent/ZdcDigits.h"
#include "ZdcEvent/ZdcDigitsCollection.h"
//#include "ZdcIdentifier/ZdcID.h"
#include "DataQualityTools/ZdcSignalSinc.h"

#include "DataQualityTools/DQTNonCollBkg_ZDC.h"

#include "xAODEventInfo/EventInfo.h"

using namespace Trig;
namespace Trig {
  class ChainGroup;
}

//----------------------------------------------------------------------------------
DQTNonCollBkg_ZDC::DQTNonCollBkg_ZDC(const std::string & type,
                                          const std::string & name,
                                          const IInterface* parent) :
                                          DataQualityFatherMonTool(type, name, parent),
					  m_HistEnergySideA(nullptr),
					  m_HistEnergySideC(nullptr),
					  m_HistTimeSideA(nullptr),
					  m_HistTimeSideC(nullptr),
					  m_HistTimeDeltaSidesAC(nullptr),
                                          m_digitsContainerName("ZdcDigitsCollection"),
                                          m_complainContain(1),
                                          m_complainRetrieve(1),
					  m_nsamples(0),
					  m_bcid(nullptr),
                                          m_trigDec("Trig::TrigDecisionTool")

{
  declareInterface<IMonitorToolBase> (this);

  declareProperty("histoPath",           m_path           = "GLOBAL/DQTNonCollBkg_ZDC");
  //declareProperty("doRunCosmics",   m_doRunCosmics   = 1);
  //declareProperty("doRunBeam",       m_doRunBeam      = 1);
  //declareProperty("doOfflineHists",    m_doOfflineHists = 1);
  //declareProperty("doOnlineHists",     m_doOnlineHists  = 1);
  declareProperty("complainContain",m_complainContain   = 1);
  declareProperty("complainRetrieve",m_complainRetrieve = 1);

  m_EnergySideA.resize(4,0);
  m_EnergySideC.resize(4,0);
  m_TimeSideA.resize(4,0);
  m_TimeSideC.resize(4,0);

  m_map_LowGain[0xec000000] =   1.00;  // verify
  m_map_LowGain[0xec400000] =   5.21;
  m_map_LowGain[0xec800000] =  17.40;
  m_map_LowGain[0xecc00000] =  22.60;
  m_map_LowGain[0xed000000] =   1.00;  // verify
  m_map_LowGain[0xed400000] =   4.52;
  m_map_LowGain[0xed800000] =   8.52;
  m_map_LowGain[0xedc00000] =  50.90;
  
  m_map_HighGain[0xec000000] =  0.10;  // verify
  m_map_HighGain[0xec400000] =  0.55;
  m_map_HighGain[0xec800000] =  1.80;
  m_map_HighGain[0xecc00000] =  2.41;
  m_map_HighGain[0xed000000] =  0.10;  // verify 
  m_map_HighGain[0xed400000] =  0.45;
  m_map_HighGain[0xed800000] =  0.93;
  m_map_HighGain[0xedc00000] =  5.10;

}

//----------------------------------------------------------------------------------
DQTNonCollBkg_ZDC::~DQTNonCollBkg_ZDC()
//----------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------
StatusCode DQTNonCollBkg_ZDC::bookHistograms()
//StatusCode DQTNonCollBkg_ZDC::bookHistograms(bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun )
//----------------------------------------------------------------------------------
{
  bool failure(false);
  
  MsgStream log(msgSvc(), name());
  //if (newRun)    {
    log << MSG::INFO << "In bookHistograms(): " <<  "m_doRunCosmics = " << m_doRunCosmics << "; m_doRunBeam = "<< m_doRunBeam << endmsg;
    
    //This will do the real booking
    failure = bookDQTNonCollBkg_ZDC();
    
    // reset the map                                                                                                                             
    // do NOT dereg (histo are not written to file)                                                                                              
    m_map_BCID_DeltaT.clear();
    
  //}
  //else if (newEventsBlock || newLumiBlock)    {
  //  return StatusCode::SUCCESS;
  //}
  if (failure)    {
    return StatusCode::FAILURE;
  }
  else    {
    return StatusCode::SUCCESS;
  }

  /*
  StatusCode sc = m_trigDec.retrieve();
  
  if ( sc.isFailure() ){
    log << MSG::ERROR << "Can't get handle on TrigDecisionTool" << endmsg;
    return  StatusCode::FAILURE;
  }
  else {
    log << MSG::DEBUG << "Got handle on TrigDecisionTool" << endmsg;
    return StatusCode::SUCCESS;
  }
  */
}

//----------------------------------------------------------------------------------
bool DQTNonCollBkg_ZDC::bookDQTNonCollBkg_ZDC()
//Here the histograms are defined and booked. If anyone fails, it will trigger an
//error message and return an error.
//----------------------------------------------------------------------------------
{
  bool failure(false);

  MsgStream log(msgSvc(), name());

  std::string  fullPath = m_path+"/ZDC";

  //failure = failure | registerHist(fullPath, m_bcid  = TH1F_LW::create("bcid", "Events per BCID", 3564,0.5,3564.5),ManagedMonitorToolBase::shift).isFailure();
  failure = failure | registerHist(fullPath, m_bcid  = TH1F_LW::create("bcid", "Events per BCID", 3564,0.5,3564.5)).isFailure();

  failure = failure | registerHist(fullPath,
		  m_HistEnergySideA = TH1F_LW::create("m_EnergySideA",
		                                  "Total Energy Side A (TeV)",
                                                  200,0.,7.),run).isFailure();
                                                  //200,0.,7.),ManagedMonitorToolBase::expert,run).isFailure();

  failure = failure | registerHist(fullPath,
  		  m_HistEnergySideC = TH1F_LW::create("m_EnergySideC",
  		                                  "Total Energy Side C (TeV)",
                                                  200,0.,7.),run).isFailure();

  failure = failure | registerHist(fullPath,
          m_HistTimeSideA   = TH1F_LW::create("m_TimeSideA",
                                          "Absolute Time Side A (ns)",
                                          200,0.,50.),run).isFailure();

  failure = failure | registerHist(fullPath,
          m_HistTimeSideC   = TH1F_LW::create("m_TimeSideC",
                                          "Absolute Time Side C (ns)",
                                           200,0.,50.),run).isFailure();

  failure = failure | registerHist(fullPath,
          m_HistTimeDeltaSidesAC = TH1F_LW::create("m_TimeDeltaSidesAC",
                                               "Time Difference Side A - C (ns)",
                                                500,-50.,50.),run).isFailure();


  if (failure) {
     log << MSG::ERROR << "Error Booking histograms " << endmsg;
  }

  return failure;
}


//This method will retrieve the SG container, loop over the ZDC channels
//and for each channel call the methods to extract energy and timing information
//----------------------------------------------------------------------------------
StatusCode DQTNonCollBkg_ZDC::fillHistograms()
//----------------------------------------------------------------------------------
{

  bool failure(false);

  MsgStream log(msgSvc(), name());

  // Do the trigger selection here
  // Trigger selection suggested by D.Berge
  // Too rare :for testing, comment it 
  if (! m_trigDec->isPassed("L1_ZDC_UNPAIRED")) return 0;
 
  std::vector<std::string> L1items = m_trigDec->getChainGroup("L1_.*")->getListOfTriggers();
  unsigned int nL1Items = L1items.size();

  for(unsigned int it=0; it<nL1Items; ++it) {
    if( L1items[it] != "" ) {
      m_L1Names.push_back( L1items[it]);
      m_L1Trigs.push_back(0);
      msg(MSG::DEBUG) <<"L1Chain " <<it <<" " <<L1items <<m_trigDec->isPassed(L1items[it]) <<endmsg;
    }
  }

  unsigned bunch_crossing_id=0;
  const DataHandle<xAOD::EventInfo> event_info;
 
  if (!evtStore()->retrieve( event_info ).isSuccess() || !event_info.isValid() ) {
    log<< MSG::ERROR << "ERROR cannot get BCID = " <<endmsg;
  }
  else {
    bunch_crossing_id = event_info->bcid();
    log<< MSG::DEBUG << "BCID = " <<  bunch_crossing_id<<endmsg;
    m_bcid->Fill(bunch_crossing_id);
  }

  

  //int ped = 0;

  float eA = 0.;
  float eC = 0.;

  std::vector<std::vector<int> > wfm;
  std::vector<std::vector<int> >::iterator vit;
  std::vector<int>::iterator it;


  Identifier id;


  const ZdcDigitsCollection* digitsCollection;
  ZdcDigitsCollection::const_iterator iter;
  const ZdcDigits* digits_p;


  //1) Start a set of checks for the container in evtStore

  //Look for the container presence
  bool dg = evtStore()->contains<ZdcDigitsCollection> (m_digitsContainerName);
  if (!dg)
    {
      if (m_complainContain)
        log << MSG::WARNING
            << "--> DQT_ZDC: evtStore does not contain " << m_digitsContainerName << endmsg;
      m_complainContain = 0;
      return StatusCode::SUCCESS;
    }

  // Look up the  ZdcDigitsCollection in Storegate
  StatusCode digitsLookupSC = evtStore()->retrieve(digitsCollection, m_digitsContainerName);
  if (digitsLookupSC.isFailure())
    {
      if (m_complainRetrieve)
        log << MSG::WARNING
            << "--> DQT_ZDC: Could not retrieve " << m_digitsContainerName << " from evtStore"
            << endmsg;
      m_complainRetrieve = 0;
      return StatusCode::SUCCESS;
    }

  // Check if  ZdcDigitsCollection  really exists
  if (digitsLookupSC.isSuccess() && !digitsCollection)
    {
      log << MSG::ERROR <<
          "--> DQT_ZDC: Storegate returned zero pointer for " << m_digitsContainerName << endmsg;
      return StatusCode::SUCCESS;
    }

  //2) The container was sucessfully retrieved, go ahead looping over it
  //    and extract the information
  log << MSG::DEBUG <<
      "--> DQT_ZDC: SUCCESS retrieving " << m_digitsContainerName << endmsg;

  //Loop over the channels
  for (iter = digitsCollection->begin(); iter != digitsCollection->end(); iter++)
    {
      digits_p = *iter;
      id = digits_p->identify();
      wfm.clear();
      wfm.resize(2);

      //These combinations are present for all channels
      // wfm[0] -> low gain,  undelayed
      // wfm[1] -> high gain, undelayed
      wfm[0] = digits_p->get_digits_gain0_delay0();
      wfm[1] = digits_p->get_digits_gain1_delay0();


      //Sometimes  we get a zero size back. Handle it here
      if (wfm[0].size() == 0)
        {
          msg(MSG::WARNING) << "ZERO SIZE g0d0 at ID  " << id.getString()
              << endmsg;
          wfm[0].resize(m_nsamples);
        }


      if (wfm[1].size() == 0)
        {
          msg(MSG::WARNING) << "ZERO SIZE g1d0 at ID  " << id.getString()
              << endmsg;
          wfm[1].resize(7);
        }




      // Check to see if there are delayed information
      //        module type 0 -> (full Energy) delayed info
      //        module type 1 -> (segmented) do not have delayed info
      //        wfm[2] -> low gain,  delayed
      //        wfm[3] -> high gain, delayed
      //        - use the identifiers here -
      if (((id.get_identifier32().get_compact() >> 21) & 1) == 0)
        {
          wfm.resize(4);
          wfm[2] = digits_p->get_digits_gain0_delay1();
          wfm[3] = digits_p->get_digits_gain1_delay1();
        }

      //3) Subtratct baseline pedestal
      //We need to be carefull. Sometimes the vector size here is zero (PPM flaw) and
      //the code crashs if we do not treat this.
      for (vit = wfm.begin(); vit < wfm.end(); vit++)
        {
          if (vit->size() == 0)
            vit->resize(7);
          //ped = *vit->begin();
          //TODO: use algo
          //for (it = vit->begin(); it < vit->end(); it++)
          //  {
          //    (*it) -= ped;
          //  }
        }
      getEnergyAndTimeByModule(id, wfm);
    }

  //Fill the ZDC global information histograms

  //total energy: multiply vectors of weight by energy and accumulate
  //float vmul (float i, float j) { return i*j; }
  //result gets back to a
  //transform (a.begin(), a.end(), b.begin(), b.begin(), vmul);

  //getTotalEnergy(m_EnergySideA,"A");
  //getTotalEnergy(m_EnergySideC,"C");


  eA = m_EnergySideA[1] +  m_EnergySideA[2] +  m_EnergySideA[3];
  eC = m_EnergySideC[1] +  m_EnergySideC[2] +  m_EnergySideC[3]; 


 // std::cout << "**** ZDC+ EA EC" << eA << "   " << eC << std::endl;


  
  m_HistEnergySideA->Fill(eA);
  m_HistEnergySideC->Fill(eC);

  m_HistTimeSideA->Fill(m_TimeSideA[0]);
  m_HistTimeSideC->Fill(m_TimeSideC[0]);

  m_HistTimeDeltaSidesAC->Fill(m_TimeSideA[0] - m_TimeSideC[0]);

  if(m_map_BCID_DeltaT[bunch_crossing_id] == 0){

    TString histoName ="deltaT_"; histoName+=bunch_crossing_id;
    TString histoTitle="Arrival time BCID:"; histoTitle+=bunch_crossing_id;
    std::string  fullPath=m_path+"/ZDC";
    failure = failure | registerHist(fullPath, m_map_BCID_DeltaT[bunch_crossing_id] = TH1F_LW::create(histoName,histoTitle,500,-50.,50.)).isFailure();
    //failure = failure | registerHist(fullPath, m_map_BCID_DeltaT[bunch_crossing_id] = TH1F_LW::create(histoName,histoTitle,500,-50.,50.),ManagedMonitorToolBase::expert).isFailure();
  }

  if(m_map_BCID_DeltaT[bunch_crossing_id]) m_map_BCID_DeltaT[bunch_crossing_id]->Fill(m_TimeSideA[0] - m_TimeSideC[0]); // fill with the DeltaT                                                       
  
  
  if (failure) {
    log << MSG::ERROR << "Error Booking histograms " << endmsg;
  }
  
  
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------------
StatusCode DQTNonCollBkg_ZDC::procHistograms( )
//StatusCode DQTNonCollBkg_ZDC::procHistograms( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun )
//----------------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "In procHists()" << endmsg;

  //  std::vector<TH1D*> proj;
  //  if( isEndOfRun ){
  //if ( endOfEventsBlock || endOfLumiBlock || endOfRun ) {

  //}

  return StatusCode::SUCCESS;

}

//Here we could make checks like the
// -   m_TimeDeltaSidesAC is peaked at zero
// -   m_TimeDeltaSidesAC width
// -   m_EnergySideA - m_EnergySideA residual
// -   etc.
//----------------------------------------------------------------------------------
StatusCode DQTNonCollBkg_ZDC::checkHists(bool /* fromFinalize */)
//----------------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "In checkHists()" << endmsg;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
int  DQTNonCollBkg_ZDC::getEnergyAndTimeByModule(const Identifier& id,
                 const std::vector< std::vector <int> >& wfm) {
//----------------------------------------------------------------------------------

  int i = 0;
  int wfmIndex = 0;
  float energy = 0.;
  float w = 0.;
  float timing = 0.;
  //int error = 0;
  //int warning = 0;

  double Slices[10];
  int NSlice = 5;
  double gain=1.;
  double pedestal=0;
  double CFD_frac = 1.0;
  bool corr=0;

  ZdcSignalSinc* zdcSignalSinc;

  std::vector<std::vector<int> >::const_iterator vit;
  std::vector<int>::const_iterator it;
  std::vector<int>  y;

  Identifier32::value_type x;

  /*
   Id:       Side       Module Number  Module Type
   0xec000000   0  (C)           0               0
   0xec400000   0  (C)           1               0
   0xec800000   0  (C)           2               0
   0xecc00000   0  (C)           3               0
   0xed000000   1  (A)           0               0
   0xed400000   1  (A)           1               0
   0xed800000   1  (A)           2               0
   0xedc00000   1  (A)           3               0
   */
  x = id.get_identifier32().get_compact();
  // We will fix only on low gain, undelayed information
  wfmIndex = 0;  
  i = 0;
  for (vit = wfm.begin(); vit<wfm.end(); vit++) {
      if (i == wfmIndex)
        {
          y = *vit;

           //Very simple: energy is the maximum sample and
           //timing is the ratio of A2/A1

          //energy = *(std::max_element(y.begin(), y.end()));
          //if (y[1]) timing = y[2]/y[1];


          // This is for the sinx/x interpolation

          for(int I=0;I<NSlice;I++)  {Slices[I]=y[I];}
          zdcSignalSinc = new ZdcSignalSinc(NSlice);

          zdcSignalSinc->process(Slices,gain,pedestal,CFD_frac,corr);

          energy  = zdcSignalSinc->getAmp() ;
          timing  = zdcSignalSinc->getTime();
          //error   = zdcSignalSinc->getError();
          //warning = zdcSignalSinc->getWarning();


          //Retrieves only total energy (big PMT's) information
          if (((x >> 21) & 1) == 0)
            {
               //Use the appropriate weight for energy according to the module
	       w = (wfmIndex == 0)	?  m_map_LowGain[x] :  m_map_HighGain[x]; 
	       energy = w * energy;
       
	       //std::cout << "ZDC+++ID " << x << std::endl;
               //std::cout << "ZDC++Weight " << w << std::endl;
               //std::cout << "ZDC+++Energy " << energy << std::endl;
               //std::cout << "ZDC+++Timing " << timing << std::endl;
               //std::cout << "ZDC+++Error " << error << std::endl;
               //std::cout << "ZDC+++Warning " << warning << std::endl;


              switch (x)
                {
              case 0xec000000: //Side C Mod 0
                m_EnergySideC[0] = energy; //Pin Diode !!
                m_TimeSideC[0]   = timing;
                break;

              case 0xec400000: //Side C Mod 1
                m_EnergySideC[1] = energy;
                m_TimeSideC[1]   = timing;
                break;

              case 0xec800000: //Side C Mod 2
                m_EnergySideC[2] = energy;
                m_TimeSideC[2]   = timing;
                break;

              case 0xecc00000: //Side C Mod 3
                m_EnergySideC[3] = energy;
                m_TimeSideC[3]   = timing;
                break;

              case 0xed000000: //Side A Mod 0
                m_EnergySideA[0] = energy; //Pin Diode !!
                m_TimeSideA[0]   = timing;
                break;

              case 0xed400000: //Side A Mod 1
                m_EnergySideA[1] = energy;
                m_TimeSideA[2]   = timing;
                break;

              case 0xed800000: //Side A Mod 2
                m_EnergySideA[2] = energy;
                m_TimeSideA[2]   = timing;
                break;

              case 0xedc00000: //Side A Mod 3
                m_EnergySideA[3] = energy;
                m_TimeSideA[3]   = timing;
                break;
                }
            }
        }
      i = (i<4) ? (i+1) :  0;
  }
      /*
   #These are constants Andrei used to convert from ADC counts to GeV
   #http://indico.cern.ch/getFile.py/access?contribId=0&resId=0&materialId=slides&confId=89265
   #            neutrons      photons
   #      c/g   hg    lg     hg    lg
   #A0   0.74  0.45  4.52    0.62  6.22
   #A1   1.19  0.93  8.52
   #A2   1.60  5.10  50.9

   #C0   0.78  0.55  5.21    0.71  6.73
   #C1   1.09  1.80  17.4
   #C2   1.75  2.41  22.6

   #For photons in hadronic section I just repeat the neutron weight
   #numbers unit is GeV/PPM count
   self.hg_weigths= {'A':{'neutron':[0.45,0.93,5.10], 'photon':[0.62,0.93,5.10], 'total':[0.45,0.93,5.10]},
   'C':{'neutron':[0.55,1.80,2.41], 'photon':[0.71,1.80,2.41], 'total':[0.55,1.80,2.41]} }

   self.lg_weigths= {'A':{'neutron':[4.52, 8.52, 50,9], 'photon':[6.22,8.52, 50,9], 'total':[4.52, 8.52, 50,9]},
   'C':{'neutron':[5.21, 17.4, 22.6], 'photon':[6.73,17.4, 22.6], 'total':[5.21, 17.4, 22.6]} }

   # photon:    E0 >= 5 ADC counts, E1<  20GeV ; E2 <  4 ADC Counts
   # neutrons:  E0 >= 5 ADC counts, E1>= 20GeV ; E2 >= 4 ADC Counts
   */
return 0;
}
