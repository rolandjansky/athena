/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "McEventCollectionTestTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
//#include <boost/io/ios_state.hpp>
#include "GeneratorObjects/McEventCollection.h"
#include <TH2D.h>
#include <TProfile.h>
#include "AthenaKernel/errorcheck.h"

McEventCollectionTestTool::McEventCollectionTestTool(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent)
  : DigiTestToolBase(type, name, parent),
    m_collection("TruthEvent"), //("GEN_EVENT"),
    m_nGenEvents(0),
    m_nEmptyGenEvents(0),
    m_sig_n_vert(0),
    m_sig_n_part(0),
    m_bkg_n_vert(0),
    m_bkg_n_part(0)
{
  declareProperty("McEventCollectionName" , m_collection);
}

StatusCode McEventCollectionTestTool::initialize()
{

  // initialise Truth variables:
  m_path+="Truth/McEventCollection/";
  std::string basepath=m_path;
  _TH1D(m_nGenEvents, "Number_of_GenEvents",1000,0,8000);
  _SET_TITLE(m_nGenEvents, "Number of GenEvents", "Number of GenEvents", "Frequency");
  _TH1D(m_nEmptyGenEvents, "Number_of_Empty_GenEvents",1000,0,8000);
  _SET_TITLE(m_nEmptyGenEvents, "Number of Empty GenEvents", "Number of Empty GenEvents", "Frequency");

  m_path+="Signal/";
  _TH1D(m_sig_n_vert,"N_vert_all",50,0.,1000.);
  _SET_TITLE(m_sig_n_vert,"total number of verticies","n","dN/dn");
  m_sig_n_vert -> StatOverflows();
  _TH1D(m_sig_n_part,"N_part_all",50,0.,5000.);
  _SET_TITLE(m_sig_n_part,"total number of particles","n","dN/dn");
  m_sig_n_part -> StatOverflows();

  m_path = basepath+"Background";
  _TH1D(m_bkg_n_vert,"N_vert_all",50,0.,1000.);
  _SET_TITLE(m_bkg_n_vert,"total number of verticies","n","dN/dn");
  m_bkg_n_vert -> StatOverflows();
  _TH1D(m_bkg_n_part,"N_part_all",50,0.,5000.);
  _SET_TITLE(m_bkg_n_part,"total number of particles","n","dN/dn");
  m_bkg_n_part -> StatOverflows();

  return StatusCode::SUCCESS;
}

StatusCode McEventCollectionTestTool::processEvent() {

  const DataHandle<McEventCollection> p_mceventcollection;
  if(evtStore()->retrieve(p_mceventcollection, m_collection).isFailure())
    {
      return StatusCode::SUCCESS;
    }
  //int NumberModulesVetoed[5]={0,0,0,0,0};
  unsigned int n_genEvent(0);
  unsigned int n_genEventNonEmpty(0);

  // loop over McEventCollection
  DataVector<HepMC::GenEvent>::const_iterator genEventIter(p_mceventcollection->begin());
  const DataVector<HepMC::GenEvent>::const_iterator lastGenEvent(p_mceventcollection->end());
  bool isFirstEvent(true);
  while(genEventIter!= lastGenEvent)
    {
      const HepMC::GenEvent* currentGenEvent(*genEventIter);
      if(!currentGenEvent)
        {
          ++genEventIter;
          continue;
        }
      ++n_genEvent;

      if(genEventIter != p_mceventcollection->begin()) isFirstEvent = false;

#ifdef HEPMC3
      ATH_MSG_INFO ("GenEvent Position in McEventCollection: " << n_genEvent-1 <<
                    ", signal_process_id: " << HepMC::signal_process_id(currentGenEvent) <<
                    ", event_number: " << currentGenEvent->event_number() <<
                    ", particles_size: " << currentGenEvent->particles().size() <<
                    ", vertices_size: " << currentGenEvent->vertices().size() <<
                    " .");
      if(currentGenEvent->particles().empty())
        {
          ++genEventIter;
          continue;
        }
      ++n_genEventNonEmpty;
      if(isFirstEvent)
        {
          m_sig_n_vert->Fill(currentGenEvent->vertices().size());
          m_sig_n_part->Fill(currentGenEvent->particles().size());
        }
      else
        {
          m_bkg_n_vert->Fill(currentGenEvent->vertices().size());
          m_bkg_n_part->Fill(currentGenEvent->particles().size());
        }
      ++genEventIter;
#else
      ATH_MSG_INFO ("GenEvent Position in McEventCollection: " << n_genEvent-1 <<
                    ", signal_process_id: " << currentGenEvent->signal_process_id() <<
                    ", event_number: " << currentGenEvent->event_number() <<
                    ", particles_size: " << currentGenEvent->particles_size() <<
                    ", vertices_size: " << currentGenEvent->vertices_size() <<
                    " .");
      if(currentGenEvent->particles_empty())
        {
          ++genEventIter;
          continue;
        }
      ++n_genEventNonEmpty;
      if(isFirstEvent)
        {
          m_sig_n_vert->Fill(currentGenEvent->vertices_size());
          m_sig_n_part->Fill(currentGenEvent->particles_size());
        }
      else
        {
          m_bkg_n_vert->Fill(currentGenEvent->vertices_size());
          m_bkg_n_part->Fill(currentGenEvent->particles_size());
        }
      ++genEventIter;
#endif
    }
  m_nGenEvents->Fill(n_genEvent,1);
  m_nEmptyGenEvents->Fill(n_genEvent-n_genEventNonEmpty,1);

  return StatusCode::SUCCESS;
}

StatusCode McEventCollectionTestTool::finalize()
{
  return StatusCode::SUCCESS;
}
