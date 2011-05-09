// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTau_ROI.h,v 1.8 2008-05-16 08:34:32 watsona Exp $
#ifndef ANALYSISTRIGGEREVENT_EMTAU_ROI_H
#define ANALYSISTRIGGEREVENT_EMTAU_ROI_H

// STL/System include(s):
#include <string>
#include <vector>
#include <inttypes.h>

// Athena include(s):
#include "EventKernel/INavigable4Momentum.h"
#include "FourMom/P4PtEtaPhiMBase.h"
#include "Navigation/NavigableTerminalNode.h"

/**
 *   @short Em/Tau RoI class for analysis
 *
 *          As the others, this RoI class also inherits from INavigable4Momentum to make
 *          handling it a bit easier in analyses.
 *
 *  @author Tadashi Maeno
 *  @author Alan Watson
 *  @author Attila Krasznahorkay
 * @version $Revision: 1.8 $
 *    @date $Date: 2008-05-16 08:34:32 $
 *
 *     @see INavigable4Momentum
 */
class EmTau_ROI : public P4PtEtaPhiMBase,
                  public NavigableTerminalNode,
                  virtual public INavigable4Momentum {

public:
   /// Constructor used when creating the object from RoIBResult data
   EmTau_ROI( uint32_t roiWord, float eta, float phi, uint32_t thrPattern );
   /// Default constructor (for persistency purposes)
   EmTau_ROI();
   /// Destructor
   virtual ~EmTau_ROI();

   /// Type storing the names of the passed thresholds
   typedef std::vector< std::string > thresholds_type;
   /// Type storing the values of the passed thresholds
   typedef std::vector< float >       thr_value_type;

   /// Return the RoI word produced by the L1Calo hardware
   uint32_t    getROIWord()    const { return m_roiWord; }
   /// Legacy function returning the eta coordinate of the RoI
   float       getEta()        const { return m_eta; }
   /// Legacy function returning the phi coordinate of the RoI
   float       getPhi()        const { return m_phi; }
   /// Return the ET of the RoI Core cluster (2x2 towers, EM+Had)
   float       getCore()       const { return m_Core; }
   /// Return the deposited ET from the "EM cluster"
   float       getEMClus()     const { return m_EMClus; }
   /// Return the deposited ET from the "tau cluster"
   float       getTauClus()    const { return m_TauClus; }
   /// Return the EM calorimeter isolation (outer ring of EM towers)
   float       getEMIsol()     const { return m_EMIsol; }
   /// Return the hadron calorimeter isolation (outer ring of had towers)
   float       getHadIsol()    const { return m_HadIsol; }
   /// Return the ET deposited in the inner hadronic isolation region (2x2 core)
   float       getHadCore()    const { return m_HadCore; }
   /// Return the bit-pattern describing the passed thresholds
   uint32_t    getThrPattern() const { return m_thrPattern; }

   /// Return the names of the passed threshodlds
   const thresholds_type& getThresholdNames()  const { return m_thresholdNames; }
   /// Return the values of the passed thresholds
   const thr_value_type&  getThresholdValues() const { return m_thresholdValues; }

   /// Add the name of a threshold that this RoI passed
   void addThresholdName( thresholds_type::value_type thr ) { m_thresholdNames.push_back( thr ); }
   /// Add the value of a threshold that this RoI passed
   void addThresholdValue( thr_value_type::value_type thr ) { m_thresholdValues.push_back( thr ); }

   /// Set the ET deposited in the "RoI core"
   void setCore( float value )    { m_Core = value; }
   /// Set the deposited ET in the "EM cluster"
   void setEMClus( float value )  { m_EMClus = value; }
   /// Set the deposited ET in the "tau cluster"
   void setTauClus( float value ) { m_TauClus = value; }
   /// Set the EM calorimeter isolation (outer ring of EM towers)
   void setEMIsol( float value )  { m_EMIsol = value; }
   /// Set the hadron calorimeter isolation (outer ring of Had towers)
   void setHadIsol( float value ) { m_HadIsol = value; }
   /// Set the ET deposited in the inner hadronic isolation region (2x2 core)
   void setHadCore( float value ) { m_HadCore = value; }

   /// Return the p<sub>T</sub> of the highest threshold passed
   virtual double pt()  const;
   /// Return the eta coordinate of the center of the RoI
   virtual double eta() const { return m_eta; }
   /// Return the phi coordinate of the center of the RoI
   virtual double phi() const { return m_phi; }
   /// Return the mass of the object, always 0.0 for RoIs
   virtual double m()   const { return 0.0; }

private:
   uint32_t m_roiWord; //!< 32bit encoded ROI word

   float m_eta; //!< eta coordinate of RoI
   float m_phi; //!< phi coordinate of RoI

   uint32_t m_thrPattern; //!< threshold bit pattern

   thresholds_type m_thresholdNames;  //!< Names of the passed thresholds
   thr_value_type  m_thresholdValues; //!< Values of the passed thresholds

   float m_Core;    //!< ET of RoI core cluster (used to select window)
   float m_EMClus;  //!< Deposited ET in the "EM cluster"
   float m_TauClus; //!< Deposited ET in the "tau cluster"
   float m_EMIsol;  //!< EM calorimeter isolation
   float m_HadIsol; //!< Outer Hadron calorimeter isolation
   float m_HadCore; //!< ET of inner hadronic isolation sum

   mutable float m_highestThreshold; //!< return value for inherited pt() accessor

}; // class EmTau_ROI

#endif // ANALYSISTRIGGEREVENT_EMTAU_ROI_H
