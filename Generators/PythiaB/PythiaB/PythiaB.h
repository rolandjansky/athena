/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef GENERATORMODULESPYTHIAB_H
#define GENERATORMODULESPYTHIAB_H
#include "Pythia_i/Pythia.h"
#include "GeneratorModules/GenModule.h"
// commons from input cards
//#include "Cutbq.h"

#include "Pythia_i/Pydat1.h"
#include "Pythia_i/Pydat2.h"
#include "Pythia_i/Pydat3.h"
#include "Pythia_i/Pysubs.h"
#include "Pythia_i/Pypars.h"
#include "Pythia_i/Pydatr.h"
#include "Pythia_i/Pymssm.h"
#include "Pythia_i/Pyint2.h"
#include "Pythia_i/Pyint5.h"

#include "GeneratorObjects/McEventCollection.h"
#include "StoreGate/DataHandle.h"

using std::string;
typedef std::vector<std::string> CommandVector;

/**
@class  PythiaB

@brief   This code is used to get a PYTHIA Monte Carlo event.

    genInitialize() is used to read parameters

    callGenerator() makes the event

    genFinalize() writes log files etc

    fillEvt(GeneratorEvent* evt) passes the event to HepMC

    The output will be stored in the transient event store so it can be
    passed to the simulation.

@author        M.Smizanska

         Ian Hinchliffe June 2000

         Modeled after the CDF code by Stan Thompson
         added access to pyint2 common Feb 2001

         W.Walkowiak, 2006-04-02 added m_maxTriesHard, m_forceDecayChannel
                                 and m_decayChannelParameters 
                                (for user_finsel.F reorganization) 

        E.Lobodzinska doxygen docu (Jan. 2008)
*/

class PythiaB : public Pythia
{
public:
  PythiaB(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PythiaB();
	
  virtual StatusCode genuserInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  //	Pyint5& pyint5();
		
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);
		
private:
  StatusCode storeBEvent();
  long pCounter(HepMC::GenEvent* myEvt) const;
		
  McEventCollection m_BEventBuffer;
  // HepMC::GenEvent currentEvent;
		
  CommandVector m_cutbqVector;
  string m_cutbqandor; 
  std::vector<double> m_lvl1cut;
  std::vector<double> m_lvl2cut;
  std::vector<double> m_offcut;
  double m_cutbqp, m_cutbqe, m_cutabqp, m_cutabqe;
  double m_mhad;
  double m_xhad;
  double m_fla;
  double m_Ntup;

  double m_maxTriesHard;
  string m_forceDecayChannel;
  std::vector<double> m_decayChannelParameters;	

  string m_forceb;
  string m_forcec;

};

#endif
