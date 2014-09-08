// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <map>
#include <utility> //std::pair
#include <iostream>
#include <vector>

//Local include(s):
#include "../TrigCostRootAnalysis/MonitorL1ChainMap.h"
#include "../TrigCostRootAnalysis/CounterL1ChainMap.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"

// ROOT includes
#include <TCanvas.h>
#include <TH1.h>
#include <TDirectory.h>
#include <TSystem.h>

namespace TrigCostRootAnalysis {

  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorL1ChainMap::MonitorL1ChainMap(const TrigCostData* _costData) : MonitorBase(_costData, "L1Chain") {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
  }
  
  /**
   * Process new event for this monitor.
   * For the L1 monitor, all chains in the event are recorded - but they are specifically linked to their seeding
   * L1 chain.
   * @param _weight The event weight.
   */
  void MonitorL1ChainMap::newEvent(Float_t _weight) {
    if ( Config::config().debug() ) Info("MonitorL1ChainMap::newEvent", "*** Processing L1ChainMap ***");
    
    // Populate the counter collections we'll be using for this event.
    collateCounterCollectionsForEvent( m_costData->getLumi(), TrigConfInterface::getCurrentDBKey() );
    
    //Now loop over the counter collections;
    for (CounterMapSetIt_t _it = m_collectionsToProcess.begin(); _it != m_collectionsToProcess.end(); ++_it) {
      CounterMap_t* _counterMap = *_it;
      
      // Loop over all chains - but go by L1.
      for (UInt_t _c = 0; _c < m_costData->getNChains(); ++_c) {
      
        // Get the name of the chain (Supplying L2 or EF helps, but is not needed)
        Int_t _chainID = m_costData->getChainID(_c);
        std::string _chainName = TrigConfInterface::getHLTNameFromChainID( _chainID, m_costData->getChainLevel(_c) );
        
        // Did we fail at reading the config?
        if ( _chainName == "" ) {
          Warning("MonitorL1ChainMap::newEvent", "Skipping Chain ID %i. Cannot get name from current configuration.", _chainID);
          continue;
        }
        
        std::string _L1Name = TrigConfInterface::getLowerChainName( _chainName );
        //std::cout << _chainName << "-> TRIGL1: '" << _L1Name << "'" << std::endl;
        
        // Check if lower chain is known
        Int_t _L1ID;
        if ( _L1Name == "" ) {
          _L1Name = "NO_SEED";
          _L1ID = -1;
        }
        
        // The HLT chain may be seeded by many L1 items
        if ( _L1Name.find(",") != std::string::npos ) {
          //Get list of individual items. Remove dependence on boost
          std::vector<std::string> _L1NameVector;
          std::istringstream _ss(_L1Name);
          std::string _item;
          while(std::getline(_ss, _item, ',')) {
            _L1NameVector.push_back( _item );
          }
          for (UInt_t i = 0; i < _L1NameVector.size(); ++i) {
            _L1ID = TrigConfInterface::getCtpId( _L1NameVector.at(i) );
            //std::cout << " mID " << _L1ID << std::endl;
            CounterBase* _counter = getCounter( _counterMap, _L1NameVector.at(i), _L1ID );
            _counter->processEventCounter( _c, 0, _weight );
          }
        } else { // Or be seeded by just one chain
          _L1ID = TrigConfInterface::getCtpId( _L1Name );
          //std::cout << " ID " << _L1ID << std::endl;
          CounterBase* _counter = getCounter( _counterMap, _L1Name, _L1ID );
          _counter->processEventCounter( _c, 0, _weight );
        }
        
      }
      
      endEvent();
    }
    
  }
  
  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t _ID )
   * @param _name Cost reference to name of counter.
   * @param _ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorL1ChainMap::newCounter(  const std::string &_name, Int_t _ID  ) {
    return new CounterL1ChainMap( m_costData, _name,  _ID, m_detailLevel );
  }
  
  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorL1ChainMap::saveOutput() {

    Bool_t _doRoot = Config::config().getInt(kOutputRoot);
    Bool_t _doCanv = Config::config().getInt(kOutputCanvas);
    Bool_t _doHist = Config::config().getInt(kOutputHist);
    Bool_t _doImage = Config::config().getInt(kOutputImage);
    Bool_t _doPng = Config::config().getInt(kOutputPng);
    Bool_t _doPdf = Config::config().getInt(kOutputPdf);
  
    disableROOTMsg();
    
    TCanvas* _c = new TCanvas("", "", 800 * 2, 600);
    _c->Divide(2, 1);
    _c->SetBatch(kTRUE);
    
    const std::string _outputFolder = Config::config().getStr(kOutputDirectory);
    const std::string _imgFolder = Config::config().getStr(kOutputImageDirectory);
    
    // Loop over all counter collections
    CounterCollectionIt_t _colIt = m_counterCollections.begin();
    for (; _colIt != m_counterCollections.end(); ++_colIt) {
      CounterMapIt_t _mapIt = _colIt->second.begin();
      std::string _counterCollectionName = _colIt->first;
      
      if (_doRoot) gDirectory->mkdir( _counterCollectionName.c_str() );
      if (_doRoot) gDirectory->cd   ( _counterCollectionName.c_str() );
      
      if (_doRoot) gDirectory->mkdir( m_name.c_str() );
      if (_doRoot) gDirectory->cd   ( m_name.c_str() );
      
      std::string _outputPath = _outputFolder + "/" + _imgFolder + "/" + _counterCollectionName + "/" + m_name;
      if (_doImage)  gSystem->mkdir( _outputPath.c_str(), kTRUE);
      
      for (; _mapIt != _colIt->second.end(); ++_mapIt) {
        CounterL1ChainMap* _TCL1 = static_cast<CounterL1ChainMap*>( _mapIt->second );
        TH1F* _h = _TCL1->getChainsSeededHistogram();
        
        //std::cout << "L1 Seed: " << TCL1->getName() << " TotalChainTime:" << TCL1->m_totalTime << " TotalChainCalls:" << TCL1->m_totalCalls
        //<< " L1PS:" << TrigConfInterface::getPrescale( TCL1->getName() ) << " Ev>=1Cal:" << _h->GetEntries()
        //<< " EvMeanCal:" << _h->GetMean() << " EvTotCal:" << TCL1->getTotalChainSeeds() << std::endl;
        
        if (_doRoot) gDirectory->mkdir( _TCL1->getName().c_str() );
        if (_doRoot) gDirectory->cd   ( _TCL1->getName().c_str() );
        
        TVirtualPad* _p = _c->cd(1);
        _p->SetLogx(kFALSE);
        _p->SetLogy(kTRUE);
        plotHistogram(_h);
        Float_t _y = 0.9, _step = 0.045;
        _c->cd(2);
        plotText(0.25, _y -= _step, ("L1 Chain: " + _TCL1->getName()).c_str() );
        plotText(0.25, _y -= _step, ("Events: " + intToString((Int_t)_h->GetEntries()) + " Total chain calls: " + intToString((Int_t)_TCL1->getTotalChainCalls()) ).c_str() );
        plotText(0.25, _y -= _step, ("Mean chain calls per event: " + doubleToString(_h->GetMean()) ).c_str() );
        for (UInt_t _p = 0; _p < _TCL1->getNChains(); ++_p) {
          plotText(0.27, _y -= _step, ("#rightarrow " + intToString(_TCL1->getChainCalls(_p)) + "x " + _TCL1->getChainName(_p) ).c_str() );
        }
        std::string _plotName = std::string( _TCL1->getName() + "_chainSeeds");
        if (_doPng)  _c->Print(std::string(_outputPath + "/" + _plotName + ".png").c_str());
        if (_doPdf)  _c->Print(std::string(_outputPath + "/" + _plotName + ".pdf").c_str());
        if (_doCanv || _doHist) _c->Write( std::string("c_" + _plotName).c_str() );
        //if (_doHist) _h->Write( std::string("h_" + _plotName).c_str() );
        _c->Clear("D");
        
        _p = _c->cd(1);
        _p->SetLogx(kTRUE);
        _p->SetLogy(kTRUE);
        _h = _TCL1->getChainsTimeHistogram();
        _h->SetLineWidth(5);
        plotHistogram(_h);
        _y = 0.9;
        _c->cd(2);
        plotText(0.25, _y -= _step, ("L1 Chain: " + _TCL1->getName()).c_str() );
        plotText(0.25, _y -= _step, ("Events: " + intToString((Int_t)_h->GetEntries()) + " Total chain calls: " + intToString((Int_t)_TCL1->getTotalChainCalls()) ).c_str() );
        for (UInt_t _p = 0; _p < _TCL1->getNChains(); ++_p) {
          int _col = _p + 2;
          if (_col > 9) {
            _col += 50;
            _col += (_p - 10) * 5;
          }
          _TCL1->getChainTimeHistogram( _TCL1->getChainName(_p) )->SetLineColor(_col);
          _c->cd(1);
          plotHistogram( _TCL1->getChainTimeHistogram( _TCL1->getChainName(_p) ), kTRUE, "same");
          _c->cd(2);
          plotText(0.27, _y -= _step, ("#color[" + intToString(_col) + "]{#rightarrow " + intToString(_TCL1->getChainCalls(_p)) + "x " + _TCL1->getChainName(_p) + "}" ).c_str() );
        }
        //plotHistogram(_h, kTRUE, "same");
        _plotName = std::string( _TCL1->getName() + "_chainTime");
        if (_doPdf) _c->Print(std::string(_outputPath + "/" + _plotName + ".pdf").c_str());
        if (_doPng) _c->Print(std::string(_outputPath + "/" + _plotName + ".png").c_str());
        if (_doCanv || _doHist) _c->Write( std::string("c_" + _plotName).c_str() );
        //if (_doHist) _h->Write( std::string("h_" + _plotName).c_str() );
        _c->SetLogx(kFALSE);
        _c->Clear("D");
        
        if (_doRoot) gDirectory->cd( ".." );
      }
      
      if (_doRoot) gDirectory->cd( ".." );
      if (_doRoot) gDirectory->cd( ".." );
    }
    
    delete _c;
    enableROOTMsg();
  }
  
} // namespace TrigCostRootAnalysis
