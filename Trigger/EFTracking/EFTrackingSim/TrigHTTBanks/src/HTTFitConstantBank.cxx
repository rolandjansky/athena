// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include <Eigen/StdVector>
#include "TrigHTTBanks/HTTFitConstantBank.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTConstants.h"
#include "limits.h"
#include <TMath.h>
#include <Eigen/Dense>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <bitset>
#include <ieee754.h>

#include <sstream>

HTTFitConstantBank::HTTFitConstantBank(HTTPlaneMap const * pmap, int ncoords, std::string const & fname, bool isFirstStage, int missingPlane) :
    AthMessaging ("HTTFitConstantBank"),
    m_pmap(pmap),
    m_bankID(0),
    m_nsectors(0),
    m_ncoords(ncoords),
    m_nconstr(0),
    m_npixcy(0),
    m_missingPlane(missingPlane),
    m_isFirstStage(isFirstStage),
    m_isIdealCoordFit(true)
{
  std::ifstream geocfile(fname);
  if (not geocfile.is_open()) {
    ATH_MSG_WARNING("FitConstants file: " << fname << " cannot be opened");
  }
  else {
    ATH_MSG_INFO("Reading " << fname);
    
    // Read the file header
    readHeader(geocfile);
    ATH_MSG_INFO("Settings: m_ncoords="<<m_ncoords<<" m_npars="<<m_npars);
    
    // Read the sector constants
    readSectorInfo(geocfile);
    
    // Pre-calculate the majority logic elements
    if (m_missingPlane == -1)
      calculateMajority();
    
    if (sizeof(float) * CHAR_BIT != 32)
      ATH_MSG_WARNING("Floating points on this computer are not 32 bit. This may cause a problem for the hardware agreement. Be careful!");
    
    setIdealCoordFit(true);
    
    prepareInvFitConstants();
  }
}


// Reads the file header, sets configuration and sizes the sector variables
void HTTFitConstantBank::readHeader(std::ifstream & geocfile)
{
  std::string key;
  int ival;
  int nplanes;
  
  for (int i = 0; i < 5; ++i) getline(geocfile, key); // skip 5 lines
  
  geocfile >> key;
  if (key != "NPLANES") ATH_MSG_ERROR("Invalid file format");
  geocfile >> nplanes;
  
  geocfile >> key;
  if (key!="NSECTORS") ATH_MSG_ERROR("Invalid file format");
  geocfile >> m_nsectors;
  
  geocfile >> key;
  if (key!="NDIM") ATH_MSG_ERROR("Invalid file format");
  geocfile >> ival;
  
  // Set derived configuration variables
  if (ival==1) m_npars = 3;
  else if (ival==2) m_npars = 5;
  else ATH_MSG_ERROR("Number of dimensions invalid");
  
  m_npixcy = 2*(m_ncoords - nplanes); // difference between number of coordinates and planes is number of 2d measurements
  m_nconstr = m_ncoords - m_npars;
  
  // Allocate the block of pointer per sector
  m_sector_good.resize(m_nsectors);
  m_fit_pars.resize(m_nsectors, 5, m_ncoords);
  m_fit_const.resize(m_nsectors, 5);
  m_kernel.resize(m_nsectors, m_nconstr, m_ncoords);
  m_kaverage.resize(m_nsectors, m_nconstr);
  
  m_WCs.resize(m_nsectors, m_ncoords);
  
  m_maj_a.resize(m_nsectors, m_ncoords, 0);
  m_maj_kk.resize(m_nsectors, m_ncoords, m_ncoords, 0);
  m_maj_invkk.resize(m_nsectors, m_ncoords, m_ncoords, 0);
}


// Reads the main body of the constants file
void HTTFitConstantBank::readSectorInfo(std::ifstream & geocfile)
{
    std::string key;
    int ival;
    double dval; // temp value used to conver from double to float

    for (int isec = 0; isec < m_nsectors; ++isec)
    {
        // Read sector number
        geocfile >> key >> ival;
        if (ival != isec) ATH_MSG_WARNING("Reached sector " << ival << " but expected sector " << isec);

        // Read fit parameters
	for (int ipar = 0; ipar < m_npars; ++ipar)
        {
            geocfile >> key;
            // no check on key?
            for (int icoord = 0; icoord < m_ncoords; ++icoord)
            {
                geocfile >> dval;
                if (geocfile.fail())
		  ATH_MSG_WARNING("par loop (1) key=\"" << key << "\" ipar,icoord=" << ipar << "," << icoord);

                if (dval != 0) m_sector_good[isec] = true;
                m_fit_pars(isec, ipar, icoord) = dval;
            }
        }

        // Read k averages
        geocfile >> key;
        assert(key=="kaverages");
        for (int ik=0;ik<m_nconstr;++ik)
        {
            geocfile >> dval;
            if (dval != 0) m_sector_good[isec] = true;
            m_kaverage(isec,ik) = dval;
        }

        // Read kernel
        geocfile >> key;
        assert(key=="kernel");
        for (int ik=0;ik<m_nconstr;++ik)
        {
            for (int ik2=0;ik2<m_ncoords;++ik2)
            {
                geocfile >> dval;
                if (dval != 0) m_sector_good[isec] = true;
                m_kernel(isec, ik, ik2) = dval;
            }
        }

        // Read fit const
	for (int ipar=0;ipar<m_npars;++ipar)
        {
            geocfile >> key;
            geocfile >> dval;
            if (dval != 0) m_sector_good[isec] = true;
            m_fit_const(isec, ipar) = dval;
        }

    }
}


// Majority fits are those for which we have missing hits or when we drop a hit on 8/8 to improve chi2
void HTTFitConstantBank::calculateMajority()
{

    for (int isec=0;isec!=m_nsectors;++isec)
    {
        // Fill m_maj_a and m_maj_kk
        for (int ix=0;ix!=m_ncoords;++ix)
	  {
            for (int row=0;row!=m_nconstr;++row)
	      m_maj_a(isec, ix) += m_kaverage(isec, row) * m_kernel(isec, row, ix);
            for (int jx=0;jx!=m_ncoords;++jx)
	      {
                for (int row=0;row!=m_nconstr;++row)
		  m_maj_kk(isec, ix, jx) += m_kernel(isec, row, ix) * m_kernel(isec, row, jx);
	      }
	  }

        // Fill m_maj_invkk

        // Pixel layers (2 coordinates each), assume these come first!
        for (int ix=0;ix!=m_npixcy;ix+=2)
        {
            float det = m_maj_kk(isec, ix, ix) * m_maj_kk(isec, ix+1, ix+1)
                      - m_maj_kk(isec, ix+1, ix) * m_maj_kk(isec, ix, ix+1);

            m_maj_invkk(isec, ix, ix)       = m_maj_kk(isec, ix+1, ix+1)/det;
            m_maj_invkk(isec, ix, ix+1)     = -m_maj_kk(isec, ix+1, ix)/det;
            m_maj_invkk(isec, ix+1, ix)     = -m_maj_kk(isec, ix, ix+1)/det;
            m_maj_invkk(isec, ix+1, ix+1)   = m_maj_kk(isec, ix, ix)/det;
        }

        // Strip layers (1 coordinate)
        for (int ix=m_npixcy;ix!=m_ncoords;++ix)
            m_maj_invkk(isec, ix, ix) = 1./m_maj_kk(isec, ix, ix);
    }
}


// Prepares the inverted constants used in the invlinfit()
void HTTFitConstantBank::prepareInvFitConstants()
{
    m_invfit_consts.resize(m_nsectors);

    for (int isec=0;isec!=m_nsectors;++isec)
    {
      Eigen::MatrixXf thisMatrix(m_ncoords, m_ncoords);

        // Parameters, by row
        for (int ip=0;ip!=m_npars;++ip)
            for (int ix=0;ix!=m_ncoords;++ix)
                thisMatrix(ip,ix)= (m_fit_pars(isec, ip, ix));

        // Constraints, by row
        for (int ic=0;ic!=m_nconstr;++ic)
            for (int ix=0;ix!=m_ncoords;++ix)
                thisMatrix(ic+m_npars,ix) = m_kernel(isec, ic, ix);

	// reset usable coordinate
	for (int ix = 0; ix != m_ncoords; ++ix)
	  m_WCs(isec,ix) = 1; // start with all WCs

	// now let's check if we have any "null" row or column, and if so, we will get rid of them temporarily
	// we don't have access to module/WC information at this stage should just look for non-zero values
	for (int ix=0;ix!=m_ncoords;++ix) {
	  bool foundrow = false;
	  bool foundcolumn = false;
	  for (int iy=0;iy!=m_ncoords;++iy) {
	    if (abs(thisMatrix(ix,iy)) > MTX_TOLERANCE) foundrow = true;
	    if (abs(thisMatrix(iy,ix)) > MTX_TOLERANCE) foundcolumn = true;
	  }
	  if (foundrow && foundcolumn) m_WCs(isec,ix) = 0;
	}

	// now count the number of usable entries
	size_t nDimToUse = 0; for (int ix=0;ix!=m_ncoords;++ix) if(!m_WCs(isec,ix)) ++nDimToUse;

	Eigen::MatrixXf tempMatrix(nDimToUse, nDimToUse); // this is the matrix we will invert

	size_t counteri = 0, counterj = 0;
	for (int i = 0; i < m_ncoords; i++) {
	  counterj = 0;
	  if (!m_WCs(isec,i)) {
	    for (int j = 0; j < m_ncoords; j++)  {
	      if (!m_WCs(isec,j)) {
		tempMatrix(counteri,counterj) = thisMatrix(i,j); // if we use this coordinate, copy it over
		counterj++; // iterate counter
	      }
	    }
	    counteri++;
	  }
	}

        // Invert the matrix
	Eigen::MatrixXf inverted = tempMatrix.inverse();

	// this is the full matrix that includes inverted coordinates plus the padded zeros
	Eigen::MatrixXf fullInverted(m_ncoords, m_ncoords);

	// now we put things back into the full matrix, adding back the padded zeros
	counteri = 0; counterj = 0;
	for (int i = 0; i < m_ncoords; i++) {
	  counterj = 0;
	  if (!m_WCs(isec,i)) {
	    for (int j = 0; j < m_ncoords; j++)  {
	      if (!m_WCs(isec,j)) {
		fullInverted(i,j) = inverted(counteri,counterj); // if we use this coordinate, copy it over
		counterj++; // iterate counter
	      }
	      else {
		fullInverted(i,j) = 0;
	      }
	    }
	    counteri++;
	  }
	  else {
	    for (int j = 0; j < m_ncoords; j++)  {
	      fullInverted(i,j) = 0;
	    }
	  }
	}
        m_invfit_consts[isec]= fullInverted;

    }
}



///////////////////////////////////////////////////////////////////////////////
// Main Interface Functions
// This functions takes in hit coordinates from 'track' and does the linear fit
// to calculate the track parameters, which are populated back into 'track'.
// Returns true on success.
bool HTTFitConstantBank::linfit(sector_t sector, HTTTrack & track, bool isSecondStage) const
{
    // Do majority guess if it's needed
    if (m_missingPlane == -1 && !m_isIdealCoordFit)
    {
        int nmissing = track.getNMissing();
        if (nmissing > 0)
        {
	    int guess_res = missing_point_guess(sector, track, !isSecondStage, false);
            if (nmissing != guess_res) return false; // majority failed
        }
    }

    // evaluate the chi2
    linfit_chisq(sector, track);

    // Do the fit
    linfit_pars_eval(sector, track);

    return true;
}


int HTTFitConstantBank::missing_point_guess(sector_t sector, HTTTrack &track, bool isFirstStage, bool doExtrapolation) const
{

    std::vector<int> coordsmask(m_ncoords);
    std::vector<int> missid;

    // Keep track of which hits are missing
    int nmissing = 0;
    std::vector<int> missing_planes;
    for (int j = 0; j < m_ncoords ; ++j)
    {
        coordsmask[j] = (track.getHitMap()>>j) & 1;
        if (!coordsmask[j]) {
            int plane = m_pmap->getCoordLayer(j);
            if(missing_planes.size() == 0) {
                missing_planes.push_back(plane);
            }
            else {
                for (unsigned k = 0; k < missing_planes.size(); k++) {
                    if(plane == missing_planes[k]) break;
                    if(k == missing_planes.size() - 1) missing_planes.push_back(plane);
                }
            }
            missid.push_back(j);
            nmissing++;
        }
    }

    if(!doExtrapolation){
      if(isFirstStage){
	if(missing_planes.size() > 1){
	  ATH_MSG_WARNING("missing_point_guess() Can only guess 1 layer in the first stage");
	  return 0;
	}
	if (nmissing > 2){
	  ATH_MSG_WARNING("missing_point_guess() Cannot guess more than two coordinates in the first stage");
	  return 0;
	}
      }
      else{
	if(missing_planes.size() > 2){
	  ATH_MSG_WARNING("missing_point_guess() Can only guess 2 layers in the second stage");
	  return 0;
	}
	if (nmissing > 4){
	  ATH_MSG_WARNING("missing_point_guess() Cannot guess more than four coordinates in the second stage");
	  return 0;
	} 
      }
    }

    Eigen::MatrixXd coef(nmissing,nmissing);
    Eigen::VectorXd a(nmissing);

    for (int i=0; i<nmissing; ++i)
    {
      a[i] = -m_maj_a(sector, missid[i]);
        // complete the calculation with the terms that depend on the known hits
        for (int col=0; col<m_ncoords; ++col)
        {
            if (!coordsmask[col]) continue;

            a[i] -= m_maj_kk(sector, col, missid[i])*track.getPhiCoord(m_pmap->getCoordLayer(col));

            if (m_pmap->getDim(m_pmap->getCoordLayer(col)) == 2) { // do two at a time if 2d, then skip ahead
               a[i] -= m_maj_kk(sector, col+1, missid[i])*track.getEtaCoord(m_pmap->getCoordLayer(col));
               ++col;
            }
        }

        for (int j=0; j<nmissing; ++j)
        {
	    // here for missing pieces need to know the coordinate and sector number only
            coef(i,j) = m_maj_kk(sector, missid[i], missid[j]);
        }
    }

    if (coef.determinant()==0) return -1;
    Eigen::VectorXd missing_hits = coef.inverse()*a; // our missing hits!

    for(int m=0; m<nmissing; m++){

      int missedplane = m_pmap->getCoordLayer(missid[m]);

      if(m_pmap->getDim(m_pmap->getCoordLayer(missid[m])) == 1){
      
	HTTHit newhit;
	if(!doExtrapolation)
	  newhit.setHitType(HitType::guessed);
	else
	  newhit.setHitType(HitType::extrapolated);
	newhit.setDetType(SiliconTech::strip);
	newhit.setLayer(missedplane);
	newhit.setSection(0);
	if (m_isIdealCoordFit) {
	  double target_r = (m_isFirstStage) ? htt::TARGET_R_1STAGE[missedplane] : htt::TARGET_R_2STAGE[missedplane];
	  newhit.setX(target_r*TMath::Cos(missing_hits[m]));
	  newhit.setY(target_r*TMath::Sin(missing_hits[m]));
	}
	else{
	  newhit.setPhiCoord(missing_hits[m]);
	}

	track.setHTTHit(missedplane, newhit);
      }
      else if (m_pmap->getDim(m_pmap->getCoordLayer(missid[m])) == 2){

	HTTHit newhit;
	if(!doExtrapolation)
          newhit.setHitType(HitType::guessed);
	else
          newhit.setHitType(HitType::extrapolated);
	newhit.setDetType(SiliconTech::pixel);
	newhit.setLayer(missedplane);
	newhit.setSection(0);
	if (m_isIdealCoordFit) {
	  double target_r = (m_isFirstStage) ? htt::TARGET_R_1STAGE[missedplane] : htt::TARGET_R_2STAGE[missedplane];
	  newhit.setX(target_r*TMath::Cos(missing_hits[m]));
	  newhit.setY(target_r*TMath::Sin(missing_hits[m]));
	  newhit.setZ(missing_hits[m+1]);
	}
	else {
	  newhit.setPhiCoord(missing_hits[m]);
	  newhit.setEtaCoord(missing_hits[m+1]);
	}
	m++; //skip ahead

	track.setHTTHit(missedplane, newhit);
      }
    }

    return nmissing;
}

void HTTFitConstantBank::linfit_chisq(sector_t sector, HTTTrack &trk) const
{
    float chi2 = 0;

    for (int i = 0; i < m_nconstr; i++)
    {
        float chi_component = m_kaverage(sector, i);
        for (int ix = 0; ix != m_npixcy/2; ix++) // pxl plane loop (divide by two to get number of pix planes from pix coords)
        {
	  chi_component += m_kernel(sector, i, 2*ix) * trk.getPhiCoord(m_pmap->getCoordLayer(2*ix));
	  chi_component += m_kernel(sector, i, 2*ix+1) * trk.getEtaCoord(m_pmap->getCoordLayer(2*ix));
        }

        for (int ix = m_npixcy; ix != m_ncoords; ix++) // strip coords, easier
        {
	  chi_component += m_kernel(sector, i, ix) * trk.getPhiCoord(m_pmap->getCoordLayer(ix));
        }

        chi2 += chi_component * chi_component;
    }

    trk.setChi2(chi2);
}



/* This function takes as input the pointer to the geometrical constants
   structure, the coords array and and array that will contain the
   results of the fit.

   The fits return the number of useful parameter for the fits.
 */
void HTTFitConstantBank::linfit_pars_eval(sector_t sector, HTTTrack &trk) const
{
  std::vector<float> pars(m_npars);

    for (int ip = 0; ip < m_npars; ip++)
    {
        pars[ip] = m_fit_const(sector, ip);

        for (int coord = 0; coord < m_ncoords; coord++) {
           pars[ip] += m_fit_pars(sector, ip, coord) * trk.getPhiCoord(m_pmap->getCoordLayer(coord));
           if (m_pmap->getDim(m_pmap->getCoordLayer(coord)) == 2) { // do two at a time if 2d, then skip ahead
              pars[ip] += m_fit_pars(sector, ip, coord+1) * trk.getEtaCoord(m_pmap->getCoordLayer(coord));
              ++coord;
           }
        }
    }
    
    trk.setQOverPt(pars[0]);
    trk.setD0(pars[1]);
    trk.setPhi(pars[2]); // angle is moved within -pi to +pi
    trk.setZ0(pars[3]);
    trk.setEta(pars[4]);
    if (trk.getDoDeltaGPhis()) {
      trk.setQOverPt(trk.getHoughY()/1000.0 - trk.getQOverPt()); // final  q/pT =  q/pT from HT + delta q/pT
      trk.setPhi(trk.getHoughX() - trk.getPhi());                // final phi_0 = phi_0 from HT + delta phi_0
    }
}



/** This method uses the track parameters and additional constraints to
  use the constants to calculate the corresponding hits.
  This method is the base of the patt-from-constant generation. */
void HTTFitConstantBank::invlinfit(sector_t sector, HTTTrack &track, double const *constr) const
{
    // vector of the acutal parameters, it is zeroed
    Eigen::VectorXf pars(m_ncoords);

    for (int ip = 0; ip < m_npars; ++ip)
    {
        // The first elements are the track parameters. The track are shifted using the sector constants
        pars(ip) = track.getParameter(ip) - m_fit_const(sector, ip);
        if (ip == 2) pars(ip) = remainder(pars(ip), 2.*M_PI);
    }
    for (int ic = 0; ic < m_nconstr; ++ic)
    {
        // The rest of the paramaters are the external constraints.
        // The external constraint is also shifted by the kAverage value
        if (!constr)
            pars(m_npars+ic) = -m_kaverage(sector, ic);
        else
            pars(m_npars+ic) = constr[ic] - m_kaverage(sector, ic);
    }

    // The raw hits are obtained multiplying the parameters to the inverted constants
    Eigen::VectorXf rawhits = (m_invfit_consts[sector]) * pars;

    // The hits are assigned to the original track
    for (int j = 0; j < m_ncoords ; ++j) {
      int plane = m_pmap->getCoordLayer(j);
      SiliconTech Tech = m_pmap->getDetType(plane);
      HTTHit hit;
      hit.setHitType(HitType::mapped);
      hit.setDetType(Tech);
      hit.setLayer(plane);
      if (m_WCs(sector,j))
	hit.setPhiCoord(-1); // to keep track later on, set hit position negative
      else
	hit.setPhiCoord(rawhits(j));

      if (Tech == SiliconTech::pixel) {
	if (m_WCs(sector,j))
	  hit.setEtaCoord(-1); // to keep track later on, set hit position negative
	else
	  hit.setEtaCoord(rawhits(j+1));

	++j; // skip a coordinate if doing two at once
      }
      track.setHTTHit(plane, hit);
    }
}
