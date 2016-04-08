/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      tool for calculating MC event weights due to PDF
//      uncertainties. Can be used in systematics studies
//
//      authors: Gia Khoriauli  and  Markus Cristinziani
//              Universitat Bonn
//
//      date:   November 2007
//      
/////////////////////////////////////////////////////////////

#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"
#include "DataModel/DataVector.h"
#include "GeneratorObjects/McEventCollection.h"

#include "HepMC/GenEvent.h"
#include "HepMC/PdfInfo.h"
#include "ReweightTools/PDFReweightTool.h"

#include <math.h>

PDFReweightTool::PDFReweightTool(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent)
	: AthAlgTool(type, name, parent), 
	m_evt(NULL),
	m_mceventTESout(NULL),
	m_storeGate(NULL),
	m_fl1(0),
	m_fl2(0),
	m_x1(0.),
	m_x2(0.),
	m_Q(0.),
	m_used_pdf1(0.),
	m_used_pdf2(0.),
	m_weightFactor(1.)	
{	
	declareProperty("GeneratorUse", 		m_GeneratorUse			= false);
	declareProperty("RewriteWeights", 		m_RewriteWeights		= false);
	declareProperty("OrigPDFSetName", 		m_OrigPDFSetName		= "");
	declareProperty("PDFSetName", 			m_PDFSetName			= "");
	declareProperty("PDFSetID", 			m_PDFSetID				= 0);
	declareProperty("McEventCollection", 	m_McEventCollection		= "");
	declareProperty("McEventCollectionOut",	m_McEventCollectionOut	= "");
	declareProperty("DifferentCMEnergies",	m_DifferentCMEnergies	= false);
	declareProperty("OrigBeamEnergy", 		m_OrigBeamEnergy		= -999.);
	declareProperty("NewBeamEnergy", 		m_NewBeamEnergy			= -999.);
}


PDFReweightTool::~PDFReweightTool() {
}


StatusCode PDFReweightTool::initialize() {
	msg(MSG::INFO) 
	<< "Initializing PDF Reweighting Tool " 
	<< endreq;	

	//retrieve StoreGate
	StatusCode sc = service("StoreGateSvc", m_storeGate);
	if (sc.isFailure()) {
     		msg(MSG::ERROR)
          	<< "Unable to retrieve pointer to StoreGateSvc"
          	<< endreq;
    		return sc;
  	}
	
	//consistency check for the tool propierties
	//check #1	
	if(m_GeneratorUse && m_DifferentCMEnergies)	{
		msg(MSG::WARNING)
		<<"Reweighting between the different c.m. energies is requested during the event generation... "
		<<"Usually, it is needed for the existing events. At this moment the run continues but it might fail. "
		<<"Please, check the tool settings"
		<< endreq;
	}

	//check #2
	if(m_DifferentCMEnergies)	{
		if (m_OrigBeamEnergy<=0. || m_NewBeamEnergy<=0.)	{ 
			msg(MSG::ERROR)
			<< "Reweighting between the different c.m. energies is requested. "
			<< "But the proper original and new beam energy values are not set. "
			<< "The corresponding propierty names are: 'OrigBeamEnergy' and 'NewBeamEnergy' "
			<< endreq;

			return StatusCode::FAILURE;			
		}
	
		//force do not store any PDF set id in this mode of reweighting
		m_PDFSetID=0;
	}
	
	//check #3
	if(m_OrigPDFSetName == "")	{
		msg(MSG::ERROR)
		<<"The original (used or being used in the event generation) PDF set name has to be set. "
		<<"Check the tool propierty 'OrigPDFSetName' "
		<< endreq;

		return StatusCode::FAILURE;
	}

	//check #4
	if( (m_GeneratorUse || m_DifferentCMEnergies) && m_PDFSetName != "") 	{
		msg(MSG::WARNING)
		<<"In case of reweighting with the event generation or the different c.m. energies modes "
		<<"the second PDF set name has to be empty (it is not needed). Force 'PDFSetName' = ''. "
		<<"If none of the above modes are assumed, then make the both propierties false: "
		<<"'GeneratorUse' and 'DifferentCMEnergies' and re-run "
		<< endreq;

		m_PDFSetName = "";
	}

	//initialize PDF error sets.
	int  oldset=1;	//do not change the number
	char pdf_set_name[500];
               
	if (sizeof(m_OrigPDFSetName) < 500) (strcpy(pdf_set_name, m_OrigPDFSetName.c_str()));
	else {
             msg(MSG::ERROR)
             << "OigPDFSetName is too long"
             << endreq;
  	}

	initpdfsetbynamem_(oldset, pdf_set_name, strlen(pdf_set_name));
	msg(MSG::INFO) << " PDF set "<<m_OrigPDFSetName<<" has been initialized"<< endreq;

	if(m_PDFSetName != "")	{	
		int newset=2;	//do not change the number
		char new_pdf_set_name[500];
		if (sizeof(m_PDFSetName) < 500) {
		      strcpy(new_pdf_set_name, m_PDFSetName.c_str());}
		else
		   {
                    msg(MSG::ERROR)
                    << "PDFSetName is too long"
                    << endreq;}

		initpdfsetbynamem_(newset, new_pdf_set_name, strlen(new_pdf_set_name));
		msg(MSG::INFO) << " PDF set "<<m_PDFSetName<<" has been initialized"<< endreq;
	}

	//the re-weighting 'flavor' is defined. anounce about it.
	//scenario #1
	if(m_DifferentCMEnergies)	{
		msg(MSG::INFO)
		<<"Reweighting from the original bean energy "
		<< m_OrigBeamEnergy
		<<" TeV to the new energy "
		<< m_NewBeamEnergy
		<<" TeV will be performed"
		<< endreq;
	}
	//scenario #2
	//this scenario has two possible options: 
	// 1) reweighting runs during the event generation
	// 2) run on the already existing events
	else if(m_OrigPDFSetName!="" && m_PDFSetName=="")	{
		msg(MSG::INFO) 
		<<"Event PDF weights will be calculated for this error PDF set - "
		<<m_OrigPDFSetName
		<<endreq;
	}
	//scenario #3
	else {	// m_PDFSetName!=""
		msg(MSG::INFO) 
		<<"Events generated with the original (central value) PDF - " 
		<< m_OrigPDFSetName
		<<" will be reweighted to the new error PDF set - "
		<<m_PDFSetName
		<<endreq;
	}
	
	return StatusCode::SUCCESS;
}



StatusCode PDFReweightTool::execute() {
 	
	msg(MSG::DEBUG) << " Execute PDF Reweighting Tool " << endreq;	

	StatusCode sc;	

	if(!m_GeneratorUse && m_McEventCollection=="" && m_McEventCollectionOut!="") {
		msg(MSG::WARNING)
		<< "No input McEventCollection is specified. "
		<< "Can't create an output collection for reweighted events. Nothing will be done"
		<< endreq;
		return StatusCode::SUCCESS;
	}

	//runs on the existing McEventCollection and to store the re-weighted events 
	//in a new McEventCollection is requested 
	if(!m_GeneratorUse && m_McEventCollectionOut!="")	{
		m_mceventTESout = new McEventCollection();
		sc = m_storeGate->record( m_mceventTESout, m_McEventCollectionOut);
  		if( sc.isFailure() ) {
     			msg(MSG::WARNING)
          		<< "New MC event container was not recorded in TDS"
          		<< endreq;
     			return StatusCode::SUCCESS;
  		}
		msg(MSG::DEBUG) << "New MC Event Container Was Successfully Recorded" << endreq;
	}

	
	const McEventCollection* mceventTES;	
	
	if(!m_GeneratorUse) { //runs on a POOL file (EVNT, AOD and etc.)
		if(m_McEventCollection!="") {//input McEventCollection is specified
			sc = m_storeGate->retrieve( mceventTES, m_McEventCollection);
  			if( sc.isFailure()  ||  !mceventTES ) {
     				msg(MSG::WARNING)
          			<< "No MC event container found in TDS"
	          		<< endreq;
     				return StatusCode::SUCCESS;
  			}
			msg(MSG::DEBUG) << "MC Event Container Was Successfully Retrieved" << endreq;
		
			//loop over the events in a container
			McEventCollection::const_iterator iter 		= mceventTES->begin();
			McEventCollection::const_iterator iterEnd 	= mceventTES->end();
		
			for (; iter!=iterEnd; iter++) {
		    	HepMC::GenEvent* evt = new HepMC::GenEvent(**iter);
				sc = this->Reweight(evt);
				if( sc.isFailure() ) {
					msg(MSG::WARNING)
							<< "Event PDF re-weighting failed"
							<< endreq;
					return StatusCode::SUCCESS;
				}

				if(m_McEventCollectionOut!="")	{
					m_mceventTESout->push_back(evt);
				}
			}
		}
		else {
                        msg(MSG::WARNING) 
			<< "Input MC Event Container is not specified. "
			<< "Reweighting won't be performed" 
			<< endreq;
		}
	}
	else { //runs during event generation
		//assumes that the tool owner generator initializes its member 
		//GenEvent* pointer by setting it to the current event 
		HepMC::GenEvent* evt = this->GetGenEvent();
		if(evt)	{
			sc = this->Reweight(evt);
			if( sc.isFailure() ) {
				msg(MSG::WARNING)
						<< "Event PDF re-weighting failed"
						<< endreq;
				return StatusCode::SUCCESS;
			}
		}
		else	{
			msg(MSG::WARNING) 
					<< "Can't retrieve the current GenEvent. "
					<< "Reweighting can't be performed"
					<< endreq;
		}
	}

	return StatusCode::SUCCESS;
}
	

	
//StatusCode PDFReweightTool::Reweight(const HepMC::GenEvent* evt_orig) {
StatusCode PDFReweightTool::Reweight(HepMC::GenEvent* evt) {
			
	msg(MSG::DEBUG) << " Reweight PDF " << endreq;	
	
	//safety check
	if (!evt) {
                msg(MSG::WARNING) 
		<< " Invalid pointer to a GenEvent. Nothing will be done"
		<< endreq;
	        return StatusCode::SUCCESS;	
	}

	HepMC::PdfInfo* pdf_info = evt->pdf_info();
	
	//safety check
	if (pdf_info) {
	        msg(MSG::DEBUG) << " PdfInfo has been retrieved"<< endreq;		
	}
	else {
                msg(MSG::WARNING) 
		<< " PdfInfo could not be retrieved. Nothing will be done"
		<< endreq;
	        return StatusCode::SUCCESS;	
	}
	
	//get the event info about x's, flavor's and an event scale
	this->SetPDFInputs(
		pdf_info->x1(),
		pdf_info->id1(),
		pdf_info->x2(),
		pdf_info->id2(),
		pdf_info->scalePDF()
	);

	//if the initial partons are gluons change their PDG values 
	//to the right indices of the corresponding PDF values in
	//the PDF array filled by evolvepdf_() - method 
	int fl1 = 0;
	int fl2 = 0;
	if(m_fl1==21)	fl1 = 0;	else fl1 = m_fl1;
	if(m_fl2==21)	fl2 = 0; 	else fl2 = m_fl2;

	m_weightFactor	= 1.;

	//Retrieve PDF evoluation values for the initial partons.
	if (!m_GeneratorUse) {
		m_used_pdf1 = pdf_info->pdf1();
		m_used_pdf2 = pdf_info->pdf2();
	}
	else {
		//store the fake values. they will be recalculated later
		//using the central value PDF of the specified error PDF set
		this->SetEventPDF1( -999. );
		this->SetEventPDF2( -999. );
	}

	
	//do not change the numbers
	int cvpdf=0; 
	int oldset=1; 

	//pdf values are positive and non-zero.
	//if the pdf_info is empty for some reason or tool is running during the events generation,
	//then the pdf values either are not retrieved or they are not stored in the pdfinfo yet. calculate them here.
	if(m_used_pdf1<=0. || m_used_pdf2<=0.)	{
		double f1[13];
		double f2[13];
   			
		initpdfm_(oldset,cvpdf);
		evolvepdfm_(oldset,m_x1,m_Q,f1);
		evolvepdfm_(oldset,m_x2,m_Q,f2);

		this->SetEventPDF1( f1[fl1+6] );
		this->SetEventPDF2( f2[fl2+6] );
	}


	msg(MSG::DEBUG) <<" Interacted partons info :  		" 
			<<"   Q="<<m_Q
			<<"   x1="<<m_x1
			<<"   fl1="<<m_fl1
			<<"   pdf1="<<m_used_pdf1
			<<"   x2="<<m_x2
			<<"   fl2="<<m_fl2
			<<"   pdf2="<<m_used_pdf2
			<< endreq;	


	//clear/reset the weight vector		
	m_weights_vec.clear();		

	//do not change the numbers										
	int n_pdf=0;
	int newset=2;

	//retrieve number of error PDFs in the set
	//scenario #3
	if(m_PDFSetName!="")	
		numberpdfm_(newset, n_pdf);
	//scenario #2
	else if(!m_DifferentCMEnergies)
		numberpdfm_(oldset, n_pdf);

	//scenario #1
	//needs x1 and x2 to be recalculated according to the new beam energy
	if(m_DifferentCMEnergies)
		this->ScaleXs();

	//loop over error PDF set and calculate event weights.
	//in case of the scenario #1 n_pdf=0 and therefore, 
	//only the central value PDF of the specified (original) 
	//error PDF set will be used
	for(int i=0; i<=n_pdf; i++) {
		
		double f1[13];
		double f2[13];

		//scenario #3
		if(m_PDFSetName!="")	{
			//init i-th member of the PDF set
			initpdfm_(newset,i);
			//evaluate PDFs
			evolvepdfm_(newset,m_x1,m_Q,f1);
			evolvepdfm_(newset,m_x2,m_Q,f2);
		}
		//scenario #1 and #2
		else {
			initpdfm_(oldset,i);
			evolvepdfm_(oldset,m_x1,m_Q,f1);
			evolvepdfm_(oldset,m_x2,m_Q,f2);
		}
	
		msg(MSG::DEBUG) <<" Interacted partons re-weighted info: "
				<<"  pdf id="<<i
				<<"  x1="<<m_x1<<"  fl1="<<m_fl1<<"     pdf1="<<f1[fl1+6]
				<<"  x2="<<m_x2<<"  fl2="<<m_fl2<<"     pdf2="<<f2[fl2+6]
				<<endreq;

	
		//if this is the first PDF from the PDF set (the central value PDF)	
		if (i==0) {
			//if the used PDF values are not set, then use the PDFs evaluated
			//at the current PDF set central value.
			if(m_used_pdf1<=0.)	{	//assumes that m_used_pdf2 was not set too
				this->SetEventPDF1( f1[fl1+6] );
				this->SetEventPDF2( f2[fl2+6] );
			}
			//if they are there, then use them to calculate the 'weighting factor' for all new weights, 
			//which is equal to
			//	 (cvPDF_new(p1) * cvPDF_new(p2)) / (cvPDF_original(p1) * cvPDF_original(p2))  
			else {
				if (fabs(m_used_pdf1)>0.) m_weightFactor *= f1[fl1+6]/m_used_pdf1;
				if (fabs(m_used_pdf2)>0.) m_weightFactor *= f2[fl2+6]/m_used_pdf2;

				//and replace original PDF values
				this->SetEventPDF1( f1[fl1+6] );
				this->SetEventPDF2( f2[fl2+6] );
			}
		}	
	
		
		//calculate event weight at 'this' error PDF
		double weight = m_weightFactor;
		if (fabs(m_used_pdf1)>0.) weight *= f1[fl1+6]/m_used_pdf1;
		if (fabs(m_used_pdf2)>0.) weight *= f2[fl2+6]/m_used_pdf2;

		msg(MSG::DEBUG)  <<" weight = "
				<< weight
				<< endreq;

		//store the weight		   
		m_weights_vec.push_back(weight);
	}        


	//if the weights are being calculated during the event generation then nothing to re-write
	if (m_GeneratorUse) 	m_RewriteWeights = false;
	
	//if re-write of the already existing PDF weights is required then save only the first weight
	//which comes from a generator, flush other weights, which are supposed to be PDF weights 
	if(m_RewriteWeights)	{
		if(!(evt->weights().empty()))	{
			msg(MSG::WARNING) 
			<<" Event weights are going to be re-writed." 
			<<" This option works properly only for the HERWIG samples "
			<<endreq; 

			double genWeight = evt->weights()[0];
			evt->weights().clear();
			if(genWeight<99.) //so, it's probably the generator event weight but not a PDF LHAGLUE number 
				evt->weights().push_back(genWeight);
		}
	}

	//Save the unique LHAGLUE number of the PDF or the PDF set, which was used for the weights calculation.
	//push that PDF LHAGLUE number and the number of the PDF set members into weight vector first
	if(m_PDFSetID!=0) {
		evt->weights().push_back((double)m_PDFSetID);
		evt->weights().push_back((double)(n_pdf+1));			
	}
		
	//now, store the weights into the GenEvent WeightContainer
	for(unsigned int i=0; i<m_weights_vec.size(); i++)	{
		evt->weights().push_back(m_weights_vec[i]);
	}

	//re-initialize a central value PDF for the next event
	int cv_pdf = 0;
	if(m_PDFSetName!="") 	
		initpdfm_(newset, cv_pdf);
	else
		initpdfm_(oldset, cv_pdf);
	

	return StatusCode::SUCCESS;
}


void PDFReweightTool::SetPDFInputs(	double  p_x1, 
					int 	p_fl1, 
					double  p_x2, 
					int 	p_fl2, 
					double	p_Q) {
	SetX1(p_x1);
	SetFl1(p_fl1);
	SetX2(p_x2);
	SetFl2(p_fl2);
	SetQ(p_Q);
	
}


double PDFReweightTool::GetWeight (int i) {
	if(i>-1 && i<(int)m_weights_vec.size())	
		return m_weights_vec[i];
	else
		return -999.;	
}

void PDFReweightTool::ScaleXs()	{
	//the tool checks that m_NewBeamEnergy>0. 
	//in its initialize() - method
	m_x1 *= m_OrigBeamEnergy/m_NewBeamEnergy;
	m_x2 *= m_OrigBeamEnergy/m_NewBeamEnergy;
}


