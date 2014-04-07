/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EMTRACKMATCH_H
#define EGAMMAEVENT_EMTRACKMATCH_H

/**
  @class EMTrackMatch
          Track match property data class. It is made by EMTrackMatchBuilder, 
          which is an algorithm responsible for the match of a calo cluster 
          with the best matched track in front of the shower.
   @author H. Ma, S. Rajagopalan
*/

#include <vector>
#include "egammaEvent/egDetail.h"

class EMTrackMatch : public egDetail
{
public:
 
  /** Enum for track extrapolation to calo */
  enum TrkExtrapDef {
    /** from the last measurement of TrackParticle */
    fromLastMeasurement,
    /** from the perigee of TrackParticle */
    fromPerigee,
    /** from the perigee of TrackParticle recaled by Ecluster */
    fromPerigeeRescaled,
    /** from the calo to the perigee - fast extrapolation */
    fromCaloToPerigee
  };

  /** @brief Default constructor*/
  EMTrackMatch();

  /** @brief constructor with data*/
  EMTrackMatch(const std::vector<double>& deltaEta,
               const std::vector<double>& deltaPhi,
               bool                       isPrimary,
               int                        trackNumber,
               bool                       trackRefit,
	       int                        linkIndex = 0);

  /** @brief constructor with data*/
  EMTrackMatch(const std::vector<double>& EtaExtrap,
	       const std::vector<double>& PhiExtrap,
	       const std::vector<double>& deltaEta,
	       const std::vector<double>& deltaPhi,
	       bool                       isPrimary,
	       int                        trackNumber,
	       bool                       trackRefit,
	       int                        linkIndex = 0);
  
  /** @brief Destructor*/
  virtual ~EMTrackMatch();

  virtual const std::string& className() const;
  
  virtual double        parameter(egammaParameters::ParamDef) const ;
  virtual bool          hasParameter(egammaParameters::ParamDef) const ;
  virtual bool          hasBoolParameter(egammaParameters::ParamDef) const ;
  
  void                  set_parameter(egammaParameters::ParamDef, double, bool overwrite=false) ;
  void                  set_boolParameter(egammaParameters::ParamDef, bool, bool overwrite = false);
  
  /** @brief deltaEta at pre sampler(0) -> 3rd sampling(3)*/
  double		deltaEta (int) const;
  /** @brief deltaPhi at pre sampler(0) -> 3rd sampling(3)*/
  double		deltaPhi (int) const;
  /** @brief Eta of track extrapolated at pre sampler(0) -> 3rd sampling(3)*/
  double		EtaExtrap (int) const;
  /** @brief Phi of track extrapolated at pre sampler(0) -> 3rd sampling(3)*/
  double		PhiExtrap (int) const;
  /** @brief PhiRescaled at pre sampler(0) -> 3rd sampling(3)*/
  double		PhiRescaled (int) const;
  

  /** @brief eta at sampling 1 for extrapolation from the last hit -
      allows recalcultion of deltaEta */
  double                etaSampling1 () const;
  
  /** @brief deltaPhi at sampling 2 for rescaled momentum extrapolation 
      from the perigee. 
      Only available if not in deltaPhi, i.e. default.*/
  double                deltaPhiRescaled () const;

  /** @brief enum to indicate which track extrapolation method was
   *  used for track match. */
  TrkExtrapDef          extrapMethod () const;

  /** @brief deltaPhi between the direction of the momentum of the track 
      at its perigee and the phi of the POSITION of the "center of gravity" of the electron as seen from the POSITION of the vertex of the track. (And not the PHI DIRECTION of the momentum of the track at its center of gravity in the calo. There's a factor of 2 if one uses the phi direction of the momentum.) */ 

  double                deltaPhiRot () const;

  /** @brief deltaPhi from the last Point */

  double                deltaPhiLast () const;

  /** @brief return track transverse impact parameter wrt beam spot */
  double trackd0_physics() const;
  
  /** @brief return track transverse impact parameter wrt primary vertex */
  double trackd0_pv() const;

  /** @brief return track transverse impact parameter wrt beam spot */
  double trackd0sigma_physics() const;
  
  /** @brief return track transverse impact parameter wrt primary vertex */
  double trackd0sigma_pv() const;

  /** @brief boolean to know if b-layer hit is expected */
  bool expectHitInBLayer() const;

  /** @brief general set method (arguments as constructor)*/
  void                  fill(const std::vector<double>& deltaEta,
                             const std::vector<double>& deltaPhi,
                             bool                       isPrimary,
                             int                        trackNumber,
                             bool                       trackRefit,
			     int                        linkIndex = 0);
  /** @brief general set method (arguments as constructor)*/
  void                  fill(const std::vector<double>& EtaExtrap,
                             const std::vector<double>& PhiExtrap,
			     const std::vector<double>& deltaEta,
                             const std::vector<double>& deltaPhi,
                             bool                       isPrimary,
                             int                        trackNumber,
                             bool                       trackRefit,
			     int                        linkIndex = 0);

   
  /** @brief set method (only E/p - which is obsolete BTW !)*/
  void			fill(int		trackNumber);
  
  /** @brief set method (EtaExtrap and PhiExtrap)*/
  void                  fillEtaPhiExtrap(const std::vector<double>& EtaExtrap,
					 const std::vector<double>& PhiExtrap);
  /** @brief set method (DeltaEta and Phi)*/
  void                  fill(const std::vector<double>& deltaEta,
                             const std::vector<double>& deltaPhi);
  
  /** @brief true if matched track has a hit in first or second pixel layer*/
  bool			isPrimary () const;
  
  /** @brief true if track was refitted to account for brem effects*/
  bool                  trackRefit () const;
  
  // debugging aid
  void			print() const;
  
  /** @brief index number in TrackContainer (counting starts at 1 for sake of CBNT)*/
  int			trackNumber () const;

  /** @brief index number among TrackParticle links of the egamma object (counting starts at 0)*/
  int			linkIndex () const;
  void			set_linkIndex (int value);
  
  /** @brief number of tracks in isolation window (taken at sampling 2): 
      (# primary tracks if matched track is primary; otherwise all tracks)
      note: overload used to distinguish get/set method*/
  int			tracksInBroadWindow () const;
  void			tracksInBroadWindow (int value, bool overwrite=true);
  void setEtaExtrap (int sampl, double value, bool overwrite=false); 
  void setPhiExtrap (int sampl, double value, bool overwrite=false); 
  void setPhiRescaled (int sampl, double value, bool overwrite=false); 
  void setDeltaEta (int sampl, double value, bool overwrite=false); 
  void setDeltaPhi (int sampl, double value, bool overwrite=false); 
  void setEtaSampling1 (double value, bool overwrite=false); 
  void setDeltaPhiRescaled (double value, bool overwrite=false); 
  void setextrapMethod (TrkExtrapDef value, bool overwrite=false); 
  void setDeltaPhiRot (double value, bool overwrite=false); 
  void setDeltaPhiLast (double value, bool overwrite=false); 
  
 private: 
  
  std::vector < std::pair<egammaParameters::ParamDef , double > > m_parameters;
  std::vector < std::pair<egammaParameters::ParamDef, bool> > m_parametersBool;
  
  virtual bool boolParameter(egammaParameters::ParamDef)     const;
};


#endif 


