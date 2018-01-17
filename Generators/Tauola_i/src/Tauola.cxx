//--------------------------------------------------------------------------
// File and Version Information:
//      Tauola.cxx
//
// Description:
//      Interface to Tauola for Herwig 6.5 and Pythia 6.2+
//      to modify the HEPEVT common.
//
// Author List:
// Borut Paul Kersevan (BPK), June 2003
//
//  Vladimir Savinov (V.S.), Sept. 13, 2008:  when a tau (or, for that matter, any number of any particles) are read
//  from a file, information about MC generator is absent. "Fixed" this by counting particles in the event and
//  declaring that, in case when MC generator info is absent (technically speaking, set to 0), but the number of
//  MC vertices is equal to the number of MC particles, it is single particle MC production (though it could
//  contain many particles per event!). If this condition is not satisfied, but MC generator info is not available,
//  report a warning.
//
//  V.S., Sept.19, 2008: protected against empty events WRONGLY picking up particles from the previous event (!)
//  V.S., Feb. 18, 2009: removed "-999" (which is an error code) from the "if" conditions (after being rightfully poked by ewelinA),
//        also, introduced a piece of code to report such condition
//  V.S., June 25, 2009: tau's production vertex (if available) is now retrieved from the event (for single-particle MC)
//  V.S., July  7, 2009: this algorithm can now handle more than one particle in "single-particle" MC.
//  V.S., Sept.22, 2009: Eric Torrence's tau polarization work
//  V.S., Sept.22, 2009: SHERPA blob mode works again! This required to change the logic of calling savePythia() used to store tau polarization.
//  V.S., Sept.29, 2009: Single-particle MC mode with possibly more than one particle is reconciled with SHERPA_blob-like processing.
//  V.S., Nov. 11, 2009: Protection against insufficient length of evt->weights() (i.e. evt->weights().size())
//  V.S., Dec. 18, 2009: Number of taus that need to be decayed are now counted (this was implemented for HERWIG/TauDetails purposes)
//
// Copyright Information:
//
//------------------------------------------------------------------------

#include "Tauola_i/Tauola.h"

#include "GeneratorFortranCommon/Gen_HEPEVT.h"
#include "TruthUtils/GeneratorName.h"

#include "GaudiKernel/DataSvc.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/IO_HERWIG.h"
#include "HepMC/HEPEVT_Wrapper.h"

#include "GeneratorUtils/StringParse.h"
#include <cstdlib>

#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

using std::string;

IAtRndmGenSvc* Tauola::p_AtRndmGenSvc;
std::string	Tauola::tauola_stream = "TAUOLA_INIT";


// Functions to be used as Fortran routines
extern "C" void acrmar_(float* rvec, int* lenv ) {
  /// @todo Reduce statics: get engine directly from GenModule
  CLHEP::HepRandomEngine* engine = Tauola::p_AtRndmGenSvc->GetEngine(Tauola::tauola_stream);
  for (int i=0 ; i < *lenv ; ++i) { rvec[i] = CLHEP::RandFlat::shoot(engine); }
}
extern "C" double atl_pyr_( int* /*idummy*/ ) {
  /// @todo Reduce statics: get engine directly from GenModule
  CLHEP::HepRandomEngine* engine = Tauola::p_AtRndmGenSvc->GetEngine(Tauola::tauola_stream);
  return CLHEP::RandFlat::shoot(engine);
}

extern "C" {
  void tauola_hepevt_(int*);
  // void tauola_decres_py_(int*);
  // void tauola_decres_hw_(int*);
  void* atlas_hepevt_address_(void);
  void pyhepc_(int*);
  void pyexec_();
  void pylist_(int*);
  void tauola_setgen_(int*);
}


//--------------------------------------------------------------------------


using HepMC::HEPEVT_Wrapper;

Atlas_HEPEVT* Tauola::s_atlas_HEPEVT = new Atlas_HEPEVT();

// Declared static for re-use
static hybridPartSet_t hybridPartSet;


//--------------------------------------------------------------------------


Tauola::Tauola(const std::string& name, ISvcLocator* pSvcLocator)
  : GenModule(name,pSvcLocator),
    m_genEvt(1,1),
    m_saveDetails(true)
{
  tauola_stream	= "TAUOLA_INIT";
  declareProperty("TauolaCommand", m_tauolaCommandVector);
  declareProperty("SaveDetails", m_saveDetails);

  m_GenIndex = 0;
  m_SuperIndex = 0;
  m_avwgt = 0.;
  m_eventnum = 0;
  m_eventqcd  = 0.;
  m_eventqed  = 0.;
  m_eventscale = 0.;
  m_events = 0;
  m_evwgt = 0.;
  m_fortin = 0;
  m_fortout = 0;
  m_numberOfTauToDecay  = 0;
  m_pari10 = 0.;
  m_pari7 = 0.;
  m_pari9 = 0.;
  m_pdgid = 0;
  m_sherpa_blob = false;
  m_usrwgt = false;
  m_isize = 0;
  for( int i=0 ; i < 28 ; i++ )
    m_userwgts[i] = 0.;
}


//--------------------------------------------------------------------------


Tauola::~Tauola() {  }


//--------------------------------------------------------------------------

//--------------
// Operations --
//--------------

Fftau& Tauola::fftau(){
  return m_fftau;
}

Ffinout& Tauola::ffinout(){
  return m_ffinout;
}



UserGamprt& Tauola::usergamprt(){
  return m_usergamprt;
}


//---------------------------------------------------------------------------


StatusCode Tauola::genInitialize() {
  // Initialise the Tauola parameters
  ATH_MSG_INFO("ATLAS TAUOLA INITIALISING.\n" );


  /// @todo Reduce statics: get engine directly from GenModule
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("AtRndmGenSvc",
                                  Tauola::p_AtRndmGenSvc,
                                  CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == Tauola::p_AtRndmGenSvc)
  {
      ATH_MSG_ERROR("Could not initialize Random Number Service");
      return RndmStatus;
  }
  Tauola::tauola_stream	= "TAUOLA_INIT";


  // Set up default values for input parameters
  m_fortout=37;
  this->ffinout().iout() = m_fortout ;
  m_fortin=26;
  this->ffinout().inut() = m_fortin ;

  this->fftau().ifpol() = 1 ;
  this->fftau().ifradcor() = 1 ;
  this->fftau().ifdmod() = 0 ;
  this->fftau().ifjak1() = 0 ;
  this->fftau().ifjak2() = 0 ;
  this->fftau().ffphx() = 0.01 ;

  // Missing boson set to Z0 by default
  m_pdgid = 23;

  for (unsigned int i = 0; i < m_tauolaCommandVector.size(); ++i) {
    ATH_MSG_INFO("Command is: " << m_tauolaCommandVector[i]);
    StringParse mystring(m_tauolaCommandVector[i]);
    string myblock=mystring.piece(1);
    string myentry=mystring.piece(2);
    string mystr=mystring.piece(3);
    int myint1=mystring.intpiece(3);
    double  myfl1=mystring.numpiece(3);
    double  myfl2=mystring.numpiece(4);

    // Note that Tauola needs doubles hence the convert here
    ATH_MSG_INFO(myblock << " block  " << myentry << " item  " << myint1 << "  value " << myfl1);

    if (myblock=="tauola"){
      if (myentry == "polar"){
        this->fftau().ifpol() = myint1;
      }
      else if (myentry == "radcor"){
        this->fftau().ifradcor() = myint1;
      }
      else if (myentry == "phox"){
        this->fftau().ffphx() = myfl1;
      }
      else if (myentry == "dmode"){
        this->fftau().ifdmod() = myint1;
      }
      else if (myentry == "jak1"){
        this->fftau().ifjak1() = myint1;
      }
      else if (myentry == "jak2"){
        this->fftau().ifjak2() = myint1;
      }
      else if (myentry == "boson"){
        m_pdgid = myint1;
      }
      else {
        ATH_MSG_ERROR("ATLAS TAUOLA ERROR, entry TAUOLA has POLAR RADCOR  PHOX DMODE JAK1 JAK2: YOU HAVE SPECIFIED "<< myentry);
      }
    }
    else if (myblock=="usergamprt"){
      if (myentry == "ugused"){
	this->usergamprt().ugused() = myint1;
      }
      else if  (myentry == "usergamprt"){
	this->usergamprt().userGamprt(myint1)=myfl2;
      }
      else {
        ATH_MSG_ERROR("ATLAS TAUOLA ERROR, entry USERGAMPRT has UGUSED USERGAMPRT: YOU HAVE SPECIFIED "<< myentry);
      }
    }
    else {
      ATH_MSG_ERROR(" ERROR in TAUOLA PARAMETERS   " << myblock << " is and invalid common block name !");
    }
  }
  // end of parsing

  // Save the TAUOLA_INIT stream seeds....
  CLHEP::HepRandomEngine* engine = Tauola::p_AtRndmGenSvc->GetEngine(Tauola::tauola_stream);
  const long* sip =	engine->getSeeds();
  long int si1 = sip[0];
  long int si2 = sip[1];

  // Tauola init
  int imode = -1;
  tauola_hepevt_(&imode);

  // ... and set them back to the stream for proper save
  Tauola::p_AtRndmGenSvc->CreateStream(si1, si2, Tauola::tauola_stream);

  Tauola::tauola_stream = "TAUOLA";

  m_events = 0;

  // HepMC::HEPEVT_Wrapper::set_sizeof_int(4);
  //HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);

  // Set initial values for TauDetails
  m_tauDetails.clear();
  m_tauDetails.setEvent(-999);

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode Tauola::callGenerator() {
  ATH_MSG_DEBUG("ATLASTAUOLA generating." );

  StatusCode status = StatusCode::SUCCESS;

  m_numberOfTauToDecay = 0;

  // The McEvent to process by Tauola is the last one stored in the event store
  HepMC::GenEvent* evt = events()->back();
  if (evt==NULL) {
    ATH_MSG_ERROR("McEvent was not successfully created");
    return StatusCode::FAILURE;
  }
  else {
    // Count how many taus (still) need to be decayed

    for ( HepMC::GenEvent::particle_const_iterator partItr = evt->particles_begin();
          partItr != evt->particles_end(); ++partItr ) {
      if ( abs( (*partItr)->pdg_id() ) == 15) {
        if ( (*partItr)->status() == 1 ) {
          if ( !(*partItr)->end_vertex() ) {
            m_numberOfTauToDecay++;
          }
        }
      }
    }

    // std::cout << "Tauola_i-D-VS-m_numberOfTauToDecay = " << m_numberOfTauToDecay << std::endl;

    // Create the GeneratorEvent and restore the HEPEVT common block
    int pr_id=evt->signal_process_id();
    if (pr_id == 1 ) {pr_id = SINGLE ; } // temporary fix

    //--->>> V.S.

    // check if MC generator info is available
    int mc_process = evt->signal_process_id();

    // count vertices
    int n_vtx(0);
    for ( HepMC::GenEvent::vertex_iterator v = evt->vertices_begin();
          v != evt->vertices_end(); ++v ) {
      n_vtx++;
    }

    // count MC particles
    int n_part(0);
    for ( HepMC::GenEvent::particle_const_iterator p = evt->particles_begin();
          p != evt->particles_end(); ++p ) {

      ATH_MSG_DEBUG("Found MC particle with PDG id = " << (*p)->pdg_id());

      n_part++;
    }

    // count MC particles that come from a (presumably) proper MC vertex with at least one parent particle (could be fake for single-particle MC with more than one particle)
    int n_part_with_parent(0);

    for ( HepMC::GenEvent::particle_const_iterator p = evt->particles_begin();
          p != evt->particles_end(); ++p ) {

      ATH_MSG_DEBUG("Found MC particle with PDG id = " << (*p)->pdg_id());

      int pdg_id_self = (*p)->pdg_id();

      // retrieve info on start_vertex
      const HepMC::GenVertex * startVtx = (*p)->production_vertex();

      //      if ( !startVtx ) std::cout << "No start_vertex found!"  << std::endl;

      if ( startVtx ) {
        //	std::cout << "start_vertex found!" << std::endl;

        int number_parts_in = startVtx->particles_in_size();

        //	std::cout << " Number of in particles = " << number_parts_in << std::endl;

        if ( number_parts_in >= 1 ) {

          HepMC::GenVertex::particles_in_const_iterator  mcpartItr_in  = startVtx->particles_in_const_begin();
          HepMC::GenParticle* mcpart_in = (*mcpartItr_in);

          int pdg_id_in = mcpart_in->pdg_id();

          if ( pdg_id_in != pdg_id_self ) {
            // std::cout << "PDG id of the first particle in = " << pdg_id_in << " while child's pdg_id = " << pdg_id_self << std::endl;
            n_part_with_parent++;
          }
          // else {
          // std::cout << "Parent exists, but it is the child itself! NOT counting this in particle!" << std::endl;
          //}

        }

      }

      /*
      // retrieve info on end_vertex
      const HepMC::GenVertex * endVtx = (*p)->end_vertex();

      if ( !endVtx ) std::cout << "No end_vertex found!"  << std::endl;
      if ( endVtx ) {
      std::cout << "end_vertex found!" << std::endl;
      int number_parts_out = endVtx->particles_out_size();
      std::cout << " Number of daughters = " << number_parts_out << std::endl;
      }
      */

    }

    m_sherpa_blob = false;

    if ( mc_process == 0 ) {


      // V.S.  July 7, 2009
      //            if ( n_vtx == n_part ) {
      //      if ( n_vtx == n_part || ( n_vtx == 1 && n_part >= 1 ) ) {
      if ( ( n_vtx == n_part || ( n_vtx == 1 && n_part >= 1 ) ) && n_part_with_parent <= 1 ) {

        ATH_MSG_DEBUG("This MC event seems to contain "
                      << n_vtx << " vertices, "
                      << n_part << " particle(s) of which "
                      << n_part_with_parent << " particles seem to have at least one parent (could be fake)."
                      << " However, no MC generator information is available, so we assume that this is 'single-particle' MC (that could contain more than one particle).");

        pr_id = SINGLE;
      }
      else {

        ATH_MSG_WARNING("This MC event seems to contain "
                        << n_vtx << " vertices, "
                        << n_part << " particles of which "
                        << n_part_with_parent << " particles seem to have at least one parent (could be fake)."
                        << " However, no MC generator information is available, so we assume that this is 'SHERPA blob' event.");

        m_sherpa_blob = true;
        pr_id = SINGLE;
      }
    }
    //---<<< V.S.


    m_GenIndex = pr_id + TAUOLA;

    // REMOVE THE HERWIG TAUOLA_PHOTOS ENTRY

    // int tau_id=(pr_id/10000)%10 * 10000; <- direct use of gen. codes, should be avoided (E.M.Lobodzinska 12.12.08)
    //
    //  V.S. Jan.17, 2009: Before this change from 12.12.08 second_generator(pr_id) and third_generator(pr_id) were reporting -999
    //

    int tau_id=third_generator(pr_id);
    if (tau_id == TAUOLA_PHOTOS)
      {
        m_GenIndex -= TAUOLA_PHOTOS;
      }

    m_SuperIndex= first_generator(pr_id);

    if ( m_SuperIndex == -999 ) {

      ATH_MSG_ERROR(" first_generator(" << pr_id << ") reported " << m_SuperIndex
                    << " which is a serious error condition. Will not try to decay tau leptons for this event. Please report this error.");

      status = StatusCode::FAILURE;
      return status;

    }

    //            std::cout << "pr_id = " << (int) pr_id << std::endl;
    //            std::cout << "first_generator  = " << (int) first_generator(pr_id) << std::endl;
    //            std::cout << "second_generator = " << (int) second_generator(pr_id) << std::endl;
    //            std::cout << "third_generator  = " << (int) third_generator(pr_id) << std::endl;
    //            std::cout << "m_SuperIndex = " << (int) m_SuperIndex << std::endl;
    //            std::cout << "SINGLE = " << (int) SINGLE << std::endl;
    //            std::cout << "PYTHIA = " << (int) PYTHIA << std::endl;
    //            std::cout << "HERWIG = " << (int) HERWIG << std::endl;
    //            std::cout << "Sherpa = " << (int) Sherpa << std::endl;
    //
    //          if (m_SuperIndex ==0 ) {m_SuperIndex = SINGLE ;} // temporary
    //          std::cout << "m_SuperIndex = " << (int) m_SuperIndex << std::endl;

    m_seeds.clear();
    //  	std::copy(evt->random_states().begin(), evt->random_states().end(), back_inserter(m_seeds));


    // --->>> V.S. Sept.1, 2008

    if ( n_part <= 0 ) {

      ATH_MSG_WARNING("Empty event identified! No particles, no TAUOLA for you! "
                      << "The implications for the remaining part of MC chain are unpredictable. Take a note: you have been warned. "
                      << "Event info: n_vtx = " << n_vtx << ", n_part = " << n_part);

    }

    else {

      if ( m_SuperIndex == SINGLE && !m_sherpa_blob )
        {

          HEPEVT_Wrapper::set_event_number(evt->event_number());
          //	  HEPEVT_Wrapper::set_number_entries(1);
          HEPEVT_Wrapper::set_number_entries(n_part);

          unsigned int index = 0;

          for ( HepMC::GenEvent::particle_const_iterator p = evt->particles_begin();
                p != evt->particles_end(); ++p ) {
            //(*p)->print(std::cout);
            // int p_bcode=(*p)->barcode();
            int p_pdg_id=(*p)->pdg_id();

            double p_px=(*p)->momentum().px()/1000.;
            double p_py=(*p)->momentum().py()/1000.;
            double p_pz=(*p)->momentum().pz()/1000.;
            double p_pe=(*p)->momentum().e()/1000.;

            // int p_stat=(*p)->status();
            double p_pm=sqrt(p_pe*p_pe-p_px*p_px-p_py*p_py-p_pz*p_pz);

            index++;

            if ( index > 10000 ) {

              ATH_MSG_FATAL(" Too many particles in (presumably) 'single-particle' MC! "
                            << "n_part = " << n_part
                            << " which is a serious error condition. Exiting. Please report this error.");

              status = StatusCode::FAILURE;
              return status;

            }

            HEPEVT_Wrapper::set_status    ( index, 1);
            HEPEVT_Wrapper::set_id        ( index, p_pdg_id);
            HEPEVT_Wrapper::set_parents   ( index, 0,0);
            HEPEVT_Wrapper::set_children  ( index, 0,0);
            HEPEVT_Wrapper::set_momentum  ( index, p_px,p_py,p_pz,p_pe);
            HEPEVT_Wrapper::set_mass      ( index, p_pm);

            // V.S.  June 25, 09 ------------------->
            //	REPLACED
            //  HEPEVT_Wrapper::set_position(1,0.,0.,0.,0.);
            //  HEPEVT_Wrapper::set_position(index,0.,0.,0.,0.);
            //  by

            double vx   = 0.0;
            double vy   = 0.0;
            double vz   = 0.0;
            double ctau = 0.0;

            HepMC::GenVertex* production_vertex = (*p)->production_vertex();

            //	    std::cout << "Production vertex's pointer = " << production_vertex << std::endl;

            if ( production_vertex ) {
              vx   = production_vertex->position().x();
              vy   = production_vertex->position().y();
              vz   = production_vertex->position().z();
              ctau = production_vertex->position().t();
            }

            HEPEVT_Wrapper::set_position ( index, vx,vy,vz,ctau);

            // V.S.  June 25, 09 <--------------------

          }
          //	  HEPEVT_Wrapper::print_hepevt();
        }
      //      else if ( m_SuperIndex == Sherpa || m_SuperIndex == -999 )
      else if ( m_SuperIndex == Sherpa || m_sherpa_blob )
        {
          sherpa_BLOB(evt);
          m_genEvt=(*evt); //copy to preserve
        }
      else {
        restore_Atlas_HEPEVT();
      }
    }

    // ---<<< V.S. Sept.1, 2008

    //HEPEVT_Wrapper::print_hepevt();
    //    ATH_MSG_DEBUG(" Event in the TDS printout before TAUOLA  \n" << evt->signal_process_id() << (int)TAUOLA << (int)TAUOLA_PHOTOS << (int)PHOTOS << (int)m_GenIndex);
  }

  // Save the random number seeds in the event
  CLHEP::HepRandomEngine*	engine	=	Tauola::p_AtRndmGenSvc->GetEngine(Tauola::tauola_stream);
  const long*		s	=	engine->getSeeds();
  m_seeds.clear();
  m_seeds.push_back(s[0]);
  m_seeds.push_back(s[1]);

  //store pdfinfo
  if (evt->pdf_info()) {
    m_pdfinfo=(*evt->pdf_info());
  }


  // Generate event - i.e. decay taus and resulting resonances


  //  std::cout << "Tauola_i-I-event number = " << evt->event_number() << std::endl;

  //  if ( m_SuperIndex != Sherpa && m_SuperIndex != -999 )
  if ( m_SuperIndex != Sherpa && !m_sherpa_blob )
    {
      //  V.S.  Jan. 17, 2009: prepare to extract coupling from PYTHIA common blocks
      int igen = 0;
      if ( m_SuperIndex == PYTHIA ) {
        igen = 1;
      }
      tauola_setgen_(&igen);

      // (ET) Empty tau details if this is new event
      // Needed to allow Pythia to properly fill tau details here,
      // while allowing that another generator (Herwig) may have already
      // filled this before.
      m_tauDetails.clearNewEvent(evt->event_number());

      //      ATH_MSG_DEBUG(" Event in the TDS printout before TAUOLA  \n" << evt->signal_process_id() << (int)TAUOLA << (int)TAUOLA_PHOTOS << (int)PHOTOS << (int)m_GenIndex);
      //      HEPEVT_Wrapper::print_hepevt();

      int imode = 0;
      tauola_hepevt_(&imode);

      //      ATH_MSG_DEBUG(" Event in the TDS printout AFTER TAUOLA  \n" << evt->signal_process_id() << (int)TAUOLA << (int)TAUOLA_PHOTOS << (int)PHOTOS << (int)m_GenIndex);
      //      HEPEVT_Wrapper::print_hepevt();

    }

  if (m_SuperIndex == HERWIG  || m_SuperIndex == JIMMY)
    {
      m_evwgt = 1.0;
      m_usrwgt=false;

      if ( evt->weights().size() > 0 ) {
	if ( evt->weights().size() >= 10 ) {
	  m_usrwgt=true;
          if (evt->weights().size() >= 28 ) m_isize = 28;
          else m_isize = 10;
          for(int i=0;i<m_isize;i++){
	    m_userwgts[i]=evt->weights().back();
	    evt->weights().pop_back();
	  }
	}

	//	if ( evt->weights().size() > 10 || evt->weights().size() < 10  ) {
        if (evt->weights().size() != 10 && evt->weights().size() != 28) {
	  m_evwgt=evt->weights().back();
	  evt->weights().pop_back();
	}
      }

    }
  else if(m_SuperIndex == PYTHIA)
    {

      m_pari10 = 1.0;
      m_pari9  = 1.0;
      m_pari7  = 1.0;

      int size_w = evt->weights().size();

      if ( size_w >= 1 ) {
        m_pari10=evt->weights().back();
        evt->weights().pop_back();
        if ( size_w >= 2 ) {
          m_pari9=evt->weights().back();
          evt->weights().pop_back();
          if ( size_w >= 3 ) {
            m_pari7=evt->weights().back();
            evt->weights().pop_back();
          }
        }
      }
    }
  else if (m_SuperIndex == SINGLE && !m_sherpa_blob )
    {
    }
  //  else if ( m_SuperIndex == Sherpa || m_SuperIndex == -999 )
  else if ( m_SuperIndex == Sherpa || m_sherpa_blob )
    {
      hybridPartSet.clear();
    }

  // explicitly copy the event number ( & other vals)
  m_eventnum=evt->event_number();
  m_eventscale=evt->event_scale() ;
  m_eventqed=evt->alphaQED() ;
  m_eventqcd=evt->alphaQCD() ;

  //std::cout << " ----------------- Spill evt " << std::endl;
  //  std::cout << "wgt" << evt->event_number()  << std::endl;
  //std::cout << "wgt" << m_avwgt << "  " << m_evwgt << std::endl;
  //std::cout << " ----------------- END " << std::endl;

  //ATH_MSG_DEBUG(" Event in the SvC printout AFTER TAUOLA  \n" );
  // HEPEVT_Wrapper::print_hepevt();

  ++m_events;

  // debug
  ATH_MSG_DEBUG("ATLAS TAUOLA generating done." );
  return status;

}


//---------------------------------------------------------------------------


StatusCode Tauola::genFinalize() {
  int imode = 1;
  tauola_hepevt_(&imode);

  ATH_MSG_INFO(" ATLAS TAUOLA Ending." );
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode Tauola::fillEvt(HepMC::GenEvent* evt) {
  ATH_MSG_DEBUG(" ATLAS TAUOLA Filling... " );

  //  if ( m_SuperIndex != Sherpa && m_SuperIndex != -999 ) {
  if ( m_SuperIndex != Sherpa && !m_sherpa_blob ) {
    store_Atlas_HEPEVT();
  }

  //std::cout << (int)JIMMY << " m_SuperIndex "<< (int)m_SuperIndex << std::endl;

  if (m_SuperIndex == HERWIG || m_SuperIndex == JIMMY) {
    //      std::cout << "Herwig IO" << std::endl;
    HepMC::IO_HERWIG hepio;
    hepio.fill_next_event(evt);

    evt->weights().push_back(m_evwgt);

    if(m_usrwgt){
      for(int i=m_isize-1;i>=0;i--){
	evt->weights().push_back(m_userwgts[i]);
      }
    }

    //evt->weights().push_back(m_avwgt);
    // Convert CLHEP::cm->CLHEP::mm and CLHEP::GeV->CLHEP::MeV
    //   cmTomm(evt);
    GeVToMeV(evt);

    // And fill extra details
    if (m_saveDetails) {
      ATH_MSG_DEBUG(" Tauola::fillEvt - found " << m_tauDetails.size() << " taus for HERWIG");

      // std::cout << "Tauola_i-D-VS-Number of taus to decay = " << m_numberOfTauToDecay << std::endl;

      if ( m_numberOfTauToDecay > 0 ) {
        MsgStream log(msgSvc(), name());
        m_tauDetails.saveHerwig(log, evt);
      }

      // clear the COMMON block "the hard way"
      m_tauDetails.clear();
      m_tauDetails.setEvent(-999);
    }
  }

  else {

    if(m_SuperIndex == PYTHIA) {
      //      std::cout << "Pythia IO" << std::endl;
      HepMC::IO_HEPEVT hepio;
      hepio.set_print_inconsistency_errors(0);
      hepio.fill_next_event(evt);
      evt->weights().push_back(m_pari7);
      evt->weights().push_back(m_pari9);
      evt->weights().push_back(m_pari10);
      // Convert CLHEP::cm->CLHEP::mm and CLHEP::GeV->CLHEP::MeV
      //   cmTomm(evt);
      GeVToMeV(evt);

    }
    //  else if( m_SuperIndex == Sherpa || m_SuperIndex == -999 )
    else if( m_SuperIndex == Sherpa || m_sherpa_blob ) {
      (*evt)=m_genEvt;
    }
    else {
      HepMC::IO_HEPEVT hepio;
      hepio.fill_next_event(evt);
      // Convert CLHEP::cm->CLHEP::mm and CLHEP::GeV->CLHEP::MeV
      //   cmTomm(evt);
      GeVToMeV(evt);

    }

    if ( m_SuperIndex != Sherpa && !m_sherpa_blob ) {
      // And fill extra details, assume HEPEVT order matches HepMC order
      // which is the case for all scenarios except "SHERPA BLOB"
      if (m_saveDetails) {
        ATH_MSG_DEBUG("Tauola::fillEvt - found "
                      << m_tauDetails.size() << " taus for PYTHIA, SINGLE or OTHER but SHERPA");
        MsgStream log(msgSvc(), name());

        m_tauDetails.savePythia(log, evt);
        // clear the COMMON block "the hard way"
        m_tauDetails.clear();
        m_tauDetails.setEvent(-999);
      }
    }

  }

  evt->set_event_number(m_eventnum);
  evt->set_signal_process_id(m_GenIndex);
  evt->set_random_states(m_seeds);
  evt->set_event_scale(m_eventscale) ;
  evt->set_alphaQED(m_eventqed) ;
  evt->set_alphaQCD(m_eventqcd) ;

  //pdfinfo restore
  if (m_pdfinfo.id1() !=0) {
    evt->set_pdf_info(m_pdfinfo);
  }

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode Tauola::restore_Atlas_HEPEVT(void) {
  StatusCode status = StatusCode::SUCCESS;

  ATH_MSG_DEBUG(" ATLAS TAUOLA Filling...  \n" );

  Atlas_HEPEVT* Ahep = 0;

  //std::cout << (int)PYTHIA << "m_SuperIndex"<< (int)m_SuperIndex << std::endl;

  if (m_SuperIndex == HERWIG || m_SuperIndex == JIMMY)
    {
      //std::cout << "Herwig IN" << std::endl;
      std::string keyid="Herwig";
      status = evtStore()->retrieve(Ahep, keyid);
    }
  else if(m_SuperIndex == PYTHIA)
    {
      //std::cout << "Pythia IN" << std::endl;
      std::string keyid = "Pythia";
      status = evtStore()->retrieve(Ahep, keyid);
    }
  else
    {
      status = StatusCode::FAILURE; // !!!!! It wasn't here
    }
  if ( status.isFailure() )
    {
      ATH_MSG_DEBUG("ATLAS TAUOLA - NO HEPEVT RECORD FOUND... \n" );
      return status;
    }

  if(Ahep){
    s_atlas_HEPEVT=(Atlas_HEPEVT*)Ahep;
    s_atlas_HEPEVT->spill();
  }
  else
    status = StatusCode::FAILURE; // !!!!! It wasn't here

  return status;

}


void Tauola::store_Atlas_HEPEVT(void)
{
  //std::cout << "atlas_HEPEVT------" << s_atlas_HEPEVT->nhep()  << std::endl;
  //std::cout << "atlas_HEPEVT------" << s_atlas_HEPEVT->isthep(10)  << std::endl;
  //std::cout << "atlas_HEPEVT------" << s_atlas_HEPEVT->idhep(10)  << std::endl;
  //std::cout << "atlas_HEPEVT------" << s_atlas_HEPEVT->jmohep(1,10)  << std::endl;
  //std::cout << "atlas_HEPEVT------" << s_atlas_HEPEVT->jdahep(2,10)  << std::endl;
  s_atlas_HEPEVT->fill();

  Atlas_HEPEVT* Ahep = new Atlas_HEPEVT();
  *(Ahep)=*(s_atlas_HEPEVT);
  std::string keyid = "AtlasTauola";

  StatusCode status = evtStore()->record(Ahep, keyid);
  if (!status.isSuccess()) ATH_MSG_WARNING(" Could not record Atlas_HEPEVT");
}


//---------------------------------------------------------------------------


StatusCode Tauola::sherpa_BLOB(HepMC::GenEvent* evt) {
  //---------------------------------------------------------------------------
  //
  //  Some remarks by V.S. (July 7, 2009): the code below is unable to
  //  handle events where, e.g., there is an undecayed tau, that has no
  //  parent (only production vertex). Eventually, this code needs to be
  //  rewritten to handle such cases. All this would be needed to handle events
  //  of arbitrary complexity read in from ASCII files, where (yet) undecayed
  //  tau(s) is/are present in event history graph. Notice that (relatively)
  //  simple events of such origin are handled elsewhere.
  //
  ATH_MSG_DEBUG(" TAUOLA: SHERPA BLOB HepMC conversion...  \n" );


  // give photos events in blobs


  std::deque<HepMC::GenParticle*> alltmp_part; //all particles to be skipped

  // vertex iterator to avoid double counts
  for ( HepMC::GenEvent::vertex_iterator v = evt->vertices_begin();
        v != evt->vertices_end(); ++v ) {

    hybridPartSet_ptr_t ptr_h;
    hybridPart_t hybridEntry;

    HepMC::GenParticle* part = 0;

    HepMC::GenParticle* new_par1 = 0;
    HepMC::GenParticle* new_par2 = 0;

    for ( HepMC::GenVertex::particle_iterator
            p=(*v)->particles_begin(HepMC::children);
          p != (*v)->particles_end(HepMC::children);
          ++p ) {
      //skip if already used
      bool p_used= false;
      for ( std::deque<HepMC::GenParticle*>::iterator used=alltmp_part.begin();
            used !=alltmp_part.end();
            used++ ) {
        if ( (*used) == (*p) ) { p_used=true;}
      }
      if (p_used) { continue; }

      if ( ! (*p)->production_vertex() ) { continue ; } // needs a parent
      int p_pdg_id=(*p)->pdg_id();
      int p_stat=(*p)->status();

      //check if this particle tau and stable
      if ( abs(p_pdg_id) == 15 && p_stat == 1 ) {
        part=(*p);
      }
      if ( part ) { break; } // one candidate is all we need?
    } // end of p-loop

    if (  !part) { continue; } // if a stable tau not found skip this vertex


    //find mother and siblings via vertex
    HepMC::GenVertex* true_parent = 0;
    HepMC::GenParticle* part_parent = 0;
    HepMC::GenParticle* part_parent2 = 0;  //hard Z/W

    HepMC::GenVertex* true_desc = 0;
    HepMC::GenParticle* sib_desc = 0;

    //find the true production vertex if there are copies
    true_parent=part->production_vertex();
    bool true_found = false;
    int nparents = 0;
    do {
      bool vjump = false;
      nparents=0;
      for ( HepMC::GenVertex::particle_iterator
              mother= true_parent->particles_begin(HepMC::parents);
            mother != true_parent->particles_end(HepMC::parents);
            ++mother ) {
        if ( (*mother)->pdg_id() == part->pdg_id() && (*mother)->production_vertex() ) {
          true_parent=(*mother)->production_vertex();
          vjump=true;
          break;
        }
      }
      if (!vjump) {
        for ( HepMC::GenVertex::particle_iterator
                mother= true_parent->particles_begin(HepMC::parents);
              mother != true_parent->particles_end(HepMC::parents);
              ++mother ) {
          nparents++;
          true_found = true;
          part_parent=(*mother);
        }
      }
    } while ( !true_found );


    bool needtmp_parent=false;
    if (part_parent->status() != 3 && abs(part_parent->pdg_id())>21 ) {
    } else {
      needtmp_parent=true;
    }

    // now look for sibling
    int p_sib=1;
    if (part->pdg_id() < 0) {
      p_sib=-1;
    }

    sib_desc=0;
    for ( HepMC::GenVertex::particle_iterator
            des=part_parent->end_vertex()->particles_begin(HepMC::children);
          des != part_parent->end_vertex()->particles_end(HepMC::children);
          ++des ) {
      if ( (*des)->pdg_id() == -part->pdg_id() || (*des)->pdg_id() == -(16*p_sib)  ) {
        // sibling found, find the last copy

        //find the final decay vertex if there are copies
        if ( (*des)->end_vertex() ) {
          true_desc=(*des)->end_vertex();
          bool true_found = false;

          do {
            bool vjump = false;
            for ( HepMC::GenVertex::particle_iterator
                    daug= true_desc->particles_begin(HepMC::children);
                  daug != true_desc->particles_end(HepMC::children);
                  ++daug ) {
              if ( (*daug)->pdg_id() == (*des)->pdg_id() && (*daug)->end_vertex() ) {
                true_desc=(*daug)->end_vertex();
                vjump=true;
                break;
              }
            }
            if (!vjump) {
              for ( HepMC::GenVertex::particle_iterator
                      daug= true_desc->particles_begin(HepMC::children);
                    daug != true_desc->particles_end(HepMC::children);
                    ++daug ) {
                if ( (*daug)->pdg_id() == (*des)->pdg_id() ) {
                  true_found = true;
                  sib_desc=(*daug);
                }
              }
            }
          } while ( !true_found );
          break;
        }
        else  {
          sib_desc=(*des);
          break;
        }
        //	 if (!sib_desc) {
        //  std::cout << "sibwrong=" << p_sib << std::endl;
        //}
      }
    }

    //     std::cout << "sibling chosen "  << std::endl;
    //put to stack for double counting checks
    alltmp_part.push_back(part);
    alltmp_part.push_back(sib_desc);
    //sib_desc->print();
    //part->print();

    if (needtmp_parent) {
      //part parent points to the hard vertex
      int nparent = 0;
      for ( HepMC::GenVertex::particle_iterator
              p=true_parent->particles_begin(HepMC::parents);
            p != true_parent->particles_end(HepMC::parents);
            ++p ) {
        nparent++;
        if (nparent==1) { new_par1=(*p); }
        if (nparent==2) { new_par2=(*p); }
        //	 if (nparent >2) { std::cout << "too many parents!" << std::endl; }
        if (nparent >2) { ATH_MSG_FATAL("too many parents!"); }
      }
      if(nparent <= 0 ){
	ATH_MSG_FATAL("No parents!");
	return StatusCode::FAILURE;
      }
      HepMC::FourVector fictive_mom(part->momentum().px(),
				    part->momentum().py(),
				    part->momentum().pz(),
				    part->momentum().e());

      fictive_mom.setPx(fictive_mom.px() + sib_desc->momentum().px());
      fictive_mom.setPy(fictive_mom.py() + sib_desc->momentum().py());
      fictive_mom.setPz(fictive_mom.pz() + sib_desc->momentum().pz());
      fictive_mom.setE(fictive_mom.e() + sib_desc->momentum().e());

      int p_pid=m_pdgid; //add external m_pid
      if ( (part->pdg_id()+sib_desc->pdg_id()) != 0 ) { p_pid=24; }

      part_parent= new HepMC::GenParticle(fictive_mom,p_pid,2);

      if(new_par2){

	HepMC::FourVector fictive_mom2(new_par1->momentum().px(),
				       new_par1->momentum().py(),
				       new_par1->momentum().pz(),
				       new_par1->momentum().e());

	fictive_mom2.setPx(fictive_mom2.px() + new_par2->momentum().px());
	fictive_mom2.setPy(fictive_mom2.py() + new_par2->momentum().py());
	fictive_mom2.setPz(fictive_mom2.pz() + new_par2->momentum().pz());
	fictive_mom2.setE(fictive_mom2.e() + new_par2->momentum().e());

	part_parent2= new HepMC::GenParticle(fictive_mom2,p_pid,3);
      }
    }


    //fill the hepevt entries
    hybridPartSet.clear();
    int mothpos=1;
    int taupos=2;
    if (part_parent2) { mothpos=4; taupos=5; }
    //tau

    int p_bcode=part->barcode();
    int p_pdg_id=part->pdg_id();
    int p_stat=part->status();
    int p_prodvtx=0;
    if (part->production_vertex() && part->production_vertex()->barcode()!=0) {
      p_prodvtx=part->production_vertex()->barcode();}
    int p_endvtx=0;
    if (part->end_vertex() && part->end_vertex()->barcode()!=0) {
      p_endvtx=part->end_vertex()->barcode();}

    hybridEntry.part_ID=p_pdg_id;
    hybridEntry.part_barcode=p_bcode;
    hybridEntry.part_inV=p_prodvtx;
    hybridEntry.part_outV=p_endvtx;
    hybridEntry.part_istat=p_stat;
    hybridEntry.part_moth1=mothpos;
    hybridEntry.part_moth2=mothpos;
    hybridEntry.part_daugh1=0;
    hybridEntry.part_daugh2=0;

    //std::cout << "N " << p_bcode << " id " <<  p_pdg_id << " stat " << p_stat << " pvx " << p_prodvtx << " dvx " << p_endvtx << std::endl;

    double p_px=part->momentum().px()/1000.;
    double p_py=part->momentum().py()/1000.;
    double p_pz=part->momentum().pz()/1000.;
    double p_pe=part->momentum().e()/1000.;
    double p_pm=p_pe*p_pe-p_px*p_px-p_py*p_py-p_pz*p_pz;
    if (p_pm < 0.)
      {
        p_pm=0.;
      }
    p_pm=sqrt(p_pm);
    double p_vx=0.;
    double p_vy=0.;
    double p_vz=0.;
    double p_vt=0.;

    if (p_prodvtx != 0)
      {
        p_vx=part->production_vertex()->position().x();
        p_vy=part->production_vertex()->position().y();
        p_vz=part->production_vertex()->position().z();
        p_vt=part->production_vertex()->position().t();
      }

    hybridEntry.part_Px=p_px;
    hybridEntry.part_Py=p_py;
    hybridEntry.part_Pz=p_pz;
    hybridEntry.part_E=p_pe;
    hybridEntry.part_M=p_pm;
    hybridEntry.part_Vx=p_vx;
    hybridEntry.part_Vy=p_vy;
    hybridEntry.part_Vz=p_vz;
    hybridEntry.part_Vt=p_vt;

    hybridPartSet.push_back(hybridEntry);


    //sibling

    p_bcode=sib_desc->barcode();
    p_pdg_id=sib_desc->pdg_id();
    p_stat=sib_desc->status();
    p_prodvtx=0;
    if (sib_desc->production_vertex() && sib_desc->production_vertex()->barcode()!=0) {
      p_prodvtx=sib_desc->production_vertex()->barcode();}
    p_endvtx=0;
    if (sib_desc->end_vertex() && sib_desc->end_vertex()->barcode()!=0) {
      p_endvtx=sib_desc->end_vertex()->barcode();}

    hybridEntry.part_ID=p_pdg_id;
    hybridEntry.part_barcode=p_bcode;
    hybridEntry.part_inV=p_prodvtx;
    hybridEntry.part_outV=p_endvtx;
    hybridEntry.part_istat=p_stat;
    hybridEntry.part_moth1=mothpos;
    hybridEntry.part_moth2=mothpos;
    hybridEntry.part_daugh1=0;
    hybridEntry.part_daugh2=0;

    //std::cout << "N " << p_bcode << " id " <<  p_pdg_id << " stat " << p_stat << " pvx " << p_prodvtx << " dvx " << p_endvtx << std::endl;

    p_px=sib_desc->momentum().px()/1000.;
    p_py=sib_desc->momentum().py()/1000.;
    p_pz=sib_desc->momentum().pz()/1000.;
    p_pe=sib_desc->momentum().e()/1000.;
    p_pm=p_pe*p_pe-p_px*p_px-p_py*p_py-p_pz*p_pz;
    if (p_pm < 0.)
      {
        p_pm=0.;
      }
    p_pm=sqrt(p_pm);
    p_vx=0.;
    p_vy=0.;
    p_vz=0.;
    p_vt=0.;

    if (p_prodvtx != 0)
      {
        p_vx=sib_desc->production_vertex()->position().x();
        p_vy=sib_desc->production_vertex()->position().y();
        p_vz=sib_desc->production_vertex()->position().z();
        p_vt=sib_desc->production_vertex()->position().t();
      }

    hybridEntry.part_Px=p_px;
    hybridEntry.part_Py=p_py;
    hybridEntry.part_Pz=p_pz;
    hybridEntry.part_E=p_pe;
    hybridEntry.part_M=p_pm;
    hybridEntry.part_Vx=p_vx;
    hybridEntry.part_Vy=p_vy;
    hybridEntry.part_Vz=p_vz;
    hybridEntry.part_Vt=p_vt;

    hybridPartSet.push_back(hybridEntry);

    //and now parent

    p_bcode=part_parent->barcode();
    p_pdg_id=part_parent->pdg_id();
    p_stat=part_parent->status();
    p_prodvtx=0;
    if (part_parent->production_vertex() && part_parent->production_vertex()->barcode()!=0) {
      p_prodvtx=part_parent->production_vertex()->barcode();}
    p_endvtx=0;
    if (part_parent->end_vertex() && part_parent->end_vertex()->barcode()!=0) {
      p_endvtx=part_parent->end_vertex()->barcode();}

    hybridEntry.part_ID=p_pdg_id;
    hybridEntry.part_barcode=p_bcode;
    hybridEntry.part_inV=p_prodvtx;
    hybridEntry.part_outV=p_endvtx;
    hybridEntry.part_istat=p_stat;
    hybridEntry.part_moth1=0;
    hybridEntry.part_moth2=0;
    hybridEntry.part_daugh1=taupos;
    hybridEntry.part_daugh2=taupos+1;
    //std::cout << "N " << p_bcode << " id " <<  p_pdg_id << " stat " << p_stat << " pvx " << p_prodvtx << " dvx " << p_endvtx << std::endl;

    p_px=part_parent->momentum().px()/1000.;
    p_py=part_parent->momentum().py()/1000.;
    p_pz=part_parent->momentum().pz()/1000.;
    p_pe=part_parent->momentum().e()/1000.;
    p_pm=p_pe*p_pe-p_px*p_px-p_py*p_py-p_pz*p_pz;
    if (p_pm < 0.)
      {
        p_pm=0.;
      }
    p_pm=sqrt(p_pm);
    p_vx=0.;
    p_vy=0.;
    p_vz=0.;
    p_vt=0.;

    if (p_prodvtx != 0)
      {
        p_vx=part_parent->production_vertex()->position().x();
        p_vy=part_parent->production_vertex()->position().y();
        p_vz=part_parent->production_vertex()->position().z();
        p_vt=part_parent->production_vertex()->position().t();
      }

    hybridEntry.part_Px=p_px;
    hybridEntry.part_Py=p_py;
    hybridEntry.part_Pz=p_pz;
    hybridEntry.part_E=p_pe;
    hybridEntry.part_M=p_pm;
    hybridEntry.part_Vx=p_vx;
    hybridEntry.part_Vy=p_vy;
    hybridEntry.part_Vz=p_vz;
    hybridEntry.part_Vt=p_vt;

    hybridPartSet.push_front(hybridEntry);


    //and now hard record if there
    if (part_parent2) {
      p_bcode=part_parent2->barcode();
      p_pdg_id=part_parent2->pdg_id();
      p_stat=part_parent2->status();
      p_prodvtx=0;
      if (part_parent2->production_vertex() && part_parent2->production_vertex()->barcode()!=0) {
        p_prodvtx=part_parent2->production_vertex()->barcode();}
      p_endvtx=0;
      if (part_parent2->end_vertex() && part_parent2->end_vertex()->barcode()!=0) {
        p_endvtx=part_parent2->end_vertex()->barcode();}

      hybridEntry.part_ID=p_pdg_id;
      hybridEntry.part_barcode=p_bcode;
      hybridEntry.part_inV=p_prodvtx;
      hybridEntry.part_outV=p_endvtx;
      hybridEntry.part_istat=p_stat;
      hybridEntry.part_moth1=1;
      hybridEntry.part_moth2=2;
      hybridEntry.part_daugh1=3;
      hybridEntry.part_daugh2=3;

      //std::cout << "N " << p_bcode << " id " <<  p_pdg_id << " stat " << p_stat << " pvx " << p_prodvtx << " dvx " << p_endvtx << std::endl;

      p_px=part_parent2->momentum().px()/1000.;
      p_py=part_parent2->momentum().py()/1000.;
      p_pz=part_parent2->momentum().pz()/1000.;
      p_pe=part_parent2->momentum().e()/1000.;
      p_pm=p_pe*p_pe-p_px*p_px-p_py*p_py-p_pz*p_pz;
      if (p_pm < 0.)
        {
          p_pm=0.;
        }
      p_pm=sqrt(p_pm);
      p_vx=0.;
      p_vy=0.;
      p_vz=0.;
      p_vt=0.;

      if (p_prodvtx != 0)
        {
          p_vx=part_parent2->production_vertex()->position().x();
          p_vy=part_parent2->production_vertex()->position().y();
          p_vz=part_parent2->production_vertex()->position().z();
          p_vt=part_parent2->production_vertex()->position().t();
        }

      hybridEntry.part_Px=p_px;
      hybridEntry.part_Py=p_py;
      hybridEntry.part_Pz=p_pz;
      hybridEntry.part_E=p_pe;
      hybridEntry.part_M=p_pm;
      hybridEntry.part_Vx=p_vx;
      hybridEntry.part_Vy=p_vy;
      hybridEntry.part_Vz=p_vz;
      hybridEntry.part_Vt=p_vt;

      hybridPartSet.push_front(hybridEntry);

      //second hard incoming
      p_bcode=new_par2->barcode();
      p_pdg_id=new_par2->pdg_id();
      p_stat=new_par2->status();
      p_prodvtx=0;
      if (new_par2->production_vertex() && new_par2->production_vertex()->barcode()!=0) {
        p_prodvtx=new_par2->production_vertex()->barcode();}
      p_endvtx=0;
      if (new_par2->end_vertex() && new_par2->end_vertex()->barcode()!=0) {
        p_endvtx=new_par2->end_vertex()->barcode();}

      hybridEntry.part_ID=p_pdg_id;
      hybridEntry.part_barcode=p_bcode;
      hybridEntry.part_inV=p_prodvtx;
      hybridEntry.part_outV=p_endvtx;
      hybridEntry.part_istat=p_stat;
      hybridEntry.part_moth1=0;
      hybridEntry.part_moth2=0;
      hybridEntry.part_daugh1=3;
      hybridEntry.part_daugh2=3;

      //std::cout << "N " << p_bcode << " id " <<  p_pdg_id << " stat " << p_stat << " pvx " << p_prodvtx << " dvx " << p_endvtx << std::endl;

      p_px=new_par2->momentum().px()/1000.;
      p_py=new_par2->momentum().py()/1000.;
      p_pz=new_par2->momentum().pz()/1000.;
      p_pe=new_par2->momentum().e()/1000.;
      p_pm=p_pe*p_pe-p_px*p_px-p_py*p_py-p_pz*p_pz;
      if (p_pm < 0.)
        {
          p_pm=0.;
        }
      p_pm=sqrt(p_pm);
      p_vx=0.;
      p_vy=0.;
      p_vz=0.;
      p_vt=0.;

      if (p_prodvtx != 0)
        {
          p_vx=new_par2->production_vertex()->position().x();
          p_vy=new_par2->production_vertex()->position().y();
          p_vz=new_par2->production_vertex()->position().z();
          p_vt=new_par2->production_vertex()->position().t();
        }

      hybridEntry.part_Px=p_px;
      hybridEntry.part_Py=p_py;
      hybridEntry.part_Pz=p_pz;
      hybridEntry.part_E=p_pe;
      hybridEntry.part_M=p_pm;
      hybridEntry.part_Vx=p_vx;
      hybridEntry.part_Vy=p_vy;
      hybridEntry.part_Vz=p_vz;
      hybridEntry.part_Vt=p_vt;

      hybridPartSet.push_front(hybridEntry);

      //first hard incoming
      p_bcode=new_par1->barcode();
      p_pdg_id=new_par1->pdg_id();
      p_stat=new_par1->status();
      p_prodvtx=0;
      if (new_par1->production_vertex() && new_par1->production_vertex()->barcode()!=0) {
        p_prodvtx=new_par1->production_vertex()->barcode();}
      p_endvtx=0;
      if (new_par1->end_vertex() && new_par1->end_vertex()->barcode()!=0) {
        p_endvtx=new_par1->end_vertex()->barcode();}

      hybridEntry.part_ID=p_pdg_id;
      hybridEntry.part_barcode=p_bcode;
      hybridEntry.part_inV=p_prodvtx;
      hybridEntry.part_outV=p_endvtx;
      hybridEntry.part_istat=p_stat;
      hybridEntry.part_moth1=0;
      hybridEntry.part_moth2=0;
      hybridEntry.part_daugh1=3;
      hybridEntry.part_daugh2=3;

      //std::cout << "N " << p_bcode << " id " <<  p_pdg_id << " stat " << p_stat << " pvx " << p_prodvtx << " dvx " << p_endvtx << std::endl;

      p_px=new_par1->momentum().px()/1000.;
      p_py=new_par1->momentum().py()/1000.;
      p_pz=new_par1->momentum().pz()/1000.;
      p_pe=new_par1->momentum().e()/1000.;
      p_pm=p_pe*p_pe-p_px*p_px-p_py*p_py-p_pz*p_pz;
      if (p_pm < 0.)
        {
          p_pm=0.;
        }
      p_pm=sqrt(p_pm);
      p_vx=0.;
      p_vy=0.;
      p_vz=0.;
      p_vt=0.;

      if (p_prodvtx != 0)
        {
          p_vx=new_par1->production_vertex()->position().x();
          p_vy=new_par1->production_vertex()->position().y();
          p_vz=new_par1->production_vertex()->position().z();
          p_vt=new_par1->production_vertex()->position().t();
        }

      hybridEntry.part_Px=p_px;
      hybridEntry.part_Py=p_py;
      hybridEntry.part_Pz=p_pz;
      hybridEntry.part_E=p_pe;
      hybridEntry.part_M=p_pm;
      hybridEntry.part_Vx=p_vx;
      hybridEntry.part_Vy=p_vy;
      hybridEntry.part_Vz=p_vz;
      hybridEntry.part_Vt=p_vt;

      hybridPartSet.push_front(hybridEntry);
    }

    // number the positions
    int ipos=1;
    for ( ptr_h = hybridPartSet.begin();
          ptr_h != hybridPartSet.end();
          ptr_h++ )
      {
        (*ptr_h).part_ipos=ipos;
        ipos++;
      }

    // now fill hepevt
    int nent=0;
    for ( ptr_h = hybridPartSet.begin();
          ptr_h != hybridPartSet.end();
          ptr_h++ )
      {
        nent++;
        HEPEVT_Wrapper::set_status(nent,(*ptr_h).part_istat);
        HEPEVT_Wrapper::set_id(nent,(*ptr_h).part_ID);
        HEPEVT_Wrapper::set_parents(nent,(*ptr_h).part_moth1,(*ptr_h).part_moth2);
        HEPEVT_Wrapper::set_children(nent,(*ptr_h).part_daugh1,(*ptr_h).part_daugh2);
        HEPEVT_Wrapper::set_momentum(nent,(*ptr_h).part_Px,(*ptr_h).part_Py,(*ptr_h).part_Pz,(*ptr_h).part_E);
        HEPEVT_Wrapper::set_mass(nent,(*ptr_h).part_M);
        HEPEVT_Wrapper::set_position(nent,(*ptr_h).part_Vx,(*ptr_h).part_Vy,(*ptr_h).part_Vz,(*ptr_h).part_Vt);
      }
    HEPEVT_Wrapper::set_event_number(evt->event_number());
    HEPEVT_Wrapper::set_number_entries(nent);

    //     ATH_MSG_DEBUG(" TAUOLA: SHERPA BLOB HepMC in...  \n" );
    //     HEPEVT_Wrapper::print_hepevt();

    int imode = 0;
    tauola_hepevt_(&imode);

    //     ATH_MSG_DEBUG(" TAUOLA: SHERPA BLOB HepMC out...  \n" );
    //     HEPEVT_Wrapper::print_hepevt();

    //fill back HepMC - add tau decay tree
    part->set_status(2);
    bool p_done=false;
    HepMC::GenParticle* ppar = part;
    HepMC::GenParticle* pdesc = 0;
    int index=taupos;
    int numit=0;
    int numax=0;
    int istack[30];
    std::vector<HepMC::GenParticle*> pstack;
    do {
      HepMC::GenVertex *new_vert= new HepMC::GenVertex();
      evt->add_vertex( new_vert);
      new_vert->add_particle_in( ppar );
      int ifirst=HEPEVT_Wrapper::first_child(index);
      // add vertex position
      HepMC::FourVector prod_pos( HEPEVT_Wrapper::x(ifirst), HEPEVT_Wrapper::y(ifirst),
                                  HEPEVT_Wrapper::z(ifirst), HEPEVT_Wrapper::t(ifirst) );
      new_vert->set_position(prod_pos);

      for (int i=HEPEVT_Wrapper::first_child(index); i <= HEPEVT_Wrapper::last_child(index); i++)
        {
          pdesc
            = new HepMC::GenParticle( HepMC::FourVector( HEPEVT_Wrapper::px(i)*1000.,
                                                         HEPEVT_Wrapper::py(i)*1000.,
                                                         HEPEVT_Wrapper::pz(i)*1000.,
                                                         HEPEVT_Wrapper::e(i)*1000. ),
                                      HEPEVT_Wrapper::id(i),
                                      HEPEVT_Wrapper::status(i) );
          new_vert->add_particle_out( pdesc );
          if ( HEPEVT_Wrapper::number_children(i)>0 ) { numit++; istack[numit]=i; pstack.push_back(pdesc); }
        }
      if ( numit > numax ) {
        numax++;
        index=istack[numax];
        //	 std::cout << "new loop " << numax <<" istack" << index << std::endl;
        ppar=pstack.at(numax-1);
        //reconnect parents!
      } else {
        p_done=true;
      }
    } while (!p_done);
    //fill back HepMC - add sibling decay tree if tau
    if (abs(sib_desc->pdg_id())==15) {
      sib_desc->set_status(2);
      p_done=false;
      ppar=sib_desc;
      pdesc=0;
      index=taupos+1;
      numit=0;
      numax=0;
      pstack.clear();
      do {
        HepMC::GenVertex *new_vert= new HepMC::GenVertex();
        evt->add_vertex( new_vert);
        new_vert->add_particle_in( ppar );
        int ifirst=HEPEVT_Wrapper::first_child(index);
        // add vertex position
        HepMC::FourVector prod_pos( HEPEVT_Wrapper::x(ifirst), HEPEVT_Wrapper::y(ifirst),
                                    HEPEVT_Wrapper::z(ifirst), HEPEVT_Wrapper::t(ifirst) );
        new_vert->set_position(prod_pos);

        for (int i=HEPEVT_Wrapper::first_child(index); i <= HEPEVT_Wrapper::last_child(index); i++)
          {
            pdesc
              = new HepMC::GenParticle( HepMC::FourVector( HEPEVT_Wrapper::px(i)*1000.,
                                                           HEPEVT_Wrapper::py(i)*1000.,
                                                           HEPEVT_Wrapper::pz(i)*1000.,
                                                           HEPEVT_Wrapper::e(i)*1000. ),
                                        HEPEVT_Wrapper::id(i),
                                        HEPEVT_Wrapper::status(i) );
            new_vert->add_particle_out( pdesc );
            if ( HEPEVT_Wrapper::number_children(i)>0 ) { numit++; istack[numit]=i; pstack.push_back(pdesc); }
          }
        if ( numit > numax ) {
          numax++;
          index=istack[numax];
          //std::cout << "new loop " << numax <<" istack" << index << std::endl;
          ppar=pstack.at(numax-1);
          //reconnect parents!
        } else {
          p_done=true;
        }
      } while (!p_done);
    }
  }
  StatusCode status = StatusCode::SUCCESS;

  return status;

}
