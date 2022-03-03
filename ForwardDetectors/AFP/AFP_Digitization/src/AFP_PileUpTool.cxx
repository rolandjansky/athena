/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "AFP_Digitization/AFP_PileUpTool.h"
#include <algorithm>
#include "AFP_SimEv/AFP_TDSimHit.h"
#include "AFP_SimEv/AFP_TDSimHitCollection.h"
#include "AFP_DigiEv/AFP_TDDigiCollection.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "Identifier/Identifier.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include <map>
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"
#include "CLHEP/Random/RandPoissonQ.h"

AFP_PileUpTool::AFP_PileUpTool(const std::string& type,
                               const std::string& name,
                               const IInterface* parent) :
  PileUpToolBase(type, name, parent)
{
  // NOTE: The following variables are actually re-initialized by AFP_DigiTop::initialize() or AFP_PileUpTool::initialize()

  // Quantum efficincy of PMT in 100 nm steps int(WeaveLength-100)/100

  m_QuantumEff_PMT[0] = 0.00;
  m_QuantumEff_PMT[1] = 0.15;
  m_QuantumEff_PMT[2] = 0.20;
  m_QuantumEff_PMT[3] = 0.15;
  m_QuantumEff_PMT[4] = 0.05;
  m_QuantumEff_PMT[5] = 0.01;
  m_QuantumEff_PMT[6] = 0.00;

  m_deposited_charge.resize(645120, 0.f); // = 4 x 6 x 336 x 80
  m_deposited_energy.resize(645120, 0.f);

  // end of ToolBox variables

  declareProperty("TDSimHitCollection" , m_TDSimHitCollectionName, "Name of the input Collection of the simulated TD Hits");
  declareProperty("SIDSimHitCollection" , m_SIDSimHitCollectionName, "Name of the input Collection of the simulated SiD hits");
  declareProperty("CollectionEff", m_CollectionEff);
  declareProperty("ConversionSpr", m_ConversionSpr);
  declareProperty("FallTime", m_FallTime);
  declareProperty("RiseTime", m_RiseTime);
  declareProperty("CfdThr", m_CfdThr);
  declareProperty("Gain", m_Gain);

  m_SignalVect.resize( int((m_RiseTime+5*m_FallTime)/5.));

  if (!(m_SignalVect.empty()))
    {
      for(unsigned long i=0; i< m_SignalVect.size(); i++) { // size is an unsigned long so abs is redundant
        m_SignalVect[i] = SignalFun(2.5+5.*i,m_RiseTime,m_FallTime);
      }
    }
  else ATH_MSG_DEBUG("m_SignalVector is empty");


  TString Id1, Id2, Id3;

  for( int i=0; i<4; i++) {
    Id1.Form("%d",i);
    for( int j=0; j<32; j++) {
      Id2.Form("%d",j);
      for( int k=0; k<2; k++) {
        Id3.Form("%d",k);
        m_Signal[i][j][k] =  TH1F("Signal_"+Id1+"_"+Id2+"_"+Id3,"",2000,707000,717000);
      }
    }
  }

}

StatusCode AFP_PileUpTool::initialize() {

  ATH_MSG_DEBUG ( "AFP_PileUpTool::initialize() called" );

  ATH_MSG_DEBUG ( "    CollectionEff: " << m_CollectionEff << endmsg
                  );

  ATH_CHECK (m_randomSvc.retrieve());
  ATH_MSG_DEBUG ( "Retrieved RandomNumber Service" );

  ATH_CHECK (m_mergeSvc.retrieve());
  ATH_MSG_DEBUG("Retrieved PileUpMergeSvc");

  m_mergedTDSimHitList = new AFP_TDSimHitCollection("mergedTDSimHitList");
  m_mergedSIDSimHitList = new AFP_SIDSimHitCollection("mergedSIDSimHitList");

  ATH_CHECK(m_TDDigiCollectionKey.initialize());
  ATH_CHECK(m_SiDigiCollectionKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode AFP_PileUpTool::processAllSubEvents(const EventContext& ctx) {

  ATH_MSG_DEBUG ( "AFP_PileUpTool::processAllSubEvents()" );

  typedef PileUpMergeSvc::TimedList<AFP_TDSimHitCollection>::type TimedTDSimHitCollList;
  typedef PileUpMergeSvc::TimedList<AFP_SIDSimHitCollection>::type TimedSIDSimHitCollList;

  TimedTDSimHitCollList TDSimHitCollList;
  unsigned int numberOfTDSimHits{0};
  if (not (m_mergeSvc->retrieveSubEvtsData(m_TDSimHitCollectionName, TDSimHitCollList, numberOfTDSimHits).isSuccess()) and TDSimHitCollList.size() == 0) {
    ATH_MSG_FATAL ( "Could not fill TimedTDSimHitCollList" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( TDSimHitCollList.size() << " AFP_TDSimHitCollections with key " << m_TDSimHitCollectionName << " found." );

  TimedSIDSimHitCollList SIDSimHitCollList;
  unsigned int numberOfSIDSimHits{0};
  if (not (m_mergeSvc->retrieveSubEvtsData(m_SIDSimHitCollectionName, SIDSimHitCollList, numberOfSIDSimHits).isSuccess()) and SIDSimHitCollList.size() == 0) {
    ATH_MSG_FATAL ( "Could not fill TimedSIDSimHitCollList" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( SIDSimHitCollList.size() << " AFP_SIDSimHitCollections with key " << m_SIDSimHitCollectionName << " found." );

  TimedHitCollection<AFP_TDSimHit> thpcAFP_TDPmt;
  TimedTDSimHitCollList::iterator iColl  (TDSimHitCollList.begin());
  TimedTDSimHitCollList::iterator endColl(TDSimHitCollList.end());

  while (iColl != endColl) {
    const AFP_TDSimHitCollection* tmpColl(iColl->second);
    thpcAFP_TDPmt.insert(iColl->first, tmpColl);
    ATH_MSG_DEBUG ( " AFP_TDSimHitCollection found with " << tmpColl->size() << " hits " << iColl->first );
    ++iColl;
  }

  TimedHitCollection<AFP_SIDSimHit> thpcAFP_SiPmt;
  TimedSIDSimHitCollList::iterator iSiColl  (SIDSimHitCollList.begin());
  TimedSIDSimHitCollList::iterator endSiColl(SIDSimHitCollList.end());

  while (iSiColl != endSiColl) {
    const AFP_SIDSimHitCollection* tmpSiColl(iSiColl->second);
    thpcAFP_SiPmt.insert(iSiColl->first, tmpSiColl);
    ATH_MSG_DEBUG ( " AFP_SIDSimHitCollection found with " << tmpSiColl->size() << " hits " << iSiColl->first );
    ++iSiColl;
  }

  ATHRNG::RNGWrapper* rngWrapper = m_randomSvc->getEngine(this, m_randomStreamName);
  rngWrapper->setSeed( m_randomStreamName, ctx );
  CLHEP::HepRandomEngine* rngEngine = rngWrapper->getEngine(ctx);

  ATH_CHECK(fillTDDigiCollection(thpcAFP_TDPmt, rngEngine, ctx));

  ATH_CHECK(fillSiDigiCollection(thpcAFP_SiPmt, ctx));

  return StatusCode::SUCCESS;
}


StatusCode AFP_PileUpTool::prepareEvent(const EventContext& /*ctx*/, const unsigned int nInputEvents){

  ATH_MSG_DEBUG ( "AFP_PileUpTool::prepareEvent() called for " << nInputEvents << " input events" );

  m_mergedTDSimHitList->clear();

  m_mergedSIDSimHitList->clear();

  return StatusCode::SUCCESS;
}


StatusCode AFP_PileUpTool::processBunchXing(int bunchXing,
                                            SubEventIterator bSubEvents,
                                            SubEventIterator eSubEvents) {

  ATH_MSG_DEBUG ( "AFP_PileUpTool::processBunchXing() " << bunchXing );
  SubEventIterator iEvt = bSubEvents;
  for (; iEvt!=eSubEvents; iEvt++) {
    StoreGateSvc& seStore = *iEvt->ptr()->evtStore();
    //PileUpTimeEventIndex thisEventIndex = PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index());   // does not do anything
    ATH_MSG_VERBOSE ("SubEvt StoreGate " << seStore.name() << " :"
                     << " bunch crossing : " << bunchXing
                     << " time offset : " << iEvt->time()
                     << " event number : " << iEvt->ptr()->eventNumber()
                     << " run number : " << iEvt->ptr()->runNumber()
                     );

    const AFP_TDSimHitCollection* tmpColl = 0;

    if (!seStore.retrieve(tmpColl, m_TDSimHitCollectionName).isSuccess()) {

      ATH_MSG_ERROR ( "SubEvent AFP_TDSimHitCollection not found in StoreGate " << seStore.name() );

      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG ( "AFP_TDSimHitCollection found with " << tmpColl->size() << " hits" );

    AFP_TDSimHitCollection::const_iterator iPmt = tmpColl->begin();
    AFP_TDSimHitCollection::const_iterator ePmt = tmpColl->end();

    for (; iPmt!=ePmt; ++iPmt) m_mergedTDSimHitList->push_back((*iPmt));


    const AFP_SIDSimHitCollection* tmpSiColl = 0;

    if (!seStore.retrieve(tmpSiColl, m_SIDSimHitCollectionName).isSuccess()) {

      ATH_MSG_ERROR ( "SubEvent AFP_SIDSimHitCollection not found in StoreGate " << seStore.name() );

      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG ( "AFP_TDSimHitCollection found with " << tmpSiColl->size() << " hits" );

    AFP_SIDSimHitCollection::const_iterator iSiPmt = tmpSiColl->begin();
    AFP_SIDSimHitCollection::const_iterator eSiPmt = tmpSiColl->end();

    for (; iSiPmt!=eSiPmt; ++iSiPmt) m_mergedSIDSimHitList->push_back((*iSiPmt));


  }

  return StatusCode::SUCCESS;
}

StatusCode AFP_PileUpTool::mergeEvent(const EventContext& ctx){

  ATHRNG::RNGWrapper* rngWrapper = m_randomSvc->getEngine(this, m_randomStreamName);
  rngWrapper->setSeed( m_randomStreamName, ctx );
  CLHEP::HepRandomEngine* rngEngine = rngWrapper->getEngine(ctx);
  ATH_CHECK(fillTDDigiCollection(m_mergedTDSimHitList, rngEngine, ctx));
  ATH_CHECK(fillSiDigiCollection(m_mergedSIDSimHitList, ctx));

  return StatusCode::SUCCESS;
}



StatusCode AFP_PileUpTool::finalize() { return StatusCode::SUCCESS; }



StatusCode AFP_PileUpTool::fillTDDigiCollection(TimedHitCollection<AFP_TDSimHit>& thpcAFP, CLHEP::HepRandomEngine* rndEngine, const EventContext& ctx)
{
  TimedHitCollection<AFP_TDSimHit>                thpc = thpcAFP;
  TimedHitCollection<AFP_TDSimHit>::const_iterator i, e, it;

  for( int i=0; i<4; i++) {
    for( int j=0; j<32; j++) {
      for( int k=0; k<2; k++) {
        m_Signal[i][j][k].Reset();
      }
    }
  }

  while (thpc.nextDetectorElement(i, e)) for (it = i; it != e; it++) {

      int Station     = (*it)->m_nStationID;
      int Detector    = (*it)->m_nDetectorID;
      int SensitiveElement = (*it)->m_nSensitiveElementID;
      float GlobalTime     =  (*it)->m_fGlobalTime;
      float WaveLength = (*it)->m_fWaveLength;

      if(SensitiveElement%2 == 1) createTDDigi(Station, Detector, SensitiveElement, GlobalTime, WaveLength, rndEngine);
    }
  ATH_CHECK(StoreTDDigi(ctx));

  return StatusCode::SUCCESS;
}

StatusCode AFP_PileUpTool::fillTDDigiCollection(const AFP_TDSimHitCollection* AFP_TDSimHitCollection,
                                          CLHEP::HepRandomEngine* rndEngine, const EventContext& ctx)
{

  for( int i=0; i<4; i++) {
    for( int j=0; j<32; j++) {
      for( int k=0; k<2; k++) {
        m_Signal[i][j][k].Reset();
      }
    }
  }


  AFP_TDSimHitConstIter it    = AFP_TDSimHitCollection->begin();
  AFP_TDSimHitConstIter itend = AFP_TDSimHitCollection->end();


  for (; it != itend; it++) {

    int Station     = it->m_nStationID;
    int Detector = it->m_nDetectorID;
    int SensitiveElement = it->m_nSensitiveElementID;
    float GlobalTime     =  it->m_fGlobalTime;
    float WaveLength = it->m_fWaveLength;


    if(SensitiveElement%2 == 1) createTDDigi(Station, Detector, SensitiveElement, GlobalTime, WaveLength, rndEngine);
  }
  ATH_CHECK(StoreTDDigi(ctx));

  return StatusCode::SUCCESS;
}


StatusCode AFP_PileUpTool::fillSiDigiCollection(TimedHitCollection<AFP_SIDSimHit>& thpcAFP, const EventContext& ctx)
{
  TimedHitCollection<AFP_SIDSimHit>           thpc = thpcAFP;
  TimedHitCollection<AFP_SIDSimHit>::const_iterator i, e, it;

  //  const long indexend = (long)width * (long)height * (long)depth;
  m_deposited_charge.assign(645120, 0.f);  // here just 6 layers per detector are considered

  while (thpc.nextDetectorElement(i, e)) {
    for (it = i; it != e; it++) {
      int Station     = (*it)->m_nStationID;
      int Detector    = (*it)->m_nDetectorID;
      int PixelRow    = (*it)->m_nPixelRow;
      int PixelCol    = (*it)->m_nPixelCol;
      float PreStepX  = (*it)->m_fPreStepX;
      float PreStepY  = (*it)->m_fPreStepY;
      float PreStepZ  = (*it)->m_fPreStepZ;
      float PostStepX = (*it)->m_fPostStepX;
      float PostStepY = (*it)->m_fPostStepY;
      float PostStepZ = (*it)->m_fPostStepZ;
      float DepEnergy = (*it)->m_fEnergyDeposit;

      createSiDigi(Station, Detector, PixelRow, PixelCol, PreStepX, PreStepY, PreStepZ, PostStepX, PostStepY, PostStepZ,  DepEnergy);
    }
  }
  ATH_CHECK(StoreSiDigi(ctx));
  return StatusCode::SUCCESS;
}


StatusCode AFP_PileUpTool::fillSiDigiCollection(const AFP_SIDSimHitCollection* AFP_SIDSimHitCollection, const EventContext& ctx)
{
  AFP_SIDSimHitConstIter it    = AFP_SIDSimHitCollection->begin();
  AFP_SIDSimHitConstIter itend = AFP_SIDSimHitCollection->end();

  m_deposited_charge.assign(645120, 0.f);  // here just 6 layers per detector are considered

  for (; it != itend; it++) {
    int Station     = it->m_nStationID;
    int Detector    = it->m_nDetectorID;
    int PixelRow    = it->m_nPixelRow;
    int PixelCol    = it->m_nPixelCol;
    float PreStepX  = it->m_fPreStepX;
    float PreStepY  = it->m_fPreStepY;
    float PreStepZ  = it->m_fPreStepZ;
    float PostStepX = it->m_fPostStepX;
    float PostStepY = it->m_fPostStepY;
    float PostStepZ = it->m_fPostStepZ;
    float DepEnergy = it->m_fEnergyDeposit;

    createSiDigi(Station, Detector, PixelRow, PixelCol, PreStepX, PreStepY, PreStepZ, PostStepX, PostStepY, PostStepZ, DepEnergy);
  }
  ATH_CHECK(StoreSiDigi(ctx));
  return StatusCode::SUCCESS;
}


void AFP_PileUpTool::createTDDigi(int Station, int Detector, int SensitiveElement, float GlobalTime, float
                                  WaveLength, CLHEP::HepRandomEngine* rndEngine)
{
  ATH_MSG_DEBUG ( " iterating Pmt " << Station << "  " << Detector << "  " << SensitiveElement << " " << GlobalTime
                  << WaveLength );


  int id =  (int(WaveLength)-100)/100;
  if (id > 6) id=6;
  if (id < 0) id=0;

  double prescale=1.;

#ifdef TDMAXQEFF
  prescale = TDMAXQEFF ;
#endif


  if (CLHEP::RandFlat::shoot(rndEngine, 0.0, 1.0) > m_QuantumEff_PMT[id]*m_CollectionEff/prescale) return;

  double PheTime = CLHEP::RandGaussQ::shoot(rndEngine, GlobalTime + 5.* m_ConversionSpr, m_ConversionSpr);
  int NumberOfSecondaries =  CLHEP::RandPoissonQ::shoot(rndEngine, m_Gain);

  if ( Station >3 || Station < 0 || Detector >31 || Detector < 0 || (SensitiveElement-1)/2>1 || (SensitiveElement-1)/2<0) {
    ATH_MSG_FATAL ( "Wrong station, detector or sensitive detector id" );
    return;
  }

  int iStart =  m_Signal[Station][Detector][(SensitiveElement-1)/2].FindBin(PheTime);

  if (!(m_SignalVect.empty()))
    {
      for (unsigned long i = 0; i < (m_SignalVect.size()); i++) {
        if( (int)i+iStart > m_Signal[Station][Detector][(SensitiveElement-1)/2].GetNbinsX()) break ;
        m_Signal[Station][Detector][(SensitiveElement-1)/2].AddBinContent(i+iStart,m_SignalVect[i]*NumberOfSecondaries/m_Gain);
      }
    }
  else ATH_MSG_DEBUG("m_SignalVector is empty");

  return;

}

StatusCode AFP_PileUpTool::StoreTDDigi(const EventContext& ctx)
{
  SG::WriteHandle<AFP_TDDigiCollection> digitCollection = SG::makeHandle(m_TDDigiCollectionKey, ctx);
  ATH_CHECK(digitCollection.record(std::make_unique<AFP_TDDigiCollection>()));

  TString Id1, Id2, Id3;

  for( int i=0; i<4; i++) {
    for( int j=0; j<32; j++) {
      for( int k=0; k<2; k++) {
        double ADC = m_Signal[i][j][k].GetMaximum();
        double TDC =m_Signal[i][j][k].GetBinCenter( m_Signal[i][j][k].FindFirstBinAbove(ADC*m_CfdThr));
        if( ADC>0) {
          // Saturation
          double curr=0;
          for( int l =0; l<m_Signal[i][j][k].GetNbinsX(); l++ ) {
            curr = m_Signal[i][j][k].GetBinContent(l);
            if( curr > 100) curr=50.*log10(curr);
            m_Signal[i][j][k].SetBinContent(l,curr);
          }
          ADC = m_Signal[i][j][k].GetMaximum();
          TDC = m_Signal[i][j][k].GetBinCenter( m_Signal[i][j][k].FindFirstBinAbove(ADC*m_CfdThr));
          // Constant fraction discriminator

          int delay = ((m_RiseTime/4)/5.);
          for(int  l= m_Signal[i][j][k].GetNbinsX(); l>0; l-- ) {
            curr = m_Signal[i][j][k].GetBinContent(l);
            if ( l > delay) { curr = m_Signal[i][j][k].GetBinContent(l-delay) - curr;}
            else { curr = -1.;}
            m_Signal[i][j][k].SetBinContent(l,curr);
          }

          // correction two all delays and different position of sensitive elements

          if( k==0 ) TDC = TDC-2130.;
          if( k==1 ) TDC = TDC-2797.;

          AFP_TDDigi* tddigi = new AFP_TDDigi ();
          tddigi->m_nStationID=i;
          tddigi->m_nDetectorID=j;
          tddigi->m_nSensitiveElementID=2*k+1;
          tddigi->m_fADC=ADC;
          tddigi->m_fTDC=TDC;

          Id1.Form("%d",i);
          Id2.Form("%d",j);
          Id3.Form("%d",k);

          digitCollection->Insert(*tddigi);
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}


void AFP_PileUpTool::createSiDigi(int Station, int Detector, int PixelRow, int PixelCol, float PreStepX, float PreStepY, float PreStepZ, float PostStepX, float PostStepY, float PostStepZ, float DepEnergy)
{
  ATH_MSG_DEBUG ( " iterating Pmt, station " << Station << ", detector " << Detector << ", pixel_col " << PixelCol << ", pixel_row " << PixelRow << ", dep_energy" << DepEnergy << " (x,y,z)_pre (" << PreStepX <<"," << PreStepY <<"," << PreStepZ <<"), (x,y,z)_post (" << PostStepX <<"," << PostStepY <<"," << PostStepZ <<")" );

  float distance; // in mm
  distance = sqrt(pow(PostStepX-PreStepX,2)+pow(PostStepY-PreStepY,2)+pow(PostStepZ-PreStepZ,2));

  ATH_MSG_DEBUG ( "actual distance in pixel: " << distance);

  distance = 1000.0* distance; // in mu

  // we do not consider energy dependende
  // 1mu corresponds to ~ 110 e-hole pairs
  // this we should slighlty modify to some distribution

  ATH_MSG_DEBUG ( "deposted charge for given hit " << distance*110.0 );

  if ((PixelCol>=336) || (PixelRow >= 80) || (Station >= 4) || (Detector >= 6 ) )
    {
      if (Detector == 11)
        {
          ATH_MSG_DEBUG ( "Hit in the vacuum layer in front of the station " << Station );

        }
      else
        {
          ATH_MSG_WARNING ( "WRONG NUMBER of PIXEL coordinates or station or detector !!!:" );
          ATH_MSG_WARNING ( "station [max 4] " << Station << ", detector [max 6]" << Detector << ", pixel_col [max 336] " << PixelCol << ", pixel_row [max 80] " << PixelRow );
        }
      return;
    }

  m_deposited_charge[6*336*80*Station + 80*336*Detector + 80*PixelCol + PixelRow] += distance * 110.0;
  m_deposited_energy[6*336*80*Station + 80*336*Detector + 80*PixelCol + PixelRow] += DepEnergy;

  return;

}

StatusCode AFP_PileUpTool::StoreSiDigi(const EventContext& ctx)
{
  SG::WriteHandle<AFP_SiDigiCollection> siDigitCollection = SG::makeHandle(m_SiDigiCollectionKey, ctx);
  ATH_CHECK(siDigitCollection.record(std::make_unique<AFP_SiDigiCollection>()));

  long index = 0;
  while ( index < 645120 ) // here just 6 layers per detector are considered
    {
      // here should  come noise

      if (m_deposited_charge[index] > 1000.)
        {

          ATH_MSG_DEBUG ( " total # of pairs from length " << m_deposited_charge[index]);
          ATH_MSG_DEBUG ( " total # of pairs from dep_energy " << 1000000.*m_deposited_energy[index]/3.6);

          int station  = (int)(index/(80*336*6));
          int detector = (int)((index-station*80*336*6)/(80*336));
          int column   = (int)((index-station*80*336*6-detector*80*336)/80);
          int row      = (int)(index-station*80*336*6-detector*80*336-column*80);

          ATH_MSG_DEBUG ( " reversed mapping, station " << station << ", detector " << detector << ", pixel_col " << column << ", pixel_row " << row );


          AFP_SiDigi* sidigi = new AFP_SiDigi();

          sidigi->m_nStationID = station;
          sidigi->m_nDetectorID = detector;
          sidigi->m_nPixelCol = column;
          sidigi->m_nPixelRow = row;

          sidigi->m_fADC = 1000000.*m_deposited_energy[index]/3.6;
          sidigi->m_fTDC = 0.;


          ATH_MSG_DEBUG ( " size: " << siDigitCollection->size());

          siDigitCollection->Insert(*sidigi);
        }

      index++;
    }

  return StatusCode::SUCCESS;
}


double AFP_PileUpTool::SignalFun(double Time, double RiseTime, double FallTime)
{
  double f=0;
  if ( Time < 0) return f;
  double p = (FallTime-RiseTime*TMath::Log(1.+FallTime/RiseTime))/TMath::Log(10.);
  f = TMath::Power(Time/p,RiseTime/p)*TMath::Exp(-(Time/p));
  f = f/(TMath::Power(RiseTime/p,RiseTime/p)*TMath::Exp(-(RiseTime/p)));
  return f;
}
