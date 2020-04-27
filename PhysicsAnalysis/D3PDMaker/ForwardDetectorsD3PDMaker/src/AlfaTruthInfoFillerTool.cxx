/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AlfaTruthInfoFillerTool.cxx 2012-03-21 10:33:45Z lnozka $
/**
  * @file ForwardDetectorsD3PDMaker/AlfaTruthInfoFillerTool.h
  * @author Libor Nozka
  * @date March 203
  * @brief Block filler tool for Alfa information.
  */

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"
#include "Identifier/Identifier.h"

// Local include(s):
#include "AlfaTruthInfoFillerTool.h"

namespace D3PD {

	/**
	* @brief Standard Gaudi tool constructor.
	* @param type The name of the tool type.
	* @param name The tool name.
	* @param parent The tool's Gaudi parent.
	*/
	AlfaTruthInfoFillerTool::AlfaTruthInfoFillerTool( const std::string& type, const std::string& name, const IInterface* parent)
		: AlfaTruthInfoFillerBase( type, name, parent )
	{
		ATH_MSG_INFO( "AlfaTruthInfoFillerTool---> Constructor" );

		//m_pvecVtx_g=NULL;
		m_pvecVtx_g_x=NULL;
		m_pvecVtx_g_y=NULL;
		m_pvecVtx_g_z=NULL;

		//m_pvecVtx_a=NULL;
		m_pvecVtx_a_x=NULL;
		m_pvecVtx_a_y=NULL;
		m_pvecVtx_a_z=NULL;

		//m_pvecVtx_c=NULL;
		m_pvecVtx_c_x=NULL;
		m_pvecVtx_c_y=NULL;
		m_pvecVtx_c_z=NULL;

		//m_pvecP_a=NULL;
		m_pvecPa_pt=NULL;
		m_pvecPa_phi=NULL;
		m_pvecPa_pz=NULL;
		m_pvecPa_m=NULL;

		//m_pvecP_c=NULL;
		m_pvecPc_pt=NULL;
		m_pvecPc_phi=NULL;
		m_pvecPc_pz=NULL;
		m_pvecPc_m=NULL;

		//m_pvecPBeam1_i=NULL;
		m_pvecPBeam1_i_pt=NULL;
		m_pvecPBeam1_i_phi=NULL;
		m_pvecPBeam1_i_pz=NULL;
		m_pvecPBeam1_i_E=NULL;

		//m_pvecPBeam1_f=NULL;
		m_pvecPBeam1_f_pt=NULL;
		m_pvecPBeam1_f_phi=NULL;
		m_pvecPBeam1_f_pz=NULL;
		m_pvecPBeam1_f_E=NULL;

		//m_pvecPBeam2_i=NULL;
		m_pvecPBeam2_i_pt=NULL;
		m_pvecPBeam2_i_phi=NULL;
		m_pvecPBeam2_i_pz=NULL;
		m_pvecPBeam2_i_E=NULL;

		//m_pvecPBeam2_f=NULL;
		m_pvecPBeam2_f_pt=NULL;
		m_pvecPBeam2_f_phi=NULL;
		m_pvecPBeam2_f_pz=NULL;
		m_pvecPBeam2_f_E=NULL;

		// data type using in the local reconstruction
		// for the simulation data the value is 0, for the real data the value is 1. Unset value is -1
		//declareProperty("DataType", m_nDataType=1, "data type using in the local reconstruction");
   }

	/**
	* Initialize: informs the type container to D3PDMaker
	*             retrieves the mapping database
	*             prepares the coefficients for the calibration
	*
	* @return Success or failure (will stop the processing)
	*/
	StatusCode AlfaTruthInfoFillerTool::initialize()
	{
		// Initialize the base class:
		CHECK( AlfaTruthInfoFillerBase::initialize() );

		ATH_MSG_INFO( "AlfaTruthInfoFillerTool---> Initialized" );

		return StatusCode::SUCCESS;
	}

	StatusCode AlfaTruthInfoFillerTool::book()
	{
		ATH_MSG_DEBUG( "AlfaTruthInfoFillerTool---> Booking Histograms" );

		//CHECK( addVariable ("vtx_G", m_pvecVtx_g));
		CHECK( addVariable ("vtx_G_x", m_pvecVtx_g_x));
		CHECK( addVariable ("vtx_G_y", m_pvecVtx_g_y));
		CHECK( addVariable ("vtx_G_z", m_pvecVtx_g_z));

		//CHECK( addVariable ("vtx_A", m_pvecVtx_a));
		CHECK( addVariable ("vtx_x_A", m_pvecVtx_a_x));
		CHECK( addVariable ("vtx_y_A", m_pvecVtx_a_y));
		CHECK( addVariable ("vtx_z_A", m_pvecVtx_a_z));

		//CHECK( addVariable ("vtx_C", m_pvecVtx_c));
		CHECK( addVariable ("vtx_x_C", m_pvecVtx_c_x));
		CHECK( addVariable ("vtx_y_C", m_pvecVtx_c_y));
		CHECK( addVariable ("vtx_z_C", m_pvecVtx_c_z));

		//CHECK( addVariable ("p_A", m_pvecP_a));
		CHECK( addVariable ("pt_A", m_pvecPa_pt));
		CHECK( addVariable ("phi_A", m_pvecPa_phi));
		CHECK( addVariable ("pz_A", m_pvecPa_pz));
		CHECK( addVariable ("m_A", m_pvecPa_m));

		//CHECK( addVariable ("p_C", m_pvecP_c));
		CHECK( addVariable ("pt_C", m_pvecPc_pt));
		CHECK( addVariable ("phi_C", m_pvecPc_phi));
		CHECK( addVariable ("pz_C", m_pvecPc_pz));
		CHECK( addVariable ("m_C", m_pvecPc_m));

		//CHECK( addVariable ("p_beam1_i", m_pvecPBeam1_i));
		CHECK( addVariable ("p_beam1_i_pt", m_pvecPBeam1_i_pt));
		CHECK( addVariable ("p_beam1_i_phi", m_pvecPBeam1_i_phi));
		CHECK( addVariable ("p_beam1_i_pz", m_pvecPBeam1_i_pz));
		CHECK( addVariable ("p_beam1_i_E", m_pvecPBeam1_i_E));

		//CHECK( addVariable ("p_beam1_f", m_pvecPBeam1_f));
		CHECK( addVariable ("p_beam1_f_pt", m_pvecPBeam1_f_pt));
		CHECK( addVariable ("p_beam1_f_phi", m_pvecPBeam1_f_phi));
		CHECK( addVariable ("p_beam1_f_pz", m_pvecPBeam1_f_pz));
		CHECK( addVariable ("p_beam1_f_E", m_pvecPBeam1_f_E));

		//CHECK( addVariable ("p_beam2_i", m_pvecPBeam2_i));
		CHECK( addVariable ("p_beam2_i_pt", m_pvecPBeam2_i_pt));
		CHECK( addVariable ("p_beam2_i_phi", m_pvecPBeam2_i_phi));
		CHECK( addVariable ("p_beam2_i_pz", m_pvecPBeam2_i_pz));
		CHECK( addVariable ("p_beam2_i_E", m_pvecPBeam2_i_E));

		//CHECK( addVariable ("p_beam2_f", m_pvecPBeam2_f));
		CHECK( addVariable ("p_beam2_f_pt", m_pvecPBeam2_f_pt));
		CHECK( addVariable ("p_beam2_f_phi", m_pvecPBeam2_f_phi));
		CHECK( addVariable ("p_beam2_f_pz", m_pvecPBeam2_f_pz));
		CHECK( addVariable ("p_beam2_f_E", m_pvecPBeam2_f_E));

		return StatusCode::SUCCESS;
	}

	StatusCode AlfaTruthInfoFillerTool::fill(const McEventCollection& EventCollection)
	{
		MsgStream LogStream(Athena::getMessageSvc(), "ALFA_Ntuple::TruthInfo()");
		LogStream << MSG::DEBUG << "begin ALFA_Ntuple::TruthInfo(), tom version" << endmsg;

		int coll_counter = 0;

		McEventCollection::const_iterator mcTruBeg = EventCollection.begin(); // = (*mcTru).begin()
		McEventCollection::const_iterator mcTruEnd = EventCollection.end();

		int nPileUpCnt=EventCollection.size();

		ClearData(nPileUpCnt);
		int vtx_counter[MAXPILEUP];
		memset(&vtx_counter, 0, sizeof(vtx_counter));

		LogStream << MSG::INFO << "MARK01" << endmsg;
		//loop over collection (container)
		for(;mcTruBeg!=mcTruEnd;++mcTruBeg){

			HepMC::GenParticle* pi1=0;
			HepMC::GenParticle* pi2=0;
			HepMC::GenParticle* p1=0;
			HepMC::GenParticle* p2=0;

			int pint = 0;
			int pcount = 0;

			//b_vtx_kin_fill_flag = false;

			coll_counter++;

			HepMC::GenEvent::vertex_const_iterator begGenVtxItr = (**mcTruBeg).vertices_begin();
			HepMC::GenEvent::vertex_const_iterator endGenVtxItr = (**mcTruBeg).vertices_end();


			//particles at IP and IP vertex properties

			LogStream << MSG::INFO << "MARK02" << endmsg;
			//loop over verteces belonging to one event
			for(;begGenVtxItr!=endGenVtxItr;++begGenVtxItr){

				LogStream << MSG::DEBUG << " * collection no: " << coll_counter << endmsg;
				LogStream << MSG::DEBUG << " * vertex no: " << vtx_counter[coll_counter-1] << endmsg;
				LogStream << MSG::DEBUG << " * position x = " << (**begGenVtxItr).position().x() << ", y = " << (**begGenVtxItr).position().y()<< ", z =" << (**begGenVtxItr).position().z() << endmsg;

				if (fabs((**begGenVtxItr).position().z()) > 500.){  // the IP vertex should be max +/-50 cm from z=0.
					continue;
				}

				vtx_counter[coll_counter-1]++;

				HepMC::GenVertex::particle_iterator child;
				child = (*begGenVtxItr)->particles_begin(HepMC::family);
				HepMC::GenVertex::particle_iterator child_end;
				child_end = (*begGenVtxItr)->particles_end(HepMC::family);

				for(; child != child_end; ++child){

                                        //double px = (*child)->momentum().px();
                                        //double py = (*child)->momentum().py();
					double pz = (*child)->momentum().pz();


					//if ((*child == genEvt->beam_particles().first) || (*child == genEvt->beam_particles().second))
					//{
					if (fabs((**begGenVtxItr).position().z()) <= 500.){

						// incoming protons (status code = 4) at the interaction point; for p-p collisions should be 2 per collection (assumption), for p-Pb should be 1;
						if( (*child)->status() == 4){ // note, it can be any colliding particle, hopefully Pb too
							pint++;
							// assuming no bugs in generation - vertex should be just 1 ;-)

							if (pint == 1){
								(*m_pvecVtx_g_x)[coll_counter-1] = (**begGenVtxItr).position().x();
								(*m_pvecVtx_g_y)[coll_counter-1] = (**begGenVtxItr).position().y();
								(*m_pvecVtx_g_z)[coll_counter-1] = (**begGenVtxItr).position().z();
							}


							if(pz > 0){
								pi1=(*child);
								(*m_pvecPBeam1_i_pt)[coll_counter-1] = sqrt(pi1->momentum().px()*pi1->momentum().px()+pi1->momentum().py()*pi1->momentum().py());
								(*m_pvecPBeam1_i_phi)[coll_counter-1] = pi1->momentum().phi();
								(*m_pvecPBeam1_i_pz)[coll_counter-1] = pi1->momentum().pz();
								(*m_pvecPBeam1_i_E)[coll_counter-1] = pi1->momentum().e();
								//LogStream << MSG::DEBUG << "initial particle 1: px = " <<(*m_pvecPBeam1_i)[1] << ", py = " << (*m_pvecPBeam1_i)[2] << ", pz = " << (*m_pvecPBeam1_i)[3] << ", E = " << (*m_pvecPBeam1_i)[0] << endmsg;
								//LogStream << MSG::DEBUG << " ** " << endmsg;
							}

							if(pz < 0){
								pi2=(*child);
								(*m_pvecPBeam2_i_pt)[coll_counter-1] = sqrt(pi2->momentum().px()*pi2->momentum().px()+pi2->momentum().py()*pi2->momentum().py());
								(*m_pvecPBeam2_i_phi)[coll_counter-1] = pi2->momentum().phi();
								(*m_pvecPBeam2_i_pz)[coll_counter-1] = pi2->momentum().pz();
								(*m_pvecPBeam2_i_E)[coll_counter-1] = pi2->momentum().e();
								//LogStream << MSG::DEBUG << "initial particle 2: px = " << (*m_pvecPBeam2_i)[1] << ", py = " << (*m_pvecPBeam2_i)[2] << ", pz = " << (*m_pvecPBeam2_i)[3] << ", E = " << (*m_pvecPBeam2_i)[0] << endmsg;
								//LogStream << MSG::DEBUG << " ** " << endmsg;
							}

							if(pint > 2) {LogStream << MSG::DEBUG << "Strange: More than two incoming protons in this event! (depends what is simulated)" << endmsg;}
							LogStream << MSG::DEBUG << "pint = " << pint << endmsg;

							// we know we have interaction vertex (==4), outgoing protons at the interaction point; 2 will be just in case of elastic or exlusive processes(and if no additional protons are produced)

							HepMC::GenVertex::particle_iterator childik;
							childik = (*begGenVtxItr)->particles_begin(HepMC::children);
							HepMC::GenVertex::particle_iterator childik_end;
							childik_end = (*begGenVtxItr)->particles_end(HepMC::children);

							for(; childik != childik_end; ++childik){

								if( (*childik)->pdg_id() == 2212){ // can be elastic, diffractive, exclusive, p-Pb; do not know how to filter on elastic Pb

									pcount++;

									if(pz > 0){

										p1=(*childik);
										(*m_pvecPBeam1_f_pt)[coll_counter-1] = sqrt(p1->momentum().px()*p1->momentum().px()+p1->momentum().py()*p1->momentum().py());
										(*m_pvecPBeam1_f_phi)[coll_counter-1] = p1->momentum().phi();
										(*m_pvecPBeam1_f_pz)[coll_counter-1] = p1->momentum().pz();
										(*m_pvecPBeam1_f_E)[coll_counter-1] = p1->momentum().e();
									}

									if(pz < 0){

										p2=(*childik);
										(*m_pvecPBeam2_f_pt)[coll_counter-1] = sqrt(p2->momentum().px()*p2->momentum().px()+p2->momentum().py()*p2->momentum().py());
										(*m_pvecPBeam2_f_phi)[coll_counter-1] = p2->momentum().phi();
										(*m_pvecPBeam2_f_pz)[coll_counter-1] = p2->momentum().pz();
										(*m_pvecPBeam2_f_E)[coll_counter-1] = p2->momentum().e();
									}
								}
							}

							if(pcount > 2) {LogStream << MSG::DEBUG << "More than two outcoming protons in this event; not possible for elastic scaterring!" << endmsg;}
							LogStream << MSG::DEBUG << "pcount = " << pcount << endmsg;
						}
					}
				}
			}

			LogStream << MSG::INFO << "MARK03" << endmsg;
			//transported/simulated particles and vertices
			begGenVtxItr = (**mcTruBeg).vertices_begin();

			vtx_counter[coll_counter-1] = 0.;
			int nAux=0;

			for(;begGenVtxItr!=endGenVtxItr;++begGenVtxItr)
			{
				nAux++;
				if (fabs((fabs((**begGenVtxItr).position().z())-236908.)) > 50.) // transported/simulated to 236.908m
				{
					continue;
				}

				vtx_counter[coll_counter-1]++;

				HepMC::GenVertex::particle_iterator child, child_end;
				child = (*begGenVtxItr)->particles_begin(HepMC::family);
				child_end = (*begGenVtxItr)->particles_end(HepMC::family);

				double px = (*child)->momentum().px();
				double py = (*child)->momentum().py();
				double pz = (*child)->momentum().pz();
				double E  = (*child)->momentum().e();
				double phi= (*child)->momentum().phi();
				double m  = (*child)->momentum().m();

				LogStream << MSG::DEBUG << "particle barcode = " << (*child)->barcode() << endmsg;
				LogStream << MSG::DEBUG << "particle pdg = " << (*child)->pdg_id() << endmsg;
				LogStream << MSG::DEBUG << "particle status = " << (*child)->status() << endmsg;
				LogStream << MSG::DEBUG << " *  px = " << px << ", py = " << py << ", pz =" << pz << endmsg;
				LogStream << MSG::DEBUG << " " << endmsg;

				for(; child != child_end; ++child) {

					// here we are interested in the proton(s) coming from IP (elastic, difractive), minimally interacting till coming in front of ALFA
					// - in case of forward transport - the vertex was newly created at the location specified in JO of the ForwardTransportSvc
					// - in case of full simulation, inside of one collection is maximally one proton (per side) with energy close to the collision one

					if (fabs(fabs((**begGenVtxItr).position().z())-236908.) <= 50.){

						if( ((*child)->status() == 1)){

							if( (*child)->pdg_id() == 2212){

								if(pz > 0){

									//float faux=(*m_pvecPBeam1_f)[4*(coll_counter-1)+0];
									if (fabs((*m_pvecPBeam1_f_E)[coll_counter-1] - E) < 500.0){ // deviation of the energy of the outcoming particle should be, after transport/simulation, less then 1 MeV}
										(*m_pvecVtx_a_x)[coll_counter-1] = (**begGenVtxItr).position().x();
										(*m_pvecVtx_a_y)[coll_counter-1] = (**begGenVtxItr).position().y();
										(*m_pvecVtx_a_z)[coll_counter-1] = (**begGenVtxItr).position().z();

										(*m_pvecPa_pt)[coll_counter-1] = sqrt(px*px+py*py);
										(*m_pvecPa_phi)[coll_counter-1]  = phi;
										(*m_pvecPa_pz)[coll_counter-1]  = pz;
										(*m_pvecPa_m)[coll_counter-1]  = m;
									}
								}
								else if(pz < 0){

									if (fabs((*m_pvecPBeam2_f_E)[coll_counter-1] - E) < 500.0){ // deviation of the energy of the outcoming particle should be, after transport/simulation, less then 1 MeV}
										(*m_pvecVtx_c_x)[coll_counter-1] = (**begGenVtxItr).position().x();
										(*m_pvecVtx_c_y)[coll_counter-1] = (**begGenVtxItr).position().y();
										(*m_pvecVtx_c_z)[coll_counter-1] = (**begGenVtxItr).position().z();

										(*m_pvecPc_pt)[coll_counter-1] = sqrt(px*px+py*py);
										(*m_pvecPc_phi)[coll_counter-1]  = phi;
										(*m_pvecPc_pz)[coll_counter-1]  = pz;
										(*m_pvecPc_m)[coll_counter-1]  = m;
									}
								}
							}
						}
					}
				}
			}

			LogStream << MSG::INFO << "MARK04" << endmsg;
			if ((pi1 != 0) && (pi2 != 0) && (p1 != 0) && (p2 != 0) && (pint == 2) && (pcount == 2)){ //this check works only for elastic

				//b_vtx_kin_fill_flag = true;

				/*
					HepMC::FourVector pv1 = (pi1->momentum());
					HepMC::FourVector pv2 = (pi2->momentum());
					HepMC::FourVector pv3 = (p1->momentum());
					HepMC::FourVector pv4 = (p2->momentum());

					CLHEP::HepLorentzVector hp1(pv1.px(),pv1.py(),pv1.pz(),pv1.e());
					CLHEP::HepLorentzVector hp2(pv2.px(),pv2.py(),pv2.pz(),pv2.e());
					CLHEP::HepLorentzVector hp3(pv3.px(),pv3.py(),pv3.pz(),pv3.e());
					CLHEP::HepLorentzVector hp4(pv4.px(),pv4.py(),pv4.pz(),pv4.e());

					t_13 = (hp1-hp3).m2();
					t_24 = (hp2-hp4).m2();
					LogStream << MSG::DEBUG << " ******************************************************* " << endmsg;
					LogStream << MSG::DEBUG << " " << endmsg;
					LogStream << MSG::DEBUG << " t_13 = " << t_13 << endmsg;
					LogStream << MSG::DEBUG << " t_24 = " << t_24 << endmsg;
					LogStream << MSG::DEBUG << " " << endmsg;
					LogStream << MSG::DEBUG << " ******************************************************* " << endmsg;
					*/

			}
		}

		LogStream << MSG::INFO << "MARK05" << endmsg;
		return StatusCode::SUCCESS;
	}

	void AlfaTruthInfoFillerTool::ClearData(const int nPileUpCnt)
	{
		//m_pvecVtx_g->resize(nPileUpCnt*4);
		//fill_n(m_pvecVtx_g->begin(),m_pvecVtx_g->size(),0);
		m_pvecVtx_g_x->resize(nPileUpCnt);
		fill_n(m_pvecVtx_g_x->begin(),m_pvecVtx_g_x->size(),0);
		m_pvecVtx_g_y->resize(nPileUpCnt);
		fill_n(m_pvecVtx_g_y->begin(),m_pvecVtx_g_y->size(),0);
		m_pvecVtx_g_z->resize(nPileUpCnt);
		fill_n(m_pvecVtx_g_z->begin(),m_pvecVtx_g_z->size(),0);

		//m_pvecVtx_a->resize(nPileUpCnt*4);
		//fill_n(m_pvecVtx_a->begin(),m_pvecVtx_a->size(),0);
		m_pvecVtx_a_x->resize(nPileUpCnt);
		fill_n(m_pvecVtx_a_x->begin(),m_pvecVtx_a_x->size(),0);
		m_pvecVtx_a_y->resize(nPileUpCnt);
		fill_n(m_pvecVtx_a_y->begin(),m_pvecVtx_a_y->size(),0);
		m_pvecVtx_a_z->resize(nPileUpCnt);
		fill_n(m_pvecVtx_a_z->begin(),m_pvecVtx_a_z->size(),0);

		//m_pvecVtx_c->resize(nPileUpCnt*4);
		//fill_n(m_pvecVtx_c->begin(),m_pvecVtx_c->size(),0);
		m_pvecVtx_c_x->resize(nPileUpCnt);
		fill_n(m_pvecVtx_c_x->begin(),m_pvecVtx_c_x->size(),0);
		m_pvecVtx_c_y->resize(nPileUpCnt);
		fill_n(m_pvecVtx_c_y->begin(),m_pvecVtx_c_y->size(),0);
		m_pvecVtx_c_z->resize(nPileUpCnt);
		fill_n(m_pvecVtx_c_z->begin(),m_pvecVtx_c_z->size(),0);

		//m_pvecP_a->resize(nPileUpCnt*4);
		//fill_n(m_pvecP_a->begin(),m_pvecP_a->size(),0);
		m_pvecPa_pt->resize(nPileUpCnt);
		fill_n(m_pvecPa_pt->begin(),m_pvecPa_pt->size(),0);
		m_pvecPa_phi->resize(nPileUpCnt);
		fill_n(m_pvecPa_phi->begin(),m_pvecPa_phi->size(),0);
		m_pvecPa_pz->resize(nPileUpCnt);
		fill_n(m_pvecPa_pz->begin(),m_pvecPa_pz->size(),0);
		m_pvecPa_m->resize(nPileUpCnt);
		fill_n(m_pvecPa_m->begin(),m_pvecPa_m->size(),0);

		//m_pvecP_c->resize(nPileUpCnt*4);
		//fill_n(m_pvecP_c->begin(),m_pvecP_c->size(),0);
		m_pvecPc_pt->resize(nPileUpCnt);
		fill_n(m_pvecPc_pt->begin(),m_pvecPc_pt->size(),0);
		m_pvecPc_phi->resize(nPileUpCnt);
		fill_n(m_pvecPc_phi->begin(),m_pvecPc_phi->size(),0);
		m_pvecPc_pz->resize(nPileUpCnt);
		fill_n(m_pvecPc_pz->begin(),m_pvecPc_pz->size(),0);
		m_pvecPc_m->resize(nPileUpCnt);
		fill_n(m_pvecPc_m->begin(),m_pvecPc_m->size(),0);

		//m_pvecPBeam1_i->resize(nPileUpCnt*4);
		//fill_n(m_pvecPBeam1_i->begin(),m_pvecPBeam1_i->size(),0);
		m_pvecPBeam1_i_pt->resize(nPileUpCnt);
		fill_n(m_pvecPBeam1_i_pt->begin(),m_pvecPBeam1_i_pt->size(),0);
		m_pvecPBeam1_i_phi->resize(nPileUpCnt);
		fill_n(m_pvecPBeam1_i_phi->begin(),m_pvecPBeam1_i_phi->size(),0);
		m_pvecPBeam1_i_pz->resize(nPileUpCnt);
		fill_n(m_pvecPBeam1_i_pz->begin(),m_pvecPBeam1_i_pz->size(),0);
		m_pvecPBeam1_i_E->resize(nPileUpCnt);
		fill_n(m_pvecPBeam1_i_E->begin(),m_pvecPBeam1_i_E->size(),0);

		//m_pvecPBeam1_f->resize(nPileUpCnt*4);
		//fill_n(m_pvecPBeam1_f->begin(),m_pvecPBeam1_f->size(),0);
		m_pvecPBeam1_f_pt->resize(nPileUpCnt);
		fill_n(m_pvecPBeam1_f_pt->begin(),m_pvecPBeam1_f_pt->size(),0);
		m_pvecPBeam1_f_phi->resize(nPileUpCnt);
		fill_n(m_pvecPBeam1_f_phi->begin(),m_pvecPBeam1_f_phi->size(),0);
		m_pvecPBeam1_f_pz->resize(nPileUpCnt);
		fill_n(m_pvecPBeam1_f_pz->begin(),m_pvecPBeam1_f_pz->size(),0);
		m_pvecPBeam1_f_E->resize(nPileUpCnt);
		fill_n(m_pvecPBeam1_f_E->begin(),m_pvecPBeam1_f_E->size(),0);

		//m_pvecPBeam2_i->resize(nPileUpCnt*4);
		//fill_n(m_pvecPBeam2_i->begin(),m_pvecPBeam2_i->size(),0);
		m_pvecPBeam2_i_pt->resize(nPileUpCnt);
		fill_n(m_pvecPBeam2_i_pt->begin(),m_pvecPBeam2_i_pt->size(),0);
		m_pvecPBeam2_i_phi->resize(nPileUpCnt);
		fill_n(m_pvecPBeam2_i_phi->begin(),m_pvecPBeam2_i_phi->size(),0);
		m_pvecPBeam2_i_pz->resize(nPileUpCnt);
		fill_n(m_pvecPBeam2_i_pz->begin(),m_pvecPBeam2_i_pz->size(),0);
		m_pvecPBeam2_i_E->resize(nPileUpCnt);
		fill_n(m_pvecPBeam2_i_E->begin(),m_pvecPBeam2_i_E->size(),0);

		//m_pvecPBeam2_f->resize(nPileUpCnt*4);
		//fill_n(m_pvecPBeam2_f->begin(),m_pvecPBeam2_f->size(),0);
		m_pvecPBeam2_f_pt->resize(nPileUpCnt);
		fill_n(m_pvecPBeam2_f_pt->begin(),m_pvecPBeam2_f_pt->size(),0);
		m_pvecPBeam2_f_phi->resize(nPileUpCnt);
		fill_n(m_pvecPBeam2_f_phi->begin(),m_pvecPBeam2_f_phi->size(),0);
		m_pvecPBeam2_f_pz->resize(nPileUpCnt);
		fill_n(m_pvecPBeam2_f_pz->begin(),m_pvecPBeam2_f_pz->size(),0);
		m_pvecPBeam2_f_E->resize(nPileUpCnt);
		fill_n(m_pvecPBeam2_f_E->begin(),m_pvecPBeam2_f_E->size(),0);
	}


} // namespace D3PD

