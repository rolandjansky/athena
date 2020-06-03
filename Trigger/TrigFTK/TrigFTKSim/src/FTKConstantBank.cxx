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
#include <Eigen/Dense>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <bitset>
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
  m_kaverage_aux(0),
  m_maj_invkk_aux(0),
  m_maj_invkk_hw(0),
  m_maj_invkk_pow(0), m_maj_invkk_pow_hw(0),
  m_dTIBL(-999999)
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
  m_maj_invkk_aux(0),
  m_maj_invkk_hw(0),
  m_maj_invkk_pow(0),
  m_maj_invkk_pow_hw(0),
  m_dTIBL(-999999)
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
  m_maj_invkk_hw = new int**[m_nsectors];
  m_maj_invkk_aux = new signed long long**[m_nsectors];
  m_maj_invkk_pow = new short int**[m_nsectors];
  m_maj_invkk_pow_hw = new int**[m_nsectors];
  for (int isec=0;isec!=m_nsectors;++isec) { // sector loop
    m_maj_invkk_hw [isec] = new int*[m_ncoords];
    m_maj_invkk_aux[isec] = new signed long long*[m_ncoords];
    m_maj_invkk_pow[isec] = new short int*[m_ncoords];
    m_maj_invkk_pow_hw[isec] = new int*[m_ncoords];
    for (int ix=0;ix!=m_ncoords;++ix) { // 1st coordinate loop    

      m_maj_invkk_hw [isec][ix] = new int[m_ncoords];
      m_maj_invkk_aux[isec][ix] = new signed long long[m_ncoords];
      m_maj_invkk_pow[isec][ix] = new short int[m_ncoords];
      m_maj_invkk_pow_hw[isec][ix] = new int[m_ncoords];
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
    temp = new std::pair<int,int>*[m_ncoords];
    for (int ix=0;ix!=m_ncoords;++ix) { // 1st coordinate loop
      temp[ix] = new std::pair<int,int>[m_ncoords];
      for (int jx=0;jx!=m_ncoords;++jx) { // 2nd coordinate loop
        temp[ix][jx].first = 0;
        temp[ix][jx].second = 0;
      }
    }
    for (int ix=0;ix!=npixcy;ix+=2) { // pxl layers loop (2 coordinates each)

      m_maj_invkk_pow[isec][ix][ix]        = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(m_maj_invkk[isec][ix][ix]    ))) - 1;
      m_maj_invkk_pow[isec][ix][ix+1]      = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(m_maj_invkk[isec][ix][ix+1]  ))) - 1;
      m_maj_invkk_pow[isec][ix+1][ix]      = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(m_maj_invkk[isec][ix+1][ix]  ))) - 1;
      m_maj_invkk_pow[isec][ix+1][ix+1]    = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(m_maj_invkk[isec][ix+1][ix+1]))) - 1;

      m_maj_invkk_aux[isec][ix][ix]     = aux_asr(m_maj_invkk[isec][ix][ix]     * pow(2, m_maj_invkk_pow[isec][ix][ix]),     0, CONST_PREC, ofl);
      m_maj_invkk_aux[isec][ix][ix+1]   = aux_asr(m_maj_invkk[isec][ix][ix+1]   * pow(2, m_maj_invkk_pow[isec][ix][ix+1]),   0, CONST_PREC, ofl);
      m_maj_invkk_aux[isec][ix+1][ix]   = aux_asr(m_maj_invkk[isec][ix+1][ix]   * pow(2, m_maj_invkk_pow[isec][ix+1][ix]),   0, CONST_PREC, ofl);
      m_maj_invkk_aux[isec][ix+1][ix+1] = aux_asr(m_maj_invkk[isec][ix+1][ix+1] * pow(2, m_maj_invkk_pow[isec][ix+1][ix+1]), 0, CONST_PREC, ofl);

      temp[ix][ix] = getFloating(-m_maj_invkk[isec][ix][ix]/(const_scale_map[ix]* const_scale_map[ix] ));
      m_maj_invkk_hw [isec][ix][ix]     = temp[ix][ix].first;
      m_maj_invkk_pow_hw[isec][ix][ix]  = temp[ix][ix].second;

      temp[ix][ix+1] = getFloating(-m_maj_invkk[isec][ix][ix+1]/(const_scale_map[ix]* const_scale_map[ix+1] ));
      m_maj_invkk_hw [isec][ix][ix+1]     = temp[ix][ix+1].first;
      m_maj_invkk_pow_hw[isec][ix][ix+1]  = temp[ix][ix+1].second;
 
      temp[ix+1][ix] = getFloating(-m_maj_invkk[isec][ix+1][ix]/(const_scale_map[ix+1]* const_scale_map[ix] ));
      m_maj_invkk_hw [isec][ix+1][ix]     = temp[ix+1][ix].first;
      m_maj_invkk_pow_hw[isec][ix+1][ix]  = temp[ix+1][ix].second;

      temp[ix+1][ix+1] = getFloating(-m_maj_invkk[isec][ix+1][ix+1]/(const_scale_map[ix+1]* const_scale_map[ix+1] ));
      m_maj_invkk_hw [isec][ix+1][ix+1]     = temp[ix+1][ix+1].first;
      m_maj_invkk_pow_hw[isec][ix+1][ix+1]  = temp[ix+1][ix+1].second;
     
    } // end pxl layers loop
    /* SCT layers */
    for (int ix=npixcy;ix!=m_ncoords;++ix) { // SCT layers loop (1 coordinate each)
      m_maj_invkk_pow[isec][ix][ix] = CONST_PREC - TMath::CeilNint(TMath::Log2(TMath::Abs(m_maj_invkk[isec][ix][ix]))) - 1;
      m_maj_invkk_aux[isec][ix][ix] = aux_asr(m_maj_invkk[isec][ix][ix] * pow(2, m_maj_invkk_pow[isec][ix][ix]), 0, CONST_PREC, ofl);

      temp[ix][ix] = getFloating(-m_maj_invkk[isec][ix][ix]);///(const_scale_map[ix]* const_scale_map[ix] ));
      m_maj_invkk_hw [isec][ix][ix]     = temp[ix][ix].first;
      m_maj_invkk_pow_hw[isec][ix][ix]  = temp[ix][ix].second;
    } // end SCT layers loop

    if (ofl) FTKSetup::PrintMessageFmt(ftk::warn, "  maj/invkk overflowed allowed precision in sector %d.\n", isec);
    
    //clean up dynamic memory
    for (int ix=0;ix!=m_ncoords;++ix) {
      for (int jx=0;jx!=m_ncoords;++jx) {
        //delete [] temp[ix][jx];
      }
      delete [] temp[ix];
    }
    delete temp;

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
    track.setChi2FW(0);
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
	// JAA: This is a bit of a kludge, but it works - if the temperature difference is more than 10,000 kelvin, we probably have other troubles
	// It is one-sided (so we do allow such ridiculous positive variations), but that is ok
	if (j == 0 && m_ncoords == 16 && (trk.getFTKHit(0).getIsBarrel() == 1) && m_dTIBL > -9999) { // SSB, IBL and row coordinate - make temperature correction
	  int etamodule = trk.getFTKHit(0).getEtaModule(); // this counts from 0, not from -10
	  double coord = trk.getCoords()[0];
	  if (etamodule >= 0 && etamodule < 20) {
	    //	    double dx = dxdtIBL*m_dTIBL*(z*z-z0IBL*z0IBL)/(z0IBL*z0IBL);
	    double dx = dxdtIBL*m_dTIBL*(zIBL_squared[etamodule]-z0IBL_squared)*z0IBL_squared_inv;	    

	    // convert from 6.25 micron units. But we seem to be missing a factor of 10 still, for now this works - unclear why
	    coord = coord - dx*0.016;
	  }
	  s += m_kernel[secid][i][j]*coord;
	}
	else 
	  s += m_kernel[secid][i][j]*trk.getCoords()[j];
      }
      chi2 += s*s;
    }

  trk.setChi2(chi2);
  trk.setChi2FW(round(chi2));
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
      // JAA: This is a bit of a kludge, but it works - if the temperature difference is more than 10,000 kelvin, we probably have other troubles
      // It is one-sided (so we do allow such ridiculous positive variations), but that is ok
      if (j == 0 && m_ncoords == 16 && (trk.getFTKHit(0).getIsBarrel() == 1) && m_dTIBL > -9999) { // SSB, IBL and row coordinate - make temperature correction
	  int etamodule = trk.getFTKHit(0).getEtaModule(); // this counts from 0, not from -10
	  double coord = trk.getCoords()[0];
	  if (etamodule >= 0 && etamodule < 20) {
	    //	    double dx = dxdtIBL*m_dTIBL*(z*z-z0IBL*z0IBL)/(z0IBL*z0IBL);
	    double dx = dxdtIBL*m_dTIBL*(zIBL_squared[etamodule]-z0IBL_squared)*z0IBL_squared_inv;	    

	    // convert from 6.25 micron units. But we seem to be missing a factor of 10 still, for now this works - unclear why
	    coord = coord - dx*0.016;
	  }
	pars[ip] += m_fit_pars[secid][ip][j]*coord;
      }      
      else 
	pars[ip] += m_fit_pars[secid][ip][j]*trk.getCoords()[j];
    }
  } // end parameter loop

  if (m_npars>3) {
    trk.setZ0(pars[3]);
    trk.setZ0FW(round(pars[3]));
    trk.setZ0Raw(pars[3]);
    trk.setCotTheta(pars[4]);
    trk.setCTheta(round(pars[4]));
  }
  //cout << "secid: " << secid << endl; // cy debug
  //cout << "phi after corrgen: " << pars[2] << endl; //cy debug
  trk.setHalfInvPt(pars[0]);
  trk.setInvPtFW(round(pars[0]*1e5)/1e5);
  trk.setIP(pars[1]);
  trk.setIPFW(round(pars[1]));
  trk.setIPRaw(pars[1]);
  trk.setPhi(pars[2]); // angle is moved within -pi to +pi
  trk.setPhiFW(round(pars[2])); // angle is moved within -3 to +3
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
        FTKSetup::PrintMessageFmt(ftk::info,"\ta[0] = %f\n",a[0]);
        FTKSetup::PrintMessageFmt(ftk::info,"\ta[1] = %f\n",a[1]);
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
      if (newcoords) {
        for (int i=0;i<m_ncoords;++i) FTKSetup::PrintMessageFmt(ftk::debg,"\t%d: %f\n",i,newcoords[i]);
      }  
      else {
        for (int i=0;i<m_ncoords;++i) FTKSetup::PrintMessageFmt(ftk::info,"\t%d: %f\n",i,coords[i]);
      }
      FTKSetup::PrintMessage(ftk::debg,"\n");
    }

   return nmissing;
  }

  return 0;
}

unsigned int FTKConstantBank::floatToReg27(float f) {

  int f_f = (*(int*)&f);
  int f_sign = (f_f >> 31) & 0x1;
  int f_exp = (f_f >> 23) & 0xFF;
  int f_frac = f_f & 0x007FFFFF;
  int r_sign;
  int r_exp;
  int r_frac;
  r_sign = f_sign;
  if((f_exp == 0x00) || (f_exp == 0xFF)) {
    // 0x00 -> 0 or subnormal
    // 0xFF -> infinity or NaN
    r_exp = 0;
    r_frac = 0;
  } else {
    r_exp = (f_exp) & 0xFF;
    r_frac = ((f_frac >> 5)) & 0x0003FFFF;
  }
  return (r_sign << 26) | (r_exp << 18) | r_frac;
}

Eigen::MatrixXd FTKConstantBank::get_A_matrix(
					      int secid,
					      ///					      std::vector<int> real_idx,
					      std::vector<int> miss_idx,
					      std::vector<double> hw_scale
					      )
{

  // instantitae A_matrix
  Eigen::MatrixXd A_matrix(11, miss_idx.size()); // 11 will never change for FTK system (this is 16 coordinates minus 5 helix parameters)

  // Defining A column from the "m_kernel"
  for (unsigned int icol = 0; icol < miss_idx.size(); ++icol)
    {
      for (unsigned int irow = 0; irow < 11; ++irow)
	{
	  // obtain the A vector in ideal coordinates
	  double A_irow_icol = m_kernel[secid][irow][miss_idx[icol]];

	  // scale them to hw_scale
	  A_irow_icol /= hw_scale.at(miss_idx[icol]);

	  // set the A matrix
	  A_matrix(irow, icol) = A_irow_icol;
	}
    }

  return A_matrix;
}
//======================
// B column vectors = B matrix
//======================
Eigen::MatrixXd FTKConstantBank::get_B_matrix(
					      int secid,
					      std::vector<int> real_idx,
					      //					      std::vector<int> miss_idx,
					      std::vector<double> hw_scale
					      )
{
  // instantiate B_matrix, very similar to A_matrix case
  Eigen::MatrixXd B_matrix(11, real_idx.size()); // 11 will never change for FTK system (this is 16 coordinates minus 5 helix parameters)

  // set the values
  for (unsigned int icol = 0; icol < real_idx.size(); ++icol)
    {
      for (unsigned int irow = 0; irow < 11; ++irow)
	{
	  double B_irow_icol = m_kernel[secid][irow][real_idx[icol]];

	  B_irow_icol /= hw_scale.at(real_idx[icol]);

	  B_matrix(irow, icol) = B_irow_icol;
	}
    }

  return B_matrix;
}
//======================
// C matrix
//======================
Eigen::MatrixXd FTKConstantBank::get_C_matrix(
					      Eigen::MatrixXd A_matrix
					      )
{
  // the number of column vectors will be the row and col dim of C_matrix
  int col_dim_A = A_matrix.cols();

  // create C_matrix
  Eigen::MatrixXd C_matrix(col_dim_A, col_dim_A);

  // set the values;
  for (int i = 0; i < col_dim_A; ++i)
    {
      for (int j = 0; j < col_dim_A; ++j)
	{
	  C_matrix(i, j) = A_matrix.col(i).transpose() * A_matrix.col(j);
	}
    }

  return C_matrix;
}




//======================
// h vector
//======================
// The h vectors
Eigen::VectorXd FTKConstantBank::get_h_vector(
					      int secid,
					      std::vector<int> real_idx
					      )
{
  // h_vector has dimension of number of real hits
  Eigen::VectorXd h_vector(real_idx.size());

  // set the values
  for (unsigned int i = 0; i < real_idx.size(); ++i)
    {
      h_vector(i) = m_kaverage[secid][i];
    }

  return h_vector;
}


//======================
// J vector
//======================
Eigen::VectorXd FTKConstantBank::get_J_vector(
					      Eigen::MatrixXd A_matrix,
					      Eigen::VectorXd h_vector
					      )
{
  // J_vector has the dimension of missed coordinates
  // (which is same as columns in A_matrix)
  Eigen::VectorXd J_vector(A_matrix.cols());

  // set the values
  for (unsigned int i = 0; i < A_matrix.cols(); ++i)
    {
      J_vector(i) = A_matrix.col(i).transpose() * h_vector;
    }

  return J_vector;
}




//======================
// D matrix
//======================
Eigen::MatrixXd FTKConstantBank::get_D_matrix(
					      Eigen::MatrixXd A_matrix,
					      Eigen::MatrixXd B_matrix
					      )
{
  // instantitaion
  Eigen::MatrixXd D_matrix(A_matrix.cols(), B_matrix.cols());

  // set the value
  for (unsigned int i = 0; i < A_matrix.cols(); ++i)
    {
      for (unsigned int j = 0; j < B_matrix.cols(); ++j)
	{
	  D_matrix(i, j) = A_matrix.col(i).transpose() * B_matrix.col(j);
	}
    }

  return D_matrix;
}


//======================
// F vector
//======================
Eigen::VectorXd FTKConstantBank::get_F_vector(
					      Eigen::MatrixXd C_matrix,
					      Eigen::VectorXd J_vector
					      )
{
  // instantiation
  Eigen::VectorXd F_vector(C_matrix.cols());

  F_vector = C_matrix.inverse() * J_vector * -1;

  return F_vector;
}
//==============================================
// E_matrix
//==============================================
Eigen::MatrixXd FTKConstantBank::get_E_matrix(
    Eigen::MatrixXd C_matrix,
    Eigen::MatrixXd D_matrix
)
{
  // instantitaion
  Eigen::MatrixXd E_matrix(C_matrix.cols(), D_matrix.cols());

  // calculate
  E_matrix = C_matrix.inverse() * D_matrix * -1.;

  return E_matrix;
}
unsigned int FTKConstantBank::createMask(unsigned int a, unsigned int b)
{
   unsigned int r = 0;
   for (unsigned int i=a; i<=b; i++)
       r |= 1 << i;

   return r;
}


void FTKConstantBank::printExtrapolationConstant(int secid, vector<int> moduleid, int eightl_secid, int nconn, int sizenconn , std::ofstream& myfile){



  int nmissing = 0;
  //int i;
  // evaluate the number of missings points
  for (int i=0;i<m_ncoords;++i) {
    m_missid[i] = -1;
    if (!m_coordsmask[i]) {
      m_missid[nmissing] = i; // rec the index of missing
      nmissing++;
    }
  
}
  std::vector<double> hw_scale;

  hw_scale.push_back(8.);  
  hw_scale.push_back(16.32); 
  hw_scale.push_back(8.);   
  hw_scale.push_back(19./18. * 16.);
  hw_scale.push_back(8.);
  hw_scale.push_back(19./18. * 16.);
  hw_scale.push_back(8.);
  hw_scale.push_back(19./18. * 16.);
  hw_scale.push_back(2.);
  hw_scale.push_back(2.);
  hw_scale.push_back(2.);
  hw_scale.push_back(2.); 
  hw_scale.push_back(2.); 
  hw_scale.push_back(2.); 
  hw_scale.push_back(2.); 
  hw_scale.push_back(2.);                                                           
                       

  std::vector<int> real_idx;
  std::vector<int> miss_idx;
  miss_idx.push_back(0);
  miss_idx.push_back(1);
  miss_idx.push_back(9);
  miss_idx.push_back(11);
  miss_idx.push_back(15);

  real_idx.push_back(2);
  real_idx.push_back(3);
  real_idx.push_back(4);
  real_idx.push_back(5);
  real_idx.push_back(6);
  real_idx.push_back(7);
  real_idx.push_back(8);
  real_idx.push_back(10);
  real_idx.push_back(12);
  real_idx.push_back(13);
  real_idx.push_back(14);

  Eigen::MatrixXd A = get_A_matrix(secid,miss_idx,hw_scale);
  //  Eigen::MatrixXd A = get_A_matrix(secid,real_idx,miss_idx,hw_scale);
  Eigen::MatrixXd B = get_B_matrix(secid,real_idx,hw_scale);
//  Eigen::MatrixXd B = get_B_matrix(secid,real_idx,miss_idx,hw_scale);
  Eigen::MatrixXd C = get_C_matrix(A);
  Eigen::MatrixXd D = get_D_matrix(A,B);
  Eigen::VectorXd h = get_h_vector(secid,real_idx);
  Eigen::VectorXd J = get_J_vector(A,h);
  Eigen::MatrixXd E = get_E_matrix(C,D);
  Eigen::VectorXd F = get_F_vector(C,J);



  vector<double> Constants;
  Constants.clear();

  Constants.push_back(F[0]);
  Constants.push_back(E(0,0));
  Constants.push_back(E(0,1));
  Constants.push_back(E(0,2));
  Constants.push_back(E(0,3));
  Constants.push_back(E(0,4));
  Constants.push_back(E(0,5));
  Constants.push_back(E(0,6));
  Constants.push_back(E(0,7));
  Constants.push_back(E(0,8));
  Constants.push_back(E(0,9));
  Constants.push_back(E(0,10));
  
  Constants.push_back(F[1]);
  Constants.push_back(E(1,0));
  Constants.push_back(E(1,1));
  Constants.push_back(E(1,2));
  Constants.push_back(E(1,3));
  Constants.push_back(E(1,4));
  Constants.push_back(E(1,5));
  Constants.push_back(E(1,6));
  Constants.push_back(E(1,7));
  Constants.push_back(E(1,8));
  Constants.push_back(E(1,9));
  Constants.push_back(E(1,10));
  
  Constants.push_back(F[2]);
  Constants.push_back(E(2,0));
  Constants.push_back(E(2,1));
  Constants.push_back(E(2,2));
  Constants.push_back(E(2,3));
  Constants.push_back(E(2,4));
  Constants.push_back(E(2,5));
  Constants.push_back(E(2,6));
  Constants.push_back(E(2,7));
  Constants.push_back(E(2,8));
  Constants.push_back(E(2,9));
  Constants.push_back(E(2,10));
  

    Constants.push_back(F[3]);
    Constants.push_back(E(3,0));
    Constants.push_back(E(3,1));
    Constants.push_back(E(3,2));
    Constants.push_back(E(3,3));
    Constants.push_back(E(3,4));
    Constants.push_back(E(3,5));
    Constants.push_back(E(3,6));
    Constants.push_back(E(3,7));
    Constants.push_back(E(3,8));
    Constants.push_back(E(3,9));
    Constants.push_back(E(3,10));


    Constants.push_back(F[4]);
    Constants.push_back(E(4,0));
    Constants.push_back(E(4,1));
    Constants.push_back(E(4,2));
    Constants.push_back(E(4,3));
    Constants.push_back(E(4,4));
    Constants.push_back(E(4,5));
    Constants.push_back(E(4,6));
    Constants.push_back(E(4,7));
    Constants.push_back(E(4,8));
    Constants.push_back(E(4,9));
    Constants.push_back(E(4,10));


    int NconnID = nconn ;
    unsigned int word = (eightl_secid << 2) | NconnID ;
    myfile << internal // fill between the prefix and the number                     
	   << setfill('0'); // fill with 0s                                                                         
   

    myfile << hex<< setw(8) << word <<endl;

    for(Int_t y=0;y<=8;y++){
      Int_t x =y+y;
      unsigned mask =  (((1 << 2) - 1) << (x));
      word = ((moduleid[0] & mask) << (28-x)) | floatToReg27(Constants[y]);
      myfile << hex<< setw(8) << word  << endl;         
    }
    word = floatToReg27(Constants[9]);
    myfile << hex << setw(8) << word  << endl;         
    for(Int_t y=10;y<=18;y++){
      Int_t x =(y+y-20);
      unsigned mask =  (((1 << 2) - 1) << x)  ;
      word = ((moduleid[1] & mask) << (28-x)) | floatToReg27(Constants[y]);
      myfile << hex<< setw(8) << word  << endl;         
    }
    word = floatToReg27(Constants[19]);
    myfile << hex << setw(8) << word  << endl;
    for(Int_t y=20;y<=28;y++){
      Int_t x =(y+y-40);
      unsigned mask =  (((1 << 2) - 1) << x);
      word = ((moduleid[2] & mask) << (28-x)) | floatToReg27(Constants[y]);
      myfile << hex<< setw(8) << word  << endl;
    }
    word = floatToReg27(Constants[29]);
    myfile << hex << setw(8) << word  << endl;
    for(Int_t y=30;y<=38;y++){
      Int_t x =(y+y-60);
      unsigned mask =  (((1 << 2) - 1) << x);
      word = ((moduleid[3] & mask) << (28-x)) | floatToReg27(Constants[y]);
      myfile << hex<< setw(8) << word  << endl;
    }
    word = floatToReg27(Constants[39]);
    myfile << hex << setw(8) << word  << endl;
    for(Int_t y=40;y<=48;y++){
      Int_t x =(y+y-80);
      unsigned mask =  (((1 << 2) - 1) << x) ;
      word = ((secid  & mask) << (28-x)) | floatToReg27(Constants[y]);
      myfile << hex<< setw(8) << word  << endl;
    }
    word = floatToReg27(Constants[49]);
    myfile << hex << setw(8) << word  << endl;
    unsigned mask =  ((1 << 2) - 1) << 0;
    word = ((sizenconn & mask)<< (28)) | floatToReg27(Constants[50]);
    myfile << hex << setw(8) << word  << endl;
    mask =  ((1 << 2) - 1) << 2;
    word = ((sizenconn & mask) << 26)  | floatToReg27(Constants[51]);
    myfile << hex << setw(8) << word  << endl;
    word = floatToReg27(Constants[52]);
    myfile << hex << setw(8) << word  << endl;
    word = floatToReg27(Constants[53]);
    myfile << hex << setw(8) << word  << endl;
    word = floatToReg27(Constants[54]);
    myfile << hex << setw(8) << word  << endl;
    word = floatToReg27(Constants[55]);
    myfile << hex << setw(8) << word  << endl;
    word = floatToReg27(Constants[56]);
    myfile << hex << setw(8) << word  << endl;
    word = floatToReg27(Constants[57]);
    myfile << hex << setw(8) << word  << endl;
    word = floatToReg27(Constants[58]);
    myfile << hex << setw(8) << word  << endl;
    word = floatToReg27(Constants[59]);
    myfile << hex << setw(8) << word  << endl;
    myfile << hex<< "00000000" << endl;
    myfile << hex<< "00000000" << endl;
    myfile << hex<< "00000000" << endl;

    return;

}
void FTKConstantBank::printTFConstant(int secid, std::ofstream& myfile){




  std::vector<double> hw_scale;
  //scaling for hw coordinates
  hw_scale.push_back(8.);  
  hw_scale.push_back(16.32); 
  hw_scale.push_back(8.);   
  hw_scale.push_back(19./18. * 16.);
  hw_scale.push_back(8.);
  hw_scale.push_back(19./18. * 16.);
  hw_scale.push_back(8.);
  hw_scale.push_back(19./18. * 16.);
  hw_scale.push_back(2.);
  hw_scale.push_back(2.);
  hw_scale.push_back(2.);
  hw_scale.push_back(2.); 
  hw_scale.push_back(2.); 
  hw_scale.push_back(2.); 
  hw_scale.push_back(2.); 
  hw_scale.push_back(2.);                                                           


  std::vector<int> real_idx;

  real_idx.push_back(2);
  real_idx.push_back(3);
  real_idx.push_back(4);
  real_idx.push_back(5);
  real_idx.push_back(6);
  real_idx.push_back(7);
  real_idx.push_back(8);
  real_idx.push_back(10);
  real_idx.push_back(12);
  real_idx.push_back(13);
  real_idx.push_back(14);

  vector<Eigen::MatrixXd> Cinverse;


  for(Int_t pp=0; pp<=11;pp++){
    std::vector<int> miss_idx1;                       
    miss_idx1.clear();
    if(pp ==0){
      miss_idx1.push_back(0);
      miss_idx1.push_back(1);
    }
    else if (pp ==1){
      miss_idx1.push_back(2);
      miss_idx1.push_back(3);
    }
    else if (pp ==2){
      miss_idx1.push_back(4);
      miss_idx1.push_back(5);
    }
    else if (pp ==3){
      miss_idx1.push_back(6);
      miss_idx1.push_back(7);
    }else{
      miss_idx1.push_back(pp+4);
    }


    Eigen::MatrixXd A = get_A_matrix(secid,miss_idx1,hw_scale);//11 (column)x16 (row) 
    //    Eigen::MatrixXd A = get_A_matrix(secid,real_idx,miss_idx1,hw_scale);//11 (column)x16 (row) 
    Eigen::MatrixXd C = get_C_matrix(A);
    Eigen::MatrixXd Cinverse_temp(C.rows(),C.cols());
    Cinverse_temp = C.inverse();
    Cinverse.push_back(Cinverse_temp);
  }
 
  std::vector<int> miss_idx;
  miss_idx.push_back(0);
  miss_idx.push_back(1);
  miss_idx.push_back(2);
  miss_idx.push_back(3);
  miss_idx.push_back(4);
  miss_idx.push_back(5);
  miss_idx.push_back(6);
  miss_idx.push_back(7);
  miss_idx.push_back(8);
  miss_idx.push_back(9);
  miss_idx.push_back(10);
  miss_idx.push_back(11);
  miss_idx.push_back(12);
  miss_idx.push_back(13);
  miss_idx.push_back(14);
  miss_idx.push_back(15);


  Int_t start = m_ncoords-1; 
  Int_t finish =0;
  int number_block_transfer =0;
  unsigned int parola = (secid << 4) | number_block_transfer ; 
  myfile << internal // fill between the prefix and the number
	 << setfill('0'); // fill with 0s



  myfile << hex<< setw(8) << parola <<endl;
  myfile << hex<< "00000000" << endl; 
  myfile << hex<< setw(8) << floatToReg27(Cinverse[0](1,0)) << endl; //missing pix0 constants
  myfile << hex<< setw(8) << floatToReg27(Cinverse[0](0,0)) << endl; //missing pix0 constants
  myfile << hex<< setw(8) << floatToReg27(m_kaverage[secid][0]) << endl; 
  for (int ik2=start;ik2>=finish;--ik2) { // loop kaverages
    myfile << std::hex<< setw(8) <<floatToReg27(m_kernel[secid][0][ik2]/hw_scale.at(ik2)) << endl;    
  }
  myfile << hex<< "00000000" << endl;
  myfile << std::hex<<  setw(8) << floatToReg27(Cinverse[0](1,1)) << endl; //missing pix0 constants
  myfile << std::hex<<  setw(8) << floatToReg27(Cinverse[0](0,1)) << endl; //missing pix0 constants
  myfile << hex<< setw(8) << floatToReg27(m_kaverage[secid][1]) << endl; 
  for (int ik2=start;ik2>=finish;ik2--) { // loop kaverages
    myfile << std::hex<<  setw(8) <<floatToReg27(m_kernel[secid][1][ik2]/hw_scale.at(ik2)) << endl;    
  }
  myfile << hex<< "00000000" << endl;
  myfile << std::hex<<setw(8) <<   floatToReg27(Cinverse[1](1,0)) << endl; //missing pix1 constants
  myfile << std::hex<<setw(8) <<   floatToReg27(Cinverse[1](0,0)) << endl; //missing pix1 constants
  myfile << hex<< setw(8) << floatToReg27(m_kaverage[secid][2]) << endl; 
  for (int ik2=start;ik2>=finish;ik2--) { // loop kaverages
    myfile << std::hex<<setw(8) <<    floatToReg27(m_kernel[secid][2][ik2]/hw_scale.at(ik2)) << endl;    
  }
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  number_block_transfer =1;
  parola = (secid << 4) | number_block_transfer ; 
  myfile << hex<< setw(8) << parola <<endl;
  myfile << hex<< "00000000" << endl;
  myfile << std::hex<<  setw(8) << floatToReg27(Cinverse[1](1,1)) << endl; //missing pix1 constants
  myfile << std::hex<<  setw(8) << floatToReg27(Cinverse[1](0,1)) << endl; //missing pix1 constants
  myfile << hex<< setw(8) << floatToReg27(m_kaverage[secid][3]) << endl; 
  for (int ik2=start;ik2>=finish;ik2--) { // loop kaverages
    myfile << std::hex<< setw(8) <<   floatToReg27(m_kernel[secid][3][ik2]/hw_scale.at(ik2)) << endl;    
  }
  myfile << std::hex<< hex<< "00000000" << endl;
  myfile << std::hex<<  setw(8) << floatToReg27(Cinverse[2](1,0)) << endl; //missing pix2 constants
  myfile << std::hex<<  setw(8) << floatToReg27(Cinverse[2](0,0)) << endl; //missing pix2 constants
  myfile << hex<< setw(8) << floatToReg27(m_kaverage[secid][4]) << endl; 
  for (int ik2=start;ik2>=finish;ik2--) { // loop kaverages
    myfile << std::hex<<  setw(8) <<  floatToReg27(m_kernel[secid][4][ik2]/hw_scale.at(ik2)) << endl;    
  }
  myfile << hex<< "00000000" << endl;
  myfile << std::hex<< setw(8) <<  floatToReg27(Cinverse[2](1,1)) << endl; //missing pix2 constants
  myfile << std::hex<< setw(8) <<  floatToReg27(Cinverse[2](0,1)) << endl; //missing pix2 consytants
  myfile << hex<< setw(8) << floatToReg27(m_kaverage[secid][5]) << endl; 
  for (int ik2=start;ik2>=finish;ik2--) { // loop kaverages
    myfile << std::hex<< setw(8) <<   floatToReg27(m_kernel[secid][5][ik2]/hw_scale.at(ik2)) << endl;    
  }
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  number_block_transfer =2;
  parola = (secid << 4) | number_block_transfer ; 
  myfile << hex<< setw(8) << parola <<endl;
  myfile << hex<< "00000000" << endl;
  myfile << std::hex<< setw(8) <<  floatToReg27(Cinverse[3](1,0)) << endl; //missing pix3 constants
  myfile << std::hex<< setw(8) <<  floatToReg27(Cinverse[3](0,0)) << endl; //missing pix3 constants
  myfile << hex<< setw(8) << floatToReg27(m_kaverage[secid][6]) << endl; 
  for (int ik2=start;ik2>=finish;ik2--) { // loop kaverages
    myfile << std::hex<< setw(8) <<   floatToReg27(m_kernel[secid][6][ik2]/hw_scale.at(ik2)) << endl;    
  }
  myfile << std::hex<< hex<< "00000000" << endl;
  myfile << std::hex<< setw(8) <<  floatToReg27(Cinverse[3](1,1)) << endl; //missing pix3 constants
  myfile << std::hex<< setw(8) <<  floatToReg27(Cinverse[3](0,1)) << endl; //missing pix3 constants
  myfile << hex<< setw(8) << floatToReg27(m_kaverage[secid][7]) << endl; 
  for (int ik2=start;ik2>=finish;ik2--) { // loop kaverages
    myfile << std::hex<< setw(8) <<   floatToReg27(m_kernel[secid][7][ik2]/hw_scale.at(ik2)) << endl;    
  }
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  myfile << std::hex<< setw(8) <<  floatToReg27(Cinverse[4](0,0)) << endl; //missing sct4 constants
  myfile << std::hex<< setw(8) << floatToReg27(m_kaverage[secid][8]) << endl; 
  for (int ik2=start;ik2>=finish;ik2--) { // loop kaverages
    myfile << std::hex<< setw(8) <<   floatToReg27(m_kernel[secid][8][ik2]/hw_scale.at(ik2)) << endl;    
  }
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  number_block_transfer =3;
  parola = (secid << 4) | number_block_transfer ; 
  myfile << hex<< setw(8) << parola <<endl;
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  myfile << std::hex<< setw(8) <<  floatToReg27(Cinverse[5](0,0)) << endl; //missing sct5 constants
  myfile << std::hex<< setw(8) << floatToReg27(m_kaverage[secid][9]) << endl; 
  for (int ik2=start;ik2>=finish;ik2--) { // loop kaverages
    myfile << std::hex<< setw(8) <<   floatToReg27(m_kernel[secid][9][ik2]/hw_scale.at(ik2)) << endl;    
  }
  myfile << std::hex<< hex<< "00000000" << endl;
  myfile << std::hex<< hex<< "00000000" << endl;
  myfile << std::hex<< setw(8) <<  floatToReg27(Cinverse[6](0,0)) << endl; //missing sct6 constants
  myfile << std::hex<< setw(8) << floatToReg27(m_kaverage[secid][10]) << endl; 
  for (int ik2=start;ik2>=finish;ik2--) { // loop kaverages
    myfile << std::hex<< setw(8) <<   floatToReg27(m_kernel[secid][10][ik2]/hw_scale.at(ik2)) << endl;    
  }
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  myfile << std::hex<<  setw(8) <<  floatToReg27(Cinverse[7](0,0)) << endl; //missing sct7 constants
  myfile << std::hex<<  setw(8) <<  floatToReg27(m_fit_const[secid][1]) << endl; //d0
  for (int ik2=start;ik2>=finish;ik2--) { 
    Double_t scaled = (m_fit_pars[secid][1][ik2] / hw_scale.at(ik2));
    myfile << std::hex<< setw(8) <<    floatToReg27(scaled) << std::endl;
  } 
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  number_block_transfer =4;
  parola = (secid << 4) | number_block_transfer ; 
  myfile << hex<< setw(8) << parola <<endl;
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  myfile << std::hex<< setw(8) <<   floatToReg27(Cinverse[8](0,0)) << endl; //missing sct8 constants
  myfile << std::hex<< setw(8) <<   floatToReg27(m_fit_const[secid][3]) << endl; //z0
  for (int ik2=start;ik2>=finish;ik2--) { 
    Double_t scaled = (m_fit_pars[secid][3][ik2] / hw_scale.at(ik2));
    myfile << std::hex<< setw(8) << floatToReg27(scaled) << std::endl;
  } 
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;

  myfile << std::hex<<  setw(8) << floatToReg27(Cinverse[9](0,0)) << endl; //missing sct9 constants 
  myfile << std::hex<<  setw(8) << floatToReg27(m_fit_const[secid][4]) << endl; //coth
  for (int ik2=start;ik2>=finish;ik2--) { 
    Double_t scaled = (m_fit_pars[secid][4][ik2] / hw_scale.at(ik2));
    myfile << std::hex<< setw(8) <<   floatToReg27(scaled) << std::endl;
  } 
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  myfile << std::hex<< setw(8) <<   floatToReg27(Cinverse[10](0,0)) << endl; //missing sct10 constants 
  myfile << std::hex<< setw(8) <<   floatToReg27(m_fit_const[secid][2]) << endl; //phi0
  for (int ik2=start;ik2>=finish;ik2--) { 
    Double_t scaled = (m_fit_pars[secid][2][ik2] / hw_scale.at(ik2));
    myfile<< std::hex<< setw(8) <<   floatToReg27(scaled) << std::endl;
  } 
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  number_block_transfer =5;
  parola = (secid << 4) | number_block_transfer ; 
  myfile << hex<< setw(8) << parola <<endl;
  myfile << hex<< "00000000" << endl;
  myfile << hex<< "00000000" << endl;
  myfile << std::hex<< setw(8) <<   floatToReg27(Cinverse[11](0,0)) << endl; //missing sct11 constants
  myfile << std::hex<< setw(8) <<   floatToReg27(m_fit_const[secid][0]) << endl; //curvature
  for (int ik2=start;ik2>=finish;ik2--) { 
    Double_t scaled = (m_fit_pars[secid][0][ik2] / hw_scale.at(ik2));
    myfile << std::hex<< setw(8) <<   floatToReg27(scaled) << std::endl;
  } 
  for (Int_t i=0; i<43;i++){
    myfile << hex<< "00000000" << endl;
  }


  return;
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
  if (shift >= 0) {
    shifted = (shifted << shift);
  } else if (shift < 0) {
    // clear bits that will "go negative"  
    shifted &= ~((static_cast<long long>(1) << -shift) - 1);
    shifted = (shifted >> -shift);
  }

  // save bits within the width (subtracting one bit for sign)
  signed long long output = shifted & ((static_cast<long long>(1) << (width-1))-1);
  if (static_cast<unsigned long long>(llabs(output)) != shifted) overflow = true;

  // reinstate the sign.
  if (input < 0) {
    output *= -1;
    if (shift <= 0){
      if (abs(input) != shifted * pow(2,-shift)) output -= 1;
    }
  }

  return output;

}

std::pair<int, int> FTKConstantBank::getFloating(double input)
{
  //floating point ranges (assuming 13 bits for significand and 5 bits for exponent)
  const int minsignificand = -4096;
  const int maxsignificand = 4095;
  const int maxexponent = 31;
 
  std::pair<int, int> output;

  //if signficand is greater than 13 bits, return max value
  if(round(input) > maxsignificand){
    output.first = maxsignificand;
    output.second = 0;
    return output;
  }
  if(round(input) < minsignificand){
    output.first = maxsignificand;
    output.second = 0;
    return output;
  }

  double significand = input;
  int exponent = 0;
  while(round(significand*2.0) <= maxsignificand &&
    round(significand*2.0) >= minsignificand &&
    exponent <= maxexponent){
    significand *= 2.0;
    exponent++;
  }

  int sig = round(significand);

  output.first = sig;
  output.second = exponent;
  return output;
}


void FTKConstantBank::linfit_chisq_aux(int secid, FTKTrack &trk) const {

  long double chi2(0);
  long long chi2LL(0);
  long long chi2HW(0);

  bool ofl = false; // overflow
  bool oflhw = false;
 
  for (int i=0;i<m_ncoords;++i) {
    m_coordsmask[i] = (trk.getBitmask()>>i)&1;
  }
  // cout << __LINE__ << "::" << __FUNCTION__ << "  sector=" << secid << endl;

  for( int i = 0 ; i < m_nconstr ; ++i ) {

    long double chi_component = m_kaverage[secid][i];
    signed long long chi_componentLL = aux_asr(m_kaverage_aux[secid][i], 10, 30, ofl); // 30 to the same level as the Sij
    signed long long chi_componentHW = aux_asr(m_kaverage_aux[secid][i], 11, 29, oflhw); //Rui's change to match fw

    for( int j = 0 ; j < m_ncoords ; ++j ) {

      int p = const_plane_map[j]; int c = const_coord_map[j]; 

      chi_component   += m_kernel[secid][i][j]     * trk.getCoord(j); 
      chi_componentLL += m_kernel_aux[secid][i][j] * trk.getAUXCoord(j);
      if (m_coordsmask[j] == 1) chi_componentHW += aux_asr(m_kernel_hw [secid][i][j] * const_scale_map1[j] * trk.getHwCoord(p, c), -1, 33, oflhw);

      //RZ: Calculate chi_partials without guessed hit first and do the shift, then add the shifted guessed hit component (different from add  then shift)
    }

    chi2 += (chi_component * chi_component);
    
    chi_componentLL = aux_asr(chi_componentLL, 0, 30, ofl);
    chi2LL += (chi_componentLL * chi_componentLL);

    chi_componentHW = aux_asr(chi_componentHW, -2, 27, oflhw); //Rui's change to match fw

    signed long long chi_componentHW_guess=0;
    if (trk.getNMissing() != 0){ //RZ: calculate chipartial component for guessed hit here
      for( int j = 0 ; j < m_ncoords ; ++j ) {
        int p = const_plane_map[j]; int c = const_coord_map[j];
        if (m_coordsmask[j] == 0) chi_componentHW_guess += aux_asr(m_kernel_hw [secid][i][j] * const_scale_map1[j] * trk.getHwCoord(p, c), 0, 33, oflhw);
      }
    }
    chi_componentHW += aux_asr(chi_componentHW_guess, -3, 27, oflhw); //Rui's change to match fw

    chi2HW += (chi_componentHW * chi_componentHW);

  }  

  chi2LL = aux_asr(chi2LL, 0, 45, ofl);
  chi2HW = aux_asr(chi2HW, 0, 57, oflhw); //Rui's change to match fw

  // If there was a bit overflow, set the chi-square to some artificially large value so it fails the cut.
  // Otherwise, set to the caluclated value, scaled back to nominal units.

  float fchi2 = oflhw ? 9999999. : chi2HW / pow(2.0, 2.*13);//EFF_SHIFT); //Rui's change to match fw
  trk.setChi2FW(round(fchi2));
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
  int nmissing = 0; int m_lmissid[2] = {-1, -1};
  for( int j = 0 ; j < m_ncoords ; ++j ) {
    if (!m_coordsmask[j]) {
      m_lmissid[nmissing] = j;
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
    if (m_lmissid[0] >= npixcy || m_lmissid[0]+1 != m_lmissid[1]) {
      // FTKSetup::PrintMessageFmt(ftk::warn, "Two guessed coordinates must be from the same pix hit: missing %d, %d.\n", m_missid[0], m_missid[1]);
      return 0;
    }
  }
  if (nmissing == 1) { // can't imagine how this would happen...
    if (m_lmissid[0] < npixcy) {
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
    m_partialsHW[i] = aux_asr(m_kaverage_aux[secid][i], 11, 30, ofl); //9

    for (int j = 0 ; j < m_ncoords ; ++j ) {
      if (m_coordsmask[j]) {

        int p = const_plane_map[j]; int c = const_coord_map[j]; 

        // m_partials[i]   += m_kernel[secid][i][j] * track.getCoord(j);
        m_partialsLL[i] += m_kernel_aux[secid][i][j] * track.getAUXCoord(j); 
        m_partialsHW[i] += aux_asr(m_kernel_hw[secid][i][j]  * const_scale_map1[j] * track.getHwCoord(p, c), -1, 33, ofl); //Rui's change to match fw
      }
    }
    m_partialsHW[i] = aux_asr(m_partialsHW[i], -2, 27, ofl); //Rui's change to match fw
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
      tLL[j] -= m_kernel_aux[secid][i][m_lmissid[j]] * m_partialsLL[i];
      tHW[j] += m_kernel_hw[secid][i][m_lmissid[j]]  * m_partialsHW[i]; 
    }

    tLL[j] = aux_asr(tLL[j], 0, 50, ofl); // 50
    tHW[j] = aux_asr(tHW[j], -16, 27, ofl); //Rui's change to match fw
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
  int cHW_signed[2] = {0, 0};
  unsigned int cHW[2] = {0, 0};

  float *coords = track.getCoords();
  if (nmissing == 1) {
    coords[m_lmissid[0]] = tLL[0] * m_maj_invkk_aux[secid][m_lmissid[0]][m_lmissid[0]]
      / pow(2., EFF_SHIFT + KERN_SHIFT + m_maj_invkk_pow[secid][m_lmissid[0]][m_lmissid[0]] - 1);
    cHW_signed[0] = aux_asr(tHW[0] * m_maj_invkk_hw[secid][m_lmissid[0]][m_lmissid[0]],-( 5 + m_maj_invkk_pow_hw[secid][m_lmissid[0]][m_lmissid[0]]), 40, ofl); //Rui's change to match fw
    cHW[0] = max(cHW_signed[0],0); //RZ: if guessed hit < 0, set it to 0

    FTKHit newhit(1);
    cHW[0] = aux_asr(cHW[0], -3, 27, ofl); //Rui's change to match fw

    newhit.setHwCoord(0, cHW[0]);
    track.setFTKHit(const_plane_map[m_lmissid[0]], newhit);

  } else if (nmissing == 2) {
    coords[m_lmissid[0]] =   tLL[0] * m_maj_invkk_aux[secid][m_lmissid[0]][m_lmissid[0]]
                           / pow(2., EFF_SHIFT + KERN_SHIFT + m_maj_invkk_pow[secid][m_lmissid[0]][m_lmissid[0]] - 1)
                         + tLL[1] * m_maj_invkk_aux[secid][m_lmissid[0]][m_lmissid[1]]
                           / pow(2., EFF_SHIFT + KERN_SHIFT + m_maj_invkk_pow[secid][m_lmissid[0]][m_lmissid[1]] - 1);
    coords[m_lmissid[1]] =   tLL[0] * m_maj_invkk_aux[secid][m_lmissid[1]][m_lmissid[0]]
                           / pow(2., EFF_SHIFT + KERN_SHIFT + m_maj_invkk_pow[secid][m_lmissid[1]][m_lmissid[0]] - 1)
                         + tLL[1] * m_maj_invkk_aux[secid][m_lmissid[1]][m_lmissid[1]]
                           / pow(2., EFF_SHIFT + KERN_SHIFT + m_maj_invkk_pow[secid][m_lmissid[1]][m_lmissid[1]] - 1);

    cHW_signed[0] =  aux_asr(tHW[0] * m_maj_invkk_hw[secid][m_lmissid[0]][m_lmissid[0]], -(5+m_maj_invkk_pow_hw[secid][m_lmissid[0]][m_lmissid[0]]), 40,ofl)+ aux_asr(tHW[1] * m_maj_invkk_hw[secid][m_lmissid[0]][m_lmissid[1]], -(5+m_maj_invkk_pow_hw[secid][m_lmissid[0]][m_lmissid[1]]), 40, ofl); //Rui's change to match fw
    cHW_signed[1] =  aux_asr(tHW[0] * m_maj_invkk_hw[secid][m_lmissid[1]][m_lmissid[0]], -(5+m_maj_invkk_pow_hw[secid][m_lmissid[1]][m_lmissid[0]]), 40, ofl)+aux_asr(tHW[1] * m_maj_invkk_hw[secid][m_lmissid[1]][m_lmissid[1]], -(5+m_maj_invkk_pow_hw[secid][m_lmissid[1]][m_lmissid[1]]), 40, ofl); //Rui's change to match fw

    cHW[0] = max(cHW_signed[0],0);
    cHW[1] = max(cHW_signed[1],0);

    FTKHit newhit(2);
    cHW[0] = aux_asr(cHW[0], -1, 27, ofl); //Rui's change to match fw
    cHW[1] = aux_asr(cHW[1], 0, 27, ofl); //Rui's change to match fw

    newhit.setHwCoord(0, cHW[0]);
    newhit.setHwCoord(1, cHW[1]);
    track.setFTKHit(const_plane_map[m_lmissid[0]], newhit);

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



