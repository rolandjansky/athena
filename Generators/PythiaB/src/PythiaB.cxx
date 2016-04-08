/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ------------------------------------------------------------- File:
// GeneratorModules/PythiaB.cxx 
// Description: Allows the user
// to generate pythia events and store the result in the
// Transient Store.
// adapted for use by the B group.
// This should not be used in the same job as PythiaGenerator
//
// AuthorList:U
//         Ian Hinchliffe:  Initial Code June: 2000
//         Modeled after the CDF code by Stan Thompson
// parsing added August 2000
// more switches added Feb 2001
// B version by Maria Smizanska (March 2002)
// Modified and Incorporated into Generator Modules by IH (March 2002)
// Update on repeated hadronization by Malte Muller and M.Smizanska (August 2002)
// New: PythiaB::storeBEvent PythiaB::fillEvt by Malte Muller(August 2002)
// Special treatment for bbbb, bbcc events if forced channels, M.Smizanska (August 2002)
// Changes due to re-organization of Generators by G. Stavropoulos and I.Hinchliffe (August 2003)
//  W.Walkowiak, 2006-04-02 added m_maxTriesHard, m_forceDecayChannel
//                          and m_decayChannelParameters 
//                          (for user_finsel.F reorganization) 
// 2006-04-09, W. Walkowiak                                             
//    added uf_BsJpsimumuphiKK() -- code by James Catmore               
// 2006-04-17, W. Walkowiak                           
//    added uf_bbmumu() -- code by Sergey Sivoklokov  
// 2006-09-02, S.Sivoklokov                           
//    added uf_kstarmumu() -- code by Sergey Sivoklokov  
//    added uf_phimumu() -- code by Sergey Sivoklokov  
//    added uf_gammamumu() -- code by Sergey Sivoklokov  
// 2008-05-09, L. Gladilin
//    added uf_ChHadr() -- code by Leonid Gladilin
// 2010-08-26, P. Reznicek
//    added uf_BdJpsimumuK0starKpi() -- code by Pavel Reznicek
//

// Header for this module:-

#include "Pythia_i/Pythia.h"
#include "PythiaB/PythiaB.h"

// Framework Related Headers:-
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"

#include "GeneratorObjects/McEventCollection.h"
#include "StoreGate/StoreGateSvc.h"

#include "HepMC/IO_HEPEVT.h"
//#include "HepMC/IO_Ascii.h"
#include "HepMC/HEPEVT_Wrapper.h"

#include "TruthUtils/GeneratorName.h"

// Other classes used by this class:-
#include "GeneratorUtils/StringParse.h"
#include <stdlib.h>
//-------------------------------
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// calls to fortran routines
extern "C" {
  void initpyblock_(int*, const char*);
  void pyinit_(const char*,
               const char*, 
               const char*,
               double* ,
               int, // lengths of character strings
               int,// (should be value, not reference) 
               int  
               );
//  void pyevnt_();
    void pyevnw_();
    void pyexec_();
  void pystat_(int*);
  void pylist_(int*);
  void pyupda_(int*, int*);
  void pydocu_();
//  void lunhep_(int*); // STDHEP routine for PYJETS->HEPEVT
void pyhepc_(int*);
  int opdcay_(const char*, int*, char*, int, int);
  void cldcay_(int*);
  void parsel_(int*,int*,int*);
  void multi_flav_luexec_();
  void finsel_(int*);
//  void comons_( double*,const char*, double*,double*,double*, double*, double*, double*);
  void comons_( double*,double*, double*,double*,double*, double*, double*, double*,int*,double*);
  void bcopy_(int*);
  void xsect_(int*);
  void wri_ludat3_(int*); void copy_pydat3_();
}
//-----------------------------------
// File scope declarations:-

//--------------------------------------------------------------------------
PythiaB::PythiaB(const std::string& name, 
      ISvcLocator* pSvcLocator)
  : Pythia(name,pSvcLocator),
    m_cutbqp(0.), m_cutbqe(0.), m_cutabqp(0.), m_cutabqe(0.),
    m_xhad(0.)
{
//--------------------------------------------------------------------------  
  declareProperty("cutbq", m_cutbqVector);
  declareProperty("lvl1cut", m_lvl1cut );
  declareProperty("lvl2cut", m_lvl2cut);
  declareProperty("offcut", m_offcut);
  declareProperty("mhadr",m_mhad = 1);
  declareProperty("flavour", m_fla = 5);
  declareProperty("SignalDumptoAscii", m_Ntup = 50);
  declareProperty("ForceBDecay", m_forceb = "no");
  declareProperty("ForceCDecay", m_forcec = "no");
  
  declareProperty("maxTriesHard", m_maxTriesHard = 50000);
  declareProperty("ForceDecayChannel", m_forceDecayChannel = "none");
  declareProperty("DecayChannelParameters", m_decayChannelParameters );
}


//--------------------------------------------------------------------------
 PythiaB::~PythiaB(){
//--------------------------------------------------------------------------
}

//---------------------------------------------------------------------------
StatusCode PythiaB::genuserInitialize() {
  //---------------------------------------------------------------------------
  // Initialise the listing output, parameter and decay data input streams
  //
  ATH_MSG_INFO( " PYTHIAB INITIALISING.  \n" );

  // Define default input parameter values (stored in vectors ), 
  // can be overwritten by users datacards
  m_cutbqVector.push_back("0. 100. or 0. 100.");
  m_lvl1cut.push_back(0.); m_lvl1cut.push_back(0.); m_lvl1cut.push_back(100.);
  m_lvl2cut.push_back(0.);  m_lvl2cut.push_back(13.);m_lvl2cut.push_back(0.);m_lvl2cut.push_back(100.);
  m_offcut.push_back(0.);
  m_offcut.push_back(0.);m_offcut.push_back(100.);
  m_offcut.push_back(0.);m_offcut.push_back(100.);
  m_offcut.push_back(0.);m_offcut.push_back(100.);

  // Preset values for decay channel parameters to be used in 
  // user_finsel.F extensions  (10 parameters total)
  for (int i=0; i<10; i++) {
    m_decayChannelParameters.push_back(0.);
  }

// store decay tables default and users defined in memory to allow later manipulations
//      int ii=1;
//   wri_ludat3_(&ii);  // writes out file with defaul pydat3 - use only ones with unbiased b.txt 
      copy_pydat3_();  
  
  
  // set up the input parameters additional to Pythia
  double c0[4], c1[3], c2[4], c3[7];
      unsigned int i = 0;
      ATH_MSG_DEBUG( " Command is: " << m_cutbqVector[i] );
    StringParse mystring(m_cutbqVector[i]);
      ATH_MSG_DEBUG( " I am after StringParse mystring i=" );
      ATH_MSG_DEBUG( " mystring.numpiece(1)=" << mystring.numpiece(1) );
      ATH_MSG_DEBUG( " mystring.numpiece(4)=" << mystring.numpiece(4) );
      ATH_MSG_DEBUG( " mystring.piece(5)=" << mystring.piece(5) );
    c0[0]=mystring.numpiece(1);
    c0[1]=mystring.numpiece(2);
    c0[2]=mystring.numpiece(4);
    c0[3]=mystring.numpiece(5);
    
    m_cutbqp=c0[0];
    m_cutbqe=c0[1];
    m_cutabqp=c0[2];
    m_cutabqe=c0[3];
    m_cutbqandor=mystring.piece(3) ;  
    ATH_MSG_DEBUG( " m_cutbqandor=" << m_cutbqandor );

//    double c0[4], c1[3], c2[4], c3[7];
    for ( int ii=0; ii<= 6; ii++ )  
      { 
      
        if(ii<=2)  c1[ii]=m_lvl1cut[ii];
        if(ii<=3)  c2[ii]=m_lvl2cut[ii];
        if(ii<=6)  c3[ii]=m_offcut[ii];
       }
       
      
       double cutbqao= 1. ;
       
       if(m_cutbqandor=="and" ) cutbqao = 1. ;
       if(m_cutbqandor=="AND" ) cutbqao = 1. ;
       if(m_cutbqandor=="or" ) cutbqao = -1. ;
       if(m_cutbqandor=="OR" ) cutbqao = -1. ;
      
//    user_finsel.F decay channel number and parameters
//    New channels need to have a number assigned here!
       int idc = -1;
       if ( m_forceDecayChannel == "none" ) {
	 idc = -1;
       } else if ( m_forceDecayChannel == "Skeleton" ) {
	 idc = 0;
       } else if ( m_forceDecayChannel == "DsPhiX" ) {
	 idc = 1;
       } else if ( m_forceDecayChannel == "LambdabmumuLambda" ) {
	 idc = 2;
       } else if ( m_forceDecayChannel == "LambdabJpsimumuLambda" ) {
	 idc = 3;
       } else if ( m_forceDecayChannel == "BsJpsimumuphiKK" ) {
	 idc = 4;
       } else if ( m_forceDecayChannel == "bbmumu" ) {
	 idc = 5;
       } else if ( m_forceDecayChannel == "BdKstarMuMu" ) {
	 idc = 6;
       } else if ( m_forceDecayChannel == "BsPhiMuMu" ) {
	 idc = 7;
       } else if ( m_forceDecayChannel == "BsGammaMuMu" ) {
	 idc = 8;
       } else if ( m_forceDecayChannel == "CharmHadrons" ) {
	 idc = 9;
       } else if ( m_forceDecayChannel == "BsAngles" ) {
	 idc = 10;
       } else if ( m_forceDecayChannel == "BdJpsimumuK0starKpi" ) {
	 idc = 11;
       } else if ( m_forceDecayChannel == "BsAnglesCDF" ) {
	 idc = 12;
       } else if ( m_forceDecayChannel == "BdAnglesCDF" ) {
	 idc = 13;
       } else {
	 ATH_MSG_WARNING( "Decay channel " << m_forceDecayChannel << " is not defined!" );
       }

       // parameters for user defined decay channels
       double cdcp[10];
       for ( int ii=0; ii<10; ii++ ) {
	 cdcp[ii] = m_decayChannelParameters[ii];
       }
       
       ATH_MSG_DEBUG( "m_cutbqandor =" << m_cutbqandor << m_cutbqandor );      
       ATH_MSG_DEBUG( "  m_forceb= " << m_forceb << m_forceb );
       ATH_MSG_DEBUG( " idc = " << idc << " for channel " << m_forceDecayChannel );
      
       comons_(c0,&cutbqao,c1,c2,c3,&m_mhad,&m_fla,&m_Ntup,&idc,
	       cdcp);

       m_xhad=0.;
     
//  check decay table  after initialization   
     int ilist;
     ilist=12;
     pylist_(&ilist);
	 
     m_BEventBuffer.erase( m_BEventBuffer.begin(), m_BEventBuffer.end() );

     Pythia::pythia_stream = "PYTHIA";
	 
     // Should follow what is in Pythia_i genInitialize
     HepMC::HEPEVT_Wrapper::set_sizeof_int(sizeof(int));
     HepMC::HEPEVT_Wrapper::set_sizeof_real(8);
     HepMC::HEPEVT_Wrapper::set_max_number_entries(10000);

     return StatusCode::SUCCESS;  
 } 


//---------------------------------------------------------------------------
StatusCode PythiaB::callGenerator() {
//---------------------------------------------------------------------------
	ATH_MSG_DEBUG( " PYTHIA generating." );
	
	if( !m_BEventBuffer.empty() )   {
		ATH_MSG_DEBUG( "PythiaB skipped - still events in buffer" );
		return StatusCode::SUCCESS;   // there are still buffered events.
	};
  
	// Save the random number seeds in the event
	CLHEP::HepRandomEngine*	engine	=	Pythia::p_AtRndmGenSvc->GetEngine(Pythia::pythia_stream);
	const long*		s	=	engine->getSeeds();
	m_seeds.clear();
	m_seeds.push_back(s[0]);
	m_seeds.push_back(s[1]);

//////////////////////////////// 
// Generate partonic part of event
////////////////////////////////

	int rejectSecondary = 2;
	bool eventsAccepted  = false;
	int iiloop=0;
	
	while (!eventsAccepted)   {
		
		++iiloop;
		if(iiloop>=m_maxTriesHard)   { 
			ATH_MSG_ERROR( " PYTHIA generating inefficiently:  Check your cuts. \n  " );
			ATH_MSG_ERROR( " PYTHIA generating inefficiently:  The cross section at the output may be wrong. \n" );
			break;
		};
		
		// Stop hadronization
		int ip1=111;
		int ip2=0;
		this->pypars().mstp(ip1)=ip2;
     
		int rejectPrimary=2; // reject event as dafault
		
//		pyevnt_(); // create event
     pyevnw_(); // create event
/////////////////////////////////////////////////////////  
//  First Event selection after hard process  
/////////////////////////////////////////////////////////
                int ibq=0; int icq=0; // count number of b and c pair-of-quarks in event
		bool hardok  = false;
		parsel_(&rejectPrimary, &ibq, &icq);
		double mhad_inter = m_mhad;
		if (m_forceb=="yes" && ibq>1 ) mhad_inter = 2 * m_mhad;
		// cout << "PythiaB: after 1st cut rejectPrimary = "<< rejectPrimary <<endl;

		if (rejectPrimary==0)   {  // passed first cut
/////////////////////////////////////////////////////////  
//  Haronization mhad times
/////////////////////////////////////////////////////////

			int icopy=1;
			bcopy_(&icopy);    // Remember partonic part of pyjets
		
			// Allow hadronization
			ip2=1;
			this->pypars().mstp(ip1)=ip2;
			
			
			for ( int ihad=1; ihad<=mhad_inter ; ihad++ )   {   // Repeat hadronization 
				
				icopy=2;
				bcopy_(&icopy);     // Put back parton part removing all hadronization from pyjets
				
                         //  Execute event now
			 //  Normal regime: for  bb, cc, cccc    or   if no forced channel   
                         //  If forced channel:  special multiflav treatment for bbbb, bbcc(m_forcec=yes) , bbbbcc, bbbbcccc
                         
	      
	                       if(m_forceb=="no"  || (ibq==1 && icq==0) || (ibq==0 && icq>=1) ) pyexec_(); 
	                       if(m_forceb=="yes" &&  ibq==1 && m_forcec =="no"   && icq>=1 )     pyexec_();
	                       if(m_forceb=="yes" &&  ibq==1 && m_forcec =="yes"  && icq>=1 )     multi_flav_luexec_();
	                       if(m_forceb=="yes" &&  ibq>1 )    multi_flav_luexec_();
  
///////////////////////////////////////////////////////////////
//  Event selections after hadronization  
//  Statistics, cross sections, listings, coversion to HEPEVT
///////////////////////////////////////////////////////////////  
				
				finsel_(&rejectSecondary);

				//cout << "PythiaB: outer loop index = " << iiloop
				//		<< " ,  repeat hadro ihad = " << ihad
				//		<< " out of "<< m_mhad
				//		<< " ,  status rejectSecondary = " << rejectSecondary << endl;

				if(rejectSecondary==0)   {
					
					hardok = true;
					eventsAccepted = true;
					++m_events;
					
					ATH_MSG_INFO( "PythiaB: event number m_events accepted : "<< m_events );  

					// cout << "PythiaB: before conversion to HEPEVT" <<endl;
					int mconv=1;     // convert from PYJETS to HEPEVT,
					//lunhep_(&mconv); // using STDHEP translation routine 1999/01/27 CG
					pyhepc_(&mconv);
					HepMC::HEPEVT_Wrapper::set_event_number(m_events);
					StatusCode sc = this->storeBEvent();
					if ( sc.isFailure() ) return sc; // should newer happen (see storeBEvent())
					
				};
			}; // end of repeat hadronization loop
			
			    // calculate number of accepted hard processes for which at least one event was accepted after hadronization
			    if(hardok) m_xhad=m_xhad+1. ;
		}; 	   // end of if(rejectPrimary==0) after First cut
	};		   // end of while(!eventsAccepted) 

	ATH_MSG_DEBUG( " PYTHIA generating done" );

  return StatusCode::SUCCESS;  
  
} // end call Generator


//---------------------------------------------------------------------------
StatusCode PythiaB::storeBEvent()   {
//---------------------------------------------------------------------------

//	HepMC::HEPEVT_Wrapper::print_hepevt();
	
        HepMC::GenEvent* evt = new HepMC::GenEvent(1,1);
	
	HepMC::IO_HEPEVT hepio;
	hepio.fill_next_event(evt);
	evt->set_random_states(m_seeds);
	
	/*
	cout << endl
		 << "***********************************************************" << endl
		 << "PythiaB::storeBEvent => Print evt after conversion   " << endl
		 << "***********************************************************" << endl
		 << endl;		 
	evt->print();
	*/

	m_BEventBuffer.push_back(evt);

	return StatusCode::SUCCESS;
}


long PythiaB::pCounter(HepMC::GenEvent* myEvt) const
{
	HepMC::GenEvent::particle_iterator pitr;
	long n = 0; 
	for( pitr=myEvt->particles_begin(); pitr!=myEvt->particles_end(); ++pitr )   { n++; };
	return n;
}


//---------------------------------------------------------------------------
StatusCode PythiaB::fillEvt(HepMC::GenEvent* evt) {
//---------------------------------------------------------------------------
	
	if( m_BEventBuffer.empty() )   {
		ATH_MSG_ERROR( "PythiaB: No events in buffer" );
		return StatusCode::FAILURE;
	};
	
	// Not sure if this is really needed and whether it works correctly,
	// but following the Pythia_i code. Should have only effect if using
	// PythiaB in conjunction with Tauola_i
	store_Atlas_HEPEVT();

	/*
	cout << endl
		 << "****************************************************" << endl
		 << "PythiaB::fillEvt => Print event from Buffer" << endl
		 << "****************************************************" << endl
		 << endl;		 
	m_BEventBuffer.back()->pGenEvt()->print();
	*/
	
// 	*evt = *(m_BEventBuffer.back()->pGenEvt());
// 	McEvent* mcevt = m_BEventBuffer.back();
// 	evt = mcevt->pGenEvt();
//	evt = m_BEventBuffer.back();
	*evt = *(m_BEventBuffer.back());	

	// Should follow Pythia_i fillEvt as closely as possible
	int pr_id = PYTHIA + m_ExternalProcess + this->pyint1().mint(1);
	if (m_ExternalProcess > 0) pr_id = PYTHIA + m_ExternalProcess;
	evt->set_signal_process_id(pr_id);
	evt->set_random_states(m_seeds);
	evt->weights().push_back(this->pypars().pari(7));
	evt->weights().push_back(this->pypars().pari(9));
	evt->weights().push_back(this->pypars().pari(10));
	//PDF WEIGHTS - using HepMC::PdfInfo object.. 
	//pdf_id x_1, x_1, Q2, f_1, f_2 
	int id1=this->pypars().msti(15);
	int id2=this->pypars().msti(16);
	double x1=this->pypars().pari(33);
	double x2=this->pypars().pari(34);
	double q=this->pypars().pari(23);
	double pdf1=(double)this->pypars().mstp(51); //pdg id - awkward but..
	double pdf2=0.; //dummy 
	HepMC::PdfInfo tmp_pdi(id1,id2,x1,x2,q,pdf1,pdf2);
	evt->set_pdf_info(tmp_pdi);

  // Convert cm->mm and GeV->MeV
//   cmTomm(evt);
  GeVToMeV(evt);
  



	/*
	cout << endl
		 << "****************************************************" << endl
		 << "PythiaB::fillEvt => Print evt after copy" << endl
		 << "****************************************************" << endl
		 << endl;		 
	evt->print();
	*/
	
	m_BEventBuffer.pop_back();
	
	ATH_MSG_DEBUG( "PythiaB using event from buffer (" << m_BEventBuffer.size() << " left) \n" );
	
	return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode PythiaB::genFinalize() {
  //---------------------------------------------------------------------------
  ATH_MSG_INFO( " PYTHIABModule  Ending.  \n" );
  int mstat = 1;
  
  ATH_MSG_INFO( "Call PYSTAT at endRun" );
  ATH_MSG_INFO( "PythiaB::genFinalize  m_events accepted "<< m_events );
  pystat_(&mstat);
  
  float bx[1]; float cx[1];
  bx[0]=this->pyint5().xsec(0,3)*m_events/this->pyint5().ngen(0,2)/m_mhad;
  cx[0]=(1.*m_events)/m_xhad;

  ATH_MSG_ALWAYS( " I===================================================================================== " );
  ATH_MSG_ALWAYS( " I   CROSSSECTION OF YOUR B-CHANNEL IS                   I    " );
  ATH_MSG_ALWAYS( " I                       BX= PX*NB/AC/MHAD=              I    " << bx[0] << " mbarn" );
  ATH_MSG_ALWAYS( " I                                                       I    " );
  ATH_MSG_ALWAYS( " I   IN CASE YOU FORCED ANY DECAY YOU SHOULD             I    " );
  ATH_MSG_ALWAYS( " I   CORRECT CROSS SECTION BX FURTHER, MULTIPLYING       I    " );
  ATH_MSG_ALWAYS( " I   BX BY BRANCHING RATIO(S) OF YOUR FORCED             I    " );
  ATH_MSG_ALWAYS( " I   DECAY(S) AND BY A FACTOR OF 2 FOR SYMMETRY          I    " );
  ATH_MSG_ALWAYS( " I                                                       I    " );  
  ATH_MSG_ALWAYS( " I   MORE DETAILS ON CROSS SECTION                       I    " ); 
  ATH_MSG_ALWAYS( " I   PYTHIA MSEL=1     CROSS SECTION IS    PX=           I    " <<  this->pyint5().xsec(0,3)<<" mbarn" );
  ATH_MSG_ALWAYS( " I   NUMBER OF   ACCEPTED  MSEL=1 EVENTS   AC=           I    " <<  this->pyint5().ngen(0,2) );
  ATH_MSG_ALWAYS( " I   NUMBER OF   ACCEPTED    B-EVENTS   IS NB=           I    " << m_events );
  ATH_MSG_ALWAYS( " I   REPEATED HADRONIZATIONS IN EACH EVENT MHAD=         I    " << m_mhad );
  ATH_MSG_ALWAYS( " I   AVERAGE NUM OF ACCEPTED EVTS IN HADRONIZATION LOOP  I    " << cx[0] );
  ATH_MSG_ALWAYS( " I   IN CASE YOU FORCED ANY DECAY YOU SHOULD             I    " );
  ATH_MSG_ALWAYS( " I   CORRECT CROSS SECTION BX FURTHER, MULTIPLYING       I    " );
  ATH_MSG_ALWAYS( " I   BX BY BRANCHING RATIO(S) OF YOUR FORCED             I    " );
  ATH_MSG_ALWAYS( " I   DECAY(S) AND BY A FACTOR OF 2 FOR SYMMETRY          I    " );
  ATH_MSG_ALWAYS( " I                                                       I    " );
  ATH_MSG_ALWAYS( " I===================================================================================== " );
  
  ATH_MSG_ALWAYS( "                                                                                        " );
  ATH_MSG_ALWAYS( " I===================================================================================== " );
  ATH_MSG_ALWAYS( " I          YOUR  MAIN SIMULATION PARAMETERS AND CUTS                                   " );
  ATH_MSG_ALWAYS( " I===================================================================================== " );
  ATH_MSG_ALWAYS( " I   HARD SCATTERING  CUT  pysubs().ckin(3)             PT      I      " << this->pysubs().ckin(3) );
  ATH_MSG_ALWAYS( " I   STRUCTURE FCN (1=CTEQ3 7=CTEQ5) pypars().mstp(51)          I      " << this->pypars().mstp(51) );
  ATH_MSG_ALWAYS( " I   CUTS ON b and/or anti b QUARK                              I      " << m_cutbqp << " ; " << m_cutbqe << " ; " << m_cutbqandor <<" ; " <<m_cutabqp << " ; " << m_cutabqe );
  ATH_MSG_ALWAYS( " I   LVL1 MUON CUTS:                               PT AND ETA   I      " << m_lvl1cut[1] << " ;  "  << m_lvl1cut[2] );
  ATH_MSG_ALWAYS( " I   LVL2 CUTS:         ON(1)/OFF(0); PARTICLE-ID; PT AND ETA   I      " << m_lvl2cut[0] << " ;  "  << m_lvl2cut[1] << " ;  " << m_lvl2cut[2] << " ;  "  << m_lvl2cut[3] );
  ATH_MSG_ALWAYS( " I   CUTS FOR STABLE PARTICLES IN B-DECAY:         ON(1)/OFF(0) I      " << m_offcut[0] );
  ATH_MSG_ALWAYS( " I    CHARGED HADRONS:                             PT AND ETA   I      " <<  m_offcut[1]<< " ;  "  << m_offcut[2] );
  ATH_MSG_ALWAYS( " I    MUONS:                                       PT AND ETA   I      " <<  m_offcut[3]<< " ;  "  << m_offcut[4] );
  ATH_MSG_ALWAYS( " I    ELECTRONS:                                   PT AND ETA   I      " <<  m_offcut[5]<< " ;  "  << m_offcut[6] );
  ATH_MSG_ALWAYS( " I===================================================================================== " );

  std::cout << "MetaData: cross-section (nb)= " << bx[0]*1000000. << std::endl;

  return StatusCode::SUCCESS;
}
