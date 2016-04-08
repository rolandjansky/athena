/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//c - c++
#include "iostream"

//MdtCalibFitters
#include "MdtCalibFitters/DCSLFitter.h"

//MuonCalibStl
#include "MuonCalibStl/ToString.h"

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibSegment.h"

//MuonCalibData
#include "MdtCalibData/RtResolutionFromPoints.h"
#include "MdtCalibData/RtChebyshev.h"
#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibData/IRtResolution.h"

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibSegment.h"

//MuonCalibStandAloneTools
#include "MuonCalibStandAloneTools/NtupleChisqResolutionTool.h"
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"

//root
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMinuit.h"
#include "TString.h"
#include "TF1.h"
#include "TMath.h"


using namespace std;

namespace MuonCalib {

  //******************************************************************************
  static NtupleChisqResolutionTool* static_NtupleChisqResolutionTool_pointer = NULL;

  inline void NtupleChisqResolutionTool_fcn_wrapper(int &npar, double *gin, double &f, double *par, int iflag) {
    if(static_NtupleChisqResolutionTool_pointer==NULL) {
      exit(-1);
    }
    std::cout<<"entro in fcn_wrapper, npar="<<npar<<std::endl;
    static_NtupleChisqResolutionTool_pointer->fcn(npar, gin, f, par, iflag);
  }
  
  //****************************************************************************  

 
 
 
  NtupleChisqResolutionTool::NtupleChisqResolutionTool(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t, n, p), m_calib_input_svc("MdtCalibInputSvc", n), m_final_resolution(NULL) 
  {
    declareInterface< NtupleCalibrationTool >(this) ;	
    declareProperty("MdtCalibInputSvc", m_calib_input_svc);
  }
  
  //******************************************************************************
  
  NtupleChisqResolutionTool::~NtupleChisqResolutionTool()
  {
  }
	
  //******************************************************************************
	
  StatusCode NtupleChisqResolutionTool::initialize()
  {
    ATH_MSG_INFO( "initialize()" );
    ATH_CHECK( m_calib_input_svc.retrieve() );
    return StatusCode :: SUCCESS;	
  }
 
  //******************************************************************************

  StatusCode NtupleChisqResolutionTool::finalize()
  {
    m_resolfile->Write();
    return StatusCode::SUCCESS;
  }
 

  //******************************************************************************


  void NtupleChisqResolutionTool :: setRegion()
  {
    //try to get rt relation
    p_rt_rel = m_calib_input_svc->GetRtRelation();
    //root stuff
    m_resolfile = TFile::Open("resolfile.root","RECREATE");	
    m_prob_dist = new TH1F("egment_probability_distribution" ,"segment probability distribution",50,0.,1.);
    //    m_prob_debg = new TH2F(id.regionId().c_str() ,"segment prob vs hits mdt",20,0.,20.,50,0.,1.);
  }

  //******************************************************************************
	
  StatusCode NtupleChisqResolutionTool::analyseSegments(const std::vector<MuonCalibSegment *> & segemnts)
  {
    if(p_rt_rel == NULL)
      {
	ATH_MSG_FATAL( "Rt relation not set!" );
	return StatusCode::FAILURE;
      }
    m_seg=&segemnts;
    //	double *par;
    //      recalculate(par);
    m_minuit = new TMinuit(3); //initialize TMinuit with 3 parameters
    static_NtupleChisqResolutionTool_pointer = this;
    m_minuit->SetFCN(NtupleChisqResolutionTool_fcn_wrapper);
    m_minuitoutputlevel=1;
    Double_t vstart[3]={.1,-.3,.05};
    Double_t step[3]={.01,.01,.01};
    double arglist[10];
    int ierflg=0;
    m_minuit->mnparm(0,"resol_0", vstart[0], step[0], .01, .5, ierflg);
    m_minuit->mnparm(1,"tauexp", vstart[1], step[1], .01, .8, ierflg);
    m_minuit->mnparm(2,"resol_1", vstart[2], step[2], .05, .2, ierflg);
    arglist[0]=1;
    m_minuit->mnexcm("SET ERR",arglist,1,ierflg);
    arglist[0]=0;
    m_minuit->mnexcm("SET STR",arglist,1,ierflg);
    arglist[0]=1.e11;
    arglist[1]=0.1;
    m_minuit->mnexcm("SIMPLEX",arglist,2,ierflg);

    //formerly finalize
    TString name[3] = {"resol_0", "tauexp", "resol_1"};
    double fitpar[3];
    double dummy1, dummy2, dummy3;
    int idummy;
    m_minuit->mnpout(0, name[0], fitpar[0], dummy1, dummy2, dummy3, idummy);
    m_minuit->mnpout(1, name[1], fitpar[1], dummy1, dummy2, dummy3, idummy);
    m_minuit->mnpout(2, name[2], fitpar[2], dummy1, dummy2, dummy3, idummy);

    // get minimum and maximum drift time //
    RtResolutionFromPoints res_from_points;
    double t_min=0, t_max=800;
    const RtChebyshev *rt_Chebyshev(dynamic_cast<const RtChebyshev *>(p_rt_rel));
    const RtRelationLookUp *rt_LookUp(
				      dynamic_cast<const RtRelationLookUp *>(p_rt_rel));
    // RtChebyshev //
    if (rt_Chebyshev!=0) {
      t_min = rt_Chebyshev->tLower();
      t_max = rt_Chebyshev->tUpper();
    }

    // RtRelationLookUp, dangerous implementation, but the only way right now //
    if (rt_LookUp!=0) {
      t_min = rt_LookUp->par(0);
      t_max = rt_LookUp->par(1)*(rt_LookUp->nPar()-2);
    }

    // perform sigma-t fit //
    vector<SamplePoint> point(31);
    double r_min(p_rt_rel->radius(t_min));
    double r_max(p_rt_rel->radius(t_max));
    double bin_width((r_max-r_min)/static_cast<double>(point.size()-1));
    for (unsigned int k=0; k<point.size(); k++) {
      double radius(r_min+k*bin_width);
      point[k].set_x1(t_from_r(radius, p_rt_rel));
      point[k].set_x2(fitpar[0]*exp(-fitpar[1]*radius)+fitpar[2]);
      point[k].set_error(1.0);
    }
    m_final_resolution = new RtResolutionChebyshev(
						   (res_from_points.getRtResolutionChebyshev(
											     point, 8)));
    
    return StatusCode::SUCCESS;
  }
  
  
 
  void NtupleChisqResolutionTool::fcn(int&/*npar*/, double * /*gin*/, double &f, double *par, int iflag)
  {
    ATH_MSG_INFO( "entering fcn"<<" iflag ="<<iflag );
    m_prob_dist->Reset("ICE");
    //    m_prob_debg->Reset("ICE");
    f = recalculate(par);
    m_prob_dist->Print("all");
    m_prob_dist->Fit("pol0","L","L",0.02,1.);
    TF1 *pol0fit = m_prob_dist->GetFunction("pol0");
    double chidue = pol0fit->GetChisquare();
    /*
      now account for first bin
    */
    Double_t first_bin = m_prob_dist->GetBinContent(1);
    Double_t total_sum = m_prob_dist->Integral();
    //    Double_t total_sum = m_prob_dist->GetEntries();
    double expected_first_bin = total_sum/50.;
    //    std::cout<<" first_bin "<<first_bin<<" total_sum "<<total_sum<<" expt first bin "<<expected_first_bin<<std::endl;
    double ratio = (first_bin / expected_first_bin)*5.;
    //    double first_bin_chi2 = ratio;
    //std::cout<<" par "<<par[0]<<" "<<par[1]<<" "<<par[2]<<" ratio,chi2="<<ratio<<" "<<chidue<<std::endl;
    f=chidue+ratio*100.;
  }
  double NtupleChisqResolutionTool::recalculate(double *par)
  {
    //    ToString toString;
    double r;
//    double dr;
//    double t;
     
    for (unsigned int k=0; k<m_seg->size(); k++) {

      for(std::vector<MdtCalibHitBase*>::iterator it =(m_seg->at(k))->mdtHOTBegin() ;   
	  it!=(m_seg->at(k))->mdtHOTEnd();++it)   
	{
//	  t=(*it)->driftTime(); 
	  r=(*it)->driftRadius();
	  double newResol = par[0]*exp(-par[1]*r)+par[2];
//	  dr=(*it)->sigmaDriftRadius();
	  (*it)->setDriftRadius(r,newResol);
	}  

      DCSLFitter fitter;
      fitter.fit(*(m_seg->at(k)));
      double redchi2=(m_seg->at(k))->chi2();
      int nmdthits=(m_seg->at(k))->hitsOnTrack();
    
      float probability=(float) TMath::Prob(redchi2 * (nmdthits-2),nmdthits-2);

      m_prob_dist->Fill(probability);
    }
    
    return 1.;
  }


  //************************************************************

  /////////////////////
  // METHOD t_from_r //
  /////////////////////

  double NtupleChisqResolutionTool::t_from_r(const double & r,
					     const IRtRelation * rt_rel) {

    ///////////////
    // VARIABLES //
    ///////////////

    double precision(0.010); // spatial precision of the inversion 
    double t_max(0.0); // upper time search limit
    double t_min(1000.0); // lower time search limit

    ///////////////////////////////
    // DETERMINE t_min and t_max //
    ///////////////////////////////

    const RtChebyshev *rt_Chebyshev(
				    dynamic_cast<const RtChebyshev *>(rt_rel));
    const RtRelationLookUp *rt_LookUp(
				      dynamic_cast<const RtRelationLookUp *>(rt_rel));
    // RtChebyshev //
    if (rt_Chebyshev!=0) {
      t_min = rt_Chebyshev->tLower();
      t_max = rt_Chebyshev->tUpper();
    }

    // RtRelationLookUp, dangerous implementation, but the only way right now //
    if (rt_LookUp!=0) {
      t_min = rt_LookUp->par(0);
      t_max = rt_LookUp->par(1)*(rt_LookUp->nPar()-2);
    }

    /////////////////////////////////////////////
    // SEARCH FOR THE CORRESPONDING DRIFT TIME //
    /////////////////////////////////////////////

    while (t_max-t_min>0.1 &&
	   fabs(rt_rel->radius(0.5*(t_min+t_max))-r)>precision) {

      if (rt_rel->radius(0.5*(t_min+t_max))>r) {
	t_max = 0.5*(t_min+t_max);
      } else {
	t_min = 0.5*(t_min+t_max);
      }

    }

    return 0.5*(t_min+t_max);

  }


  const IRtResolution * NtupleChisqResolutionTool::getResolution() const
  {
    if(m_final_resolution == 0)
      {
	ATH_MSG_FATAL( "No resolution Calculated!" );
      }
    return m_final_resolution;
  }


}
