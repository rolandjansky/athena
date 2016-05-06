/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTKConstantBank.h"
#include "TrigFTKSim/FTKTrack.h"
#include "TrigFTKSim/ftk_dcap.h"

// #include <TMatrixD.h>
// #include <TVectorD.h>
#include <TMath.h>

#include <iostream>
#include <iomanip>
// #include <fstream>
#include <string>
#include <cassert>
#include <cstdio>
#include <cmath>
using namespace std;

//#define SIMPLEMJ // ibl undefined simple majority to see if we can get majority for ibl
//#define SIMPLEMJ_DBG


FTKConstantBank::FTKConstantBank() :
  m_bankID(-1), m_nsectors(0), m_nplanes(0),
  m_npars(0), m_ncoords(0), m_AUX(false), 
  m_coordsmask(0), m_missid(0),
  m_nconstr(0), m_isgood(0),
  m_fit_pars(0), m_fit_const(0), m_kernel(0), m_kaverage(0),
  m_invfit_consts(0x0),
  m_maj_a(0), m_maj_kk(0), m_maj_invkk(0),
  m_kernel_aux(0), 
  m_kernel_hw(0), 
  m_kaverage_aux(0), m_maj_invkk_hw(0), m_maj_invkk_aux(0), m_maj_invkk_pow(0), m_maj_invkk_pow_hw(0)
{
  // nothing to do
}


FTKConstantBank::FTKConstantBank(int ncoords, const char *fname) :
  m_bankID(0),
  m_nplanes(0),
  m_AUX(false),
  m_invfit_consts(0x0),
  m_kernel_aux(0),
  m_kernel_hw(0),
  m_kaverage_aux(0),
  m_maj_invkk_hw(0),
  m_maj_invkk_aux(0),
  m_maj_invkk_pow(0),
  m_maj_invkk_pow_hw(0)
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

  cout<<"FTKConstantBank settings: m_ncoords="<<m_ncoords<<" m_npars="<<m_npars<<"\n";

  int isec_step = 1 + m_nsectors/10;
  for (int isec=0;isec<m_nsectors;++isec) { // loop over sectors

    double dval; // tmp value used to conver from double to float

    if ((isec%isec_step == 0) /* ||(isec<10) */)
      cout << "Load sector: " << isec << "/" << m_nsectors << endl;

    // check variable initilized to "false"
    m_isgood[isec] = false;

    geocfile >> key >> ival;
    /* if(key!="sector") {
       cerr << "expect key=\"sector\" found key=\""<<key<<"\"\n";
       } */

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
        if(geocfile.fail()) {
           FTKSetup::PrintMessageFmt
              (ftk::sevr,"par loop (1) key=\"%s\" ipar,icoord=%d,%d\n",
               key.c_str(),ipar,icoord);
        }
        //cout<<dval<<"\n";
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
      //cout<<"second pars loop key="<<key<<"\n";
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

    // here the m_maj_kk[isec] elements are filled and the inverse element,
    // used after to simplify the guessing algorithm, can be computed
    // int npixcy = FTKSetup::getFTKSetup().getIBLMode()==1 ? 8 : 6; // this is for ibl, to know how many pixel coords there are
    int npixcy = 6; // we have 6 pixel coords -- not sure what to do about this!!
    /* PIXEL layer [0-5]*/
    for (int ix=0;ix!=npixcy;ix+=2) { // pxl layers loop (2 coordinates each)
      double det =   m_maj_kk[isec][ix][ix]   * m_maj_kk[isec][ix+1][ix+1]
                   - m_maj_kk[isec][ix+1][ix] * m_maj_kk[isec][ix][ix+1];
      m_maj_invkk[isec][ix][ix] = m_maj_kk[isec][ix+1][ix+1]/det;
      m_maj_invkk[isec][ix][ix+1] = -m_maj_kk[isec][ix+1][ix]/det;
      m_maj_invkk[isec][ix+1][ix] = -m_maj_kk[isec][ix][ix+1]/det;
      m_maj_invkk[isec][ix+1][ix+1] = m_maj_kk[isec][ix][ix]/det;

    } // end pxl layers loop
    /* SCT layers */
    for (int ix=npixcy;ix!=m_ncoords;++ix) { // SCT layers loop (1 coordinate each)
      m_maj_invkk[isec][ix][ix] = 1./m_maj_kk[isec][ix][ix];      
    } // end SCT layers loop

  } // end sector loop
}

void FTKConstantBank::doAuxFW(bool do_it) {

  m_AUX = do_it;
  if (!do_it) return;
  if (m_ncoords == 16) return;

  m_kernel_aux = new signed long long**[m_nsectors];
  m_kernel_hw = new signed long long**[m_nsectors];
  m_kaverage_aux = new signed long long*[m_nsectors];

  for (int isec=0;isec<m_nsectors;++isec) { // loop over sectors
    
    m_kernel_aux[isec] = new signed long long*[m_nconstr];
    m_kernel_hw[isec] = new signed long long*[m_nconstr];
    m_kaverage_aux[isec] = new signed long long[m_nconstr];
    for (int i=0;i<m_nconstr;++i) {
      m_kernel_aux[isec][i] = new signed long long[m_ncoords];
      m_kernel_hw[isec][i] = new signed long long[m_ncoords];
    }

    bool oflAUX = false, oflHW = false; // overflow for aux shifts.
    for (int ik=0;ik<m_nconstr;++ik) { // loop kaverages
      m_kaverage_aux[isec][ik] = aux_asr(m_kaverage[isec][ik] * pow(2., KAVE_SHIFT), 0, FIT_PREC, oflAUX); 
      m_kaverage_aux[isec][ik] = aux_asr(m_kaverage[isec][ik] * pow(2., KAVE_SHIFT), 0, FIT_PREC, oflHW); 
    } // end loop kaverages
    if (oflAUX) FTKSetup::PrintMessageFmt(ftk::warn, "  AUX kaverage overflowed allowed precision in sector %d.\n", isec);
    if (oflHW)  FTKSetup::PrintMessageFmt(ftk::warn, "  HW  kaverage overflowed allowed precision in sector %d.\n", isec);

    oflAUX = oflHW = false;
    for (int ik=0;ik<m_nconstr;++ik) { // loop kaverages
      for (int ik2=0;ik2<m_ncoords;++ik2) { // loop kaverages
        m_kernel_aux[isec][ik][ik2] = aux_asr(m_kernel[isec][ik][ik2] * pow(2., KERN_SHIFT), 0, FIT_PREC, oflAUX); 
        m_kernel_hw[isec][ik][ik2]  = aux_asr(const_scale_map[ik2] * m_kernel[isec][ik][ik2] * pow(2., KERN_SHIFT_HW), 0, 50, oflHW); // FIT_PREC
      }
    } // end loop kaverages

    if (oflAUX) FTKSetup::PrintMessageFmt(ftk::warn, "  AUX kernel overflowed allowed precision in sector %d.\n", isec);
    if (oflHW)  FTKSetup::PrintMessageFmt(ftk::warn, "  HW  kernel overflowed allowed precision in sector %d.\n", isec);
  
  } // end loop over sectors



  // pre-calculate the majority logic elements
  m_maj_invkk_hw = new signed long long**[m_nsectors];
  m_maj_invkk_aux = new signed long long**[m_nsectors];
  m_maj_invkk_pow = new short int**[m_nsectors];
  m_maj_invkk_pow_hw = new short int**[m_nsectors];
  for (int isec=0;isec!=m_nsectors;++isec) { // sector loop
    m_maj_invkk_hw [isec] = new signed long long*[m_ncoords];
    m_maj_invkk_aux[isec] = new signed long long*[m_ncoords];
    m_maj_invkk_pow[isec] = new short int*[m_ncoords];
    m_maj_invkk_pow_hw[isec] = new short int*[m_ncoords];
    for (int ix=0;ix!=m_ncoords;++ix) { // 1st coordinate loop    

      m_maj_invkk_hw [isec][ix] = new signed long long[m_ncoords];
      m_maj_invkk_aux[isec][ix] = new signed long long[m_ncoords];
      m_maj_invkk_pow[isec][ix] = new short int[m_ncoords];
      m_maj_invkk_pow_hw[isec][ix] = new short int[m_ncoords];
      for (int jx=0;jx!=m_ncoords;++jx) { // 2nd coordinate loop
        m_maj_invkk_hw [isec][ix][jx] = 0;
        m_maj_invkk_aux[isec][ix][jx] = 0;
        m_maj_invkk_pow[isec][ix][jx] = 0;
        m_maj_invkk_pow_hw[isec][ix][jx] = 0;
      	for (int row=0;row!=m_nconstr;++row) {
          m_maj_kk[isec][ix][jx] += m_kernel[isec][row][ix]*m_kernel[isec][row][jx];
      	}
      } // end 2nd coordinate loop
    } // end 1st coordinate loop

    // here the m_maj_kk[isec] elements are filled and the inverse element,
    // used after to simplify the guessing algorithm, can be computed
    // int npixcy = FTKSetup::getFTKSetup().getIBLMode()==1 ? 8 : 6; // this is for ibl, to know how many pixel coords there are
    int npixcy = 6; // we have 6 pixel coords -- not sure what to do about this!!
    /* PIXEL layer [0-5]*/
    bool ofl = false;
    for (int ix=0;ix!=npixcy;ix+=2) { // pxl layers loop (2 coordinates each)

      m_maj_invkk_pow[isec][ix][ix]        = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(m_maj_invkk[isec][ix][ix]    ))) - 1;
      m_maj_invkk_pow[isec][ix][ix+1]      = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(m_maj_invkk[isec][ix][ix+1]  ))) - 1;
      m_maj_invkk_pow[isec][ix+1][ix]      = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(m_maj_invkk[isec][ix+1][ix]  ))) - 1;
      m_maj_invkk_pow[isec][ix+1][ix+1]    = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(m_maj_invkk[isec][ix+1][ix+1]))) - 1;

      m_maj_invkk_pow_hw[isec][ix][ix]     = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(8./(const_scale_map[ix]   * const_scale_map[ix]  ) * m_maj_invkk[isec][ix][ix]    ))) - 1;
      m_maj_invkk_pow_hw[isec][ix][ix+1]   = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(8./(const_scale_map[ix]   * const_scale_map[ix+1]) * m_maj_invkk[isec][ix][ix+1]  ))) - 1;
      m_maj_invkk_pow_hw[isec][ix+1][ix]   = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(8./(const_scale_map[ix+1] * const_scale_map[ix]  ) * m_maj_invkk[isec][ix+1][ix]  ))) - 1;
      m_maj_invkk_pow_hw[isec][ix+1][ix+1] = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(8./(const_scale_map[ix+1] * const_scale_map[ix+1]) * m_maj_invkk[isec][ix+1][ix+1]))) - 1;


      m_maj_invkk_aux[isec][ix][ix]     = aux_asr(m_maj_invkk[isec][ix][ix]     * pow(2, m_maj_invkk_pow[isec][ix][ix]),     0, CONST_PREC, ofl);
      m_maj_invkk_aux[isec][ix][ix+1]   = aux_asr(m_maj_invkk[isec][ix][ix+1]   * pow(2, m_maj_invkk_pow[isec][ix][ix+1]),   0, CONST_PREC, ofl);
      m_maj_invkk_aux[isec][ix+1][ix]   = aux_asr(m_maj_invkk[isec][ix+1][ix]   * pow(2, m_maj_invkk_pow[isec][ix+1][ix]),   0, CONST_PREC, ofl);
      m_maj_invkk_aux[isec][ix+1][ix+1] = aux_asr(m_maj_invkk[isec][ix+1][ix+1] * pow(2, m_maj_invkk_pow[isec][ix+1][ix+1]), 0, CONST_PREC, ofl);

      m_maj_invkk_hw [isec][ix][ix]     = aux_asr(8./(const_scale_map[ix]   * const_scale_map[ix]  ) * m_maj_invkk[isec][ix][ix]     * pow(2, m_maj_invkk_pow_hw[isec][ix][ix]),     0, CONST_PREC, ofl);
      m_maj_invkk_hw [isec][ix][ix+1]   = aux_asr(8./(const_scale_map[ix]   * const_scale_map[ix+1]) * m_maj_invkk[isec][ix][ix+1]   * pow(2, m_maj_invkk_pow_hw[isec][ix][ix+1]),   0, CONST_PREC, ofl);
      m_maj_invkk_hw [isec][ix+1][ix]   = aux_asr(8./(const_scale_map[ix+1] * const_scale_map[ix]  ) * m_maj_invkk[isec][ix+1][ix]   * pow(2, m_maj_invkk_pow_hw[isec][ix+1][ix]),   0, CONST_PREC, ofl);
      m_maj_invkk_hw [isec][ix+1][ix+1] = aux_asr(8./(const_scale_map[ix+1] * const_scale_map[ix+1]) * m_maj_invkk[isec][ix+1][ix+1] * pow(2, m_maj_invkk_pow_hw[isec][ix+1][ix+1]), 0, CONST_PREC, ofl);

    } // end pxl layers loop
    /* SCT layers */
    for (int ix=npixcy;ix!=m_ncoords;++ix) { // SCT layers loop (1 coordinate each)
      m_maj_invkk[isec][ix][ix] = 1./m_maj_kk[isec][ix][ix];      

      m_maj_invkk_pow[isec][ix][ix] = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(m_maj_invkk[isec][ix][ix]))) - 1;
      m_maj_invkk_aux[isec][ix][ix] = aux_asr(m_maj_invkk[isec][ix][ix] * pow(2, m_maj_invkk_pow[isec][ix][ix]), 0, CONST_PREC, ofl);

      m_maj_invkk_pow_hw[isec][ix][ix] = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(8./(const_scale_map[ix] * const_scale_map[ix]) * m_maj_invkk[isec][ix][ix]))) - 1;
      m_maj_invkk_hw    [isec][ix][ix] = aux_asr(8./(const_scale_map[ix] * const_scale_map[ix]) * m_maj_invkk[isec][ix][ix] * pow(2, m_maj_invkk_pow_hw[isec][ix][ix]), 0, CONST_PREC, ofl);
    } // end SCT layers loop

    if (ofl) FTKSetup::PrintMessageFmt(ftk::warn, "  maj/invkk overflowed allowed precision in sector %d.\n", isec);

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
      	if (m_kernel_aux) delete [] m_kernel_aux[isec][icoord];
      	if (m_kernel_hw) delete [] m_kernel_hw[isec][icoord];
      }
      delete [] m_kernel[isec];
      delete [] m_kaverage[isec];
      if (m_kernel_aux) delete [] m_kernel_aux[isec];
      if (m_kernel_hw) delete [] m_kernel_hw[isec];
      if (m_kaverage_aux) delete [] m_kaverage_aux[isec];

      for (int ix=0;ix!=m_ncoords;++ix) {
        if (m_maj_kk) delete [] m_maj_kk[isec][ix];
        if (m_maj_invkk_hw) delete [] m_maj_invkk_hw[isec][ix];
        if (m_maj_invkk_aux) delete [] m_maj_invkk_aux[isec][ix];
        if (m_maj_invkk_pow) delete [] m_maj_invkk_pow[isec][ix];
        if (m_maj_invkk_pow_hw) delete [] m_maj_invkk_pow_hw[isec][ix];
      }
    }

    delete [] m_fit_pars;
    delete [] m_fit_const;
    delete [] m_kernel;
    delete [] m_kaverage;
    if (m_kernel_aux) delete [] m_kernel_aux;
    if (m_kernel_hw) delete [] m_kernel_hw;
    if (m_kaverage_aux) delete [] m_kaverage_aux;

    delete [] m_maj_a;
    delete [] m_maj_kk;
    if (m_maj_invkk_hw) delete [] m_maj_invkk_hw;
    if (m_maj_invkk_aux) delete [] m_maj_invkk_aux;
    if (m_maj_invkk_pow) delete [] m_maj_invkk_pow;
    if (m_maj_invkk_pow_hw) delete [] m_maj_invkk_pow_hw;
    delete [] m_maj_invkk;
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
    // guess_res = missing_point_guess(track,secid);    
    if (m_ncoords == 11 && m_AUX) guess_res = missing_point_guess_aux(track, secid);    
    else guess_res = missing_point_guess(track,secid);    
  } 
  // else cout << " JS :: NO MISSING HITS -- NOMINAL FIT" << endl;
   

  if (nmissing!=guess_res) {
    // majority failed, this chi2 is used as a flag
    track.setChi2(0.);
  }
  else {
    // evaluate the chi2
    if (m_ncoords == 11 && m_AUX) linfit_chisq_aux(secid,track);
    else linfit_chisq(secid,track);
  }


  // control over chi-square value
  // if (track.getChi2()==0) {
  //   /* in this case usual the constants for the sector are all
  //      0, this condition is found when there are not enough
  //      statistic to evaluate the constants */
  //   printf("*** Chi2=0, contants set problem?\n");
  // }  
  // else {
  if (track.getChi2()!=0) {
    /* track from with some missing plane */
    linfit_pars_eval(secid,track);
  }

  if (FTKSetup::getFTKSetup().getVerbosity()>2) {
    cout << "Coords: ";
    for (int ic=0;ic<track.getNCoords();++ic)
      cout << setw(13) << track.getCoord(ic);
    cout << endl;
    cout << "\t\t\tReconstructed parameters\n\t\t\t" \
	 << track.getHalfInvPt() << ' ' << track.getIP() \
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
  trk.setHalfInvPt(pars[0]);
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

    Eigen::MatrixXd coef(nmissing,nmissing);
    Eigen::VectorXd a(nmissing);

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
    //	coef[i][j] = m_maj_kk[secid][m_missid[i]][m_missid[j]];
         coef(i,j) = m_maj_kk[secid][m_missid[i]][m_missid[j]];

//	if (cmDebug > 8) printf("\tcoef[%d][%d] = %f\n",i,j,coef[i][j]);
         if (cmDebug > 8) printf("\tcoef[%d][%d] = %f\n",i,j,coef(i,j));
      }
    } // end loop to fill the coefs

    //solves the linear problem evaluating the missing coords

    //coef.Print("Coefs");
//    if (coef.Determinant()==0) return -1;
//    coef.Invert();
//    TVectorD nomiss = coef*a;

    if (coef.determinant()==0) return -1;
    Eigen::VectorXd nomiss = coef.inverse()*a;
 
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
//  m_invfit_consts = new TMatrixD[m_nsectors];
   m_invfit_consts = new std::vector<Eigen::MatrixXd>;
   m_invfit_consts->resize(m_nsectors);

   for (int isec=0;isec!=m_nsectors;++isec) { // loop over the sectors
      Eigen::MatrixXd thisMatrix(m_ncoords,m_ncoords);

      // first the parameters, by row
      for (int ip=0;ip!=m_npars;++ip) {
         for (int ix=0;ix!=m_ncoords;++ix) {
            thisMatrix(ip,ix)= m_fit_pars[isec][ip][ix];
         }
      }
      // The the constraints, by row
      for (int ic=0;ic!=m_nconstr;++ic) {
         for (int ix=0;ix!=m_ncoords;++ix) {
            thisMatrix(ic+m_npars,ix) = m_kernel[isec][ic][ix];
         }
      }
      
      //Eigen::FullPivLU<Eigen::MatrixXd> lu(thisMatrix);
      //cout<<"IsInvertible: "<<lu.isInvertible()<<endl;

      // Invert the matrix
      (*m_invfit_consts)[isec]= thisMatrix.inverse();

   } // end loop over the sectors
}


/** This method uses the track parameters and additional constraints to 
    use the constants to calculate the corresponding hits.
    This method is the base of the patt-from-constant generation. */
int FTKConstantBank::invlinfit(int secid, FTKTrack &track, double *constr) const
{
  // vector of the acutal parameters, it is zeroed
//  TVectorD pars(m_ncoords);
   Eigen::VectorXd pars(m_ncoords);

  for (int ip=0;ip!=m_npars;++ip) {
    // The first elements are the track parameters. The track are shifted using the sector constants
     pars(ip) = track.getParameter(ip,true)-m_fit_const[secid][ip];
     // STS: 2016/4/18
     // fix bug with wrap-around in phi
     if(ip==2)  pars(ip)=remainder( pars(ip),2.*M_PI);
  }
  for (int ic=0;ic!=m_nconstr;++ic) {
    // The rest of the paramaters are the external cosntraints. The external constraint it is also shifted by the kAverage value
    if (!constr) {
       pars(m_npars+ic) = -m_kaverage[secid][ic];
    }
    else {
       pars(m_npars+ic) = constr[ic]-m_kaverage[secid][ic];
    }
  }

  // The raw hits are obtained multiplying the parameters to the inverted constants
  //  TVectorD rawhits = m_invfit_consts[secid]*pars;
  Eigen::VectorXd rawhits = ((*m_invfit_consts)[secid])*pars;
  
  // The raw hits are assigned to the original track
  for (int ix=0;ix!=m_ncoords;++ix) {
     track.setCoord(ix,rawhits(ix));
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

  // TMatrixD coef(nmissing,nmissing);
  // TVectorD guessed(nmissing);
  // TVectorD term(nmissing);

  Eigen::MatrixXd coef(nmissing,nmissing);
  Eigen::VectorXd guessed(nmissing);
  Eigen::VectorXd term(nmissing);


  // prepare the matrix and the vectors
  for (int imp=0;imp!=nmissing;++imp) {
    for (int imx=0;imx!=nmissing;++imx) {
       coef(imp,imx) = m_fit_pars[secid][idpars[imp]][idmiss[imx]];
    }

    // index to check if this coordinates is missing
    term(imp) = track.getParameter(idpars[imp])-m_fit_const[secid][idpars[imp]];
    int imiss = 0;   
    for (int ix=0;ix!=m_ncoords;++ix) {
      if (ix==idmiss[imiss]) {
	imiss += 1; // now check the next, the coordinates are sorted
	continue; // skip this
      }

      // subtract the contribution of the measured hits
      term(imp) -= m_fit_pars[secid][idpars[imp]][ix]*track.getCoord(ix);
    }
  }

  // solve the problem
  // coef.Invert();
  // guessed = coef*term;
  guessed = coef.inverse()*term;


  // fill the original track
  for (int im=0;im!=nmissing;++im) {
     track.setCoord(idmiss[im],guessed(im));
  }
  
} 



signed long long FTKConstantBank::aux_asr(signed long long input , int shift, int width , bool &overflow ) const {

  // deal with the sign separately
  unsigned long long shifted = abs(input);

  // make the baseline shift
  if (shift > 0) shifted = (shifted << shift);
  if (shift < 0) {
    // clear bits that will "go negative"
    shifted &= ~((static_cast<long long>(1) << -shift) - 1);
    shifted = (shifted >> -shift);
  }

  // save bits within the width (subtracting one bit for sign)
  signed long long output = shifted & ((static_cast<long long>(1) << (width-1))-1);
  if (static_cast<unsigned long long>(llabs(output)) != shifted) overflow = true;

  // reinstate the sign.
  if (input < 0) output *= -1;

  return output;

}


void FTKConstantBank::linfit_chisq_aux(int secid, FTKTrack &trk) const {

  long double chi2(0);
  long long chi2LL(0);
  long long chi2HW(0);

  bool ofl = false; // overflow

  // cout << __LINE__ << "::" << __FUNCTION__ << "  sector=" << secid << endl;

  for( int i = 0 ; i < m_nconstr ; ++i ) {

    long double chi_component = m_kaverage[secid][i];
    signed long long chi_componentLL = aux_asr(m_kaverage_aux[secid][i], 10, 30, ofl); // 30 to the same level as the Sij
    signed long long chi_componentHW = aux_asr(m_kaverage_aux[secid][i], 10, 30, ofl); 
    
    for( int j = 0 ; j < m_ncoords ; ++j ) {

      int p = const_plane_map[j]; int c = const_coord_map[j]; 

      chi_component   += m_kernel[secid][i][j]     * trk.getCoord(j); 
      chi_componentLL += m_kernel_aux[secid][i][j] * trk.getAUXCoord(j);
      chi_componentHW += m_kernel_hw [secid][i][j] * trk.getHwCoord(p, c);



      // if (i == 0 && !trk.getNMissing() && (j < 6) && (j % 2)) 
      //   cerr << "JS: " << __FUNCTION__ << "::" << __LINE__ << "   sector=" << secid
      //                        << "    plane=" << p
      //                        << "    coord=" << setfill('0') << setw(4) << trk.getHwCoord(p, c)
      //                        << "    one?=" << setprecision(4) << 8/16.88 * trk.getHwCoord(j/2, 1) / trk.getAUXCoord(j) << endl;

    }

    chi2 += (chi_component * chi_component);
    
    chi_componentLL = aux_asr(chi_componentLL, 0, 30, ofl);
    chi2LL += (chi_componentLL * chi_componentLL);

    chi_componentHW = aux_asr(chi_componentHW, 0, 30, ofl);
    chi2HW += (chi_componentHW * chi_componentHW);


  }  

  chi2LL = aux_asr(chi2LL, 0, 45, ofl);
  chi2HW = aux_asr(chi2HW, 0, 45, ofl);

  // If there was a bit overflow, set the chi-square to some artificially large value so it fails the cut.
  // Otherwise, set to the caluclated value, scaled back to nominal units.

  float fchi2 = ofl ? 9999999. : chi2HW / pow(2.0, 2.*EFF_SHIFT);
  trk.setChi2FW(fchi2);
  trk.setChi2  (fchi2);

  // any negative hits?
  bool negatives = false;
  if (trk.getNMissing()) 
    for( int j = 0 ; j < m_ncoords ; ++j )
      if (!m_coordsmask[j] && trk.getCoord(j) < 0) negatives = true;

  if (!negatives && chi2HW/pow(2., 2.*EFF_SHIFT) - chi2 > 2. && 
      abs(1 - chi2HW/(pow(2., 2.*EFF_SHIFT) * chi2)) - abs(1 - chi2LL/(pow(2., 2.*EFF_SHIFT) * chi2)) > 0.1) {

    cerr << __LINE__ <<   " JS FLAG: " 
      << "  chiR=" << 1.*chi2HW/chi2LL 
      << "  chi=" << chi2 << "  chi2LL=" << chi2LL/pow(2., 2.*EFF_SHIFT) << "  chi2HW=" << chi2HW/pow(2., 2.*EFF_SHIFT) 
      << (!trk.getNMissing() ? "  NOMINAL  " : "  MAJORITY  ")
      << "  sector=" << secid 
      << "  ofl=" << ofl;

    // keep track of which hits are missing.
    if (trk.getNMissing()) {
      cerr << "     MISSING COORDS: ";
      for( int j = 0 ; j < m_ncoords ; ++j )
        if (!m_coordsmask[j]) cerr  << "  " << j;
    }

    cerr << endl;

    ///   ios init(NULL);
    ///   init.copyfmt(cout);
    ///   for( int i = 0 ; i < m_nconstr ; ++i ) {
    ///     for( int j = 0 ; j < m_ncoords ; ++j ) {

    ///       int p = const_plane_map[j]; int c = const_coord_map[j]; 

    ///       if (abs(1. - const_scale_map[j] * m_kernel_aux[secid][i][j] / m_kernel_hw[secid][i][j]) > 0.01 || 
    ///           abs(1. - const_scale_map[j] * trk.getHwCoord(p, c)      / trk.getAUXCoord(j)) > 0.01)  {

    ///         cout << __LINE__ << "  >>>   JS FLAG:  notone    constr=" << i << ",coord=" << j << setprecision(3) 
    ///           << "  const:" << const_scale_map[j] * m_kernel_aux[secid][i][j] / m_kernel_hw[secid][i][j] << "  (" << m_kernel_hw[secid][i][j] << "/" << const_scale_map[j] * m_kernel_aux[secid][i][j] << ")"
    ///           << "  coord:" << const_scale_map[j] * trk.getHwCoord(p, c)      / trk.getAUXCoord(j)       << "  (" << trk.getHwCoord(p, c) << "/" << trk.getAUXCoord(j)/const_scale_map[j]  << ")"
    ///           << endl;
    ///       }
    ///     }
    ///   }
    ///   cout.copyfmt(init);

  }


}


int FTKConstantBank::missing_point_guess_aux(FTKTrack &track, int secid) const {

  bool ofl = false; // overflow

  for (int i=0;i<m_ncoords;++i) {
    m_coordsmask[i] = (track.getBitmask()>>i)&1;
  }

  // keep track of which hits are missing.
  int nmissing = 0; int m_missid[2] = {-1, -1};
  for( int j = 0 ; j < m_ncoords ; ++j ) {
    if (!m_coordsmask[j]) {
      m_missid[nmissing] = j;
      nmissing++;
    } 
  }

  // Reject anything other than a *single* missing pixel or SCT hit.
  int npixcy = 6; // we have 6 pixel coords
  if (nmissing > 2) {
    // FTKSetup::PrintMessage(ftk::warn, "Cannot guess more than two coordinates.\n");
    return 0;
  }
  if (nmissing == 2) { // 2 missing, must be consecutive pixel hits.
    if (m_missid[0] >= npixcy || m_missid[0]+1 != m_missid[1]) {
      // FTKSetup::PrintMessageFmt(ftk::warn, "Two guessed coordinates must be from the same pix hit: missing %d, %d.\n", m_missid[0], m_missid[1]);
      return 0;
    }
  }
  if (nmissing == 1) { // can't imagine how this would happen...
    if (m_missid[0] < npixcy) {
      // FTKSetup::PrintMessage(ftk::warn, "Single miss/drop must be in SCT - returning.\n");
      return 0;
    }
  }


  // calculate the chi partials
  // long double* m_partials = new long double [m_ncoords];
  long long*   m_partialsLL = new long long [m_ncoords];
  long long*   m_partialsHW = new long long [m_ncoords];
  for (int i = 0; i < m_nconstr; ++i) {

    // m_partials[i]   = m_kaverage[secid][i]; 
    m_partialsLL[i] = aux_asr(m_kaverage_aux[secid][i], 10, 30, ofl); // 30
    m_partialsHW[i] = aux_asr(m_kaverage_aux[secid][i], 10, 30, ofl); // 30

    for (int j = 0 ; j < m_ncoords ; ++j ) {
      if (m_coordsmask[j]) {

        int p = const_plane_map[j]; int c = const_coord_map[j]; 

        // m_partials[i]   += m_kernel[secid][i][j] * track.getCoord(j);
        m_partialsLL[i] += m_kernel_aux[secid][i][j] * track.getAUXCoord(j); 
        m_partialsHW[i] += m_kernel_hw[secid][i][j]  * track.getHwCoord(p, c); 
      }
    }
  }  

  if (ofl) {
    FTKSetup::PrintMessage(ftk::warn, "AUX-style partials calculation had an overflow!!!\n");
    delete [] m_partialsLL;
    delete [] m_partialsHW;
    return 0; 
  }

  // calculate the t-vectors
  // TVectorD t(nmissing);
  long long tLL[2] = {0, 0};
  long long tHW[2] = {0, 0};
  for (int j = 0; j < nmissing; ++j) {
    for (int i = 0; i < m_nconstr; ++i ) {
      // t[j] -= m_kernel[secid][i][m_missid[j]] * m_partials[i];
      tLL[j] -= m_kernel_aux[secid][i][m_missid[j]] * m_partialsLL[i];
      tHW[j] -= m_kernel_hw[secid][i][m_missid[j]]  * m_partialsHW[i];
    }

    tLL[j] = aux_asr(tLL[j], 0, 50, ofl); // 50
    tHW[j] = aux_asr(tHW[j], 0, 50, ofl); // 50
    // cerr << "JS: " << j << " ofl=" << ofl << "  tHW[j]/tLL[j]=" << (1./const_scale_map[m_missid[j]])*tHW[j]/tLL[j] << endl;
  }
  // delete [] m_partials;
  delete [] m_partialsLL;
  delete [] m_partialsHW;

  if (ofl) FTKSetup::PrintMessage(ftk::warn, "AUX-style t-vector calculation had an overflow!!!\n");

  // preserved only for testing purposes --
  // delete once all validation is complete.
  // TMatrixD coef(nmissing, nmissing); 
  // for (int ix = 0; ix < nmissing; ++ix) // 1st coordinate loop    
  //   for (int jx=0; jx < nmissing; ++jx) // 2nd coordinate loop
  //     for (int row=0;row!=m_nconstr;++row) 
  //       coef[ix][jx] += m_kernel[secid][row][m_missid[ix]] * m_kernel[secid][row][m_missid[jx]];
  // coef.Invert();
  // TVectorD newcoord = coef * t;
  // fstream outfs;
  // outfs.open("test.txt", fstream::out | fstream::app);


  // get the pointer to the track coordinates
  unsigned int cHW[2] = {0, 0};
  float *coords = track.getCoords();
  if (nmissing == 1) {

    coords[m_missid[0]] = tLL[0] * m_maj_invkk_aux[secid][m_missid[0]][m_missid[0]]
                          / pow(2., EFF_SHIFT + KERN_SHIFT + m_maj_invkk_pow[secid][m_missid[0]][m_missid[0]] - 1);

    cHW[0] = tHW[0] * m_maj_invkk_hw[secid][m_missid[0]][m_missid[0]]
             / pow(2., EFF_SHIFT_HW + KERN_SHIFT_HW + m_maj_invkk_pow_hw[secid][m_missid[0]][m_missid[0]] - 1);

    FTKHit newhit(1);
    newhit.setHwCoord(0, cHW[0]);
    track.setFTKHit(const_plane_map[m_missid[0]], newhit);


    ///  cout << __LINE__ << "  JS: pow  aux=" << m_maj_invkk_pow[secid][m_missid[0]][m_missid[0]] << "   hw=" << m_maj_invkk_pow_hw[secid][m_missid[0]][m_missid[0]] << endl;
    ///  cout << __LINE__ << "  JS: aux=" << tLL[0] * m_maj_invkk_aux[secid][m_missid[0]][m_missid[0]] 
    ///                                      / pow(2., KERN_SHIFT + m_maj_invkk_pow[secid][m_missid[0]][m_missid[0]] - 1) 
    ///                   << "      hw="  << tHW[0] * m_maj_invkk_hw[secid][m_missid[0]][m_missid[0]] 
    ///                                      / pow(2., KERN_SHIFT_HW + m_maj_invkk_pow_hw[secid][m_missid[0]][m_missid[0]] - 1) << endl;

    ///  cout << __LINE__ << "  JS:  " << m_missid[0] << "    C=" << track.getCoord(m_missid[0]) 
    ///                                               << "    A=" << 1.*track.getAUXCoord(m_missid[0]) / const_scale_map[m_missid[0]]
    ///                                               << "    H=" << track.getHwCoord(const_plane_map[m_missid[0]], 0) << endl;
    ///  cout << __LINE__ << "  JS:  " << m_missid[0] << "  A/C=" << 0.125 * track.getAUXCoord(const_plane_map[m_missid[0]])   / track.getCoord(m_missid[0]) 
    ///                                               << "  H/C=" << 1. * track.getHwCoord(const_plane_map[m_missid[0]], 0) / track.getCoord(m_missid[0])
    ///                                               << "  H/A=" << 1. * track.getHwCoord(const_plane_map[m_missid[0]], 0) * const_scale_map[m_missid[0]] / track.getAUXCoord(m_missid[0]) 
    ///                                               << "  set/ret=" << 1. * cHW[0] / track.getHwCoord(const_plane_map[m_missid[0]], 0) << endl;



  } else if (nmissing == 2) {

    coords[m_missid[0]] =   tLL[0] * m_maj_invkk_aux[secid][m_missid[0]][m_missid[0]]
                            / pow(2., EFF_SHIFT + KERN_SHIFT + m_maj_invkk_pow[secid][m_missid[0]][m_missid[0]])
                          + tLL[1] * m_maj_invkk_aux[secid][m_missid[0]][m_missid[1]]
                            / pow(2., EFF_SHIFT + KERN_SHIFT + m_maj_invkk_pow[secid][m_missid[0]][m_missid[1]]);

    coords[m_missid[1]] =   tLL[0] * m_maj_invkk_aux[secid][m_missid[1]][m_missid[0]]
                            / pow(2., EFF_SHIFT + KERN_SHIFT + m_maj_invkk_pow[secid][m_missid[1]][m_missid[0]])
                          + tLL[1] * m_maj_invkk_aux[secid][m_missid[1]][m_missid[1]]
                            / pow(2., EFF_SHIFT + KERN_SHIFT + m_maj_invkk_pow[secid][m_missid[1]][m_missid[1]]);


    cHW[0] =   tHW[0] * m_maj_invkk_hw[secid][m_missid[0]][m_missid[0]]
                / pow(2., EFF_SHIFT_HW + KERN_SHIFT_HW + m_maj_invkk_pow_hw[secid][m_missid[0]][m_missid[0]])
             + tHW[1] * m_maj_invkk_hw[secid][m_missid[0]][m_missid[1]]
                / pow(2., EFF_SHIFT_HW + KERN_SHIFT_HW + m_maj_invkk_pow_hw[secid][m_missid[0]][m_missid[1]]);

    cHW[1] =   tHW[0] * m_maj_invkk_hw[secid][m_missid[1]][m_missid[0]]
                / pow(2., EFF_SHIFT_HW + KERN_SHIFT_HW + m_maj_invkk_pow_hw[secid][m_missid[1]][m_missid[0]])
             + tHW[1] * m_maj_invkk_hw[secid][m_missid[1]][m_missid[1]]
                / pow(2., EFF_SHIFT_HW + KERN_SHIFT_HW + m_maj_invkk_pow_hw[secid][m_missid[1]][m_missid[1]]);

    FTKHit newhit(2);
    newhit.setHwCoord(0, cHW[0]);
    newhit.setHwCoord(1, cHW[1]);
    track.setFTKHit(const_plane_map[m_missid[0]], newhit);


    ////  cout << __LINE__ << "  JS: pow  aux=" << m_maj_invkk_pow[secid][m_missid[0]][m_missid[0]] << "   hw=" << m_maj_invkk_pow_hw[secid][m_missid[0]][m_missid[0]] << endl;
    ////  cout << __LINE__ << "  JS: aux=" << tLL[0] * m_maj_invkk_aux[secid][m_missid[0]][m_missid[0]] 
    ////                                      / pow(2., KERN_SHIFT + m_maj_invkk_pow[secid][m_missid[0]][m_missid[0]] - 1) 
    ////                   << "      hw="  << tHW[0] * m_maj_invkk_hw[secid][m_missid[0]][m_missid[0]] 
    ////                                      / pow(2., KERN_SHIFT_HW + m_maj_invkk_pow_hw[secid][m_missid[0]][m_missid[0]] - 1) << endl;


    ////  cout << __LINE__ << "  JS:  " << m_missid[0] << "   C=" << track.getCoord(m_missid[0]) 
    ////                                               << "   A=" << track.getAUXCoord(m_missid[0]) 
    ////                                               << "   H=" << track.getHwCoord(const_plane_map[m_missid[0]], 0) << endl;
    ////  cout << __LINE__ << "  JS:  " << m_missid[0] << " A/C=" << 1. * track.getAUXCoord(const_plane_map[m_missid[0]])   / track.getCoord(m_missid[0]) 
    ////                                               << " H/C=" << 1. * track.getHwCoord(const_plane_map[m_missid[0]], 0) / track.getCoord(m_missid[0])
    ////                                               << " H/A=" << 1. * track.getHwCoord(const_plane_map[m_missid[0]], 0) * const_scale_map[m_missid[0]] / track.getAUXCoord(m_missid[0]) 
    ////                                               << "  set/ret=" << 1. * cHW[0] / track.getHwCoord(const_plane_map[m_missid[0]], 0) << endl;


    ////  cout << __LINE__ << "  JS:  " << m_missid[1] << "   C=" << track.getCoord(m_missid[1]) 
    ////                                               << "   A=" << track.getAUXCoord(m_missid[1]) 
    ////                                               << "   H=" << track.getHwCoord(const_plane_map[m_missid[1]], 1) << endl;
    ////  cout << __LINE__ << "  JS:  " << m_missid[1] << " A/C=" << 1. * track.getAUXCoord(const_plane_map[m_missid[1]])   / track.getCoord(m_missid[1]) 
    ////                                               << " H/C=" << 1. * track.getHwCoord(const_plane_map[m_missid[1]], 1) / track.getCoord(m_missid[1]) 
    ////                                               << " H/A=" << 1. * track.getHwCoord(const_plane_map[m_missid[1]], 1) * const_scale_map[m_missid[1]] / track.getAUXCoord(m_missid[1])
    ////                                               << "  set/ret=" << 1. * cHW[1] / track.getHwCoord(const_plane_map[m_missid[1]], 1) << endl;

  } 

  // outfs.close();
  return nmissing;
    
}



