/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
   
NAME:     iPaNtuple.cxx
PACKAGE:  offline/Reconstruction/iPatRec/iPatRecAlgs

AUTHORS:  Alan Poppleton - Nektarios Benekos
CREATED:  January 31, 2003
 
PURPOSE:  write out iPatRec ntuple
iPatNtuple Algorithm - produce some track histograms

In the INITIALIZE METHOD:
  -> Create an Ntuple Directory
  -> Create an Ntuple in this Directory
  -> Add some items to this Ntuple
  -> Save the pointer to the Ntuple as a private data member
 
In the EXECUTE METHOD:
  -> Get pointer to the Event from the ZebraTDREvent
  -> Calculate the ntuple variables
  -> Fill the Ntuple

ATLAS Collaboration
********************************************************************/
//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include "xAODEventInfo/EventInfo.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GeneratorObjects/McEventCollection.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "HepMC/GenEvent.h"
#include "TruthTools/ITruthSelector.h"
#include "iPatInterfaces/ITruthParameters.h"
#include "iPatRecAlgs/iPatNtuple.h"
#include "iPatRecEvent/iPatTrackContainer.h"
#include "iPatTrack/HitOnTrack.h"
#include "iPatTrack/HitStatistics.h"

//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

//<<<<<< PRIVATE TYPES                                                  >>>>>>

typedef		std::list<HitOnTrack*>		hit_list;
typedef		hit_list::const_iterator	hit_citerator;

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

//------------------------------------------------------------------------
 
 
/*
  --------------------------THE CONSTRUCTOR-------------------------------
*/ 

iPatNtuple::iPatNtuple(const std::string& name, 
		       ISvcLocator* pSvcLocator) 
    :	AthAlgorithm	       	(name, pSvcLocator),
	m_eventCount	        (0), 
	m_g3			(true),
	m_maxKine		(50),
	m_maxReco		(150),
	m_tracksName		(""),
	m_truthParameters	("TruthParameters/TruthParameters"),
	m_truthSelector		("TruthSelector/TruthSelector")

	// Read the following properties from jobOptions File :
	// (If not found in jobOptions, the defaults are as given above)

{
    declareProperty("TracksName",	m_tracksName);
    declareProperty("TruthParameters",	m_truthParameters);
    declareProperty("TruthSelector",	m_truthSelector);
    declareProperty("g3",         	m_g3);
}

/*
  --------------------------THE DESTRUCTOR-------------------------------
*/ 
iPatNtuple::~iPatNtuple(void)
{}
 
/*
  -----------------------THE INITIALIZE METHOD-----------------------------
*/

StatusCode 
iPatNtuple::initialize()
{
    // print where you are
    ATH_MSG_INFO( "iPatNtuple::initialize() - package version " << PACKAGE_VERSION );

    // get the Tools
    if (m_truthParameters.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_truthParameters );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_truthParameters );
    }
    if (m_truthSelector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_truthSelector );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_truthSelector );
    }

    // check configuration setting:
    //  check tracks have a name
    if (m_tracksName == "")
    {
	ATH_MSG_FATAL( "No name set for iPat Tracks" );
	return StatusCode::FAILURE;
    }

    // Create a column-wise ntuple
    // Get the pointer to the Ntuple Directory 
    // If a null pointer - the directory does not exist -> so create it.
    // If a non-Null pointer, it already exists -> don't create it.
    SmartDataPtr<NTuple::Directory> DiriPat(ntupleService(), "/NTUPLES/FILE1/IPATREC");
    if (! DiriPat )
    { 
	DiriPat = ntupleService()->createDirectory("/NTUPLES/FILE1/IPATREC");
    } 

    // Check the directory again if it is valid (maybe the creation failed?)
    if (! DiriPat )
    {
	ATH_MSG_FATAL(  " Invalid Ntuple Director : IPATREC" );
	return StatusCode::FAILURE;
    }

    // Book the Ntuple AND CHECK IT:
    m_ntuple = ntupleService()->book(DiriPat.ptr(),
				     10, 
				     CLID_ColumnWiseTuple,
				     "iPatRec Ntuple");

    if (! m_ntuple)
    {
	ATH_MSG_FATAL(  "Failed to book ntuple : iPatNtuple" );
	return StatusCode::FAILURE;
    }       

    ATH_MSG_DEBUG("declare variables for iPatRec ntuple" );
    m_iPatEvent=0;
    initNTuple();
 
    // Done with ntuple booking.
    ATH_MSG_DEBUG("iPatRec Ntuple initialisation completed" );
    return StatusCode::SUCCESS;
}
 
/*
  ------------------------The EXECUTE METHOD-------------------------------
*/

StatusCode 
iPatNtuple::execute()
{
    ATH_MSG_VERBOSE( "entered execution" );
 
    // Get hold of Tracks from iPatTrackContainer
    ++m_eventCount;
    StatusCode sc =  evtStore()->retrieve(m_tracks, m_tracksName);
    if (sc.isFailure())
    {
	ATH_MSG_WARNING( "iPatNtuple::Could not retrieve TrackContainer" );
	return StatusCode::SUCCESS;
    }

    // Filling the Ntuple

    //   Store a pointer to the event
    // **********************************
    const xAOD::EventInfo* eventInfo;
    //StatusCode sc =  evtStore()->retrieve(eventInfo);
    if ( evtStore()->retrieve(eventInfo).isFailure())
    {
	ATH_MSG_WARNING( "Could not retrieve event info" );
	return StatusCode::SUCCESS;
    }
  
    m_RunNum =eventInfo->runNumber();
    m_EventNum = eventInfo->eventNumber();
    ++m_iPatEvent;
    m_iPatEventNum = m_iPatEvent;

    // Initialization of ntuple variables
    m_zMax = 0.;
    m_boundary_number		=0;
    m_pending_number		=0;
    m_planar_holes		=0;
	
    m_b_layer_clusters_shared	=0;
    m_b_layer_clusters_unique	=0;
    m_b_layer_clusters_spoilt	=0;
    m_b_layer_clusters_wrong	=0; 
    m_b_layer_clusters_total	=0; 
         
    m_outer_pixel_clusters_shared  =0;
    m_outer_pixel_clusters_unique  =0;
    m_outer_pixel_clusters_spoilt  =0;
    m_outer_pixel_clusters_wrong   =0;
    m_outer_pixel_clusters_total   =0;

    m_pixel_clusters_shared   =0;
    m_pixel_clusters_unique   =0;
    m_pixel_clusters_spoilt   =0;
    m_pixel_clusters_wrong    =0;
    m_pixel_clusters_total    =0;
        
    m_strip_clusters_shared   =0;
    m_strip_clusters_unique   =0;
    m_strip_clusters_spoilt   =0;
    m_strip_clusters_wrong    =0;
    m_strip_clusters_total    =0;  
 
    m_drift_unique            =0;
    m_drift_spoilt            =0;
    m_drift_wrong             =0;
    m_drift_shared            =0;
    m_drift_rejects           =0;
    m_drift_wires	            =0;
    m_drift_hits	            =0;        
    m_drift_time_hits	    =0;        
    m_tr_hits	            =0;
    m_total_shared_hits       =0;   
    m_total_spoilt_hits       =0;   
    m_total_unique_hits       =0;   
    m_total_wrong_hits        =0;   
    m_nkine	            =0;
    m_nreco	            =0;


    //  const Track* track_container;
    if(m_tracks->size() >0 )
    {
	m_nreco	=	std::min(m_maxReco, (int) m_tracks->size());
	int k=0;
  
	for (std::vector<Track*>::const_iterator t = m_tracks->begin();
	     t != m_tracks->end();
	     ++t, ++k)
	{
	    hit_const_iterator              hit             = (**t).hit_list_begin();    

	    // Hits
	    if ((**t).hasTruthAssociation())
	    {
		//        const TruthAssociation& truthAssociation= (**t).truthAssociation();
		for (hit = (**t).hit_list_begin(); hit!= (**t).hit_list_end(); ++hit)
		{
		    if ((**hit).isPositionMeasurement())
		    {
			// maximum z for cosmic reach into endcap
			if((**hit).isPositionMeasurement() 
			   && std::abs((**hit).position().z()) > m_zMax) 
			    m_zMax = std::abs((**hit).position().z());
 
			if ((**hit).status() == pixel_cluster || (**hit).status() == broad_pixel)
			{
			    if ((**hit).position().perp() < 60.)
			    { 
				if((**hit).isShared()) ++m_b_layer_clusters_shared;
				if ((**t).truthAssociation().barcode() ==0)
				{ 
				    ++m_b_layer_clusters_spoilt;
				}
				else if ((**t).truthAssociation().barcode() ==0) 
				{                   
				    ++m_b_layer_clusters_unique; 
				}                            
				else
				{
				    ++m_b_layer_clusters_wrong;
				}    
				m_b_layer_clusters_total = m_b_layer_clusters_unique + 
							   m_b_layer_clusters_spoilt + 
							   m_b_layer_clusters_wrong;                  
			    } // hit in b layer
			    else
			    {
				if((**hit).isShared()) ++m_outer_pixel_clusters_shared;
				if ((**t).truthAssociation().barcode() ==0)
				{ 
				    ++m_outer_pixel_clusters_spoilt;
				}
				else if ((**t).truthAssociation().barcode() ==0) 
				{                   
				    ++m_outer_pixel_clusters_unique; 
				}                            
				else
				{
				    ++m_outer_pixel_clusters_wrong;
				} 
				m_outer_pixel_clusters_total = m_outer_pixel_clusters_unique + 
							       m_outer_pixel_clusters_spoilt +
							       m_outer_pixel_clusters_wrong;                                               
			    } // hit in outer layer

			    m_pixel_clusters_shared = m_b_layer_clusters_shared + m_outer_pixel_clusters_shared;
			    m_pixel_clusters_spoilt = m_b_layer_clusters_spoilt + m_outer_pixel_clusters_spoilt;
			    m_pixel_clusters_unique = m_b_layer_clusters_unique + m_outer_pixel_clusters_unique;
			    m_pixel_clusters_wrong =  m_b_layer_clusters_wrong  + m_outer_pixel_clusters_wrong;
			    m_pixel_clusters_total =  m_b_layer_clusters_total  + m_outer_pixel_clusters_total;    
			}                                      
			else if ((**hit).status() == strip_cluster || (**hit).status() == broad_strip)
			{
			    if((**hit).isShared()) ++m_strip_clusters_shared;
			    if ((**t).truthAssociation().barcode() ==0)
			    { 
				++m_strip_clusters_spoilt;
			    }
			    else if ((**t).truthAssociation().barcode() ==0) 
			    {                   
				++m_strip_clusters_unique; 
			    }                            
			    else
			    {
				++m_strip_clusters_wrong;
			    }                                                           
			    m_strip_clusters_total = m_strip_clusters_unique + m_strip_clusters_spoilt + m_strip_clusters_wrong;
			}// hit status sct 		  
			else if ((**hit).status() == drift_time || (**hit).status() == drift_wire)
			{
			    if((**hit).isShared()) ++m_drift_shared;
			}                    
			else if ((**hit).status() == drift_time)
			{
			    if ((**t).truthAssociation().barcode() ==0)
			    { 
				++m_drift_spoilt;
			    }
			    else if ((**t).truthAssociation().barcode() ==0) 
			    {                   
				++m_drift_unique; 
			    }                            
			    else
			    {
				++m_drift_wrong;
			    }                                              
                        
			}
			else if ((**hit).status() == drift_wire)
			{
			    if ((**t).truthAssociation().barcode() ==0)
			    { 
				++m_drift_spoilt;
				++m_drift_wires;
			    }
			    else if ((**t).truthAssociation().barcode() ==0) 
			    {                   
				++m_drift_unique;
				++m_drift_wires;
			    }                            
			    else
			    {
				++m_drift_wrong;
				++m_drift_wires;
			    }                                              
                        
			}
			// number of transition radiation hits          
			if((**hit).aboveTrThreshold()) ++m_tr_hits;  
                    
			// number of hits associated in the Straw Tracker
			m_drift_hits = m_drift_unique + m_drift_spoilt + m_drift_wrong;
                    
			// number of straw hits with drift time inside a narrow road around the track
			m_drift_time_hits = m_drift_unique + m_drift_spoilt + m_drift_wrong - m_drift_wires; 
		  
			m_total_shared_hits     = m_b_layer_clusters_shared*100000 +
						  m_strip_clusters_shared*1000 +
						  m_pixel_clusters_shared*100 +
						  m_drift_shared;
                                                  
			m_total_spoilt_hits     = m_b_layer_clusters_spoilt*100000 +
						  m_strip_clusters_spoilt*1000 +
						  m_pixel_clusters_spoilt*100 +
						  m_drift_spoilt;
 
			m_total_unique_hits     = m_b_layer_clusters_unique*100000 +
						  m_strip_clusters_unique*1000 +
						  m_pixel_clusters_unique*100 +
						  m_drift_unique;
		                                
			m_total_wrong_hits      = m_b_layer_clusters_wrong*100000 +
						  m_strip_clusters_wrong*1000 +
						  m_pixel_clusters_wrong*100 +
						  m_drift_wrong;        
		                                          
                    
		    }// position measurement
                    
		    else // hit has not position measurement
		    {
			if ((**hit).status() == pending)
			{                    
			    ++m_pending_number;
			}
			else if((**hit).status() == hole || (**hit).status() == pixel_reject || (**hit).status() == strip_reject)
			{
			    ++m_planar_holes;                    
			}
			else if((**hit).status() == drift_reject || (**hit).status() == rejected)
			{
			    // number of 'holes' on track (hits expected from the precision layers but missing)
			    ++m_drift_rejects;                  
			}
			else if((**hit).status() == boundary)
			{
			    ++m_boundary_number;                    
			}                                        
                    
		    }// is not measurement                    

		} // for hit
        
		if (k <= m_maxReco )
		{
		    // get some track parameters
		    // note - float rather than double for ntuple filling

		    if ((**t).hasFitQuality())
		    {
			m_fitprob[k]	= (**t).fitQuality().fit_probability();
			m_fitchi2[k]	= (**t).fitQuality().chi_squared();           
		    }      
		    m_A0[k]		= (**t).perigee_parameters().transverseImpact();
		    m_Z0[k]		= (**t).perigee_parameters().position().z();
		    m_phi[k]		= (**t).perigee_parameters().phi();
		    m_eta[k]		= (**t).perigee_parameters().eta();
		    m_cottheta[k]		= (**t).perigee_parameters().cotTheta();
		    m_pt[k]		= (**t).perigee_parameters().abs_pt();
		    m_ipt[k]          	= (**t).perigee_parameters().qOverPt();

		    // get corresponding truth parameters - if available
		    bool haveTruth		= false;
		    if ((**t).hasTruthAssociation())
		    {
			int			barcode		= (**t).truthAssociation().barcode();
			const Amg::Vector3D&	vertex		= (**t).perigee_parameters().vertex();
			const PerigeeParameters*	truthPerigee	= m_truthParameters->perigeeParameters(barcode,vertex);
		 
			if (truthPerigee != 0) 
			{

			    haveTruth		= true;
			    m_barcode         = barcode;
              
			    // protect against real data, fakes ...
			    const HepMC::GenParticle* particle=0;
  
			    // retrieve McEventCollection (G3 or G4)
			    const McEventCollection*    mcCollection;
			    // retrieve the MC truth (careful: key changed after 8.0.x production G4Truth-->TruthEvent)
			    std::string key = "TruthEvent";
			    if ((m_g3 &&  evtStore()->retrieve(mcCollection) != StatusCode::SUCCESS)
				|| (!m_g3 &&  evtStore()->retrieve(mcCollection,"G4Truth") != StatusCode::SUCCESS))
			    {
				ATH_MSG_WARNING( "Could not retrieve McEventCollection" );
			    }
                        
			    if (mcCollection->size() > 0 )
			    {
				m_nkine	=	std::min(m_maxKine, (int) mcCollection->size());
				int n=0;
				for(McEventCollection::const_iterator itr = mcCollection->begin(); 
				    itr!= mcCollection->end(); ++itr, ++n) 
				{
				    if (n <= m_maxKine )
				    {
					const HepMC::GenEvent* event = (*itr);
					particle = event->barcode_to_particle(barcode);

					if (particle == 0)
					{
					    ATH_MSG_DEBUG( "particle not found:  barcode " << barcode );
					}

					m_pdgpart[n]	= particle->pdg_id();
					m_statuspart[n]	= particle->status();                                 
              
					m_A0true[n]	= truthPerigee->transverseImpact();
					m_Z0true[n]	= truthPerigee->position().z();
					m_phitrue[n]	= truthPerigee->phi();
					m_etatrue[n]	= truthPerigee->eta();
					m_absetatrue[n]	= std::abs(m_etatrue[n]);
					m_cthetatrue[n]  	= truthPerigee->cotTheta();
					m_sthetatrue[n]  	= truthPerigee->sinTheta();
					m_pttrue[n]	= truthPerigee->abs_pt();
					m_ipttrue[n]	= truthPerigee->qOverPt();
 
					// differences ...
					m_diffA0	= m_A0[k] - m_A0true[n];          
					m_diffZR	= (m_Z0[k]-m_Z0true[n])*m_sthetatrue[n];
					m_diffeta	= m_eta[k] - m_etatrue[n];
					m_diffphi	= m_phi[k] - m_phitrue[n];
					if (m_diffphi < -M_PI) 
					{
					    m_diffphi += 2.*M_PI;
					}
					else if (m_diffphi >  M_PI) 
					{
					    m_diffphi -= 2.*M_PI;
					}
					m_diffcot       = m_cottheta[k]-m_cthetatrue[n];
					m_diffipt       = m_ipt[k]-m_ipttrue[n];
                
					// ratios.....
					m_ptptt	      = m_pt[k]/m_pttrue[n];
					m_iptptt	      = m_ipt[k]/m_ipttrue[n];

					// ... and pulls
					const AmgSymMatrix(5)& covariance = (**t).perigee_parameters().covariance();
					m_A0_pull	        = m_diffA0/(std::sqrt(covariance(0,0)));
					m_Z0_pull	        = m_diffZR/(std::sqrt(covariance(1,1)));
					m_phi_pull	= m_diffphi/(std::sqrt(covariance(2,2)));
					m_ctheta_pull	= m_diffcot/(std::sqrt(covariance(3,3)));
					m_ipt_pull	= m_diffipt/(std::sqrt(covariance(4,4)));        
 
				    }

				}//mccollection
                      
			    }
       			}// has truth

		    }// if (k <= m_maxReco )
            
		    // set 9999 when no truth available
		    if (!haveTruth)
		    {
			m_diffA0		= 9999.;
			m_diffZR		= 9999.;
			m_diffeta		= 9999.;
			m_diffphi		= 9999.;
			m_diffcot		= 9999.;
			m_diffipt		= 9999.;
			m_ptptt		= 9999.;
			m_iptptt		= 9999.;
			m_A0_pull		= 9999.;
			m_Z0_pull		= 9999.;
			m_phi_pull		= 9999.;
			m_ctheta_pull	        = 9999.;
			m_ipt_pull		= 9999.;
		    }
		}
	
		// Fill ntuple
		if (! (ntupleSvc()->writeRecord(m_ntuple)).isSuccess())
		{
		    ATH_MSG_WARNING( "problems writing ntuple record" );
		}
	    } // if has truth
      
	} // for tracks
    }
    
    ATH_MSG_VERBOSE( "exit execution" );
    return StatusCode::SUCCESS;    
}

/*
  -----------------------THE FINALIZE METHOD-----------------------------
*/
StatusCode 
iPatNtuple::finalize()
{
    ATH_MSG_INFO( "in finalize()" );
 
    return StatusCode::SUCCESS;
}

void
iPatNtuple::initNTuple(void) 
{
    StatusCode sc;

    // Event info
    sc= m_ntuple->addItem("RunNum",       m_RunNum );
    sc= m_ntuple->addItem("EventNum",     m_EventNum );
    sc= m_ntuple->addItem("iPatEventNum", m_iPatEventNum );
 
    //Hits
    sc = m_ntuple -> addItem ("b_layer_clusters_unique",     m_b_layer_clusters_unique);
    sc = m_ntuple -> addItem ("b_layer_clusters_spoilt",     m_b_layer_clusters_spoilt);
    sc = m_ntuple -> addItem ("b_layer_clusters_wrong",      m_b_layer_clusters_wrong);
    sc = m_ntuple -> addItem ("b_layer_clusters_shared",     m_b_layer_clusters_shared);
    sc = m_ntuple -> addItem ("b_layer_clusters_total",      m_b_layer_clusters_total);

    sc = m_ntuple -> addItem ("outer_pixel_clusters_unique", m_outer_pixel_clusters_unique);
    sc = m_ntuple -> addItem ("outer_pixel_clusters_spoilt", m_outer_pixel_clusters_spoilt);
    sc = m_ntuple -> addItem ("outer_pixel_clusters_wrong",  m_outer_pixel_clusters_wrong);
    sc = m_ntuple -> addItem ("outer_pixel_clusters_shared", m_outer_pixel_clusters_shared);
    sc = m_ntuple -> addItem ("outer_pixel_clusters_total",  m_outer_pixel_clusters_total);

    sc = m_ntuple -> addItem ("pixel_clusters_unique",       m_pixel_clusters_unique);
    sc = m_ntuple -> addItem ("pixel_clusters_spoilt",       m_pixel_clusters_spoilt);
    sc = m_ntuple -> addItem ("pixel_clusters_wrong",        m_pixel_clusters_wrong);
    sc = m_ntuple -> addItem ("pixel_clusters_shared",       m_pixel_clusters_shared);
    sc = m_ntuple -> addItem ("pixel_clusters_total",        m_pixel_clusters_total);

    sc = m_ntuple -> addItem ("strip_clusters_unique",       m_strip_clusters_unique);
    sc = m_ntuple -> addItem ("strip_clusters_spoilt",       m_strip_clusters_spoilt);
    sc = m_ntuple -> addItem ("strip_clusters_wrong",        m_strip_clusters_wrong);
    sc = m_ntuple -> addItem ("strip_clusters_shared",       m_strip_clusters_shared);
    sc = m_ntuple -> addItem ("strip_clusters_total",        m_strip_clusters_total);

    sc = m_ntuple -> addItem ("drift_unique",      m_drift_unique);
    sc = m_ntuple -> addItem ("drift_spoilt",      m_drift_spoilt);
    sc = m_ntuple -> addItem ("drift_wrong",       m_drift_wrong);
    sc = m_ntuple -> addItem ("drift_shared",      m_drift_shared);
    sc = m_ntuple -> addItem ("drift_wires",       m_drift_wires);
    sc = m_ntuple -> addItem ("drift_rejects",     m_drift_rejects);
    sc = m_ntuple -> addItem ("drift_hits",        m_drift_hits);
    sc = m_ntuple -> addItem ("drift_time_hits",   m_drift_time_hits);
    sc = m_ntuple -> addItem ("tr_hits",           m_tr_hits);

    sc = m_ntuple -> addItem ("total_shared_hits",   m_total_shared_hits);
    sc = m_ntuple -> addItem ("total_spoilt_hits",   m_total_spoilt_hits);
    sc = m_ntuple -> addItem ("total_unique_hits",   m_total_unique_hits);
    sc = m_ntuple -> addItem ("total_wrong_hits",    m_total_wrong_hits); 

    sc = m_ntuple -> addItem ("boundary_number",   m_boundary_number);
    sc = m_ntuple -> addItem ("pending_number",    m_pending_number);
    sc = m_ntuple -> addItem ("planar_holes",      m_planar_holes); 

    //Tracks   
    // Reconstructed  
    sc = m_ntuple -> addItem ("nreco",   m_nreco, 0 , m_maxReco);       
    sc = m_ntuple -> addItem ("fitprob", m_nreco,     m_fitprob); 
    sc = m_ntuple -> addItem ("fitchi2", m_nreco,     m_fitchi2);       
    sc = m_ntuple -> addItem ("pt",      m_nreco,     m_pt); 
    sc = m_ntuple -> addItem ("ipt",     m_nreco,     m_ipt); // 1/PT at vertex (signed with charge sign)           
    sc = m_ntuple -> addItem ("phi",     m_nreco,     m_phi); // fitted phi at closest approach to vertex [0,2*M_PI] 
    sc = m_ntuple -> addItem ("eta",     m_nreco,     m_eta);
    sc = m_ntuple -> addItem ("cot",     m_nreco,     m_cottheta); // cot(Theta) of the track at closest approach to vertex          
    sc = m_ntuple -> addItem ("a0",      m_nreco,     m_A0); // transverse impact parameter to the closest point of approach to the vertex
    sc = m_ntuple -> addItem ("z0",      m_nreco,     m_Z0); // z-coordinate of the track at the closest point of approach in R-Phi
    sc = m_ntuple -> addItem ("zmax",    m_zMax);           

    // True  
    // number of KINE tracks
    sc = m_ntuple -> addItem ("nkine",	  m_nkine, 0 , m_maxKine);
    sc = m_ntuple -> addItem ("barcode",              m_barcode);
    sc = m_ntuple -> addItem ("pdgpart",    m_nkine,  m_pdgpart);
    sc = m_ntuple -> addItem ("statuspart", m_nkine,  m_statuspart);
    sc = m_ntuple -> addItem ("pttrue",     m_nkine,  m_pttrue);
    sc = m_ntuple -> addItem ("ipttrue",    m_nkine,  m_ipttrue);               
    sc = m_ntuple -> addItem ("phitrue",    m_nkine,  m_phitrue);  
    sc = m_ntuple -> addItem ("etatrue",    m_nkine,  m_etatrue);
    sc = m_ntuple -> addItem ("absetatrue", m_nkine,  m_absetatrue);
    sc = m_ntuple -> addItem ("cottrue",    m_nkine,  m_cthetatrue);
    sc = m_ntuple -> addItem ("sintrue",    m_nkine,  m_sthetatrue);                 
    sc = m_ntuple -> addItem ("a0true",     m_nkine,  m_A0true);                    
    sc = m_ntuple -> addItem ("z0true",     m_nkine,  m_Z0true);

    // Reconstructed/True
    sc = m_ntuple -> addItem ("ratpt",      m_ptptt);
    sc = m_ntuple -> addItem ("ratipt",     m_iptptt);

    // Pulls
    sc = m_ntuple -> addItem ("pulla0",     m_A0_pull);
    sc = m_ntuple -> addItem ("pullz0",     m_Z0_pull);
    sc = m_ntuple -> addItem ("pullphi",    m_phi_pull);
    sc = m_ntuple -> addItem ("pullcot",    m_ctheta_pull);
    sc = m_ntuple -> addItem ("pullipt",    m_ipt_pull);

    // Diff --> Reconstructed-True
    sc = m_ntuple -> addItem ("diffa0",     m_diffA0);
    sc = m_ntuple -> addItem ("diffzr",     m_diffZR);
    sc = m_ntuple -> addItem ("diffphi",    m_diffphi);
    sc = m_ntuple -> addItem ("diffeta",    m_diffeta);
    sc = m_ntuple -> addItem ("diffcot",    m_diffcot);
    sc = m_ntuple -> addItem ("diffipt",    m_diffipt);
}
  
