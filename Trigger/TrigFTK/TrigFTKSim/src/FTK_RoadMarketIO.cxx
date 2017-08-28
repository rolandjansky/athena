/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTK_RoadMarketIO.h"
#include "TrigFTKSim/FTK_RoadMarketTool.h"
#include "TrigFTKSim/FTKSetup.h"

/******
 * Input part of the code
 *****/

void FTK_RoadMarketInput::notValid(const char *method)
{
  FTKSetup::PrintMessageFmt(ftk::info,"Method %s not implemented in FTK_RoadMarketInput",method);
}


FTK_RoadMarketInput::FTK_RoadMarketInput(FTK_RoadMarketTool *market) :
   m_market(market),m_cur_road(0), m_cur_iroad(0)
{;}


inline void FTK_RoadMarketInput::setNBanks(int nbanks)
{
  /* The setNBanks method for the input is called AFTER the same method
     for the output, so the number of the output banks was already decided and
     here only a check is needed */
  if (nbanks != m_market->getNBanks() ) {
    FTKSetup::PrintMessageFmt(ftk::sevr,"Input set with a number of banks different then the previous output: %d!=%d",nbanks,m_market->getNBanks());
  }

  // prepare the structures to loop over the roads
  m_cur_road = new FTKRoad*[m_market->getNBanks()];
  m_cur_iroad = new int[m_market->getNBanks()];
}

inline int FTK_RoadMarketInput::getNBanks() const
{
  return m_market->getNBanks();
}

inline FTKRoadStream* FTK_RoadMarketInput::getStream(int id)
{
  return m_market->getStream(id);
}

inline unsigned long FTK_RoadMarketInput::runNumber(const unsigned int& ibank) const
{
  return m_market->runNumber(ibank);
}


inline unsigned long FTK_RoadMarketInput::eventNumber(const unsigned int& ibank) const
{
  return m_market->eventNumber(ibank);
}


int FTK_RoadMarketInput::nextEvent()
{
  m_market->prepareFits();
  return 0;
}


/** this function go to the next road, moving to the next bank if needed
    and return the pointer to the current road. return 0 if all the roads
    were used */
const FTKRoad* FTK_RoadMarketInput::nextRoad(int ibank)
{

  // get the road
  m_cur_road[ibank] = m_market->getStream(ibank)->fetchRoad();

  // check if the bank has enough roads
  if (!m_cur_road[ibank]) {
    m_cur_road[ibank] = 0;
  }
  else {
    // increment road counter
    m_cur_iroad[ibank] += 1; 
  }

  return m_cur_road[ibank];
}

/** this function go to the first road and return the pointer to
    the current road. return 0 if all the roads were used */
const FTKRoad* FTK_RoadMarketInput::firstRoad(int ibank)
{

  // get the road
  m_cur_road[ibank] = m_market->getStream(ibank)->fetchFirstRoad();
  m_cur_iroad[ibank] = 0;

  // check if the bank has enough roads
  if (!m_cur_road[ibank]) {
    m_cur_road[ibank] = 0;
  }
  else {
    // increment road counter
    m_cur_iroad[ibank] += 1; 
  }

  return m_cur_road[ibank];
}


const FTKSS& FTK_RoadMarketInput::getSS(const unsigned int &ibank, const unsigned int &iplane, const unsigned int &ssid) const
{
  return m_market->getStream(ibank)->getSS(iplane,ssid);
}

const FTKSS& FTK_RoadMarketInput::getUnusedSS(const unsigned int &ibank, const unsigned int &iplane, 
					      const unsigned int &ssid) const
{
  return m_market->getStream(ibank)->getUnusedSS(iplane,ssid);
}


FTKRoad* FTK_RoadMarketInput::getRoad(int region,int roadid,int bankid)
{
  FTKRoadStream *curstream = m_market->getStream(region);
  int roadidx = curstream->findRoad(roadid,bankid);
  curstream->attachHits(roadidx);
  return curstream->getRoad(roadidx);

}

const std::map<int,FTKSS>& FTK_RoadMarketInput::getSSPlane(int bank,int pl) const
{
  return m_market->getStream(bank)->getSSPlane(pl);
}


/******
 * Output part of the code
 *****/
void FTK_RoadMarketOutput::notValid(const char *method)
{
  FTKSetup::PrintMessageFmt(ftk::info,"Method %s not implemented in FTK_RoadMarketOutput",method);
}


FTK_RoadMarketOutput::FTK_RoadMarketOutput(FTK_RoadMarketTool *market) :
  m_market(market)
{;}


void FTK_RoadMarketOutput::init(int, bool*)
 {
   m_market->initRoads();
 }


inline void FTK_RoadMarketOutput::setNBanks(int nbanks)
{
  m_market->setNBanks(nbanks);
}

inline int FTK_RoadMarketOutput::getNBanks() const
{
  return m_market->getNBanks();
}


inline FTKRoadStream* FTK_RoadMarketOutput::getStream(int id)
{
  return m_market->getStream(id);
}


void FTK_RoadMarketOutput::eventBegin()
{
  m_market->clearRoads();
}


inline unsigned long FTK_RoadMarketOutput::runNumber(const unsigned int& ibank) const
{
  return m_market->runNumber(ibank);
}


inline unsigned long FTK_RoadMarketOutput::eventNumber(const unsigned int& ibank) const
{
  return m_market->eventNumber(ibank);
}


inline void FTK_RoadMarketOutput::setRunNumber(const unsigned int& ibank, const unsigned long &runnum)
{
  m_market->setRunNumber(ibank,runnum);
}


inline void FTK_RoadMarketOutput::setEventNumber(const unsigned int& ibank, const unsigned long &evtnum)
{
  m_market->setEventNumber(ibank,evtnum);
}


void FTK_RoadMarketOutput::addRoad(int BankID, const FTKRoad &road)
{
  // create a new element into the clones array
  m_market->getStream(BankID)->addRoad(road);
}


void FTK_RoadMarketOutput::inc4LRoad(int BankID, const int& val)
{
  m_market->getStream(BankID)->inc4LRoad(val);
}


void FTK_RoadMarketOutput::addSS(int BankID, int plane, int ssid, const FTKSS &ssobj)
{
  m_market->getStream(BankID)->addSS(plane,ssid,ssobj);
}


inline void FTK_RoadMarketOutput::addSSPlane(int bank, int pl, const std::map<int,FTKSS> &res)
{
  m_market->getStream(bank)->addSSPlane(pl,res);
}


void FTK_RoadMarketOutput::addUnusedSSMap(int bankID, const std::map<int,std::map<int,FTKSS> > &ssmap)
{
  m_market->getStream(bankID)->addUnusedSSMap(ssmap);
}


inline void FTK_RoadMarketOutput::naoSetNhitsTot(int bank, int val)
{
  m_market->getStream(bank)->naoSetNhitsTot(val);
}


void FTK_RoadMarketOutput::naoSetNclusTot(int bank, int val)
{
  m_market->getStream(bank)->naoSetNclusTot(val);
}


void FTK_RoadMarketOutput::naoSetNclus_road(int bank, const std::vector<int> &val)
{
  m_market->getStream(bank)->naoSetNclus_road(val);
}

void FTK_RoadMarketOutput::naoAddNclus_road(int bank, const std::vector<int> &val)
{
  m_market->getStream(bank)->naoAddNclus_road(val);
}


void FTK_RoadMarketOutput::naoSetNclus(int bank, const std::vector<int> &val)
{
  m_market->getStream(bank)->naoSetNclus(val);
}


void FTK_RoadMarketOutput::naoSetNss(int bank, const std::vector<int> &val)
{
  m_market->getStream(bank)->naoSetNss(val);
}


void FTK_RoadMarketOutput::naoSetNroadsAM(int bank, int val)
{
  m_market->getStream(bank)->naoSetNroadsAM(val);
}

void FTK_RoadMarketOutput::naoSetNroadsAMComplete(int bank, int val)
{
  m_market->getStream(bank)->naoSetNroadsAMComplete(val);
}

void FTK_RoadMarketOutput::naoSetNroadsAMMissPix(int bank, int val)
{
  m_market->getStream(bank)->naoSetNroadsAMMissPix(val);
}

void FTK_RoadMarketOutput::naoSetNroadsAMMissSCT(int bank, int val)
{
  m_market->getStream(bank)->naoSetNroadsAMMissSCT(val);
}


void FTK_RoadMarketOutput::naoSetNroadsMOD(int bank, int val)
{
  m_market->getStream(bank)->naoSetNroadsMOD(val);
}


void FTK_RoadMarketOutput::naoSetNroadsRW(int bank, int val)
{
  m_market->getStream(bank)->naoSetNroadsRW(val);
}


/** Saves the roads in the market into the StoreGate. The SG calls
    are managed by the market */
void FTK_RoadMarketOutput::eventEnd()
{

}
