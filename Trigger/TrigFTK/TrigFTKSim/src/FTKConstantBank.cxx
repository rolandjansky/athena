/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKConstantBank.h"
#include "TrigFTKSim/FTKTrack.h"
#include "TrigFTKSim/ftk_dcap.h"

//#include <TMatrixD.h>
#include <TVectorD.h>
#include <TMath.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdio>
using namespace std;

//#define SIMPLEMJ // ibl undefined simple majority to see if we can get majority for ibl
//#define SIMPLEMJ_DBG

FTKConstantBank::FTKConstantBank() :
  m_bankID(-1), m_nsectors(0), m_nplanes(0),
  m_npars(0), m_ncoords(0), m_nconstr(0), m_isgood(0),
  m_fit_pars(0), m_fit_const(0), m_kernel(0), m_kaverage(0),
  m_invfit_consts(0x0),
  m_maj_a(0), m_maj_kk(0), m_maj_invkk(0)
{
  // nothing to do
}


FTKConstantBank::FTKConstantBank(int ncoords, const char *fname) :
  m_invfit_consts(0x0)
{
  //TODO: make possible to read constants in different formats
  m_ncoords = ncoords;
  m_coordsmask = new int[m_ncoords];
  m_missid = new int[m_ncoords];

   // if boost::iostreams is available, use it to give C++ iostreams
   // the ability to read from dCache, gzip, and/or bzip2 sources.
   // otherwise, stick with Guido's implementation and read text
   // files straight from a regular filesystem.
  ftk_dcap::istream geocfile;
  const bool ok_read = ftk_dcap::open_for_read(fname,geocfile);
  if (!geocfile || !ok_read) {
    FTKSetup::PrintMessageFmt(ftk::sevr,"Constants' file: %s invalid\n",
			      fname);
  }

  string line;
  for (int i=0;i<5;++i) // skip 5 lines
    getline(geocfile,line);
  
  string key;
  int ival;

  geocfile >> key;
  if (key!="NPLANES") {
    FTKSetup::PrintMessageFmt(ftk::sevr,"Invalid file format in: %s\n",
			      fname);
  }
  geocfile >> ival;

  geocfile >> key;
  if (key!="NSECTORS") {
    FTKSetup::PrintMessageFmt(ftk::sevr,"Invalid file format in: %s\n",
			      fname);
  }
  geocfile >> m_nsectors;

  geocfile >> key;
  if (key!="NDIM") {
    FTKSetup::PrintMessageFmt(ftk::sevr,"Invalid file format in: %s\n",
			      fname);
  }
  geocfile >> ival;
  if (ival==1)
    m_npars = 3;
  else if (ival==2)
    m_npars = 5;
  else 
    FTKSetup::PrintMessage(ftk::sevr,"Number of dimension invalid");

  // fix the constraints number
  m_nconstr = m_ncoords-m_npars;

  // allocate the block of pointer per sector
  m_isgood = new bool[m_nsectors];
  m_fit_pars = new float**[m_nsectors];
  m_fit_const = new float*[m_nsectors];
  m_kernel = new float**[m_nsectors];
  m_kaverage = new float*[m_nsectors];

  int isec_step = 1 + m_nsectors/10;
  for (int isec=0;isec<m_nsectors;++isec) { // loop over sectors
    double dval; // tmp value used to conver from double to float
    if (isec%isec_step == 0)
      cout << "Load sector: " << isec << "/" << m_nsectors << endl;

    // check variable initilized to "false"
    m_isgood[isec] = false;

    geocfile >> key >> ival;
    if (ival!=isec) {
      cerr << "Attempt to read sector # " << ival << " (" << isec << ")" << endl;
      FTKSetup::PrintMessage(ftk::sevr,"Lost sync in constants file");
      assert(ival-isec);
    }
    
    // allocate space for constants in this sector
    m_fit_pars[isec] = new float*[m_npars];
    m_fit_const[isec] = new float[m_npars];
    for (int i=0;i<m_npars;++i) {
      m_fit_pars[isec][i] = new float[m_ncoords];
    }
    
    m_kernel[isec] = new float*[m_nconstr];
    m_kaverage[isec] = new float[m_nconstr];
    for (int i=0;i<m_nconstr;++i) {
      m_kernel[isec][i] = new float[m_ncoords];
    }

    for (int ipar=0;ipar<m_npars;++ipar) { // pars loop
      geocfile >> key;
      for (int icoord=0;icoord<m_ncoords;++icoord) { // coords loop
	geocfile >> dval;
	if (dval!=0.) m_isgood[isec] = true;
	m_fit_pars[isec][ipar][icoord] = dval;
      } // end coords loop
    } // end pars loop


    geocfile >> key;
    assert(key=="kaverages");
    for (int ik=0;ik<m_nconstr;++ik) { // loop kaverages
      geocfile >> dval;
      if (dval!=0.) m_isgood[isec] = true;
      m_kaverage[isec][ik] = dval;
    } // end loop kaverages


    geocfile >> key;
    assert(key=="kernel");
    for (int ik=0;ik<m_nconstr;++ik) { // loop kaverages
      for (int ik2=0;ik2<m_ncoords;++ik2) { // loop kaverages
	geocfile >> dval;
	if (dval!=0.) m_isgood[isec] = true;
	m_kernel[isec][ik][ik2] = dval;
      }
    } // end loop kaverages

    for (int ipar=0;ipar<m_npars;++ipar) { // pars loop
      geocfile >> key;
      geocfile >> dval;
      if (dval!=0.) m_isgood[isec] = true;
      m_fit_const[isec][ipar] = dval;
    } // end pars loop

  } // end loop over sectors

  // pre-calculate the majority logic elements
  m_maj_a = new double*[m_nsectors];
  m_maj_kk = new double**[m_nsectors];
  m_maj_invkk = new double**[m_nsectors];
  for (int isec=0;isec!=m_nsectors;++isec) { // sector loop
    m_maj_a[isec] = new double[m_ncoords];
    m_maj_kk[isec] = new double*[m_ncoords];
    m_maj_invkk[isec] = new double*[m_ncoords];
    for (int ix=0;ix!=m_ncoords;++ix) { // 1st coordinate loop    
      m_maj_a[isec][ix] = 0.;
      for (int row=0;row!=m_nconstr;++row) {
      	m_maj_a[isec][ix] += m_kaverage[isec][row]*m_kernel[isec][row][ix];
      }

      m_maj_kk[isec][ix] = new double[m_ncoords];
      m_maj_invkk[isec][ix] = new double[m_ncoords];
      for (int jx=0;jx!=m_ncoords;++jx) { // 2nd coordinate loop
        m_maj_invkk[isec][ix][jx] = m_maj_kk[isec][ix][jx] = .0;
      	for (int row=0;row!=m_nconstr;++row) {
          m_maj_kk[isec][ix][jx] += m_kernel[isec][row][ix]*m_kernel[isec][row][jx];
      	}
      } // end 2nd coordinate loop
    } // end 1st coordinate loop

#ifdef SIMPLEMJ
    // here the m_maj_kk[isec] elements are filled and the inverse element,
    // used after to simplify the guessing algorithm, can be computed
    // int npixcy = FTKSetup::getFTKSetup().getIBLMode()==1 ? 8 : 6; // this is for ibl, to know how many pixel coords there are
    int npixcy = 6; // we have 6 pixel coords
    /* PIXEL layer [0-5]*/
    for (int ix=0;ix!=npixcy;ix+=2) { // pxl layers loop (2 coordinates each)
      double det = m_maj_kk[isec][ix][ix]*m_maj_kk[isec][ix+1][ix+1]
	-m_maj_kk[isec][ix+1][ix]*m_maj_kk[isec][ix][ix+1];
      m_maj_invkk[isec][ix][ix] = m_maj_kk[isec][ix+1][ix+1]/det;
      m_maj_invkk[isec][ix][ix+1] = -m_maj_kk[isec][ix+1][ix]/det;
      m_maj_invkk[isec][ix+1][ix] = -m_maj_kk[isec][ix][ix+1]/det;
      m_maj_invkk[isec][ix+1][ix+1] = m_maj_kk[isec][ix][ix]/det;
    } // end pxl layers loop
    /* SCT layers */
    for (int ix=npixcy;ix!=m_ncoords;++ix) { // SCT layers loop (2 coordinates each)
      m_maj_invkk[isec][ix][ix] = 1./m_maj_kk[isec][ix][ix];      
    } // end SCT layers loop
#endif // SIMPLEMJ
  } // end sector loop
}
        

FTKConstantBank::~FTKConstantBank()
{
  if (m_nsectors) {
    for (int isec=0;isec<m_nsectors;++isec) {
      for (int ipar=0;ipar<m_npars;++ipar) {
      	delete [] m_fit_pars[isec][ipar];
      }
      delete [] m_fit_pars[isec];
      delete [] m_fit_const[isec];
      for (int icoord=0;icoord<m_nconstr;++icoord) {
      	delete [] m_kernel[isec][icoord];
      }
      delete [] m_kernel[isec];
      delete [] m_kaverage[isec];

      for (int ix=0;ix!=m_ncoords;++ix) {
        delete [] m_maj_kk[isec][ix];
      }
    }

    delete [] m_fit_pars;
    delete [] m_fit_const;
    delete [] m_kernel;
    delete [] m_kaverage;

    delete [] m_maj_a;
    delete [] m_maj_kk;
    delete [] m_isgood;
  }

  delete [] m_coordsmask;
  delete [] m_missid;
}

/** this function perform the fits */
int FTKConstantBank::linfit(int secid, FTKTrack &track) const
{
  int res(0);

  // initialize values used to check majority
  int guess_res(0);
  int nmissing = track.getNMissing();

  if (nmissing>0) { // use the majority
    // evaluate the missing coordinates
    guess_res = missing_point_guess(track,secid);    
  }
   
  if (nmissing!=guess_res) {
    // majority failed, this chi2 is used as a flag
    track.setChi2(0.);
  }
  else {
    // evaluate the chi2
    linfit_chisq(secid,track);
  }


  // control over chi-square value
  if (track.getChi2()==0) {
    /* in this case usual the constants for the sector are all
       0, this condition is found when there are not enough
       statistic to evaluate the constants */
    printf("*** Chi2=0, contants set problem?\n");
  }  
  else {
    /* track from with some missing plane */
    linfit_pars_eval(secid,track);
  }

  if (FTKSetup::getFTKSetup().getVerbosity()>2) {
    cout << "Coords: ";
    for (int ic=0;ic<track.getNCoords();++ic)
      cout << setw(13) << track.getCoord(ic);
    cout << endl;
    cout << "\t\t\tReconstructed parameters\n\t\t\t" \
	 << track.getInvPt() << ' ' << track.getIP() \
	 << ' ' << track.getPhi() << ' ' << track.getZ0() \
	 << ' ' << track.getCotTheta() << endl;
    cout << "\t\t\tChi2 " << track.getChi2() << endl;
  }
  
  
  return res;
}


/** This function, giving the pointer to the geometrical constants
    and the array of coordinates, return the chisq for the linear
    fit.
*/
void FTKConstantBank::linfit_chisq(int secid, FTKTrack &trk) const {
  double chi2(0);

  for(int i=0;i<m_nconstr;++i)
    {
      double s = m_kaverage[secid][i];
      for(int j=0;j<m_ncoords;++j) {
	s += m_kernel[secid][i][j]*trk.getCoords()[j];
      }
      chi2 += s*s;
    }

  trk.setChi2(chi2);
}


/* This function takes as input the pointer to the geometrical constants
   structure, the coords array and and array that will contain the
   results of the fit.

   The fits return the number of usefull parameter for the fits.
*/
void FTKConstantBank::linfit_pars_eval(int secid, FTKTrack &trk) const
{
  float *pars = new float[m_npars];

  for (int ip=0;ip<m_npars;++ip) { // parameter loop
    pars[ip] = m_fit_const[secid][ip];
    for(int j=0;j<m_ncoords;++j)  { // scalar product
      pars[ip] += m_fit_pars[secid][ip][j]*trk.getCoords()[j];
    }
  } // end parameter loop

  if (m_npars>3) {
    trk.setZ0(pars[3]);
    trk.setZ0Raw(pars[3]);
    trk.setCotTheta(pars[4]);
  }
  //cout << "secid: " << secid << endl; // cy debug
  //cout << "phi after corrgen: " << pars[2] << endl; //cy debug
  trk.setInvPt(pars[0]);
  trk.setIP(pars[1]);
  trk.setIPRaw(pars[1]);
  trk.setPhi(pars[2]); // angle is moved within -pi to +pi
  trk.setPhiRaw(pars[2]); // angle is moved within -pi to +pi
  //    cout << "phi within [-pi,pi]: " << track.getPhi() << endl; // cy debug

  //while (track.getPhi()>= TMath::Pi()) track.setPhi(phi - TMath::TwoPi()); 
  //while (track.getPhi()< -TMath::Pi()) track.setPhi(phi + TMath::TwoPi());  

  delete [] pars;
}


//int FTKConstantBank::missing_point_guess(float *coords,int *coordsmask,
//					 int secid, float *newcoords) {
int FTKConstantBank::missing_point_guess(FTKTrack &track, int secid, float *newcoords) const {

  for (int i=0;i<m_ncoords;++i) {
    m_coordsmask[i] = (track.getBitmask()>>i)&1;
  }

  // get the pointer to the track coordinates
  float *coords = track.getCoords();

  int nmissing = 0;

  // redefine a debug level
  int cmDebug = FTKSetup::getFTKSetup().getVerbosity();
  if (cmDebug > 5) {
    FTKSetup::PrintMessage(ftk::debg,"\n===In missing_point_guess===\n\n");
    FTKSetup::PrintMessageFmt(ftk::debg,"m_ncoords = %d, ", m_ncoords);
    FTKSetup::PrintMessageFmt(ftk::debg,"DIMKER_NEW = %d\n", m_nconstr);
  }

  //int i;
  // evaluate the number of missings points
  for (int i=0;i<m_ncoords;++i) {
    m_missid[i] = -1;
    if (cmDebug > 5) printf("coordsmask[%d] = %d\n",i,m_coordsmask[i]);
    //    FTKSetup::PrintMessageFmt(ftk::sevr,"coordsmask[%d] = %d\n",i,m_coordsmask[i]);
    if (!m_coordsmask[i]) {
      m_missid[nmissing] = i; // rec the index of missing
      nmissing++;
    }
  }
  if (cmDebug > 5) FTKSetup::PrintMessageFmt(ftk::debg,"nmissing = %d\n", nmissing);

  //FTKSetup::PrintMessageFmt(ftk::sevr,"nmissing = %d\n", nmissing);

#ifdef SIMPLEMJ
  //  int npixcy = FTKSetup::getFTKSetup().getIBLMode()==1 ? 8 : 6; // this is for ibl, to know how many pixel coords there are
  int npixcy = 6; // we have 6 pixel coords
#endif

  // make a control on which type of evaluation is needed
  if (nmissing==0) {
    return 0;
  }
#ifdef SIMPLEMJ
  else if (nmissing==1 && m_missid[0]>npixcy-1) { // solve an SCT missing layer
    // initialize the contraint
    double a(-m_maj_a[secid][m_missid[0]]);

    /* complete te calculation with the terms that depends
       from the known hits */
    for (int col=0;col<m_ncoords;++col) {
      if (!m_coordsmask[col]) continue;
      a -= m_maj_kk[secid][col][m_missid[0]]*coords[col];
      if (cmDebug > 8) printf("\ta = %f\n",a);
    }

    float newcoord = m_maj_invkk[secid][m_missid[0]][m_missid[0]]*a;

    if (!newcoords) {
      coords[m_missid[0]] = newcoord;
    }
    else {
      for (int i=0;i<m_ncoords;++i)
         newcoords[i] = coords[i];
      newcoords[m_missid[0]] = newcoord;
    }
    return 1;
  }
  else if (nmissing==2 && m_missid[1]<npixcy
	   && m_missid[0]+1==m_missid[1]) { // solve one missing PIXEL layer
    // initialize the contraint
    double a[2] = {-m_maj_a[secid][m_missid[0]], 
		   -m_maj_a[secid][m_missid[1]]};

    /* complete te calculation with the terms that depends
       from the known hits */
    for (int col=0;col<m_ncoords;++col) {
      if (!m_coordsmask[col]) continue;
      a[0] -= m_maj_kk[secid][col][m_missid[0]]*coords[col];
      a[1] -= m_maj_kk[secid][col][m_missid[1]]*coords[col];
      if (cmDebug > 8) {
	printf("\ta[0] = %f\n",a[0]);
	printf("\ta[1] = %f\n",a[1]);
      }
    }

    float newcoord[2];

    newcoord[0] = m_maj_invkk[secid][m_missid[0]][m_missid[0]]*a[0]+
      m_maj_invkk[secid][m_missid[0]][m_missid[1]]*a[1];
    newcoord[1] = m_maj_invkk[secid][m_missid[1]][m_missid[0]]*a[0]+
      m_maj_invkk[secid][m_missid[1]][m_missid[1]]*a[1];

    if (!newcoords) {
      coords[m_missid[0]] = newcoord[0];
      coords[m_missid[1]] = newcoord[1];
    }
    else {
      for (int i=0;i<m_ncoords;++i)
         newcoords[i] = coords[i];
      newcoords[m_missid[0]] = newcoord[0];
      newcoords[m_missid[1]] = newcoord[1];
    }

    return 2;
  }
#ifdef SIMPLEMJ_DBG
  if (false) {
    // nothing to do, dummy condition block
  }
#endif // SIMPLEMJ_DBG
#endif // SIMPLEMJ
  else { // fill the matrix and the array needed to solve the problem
    if (cmDebug > 5) FTKSetup::PrintMessage(ftk::debg,"is it really this stupid?\n");
#ifdef SIMPLEMJ_DBG
    FTKSetup::PrintMessage(ftk::warn,"Missing IDs combination not stored\n");
#endif // SIMPLEMJ_DBG
    TMatrixD coef(nmissing,nmissing);
    TVectorD a(nmissing);
    if (cmDebug > 5) FTKSetup::PrintMessage(ftk::debg,"maybe\n");

    for (int i=0;i<nmissing;++i) { // loop to fill the coefficients
      /* get the precalcualted offset for this missing coordinate */
      a[i] = -m_maj_a[secid][m_missid[i]]; 
      if (cmDebug > 5) FTKSetup::PrintMessage(ftk::debg,"maybe not\n");

      /* complete te calculation with the terms that depends
	 from the known hits */
      for (int col=0;col<m_ncoords;++col) {
	if (!m_coordsmask[col]) continue;
	a[i] -= m_maj_kk[secid][col][m_missid[i]]*coords[col];
	if (cmDebug > 8) printf("\ta[%d] = %f\n",i,a[i]);
      }

      for (int j=0;j<nmissing;++j) {
	/* the coefficients don't depend from any measured
	   quantity, depends by the the sector and the missing
	   coordinates */
	coef[i][j] = m_maj_kk[secid][m_missid[i]][m_missid[j]];

	if (cmDebug > 8) printf("\tcoef[%d][%d] = %f\n",i,j,coef[i][j]);
      }
    } // end loop to fill the coefs

    //solves the linear problem evaluating the missing coords
    //coef.Print("Coefs");
    if (coef.Determinant()==0) return -1;
    coef.Invert();
    TVectorD nomiss = coef*a;
//     coef.Print("Invert Coefs");
//     a.Print("Term");
//     nomiss.Print("New coords");

    if (!newcoords) {
      for (int i=0;i<nmissing;++i)
      	coords[m_missid[i]] = nomiss[i];
    }
    else {
      for (int i=0;i<m_ncoords;++i)
         newcoords[i] = coords[i];
      for (int i=0;i<nmissing;++i)
         newcoords[m_missid[i]] = nomiss[i];
    }
    if (cmDebug > 5) {
      FTKSetup::PrintMessage(ftk::debg,"new coordinates:\n");
      if (newcoords)
	for (int i=0;i<m_ncoords;++i)
	  printf("\t%d: %f\n",i,newcoords[i]);
      else
	for (int i=0;i<m_ncoords;++i)
	  printf("\t%d: %f\n",i,coords[i]);
      FTKSetup::PrintMessage(ftk::debg,"\n");
    }


   return nmissing;
  }

  return 0;
}


/** This method prepares the inverted constants used in the invlinfit() */
void FTKConstantBank::prepareInvConstants()
{
  m_invfit_consts = new TMatrixD[m_nsectors];

  for (int isec=0;isec!=m_nsectors;++isec) { // loop over the sectors
    m_invfit_consts[isec].ResizeTo(m_ncoords,m_ncoords);

    // first the parameters, by row
    for (int ip=0;ip!=m_npars;++ip) {
      for (int ix=0;ix!=m_ncoords;++ix) {
	m_invfit_consts[isec][ip][ix] = m_fit_pars[isec][ip][ix];
      }
    }
    // The the constraints, by row
    for (int ic=0;ic!=m_nconstr;++ic) {
      for (int ix=0;ix!=m_ncoords;++ix) {
	m_invfit_consts[isec][ic+m_npars][ix] = m_kernel[isec][ic][ix];
      }
    }

    // Invert the matrix
    m_invfit_consts[isec].Invert();
  } // end loop over the sectors
}


/** This method uses the track parameters and additional constraints to 
    use the constants to calculate the corresponding hits.
    This method is the base of the patt-from-constant generation. */
int FTKConstantBank::invlinfit(int secid, FTKTrack &track, double *constr) const
{
  // vector of the acutal parameters, it is zeroed
  TVectorD pars(m_ncoords);

  for (int ip=0;ip!=m_npars;++ip) {
    // The first elements are the track parameters. The track are shifted using the sector constants
    pars[ip] = track.getParameter(ip,true)-m_fit_const[secid][ip];
  }
  for (int ic=0;ic!=m_nconstr;++ic) {
    // The rest of the paramaters are the external cosntraints. The external constraint it is also shifted by the kAverage value
    if (!constr) {
      pars[m_npars+ic] = -m_kaverage[secid][ic];
    }
    else {
      pars[m_npars+ic] = constr[ic]-m_kaverage[secid][ic];
    }
  }

  // The raw hits are obtained multiplying the parameters to the inverted constants
  TVectorD rawhits = m_invfit_consts[secid]*pars;

  // The raw hits are assigned to the original track
  for (int ix=0;ix!=m_ncoords;++ix) {
    track.setCoord(ix,rawhits[ix]);
  }
  return 0;
}


/** This method, passing a track with some missing hits, but with
    fixed parameters, extrapolate the missing hits. The first argument 
    is the track to complete, the second is the sector where the track lies.
    The 3rd parameters is a mask. This method can extrapulate up to 5 hits,
    when the number of missing hits is less than 5 there are no checks, you
    have to choose the equation to use, this is in abstract arbitrary.*/
void FTKConstantBank::extrapolate_coords(FTKTrack &track, int secid,
					 int *idmiss, int *idpars) const
{
  const int nmissing = track.getNMissing();

  TMatrixD coef(nmissing,nmissing);
  TVectorD guessed(nmissing);
  TVectorD term(nmissing);

  // prepare the matrix and the vectors
  for (int imp=0;imp!=nmissing;++imp) {
    for (int imx=0;imx!=nmissing;++imx) {
      coef[imp][imx] = m_fit_pars[secid][idpars[imp]][idmiss[imx]];
    }

    // index to check if this coordinates is missing
    term[imp] = track.getParameter(idpars[imp])-m_fit_const[secid][idpars[imp]];
    int imiss = 0;   
    for (int ix=0;ix!=m_ncoords;++ix) {
      if (ix==idmiss[imiss]) {
	imiss += 1; // now check the next, the coordinates are sorted
	continue; // skip this
      }

      // subtract the contribution of the measured hits
      term[imp] -= m_fit_pars[secid][idpars[imp]][ix]*track.getCoord(ix);
    }
  }

  // solve the problem
  coef.Invert();
  guessed = coef*term;

  // fill the original track
  for (int im=0;im!=nmissing;++im) {
    track.setCoord(idmiss[im],guessed[im]);
  }
  
} 



//////////////////////////////////////////////////
// ADDITIONAL FUNCTIONS 
// for calculations with limited precision
// to emulate firmware design
// (Jordan)
// 
// It is assumed here that the constants are loaded as "signed long long"s from a
// bank specficially designed for this type of run. In the current scheme, these
// are signed integers scaled by a factor of 2^20 (2^12) in the case of kernel (kaverage)
// constants. 
//////////////////////////////////////////////////

signed long long FTKConstantBank::aux_asr( const signed long long &input , const int &shift , const int &width , bool &overflow ) const {

  // This is meant to model an arithmetic shift register. In calculations in the firmware, the output from each
  // multiplier needs to be scaled and then truncated down to a size small enough to be used by other multpliers.
  // This is done by shifting bits, truncating, and keeping track of overflow.
  //
  // WARNING: could run into problems using this functions with very large shifts or widths if 2 to the power
  // of the value specified is outside int range

  signed long long output( input );

  // shift bits designated number of spots
  output *= static_cast<signed long long>( pow(2,shift) );

  // keep track of this value so we can compare after truncating the number of bits
  signed long long expected_output = output;

  // now truncate to width, assuming signed 2's complement format
  signed long long range = pow( 2 , width );
  signed long long halfrange = pow( 2 , width-1 );
  output = ( (output + halfrange) % range ) - halfrange;

  if( output != expected_output ) overflow = true;
  return output;

}

void FTKConstantBank::linfit_chisq_aux(int secid, FTKTrack &trk) const {

  // Multiply integer hits by integer constants. Then scale the resulting
  // chi-components down by 2^10 before squaring and summing.

  signed long long chi2(0);
  bool overflow = false;

  for( int i = 0 ; i < m_nconstr ; ++i ) {

    signed long long chi_component = m_kaverage_aux[secid][i] * pow(2,11);

    for( int j = 0 ; j < m_ncoords ; ++j ) {
      chi_component += m_kernel_aux[secid][i][j] * trk.getAUXCoord(j);
    }

    // arithemetic shift right 10 bits, trunccate to 27-bits
    chi_component = aux_asr( chi_component , 10 , 27 , overflow );

    chi2 += (chi_component * chi_component);

  }  

  // If there was a bit overflow overflow, set the chi-square to some artificially large value so it fails the cut.
  // Otherwise, set to the caluclated value, scaled back to nominal units.
  trk.setChi2FW(chi2);
  float fchi2 = overflow ? 9999999. : static_cast<float>(chi2) / pow(2.0,26.0);
  trk.setChi2(fchi2);

}

