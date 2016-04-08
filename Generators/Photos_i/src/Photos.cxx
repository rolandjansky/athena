//--------------------------------------------------------------------------
// File and Version Information:
//      Photos.cxx
//
// Description:
//      Interface to Photos for Herwig 6.5 and Pythia 6.2+
//      to modify the HEPEVT common.
//
// Author List:
// Borut Paul Kersevan (BPK), June 2003
//
//  May 2008:   Vladimir Savinov (V.S.) and Reza Yoosoofmiya (R.Y.) made this work for single LQ production in Pythia
//  June 10'08: V.S. and R.Y. fixed a problem where descendants of siblings that co-parent exotic objects
//              such as clusters, strings and such get boosted too many times therefore corrupting their four-momenta
//  June 25'08: V.S. and R.Y. fixed the problem that arose from color flow reconnection in tuning of Pythia used for MC8
//  June 27'08: V.S. made the conditions on NOT boosting less restrictive. June 25'08's restrictive conditions resulted
//              in 3.8% rejection rate for SM events Z->tau tau (as compared to 0.5% before that). With June 27, 2008's
//              improvement, SM Z->tau tau rejection rate is back to 0.5%
//  Sept.19'08: V.S. protected against infinite loop conditions, either when the event is empty or when a particle whose parent could radiate has no siblings.
//  Sept.23'08: V.S. changed diagnostics level for empty vertex from WARNING to DEBUG, as it is "normal" for HERWIG/MC@NLO/JIMMY,
//              also, clarified the reported condition in the body of the (now DEBUG) message
//  July 7'09:  V.S.: this algorithm can now handle more than one particle in "single-particle" MC.
//  Nov.4, 2009: V.S. further improved unfolding convoluted event history graphs.
//  Nov.11,2009: V.S.: Protection against insufficient length of evt->weights() (i.e. evt->weights().size())
//  Dec.15,2009: V.S.: Protection against an infinite loop for "hidden" orphan particles (whose parents could be a candidate for QED FSR).
//
// Copyright Information:
//
// accidentally left out of the picture
//
//------------------------------------------------------------------------

#include "Photos_i/Photos.h"

#include "TruthUtils/GeneratorName.h"

#include "GaudiKernel/DataSvc.h"
#include "GeneratorObjects/McEventCollection.h"

//#include "HepMC/ParticleDataTable.h"
//#include "HepMC/ParticleData.h"
#include "HepMC/IO_HEPEVT.h"
#include "HepMC/HEPEVT_Wrapper.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "GeneratorUtils/StringParse.h"
#include <cstdlib>

// Random numbers
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

using namespace std;

int PrintChildTree_photo( const HepMC::GenParticle* p, int tabN);


// Random number service
/// @todo Eliminate some statics and other badness cf. CavernBkgGen
IAtRndmGenSvc* Photos::p_AtRndmGenSvc;
std::string	Photos::photos_stream = "PHOTOS_INIT";


// Calls to fortran routines
extern "C" double phoran_(int* /*idummy*/) {
  /// @todo Eliminate some statics and other badness cf. CavernBkgGen
  CLHEP::HepRandomEngine* engine = Photos::p_AtRndmGenSvc->GetEngine(Photos::photos_stream);
  return CLHEP::RandFlat::shoot(engine);
}

extern "C" {
  //  void photos_hepevt_(int*);
  void photos_(int*);
  void phoini_();
  //  void pylist_(int*);
  //  void hwuepr_();
}


//--------------------------------------------------------------------------


Photos::Photos(const std::string& name, ISvcLocator* pSvcLocator)
  : GenModule(name,pSvcLocator),
    m_fortout(0),
    m_events(0),
    m_eventnum(0),
    m_eventscale(0.),
    m_eventqed(0.),
    m_eventqcd(0.),
    m_GenIndex(0),
    m_SuperIndex(0),
    m_SubIndex(0),
    m_avwgt(0.),
    m_evwgt(0.),
    m_pari7(0.),
    m_pari9(0.),
    m_pari10(0.),
    m_genEvt(1,1)
{
  photos_stream	= "PHOTOS_INIT";
  declareProperty("PhotosCommand", m_photosCommandVector);

}


//--------------------------------------------------------------------------


Photos::~Photos()
{
}


//--------------------------------------------------------------------------


Ffphot& Photos::ffphot() {
  return m_ffphot;
}

Ffpholun& Photos::ffpholun() {
  return m_ffpholun;
}


//---------------------------------------------------------------------------


StatusCode Photos::genInitialize() {
  ATH_MSG_INFO(" ATLAS PHOTOS INITIALISING.  \n");

  /// @todo Replace with GenEvent::atRndmGenSvc()
  static const bool CREATEIFNOTTHERE(true);
  StatusCode RndmStatus = service("AtRndmGenSvc", Photos::p_AtRndmGenSvc, CREATEIFNOTTHERE);
  if (!RndmStatus.isSuccess() || 0 == Photos::p_AtRndmGenSvc) {
    ATH_MSG_ERROR(" Could not initialize Random Number Service");
    return RndmStatus;
  }
  Photos::photos_stream	= "PHOTOS_INIT";


  // Set up default values for input parameters
  m_fortout = 28;
  this->ffpholun().phlun() = m_fortout;
  this->ffphot().ifpmode() = 2;
  this->ffphot().ffphcut() = 0.01;
  this->ffphot().ffalpha() = -1.;
  this->ffphot().ifinter() = 1;
  this->ffphot().ifsec() = 1;
  this->ffphot().ifitre() = 0;
  this->ffphot().ifiexp() = 1;
  this->ffphot().iftops() = 0;

  for (unsigned int i = 0; i < m_photosCommandVector.size(); i++) {
    ATH_MSG_INFO(" Command is: " << m_photosCommandVector[i]);
    StringParse mystring(m_photosCommandVector[i]);
    string myblock = mystring.piece(1);
    string myentry = mystring.piece(2);
    string mystr = mystring.piece(3);
    int myint1 = mystring.intpiece(3);
    double  myfl1 = mystring.numpiece(3);

    // Note that Photos needs doubles, hence the convert here
    ATH_MSG_INFO(myblock << " block  " << myentry << " item  " << myint1 << "  value " << myfl1);

    if (myblock=="photos") {
      if (myentry == "pmode") {
        this->ffphot().ifpmode() = myint1;
      }
      else if (myentry == "xphcut"){
        this->ffphot().ffphcut() = myfl1;
      }
      else if (myentry == "alpha"){
        this->ffphot().ffalpha() = myfl1;
      }
      else if (myentry == "interf"){
        this->ffphot().ifinter() = myint1;
      }
      else if (myentry == "isec"){
        this->ffphot().ifsec() = myint1;
      }
      else if (myentry == "itre"){
        this->ffphot().ifitre() = myint1;
      }
      else if (myentry == "iexp"){
        this->ffphot().ifiexp() = myint1;
      }
      else if (myentry == "iftop"){
        this->ffphot().iftops() = myint1;
      }
      else {
        ATH_MSG_ERROR("ATLAS PHOTOS ERROR, entry PHOTOS has PMODE XPHCUT ALPHA INTERF ISEC ITRE IEXP IFTOP: YOU HAVE SPECIFIED "<< myentry);
      }
    }
    else {
      ATH_MSG_ERROR(" ERROR in PHOTOS PARAMETERS   " << myblock << " is an invalid common block name !");
    }
  }

  // Save the PHOTOS_INIT stream seeds....
  CLHEP::HepRandomEngine* engine = Photos::p_AtRndmGenSvc->GetEngine(Photos::photos_stream);
  const long* sip = engine->getSeeds();
  long int si1 = sip[0];
  long int si2 = sip[1];

  // Photos init
  phoini_();

  // ... and set them back to the stream for proper save
  Photos::p_AtRndmGenSvc->CreateStream(si1, si2, Photos::photos_stream);

  Photos::photos_stream	= "PHOTOS";

  m_events = 0;

  HepMC::HEPEVT_Wrapper::set_sizeof_int(4);
  HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
  HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode Photos::callGenerator() {
  ATH_MSG_DEBUG(" ATLAS PHOTOS generating.  \n");

  StatusCode status = StatusCode::SUCCESS;

  // Save the random number seeds in the event
  // photos_stream is now "PHOTOS"
  CLHEP::HepRandomEngine* engine = Photos::p_AtRndmGenSvc->GetEngine(Photos::photos_stream);
  const long* s	= engine->getSeeds();
  m_seeds.clear();
  m_seeds.push_back(s[0]);
  m_seeds.push_back(s[1]);

  // The MC event to process by Photos is the last one stored in the event store
  HepMC::GenEvent* evt = events()->back();
  if (evt == NULL) {
    ATH_MSG_ERROR("McEvent was not successfully created");
    return StatusCode::FAILURE;
  } else {
    // Temporary fix --->>>  V.S.  Sept.19, 2008
    int pr_id = evt->signal_process_id();
    if (pr_id == 1) {
      pr_id = SINGLE;
    }

    // Count vertices
    /// @todo Use GenEvent::vertices_size()
    int n_vtx(0);
    for ( HepMC::GenEvent::vertex_iterator v = evt->vertices_begin();
          v != evt->vertices_end(); ++v ) {
      n_vtx++;
    }

    // Count MC particles
    /// @todo Use GenEvent::particles_size()
    int n_part(0);
    for ( HepMC::GenEvent::particle_const_iterator p = evt->particles_begin();
          p != evt->particles_end(); ++p ) {
      ATH_MSG_DEBUG("Found MC particle with PDG id = " << (*p)->pdg_id());
      n_part++;
    }

    if ( pr_id == 0 ) {
      // V.S. July 7, 2009
      // if ( n_vtx == n_part ) {
      if ( n_vtx == n_part || ( n_vtx == 1 && n_part >= 1 ) ) {
        ATH_MSG_DEBUG("This MC event has " << n_vtx << " MC vertices and " << n_part << " MC particle(s). "
                      << "However, no MC generator information is available, so we assume this is 'single-particle' MC (that could contain more than one particle).");
        pr_id = SINGLE;
      } else {
        ATH_MSG_WARNING("Found " << n_vtx << " MC vertices and " << n_part << " MC particles in this event. "
                        << "However, no MC generator information is available, so we assume that this is 'single-particle' MC (that could contain more than one particle). "
                        << "If this event contains information about event history, such information will be lost due to limitations of the algorithm (Photos_i). "
                        << "As the result, ANYTHING could happen including infinite loops! You have been warned. Please report this incident but do not expect a quick fix.");
        pr_id = SINGLE;
      }
    }

    ATH_MSG_DEBUG("signal_process_id = " << pr_id << ", "
                  << "first_generator = " << first_generator(pr_id)	<< ", "
                  << "second generator = " << second_generator(pr_id) << ", "
                  << "third generator = " << third_generator(pr_id)	);
    ATH_MSG_DEBUG("n_vtx = " << n_vtx << ", n_part = " << n_part);

    //    m_GenIndex = evt->signal_process_id() + PHOTOS;
    m_GenIndex = pr_id + PHOTOS;
    // ---<<<  V.S.  Sept.19, 2008

    m_seeds.clear();
    // std::copy(evt->random_states().begin(), evt->random_states().end(), back_inserter(m_seeds));
    //
    //    int pr_id=evt->signal_process_id();
    m_SuperIndex = first_generator(pr_id);
    //std::cout << "m_SuperIndex" << (int) m_SuperIndex << std::endl;

    int tau_id = third_generator(pr_id);
    m_SubIndex = tau_id;

    //
    // --->>>  V.S.  Sept.19, 2008
    bool call_sherpa_BLOB = true;
    if ( n_part == 0 ) {
      ATH_MSG_WARNING("Empty event identified! No particles, no FSR! "
                      << "The implications for the remaining part of MC chain are unpredictable. Take a note: you have been warned. "
                      << "Event info: n_vtx = " << n_vtx << ", n_part = " << n_part);
      call_sherpa_BLOB = false;
    }
    if (call_sherpa_BLOB) {
      status=sherpa_BLOB(evt);

      if ( status.isFailure() ){
	ATH_MSG_WARNING("sherpa_BLOB fails!");
	return status;
      }
    }
    // ---<<<  V.S.  Sept.19, 2008

    m_genEvt = (*evt); //copy to preserve
  }

  // debug
  //ATH_MSG_DEBUG("Event in the SVC before PHOTOS  \n");
  //HepMC::HEPEVT_Wrapper::print_hepevt();

  if (m_SuperIndex == HERWIG || m_SuperIndex == JIMMY) {
    // m_avwgt=evt->weights().back();
    // evt->weights().pop_back();
    m_evwgt = 1.0;
    if ( evt->weights().size() != 0 ) {
      m_evwgt=evt->weights().back();
      evt->weights().pop_back();
    }

    //std::cout << " ----------------- Print evt " << std::endl;
    //std::cout << "wgt" << m_avwgt << "  " << m_evwgt << std::endl;
    //std::cout << " ----------------- END " << std::endl;
  }
  else if(m_SuperIndex == PYTHIA) {
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

  m_eventnum = evt->event_number();
  m_eventscale = evt->event_scale() ;
  m_eventqed = evt->alphaQED() ;
  m_eventqcd = evt->alphaQCD() ;

  //ATH_MSG_DEBUG(" Event in the SVC after PHOTOS  \n");
  //HepMC::HEPEVT_Wrapper::print_hepevt();

  ++m_events;

  // debug
  ATH_MSG_DEBUG(" ATLAS PHOTOS generating done.  \n");
  return status;

}


//---------------------------------------------------------------------------


StatusCode Photos::genFinalize() {
  //  int imode = 1;
  //  photos_hepevt_(&imode);
  ATH_MSG_INFO(" ATLAS PHOTOS Ending...  \n");
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode Photos::fillEvt(HepMC::GenEvent* evt) {
  ATH_MSG_DEBUG(" ATLAS PHOTOS Filling...  \n");
  //HepMC::HEPEVT_Wrapper::print_hepevt();

  (*evt) = m_genEvt;

  evt->set_event_number(m_eventnum);
  evt->set_signal_process_id(m_GenIndex);
  evt->set_random_states(m_seeds);
  evt->set_event_scale(m_eventscale) ;
  evt->set_alphaQED(m_eventqed) ;
  evt->set_alphaQCD(m_eventqcd) ;

  //  evt->print();

  // std::cout << "Number of particles in HepMC " <<  evt->particles_size() << std::endl;
  // ATH_MSG_INFO(" Event in the SvC printout in fillEvt  \n");
  // HepMC::HEPEVT_Wrapper::print_hepevt();

  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------


StatusCode Photos::sherpa_BLOB(HepMC::GenEvent* evt) {
  ATH_MSG_DEBUG(" PHOTOS: Gen_BLOB HepMC conversion...  \n");

  // Give photos events in blobs
  std::deque<HepMC::GenParticle*> alltmp_part; // all particles to be skipped

  // V.S.  June 25, 2008:  search for indications of color flow
  // reconnections achieved using Pythia strings, clusters and such

  // ATH_MSG_DEBUG("Printing out the entire event: ");
  // evt->print();

  int nodes = 0;

  // the list of offending nodes
  std::set<int> node_barcodes;

  // analyze all MC vertices
  for ( HepMC::GenEvent::vertex_iterator vertex = evt->vertices_begin();
        vertex != evt->vertices_end(); ++vertex ) {

    int n_children = (*vertex)->particles_out_size();

    // these objects, in my experience, are "produced" alone...
    if ( n_children == 1 ) {

      HepMC::GenParticle* object = (*((*vertex)->particles_begin(HepMC::children)));

      int child_pdg_id = object->pdg_id();

      if ( child_pdg_id == 92 || child_pdg_id == 91 || child_pdg_id == 88 || child_pdg_id == 90 ) {

        // a new node identified via the vertex where it was produced
        nodes++;

        ATH_MSG_DEBUG("Identified another node worth investigating"
                      << ", with pdg_id = " << child_pdg_id
                      << ", production vertex barcode = " << (*vertex)->barcode()
                      << ", current number of identified nodes = " << nodes);

        HepMC::GenParticle* true_parent = 0;

        // list of barcodes of production vertices for true parent(s) of the object in question
        std::set<int> parent_prod_vertex_barcodes;

        // now look at parent particles that make up the object in this vertex
        for ( HepMC::GenVertex::particle_iterator parent = (*vertex)->particles_begin(HepMC::parents);
              parent != (*vertex)->particles_end(HepMC::parents); ++parent ) {

          true_parent = (*parent);

          int parent_pdg_id = 0;
          int parent_barcode = -1;

          HepMC::GenVertex* parent_prod_vertex;
          // now jump up until we get to the actual parent
          bool true_parent_found = false;
          do {

            parent_pdg_id = true_parent->pdg_id();
            parent_barcode = true_parent->barcode();
            parent_prod_vertex = true_parent->production_vertex();

            if ( !parent_prod_vertex ) {
              break;
            }

            int n_in = parent_prod_vertex->particles_in_size();
            int n_out = parent_prod_vertex->particles_out_size();

            if ( n_in == 1 && n_out == 1 ) {
              HepMC::GenParticle* tmp_parent = (*(parent_prod_vertex->particles_begin(HepMC::parents)));

              int parent_pdg_id_saved = parent_pdg_id;
              parent_pdg_id = tmp_parent->pdg_id();

              ATH_MSG_DEBUG("Travelling up the tree, parent_old, parent_new pdg_ids = "
                            << parent_pdg_id_saved << ", " << parent_pdg_id);

              true_parent = tmp_parent;
            }
            else {
              true_parent_found = true;
            }
          } while ( !true_parent_found );

          if ( parent_prod_vertex ) {

            parent_prod_vertex_barcodes.insert(parent_prod_vertex->barcode());

            ATH_MSG_DEBUG(" Investigated next parent with pdg_id = "
                          << parent_pdg_id
                          << ", parent barcode = "
                          << parent_barcode
                          << ", parent production vertex barcode = "
                          << parent_prod_vertex->barcode()
                          << ", the length of parent production vertex barcodes list = "
                          << parent_prod_vertex_barcodes.size());

          }
        }

        // check the size of the list
        if ( parent_prod_vertex_barcodes.size() > 1 ) {

          ATH_MSG_DEBUG(" Found offending node with pdg_id = "
                        << child_pdg_id
                        << ", object production vertex barcode = "
                        << (*vertex)->barcode()
                        << ", the length of parent production vertex barcodes list = "
                        << parent_prod_vertex_barcodes.size());

          // save barcode of this node in the list of offending nodes
          node_barcodes.insert(object->barcode());

        }
      }
    }
  }

  ATH_MSG_DEBUG(" The number of strings, clusters and such involved in color flow reconnection = "
                << node_barcodes.size());

  // vertex iterator to avoid double counts
  for ( HepMC::GenEvent::vertex_iterator v = evt->vertices_begin();
        v != evt->vertices_end(); ++v ) {

    // --->>> V.S.  Sept.19, 2008
    // How many parents? How many siblings?

    int n_in = (*v)->particles_in_size();
    int n_out = (*v)->particles_out_size();

    // this special condition means that the vertex does not really contain anything at all!
    if ( n_in == 0 && n_out <= 1 ) {
      ATH_MSG_DEBUG("Empty vertex or a vertex with one outgoing particle and no parents identified. "
                    << "This vertex is being skipped, because FSR would be impossible to generate. "
                    << "There is a chance that this event is completely empty - printing its contents could result in an exception (i.e. program's instant death). "
                    << "Please make sure you know what you are doing. "
                    << "The implications for the remaining part of MC chain are unpredictable. Take a note: you have been warned. "
                    << "Vertex info: n_in = " << n_in << ", n_out = " << n_out);
      continue;
    }

    // ---<<< V.S.  Sept.19, 2008

    hybridPartSet_t hybridPartSet; // in Photos it can be local
    hybridPartSet_ptr_t ptr_h;
    hybridPart_t hybridEntry;

    HepMC::GenParticle* part = 0;
    HepMC::GenVertex* vert = 0;

    HepMC::GenVertex* tmp_vert=0;
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

      if ( ! (*p)->production_vertex() ) { continue ; } // need a parent

      // ---> VS, Dec.15, 2009 --------------------------------------------------------------------
      //  However, sadly, it could be more complex than that...
      {
        HepMC::GenParticle* particle = (*p);

        if ( particle->status() == 3 ) { continue ; }

        int idabs = abs(particle->pdg_id());

        // check if this particle could be a daughter of a particle that could experience QED FSR in its decay

        if ( ( ( idabs > 9 && idabs < 40 ) || ( idabs > 100 ) ) &&
             ( ( idabs != 21 ) &&  ( idabs != 22 ) &&  ( idabs != 111 ) &&
               ( idabs != 2101 ) &&  ( idabs != 3101 ) &&  ( idabs != 3201 ) &&
               ( idabs != 1103 ) &&  ( idabs != 2103 ) &&  ( idabs != 2203 ) &&
               ( idabs != 3103 ) &&  ( idabs != 3203 ) &&  ( idabs != 3303 ) ) ) {

          HepMC::GenVertex*   particle_prod_vertex = particle->production_vertex();

          bool found_true_parent = false;
          int parent_barcode = 0;

          HepMC::GenVertex::particle_iterator parentItr;
          HepMC::GenParticle* particle_parent;

          do {

            if ( !particle_prod_vertex ) break;
            if ( particle_prod_vertex->particles_in_size() == 0 ) break;

            for ( parentItr = particle_prod_vertex->particles_begin(HepMC::parents);
                  parentItr != particle_prod_vertex->particles_end(HepMC::parents);
                  ++parentItr ) {

              particle_parent = (*parentItr);
              parent_barcode = particle_parent->barcode();

              if ( particle_parent->pdg_id() == particle->pdg_id() && particle_parent->production_vertex() ) {

                particle_prod_vertex = particle_parent->production_vertex();
                break;

              }
              else {
                // found a parent that's not the daughter particle itself!
                found_true_parent = true;
              }
            }

          } while ( !found_true_parent );

          if ( !found_true_parent ) {

            ATH_MSG_DEBUG("Unable to investigate QED FSR in parent's decay as no true parent found!");
            ATH_MSG_DEBUG("particle->pdg_id()  = " << particle->pdg_id());
            ATH_MSG_DEBUG("particle->barcode() = " << particle->barcode());
            ATH_MSG_DEBUG("Top level ascendant's barcode = " << parent_barcode);

            continue;

          }
        }
      }
      //
      // <--- VS, Dec.15, 2009 --------------------------------------------------------------------

      int p_pdg_id=(*p)->pdg_id();
      int p_stat=(*p)->status();

      //check if this particle can radiate
      int idabs = abs(p_pdg_id);
      if (p_stat == 3) { continue ; }
      if ( ( ( idabs > 9 && idabs < 40 ) || ( idabs > 100 ) ) &&
           ( ( idabs != 21 ) &&  ( idabs != 22 ) &&  ( idabs != 111 ) &&
             ( idabs != 2101 ) &&  ( idabs != 3101 ) &&  ( idabs != 3201 ) &&
             ( idabs != 1103 ) &&  ( idabs != 2103 ) &&  ( idabs != 2203 ) &&
             ( idabs != 3103 ) &&  ( idabs != 3203 ) &&  ( idabs != 3303 ) ) ) {

        if ( (*p)->end_vertex() ) {
          // check for duplicates above/below it
          bool p_copy=false;
          for ( HepMC::GenVertex::particle_iterator
                  des=(*p)->end_vertex()->particles_begin(HepMC::children);
                des != (*p)->end_vertex()->particles_end(HepMC::children);
                ++des ) {
            if ( (*des)->pdg_id() == p_pdg_id ) {
              // copy found, skip
              p_copy=true;
              break;
            }
          }
          if (!p_copy) {
            part=(*p);
            break;
          }
        }
        else { //stable
          part=(*p);
        }
      }
      if ( part ) { break; } // one candidate is all we need?
    } // end of p-loop

    if (  !part) { continue; } // if a proper particle not found skip this vertex

    //find mother and siblings via vertex
    HepMC::GenVertex* true_parent = 0;
    HepMC::GenParticle* part_parent = 0;

    HepMC::GenVertex* true_desc = 0;
    HepMC::GenParticle* sib_desc = 0;

    //---------------------------------------------------------------------------------------------------

    //find the true production vertex if there are copies
    true_parent=part->production_vertex();
    bool true_parent_found = false;

    int part_barcode_true = 0;

    int nparents = 0;
//    bool jumped = false;
    do {
      bool vjump = false;
      nparents=0;
      for ( HepMC::GenVertex::particle_iterator
              mother= true_parent->particles_begin(HepMC::parents);
            mother != true_parent->particles_end(HepMC::parents);
            ++mother ) {
        if ( (*mother)->pdg_id() == part->pdg_id() && (*mother)->production_vertex() ) {
          part_barcode_true=(*mother)->barcode();
          true_parent=(*mother)->production_vertex();
          vjump=true;
//          jumped = true;
          break;
        }
      }
      if (!vjump) {
        for ( HepMC::GenVertex::particle_iterator
                mother= true_parent->particles_begin(HepMC::parents);
              mother != true_parent->particles_end(HepMC::parents);
              ++mother ) {
          nparents++;
          true_parent_found = true;
          part_parent=(*mother);
        }
      }
    } while ( !true_parent_found );

    //---------------------------------------------------------------------------------------------------

    bool needtmp_v=false;
    if (nparents != 1) {
      //continue;
      // if number of parents > 1 this is a special vertex
      //herwig b decays?
      //look for a lepton if not found
      int lpid = abs(part->pdg_id());
      if ( lpid < 11 || lpid > 16 ) { //find a lepton
        HepMC::GenParticle* lept = 0;
        for ( HepMC::GenVertex::particle_iterator
                p=(*v)->particles_begin(HepMC::children);
              p != (*v)->particles_end(HepMC::children);
              ++p ) {
          if ((*p)->status() == 3) { continue ; }

          //check if a lepton
          int idabs = abs((*p)->pdg_id());
          if ( idabs > 10 && idabs < 17 ) { //lepton found

            //skip if already used
            bool p_used= false;
            for ( std::deque<HepMC::GenParticle*>::iterator used=alltmp_part.begin();
                  used !=alltmp_part.end();
                  used++ ) {
              if ( (*used) == (*p) ) { p_used=true;}
            }
            if (p_used) { continue; }

            if ( (*p)->end_vertex() ) {
              // check for duplicates below it
              bool p_copy=false;
              for ( HepMC::GenVertex::particle_iterator
                      des=(*p)->end_vertex()->particles_begin(HepMC::children);
                    des != (*p)->end_vertex()->particles_end(HepMC::children);
                    ++des ) {
                if ( (*des)->pdg_id() == (*p)->pdg_id() ) {
                  // copy found, skip
                  p_copy=true;
                  break;
                }
              }
              if (!p_copy) {
                lept=(*p);
                break;
              }
            }
            else { //stable
              lept=(*p);
            }
          }
          if ( lept ) { break; } // one candidate is all we need?
        } // end of lepton p-loop

        if (  !lept) { continue; } // if a lepton particle not found skip this vertex

        part=lept;
      }

      //have to add exceptions for leptons
      int pid = abs(part->pdg_id());
      if (  pid == 11 || pid == 12 || pid == 13 ||  pid == 14 || pid == 15 || pid == 16 ) {
        // check if there is a common boson/hadron origin (pythia, herwig.. TO DO)
        for ( HepMC::GenVertex::particle_iterator
                anc=part->production_vertex()->particles_begin(HepMC::ancestors);
              anc !=part->production_vertex()->particles_end(HepMC::ancestors);
              ++anc ) {
          int aid = abs((*anc)->pdg_id());
          if ( ( ( aid > 16 && aid < 40 ) || ( aid > 100 ) ) &&
               ( ( aid != 21 ) &&  ( aid != 22 ) &&  ( aid != 2212 ) &&
                 ( aid != 2101 ) &&  ( aid != 3101 ) &&  ( aid != 3201 ) &&
                 ( aid != 1103 ) &&  ( aid != 2103 ) &&  ( aid != 2203 ) &&
                 ( aid != 3103 ) &&  ( aid != 3203 ) &&  ( aid != 3303 ) ) ) {
            if ((*anc)->status() != 3 ) {
              //		 std::cout << "boson/whatever found " << pid << std::endl;
              part_parent=(*anc);
              needtmp_v=true;
              break;
            }
          }
        }
        // if no adequate origin found look for lepton partner and construct a temp vertex
        // find a sibling from hard vertex or boson/hadron (part_parent should point there?)
        int rem = abs(part->pdg_id() %2 );
        int p_sib=0;
        int p_sib2=0;
        if (rem == 1) {
          p_sib=-part->pdg_id();
          if ( part->pdg_id() > 0 ) { //neutrinos
            p_sib2=-(part->pdg_id()+1);
          }
          else {
            p_sib2=-(part->pdg_id()-1);
          }
        }
        else {
          p_sib=-part->pdg_id();
          if ( part->pdg_id() > 0 ) { //leptons
            p_sib2=-(part->pdg_id()-1);
          }
          else {
            p_sib2=-(part->pdg_id()+1);
          }
        }
        // now look for sibling in hard_vert
        sib_desc=0;
        for ( HepMC::GenVertex::particle_iterator
                des=part_parent->end_vertex()->particles_begin(HepMC::children);
              des != part_parent->end_vertex()->particles_end(HepMC::children);
              ++des ) {

          // don't include the particle itself in the list of siblings!
          if ( (*des)->barcode() != part_barcode_true ) {

            //
            //  V.Savinov and R.Yoosoofmiya, April 3, 2008, a fix to allow single leptoquark production process in Pythia
            //
            //	     if ( (*des)->pdg_id() == p_sib || (*des)->pdg_id() == p_sib2 ) {
            // E.M.L Dec. 2008 further fix to allow for e e* production
            //if ( (*des)->pdg_id() == p_sib || (*des)->pdg_id() == p_sib2 || abs((*des)->pdg_id()) == 42){

            // Nov.3, 2009	       if ( (*des)->pdg_id() == p_sib || (*des)->pdg_id() == p_sib2 || abs((*des)->pdg_id()) == 42 || abs((*des)->pdg_id()) == 4000011) {

            int barcode_sibling = (*des)->pdg_id();

            if (
                barcode_sibling == p_sib   ||
                barcode_sibling == p_sib2  ||
                abs(barcode_sibling) == 42 ||
                abs(barcode_sibling) == 4000011 ||
                abs(barcode_sibling) == 11 ||
                abs(barcode_sibling) == 13 ||
                abs(barcode_sibling) == 15 ||
                abs(barcode_sibling) == 12 ||
                abs(barcode_sibling) == 14 ||
                abs(barcode_sibling) == 16) {

              ATH_MSG_DEBUG("ACHTUNG! ATTENTION! WARNING! Will assume that particle with barcode "
                            << part->barcode()
                            << " and pdg_id = "
                            << part->pdg_id()
                            << " and particle with barcode "
                            << (*des)->barcode()
                            << " and pdg_id = "
                            << (*des)->pdg_id()
                            << " came from the same decay of a (fictitious) Z. This is a kludge. You have been warned.");

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
                            daug= true_desc->particles_begin(HepMC::parents);
                          daug != true_desc->particles_end(HepMC::parents);
                          ++daug ) {
                      if ( (*daug)->pdg_id() == (*des)->pdg_id() ) {
                        true_found = true;
                        sib_desc=(*daug);
                      }
                    }
                    // if sibling as parent check for stable daughter in this vertex
                    // taus have decay products as children and e/mu stable ones
                    if (true_found) {
                      for ( HepMC::GenVertex::particle_iterator
                              daug= true_desc->particles_begin(HepMC::children);
                            daug != true_desc->particles_end(HepMC::children);
                            ++daug ) {
                        if ( (*daug)->pdg_id() == (*des)->pdg_id() && (*daug)->status() == 1) {
                          sib_desc=(*daug);
                        }
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
	      //	      if (!sib_desc) {
	      //                  std::cout << "sibwrong=" << p_sib << std::endl;
	      //	      }
            }
          }
        }

        // Nov.3, 2009, V.S.
        if ( sib_desc == 0 ) {
          ATH_MSG_FATAL("ACHTUNG! ATTENTION! Fatal error condition! No sibling found!"
                        << " Please report this condition to vladimirsavinov@gmail.com "
                        << " Emergency return from sherpa_BLOB is going to be performed."
                        << " Anything could happen. Do NOT ignore this message! You have been warned.");
          return StatusCode::SUCCESS;
        }

        //std::cout << "sib chosen " << p_sib << std::endl;
        //put to stack for double counting checks
        alltmp_part.push_back(part);
        alltmp_part.push_back(sib_desc);
        // assume we have the sibling, construct a fictive parent as a Z0
        CLHEP::HepLorentzVector fictive_mom(part->momentum().px(),
                                     part->momentum().py(),
                                     part->momentum().pz(),
                                     part->momentum().e());
        fictive_mom.setPx(fictive_mom.px() + sib_desc->momentum().px());
        fictive_mom.setPy(fictive_mom.py() + sib_desc->momentum().py());
        fictive_mom.setPz(fictive_mom.pz() + sib_desc->momentum().pz());
        fictive_mom.setE(fictive_mom.e() + sib_desc->momentum().e());

        if (!needtmp_v) { // boson/hadron not found
          needtmp_v=true;
          part_parent= new HepMC::GenParticle(fictive_mom,23,2);

          double p_px=part_parent->momentum().px()/1000.;
          double p_py=part_parent->momentum().py()/1000.;
          double p_pz=part_parent->momentum().pz()/1000.;
          double p_pe=part_parent->momentum().e()/1000.;

          double p_pm=sqrt(p_pe*p_pe-p_px*p_px-p_py*p_py-p_pz*p_pz);

          ATH_MSG_DEBUG("Creating a fictitious vertex assuming the Z boson with mass = " << p_pm
                        << " This is a kludge! You have been warned!");

        }
        tmp_vert = new HepMC::GenVertex();
        tmp_vert->set_position(part->production_vertex()->position());
        tmp_vert->add_particle_in(part_parent);
        new_par1 = new HepMC::GenParticle(*part);
        new_par1->set_status(2);
        part->production_vertex()->add_particle_out(new_par1);
        new_par2 = new HepMC::GenParticle(*sib_desc);
        new_par2->set_status(2);
        sib_desc->production_vertex()->add_particle_out(new_par2);
        tmp_vert->add_particle_out(part);
        tmp_vert->add_particle_out(sib_desc);
        // continue;
      }
      else {
        continue;
      }
    }

    if (needtmp_v) {
      vert=tmp_vert;
    }
    else {
      //	vert=(*v);
      vert=true_parent;
    }

    // fill parent and particle with siblings

    //start with siblings
    int ndesc = 0;

    HepMC::GenParticle* sib = 0;

    // V.S and R.Y think the name 'descendants' is confusing. Renaming this to 'siblings'
    //     std::vector<HepMC::GenParticle*> descendants;
    std::vector<HepMC::GenParticle*> siblings;

    // V.S and R.Y: to keep track of siblings who co-parent "exotic" objects such as clusters and strings
    std::set<HepMC::GenParticle*> already_accounted_for; // will be used to store siblings for later checks

    //  V.S. and R.Y.: the line with CLHEP::HepLorentzVector is for releases 13 and earlier,
    //  the line with FourVector is for releases 14 and newer -
    //  this difference is due to the new HepMC (v 2.0 and later) which
    //  has its own class for four-vectors
    //     std::vector<CLHEP::HepLorentzVector> siblings_old_mom; // will be used tp store 4-moms of siblings later
    std::vector<HepMC::FourVector> siblings_old_mom; // will be used tp store 4-moms of siblings later

    for ( HepMC::GenVertex::particle_iterator p = vert->particles_begin(HepMC::children);
          p != vert->particles_end(HepMC::children);
          ++p ) {

      sib = (*p);

      //
      //  V.S. and R.Y.: this piece of code is supposed to traverse the "tree" of particle's "decay"
      //  into itself in downward direction, e.g.   CLHEP::g->CLHEP::g->CLHEP::g->CLHEP::g
      //  - in such cases we are only interested in the very last copy of the particle
      //  notice that this is so regardless of the number of parents or having "jumped"
      // while looking for the true parent (when exists)
      //
      // look for stable descendants
      //       if (nparents==1 && jumped)
      {
        bool p_copy;
        do
          {
            p_copy=false;
            if ( sib->end_vertex() ) {
              // check for duplicates below it
              for ( HepMC::GenVertex::particle_iterator
                      des=sib->end_vertex()->particles_begin(HepMC::children);
                    des != sib->end_vertex()->particles_end(HepMC::children);
                    ++des ) {
                if ( (*des)->pdg_id() == sib->pdg_id() ) {
                  // copy found, skip
                  p_copy=true;
                  sib=(*des);
                  break;
                }
              }
            }
          } while (p_copy);
      }

      alltmp_part.push_back(sib); // put on stack for double counting checks
      alltmp_part.push_back(*p);

      // descendants.push_back(sib);
      siblings.push_back(sib);
      siblings_old_mom.push_back(sib->momentum());

      int p_bcode=sib->barcode();
      int p_pdg_id=sib->pdg_id();
      int p_stat=sib->status();
      int p_prodvtx=0;
      if ((*p)->production_vertex() && (*p)->production_vertex()->barcode()!=0) {
        p_prodvtx=(*p)->production_vertex()->barcode();
      }
      int p_endvtx=0;

      if (sib->end_vertex() && sib->end_vertex()->barcode()!=0) {
        p_endvtx=sib->end_vertex()->barcode();
      }

      hybridEntry.part_ID=p_pdg_id;
      hybridEntry.part_barcode=p_bcode;
      hybridEntry.part_inV=p_prodvtx;
      hybridEntry.part_outV=p_endvtx;
      hybridEntry.part_istat=p_stat;
      hybridEntry.part_moth1=1;
      hybridEntry.part_moth2=1;
      hybridEntry.part_daugh1=0;
      hybridEntry.part_daugh2=0;

      // std::cout << "N " << p_bcode << " id " <<  p_pdg_id << " stat " << p_stat << " pvx " << p_prodvtx << " dvx " << p_endvtx << std::endl;

      double p_px=sib->momentum().px()/1000.;
      double p_py=sib->momentum().py()/1000.;
      double p_pz=sib->momentum().pz()/1000.;
      double p_pe=sib->momentum().e()/1000.;
      double p_pm=p_pe*p_pe-p_px*p_px-p_py*p_py-p_pz*p_pz;
      if (p_pm < 0.) {
        p_pm=0.;
      }
      p_pm=sqrt(p_pm);
      double p_vx=0.;
      double p_vy=0.;
      double p_vz=0.;
      double p_vt=0.;

      if (p_prodvtx != 0) {
        p_vx=(*p)->production_vertex()->position().x();
        p_vy=(*p)->production_vertex()->position().y();
        p_vz=(*p)->production_vertex()->position().z();
        p_vt=(*p)->production_vertex()->position().t();
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

      //possibly some filtering needed? status=3 involved?
      hybridPartSet.push_back(hybridEntry);
      ndesc++;
    }

    //and now parent

    int p_bcode=part_parent->barcode();
    int p_pdg_id=part_parent->pdg_id();
    int p_stat=part_parent->status();
    int p_prodvtx=0;
    if (part_parent->production_vertex() && part_parent->production_vertex()->barcode()!=0) {
      p_prodvtx=part_parent->production_vertex()->barcode();
    }
    int p_endvtx=0;
    if (part_parent->end_vertex() && part_parent->end_vertex()->barcode()!=0) {
      p_endvtx=part_parent->end_vertex()->barcode();
    }

    hybridEntry.part_ID=p_pdg_id;
    hybridEntry.part_barcode=p_bcode;
    hybridEntry.part_inV=p_prodvtx;
    hybridEntry.part_outV=p_endvtx;
    hybridEntry.part_istat=p_stat;
    hybridEntry.part_moth1=0;
    hybridEntry.part_moth2=0;
    hybridEntry.part_daugh1=2;
    hybridEntry.part_daugh2=ndesc+1;

    //std::cout << "N " << p_bcode << " id " <<  p_pdg_id << " stat " << p_stat << " pvx " << p_prodvtx << " dvx " << p_endvtx << std::endl;

    double p_px=part_parent->momentum().px()/1000.;
    double p_py=part_parent->momentum().py()/1000.;
    double p_pz=part_parent->momentum().pz()/1000.;
    double p_pe=part_parent->momentum().e()/1000.;
    double p_pm=p_pe*p_pe-p_px*p_px-p_py*p_py-p_pz*p_pz;

    if (p_pm < 0.) {
      p_pm=0.;
    }
    p_pm=sqrt(p_pm);
    double p_vx=0.;
    double p_vy=0.;
    double p_vz=0.;
    double p_vt=0.;

    if (p_prodvtx != 0) {
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

    // number the positions
    int ipos=1;
    for ( ptr_h = hybridPartSet.begin();
          ptr_h != hybridPartSet.end();
          ptr_h++ ) {
      (*ptr_h).part_ipos=ipos;
      ipos++;
    }

    // for ( ptr_h = hybridPartSet.begin();
    //       ptr_h != hybridPartSet.end();
    //       ptr_h++ ) {
    //   std::cout << " hybridSet ID " << (*ptr_h).part_ID
    //             << " ipos " << (*ptr_h).part_ipos
    //             << " stat " << (*ptr_h).part_istat
    //             << " inV " << (*ptr_h).part_inV  << " outV " << (*ptr_h).part_outV
    //             << " moth1 " << (*ptr_h).part_moth1  << " moth2 " << (*ptr_h).part_moth2
    //             << " daugh1 " << (*ptr_h).part_daugh1  << " daugh2 " << (*ptr_h).part_daugh2
    //             << std::endl;
    // }

    //now fill HEPEVT
    int nent=0;
    for ( ptr_h = hybridPartSet.begin();
          ptr_h != hybridPartSet.end();
          ptr_h++ ) {
      nent++;
      HepMC::HEPEVT_Wrapper::set_status(nent,(*ptr_h).part_istat);
      HepMC::HEPEVT_Wrapper::set_id(nent,(*ptr_h).part_ID);
      HepMC::HEPEVT_Wrapper::set_parents(nent,(*ptr_h).part_moth1,(*ptr_h).part_moth2);
      HepMC::HEPEVT_Wrapper::set_children(nent,(*ptr_h).part_daugh1,(*ptr_h).part_daugh2);
      HepMC::HEPEVT_Wrapper::set_momentum(nent,(*ptr_h).part_Px,(*ptr_h).part_Py,(*ptr_h).part_Pz,(*ptr_h).part_E);
      HepMC::HEPEVT_Wrapper::set_mass(nent,(*ptr_h).part_M);
      HepMC::HEPEVT_Wrapper::set_position(nent,(*ptr_h).part_Vx,(*ptr_h).part_Vy,(*ptr_h).part_Vz,(*ptr_h).part_Vt);
    }
    HepMC::HEPEVT_Wrapper::set_event_number(evt->event_number());
    HepMC::HEPEVT_Wrapper::set_number_entries(nent);

    // ATH_MSG_DEBUG(" PHOTOS: Gen_BLOB HepMC in...  \n");
    // std::cout << " in " << std::endl;
    // HepMC::HEPEVT_Wrapper::print_hepevt();

    //call photos
    int ippos = 1;
    photos_(&ippos);

    //          ATH_MSG_DEBUG(" PHOTOS: Gen_BLOB HepMC out...  \n");
    //            std::cout << " out " << std::endl;
    //            HepMC::HEPEVT_Wrapper::print_hepevt();

    //get output and de-code to the same state, update the vertex if necessary
    if ( HepMC::HEPEVT_Wrapper::number_entries() > nent ) {
      //photons created, added at the end
      // incoming should remain unchanged, the momenta of outgoing particles should have changed
      HepMC::GenVertex* new_vert=0;
      if (needtmp_v) {
        new_vert= new HepMC::GenVertex();
        evt->add_vertex( new_vert);
        new_vert->set_position(vert->position());
        std::list<HepMC::GenParticle*> outpart;
        for ( HepMC::GenVertex::particle_iterator
                p=vert->particles_begin(HepMC::children);
              p != vert->particles_end(HepMC::children);
              ++p ) {
          outpart.push_back(*p);
        }
        for (std::list<HepMC::GenParticle*>::iterator oitr = outpart.begin();
             oitr != outpart.end(); oitr++) {
          new_vert->add_particle_out(*oitr);
        }
        new_vert->add_particle_in(new_par1);
        new_vert->add_particle_in(new_par2);
      } else {
        new_vert=vert;
      }

      int index=2;

      // V.S  R.Y addition
      for(std::vector<HepMC::GenParticle*>::const_iterator p=siblings.begin();
          p!=siblings.end();
          p++) {
        (*p)->set_momentum( CLHEP::HepLorentzVector( HepMC::HEPEVT_Wrapper::px(index)*1000.,
                                              HepMC::HEPEVT_Wrapper::py(index)*1000.,
                                              HepMC::HEPEVT_Wrapper::pz(index)*1000.,
                                              HepMC::HEPEVT_Wrapper::e(index)*1000. ) );

        // V.S. and R.Y.: we should probably propagate this change up the tree in case the parent is the particle itself!
        index++;
      }

      // for ( HepMC::GenVertex::particle_iterator
      //   p=new_vert->particles_begin(HepMC::children);
      //   p != new_vert->particles_end(HepMC::children);
      //   ++p )


      //rz test
      //      std::vector<HepMC::GenParticle*>::const_iterator pp = siblings.begin();
      //      HepMC::GenVertex::particle_iterator ppp= (*pp)->production_vertex()->particles_begin(HepMC::parents);
      //      int tabs =1;
      //      PrintChildTree_photo( ( *ppp), tabs);
      //rz

      for (std::vector<HepMC::GenParticle*>::const_iterator p=siblings.begin();
           p!=siblings.end();
           p++) {

        // the sublist of offending nodes
        std::set<int> node_barcodes_now;

        if ( (*p)->end_vertex() ) {
          for ( HepMC::GenVertex::particle_iterator
                  des =(*p)->end_vertex()->particles_begin(HepMC::descendants);
                des  !=(*p)->end_vertex()->particles_end(HepMC::descendants);
                ++des ) {
            std::set<int>::iterator itr = node_barcodes.find( (*des)->barcode() );
            if ( itr != node_barcodes.end() ) {
              // save barcode of this descendants in the sublist of offending nodes
              node_barcodes_now.insert((*des)->barcode());
            }
          }
        }

        //checking if p is in the list
        //----------------------------
        bool p_is_already_accounted_for = false;

        std::set<HepMC::GenParticle*>::iterator itr;

        itr = already_accounted_for.find(*p);

        if(  (*itr)==(*p) ){
          p_is_already_accounted_for = true;
          // std::cout << " Already accounted for sibling with barcode " << (*p)->barcode() << std::endl;
        }
        else {
          // std::cout << "NOT yet accounted for sibling with barcode "  << (*p)->barcode() << std::endl;
        }
        //----------------------------

        // notice that when a sibling is its own daughter (just a bookkeeping artifact),
        // *p is pointing to the lowest "itself" in such "tree"!
        if ( (*p)->end_vertex() && !p_is_already_accounted_for ) {
          //get first child of p
          HepMC::GenVertex::particle_iterator q = (*p)->end_vertex()->particles_begin(HepMC::children);

          //initialize following variables to zero
          double old_sum_Px = 0;
          double old_sum_Py = 0;
          double old_sum_Pz = 0;
          double old_sum_E = 0;

          double new_sum_Px = 0;
          double new_sum_Py = 0;
          double new_sum_Pz = 0;
          double new_sum_E = 0;

          //now iterate over all parents of q
          //	    for ( HepMC::GenVertex::particle_iterator
          //		    parent= (*q)->production_vertex()->particles_begin(HepMC::parents);
          //		  parent != (*q)->production_vertex()->particles_end(HepMC::parents);
          //		  ++parent ) {

          // V.S. and R.Y.  June 25, 2008: iterate over all ancestors of q
          for ( HepMC::GenVertex::particle_iterator
                  ancestor = (*q)->production_vertex()->particles_begin(HepMC::ancestors);
                ancestor != (*q)->production_vertex()->particles_end(HepMC::ancestors);
                ++ancestor ) {

            //get position of current ancestor in vector 'siblings'
            int pos_index = -1;
            for (unsigned int pp = 0;  pp < siblings.size(); pp++) {
              if (siblings.at(pp) == (*ancestor) ) pos_index = pp;
            } // got position

            if ( pos_index >= 0 ) {
              old_sum_Px += siblings_old_mom.at(pos_index).px();
              old_sum_Py += siblings_old_mom.at(pos_index).py();
              old_sum_Pz += siblings_old_mom.at(pos_index).pz();
              old_sum_E  += siblings_old_mom.at(pos_index).e();

              new_sum_Px += (*ancestor)->momentum().px();
              new_sum_Py += (*ancestor)->momentum().py();
              new_sum_Pz += (*ancestor)->momentum().pz();
              new_sum_E  += (*ancestor)->momentum().e();

              already_accounted_for.insert(*ancestor);

            }
          }//finished iterating over ancestors

          if ( old_sum_E != 0.0 && new_sum_E != 0.0 ) {

            double oldbx = - old_sum_Px / old_sum_E ;
            double oldby = - old_sum_Py / old_sum_E ;
            double oldbz = - old_sum_Pz / old_sum_E ;

            double newbx =  new_sum_Px / new_sum_E;
            double newby =  new_sum_Py / new_sum_E;
            double newbz =  new_sum_Pz / new_sum_E;

            for ( HepMC::GenVertex::particle_iterator
                    des =(*p)->end_vertex()->particles_begin(HepMC::descendants);
                  des  !=(*p)->end_vertex()->particles_end(HepMC::descendants);
                  ++des ) {

              if((*p)->pdg_id()==(*des)->pdg_id()) { // && old_mom==des_mom)
                (*des)->set_momentum((*p)->momentum());
              }
              else {

                // // Make sure that this descendant is not an offender itself!
                // std::set<int>::iterator itr = node_barcodes.find( (*des)->barcode() );
                // if ( itr != node_barcodes.end() ) {

                std::set<int>::iterator itr = node_barcodes_now.find( (*des)->barcode() );
                if ( itr != node_barcodes_now.end() ) {

                  ATH_MSG_DEBUG("No boost for this descendant! It is in the banned list itself!"
                                << " pdg_id = " << (*des)->pdg_id()
                                << ", barcode = " << (*des)->barcode());

                  // HepMC::HEPEVT_Wrapper::print_hepevt();
                  // evt->print();

                } else {

                  int idabs = abs((*des)->pdg_id());
                  int status = abs((*des)->status());

                  double px = (*des)->momentum().px();
                  double py = (*des)->momentum().py();
                  double pz = (*des)->momentum().pz();
                  double en = (*des)->momentum().e();

                  if ( px == 0.0 && py == 0.0 && pz == 0.0 && en == 0 && ((*des)->end_vertex()) == 0 && status == 2 ) {
                    ATH_MSG_DEBUG("Photos_i/PHOTOS: unstable dummy descendant that never decayed with id = " << idabs
                                  << ", THIS PARTICLE WILL NOT BE BOOSTED!");
                  }
                  else {

                    // keep track of how many ancestors are in the list of offending objects
                    int n_match = 0;

                    // do this analysis for PYTHIA only
                    if ( m_SuperIndex == PYTHIA ) {

                      // do this complex test only if the current sibling is a quark, a gluon or a "node"

                      int id = abs((*p)->pdg_id());

                      if ( id == 92 || id == 91 || id == 88 || id == 90 || id == 21 || ( id >= 1 && id <= 8 ) ) {

                        for ( HepMC::GenVertex::particle_iterator
                                ancestor = (*des)->production_vertex()->particles_begin(HepMC::ancestors);
                              ancestor != (*des)->production_vertex()->particles_end(HepMC::ancestors);
                              ++ancestor ) {

                          // first check if this ancestor is in the banned list itself
                          int des_barcode = (*des)->barcode();

                          //			std::set<int>::iterator itr = node_barcodes.find( (*ancestor)->barcode() );
                          //			if ( itr != node_barcodes.end() ) {

                          std::set<int>::iterator itr = node_barcodes_now.find( (*ancestor)->barcode() );
                          if ( itr != node_barcodes_now.end() ) {

                            n_match++;

                            ATH_MSG_DEBUG("Descendant's ancestor is in the banned list! "
                                          << "Descendant's barcode, pdg_id = "
                                          << des_barcode << ", "
                                          << (*des)->pdg_id()
                                          << ", ancestor's barcode, pdg_id, status = "
                                          << (*ancestor)->barcode() << ", "
                                          << (*ancestor)->pdg_id() << ", "
                                          << (*ancestor)->status()
                                          << ", sibling's barcode, pdg_id, status = "
                                          << (*p)->barcode() << ", "
                                          << (*p)->pdg_id() << ", "
                                          << (*p)->status());
                          }

                          else {
                            ATH_MSG_DEBUG("This descendant will be boosted! Didn't find descendant's ancestors in the banned list."
                                          << " Descendant's pdg_id, status, barcode = "
                                          << (*des)->pdg_id() << ", "
                                          << (*des)->status() << ", "
                                          << (*des)->barcode()
                                          << ", sibling's pdg_id, status, barcode = "
                                          << (*p)->pdg_id() << ", "
                                          << (*p)->status() << ", "
                                          << (*p)->barcode());
                          }
                        }
                      }
                    }

                    if ( n_match != 0 ) {

                      ATH_MSG_DEBUG("No boost for this descendant! "
                                    << "The number of descendant's ancestors in the banned list = " << n_match
                                    << ", descendant's pdg_id, status, barcode = "
                                    << (*des)->pdg_id() << ", "
                                    << (*des)->status() << ", "
                                    << (*des)->barcode()
                                    << ", sibling's pdg_id, status, barcode = "
                                    << (*p)->pdg_id() << ", "
                                    << (*p)->status() << ", "
                                    << (*p)->barcode());

                      // HepMC::HEPEVT_Wrapper::print_hepevt();
                      // evt->print();

                    }
                    // do deboosting/boosting only for descendants of good ancestry
                    else {

                      CLHEP::HepLorentzVector des_mom((*des)->momentum().px(),
                                               (*des)->momentum().py(),
                                               (*des)->momentum().pz(),
                                               (*des)->momentum().e());

                      // This is the last line of defense: even if we are COMPLETELY wrong about EVERYTHING,
                      // this piece of code should save the day. However, because we have no idea what else
                      // could have gone wrong, we report the conditions diagnosed below

                      double mass_1 = des_mom.m();

                      des_mom.boost(oldbx,oldby,oldbz);

                      double mass_2 = des_mom.m();

                      bool broken = false;

                      // Check for NaNs
                      if ( std::isnan(des_mom.px()) || std::isinf(des_mom.px()) ||
                           std::isnan(des_mom.py()) || std::isinf(des_mom.py()) ||
                           std::isnan(des_mom.pz()) || std::isinf(des_mom.pz()) ||
                           std::isnan(des_mom.e())  || std::isinf(des_mom.e()) ) {

                        broken = true;

                        ATH_MSG_INFO(" Descendant's momentum broken after first boost! Leave this descendant as is."
                                     << " Descendant's pdg_id, barcode = "
                                     << (*des)->pdg_id() << ", "
                                     << (*des)->barcode()
                                     << ", sibling's barcode, pdg_id = "
                                     << (*p)->barcode() << ", "
                                     << (*p)->pdg_id());
                      }

                      if ( !broken ) {

                        des_mom.boost(newbx,newby,newbz);

                        double mass_3 = des_mom.m();

                        // Check for NaNs again
                        if ( std::isnan(des_mom.px()) || std::isinf(des_mom.px()) ||
                             std::isnan(des_mom.py()) || std::isinf(des_mom.py()) ||
                             std::isnan(des_mom.pz()) || std::isinf(des_mom.pz()) ||
                             std::isnan(des_mom.e())  || std::isinf(des_mom.e()) ) {

                          broken = true;

                          ATH_MSG_INFO(" Descendant's momentum broken after second boost! Leave this descendant as is."
                                       << " Descendant's pdg_id, barcode = "
                                       << (*des)->pdg_id() << ", "
                                       << (*des)->barcode()
                                       << ", sibling's barcode, pdg_id = "
                                       << (*p)->barcode() << ", "
                                       << (*p)->pdg_id());

                        }

                        if ( !broken ) {

                          // do not allow large distortion of Lorentz-invariant quantities!

                          double delta_1 = fabs( mass_2 - mass_1 );
                          double delta_2 = fabs( mass_3 - mass_2 );

                          if ( delta_1 <= 1.0 && delta_2 <= 1.0 ) {

                            (*des)->set_momentum(des_mom);

                          }
                          else {

                            ATH_MSG_INFO(" Descendant's invariant mass is significantly distorted by Lorentz boost! Leave this descendant as is."
                                         << " Masses before first and second and after second boosts = "
                                         << mass_1 << ", " << mass_2 << ", " << mass_3
                                         << " Descendant's pdg_id, barcode = "
                                         << (*des)->pdg_id() << ", "
                                         << (*des)->barcode()
                                         << ", sibling's barcode, pdg_id = "
                                         << (*p)->barcode() << ", "
                                         << (*p)->pdg_id()
                                         );

                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }

      // add photons
      for ( int ipho = index; ipho <= HepMC::HEPEVT_Wrapper::number_entries() ; ipho++) {
        HepMC::GenParticle* ppho
          = new HepMC::GenParticle( CLHEP::HepLorentzVector( HepMC::HEPEVT_Wrapper::px(index)*1000.,
                                                      HepMC::HEPEVT_Wrapper::py(index)*1000.,
                                                      HepMC::HEPEVT_Wrapper::pz(index)*1000.,
                                                      HepMC::HEPEVT_Wrapper::e(index)*1000. ),
                                    HepMC::HEPEVT_Wrapper::id(index),
                                    HepMC::HEPEVT_Wrapper::status(index) );
        new_vert->add_particle_out( ppho );
        index++;
      }
    } else {
      if (needtmp_v) {//restore particles

        new_par1->production_vertex()->add_particle_out(part);
        new_par2->production_vertex()->add_particle_out(sib_desc);
        tmp_vert->add_particle_out(new_par1);
        tmp_vert->add_particle_out(new_par2);
      }

    }

  }

  //   std::cout << "Photos_i-D->>>>>>>>>>event AFTER radiation" << std::endl;
  //   evt->print();
  //   std::cout << "Photos_i-D-<<<<<<<<<<event AFTER radiation" << std::endl;

  StatusCode status = StatusCode::SUCCESS;

  return status;

}




// rz print tree function
int PrintChildTree_photo( const HepMC::GenParticle* p, int tabN){

  int no_of_children=0;
  if(p->end_vertex()){
    no_of_children = p->end_vertex()->particles_out_size();
  }
  //  cout << "Hello anybody home? no_of_children  " << no_of_children <<endl;
  if(no_of_children > 0 /* && tabN < 6 */){

    HepMC::GenVertex * testvertex = p->end_vertex();
    HepMC::GenVertex::particle_iterator pit;
    pit = testvertex->particles_begin(HepMC::children);
    HepMC::GenVertex::particle_iterator pitE = testvertex->particles_end(HepMC::children);
    tabN += 1 ;

    int i =1;
    for(;pit != pitE;pit++){
      // cout << " " << (*pit)->pdg_id() << endl;

      const HepMC::GenParticle* child = (*pit);
      int c_pdgId = (*pit)->pdg_id();

      for (int j=0;j<tabN;j++){ std::cout << "  " ;} //pretty printing
      std::cout <<tabN-1  <<  " c# " << i << "/" << no_of_children << " " ;//just pretty printing
      std::cout <<  "PdgId: "<< c_pdgId << " barcode: " << (*pit)->barcode() ;
      std::cout << " px: " << (*pit)->momentum().px() /1000.0 << " ";
      // cout << " py: " << (*pit)->momentum().py() /1000.0 << " ";
      // cout << " pz: " << (*pit)->momentum().pz() /1000.0 << " ";
      // cout << " e : " << (*pit)->momentum().e() /1000.0 << " ";
      std::cout << std::endl;
      PrintChildTree_photo(child,tabN);
      i++;
    }
    return 1;
  } else {
    return 0;
  }
}
// rz end of function
