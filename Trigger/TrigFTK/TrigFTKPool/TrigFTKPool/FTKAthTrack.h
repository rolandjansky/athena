/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTKSIM_FTKATHTRACK_H
#define TRIGFTKSIM_FTKATHTRACK_H

#include <vector>

class FTKAthTrack {
private:
  int m_bankID; // Bank ID of the road related to this track
  int m_roadID; // Road identifier
  int m_patternID; // TODO add documentation
  int m_sectorID; // Sector identifier
  int m_trackID; // Unique track ID in this bank

  float m_invpt; // 1/(2*Pt) // aka 0.5*invpt
  float m_d0; // impact paramter
  float m_phi; // phi of the track
  float m_z0; // z0
  float m_ctheta; // cot(theta)
  float m_chi2; // chi2 of the track
  float m_origchi2; // In the case of majority recovery, this is the chi2 of 
                    // the full fit. In all other cases m_chi2 == m_origchi2

  int m_nmissing; // number of missing coordinates
  unsigned int m_typemask; // set on in bits related to the step recovery were used, ex.: 0 no recovery, 01, rec 1st step, 11, recovery in the 1st and the 2nd stage
  unsigned int m_bitmask;
  int m_ncoords; // number of coordinates for the track
  float *m_coord; //[m_ncoords] coordinates used in the fit

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

public:
  FTKAthTrack();
  FTKAthTrack(int);
  FTKAthTrack(const FTKAthTrack &);
  virtual ~FTKAthTrack();

  const int& getBankID() const { return m_bankID; }
  int getRegion() const { return m_bankID%100; }
  int getSubRegion() const { return m_bankID/100; }
  const int& getRoadID() const { return m_roadID; }
  const int& getPatternID() const { return m_patternID; }
  const int& getSectorID() const { return m_sectorID; }
  const int& getTrackID() const { return m_trackID; }
  float getParameter(int) const;
  const float& getHalfInvPt() const { return m_invpt; }
  float getPt() const { return .5/m_invpt; }
  const float& getIP() const { return m_d0; }
  const float& getPhi() const { return m_phi; }
  const float& getZ0() const { return m_z0; }
  const float& getCotTheta() const { return m_ctheta; }
  float getEta() const;
  const float& getChi2() const { return m_chi2; }
  float getChi2ndof() const { return m_chi2/(m_ncoords-m_nmissing-5); }
  const float& getOrigChi2() const { return m_origchi2; }
  float getOrigChi2ndof() const { return m_origchi2/(m_ncoords-m_nmissing-5); }
  const int&   getNMissing() const { return m_nmissing; }
  const unsigned int& getTypeMask() const { return m_typemask; }
  const unsigned int& getBitmask() const { return m_bitmask; }
  const int&   getNCoords() const { return m_ncoords; }
  const float& getCoord(int i) const { return m_coord[i]; }
  float *getCoords() { return m_coord; };
  const int& getHFRejected() const { return m_HF_rejected; }
  const int& getHWRejected() const { return m_HW_rejected; }
  const int& getHWTrackID() const { return m_HW_track; }
  const signed long& getEventIndex() const { return m_eventindex; }
  const signed long& getBarcode() const { return m_barcode; }
  const float& getBarcodeFrac() const { return m_barcode_frac; }
  
  void setBankID(int v) { m_bankID = v; }
  void setRoadID(int v) { m_roadID = v; }
  void setPatternID(int v) { m_patternID = v; }
  void setSectorID(int v) { m_sectorID = v; }
  void setTrackID(int v) { m_trackID = v; }
  void setParameter(int,float);
  void setHalfInvPt(float v) { m_invpt = v; }
  void setIP(float v) { m_d0 = v; }
  void setPhi(float v,bool ForceRange=true);
  void setZ0(float v) {m_z0 = v;}
  void setCotTheta(float v) { m_ctheta = v; }
  void setChi2(float v) { m_chi2 = v; }
  void setOrigChi2(float v) { m_origchi2 = v; }
  void setNMissing(int v) { m_nmissing = v; }
  void setTypeMask(unsigned int v) { m_typemask = v; }
  void setBitmask(unsigned int v) { m_bitmask = v; }
  void setNCoords(int);
  void setCoord(int i, float v) { m_coord[i] = v; }
  void setEventIndex( const signed long& v ) { m_eventindex = v; }
  void setBarcode( const signed long& v ) { m_barcode = v; }
  void setBarcodeFrac(const float& v ) { m_barcode_frac = v; }
  void setHFRejected(int v) { m_HF_rejected = v; }
  void setHWRejected(int v) { m_HW_rejected = v; }
  void setHWTrackID(int v) { m_HW_track = v; }

  // Function used by the HW to compare similar combinations
  unsigned int getNCommonHits(const FTKAthTrack &, const float*) const;
  int HWChoice(const FTKAthTrack&,const float*,const unsigned int,int) const;

  FTKAthTrack& operator=(const FTKAthTrack&);
};

#endif // TRIGFTKSIM_FTKATHTRACK_H
