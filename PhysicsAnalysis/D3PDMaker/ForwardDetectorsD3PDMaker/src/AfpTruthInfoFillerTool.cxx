/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// Local include(s):
#include "AfpTruthInfoFillerTool.h"
//#include "AthenaPoolUtilities/AthenaAttributeList.h"

namespace D3PD {

	AfpTruthInfoFillerTool::AfpTruthInfoFillerTool( const std::string& type, const std::string& name, const IInterface* parent)
		: AfpTruthInfoFillerBase( type, name, parent )
	{
		ATH_MSG_INFO( "AfpTruthInfoFillerTool---> Constructor" );
		
		//m_pnPrimaryIPs=NULL;
		//m_pnProtons_f=NULL;
		
		m_pvecVtx_g_x=NULL;
		m_pvecVtx_g_y=NULL;
		m_pvecVtx_g_z=NULL;
		m_pvecVtx_g_t=NULL;	

		m_pvecPBeam1_i_pt=NULL;
		m_pvecPBeam1_i_phi=NULL;
		m_pvecPBeam1_i_pz=NULL;
		m_pvecPBeam1_i_E=NULL;

		m_pvecPBeam2_i_pt=NULL;
		m_pvecPBeam2_i_phi=NULL;
		m_pvecPBeam2_i_pz=NULL;
		m_pvecPBeam2_i_E=NULL;

		m_pvecParticle_f_pt=NULL;
		m_pvecParticle_f_phi=NULL;
		m_pvecParticle_f_pz=NULL;
		m_pvecParticle_f_m=NULL;
		m_pvecParticle_f_pdg=NULL;
		m_pvecParticle_f_barcode=NULL;

   }

	StatusCode AfpTruthInfoFillerTool::initialize()
	{
		// Initialize the base class:
		CHECK( AfpTruthInfoFillerBase::initialize() );
		
		/*
		   // get detector store
           if (StatusCode::SUCCESS!=service("DetectorStore", p_detstore)) {
                  ATH_MSG_FATAL( "Detector store not found" ); 
                  return StatusCode::FAILURE;
           }

            ATH_MSG_INFO( "AfpTruthInfoFillerTool---> starting initialization" );
            
		const AthenaAttributeList* atrlist=0;
		
		ATH_MSG_INFO( "AfpTruthInfoFillerTool---> first step" ); 
		
		if (StatusCode::SUCCESS==p_detstore->retrieve(atrlist,table)) 
		{
			int beam_energy=(*atrlist)["beam_energy"].data<int>();
		}	
		
		ATH_MSG_DEBUG( " beam energy = " << beam_energy );

            */
		
		ATH_MSG_INFO( "AfpTruthInfoFillerTool---> Initialized" );

		return StatusCode::SUCCESS;
	}

	StatusCode AfpTruthInfoFillerTool::book()
	{
		ATH_MSG_DEBUG( "AfpTruthInfoFillerTool---> Booking Histograms" );

		//CHECK( addVariable ("nPrimaryIPs", m_pnPrimaryIPs));
		
		CHECK( addVariable ("vtx_G_x", m_pvecVtx_g_x));
		CHECK( addVariable ("vtx_G_y", m_pvecVtx_g_y));
		CHECK( addVariable ("vtx_G_z", m_pvecVtx_g_z));
		CHECK( addVariable ("vtx_G_t", m_pvecVtx_g_t));

		CHECK( addVariable ("p_beam1_i_pt", m_pvecPBeam1_i_pt));
		CHECK( addVariable ("p_beam1_i_phi", m_pvecPBeam1_i_phi));
		CHECK( addVariable ("p_beam1_i_pz", m_pvecPBeam1_i_pz));
		CHECK( addVariable ("p_beam1_i_E", m_pvecPBeam1_i_E));

		CHECK( addVariable ("p_beam2_i_pt", m_pvecPBeam2_i_pt));
		CHECK( addVariable ("p_beam2_i_phi", m_pvecPBeam2_i_phi));
		CHECK( addVariable ("p_beam2_i_pz", m_pvecPBeam2_i_pz));
		CHECK( addVariable ("p_beam2_i_E", m_pvecPBeam2_i_E));

		CHECK( addVariable ("particle_f_pt", m_pvecParticle_f_pt));
		CHECK( addVariable ("particle_f_phi", m_pvecParticle_f_phi));
		CHECK( addVariable ("particle_f_pz", m_pvecParticle_f_pz));
		CHECK( addVariable ("particle_f_m", m_pvecParticle_f_m));
		CHECK( addVariable ("particle_f_pdg", m_pvecParticle_f_pdg));
		CHECK( addVariable ("particle_f_barcode", m_pvecParticle_f_barcode));		

		return StatusCode::SUCCESS;
	}

	StatusCode AfpTruthInfoFillerTool::fill(const McEventCollection& EventCollection)
	{
		MsgStream LogStream(Athena::getMessageSvc(), "AfpTruthInfoFillerTool::fill()");
		LogStream << MSG::DEBUG << "begin " << endreq;

		//float px, py, pz, E, phi;
		float pz;
		//int barcode;
		int coll_counter = 0;

		McEventCollection::const_iterator mcTruBeg = EventCollection.begin(); // = (*mcTru).begin()
		McEventCollection::const_iterator mcTruEnd = EventCollection.end();

		ClearData();
		
		int vtx_counter[MAXPILEUP];
		memset(&vtx_counter, 0, sizeof(vtx_counter));

		//loop over collection (container)
		for(;mcTruBeg!=mcTruEnd;++mcTruBeg){

			HepMC::GenParticle* pi1=0;
			HepMC::GenParticle* pi2=0;
			//HepMC::GenParticle* p1=0;
			HepMC::GenParticle* p2=0;

			int pint = 0;
			int pcount = 0;
			
			LogStream << MSG::DEBUG << " just before " << endreq;
			//HepMC::GenParticle* beam_p1 = (**mcTruBeg).beam_particle_1;
			HepMC::GenParticle* beam_p1 = (**mcTruBeg).beam_particles().first;
			//HepMC::GenParticle* beam_p2 = (**mcTruBeg).beam_particle_2;
			HepMC::GenParticle* beam_p2 = (**mcTruBeg).beam_particles().second;
			
			LogStream << MSG::DEBUG << " beam1 pointer " << beam_p1 << endreq;
			LogStream << MSG::DEBUG << " beam2 pointer " << beam_p2 << endreq;
			LogStream << MSG::DEBUG << " just after " << endreq;
			
			//double beam1_energy = beam_p1->momentum().e();
			//double beam2_energy = beam_p2->momentum().e();
			
			double beam1_energy = 0.; //MeV
			double beam2_energy = 0.; //MeV
			
			LogStream << MSG::DEBUG << " beam1_energy = " << beam1_energy << ", beam2_energy = " << beam2_energy << endreq;
			

			//b_vtx_kin_fill_flag = false;
			coll_counter++;

			HepMC::GenEvent::vertex_const_iterator begGenVtxItr = (**mcTruBeg).vertices_begin();
			HepMC::GenEvent::vertex_const_iterator endGenVtxItr = (**mcTruBeg).vertices_end();


			//particles at IP and IP vertex properties

			//loop over verteces belonging to one event in the given collection
			for(;begGenVtxItr!=endGenVtxItr;++begGenVtxItr){

				//LogStream << MSG::DEBUG << " * collection no: " << coll_counter << endreq;
				//LogStream << MSG::DEBUG << " * vertex no: " << vtx_counter[coll_counter-1] << endreq;
				//LogStream << MSG::DEBUG << " * position x = " << (**begGenVtxItr).position().x() << ", y = " << (**begGenVtxItr).position().y()<< ", z =" << (**begGenVtxItr).position().z() << endreq;

				// tom sykora: following 3 lines (cut on z-vertex position) remained here from the historical/old code with no cut on *primary* vertices...
				//if (fabs((**begGenVtxItr).position().z()) > 500.){  // the IP vertex should be max +/-50 cm from z=0.
				//	continue;
				//}

				vtx_counter[coll_counter-1]++;

				HepMC::GenVertex::particle_iterator child;
				child = (*begGenVtxItr)->particles_begin(HepMC::family);
				HepMC::GenVertex::particle_iterator child_end;
				child_end = (*begGenVtxItr)->particles_end(HepMC::family);

				for(; child != child_end; ++child){

					//px =      (*child)->momentum().px();
					//py =      (*child)->momentum().py();
					pz =      (*child)->momentum().pz();
					//E  =      (*child)->momentum().e();
					//phi=      (*child)->momentum().phi();
					//barcode = (*child)->barcode();

					//LogStream << MSG::DEBUG << "particle barcode = " << (*child)->barcode() << endreq;
					//LogStream << MSG::DEBUG << "particle pdg = " << (*child)->pdg_id() << endreq;
					//LogStream << MSG::DEBUG << "particle status = " << (*child)->status() << endreq;
					//LogStream << MSG::DEBUG << " *  px = " << px << ", py = " << py << ", pz =" << pz << endreq;
					//LogStream << MSG::DEBUG << " " << endreq;

					// incoming protons (status code = 4) at the interaction point; 
					if( (*child)->status() == 4){ // note, it can be any colliding particle, hopefully Pb too
						pint++;
						// assuming no bugs in generation - vertex should be just 1 per collection ;-)

						if (pint == 1){
							m_pvecVtx_g_x->push_back((**begGenVtxItr).position().x());
							m_pvecVtx_g_y->push_back((**begGenVtxItr).position().y());
							m_pvecVtx_g_z->push_back((**begGenVtxItr).position().z());
							m_pvecVtx_g_t->push_back((**begGenVtxItr).position().t());
						}

						if(pz > 0){
							pi1=(*child);
							m_pvecPBeam1_i_pt->push_back(sqrt(pi1->momentum().px()*pi1->momentum().px()+pi1->momentum().py()*pi1->momentum().py()));
							m_pvecPBeam1_i_phi->push_back(pi1->momentum().phi());
							m_pvecPBeam1_i_pz->push_back(pi1->momentum().pz());
							m_pvecPBeam1_i_E->push_back(pi1->momentum().e());
							beam1_energy = pi1->momentum().e();
							//LogStream << MSG::DEBUG << "initial particle 1: px = " <<(*m_pvecPBeam1_i)[1] << ", py = " << (*m_pvecPBeam1_i)[2] << ", pz = " << (*m_pvecPBeam1_i)[3] << ", E = " << (*m_pvecPBeam1_i)[0] << endreq;
							//LogStream << MSG::DEBUG << " ** " << endreq;
						}

						if(pz < 0){
							pi2=(*child);
							m_pvecPBeam2_i_pt->push_back(sqrt(pi2->momentum().px()*pi2->momentum().px()+pi2->momentum().py()*pi2->momentum().py()));
							m_pvecPBeam2_i_phi->push_back(pi2->momentum().phi());
							m_pvecPBeam2_i_pz->push_back(pi2->momentum().pz());
							m_pvecPBeam2_i_E->push_back(pi2->momentum().e());
							beam2_energy = pi2->momentum().e();
							//LogStream << MSG::DEBUG << "initial particle 2: px = " << (*m_pvecPBeam2_i)[1] << ", py = " << (*m_pvecPBeam2_i)[2] << ", pz = " << (*m_pvecPBeam2_i)[3] << ", E = " << (*m_pvecPBeam2_i)[0] << endreq;
							//LogStream << MSG::DEBUG << " ** " << endreq;
						}

						if(pint > 2) {LogStream << MSG::DEBUG << "Strange: More than two incoming protons in this event! (depends what is simulated, in particle generator or gun, ...)" << endreq;}
						LogStream << MSG::DEBUG << "pint = " << pint << endreq;

						// we know we have interaction vertex (==4), outgoing protons at the interaction point; 2 will be just in case of elastic or exlusive processes(and if no additional protons are produced)

						
						// now we will run over primary IP vertex, looking for all leading protons or proton like leading particles
						HepMC::GenVertex::particle_iterator childik;
						childik = (*begGenVtxItr)->particles_begin(HepMC::children);
						HepMC::GenVertex::particle_iterator childik_end;
						childik_end = (*begGenVtxItr)->particles_end(HepMC::children);

						for(; childik != childik_end; ++childik){

							
							p2=(*childik);
							double pt = sqrt(p2->momentum().px()*p2->momentum().px()+p2->momentum().py()*p2->momentum().py());
							double pz = p2->momentum().pz();
							double eta = -log(abs(pt/(2.*pz))); //approximation !
							int pdg = p2->pdg_id();
							double xi = 1. - sqrt(pt*pt+pz*pz)/beam1_energy;
							
							LogStream << MSG::DEBUG << " *************************** " << endreq;
							LogStream << MSG::DEBUG << " forward particle eta = " << eta << endreq;
							LogStream << MSG::DEBUG << " forward particle xi = " << xi << endreq;
							LogStream << MSG::DEBUG << " forward particle pdg = " << pdg << endreq;
							
							
							//if( (*childik)->pdg_id() == 2212){ // can be elastic, diffractive, exclusive, p-Pb; do not know how to filter on elastic Pb
							if ((pdg == 2212) || ((xi > 0.01) && (xi < 0.25 ) && (abs(eta)) > 8.))
							{
							// du to this conditions only protons will be registered -> has to be changed

								pcount++;

								//p2=(*childik);
								m_pvecParticle_f_pt->push_back(sqrt(p2->momentum().px()*p2->momentum().px()+p2->momentum().py()*p2->momentum().py()));
								m_pvecParticle_f_phi->push_back(p2->momentum().phi());
								m_pvecParticle_f_pz->push_back(p2->momentum().pz());
								//m_pvecParticle_f_m->push_back(p2->generated_mass());
								m_pvecParticle_f_m->push_back(p2->momentum().m());
								m_pvecParticle_f_pdg->push_back(p2->pdg_id());
								m_pvecParticle_f_barcode->push_back(p2->barcode());
							}
						}

						LogStream << MSG::DEBUG << "Number of outgoing protons (proton like) in this event " << endreq;}
						LogStream << MSG::DEBUG << "pcount = " << pcount << endreq;
					}
					
				}
			}

		return StatusCode::SUCCESS;
	}

	void AfpTruthInfoFillerTool::ClearData()
	{

		//*m_pnPrimaryIPs = -1;
		//*m_pnProtons_f  = -1;
		
		m_pvecVtx_g_x->clear();
		m_pvecVtx_g_y->clear();
		m_pvecVtx_g_z->clear();
		m_pvecVtx_g_t->clear();

		m_pvecPBeam1_i_pt->clear();
		m_pvecPBeam1_i_phi->clear();
		m_pvecPBeam1_i_pz->clear();
		m_pvecPBeam1_i_E->clear();

		m_pvecPBeam2_i_pt->clear();
		m_pvecPBeam2_i_phi->clear();
		m_pvecPBeam2_i_pz->clear();
		m_pvecPBeam2_i_E->clear();

		m_pvecParticle_f_pt->clear();
		m_pvecParticle_f_phi->clear();
		m_pvecParticle_f_pz->clear();
		m_pvecParticle_f_m->clear();
		m_pvecParticle_f_pdg->clear();
		m_pvecParticle_f_barcode->clear();

	}


} // namespace D3PD

