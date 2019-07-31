/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTKSIM_FTKTRACK_H
#define TRIGFTKSIM_FTKTRACK_H

#include "FTKHit.h"
#include <vector>
#include <TObject.h>
#include <TMath.h>

class FTKTrack : public TObject {
private:
  int m_bankID; // Bank ID of the road related to this track
  int m_roadID; // Road identifier
  int m_patternID; // TODO add documentation
  int m_sectorID; // Sector identifier
  int m_trackID; // Unique track ID in this bank
  int m_combid; // Number of the combination within the road
  int m_extrid; // Number of the combination in the extrapolation

  float m_invpt; // 1/(2*Pt), aka this is really 0.5 * inverse of pt
  float m_d0; // impact paramter in the ATLAS reference system
  float m_rawd0; // impact parameter w.r.t. the training beam-spot position
  float m_phi; // phi of the track
  float m_rawphi; // raw phi w.r.t. to the training beam-spot position
  float m_z0; // z0 in standard ATLAS reference system
  float m_rawz0; // raw z0 w.r.t. the training beam-spot position
  float m_ctheta; // cot(theta)
  float m_chi2; // chi2 of the track
  float m_origchi2; // In the case of majority recovery, this is the chi2 of 
                    // the full fit. In all other cases m_chi2 == m_origchi2

  float m_invptfw; // curvature value in the output format of the FW
  int m_d0fw; // impact parameter as calculated by the FW
  int m_phifw; // phi value as caculated by the FW
  int m_z0fw; // z0 from the FW
  int m_cthetafw; // cot(theta) using the AUX card or SS board firmware-like code
  int m_chi2fw; // chisq calculation using the AUX card or SS board firmware-like code

  int m_nmissing; // number of missing coordinates
  unsigned int m_typemask; // set on in bits related to the step recovery were used, ex.: 0 no recovery, 01, rec 1st step, 11, recovery in the 1st and the 2nd stage
  unsigned int m_bitmask;
  int m_ncoords; // number of coordinates for the track
  float *m_coord; //[m_ncoords] coordinates used in the fit

  int m_nplanes; // number of layer planes used
  FTKHit *m_hits; //[m_nplanes] hits associated to the track

  int m_nplanes_ignored; // number of planes to extrapolate the 7L tracks
  int *m_ssid; // [m_nplanes_ignored] SSIDs associated to the extrapolated AUX tracks

  int m_HF_rejected; /* 0 if the track is accepted, >0 if rejected by hit filter
			if negative, belongs to a HF rejected road
		     */
  int m_HW_rejected; /* The flag has 3 digit: [Q][RW][HW]:
		      Q : 1 if has a bad chi2, 0 if not
		      RW: is the RW level rejected the road, 0 if not rejected
		      HW: 1 if rejected by HW, 0 if not */
  int m_HW_track; // ID of the track excluded this track

  signed long m_eventindex; // matched particle event index
  signed long m_barcode; // matched geant particle barcode
  float m_barcode_frac; // largest "matching fraction" with any "good"
                        // geant particle, corresponding to the
                        // particle with m_barcode
  int m_connindex; // 2nd stage connection index

public:
  FTKTrack();
  FTKTrack(const FTKTrack &);
  FTKTrack(const int &, const int &);
  virtual ~FTKTrack();

  const int& getBankID() const { return m_bankID; }
  int getRegion() const { return m_bankID%100; }
  int getSubRegion() const { return m_bankID/100; }
  const int& getRoadID() const { return m_roadID; }
  const int& getPatternID() const { return m_patternID; }
  const int& getSectorID() const { return m_sectorID; }
  const int& getTrackID() const { return m_trackID; }
  const int& getCombinationID() const { return m_combid; }
  const int& getExtrapolationID() const { return m_extrid; }
  float getParameter(int, bool useraw=false) const;
  const float& getHalfInvPt() const { return m_invpt; } 
  float getPt() const { return .5/m_invpt; } // 0.5 is to account for the fact that inv_pt is really 0.5/pt
  const float& getIP() const { return m_d0; }
  const float& getIPRaw() const { return m_rawd0; }
  const float& getPhi() const { return m_phi; }
  const float& getPhiRaw() const { return m_rawphi; }
  const float& getZ0() const { return m_z0; }
  const float& getZ0Raw() const { return m_rawz0; }
  const float& getCotTheta() const { return m_ctheta; }
  float getEta() const { return TMath::ASinH(m_ctheta); }
  const float& getChi2() const { return m_chi2; }
  float getChi2ndof() const { return m_chi2/(m_ncoords-m_nmissing-5); }
  const float& getOrigChi2() const { return m_origchi2; }
  float getOrigChi2ndof() const { return m_origchi2/(m_ncoords-m_nmissing-5); }
  float getInvPtFW() const { return m_invptfw; }
  int getIPFW() const { return m_d0fw; }
  int getPhiFW() const { return m_phifw; }
  int getZ0FW() const { return m_z0fw; }
  int getCTheta() const { return m_cthetafw; }
  int getChi2FW() const { return m_chi2fw; }
  const int&   getNMissing() const { return m_nmissing; }
  const unsigned int& getTypeMask() const { return m_typemask; }
  const unsigned int& getBitmask() const { return m_bitmask; }
  const int&   getNCoords() const { return m_ncoords; }
  const int& getNPlanes() const { return m_nplanes; }
  const float& getCoord(int i) const { return m_coord[i]; }
  float *getCoords() { return m_coord; };
  const int& getNPlanesIgnored() const { return m_nplanes_ignored; }
  const int& getSSID(int i) const { return m_ssid[i]; }
  const int& getHFRejected() const { return m_HF_rejected; }
  const int& getHWRejected() const { return m_HW_rejected; }
  const int& getHWTrackID() const { return m_HW_track; }
  const signed long& getEventIndex() const { return m_eventindex; }
  const signed long& getBarcode() const { return m_barcode; }
  const float& getBarcodeFrac() const { return m_barcode_frac; }
  const int& getConnectionIndex() const { return m_connindex; }
  const FTKHit& getFTKHit(int i) const { return m_hits[i]; }

  // additional function returns hit coordinate after converting
  // to integer precision used in firmware
  signed long long getAUXCoord(int i) const { return static_cast<signed long long>( floor((m_coord[i]*8.0) + 0.5) ); }

  unsigned int getHwCoord(int i, int dim) const { return m_hits[i].getHwCoord(dim); }

  void setBankID(int v) { m_bankID = v; }
  void setRoadID(int v) { m_roadID = v; }
  void setPatternID(int v) { m_patternID = v; }
  void setSectorID(int v) { m_sectorID = v; }
  void setTrackID(int v) { m_trackID = v; }
  void setCombinationID(int v) { m_combid = v; }
  void setExtrapolationID(int v) { m_extrid = v; }
  void setParameter(int,float, bool useraw=false);
  void setHalfInvPt(float v) { m_invpt = v; }
  void setIP(float v) { m_d0 = v; }
  void setIPRaw(float v) { m_rawd0 = v; }
  void setPhi(float v,bool ForceRange=true);
  void setPhiRaw(float v,bool ForceRange=true);
  void setZ0(float v) {m_z0 = v;}
  void setZ0Raw(float v) {m_rawz0 = v;}
  void setCotTheta(float v) { m_ctheta = v; }
  void setChi2(float v) { m_chi2 = v; }
  void setOrigChi2(float v) { m_origchi2 = v; }
  void setInvPtFW(float v) {  m_invptfw = v; }
  void setIPFW(int v) {  m_d0fw = v; }
  void setPhiFW(int v,bool ForceRange=true);
  void setZ0FW(int v) {  m_z0fw = v; }
  void setCTheta(int v) {  m_cthetafw = v; }
  void setChi2FW(int v) { m_chi2fw = v; }
  void setChi2FW(double v) { m_chi2fw = v; }
  void setNMissing(int v) { m_nmissing = v; }
  void setTypeMask(unsigned int v) { m_typemask = v; }
  void setBitmask(unsigned int v) { m_bitmask = v; }
  void setNCoords(int);
  void setCoord(int i, float v) { m_coord[i] = v; }
  void setEventIndex( const signed long& v ) { m_eventindex = v; }
  void setBarcode( const signed long& v ) { m_barcode = v; }
  void setBarcodeFrac(const float& v ) { m_barcode_frac = v; }
  void setConnectionIndex(const int& v) { m_connindex = v; }
  void setHFRejected(int v) { m_HF_rejected = v; }
  void setHWRejected(int v) { m_HW_rejected = v; }
  void setHWTrackID(int v) { m_HW_track = v; }
  void setNPlanes(int);
  void setFTKHit(int i,const FTKHit &hit) const { m_hits[i] = hit; }
  void setNPlanesIgnored(int);
  void setSSID(int i, int v) { m_ssid[i] = v; }

  // Function used by the HW to compare similar combinations
  unsigned int getNCommonHits(const FTKTrack &, const float*) const;
  unsigned int getNCommonHitsFirstStage(const FTKTrack &, const float*) const;

  int HWChoice(const FTKTrack&,const float*,const unsigned int,int);
  int HWChoiceFirstStage(const FTKTrack&,const float*,const unsigned int,int);

  virtual void Print(Option_t * opts="") const;

  FTKTrack& operator=(const FTKTrack&);

  friend std::ostream& operator<<(std::ostream&,const FTKTrack&);  

  ClassDef(FTKTrack,18)
};

#endif // TRIGFTKSIM_FTKTRACK_H
