/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PDFREWEIGHTTOOL_H
#define PDFREWEIGHTTOOL_H

//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GenInterfaces/IPDFReweightTool.h"
#include "HepMC/GenEvent.h"
#include <string>
#include <vector>

class StoreGateSvc;
class McEventCollection;

//the lhapdf methods to initialize, evaluate and so on
extern "C" {
  extern void initpdfset_(char*, int);
  extern void initpdfsetbyname_(char*, int);
  extern int  numberpdf_(int&); 
  extern void initpdf_(int&);
  extern void evolvepdf_(double&, double &, double*);

  extern void initpdfsetm_(int&,char*, int);
  extern void initpdfsetbynamem_(int&, char*, int);
  extern int  numberpdfm_(int&,int&);
  extern void initpdfm_(int&,int&);
  extern void evolvepdfm_(int&,double&, double &, double*);
}



/**
@class     PDFReweightTool.h 

@brief     tool for calculating MC event weights due to PDF uncertainties. 
	Can be used in systematics studies of different physics analysis 

@author    Gia Khoriauli  and  Markus Cristinziani
        Universitat Bonn

	May, 2008  
*/
class PDFReweightTool : virtual public IPDFReweightTool, public AthAlgTool {

public:

	PDFReweightTool(const std::string&, const std::string&, const IInterface*);
	~PDFReweightTool();

	virtual StatusCode initialize();

	//will call some lhapdf functions
	//to initialize a PDF set and
	//evaluate PDF values for the
	//members of this PDF set
	//calculates weights and
	//stores them
	virtual StatusCode execute();
	virtual StatusCode Reweight(HepMC::GenEvent*);

	//methods to pass interactied parton
	//characteristics (flavors, x-momentum
	//fractions and transmited momentum - Q)
	//to this tool
	void 	SetX1	(double);
	void 	SetFl1	(int);
	void 	SetX2	(double);
	void 	SetFl2	(int);
	void 	SetQ	(double);
	void 	SetPDFInputs(double, int, double, int, double);

	//to get them back (if necessary...)
	double 	GetX1	();
	int		GetFl1	();
	double 	GetX2	();
	int 	GetFl2	();
	double 	GetQ	();

	//scales m_x1 and m_x1 to the new beam energy
	//if it is requested to reweight to the different c.m. energy
	void	ScaleXs	();

	//methods to retrive the calculated weights and their vector
	double  					GetWeight		(int);
	const std::vector<double>& 	GetWeightVec	();

	//methods to be used by generetors to pass
	//the values of evaluated PDFs to this tool
	//and to retrieve those values by this tool
	void	SetEventPDF1	(double);
	void    SetEventPDF2    (double);
	double	GetEventPDF1	();
	double	GetEventPDF2	();

	void                    SetGenEvent     (HepMC::GenEvent*);
	HepMC::GenEvent*        GetGenEvent     ();

private:

	HepMC::GenEvent*	m_evt;
	McEventCollection*	m_mceventTESout;
	StoreGateSvc*		m_storeGate;


	//propearties
	bool		m_GeneratorUse;
	bool		m_RewriteWeights;
	std::string	m_OrigPDFSetName;
	std::string	m_PDFSetName;
	int			m_PDFSetID;
	std::string m_McEventCollection;
	std::string m_McEventCollectionOut;

	bool		m_DifferentCMEnergies;
	double		m_OrigBeamEnergy;
	double		m_NewBeamEnergy;


	//evnet related variables, which are
	//necesary to evaluate PDF weights
	//the user of this tool has to
	//initialize them in each event
	int    m_fl1;
	int    m_fl2;
	double m_x1;
	double m_x2;
	double m_Q;

	//the vector will be filled whith
	//calculated wheigts for the event
	//the content of m_weights_vec
	//should be stored in GenEvent
	//weights vector
	std::vector<double> m_weights_vec;

	//evaluated PDF values for interacting
	//initial partons, which were used
	//when generating event
	//should also be provided by a Tool user
	double m_used_pdf1;
	double m_used_pdf2;

	double	m_weightFactor;

};


inline void            	PDFReweightTool::SetGenEvent    (HepMC::GenEvent* p_evt){ m_evt = p_evt; }
inline HepMC::GenEvent*	PDFReweightTool::GetGenEvent    () 						{ return m_evt; }

inline void 		PDFReweightTool::SetX1			(double p_x1)		{ m_x1	= p_x1; }
inline void 		PDFReweightTool::SetFl1			(int    p_fl1)		{ m_fl1	= p_fl1; }
inline void 		PDFReweightTool::SetX2			(double p_x2)		{ m_x2	= p_x2; }
inline void 		PDFReweightTool::SetFl2			(int    p_fl2)		{ m_fl2 = p_fl2; }
inline void 		PDFReweightTool::SetQ			(double p_Q)		{ m_Q	= p_Q; }
inline void 		PDFReweightTool::SetEventPDF1	(double p_used_pdf1){ m_used_pdf1 = p_used_pdf1; }
inline void 		PDFReweightTool::SetEventPDF2	(double p_used_pdf2){ m_used_pdf2 = p_used_pdf2; }

inline double	PDFReweightTool::GetX1			()			{ return m_x1; }
inline int		PDFReweightTool::GetFl1			()			{ return m_fl1; }
inline double	PDFReweightTool::GetX2			()			{ return m_x2; }
inline int		PDFReweightTool::GetFl2			()			{ return m_fl2; }
inline double	PDFReweightTool::GetQ			()			{ return m_Q; }
inline double	PDFReweightTool::GetEventPDF1 	() 			{ return m_used_pdf1; }
inline double 	PDFReweightTool::GetEventPDF2 	()			{ return m_used_pdf2; }

inline const std::vector<double>& PDFReweightTool::GetWeightVec ()      	{ return m_weights_vec; }

#endif 
