/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/************************************************************************
                     CaloParticleIDTool.h - Copyright ATLAS Collaboration

author: Doug Schouten <dschoute at sfu dot ca>
author: Gennady Pospelov <pospelov at mppmu dot mpg dot de>

**************************************************************************/

#ifndef CALOPARTICLEIDTOOL_H
#define CALOPARTICLEIDTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloEvent/CaloCell.h"

#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"

#include "CaloInterface/ICaloParticleIDTool.h"

#include <limits>
#include <map>
#include <string>
#include <vector>

class CaloDetDescrManager;

///////////////////////////////////////////////////////////////////////////////
//
// Class to access calibration hit information, including 
// particle provenance, for (composite) calorimeter objects.
//
///////////////////////////////////////////////////////////////////////////////

class CaloParticleIDTool : virtual public ICaloParticleIDTool, 
			   virtual public IIncidentListener,
			   public AthAlgTool
{
public:
  CaloParticleIDTool(const std::string&,
		     const std::string&,
		     const IInterface*);
  
  virtual ~CaloParticleIDTool() {}
  
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  //
  // ICaloParticleIDTool API - see https://twiki.cern.ch/twiki/bin/viewauth/Atlas/CaloParticleID
  //
  
  // return the weighted sum of the energy of all hits left by a particle inside a calorimeter object
  virtual double energyFromParticle( const INavigable* calObject, 
				     const HepMC::GenParticle* particle, 
				     EnergyType t = VISIBLE ) const ;
  

  // return the energy deposited in the calorimeter by a particle, applying a filter to identify cells of interest.
  virtual double energyInCalorimeter( const HepMC::GenParticle* particle, 
				      EnergyType t = VISIBLE,
				      void* caller = 0x0, 
				      bool (*accept)(void*, const Identifier&) = &(ICaloParticleIDTool::idAccept)) const ;

  
  // return the energy-weighted pseudorapidity in the calorimeter by a particle, applying a filter to identify cells of interest.
  virtual double etaInCalorimeter( const HepMC::GenParticle* particle,
				   EnergyType t = VISIBLE,
				   void* caller = 0x0, 
				   bool (*accept)(void*, const Identifier&) = &(ICaloParticleIDTool::idAccept) ) const ;


  // return the energy deposited in dead material by a particle
  virtual double energyInDeadMaterial( const HepMC::GenParticle* particle, 
				       EnergyType t = TOTAL,
				       void* caller = 0x0, 
				       bool (*accept)(void*, const Identifier&) = &(ICaloParticleIDTool::idAccept)) const ;
  
  
  // load energy map of all particles contributing hits to a calorimeter object.
  virtual void loadParticleMap( const INavigable* caloObject, 
				std::map<const HepMC::GenParticle*,double>& m, 
				EnergyType t = VISIBLE,
				void* caller = 0x0, 
				bool (*accept)(void*, const Identifier&) = &(ICaloParticleIDTool::idAccept) ) const ;
  
  // return the energy of a given type from all hits in a calorimeter object
  virtual double energyInObject( const INavigable* calObject, 
				 EnergyType t = VISIBLE ) const ;
  
  
  // retrieve all hits associated with a particle
  virtual void retrieveHits( const HepMC::GenParticle* particle, 
			     CaloCalibrationHitContainer* hits ) const ;
  
  
  // retrieve all hits associated with a calorimeter object
  virtual void retrieveHits( const INavigable* calObject, 
			     CaloCalibrationHitContainer* hits ) const ;  


  // retrieve all hits associated with a calorimeter cell
  virtual void retrieveHits( const Identifier ID, 
			     const std::vector<const CaloCalibrationHit*>*& hits) const ;

  
  // return pointer to the HepMC::GenParticle object with the specified barcode
  virtual const HepMC::GenParticle* getParticle( unsigned int barcode ) const ;


  // register a call-back to transform hit quantities upon retrieval from SG
  virtual void registerHitTransformation( void*, CaloCalibrationHit* (*transform)( void*, const CaloCalibrationHit* ) ) ;


  //
  // IIncidentListener API
  //

  // handle a begin event incident 
  void handle( const Incident& );

  // reload the calibration hit information for an event
  virtual StatusCode setupEvent( );

protected:
  
  //
  // Useful class definitions to manage internal data
  //
  
  // ++ --------------------------------------------------- ++

  class CalibrationHitInfo {
  public:
    typedef std::vector<const CaloCalibrationHit*> hitcollection_t;
    typedef std::vector<double> weightcollection_t;
    
    mutable double energy_invisible;
    mutable double energy_em;
    mutable double energy_nonem;
    mutable double energy_total;
    
    CalibrationHitInfo( )
    {
      energy_invisible = 0.;
      energy_em        = 0.;
      energy_nonem     = 0.;
      energy_total     = 0.;
    }

    CalibrationHitInfo( const CalibrationHitInfo& other )
    {
      energy_invisible = 0.;
      energy_em        = 0.;
      energy_nonem     = 0.;
      energy_total     = 0.;
      for( unsigned int i = 0; i < other.hits().size(); ++i )
	add( other.hits()[i], other.weights()[i] );
    }
    
    CalibrationHitInfo& operator= ( const CalibrationHitInfo& other )
    {
      if (this != &other) {
        energy_invisible = 0.;
        energy_em        = 0.;
        energy_nonem     = 0.;
        energy_total     = 0.;
        m_hits.clear();
        for( unsigned int i = 0; i < other.hits().size(); ++i )
          add( other.hits()[i] );
      }
      return *this;
    }


    ~CalibrationHitInfo( ) {}

    void add( const CaloCalibrationHit* hit, double w = 1.0 ) const 
    { 
      m_hits.push_back( hit ); 
      m_weights.push_back( w );

      energy_invisible += w * (hit->energyInvisible() + hit->energyEscaped());
      energy_em        += w * hit->energyEM();
      energy_nonem     += w * hit->energyNonEM();
      energy_total     += w * hit->energyTotal();
    }
    
    const std::vector<const CaloCalibrationHit*>& hits() const { return m_hits; }
    const std::vector<double>& weights() const { return m_weights; }

    hitcollection_t::const_iterator begin( ) const { return m_hits.begin(); }
    hitcollection_t::const_iterator end( )   const { return m_hits.end();   }

    double weight( unsigned int i ) const { return ( i < m_weights.size() ? m_weights[i] : 0. ); }
  protected:
    mutable hitcollection_t      m_hits;    //! list of calibration hits in the cell
    mutable weightcollection_t   m_weights; //! weights for each hit
  };

  // ++ --------------------------------------------------- ++
  
  class ParticleInfo : public CalibrationHitInfo {    
  public:    
    mutable double energy_measurable;
    
    ParticleInfo( unsigned int bc ) : CalibrationHitInfo( )
    {
      energy_measurable = 0;
      m_barcode = bc;
    }

    ParticleInfo( const CaloCalibrationHit* hit, unsigned int bc ) : CalibrationHitInfo( )
    {
      energy_measurable = 0;
      m_barcode = bc;
      add( hit );
    }

    bool operator==(const ParticleInfo& cmp) const { return cmp.barcode() == m_barcode; }
    bool operator< (const ParticleInfo& cmp) const { return cmp.barcode() <  m_barcode; }
    
    inline unsigned int barcode( ) const { return m_barcode; }
        
    class CmpFunctorLess
    {
    public:
      bool operator()(const ParticleInfo& l, const ParticleInfo& r) const
      {
	return (l < r) ;
      }
    };

  private:
    ParticleInfo() { }
    
    unsigned int m_barcode;
  };
  
  // ++ --------------------------------------------------- ++
  
  class CellInfo : public CalibrationHitInfo {
  public:        
    CellInfo( Identifier id, 
	      double eta = std::numeric_limits<double>::max(), 
	      double phi = std::numeric_limits<double>::max() ) : CalibrationHitInfo( )
    { 
      m_cellid = id;
      m_eta = eta;
      m_phi = phi;
    }
    
    CellInfo( const CaloCalibrationHit* hit, 
	      double eta = std::numeric_limits<double>::max(), 
	      double phi = std::numeric_limits<double>::max() ) : CalibrationHitInfo( )
    {
      m_cellid = hit->cellID();
      m_eta = eta;
      m_phi = phi;

      add( hit );
    }
    
    ~CellInfo() { }
    
    bool operator==(const CellInfo& info) const { return (m_cellid == info.ID()); }
    bool operator< (const CellInfo& info) const { return (m_cellid  < info.ID()); }
     
    const Identifier& ID() const { return m_cellid; }
    double eta() const { return m_eta; }
    double phi() const { return m_phi; }
        
    class CmpFunctorLess {
    public:
      bool operator()(CellInfo const& p, CellInfo const& q) const
      {
	return ( p < q );
      }
    };
    
  private:
    CellInfo() { }
    
    double m_eta;                                           //! eta of the calorimeter cell
    double m_phi;                                           //! phi of the cell
    Identifier m_cellid;                                    //! identifier object
  };

  // ++ --------------------------------------------------- ++

  typedef ParticleInfo                                          particleinfo_t;
  typedef CellInfo                                              cellinfo_t;
  typedef std::set<CellInfo, CellInfo::CmpFunctorLess>          cellinfolist_t;
  typedef std::set<ParticleInfo, ParticleInfo::CmpFunctorLess>  particleinfolist_t;
  typedef INavigable                                            base_t;
  typedef CaloCalibrationHit                                    calibhit_t;
  typedef std::vector<const CaloCalibrationHit*>                hitcollection_t;
  
private:
  /* @brief flag to indicate whether calibration hit info is up to date */
  bool m_isConfigValid;
  
  const CaloDetDescrManager* m_caloDetDescrMgr;
  const AtlasDetectorID*     m_detectorIDHelper;
  
  /* @brief tool configuration */
  std::vector<std::string> m_calibrationCaloHitContainerNames; //! names of calibration hit containers
  std::vector<std::string> m_calibrationDMHitContainerNames;   //! names of calib hits for dead material
  std::string m_truthParticleCollectionName;                   //! name of the truth particles collection
  bool m_useCellWeights;                                       //! flag to use cell weights in calculating sums
  std::vector<int> m_maskRegions;                              //! regions to ignore when tabulating hits

  /* @brief event data to maintain */
  const TruthParticleContainer* m_truthParticles;              //! truth particle collection
  cellinfolist_t                m_calorimeterCellInfo;         //! information for each cell that has hits in the calorimeter
  particleinfolist_t            m_calorimeterParticleInfo;     //! ditto for all particles that hit the calorimeter
  particleinfolist_t            m_deadMaterialParticleInfo;    //! and for particles that leave hits in dead material
  
  static unsigned int s_numWarningsIssued;
  
  typedef CaloCalibrationHit* (*transform)( void*, const CaloCalibrationHit* );
  std::vector< std::pair<void*,
			 transform> >  m_hitCallbacks;         //! list of transformations to perform on loaded hits

  CaloCalibrationHitContainer m_allHits;                       //! store an internal copy of all loaded calibration hits
private:

  // find maximum measurable energy for an MC particle
  double getMeasurableEnergy( unsigned int barcode ) const;

  // check calibration state of composite calorimeter object
  void doSignalStateCheck( const INavigable* ) const;

  // retrieve energy of a given type from a cell/particle info object
  template <typename T>
  double getEnergy( const T&, EnergyType ) const;

  // add hit to particle/calorimeter information list
  void addHit( particleinfolist_t&, const calibhit_t* );
  void addHit( cellinfolist_t&, const calibhit_t* );

  // energy from filtered hit collection
  double energyFiltered( const particleinfolist_t& info,
			 const HepMC::GenParticle* particle,
			 EnergyType t, 
			 void* caller,
			 bool (*accept)( void*, const Identifier& ) ) const;

  CaloCalibrationHit* getCopy( const CaloCalibrationHit* );
};

/** ============================================================================ */
template <typename T>
inline double CaloParticleIDTool::getEnergy( const T& info, EnergyType t ) const 
{
  switch( t ) {
    case VISIBLE:
      return info.energy_em + info.energy_nonem;
      break;
      
    case VISIBLE_EM:
      return info.energy_em;
      break;
      
    case VISIBLE_HAD:
      return info.energy_nonem;
      break;
      
    case INVISIBLE:
      return info.energy_invisible; 
      break;
      
    case TOTAL:
      return info.energy_total;
      break;
  }
  return info.energy_total;
}

/** ============================================================================ */
inline const HepMC::GenParticle* CaloParticleIDTool::getParticle( unsigned int barcode ) const
{
  const TruthParticle* p = m_truthParticles->truthParticle( barcode );
  return p != NULL ? p->genParticle() : NULL;
}

#ifdef CALOPARTICLEIDTOOL_CXX
unsigned int CaloParticleIDTool::s_numWarningsIssued = 0;
#endif // CALOPARTICLEIDTOOL_CXX

#endif // CALOPARTICLEIDTOOL_H

