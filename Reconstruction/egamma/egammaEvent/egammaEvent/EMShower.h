/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EMSHOWER_H
#define EGAMMAEVENT_EMSHOWER_H
/**
  @class  EMShower
          EM shower property class data class.
          made by EMShowerBuilder, which is an algorithm 
          This class contains information about quantities useful for
	  e/g identification such as shower shape variables and leakage 
	  of showers into the hadronic calorimeters
   @author H. Ma, S. Rajagopalan
*/

#include "egammaEvent/egDetail.h"
#include "egammaEvent/egammaParamDefs.h"
#include <vector>
//#include <pair>

class EMShower : public egDetail

{
 public:

  /** @brief Default constructor*/
  EMShower();
  /** @brief Destructor*/
  virtual ~EMShower();
  /** @brief */
  virtual const std::string& className() const;
  /** @brief */
  virtual double parameter(egammaParameters::ParamDef) const ;
  /** @brief */
  virtual bool hasParameter(egammaParameters::ParamDef) const ;

  /** @brief */
  void set_parameter(egammaParameters::ParamDef, double, bool overwrite=false) ;


  // return values of shower parameters
  /** @brief uncalibrated energy (sum of cells) in presampler in a 1x1 window in cells in eta X phi*/
  double  e011() const;     
  /** @brief uncalibrated energy (sum of cells) in presampler in a 3x3 window in cells in eta X phi*/
  double  e033() const;     
  /** @brief uncalibrated energy (sum of cells) in strips in a 3x2 window in cells in eta X phi*/
  double  e132() const;     
  /** @brief uncalibrated energy (sum of cells) in strips in a 15x2 window in cells in eta X phi*/
  double  e1152() const;    
  /** @brief transverse energy in the first sampling of the hadronic 
  calorimeters behind the cluster calculated from ehad1*/
  double  ethad1() const;   
  /** @brief ET leakage into hadronic calorimeter with exclusion of energy in CaloSampling::TileGap3*/
  double  ethad() const;    
  /** @brief E leakage into 1st sampling of had calo (CaloSampling::HEC0 + CaloSampling::TileBar0 + CaloSampling::TileExt0)*/
  double  ehad1() const;    
  /** @brief E1/E = fraction of energy reconstructed in the first sampling, where E1 is energy in all strips belonging to the cluster and E is the total energy reconstructed in the electromagnetic calorimeter cluster*/
  double  f1() const;       
  /** @brief fraction of energy reconstructed in 3rd sampling*/
  double  f3() const;       
  /** @brief E1(3x1)/E = fraction of the energy reconstructed in the first longitudinal compartment of the electromagnetic calorimeter, where E1(3x1) the energy reconstructed in +/-3 strips in eta, centered around the maximum energy strip and E is the energy reconstructed in the electromagnetic calorimeter*/
  double  f1core() const;   
  /** @brief E3(3x3)/E fraction of the energy reconstructed in the third compartment of the electromagnetic calorimeter, where E3(3x3), energy in the back sampling, is the sum of the energy contained in a 3x3 window around the maximum energy cell*/
  double  f3core() const;   
  /** @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 3x3 (in cell units eta X phi)*/
  double  e233() const;     
  /** @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 3x5*/
  double  e235() const;     
  /** @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 5x5*/
  double  e255() const;     
  /** @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 3x7*/
  double  e237() const;     
  /** @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 7x7*/
  double  e277() const;     
  /** @brief uncalibrated energy (sum of cells) of the third sampling in a rectangle of size 3x3*/
  double  e333() const;     
  /** @brief uncalibrated energy (sum of cells) of the third sampling in a rectangle of size 3x5*/
  double  e335() const;     
  /** @brief uncalibrated energy (sum of cells) of the third sampling in a rectangle of size 3x7*/
  double  e337() const;     
  /** @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 7x7*/
  double  e377() const;     
  /** @brief shower width using +/-3 strips around the one with the maximal energy deposit: 
  w3 strips = sqrt{sum(Ei)x(i-imax)^2/sum(Ei)}, where i is the number of the strip and imax the strip number of the most energetic one */
  double  weta1() const;    
  /** @brief the lateral width is calculated with a window of 3x5 cells using the energy weighted  sum over all cells, which depends on the particle impact point inside the cell: weta2 = 
  sqrt(sum Ei x eta^2)/(sum Ei) -((sum Ei x eta)/(sum Ei))^2, where Ei is the energy of the i-th cell */
  double  weta2() const;    
  /** @brief 2nd max in strips calc by summing 3 strips*/
  double  e2ts1() const;    
  /** @brief energy of the cell corresponding to second energy maximum in the first sampling*/
  double  e2tsts1() const;  
  /** @brief shower shape in the shower core : [E(+/-3)-E(+/-1)]/E(+/-1), where E(+/-n) is the energy in +- n strips around the strip with highest energy*/
  double  fracs1() const;   
  /** @brief same as egammaParameters::weta1 but without corrections 
  on particle impact point inside the cell*/
  double  widths1() const;  
  /** @brief same as egammaParameters::weta2 but without corrections on particle impact point inside the cell*/
  double  widths2() const;  
  /** @brief relative position in eta within cell in 1st sampling */
  double  poscs1() const;   
  /** @brief relative position in eta within cell in 2nd sampling*/
  double  poscs2() const;   
  /** @brief uncorr asymmetry in 3 strips in the 1st sampling */
  double  asy1() const;     
  /** @brief difference between shower cell and predicted 
      track in +/- 1 cells*/
  double  pos() const; 
  /** @brief Difference between the track and the shower positions: 
  sum_{i=i_m-7}^{i=i_m+7}E_i x (i-i_m) / sum_{i=i_m-7}^{i=i_m+7}E_i, 
  The difference between the track and the shower positions measured 
  in units of distance between the strips, where i_m is the impact cell 
  for the track reconstructed in the inner detector and E_i is the energy 
  reconstructed in the i-th cell in the eta direction for constant phi given by the track parameters*/
  double  pos7() const; 
  /** @brief  barycentre in sampling 1 calculated in 3 strips*/
  double  barys1() const;   
  /** @brief shower width is determined in a window detaxdphi = 0,0625 x~0,2, corresponding typically to 20 strips in eta : wtot1=sqrt{sum Ei x ( i-imax)^2 / sum Ei}, where i is the strip number and imax the strip number of the first local maximum*/
  double  wtots1() const;   
  /** @brief energy reconstructed in the strip with the minimal value between the first and second maximum*/
  double  emins1() const;   
  /** @brief energy of strip with maximal energy deposit*/
  double  emaxs1() const;   
  /** @brief ET in a cone of R=0.45 in calo, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3*/
  double  etcone() const;   
  /** @brief ET in a cone with half-opening angle 0.15, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3*/
  double  etcone15() const; 
  /** @brief ET in a cone with half-opening angle 0.2, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3*/
  double  etcone20() const; 
  /** @brief ET in a cone with half-opening angle 0.25, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3*/
  double  etcone25() const; 
  /** @brief ET in a cone with half-opening angle 0.3, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3 */
  double  etcone30() const; 
  /** @brief ET in a cone with half-opening angle 0.35, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3*/
  double  etcone35() const; 
  /** @brief ET in a cone with half-opening angle 0.4, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3*/
  double  etcone40() const; 
  
  /** @brief ET in a ring of dR<20, cell abs(ET) above 2 sigma from noise*/
  double etconoisedR02SigAbs2() const;
  /** @brief ET in a ring of dR<20 cell abs (ET) above 3 sigma from noise*/
  double etconoisedR02SigAbs3() const;
  
  /** @brief ET in a ring of dR<30, cell abs(ET) above 2 sigma from noise*/
  double etconoisedR03SigAbs2() const;
  /** @brief ET in a ring of dR<30 cell abs (ET) above 3 sigma from noise*/
  double etconoisedR03SigAbs3() const;
  
  /** @brief ET in a ring of dR<40, cell abs(ET) above 2 sigma from noise*/
  double etconoisedR04SigAbs2() const;
  /** @brief ET in a ring of dR<40 cell abs (ET) above 3 sigma from noise*/
  double etconoisedR04SigAbs3() const;

    /** @brief ET in a ring of DR<0.40 above 2 sigma of noise*/
  double etconoisedR04Sig2() const;
  /** @brief ET in a ring of DR<0.40 above 3 sigma of noise*/
  double etconoisedR04Sig3() const;

  /** @brief  1-ratio of energy in 3x3 over 3x7 cells; 
      E(3x3) = E0(1x1) + E1(3x1) + E2(3x3) + E3(3x3); E(3x7) = E0(3x3) + E1(15x3) + E2(3x7) + E3(3x7)*/
  double r33over37allcalo() const;     
  /** @brief core energy in em calo 
   E(core) = E0(3x3) + E1(15x2) + E2(5x5) + E3(3x5)*/
  double ecore() const;
  /** @brief pointing z at vertex reconstructed from the cluster*/
  double zvertex() const;
  /** @brief error associated to zvertex */
  double errz() const;
  /** @brief pointing eta reconstructed from the cluster (first and second sampling)*/
  double etap() const; 
  /** @brief pointing depth of the shower as calculated in egammaqgcld*/
  double depth() const;

  /** @brief all energy in a ring : (0.1<DR<0.2) 
      with abs(energy) > 2 sigma above noise */
  double etringnoisedR02SigAbs2() const;
  /** @brief all energy in a ring : (0.1<DR<0.2) 
      with abs(energy) > 3 sigma above noise */
  double etringnoisedR02SigAbs3() const;
  
  /** @brief all energy in a ring : (0.1<DR<0.3) 
      with abs(energy) > 2 sigma above noise */
  double etringnoisedR03SigAbs2() const;
  /** @brief all energy in a ring : (0.1<DR<0.3) 
      with abs(energy) > 3 sigma above noise */
  double etringnoisedR03SigAbs3() const;
  
  /** @brief all energy in a ring : (0.1<DR<0.4) 
      with abs(energy) > 2 sigma above noise */
  double etringnoisedR04SigAbs2() const;
  /** @brief all energy in a ring : (0.1<DR<0.4) 
      with abs(energy) > 3 sigma above noise */
  double etringnoisedR04SigAbs3() const;
  

  /** @brief all energy in a ring : (0.1<DR<0.3) 
      with energy > 2 sigma above noise */
  double etringnoisedR03Sig2() const;
  /** @brief all energy in a ring : (0.1<DR<0.3) 
      with energy > 3 sigma above noise */
  double etringnoisedR03Sig3() const;
  /** @brief all energy in a ring : (0.1<DR<0.3) 
      with energy > 4 sigma above noise */
  double etringnoisedR03Sig4() const;

  /** @brief all energy in a ring : (0.1<DR<0.4) 
      with energy > 2 sigma above noise */
  double etringnoisedR04Sig2() const;
  /** @brief all energy in a ring : (0.1<DR<0.4) 
      with energy > 3 sigma above noise */
  double etringnoisedR04Sig3() const;

  /** @brief link index*/ 
  int  linkIndex() const;
  /** @brief summed pt of tracks in a cone with half-opening angle 0.2
   (no zvx cut photons, 1mm electrons) */
  double ptcone20 () const;
  /** @brief summed pt of tracks in a cone with half-opening angle 0.3*/
  double ptcone30 () const;
  /** @brief summed pt of tracks in a cone with half-opening angle 0.4*/
  double ptcone40 () const;

  /** @brief number of tracks in a cone with half-opening angle 0.2*/
  double nucone20 () const;
  /** @brief number of tracks in a cone with half-opening angle 0.3*/
  double nucone30 () const;
  /** @brief number of tracks in a cone with half-opening angle 0.4*/
  double nucone40 () const;

  /** @brief summed pt of tracks in a cone with half-opening angle 0.2, |zpv| < 5mm */
  double ptcone20_zpv05() const;
  /** @brief summed pt of tracks in a cone with half-opening angle 0.2, |zpv| < 5mm */
  double ptcone30_zpv05() const;
  /** @brief summed pt of tracks in a cone with half-opening angle 0.2, |zpv| < 5mm */
  double ptcone40_zpv05() const;
  /** @brief number of tracks in a cone with half-opening angle 0.2, |zpv| < 5mm */
  double nucone20_zpv05() const;
  /** @brief number of tracks in a cone with half-opening angle 0.2, |zpv| < 5mm */
  double nucone30_zpv05() const;
  /** @brief number of tracks in a cone with half-opening angle 0.2, |zpv| < 5mm */
  double nucone40_zpv05() const;

  /** @brief pt corrected ET in a cone with half-opening angle 0.15 */
  double  etcone15_ptcorrected() const;
  /** @brief pt corrected ET in a cone with half-opening angle 0.2 */
  double  etcone20_ptcorrected() const;
  /** @brief pt corrected ET in a cone with half-opening angle 0.25 */
  double  etcone25_ptcorrected() const;
  /** @brief pt corrected ET in a cone with half-opening angle 0.3 */
  double  etcone30_ptcorrected() const;
  /** @brief pt corrected ET in a cone with half-opening angle 0.35 */
  double  etcone35_ptcorrected() const;
   /** @brief ptcorrected ET in a cone with half-opening angle 0.4 */
  double  etcone40_ptcorrected() const;

  /** @brief pt + ED corrected ET in a cone with half-opening angle 0.2 */
  double  etcone20_corrected() const;
  /** @brief pt + ED corrected ET in a cone with half-opening angle 0.3 */
  double  etcone30_corrected() const;
  /** @brief pt + ED corrected ET in a cone with half-opening angle 0.4 */
  double  etcone40_corrected() const;

  //et cone for topo cluster 
  /** @brief ET in a cone with half-opening angle 0.20 */
  double  topoetcone20() const;
  /** @brief ET in a cone with half-opening angle 0.30 */
  double  topoetcone30() const;
  /** @brief ET in a cone with half-opening angle 0.40 */
  double  topoetcone40() const;
  /** @brief pt corrected ET in a cone with half-opening angle 0.40 */
  double  topoetcone40_ptcorrected() const;
  /** @brief fully corrected ET in a cone with half-opening angle 0.40 */
  double  topoetcone40_corrected() const;

  
  
 
  // set the values of the different shower shape and isolation variables
  void set_e011(double); 
  void set_e033(double); 
  void set_e132(double); 
  void set_e1152(double); 
  void set_ethad1(double); 
  void set_ethad(double); 
  void set_ehad1(double);  
  void set_f1(double);
  void set_f3(double);
  void set_f1core(double);
  void set_f3core(double);
  void set_e233(double);
  void set_e235(double); 
  void set_e255(double); 
  void set_e237(double);
  void set_e277(double);
  void set_e333(double);
  void set_e335(double);
  void set_e337(double);
  void set_e377(double);
  void set_weta1(double);
  void set_weta2(double);
  void set_e2ts1(double);
  void set_e2tsts1(double);
  void set_fracs1(double);
  void set_widths1(double);
  void set_widths2(double);
  void set_poscs1(double);
  void set_poscs2(double);
  void set_asy1(double);
  void set_pos(double);
  void set_pos7(double);
  void set_barys1(double);
  void set_wtots1(double);
  void set_emins1(double);
  void set_emaxs1(double);
  void set_r33over37allcalo(double);
  void set_ecore(double);
  void set_zvertex(double);
  void set_errz(double);
  void set_etap(double); 
  void set_depth(double);
  void set_etcone(double); 
  void set_etcone15(double); 
  void set_etcone20(double); 
  void set_etcone25(double); 
  void set_etcone30(double); 
  void set_etcone35(double); 
  void set_etcone40(double); 
  void set_etconoisedR04Sig2(double); 
  void set_etconoisedR04Sig3(double); 

  void set_etconoisedR02SigAbs2(double); 
  void set_etconoisedR02SigAbs3(double); 
  void set_etconoisedR03SigAbs2(double); 
  void set_etconoisedR03SigAbs3(double); 
  void set_etconoisedR04SigAbs2(double); 
  void set_etconoisedR04SigAbs3(double); 

  void set_etringnoisedR03Sig2(double); 
  void set_etringnoisedR03Sig3(double); 
  void set_etringnoisedR03Sig4(double); 

  void set_etringnoisedR04Sig2(double); 
  void set_etringnoisedR04Sig3(double); 

  void set_etringnoisedR02SigAbs2(double); 
  void set_etringnoisedR02SigAbs3(double); 
  void set_etringnoisedR03SigAbs2(double); 
  void set_etringnoisedR03SigAbs3(double); 
  void set_etringnoisedR04SigAbs2(double); 
  void set_etringnoisedR04SigAbs3(double); 

  void set_linkIndex(int); 
  void set_ptcone20(double);
  void set_ptcone30(double);
  void set_ptcone40(double);
  void set_nucone20(double);
  void set_nucone30(double);
  void set_nucone40(double);
  void set_ptcone20_zpv05(double);
  void set_ptcone30_zpv05(double);
  void set_ptcone40_zpv05(double);
  void set_nucone20_zpv05(double);
  void set_nucone30_zpv05(double);
  void set_nucone40_zpv05(double);
  void set_etcone15_ptcorrected(double);
  void set_etcone20_ptcorrected(double);
  void set_etcone25_ptcorrected(double);
  void set_etcone30_ptcorrected(double);
  void set_etcone35_ptcorrected(double);
  void set_etcone40_ptcorrected(double);

  void set_etcone20_corrected(double);
  void set_etcone30_corrected(double);
  void set_etcone40_corrected(double);

  void set_topoetcone20(double);
  void set_topoetcone30(double);
  void set_topoetcone40(double);
  void set_topoetcone40_ptcorrected(double);
  void set_topoetcone40_corrected(double);

  void print() const;

// 
 private:


  std::vector < std::pair<egammaParameters::ParamDef , double > > m_parameters;
  static const std::string s_className;
};

#include "egammaEvent/EMShower.icc"

#endif 







