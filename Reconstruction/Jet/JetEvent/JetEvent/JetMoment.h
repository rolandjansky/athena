/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETMOMENT_H
#define JETEVENT_JETMOMENT_H

#include <boost/tuple/tuple.hpp>
#include <string>

struct JetMoment
{
  typedef float                              float_t;
  typedef bool                                tag_t;
  typedef boost::tuples::tuple<float_t,tag_t> database_t;

  static float_t invalidMoment() { return float_t(0.); }
  

  class MomentData : public database_t
  {
  public:
    
    MomentData() : database_t() 
    { 
      this->setMoment(0.);
      this->setTag(false);
    }
    MomentData(float_t moment,tag_t tag) : database_t() 
    {
      this->setMoment(moment);
      this->setTag(tag);
    }

    ~MomentData() = default;

    void setMoment(float_t moment) { this->get<0>() = moment; }
    void setTag(tag_t tag)         { this->get<1>() = tag; }
    void setData(float_t moment,tag_t tag) 
    { this->get<0>() = moment; this->get<1>() = tag; }

    float_t moment() const { return this->get<0>(); }
    tag_t   tag()    const { return this->get<1>(); }

    //  private:


  };
};



/* /\** */
/*  @class JetMoment */
/*  @authors Michel Lefebvre<lefebvre@uvic.ca>, Peter Loch <loch@physics.arizona.edu>, */
/*           Rob McPherson <rmcphers@triumf.ca>, Rolf Seuster <seuster@cern.ch> */
/*           Sasha Savine <savin@physics.arizona.edu> Kai Voss <kvoss@cern.ch> */
/*  @date 28-February-2005 */
/*  @brief OBSOLETE defines enums and data types for different moments of @c Jet */

/*  After release 14.1.0 this class becomes obsolete since the new @c Jet class */
/*  defines directly an equivalent concept                                         */

/*  *\/ */

/* #include <string> */

/* class JetMoment { */

/* public: */
/*   /\** @brief (DEPRECATED) enums to identify different moments */
/*    * */
/*    *  Each enum corresponds to a different characteristic moment of a */
/*    *  @c Jet. */
/*    *  */
/*    *\/ */
/*   enum MomentType { */
/*     /\** electro-magnetic energy fraction of jets                                                     *\/ */
/*     EM_FRACTION_MCTRUTH    = 0x0001,  /\** EM fraction determined from the MC truth                   *\/ */
/*     EM_FRACTION_CLUSTER    = 0x0002,  /\** EM fraction determined from cluster classification         *\/ */
/*     EM_FRACTION_CALIBHIT   = 0x0003,  /\** EM fraction determined from Calibration Hits               *\/ */
/*     EM_FRACTION_RECOTRACKS = 0x0004,  /\** EM fraction determined from reconstructed tracks           *\/ */
/*     EM_ENERGY              = 0x0005,  /\** jet energy on EM scale                                     *\/ */
/*     EM_OVER_ESUM           = 0x0010,  /\** EM energy over jet energy                                  *\/ */
    
/*     /\** track related quantities - tracks pointing to CaloTopoClusters                               *\/ */
/*     TRACKS_E               = 0x0080,  /\** Sum of energy from reconstructed tracks                    *\/ */
/*     TRACKS_P               = 0x0081,  /\** magnitude of Sum of momenta from reconstructed tracks      *\/ */
/*     TRACKS_SUM_P           = 0x0082,  /\** scalar Sum of momenta magnitude from reconstructed tracks  *\/ */
/*     TRACKS_DELTA_ETA       = 0x0083,  /\** difference in eta between tracks and jet                   *\/ */
/*     TRACKS_DELTA_PHI       = 0x0084,  /\** difference in phi between tracks and jet                   *\/ */
/*     TRACKS_RMS_ETA         = 0x0085,  /\** squares of differences in eta between tracks and their sum *\/ */
/*     TRACKS_RMS_PHI         = 0x0086,  /\** squares of differences in phi between tracks and their sum *\/ */
    
/*     /\** measures for the jet area                                                                    *\/ */
/*     CALOAREA               = 0x0101,  /\** Area computed from the CaloCell area inside the jet        *\/ */
/*     AMAREA                 = 0x0102,  /\** Area computed from Angular (2nd) Moments : A=M2(eta)*M2(phi) *\/ */
/*     GHOSTAREA              = 0x0103,  /\** Area computed via energyless ghost particles               *\/ */
/*     ELLIPTICAREA           = 0x0104,  /\** Area computed via circles around constituents              *\/ */
/*     ENG_FR005              = 0x0110,  /\** fraction of energy in  5% of max distance parameter (R or kT)*\/ */
/*     ENG_FR010              = 0x0111,  /\** fraction of energy in 10% of max distance parameter (R or kT)*\/ */
/*     ENG_FR015              = 0x0112,  /\** fraction of energy in 15% of max distance parameter (R or kT)*\/ */
/*     ENG_FR025              = 0x0113,  /\** fraction of energy in 25% of max distance parameter (R or kT)*\/ */
/*     ENG_FR050              = 0x0114,  /\** fraction of energy in 50% of max distance parameter (R or kT)*\/ */
/*     WIDTH                  = 0x0121,  /\** pT weighted sum of constituent DR wrt to the jet axis      *\/ */
    
/*     /\** sub jet structure of jets                                                                    *\/ */
/*     FLIP_Y12               = 0x0181,  /\** for kT: flip value for 1->2 subjets                        *\/ */
/*     FLIP_Y23               = 0x0182,  /\** for kT: flip value for 2->3 subjets                        *\/ */
/*     FLIP_Y34               = 0x0183,  /\** for kT: flip value for 3->4 subjets                        *\/ */
/*     FLIP_Y45               = 0x0184,  /\** for kT: flip value for 4->5 subjets                        *\/ */

/*     /\** Convex hull of jets                                                                *\/ */
/*     HULL_AREA              = 0x0200, */
/*     HULL_LENGHT            = 0x0201, */

/*     /\** noise estimate in the jet */
/*      *\/ */

/*     NOISEEXT               = 0x300, */
  
/*     /\** Track jet corrections  */
/*      *\/ */
    
/*     SEMILEPTONIC_B_JES     = 0x0401,  /\** Multiplicative jet energy scale correction to semileptonic b-jets*\/ */
/*     FTRACK                 = 0x0402,  /\** Fraction of jet energy carried by charged tracks            *\/ */
/*     FTRACK_CORRECTION      = 0x0403,  /\** Multiplicative correction to jet energy derived from f_Track*\/ */
/*     PRIMARY_VTX_FRACTION   = 0x0404,  /\** Matched track pT fraction from 1st (sum pt^2) PV in Event  *\/ */
/*     JVF_CORRECTION         = 0x0405,  /\** Multiplicative correction to jet energy derived from JVF (above) *\/ */
    
/*     UNKNOWN                = 0x8000   /\** unknown moment *\/ */
/*   }; */
  
/*   static const double DEFAULTVALUE; */
  
/*  private: */

/*   // Friends */
  
/*   // Data members */
  
/*   MomentType m_type; */

/*   /\** @brief the value of the cluster moment. *\/ */
/*   double m_value; */
  
/* public: */
  
/*   /\**  */
/*    * @brief Constructor. */
/*    * @param value The actual value of the moment */
/*    *\/ */
/*   JetMoment(const MomentType &type ,const double &value) */
/*     : m_type(type), m_value(value) { } */

/*   /\**  */
/*    * @brief default Constructor. */
/*    * */
/*    * defines the default moment value */
/*    *\/ */
/*   JetMoment() : m_type(UNKNOWN), m_value(0.) { } */

/*   // Destructors */

/*   virtual ~JetMoment() { } */

/*   // Methods */

/*   /\**  */
/*    * @brief returns the value of this moment */
/*    * @return a const reference to the double holding the value of the moment */
/*    *\/ */
/*   const double & getValue() const { return m_value; } */
  
/*   const MomentType & getType() const { return m_type; } */
  
/*   bool getName(const MomentType& type, std::string& name) const; */
  
//};

#endif // JETEVENT_JETMOMENT_H

