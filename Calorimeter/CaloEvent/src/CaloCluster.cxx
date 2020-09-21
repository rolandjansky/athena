/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloCluster.cxx
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  H. Ma
CREATED:  Apr 2001

PURPOSE:  Cluster base class.
          Implements CaloCluster methods

Updated:  Redesign.  May 21, 2001
          CaloCluster is a concrete class

Updated:  Redesign.  Feb 17, 2004 P Loch
          Complete overhaul to accommodate navigation and 4-vector interfaces.

Updated:  Aug, 2004 sss
          Fix phi range problem and a potential division by zero.

Updated:  30 Jun, 2005 S.Menke
          updateKine uses |E| instead of E for the position calculation
          and updates all data members now

********************************************************************/

// basics
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_ID.h"

// cell links
#include "CaloEvent/CaloCompositeCellBase.h"
#include "CaloEvent/CaloClusterNavigable.h"

// data store
#include "CaloEvent/CaloShower.h"
#include "CaloEvent/CaloShowerContainer.h"
#include "CaloEvent/CaloSamplingData.h"

// kinematics
#include "CaloEvent/CaloCompositeKineBase.h"

// data helpers
#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloEvent/CaloPrefetch.h"
#include "CaloGeoHelpers/proxim.h"

// calculators
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "CxxUtils/prefetch.h"
#include <vector>
#include <algorithm>
//#include <iostream>

const double CaloCluster::m_errorValue = -999.;

/////////////////
// Constructor //
/////////////////
/*
 *
 *  \param eta0 \f$ \eta \f$ of cluster seed, optional
 *  \param phi0 \f$ \varphi \f$ of cluster seed, optional
 *
 */
CaloCluster::CaloCluster(double eta0 /*= 0*/, double phi0 /*= 0*/,
			 unsigned int varTypePattern,
			 unsigned int clusterSize)
  : CaloCompositeKineBase(),
    CaloCompositeCellBase<CaloClusterNavigable>(),
    m_dataStore(varTypePattern),
    m_ownDataStore(false),
    m_shower(nullptr),
    m_basicSignal(0.),
    m_time(0.),
    m_samplingPattern(0),
    m_barrel (false),
    m_endcap (false),
    m_clusterSize(clusterSize),
    m_eta0 (eta0),
    m_phi0 (phi0),
    m_timeNorm(0.),
    m_posNorm(0.),
    m_posSamNorm(CaloSampling::Unknown,0.),
    m_nBarrel(0),
    m_nEndcap(0),
    m_lockPattern(0x00000000),
    m_rawE(0.),
    m_rawEta(0.),
    m_rawPhi(0.),
    m_rawM(0.),
    m_altE(0.),
    m_altEta(0.),
    m_altPhi(0.),
    m_altM(0.)
{
  this->setDefaultSignalState(statename_t::CALIBRATED);
  m_badChannelData.clear();
}

/*!
 *
 *  \param pCluster pointer to non-modifiable cluster (origin)
 *
 *  The copy constructor for \a CaloCluster creates explicit copies of all
 *  data members, with the exception of the links to the external data
 *  objects. These are only partly copied, meaning the data object itself
 *  is copied (new instance), but the container information is NOT copied.
 *  The new cluster therefore owns the new external data objects until those
 *  are explictely pushed into a container. Then the owenership is transferred
 *  to the container.
 */
CaloCluster::CaloCluster(const CaloCluster* pCluster)
  : CaloCompositeKineBase(),
    CaloCompositeCellBase<CaloClusterNavigable>(),
    m_dataStore(pCluster->m_dataStore),
    m_momentStore(pCluster->m_momentStore),
    m_ownDataStore(true),
    m_basicSignal(pCluster->m_basicSignal),
    m_time(pCluster->m_time),
    m_samplingPattern(pCluster->m_samplingPattern),
    m_barrel(pCluster->m_barrel),
    m_endcap(pCluster->m_endcap),
    m_clusterSize(pCluster->m_clusterSize),
    m_eta0(pCluster->m_eta0),
    m_phi0(pCluster->m_phi0),
    m_timeNorm(pCluster->m_timeNorm),
    m_posNorm(pCluster->m_posNorm),
    m_posSamNorm(pCluster->m_posSamNorm),
    m_nBarrel(pCluster->m_nBarrel),
    m_nEndcap(pCluster->m_nEndcap),
    m_lockPattern(pCluster->m_lockPattern),
    m_rawE(pCluster->m_rawE),
    m_rawEta(pCluster->m_rawEta),
    m_rawPhi(pCluster->m_rawPhi),
    m_rawM(pCluster->m_rawM),
    m_altE(pCluster->m_altE),
    m_altEta(pCluster->m_altEta),
    m_altPhi(pCluster->m_altPhi),
    m_altM(pCluster->m_altM)
{
  signalstate_t s = pCluster->m_defSigState;
  if (!this->hasSignalState(s)) s = statename_t::CALIBRATED;
  this->setDefaultSignalState(s);
  this->setSignalState(s);
  // deep copy external data
  bool isExternalShowerThere = false;
  if ( pCluster->m_dataLink.isValid()) {
   if (*(pCluster->m_dataLink) != nullptr) isExternalShowerThere  = true;
  }
  CaloShower* pData = isExternalShowerThere
    ? new CaloShower(*(pCluster->m_dataLink)) : new CaloShower();
  m_shower = pData;
  CaloCellLink* pLink = pCluster->getCellLink() != nullptr ? new CaloCellLink(pCluster->getCellLink()) : new CaloCellLink();
  this->setStores(pData,pLink,
		  m_ownDataStore);
  P4EEtaPhiM::setE(pCluster->m_e);
  P4EEtaPhiM::setEta(pCluster->m_eta);
  P4EEtaPhiM::setPhi(pCluster->m_phi);
  P4EEtaPhiM::setM(pCluster->m_m);
  this->setRecoStatus(pCluster->getRecoStatus());
  m_badChannelData.reserve(pCluster->m_badChannelData.size());
  for (unsigned int i=0;i<pCluster->m_badChannelData.size();i++) {
    m_badChannelData.push_back(CaloClusterBadChannelData((pCluster->m_badChannelData)[i]));
  }
}

CaloCluster::CaloCluster(const CaloCluster& rCluster)
  : I4Momentum(rCluster),
    P4EEtaPhiMBase(rCluster),
    INavigable(rCluster),
    IAthenaBarCode(rCluster),
    INavigable4Momentum(rCluster),
    CaloCompositeKineBase(rCluster),
    CaloCompositeCellBase<CaloClusterNavigable>(rCluster),
    ISignalState(),
    m_dataStore(rCluster.m_dataStore),
    m_momentStore(rCluster.m_momentStore),
    m_ownDataStore(true),
    m_basicSignal(rCluster.m_basicSignal),
    m_time(rCluster.m_time),
    m_samplingPattern(rCluster.m_samplingPattern),
    m_barrel(rCluster.m_barrel),
    m_endcap(rCluster.m_endcap),
    m_clusterSize(rCluster.m_clusterSize),
    m_eta0(rCluster.m_eta0),
    m_phi0(rCluster.m_phi0),
    m_timeNorm(rCluster.m_posNorm),
    m_posNorm(rCluster.m_posNorm),
    m_posSamNorm(rCluster.m_posSamNorm),
    m_nBarrel(rCluster.m_nBarrel),
    m_nEndcap(rCluster.m_nEndcap),
    m_lockPattern(rCluster.m_lockPattern),
    m_rawE(rCluster.m_rawE),
    m_rawEta(rCluster.m_rawEta),
    m_rawPhi(rCluster.m_rawPhi),
    m_rawM(rCluster.m_rawM),
    m_altE(rCluster.m_altE),
    m_altEta(rCluster.m_altEta),
    m_altPhi(rCluster.m_altPhi),
    m_altM(rCluster.m_altM)
{
  signalstate_t s = rCluster.m_defSigState;
  if (!this->hasSignalState(s)) s = statename_t::CALIBRATED;
  this->setDefaultSignalState(s);
  this->setSignalState(s);
  // deep copy external data
  bool isExternalShowerThere = false;
  if ( rCluster.m_dataLink.isValid()) {
   if (*(rCluster.m_dataLink) != nullptr) isExternalShowerThere  = true;
  }
  CaloShower* pData = isExternalShowerThere
    ? new CaloShower(*(rCluster.m_dataLink)) : new CaloShower();
  m_shower = pData;
  CaloCellLink* pLink = rCluster.getCellLink() != nullptr ? new CaloCellLink(rCluster.getCellLink()) : new CaloCellLink();
  this->setStores(pData,pLink,
		  m_ownDataStore);
  // energy etc may be corrected, so overwrite!
  P4EEtaPhiM::setE(rCluster.m_e);
  P4EEtaPhiM::setEta(rCluster.m_eta);
  P4EEtaPhiM::setPhi(rCluster.m_phi);
  P4EEtaPhiM::setM(rCluster.m_m);
  this->setRecoStatus(rCluster.getRecoStatus());
  m_badChannelData.reserve(rCluster.m_badChannelData.size());
  for (unsigned int i=0;i<rCluster.m_badChannelData.size();i++) {
    m_badChannelData.push_back(CaloClusterBadChannelData((rCluster.m_badChannelData)[i]));
  }
}

////////////////
// Destructor //
////////////////

/*!
 *
 * The cluster destructs the associated external data objects \a CaloCellLink
 * and \a CaloShower as long as it owns them. Ownership is lost once these
 * objects have been pushed into their corresponding storable collection. This
 * is checked by the cluster on destruction.
 */
CaloCluster::~CaloCluster()
{
  //if ( m_ownDataStore && *m_dataLink !=  0 ) delete *m_dataLink;
  if ( m_ownDataStore && m_dataLink.isValid() ) delete *m_dataLink;
}


/*!
 *
 *  \param theCell pointer to a \a CaloCell
 *  \param weight  signal weight of cell in cluster
 *
 *  Present implementation performs full four-vector update for each
 *  cell added.
 */
void CaloCluster::updateKine(const CaloCell* theCell, double weight)
{

  // update global kinematics
  //
  // for the update of the position the normalization is not a trival
  // thing. The previous implementation used the sum of weighted
  // energies as normalization. This leads to unphysical eta and phi
  // values in case negative energies are added in. The new algorithm
  // takes therefore |E| instead of E which gives the same eta and phi
  // as before for the 2 cases where all cells are negative or all
  // cells are positive. In the mixed case it will give the direction
  // of activity in the calorimeter.

  if (!theCell->caloDDE()) return;

  double theEnergyNoWeight = theCell->e();
  double theEnergy    = weight * theEnergyNoWeight;
  double cellEta = theCell->eta();
  double cellPhi = theCell->phi();
  // weight might be negative for removing cells
  // therefore the AbsEnergy can be negative ...
  double theAbsEnergy = weight * fabs(theEnergyNoWeight);
  double thePhi       = proxim(cellPhi,this->phi());
  double theNewEta    =
    m_posNorm * this->eta() + theAbsEnergy * cellEta;
  double theNewPhi    = m_posNorm * this->phi() + theAbsEnergy * thePhi;
  double theNewEnergy = this->e() + theEnergy;
  m_posNorm += theAbsEnergy;

  if ( m_posNorm != 0. )
    {
      this->setEta(theNewEta/m_posNorm);
      this->setPhi(CaloPhiRange::fix (theNewPhi/m_posNorm));
    }
  else
    {
      this->setEta(0);
      this->setPhi(0);
    }

  this->setE(theNewEnergy);

  // calculate the cluster time from cells which provide a time
  // two modes are under discussion - either use an energy^2 weighted
  // average (sigma_t ~ 1/E) or just the time of the leading cell
  // for now the energy^2 weighted average is implemented ...

  // need sampling number already for time
  CaloSampling::CaloSample sam = theCell->caloDDE()->getSampling();
  // unknown sampling
  if ( sam == CaloSampling::Unknown ) return;

  unsigned pmask = 0;
  if (sam != CaloSampling::PreSamplerB &&
      sam != CaloSampling::PreSamplerE)
  {
    if ( theCell->caloDDE()->is_tile() )
      pmask = 0x8080;
    else
      pmask = 0x2000;
  }

  // Is time defined?
  if ( theCell->provenance() & pmask ) {
    // keep the sign of weight for the time norm in case a cell is removed
    double theTimeNorm = fabs(weight) * theEnergy * theEnergyNoWeight;

    double theNewTime = m_timeNorm * this->getTime()
      + theTimeNorm * theCell->time();

    m_timeNorm += theTimeNorm;

    if ( m_timeNorm != 0. )
      {
	this->setTime(theNewTime/m_timeNorm);
      }
    else
      {
	this->setTime(0);
      }
  }

  /////////////////////////////////////
  // Check Sampling Variable Updates //
  /////////////////////////////////////

  // no update required
  if ( this->allLocked() ) return;

  // update sampling quantities

  // This has gotten a bit more involved. To handle the stores for the
  // data correctly, a pointer to each store (can be different for each
  // variable type) has to be retrieved first (internal function). As all
  // variables need the sampling energy, we start with this store. Note that
  // the store for each variable can be defined by calling the corresponding
  // set method before the first \a updateKine invocation.

  ////////////////////////
  // Energy in Sampling //
  ////////////////////////

  CaloVariableType::VariableType typeE = CaloVariableType::ENERGY;
  if ( ! this->isLocked(typeE) )
    {
      CaloSamplingData* pEStore = this->getDataStore(typeE,true);
      if ( pEStore != nullptr )
	{
	  // note that this is a reference to a modifiable data word!
	  pEStore->retrieveData(typeE,sam) += theEnergy;
	}
    }

  double theOldNorm(m_posSamNorm[sam]);
  m_posSamNorm[sam]+=theAbsEnergy;
  double theNewNorm(m_posSamNorm[sam]);

  /////////////
  // Azimuth //
  /////////////

  // azimuth
  CaloVariableType::VariableType typePhi = CaloVariableType::PHI;
  if ( ! this->isLocked(typePhi) )
    {
      // central azimuth
      CaloSamplingData* pPhiStore = this->getDataStore(typePhi,true);
      if ( pPhiStore != nullptr )
	{
	  double& refPhiSamp = pPhiStore->retrieveData(typePhi,sam);
	  thePhi     = proxim(cellPhi,refPhiSamp);
	  theNewPhi  = theNewNorm != 0.
	    ? CaloPhiRange::fix((theOldNorm*refPhiSamp+theAbsEnergy*thePhi)/theNewNorm)
	    : double(0);

	  // spread in azimuth
	  CaloVariableType::VariableType typeDPhi =
	    CaloVariableType::DELTA_PHI;
	  if ( ! this->isLocked(typeDPhi) )
	    {
	      CaloSamplingData* pDPhiStore = this->getDataStore(typeDPhi,true);
	      if ( pDPhiStore != nullptr )
		{
		  // non-const reference -> modification in place!
		  double& refDPhiSamp = pDPhiStore->retrieveData(typeDPhi,sam);
		  double wP2
		    = theOldNorm *
		    ( refDPhiSamp*refDPhiSamp + refPhiSamp*refPhiSamp)
		    + theAbsEnergy * thePhi * thePhi;
		  refDPhiSamp = theNewNorm != 0.
		    ? wP2 / theNewNorm - theNewPhi*theNewPhi
		    : double(0);
		  if ( refDPhiSamp > 0. )
		    {
		      refDPhiSamp = sqrt(refDPhiSamp);
		    }
		  else
		    {
		      refDPhiSamp = double(0);
		    }
		}
	    }
	  // save the new phi value
	  refPhiSamp = theNewPhi;
	}
    }

  //////////////
  // Rapidity //
  //////////////


  // rapidity
  CaloVariableType::VariableType typeEta = CaloVariableType::ETA;
  if ( ! this->isLocked(typeEta) )
    {
      CaloSamplingData* pEtaStore = this->getDataStore(typeEta,true);
      if ( pEtaStore != nullptr )
	{
	  double& refEtaSamp = pEtaStore->retrieveData(typeEta,sam);
	  double  theNewEta  = theNewNorm != 0.
	    ? (theOldNorm*refEtaSamp+theAbsEnergy* cellEta)/theNewNorm
	    : double(0);

	  // spread in eta
	  CaloVariableType::VariableType typeDEta =
	    CaloVariableType::DELTA_ETA;
	  if ( ! this->isLocked(typeDEta) )
	    {
	      CaloSamplingData* pDEtaStore = this->getDataStore(typeDEta,true);
	      if ( pDEtaStore != nullptr )
		{
		  double& refDEtaSamp = pDEtaStore->retrieveData(typeDEta,sam);
		  double wE2 = theOldNorm * (refDEtaSamp * refDEtaSamp +
					     refEtaSamp  * refEtaSamp )
		    + theAbsEnergy * cellEta * cellEta;
		  refDEtaSamp = theNewNorm != 0
		    ? wE2 / theNewNorm - theNewEta * theNewEta
		    : double(0);
		  if ( refDEtaSamp > 0. )
		    {
		      refDEtaSamp = sqrt(refDEtaSamp);
		    }
		  else
		    {
		      refDEtaSamp = double(0.0);
		    }
		}
	    }
	  // save the new eta value
	  refEtaSamp = theNewEta;
	}
    }

  /////////////////////////
  // Maximum Energy Cell //
  /////////////////////////

  CaloVariableType::VariableType typeEMax = CaloVariableType::MAX_ENERGY;
  if ( ! this->isLocked(typeEMax) )
    {
      CaloSamplingData* pEMaxStore = this->getDataStore(typeEMax,true);
      if ( pEMaxStore != nullptr )
	{
	  double& refEMaxSamp = pEMaxStore->retrieveData(typeEMax,sam);
	  if ( refEMaxSamp < theEnergy )
	    {
	      // set new maximum energy
	      refEMaxSamp = theEnergy;
	      // set new eta
	      CaloVariableType::VariableType typeEtaMax =
		CaloVariableType::MAX_ETA;
	      if ( ! this->isLocked(typeEtaMax) )
		{
		  CaloSamplingData* pEtaMaxStore =
		    this->getDataStore(typeEtaMax,true);
		  if ( pEtaMaxStore != nullptr )
		    {
		      pEtaMaxStore->retrieveData(typeEtaMax,sam) =
			cellEta;
		    }
		}
	      // set new phi
	      CaloVariableType::VariableType typePhiMax =
		CaloVariableType::MAX_PHI;
	      if ( ! this->isLocked(typePhiMax) )
		{
		  CaloSamplingData* pPhiMaxStore =
		    this->getDataStore(typePhiMax,true);
		  if ( pPhiMaxStore != nullptr )
		    {
		      pPhiMaxStore->retrieveData(typePhiMax,sam) = thePhi;
		    }
		} // lock on phi of maximum sampling signal
	    } // maximum condition
	} // data store pointer
    } // lock on max energy

  if ( theEnergy != 0)
    {
      // check sampling bit
      m_samplingPattern =
	m_samplingPattern | CaloSamplingHelper::getSamplingBit(sam);
      // count cells in endcap

      switch(sam)
	{
	case CaloSampling::PreSamplerE:
	case CaloSampling::EME1:
	case CaloSampling::EME2:
	case CaloSampling::EME3:
	case CaloSampling::HEC0:
	case CaloSampling::HEC1:
	case CaloSampling::HEC2:
	case CaloSampling::HEC3:
	case CaloSampling::FCAL0:
	case CaloSampling::FCAL1:
	case CaloSampling::FCAL2:
	  m_nEndcap += (weight<0?-1:1);
	  break;
	default:
	  m_nBarrel += (weight<0?-1:1);
	  break;
	}
    }

  if ( m_nBarrel < 0 ) m_nBarrel = 0;
  if ( m_nEndcap < 0 ) m_nEndcap = 0;

  m_barrel = (m_nBarrel>0);
  m_endcap = (m_nEndcap>0);

  return;
}


namespace {


// This is factored out in order to allow instantiating the cell loop
// in versions that both use and do not use cell weights.  In the no-weights
// case, the weight is a compile-time constant of 1, so all operations
// involving it will be optimized away.


struct CellAccum
{
  CellAccum (double& the_posNorm,
             std::vector<double>& the_posSamNorm,
             int& the_nBarrel,
             int& the_nEndcap,
             double& the_timeNorm)
    : EnergyInSample(),
      EtaInSample(),
      PhiInSample(),
      MaxEnergyInSample(),
      EtaMaxEnergyInSample(),
      PhiMaxEnergyInSample(),
      PresenceInSample(),
      theNewEnergy(0),
      theNewTime(0),
      theNewEta(0),
      theNewPhi(0),
      phi0(-999),
      posNorm (the_posNorm),
      posSamNorm (the_posSamNorm),
      nBarrel (the_nBarrel),
      nEndcap (the_nEndcap),
      timeNorm (the_timeNorm)
  {}
  double EnergyInSample[CaloSampling::Unknown];
  double EtaInSample[CaloSampling::Unknown];
  double PhiInSample[CaloSampling::Unknown];
  double MaxEnergyInSample[CaloSampling::Unknown];
  double EtaMaxEnergyInSample[CaloSampling::Unknown];
  double PhiMaxEnergyInSample[CaloSampling::Unknown];
  bool PresenceInSample[CaloSampling::Unknown];
  double theNewEnergy;
  double theNewTime;
  double theNewEta;
  double theNewPhi;
  double phi0;

  double& posNorm;
  std::vector<double>& posSamNorm;
  int& nBarrel;
  int& nEndcap;
  double& timeNorm;
};


struct AccumWeight
{
  AccumWeight (const CaloCluster& cl) : m_cl (cl) {}

  double operator() (const CaloCell& cell) const
  {
    return m_cl.getCellWeight (&cell);
  }

  const CaloCluster& m_cl;
};


struct AccumNoWeight
{
  double operator() (const CaloCell&) const { return 1; }
};


template <class WEIGHT>
inline
void accumCell (const CaloCell& cell, CellAccum& accum, const WEIGHT& w)
{
  const CaloDetDescrElement* dde = cell.caloDDE();
  if (!dde) return;
  double weight = w (cell);
  double fabsweight = fabs (weight);

  double theEnergyNoWeight = cell.e();
  double theEnergy    = weight * theEnergyNoWeight;
  // weight might be negative for removing cells
  // therefore the AbsEnergy can be negative ...
  double theAbsEnergy = weight * fabs(theEnergyNoWeight);

  CaloSampling::CaloSample sam = dde->getSampling();
  accum.PresenceInSample[sam] = true;
  double cellEta = dde->eta();
  double cellPhi = dde->phi();

  accum.EnergyInSample[sam] += theEnergy;
  accum.theNewEnergy += theEnergy;
  double thePhi;
  if (accum.phi0 < -900) {
    thePhi = accum.phi0 = cellPhi;
  }
  else
    thePhi = proxim (cellPhi, accum.phi0);

  accum.theNewEta    += theAbsEnergy * cellEta;
  accum.theNewPhi    += theAbsEnergy * thePhi;
  if ( accum.MaxEnergyInSample[sam] < theEnergy ) {
    accum.MaxEnergyInSample[sam] = theEnergy;
    accum.EtaMaxEnergyInSample[sam] = cellEta;
    accum.PhiMaxEnergyInSample[sam] = cellPhi;
  }

  accum.posSamNorm[sam] += theAbsEnergy;
  accum.posNorm += theAbsEnergy;

  accum.EtaInSample[sam] += theAbsEnergy * cellEta;
  accum.PhiInSample[sam] += theAbsEnergy * thePhi;

  if ( theEnergy != 0)
  {
    // count cells in endcap
    switch(sam)
    {
    case CaloSampling::PreSamplerE:
    case CaloSampling::EME1:
    case CaloSampling::EME2:
    case CaloSampling::EME3:
    case CaloSampling::HEC0:
    case CaloSampling::HEC1:
    case CaloSampling::HEC2:
    case CaloSampling::HEC3:
    case CaloSampling::FCAL0:
    case CaloSampling::FCAL1:
    case CaloSampling::FCAL2:
      accum.nEndcap += (weight<0?-1:1);
      break;
    case CaloSampling::EMB1:
    case CaloSampling::EMB2:
    default:
      accum.nBarrel += (weight<0?-1:1);
      break;
    }

    if (sam != CaloSampling::PreSamplerB &&
        sam != CaloSampling::PreSamplerE)
    {
      unsigned int pmask = dde->is_tile() ? 0x8080 : 0x2000;

      // Is time defined?
      if ( cell.provenance() & pmask ) {
        // keep the sign of weight for the time norm in case a cell is removed
        double theTimeNorm = fabsweight * theEnergy * theEnergyNoWeight;
        accum.theNewTime += theTimeNorm * cell.time();
        accum.timeNorm += theTimeNorm;
      }
    }
  }
}


template <class WEIGHT>
inline
void accumCells (CaloCluster& cl, CellAccum& accum, const WEIGHT& w)
{
  for (const CaloCell* c : cl)
    CxxUtils::prefetchObj (c);

  CaloCluster::cell_iterator it = cl.begin();
  CaloCluster::cell_iterator end = cl.end();
  for (; it != end; ++it) {
    CaloPrefetch::nextDDE(it, end);
    accumCell (**it, accum, w);
  }
}


} // anonymous namespace


void CaloCluster::calculateKine(const bool useweight,const bool updateLayers)
{
  //static CaloPhiRange range;

  // update global kinematics
  //
  // for the update of the position the normalization is not a trival
  // thing. The previous implementation used the sum of weighted
  // energies as normalization. This leads to unphysical eta and phi
  // values in case negative energies are added in. The new algorithm
  // takes therefore |E| instead of E which gives the same eta and phi
  // as before for the 2 cases where all cells are negative or all
  // cells are positive. In the mixed case it will give the direction
  // of activity in the calorimeter.

  m_posNorm = 0;
  m_nBarrel = 0;
  m_nEndcap = 0;
  m_timeNorm = 0.;

  std::fill (m_posSamNorm.begin(), m_posSamNorm.end(), 0);

  CellAccum accum (m_posNorm, m_posSamNorm, m_nBarrel, m_nEndcap, m_timeNorm);
  accum.theNewPhi = this->phi();
  if (useweight)
    accumCells (*this, accum, AccumWeight(this));
  else
    accumCells (*this, accum, AccumNoWeight());

  if ( m_posNorm != 0. )
  {
    double inorm = 1 / m_posNorm;
    this->setEta(accum.theNewEta * inorm);
    this->setPhi(CaloPhiRange::fix(accum.theNewPhi * inorm));
  }
  else
  {
    this->setEta(0);
    this->setPhi(0);
  }

  this->setE(accum.theNewEnergy);

  if ( m_timeNorm != 0. )
    this->setTime(accum.theNewTime/m_timeNorm);
  else
    this->setTime(0);


  /////////////////////////////////////
  // Check Sampling Variable Updates //
  /////////////////////////////////////

  if ( !updateLayers ) return;
  if ( this->allLocked() ) return;
  // update sampling quantities

  for(int i=0;i<(int)CaloSampling::Unknown;i++) {
    if ( !accum.PresenceInSample[i] ) continue;
    // check sampling bit
    CaloSampling::CaloSample sam = (CaloSampling::CaloSample)i;
    m_samplingPattern |= CaloSamplingHelper::getSamplingBit(sam);
    if ( m_posSamNorm[i] != 0 ) {
      double inorm = 1 / m_posSamNorm[i];
      accum.EtaInSample[i] *= inorm;
      accum.PhiInSample[i] = CaloPhiRange::fix (accum.PhiInSample[i] * inorm);
    }
  }

#define SETVAR(v, a) maybeSetVariable(v, a, (a)+sizeof(a)/sizeof((a)[0]))
  SETVAR (CaloVariableType::ENERGY,     accum.EnergyInSample);
  SETVAR (CaloVariableType::ETA,        accum.EtaInSample);
  SETVAR (CaloVariableType::PHI,        accum.PhiInSample);
  SETVAR (CaloVariableType::MAX_ENERGY, accum.MaxEnergyInSample);
  SETVAR (CaloVariableType::MAX_ETA,    accum.EtaMaxEnergyInSample);
  SETVAR (CaloVariableType::MAX_PHI,    accum.PhiMaxEnergyInSample);
#undef SETVAR

  if ( m_nBarrel < 0 ) m_nBarrel = 0;
  if ( m_nEndcap < 0 ) m_nEndcap = 0;

  m_barrel = (m_nBarrel>0);
  m_endcap = (m_nEndcap>0);
  return;
}

///////////////////////////
// Basic Data Management //
///////////////////////////

/*!
 *   \param varType    variable type indicator
 *   \param samType    sampling type indicator
 *   \param useLink  extends checks to external data store only if
 *                     <tt>true</tt>
 *
 *   This method first checks if the requested data is in the \a CaloCluster
 *   cache. If not, it tries to retrieve the data from the linked data
 *   store if the \a traceLink flag is set to <tt>true</tt>. This allows
 *   full control of the data source by the client.
 */
double
CaloCluster::getVariable(const variable_type& varType,
			 const sampling_type& samType,
			 bool                 useLink ) const
{
  const CaloSamplingData* pDataStore = this->getDataStore(varType,useLink);
  return pDataStore != nullptr
    ? pDataStore->retrieveData(varType,samType)
    : double(0);
}

/*!
 *
 * \overload
 *
 * \param  varType   variable type
 * \param  varList   reference to modifiable list of variable values
 * \param  useLink extends checks to external data store if
 *                   <tt>true</tt>
 */
bool
CaloCluster::getVariable(const variable_type& varType,
			 std::vector<double>& varList,
			 bool                 useLink) const
{
  const CaloSamplingData* pDataStore = this->getDataStore(varType,useLink);
  return pDataStore != nullptr ? pDataStore->retrieveData(varType,varList) : false;
}

/*!
 *  \param varType  variable type
 *  \param samType  sampling indicator
 *  \param varData  data value
 *  \param useLink   if <tt>true</tt> data is stored in external store
 */
bool
CaloCluster::setVariable(const variable_type& varType,
			 const sampling_type& samType,
			 const double&        varData,
			 bool                 useLink)
{
  // check samplingpattern
  if(!this->hasSampling(samType)){
    //    std::cout << " Cluster does not have sampling " << samType << std::endl;
    m_samplingPattern =
      m_samplingPattern | CaloSamplingHelper::getSamplingBit(samType);
  }

  // allocate modifiable data store
  CaloSamplingData* pDataStore = this->getDataStore(varType,useLink);
  return pDataStore != nullptr
    ? pDataStore->storeData(varType,samType,varData)
    : false;
}

bool
CaloCluster::setVariable(const variable_type&           varType,
			 const std::vector<double>& varList,
			 bool                           useLink)
{
  CaloSamplingData* pDataStore = this->getDataStore(varType,useLink);
  return pDataStore != nullptr
    ? pDataStore->storeData(varType,varList)
    : false;
}

/////////////////////////////////////////
// Retrieve and Set Sampling Variables //
/////////////////////////////////////////

// most methods are inlined!
void
CaloCluster::setenergymax(sampling_type sampling, double m)
{
  if ( this->isEMSampling(sampling) )
    this->setVariable(CaloVariableType::MAX_ENERGY,sampling,m,true);
}

void CaloCluster::setetamax(sampling_type sampling, double m)
{
  if ( this->isEMSampling(sampling) )
    this->setVariable(CaloVariableType::MAX_ETA,sampling,m,true);
}

void CaloCluster::setphimax(sampling_type sampling, double m)
{
  if ( this->isEMSampling(sampling) )
    this->setVariable(CaloVariableType::MAX_PHI,sampling,m,true);
}

/*!
 *  \param sampling calorimeter sampling indicator
 *  \param size     input \f$ \Delta\eta \f$ cluster size
 */
void CaloCluster::setetasize(sampling_type sampling, double size)
{
  if ( this->isEMSampling(sampling) )
    this->setVariable(CaloVariableType::DELTA_ETA,sampling,size,true);
}

/*!
 *  \param sampling calorimeter sampling indicator
 *  \param size     input \f$ \Delta\varphi \f$ cluster size
 */
void CaloCluster::setphisize(sampling_type sampling, double size)
{
  if ( this->isEMSampling(sampling) )
    this->setVariable(CaloVariableType::DELTA_PHI,sampling,size,true);
}
/*!
 *
 *  Rewrite \f$ e_{0} \varphi_{0} + e_{1} \varphi_{1} \f$ as
 *  \f$ \left( e_{0} + e_{1} \right) \varphi_{0} +
 *      e_{1} \left( \varphi_{1} - \varphi_{0} \right) \f$ for
 *  \f$ \varphi \f$ calculation.
 */
double
CaloCluster::variableBE(const variable_type& varType,
			const int& sam) const
{
  // check input
  unsigned int iSamp((unsigned int)sam);
  if ( iSamp > 3 ) return m_errorValue;

  // barrel and endcap
  if ( this->inBarrel() && this->inEndcap() )
    {
      // offsets for sampling indicators
      unsigned int bOff = (unsigned int)CaloSampling::PreSamplerB;
      unsigned int eOff = (unsigned int)CaloSampling::PreSamplerE;
      CaloSampling::CaloSample bSample =
	(CaloSampling::CaloSample)(bOff+iSamp);
      CaloSampling::CaloSample eSample =
	(CaloSampling::CaloSample)(eOff+iSamp);
      double eBarrel = this->getVariable(CaloVariableType::ENERGY,
					     bSample,true);
      double eEndCap = this->getVariable(CaloVariableType::ENERGY,
					     eSample,true);

      double vBarrel = this->getVariable(varType,bSample,true);
      double vEndCap = this->getVariable(varType,eSample,true);

      // Deal with invalid eta/phi values.
      if (varType == CaloVariableType::PHI ||
          varType == CaloVariableType::ETA)
      {
        if (vBarrel <= m_errorValue) {
          vBarrel = 0;
          eBarrel = 0;
        }
        if (vEndCap <= m_errorValue) {
          vEndCap = 0;
          eEndCap = 0;
        }
      }

      // invalid signals
      double eSum = eBarrel + eEndCap;
      if ( eSum == 0. ) return m_errorValue;
      // average
      double vAverage(m_errorValue);
      switch ( varType )
	{
	case CaloVariableType::PHI:
	  {
	    double phiSum =
	      eSum * vBarrel + eEndCap * CaloPhiRange::diff(vEndCap,vBarrel);
	    vAverage = CaloPhiRange::fix(phiSum/eSum);
	    break;
	  }
	case CaloVariableType::ETA:
	  {
	    vAverage = ( eBarrel * vBarrel + eEndCap * vEndCap ) / eSum;
	    break;
	  }
	case CaloVariableType::ENERGY:
	  {
	    vAverage = eSum;
	    break;
	  }
	default:
	  {
	    break;
	  }
	}
      return vAverage;
    }

  // barrel only
  if ( this->inBarrel() )
    {
      unsigned int bOff = (unsigned int)CaloSampling::PreSamplerB;
      CaloSampling::CaloSample bSample =
	(CaloSampling::CaloSample)(bOff+iSamp);
      return this->getVariable(varType,bSample);
    }

  // endcap only
  if ( this->inEndcap() )
    {
      unsigned int eOff = (unsigned int)CaloSampling::PreSamplerE;
      CaloSampling::CaloSample eSample =
	(CaloSampling::CaloSample)(eOff+iSamp);
      return this->getVariable(varType,eSample);
    }

  // just in case
  return m_errorValue;
}

/*!
 *
 * \param  varType    reference to non-modifiable variable type indicator
 * \param  traceLink  flag allows tracing external storelinks if set
 *                    <tt>true</tt>
 *
 * Dynamically allocates the correct store (local cache or external) depending
 * on availability and indicator. The store is non-modifiable.
 */
const CaloSamplingData*
CaloCluster::getDataStore(const variable_type& varType,bool traceLink) const
{
  //
  //DEBUGstd::cout << "[CaloCluster@" << this
	//DEBUG    << "]::getDataStore - const data store pointer requested, "
	//DEBUG    << "try internal pointer = " << &m_dataStore;

  // check if variable in local data store
  if ( m_dataStore.contains(varType) )
    {
      //      std::cout << "...successful!" << std::endl;
      return &m_dataStore;
    }

  // check linked data store if requested
  if ( traceLink && m_dataLink.isValid())
    {
  //DEBUG    std::cout << "... unsuccessful, use linked pointer "
  //DEBUG    		<< &((*m_dataLink)->getSamplingStore()) << std::endl;
      return &((*m_dataLink)->getSamplingStore());
    }

  //DEBUGstd::cout << "... failed, no external or internal pointer available!"
	//DEBUG    << std::endl;
  return (const CaloSamplingData*)nullptr;
}

/*!
 *
 * \param  varType    reference to non-modifiable variable type indicator
 * \param  traceLink  if <tt>true</tt> return pointer to external store
 *
 * Dynamically allocates the correct store (local cache or external) depending
 * on availability and indicator. The store is modifiable, i.e. write access
 * is granted.
 */
CaloSamplingData*
CaloCluster::getDataStore(const variable_type& varType,bool traceLink)
{
  if ( m_dataStore.contains(varType) )
    {
      return &m_dataStore;
    }

  // check external store if requested
  if ( m_ownDataStore && !m_dataLink.isValid()) {
    m_shower = new CaloShower();
    m_dataLink.setElement(m_shower);
  }

  // trace
  if ( ( traceLink || m_ownDataStore ) && m_shower )
    {
      return &(m_shower->getSamplingStore());
    }

  return (CaloSamplingData*)nullptr;
}

/////////////
// Moments //
/////////////

CaloCluster::moment_iterator CaloCluster::beginMoment(bool useLink) const
{
  moment_iterator iMom;
  // get store pointers
  const CaloClusterMomentStore* pFirstMomStore = nullptr;
  const CaloClusterMomentStore* pSecndMomStore = nullptr;
  // try to return valid iterator to first data object...
  if ( this->getMomentStorePtrs(pFirstMomStore,pSecndMomStore,
				useLink) ) //DEBUG,"begin") )
    {
      iMom = moment_iterator(pFirstMomStore->begin(),pFirstMomStore,
			     pSecndMomStore);
    }
  else
    {
      iMom = moment_iterator();
    }

  return iMom;
}


CaloCluster::moment_iterator CaloCluster::endMoment(bool useLink) const
{
  const CaloClusterMomentStore* pFirstMomStore = nullptr;
  const CaloClusterMomentStore* pSecndMomStore = nullptr;
  if ( this->getMomentStorePtrs(pFirstMomStore,pSecndMomStore,useLink) )
                                //DEBUG,"end") )
    {
      CaloClusterMomentStore::moment_iterator lMom =
	pSecndMomStore != nullptr && pSecndMomStore->size() > 0
	? pSecndMomStore->end()
	: pFirstMomStore->end();
      return moment_iterator(lMom,pFirstMomStore,pSecndMomStore);
    }
  else
    {
      return moment_iterator();
    }
}

const CaloClusterMomentStore* CaloCluster::getMomentStore(bool useLink) const
{
  // follow link policy
  if ( !useLink ) return &m_momentStore;

  // check on external link usage
  if ( ( m_dataLink.isValid() || m_ownDataStore ) && m_dataLink.isValid() )
    return &((*m_dataLink)->getMomentStore());

  return (const CaloClusterMomentStore*)nullptr;
}

CaloClusterMomentStore* CaloCluster::getMomentStore(bool useLink)
{
  // follow link policy: do not use link!
  if ( !useLink ) return &m_momentStore;

  // follow link policy: use link!
  if ( m_ownDataStore && m_dataLink.isValid() == 0 )
    {
      m_shower = new CaloShower();
      m_dataLink.setElement(m_shower);
      return this->getMomentStore(useLink);
    }

  if ( m_shower || m_ownDataStore )
    {
      return &(m_shower->getMomentStore());
    }
  return (CaloClusterMomentStore*)nullptr;
}

CaloClusterMomentStore* CaloCluster::getMomentStore(const moment_type& momType,
						    bool  useLink)
{
  // this is the "write" implementation!
  return m_momentStore.contains(momType)
    ? &m_momentStore                 // moment already in local store
    : this->getMomentStore(useLink); // retrieve local or external
}

const CaloClusterMomentStore*
CaloCluster::getMomentStore(const moment_type& momType, bool  useLink) const
{
  // local store
  if ( m_momentStore.contains(momType) ) return &m_momentStore;

  // external store
  const CaloClusterMomentStore* pMomStore = this->getMomentStore(useLink);

  //
  return pMomStore != nullptr && pMomStore->contains(momType)
    ? pMomStore : (const CaloClusterMomentStore*) nullptr;
}

bool
CaloCluster::getMomentStorePtrs(const CaloClusterMomentStore*&
				pFirstMomStore,
				const CaloClusterMomentStore*&
				pSecndMomStore,
				bool useLink) const
//				const std::string& mode) const
{
  // first store is assumed local by default
  pFirstMomStore = this->getMomentStore();
  // second store is assumed NOT to exist by default
  pSecndMomStore = nullptr;
  // local first store does not exist or is empty...
  if ( pFirstMomStore == nullptr || pFirstMomStore->size() == 0 )
    {
      // ...try to make external store the first one!
      pFirstMomStore = this->getMomentStore(useLink);
    }
  // local first store exists...
  else
    {
      // ...check if second store available!
      pSecndMomStore = useLink ? this->getMomentStore(useLink) : nullptr;
    }
  return pFirstMomStore != nullptr;
}

/*!
 *   \param momType  reference to (non-modifiable) moment type indicator
 *   \param momData  reference to (non-modifiable) moment value object
 *   \param useLink  not used.
 */
void CaloCluster::insertMoment(const moment_type&  momType,
			       const moment_value& momData,
			       bool                useLink)
{
  // use momType for getMomentStore to override useLink in case
  // the moment has already been stored and should just be updated ...
  CaloClusterMomentStore* pMomStore = this->getMomentStore(momType,useLink);
  if ( pMomStore != nullptr ) pMomStore->insert(momType,momData);
}
/*!
 *
 *   \param momType  reference to (non-modifiable) moment type indicator
 *   \param momData reference to (modifiable) moment value object
 *   \param  useLink extends checks to external data store if
 *                   <tt>true</tt>
 *
 *   Returns <tt>true</tt> if requested moment type in cluster.
 */
bool
CaloCluster::retrieveMoment(const moment_type& momType,
			    moment_value&      momData,
			    bool               useLink) const
{
  const CaloClusterMomentStore* pMomStore = this->getMomentStore(momType,
								 useLink);
  return pMomStore != nullptr
    ? pMomStore->retrieve(momType,momData)
    : false;
}

bool
CaloCluster::retrieveMoment(const moment_type& momType,
			    double&            momValue,
			    bool               useLink) const
{
  CaloClusterMoment momData(0);
  if ( this->retrieveMoment(momType,momData,useLink) )
    {
      momValue = momData.getValue();
      return true;
    }
  return false;
}


// lock variable
bool CaloCluster::lockVariable(const variable_type& varType)
{
  unsigned int bitPattern(CaloVariableType::getVariableBit(varType));
  m_lockPattern = ( m_lockPattern | bitPattern );
  return this->isLocked(bitPattern);
}

// unlock variable
bool CaloCluster::unlockVariable(const variable_type& varType)
{
  unsigned int bitPattern(CaloVariableType::getVariableBit(varType));
  if ( this->isLocked(bitPattern) )
    {
      m_lockPattern = m_lockPattern ^ bitPattern;
      return ! this->isLocked(bitPattern);
    }
  else
    {
      return false;
    }
}

double CaloCluster::getRawE()   const { return m_rawE; }
double CaloCluster::getRawEta() const { return m_rawEta; }
double CaloCluster::getRawPhi() const { return m_rawPhi; }
double CaloCluster::getRawM()   const { return m_rawM; }

void CaloCluster::setRawE(double e)     { m_rawE   = e; }
void CaloCluster::setRawEta(double eta) { m_rawEta = eta; }
void CaloCluster::setRawPhi(double phi) { m_rawPhi = phi; }
void CaloCluster::setRawM(double m)     { m_rawM   = m; }

double CaloCluster::getAltE()   const { return m_altE; }
double CaloCluster::getAltEta() const { return m_altEta; }
double CaloCluster::getAltPhi() const { return m_altPhi; }
double CaloCluster::getAltM()   const { return m_altM; }

void CaloCluster::setAltE(double e)     { m_altE   = e; }
void CaloCluster::setAltEta(double eta) { m_altEta = eta; }
void CaloCluster::setAltPhi(double phi) { m_altPhi = phi; }
void CaloCluster::setAltM(double m)     { m_altM   = m; }

double CaloCluster::getCalE()   const   { return P4EEtaPhiM::e(); }
double CaloCluster::getCalEta() const   { return P4EEtaPhiM::eta(); }
double CaloCluster::getCalPhi() const   { return P4EEtaPhiM::phi(); }
double CaloCluster::getCalM()   const   { return P4EEtaPhiM::m(); }

void CaloCluster::setCalE(double e)     { P4EEtaPhiM::setE(e); }
void CaloCluster::setCalEta(double eta) { P4EEtaPhiM::setEta(eta); }
void CaloCluster::setCalPhi(double phi) { P4EEtaPhiM::setPhi(phi); }
void CaloCluster::setCalM(double m)     { P4EEtaPhiM::setM(m); }

bool CaloCluster::setSignalState(signalstate_t s)
{
  if ( !this->hasSignalState(s) ) return false;

  m_signalState = s;

  return
    s == statename_t::CALIBRATED    ? this->setStateCal() :
    s == statename_t::ALTCALIBRATED ? this->setStateAlt() :
    s == statename_t::UNCALIBRATED  ? this->setStateRaw() :
    false;
}

bool CaloCluster::hasSignalState(signalstate_t s) const
{
  return s == statename_t::CALIBRATED ||
         s == statename_t::UNCALIBRATED ||
         s == statename_t::ALTCALIBRATED;
}

bool CaloCluster::isAtSignalState(signalstate_t s) const
{
  return s == m_signalState;
}

void CaloCluster::resetSignalState()
{
  this->setSignalState(m_defSigState);
}

bool CaloCluster::setStateRaw()
{
  // std::cout << "CaloCluster [" << this
  //	    << "] setStateRaw()" << std::endl;
  m_getE   = &CaloCluster::getRawE;
  m_getEta = &CaloCluster::getRawEta;
  m_getPhi = &CaloCluster::getRawPhi;
  m_getM   = &CaloCluster::getRawM;
  //
  m_setE   = &CaloCluster::setRawE;
  m_setEta = &CaloCluster::setRawEta;
  m_setPhi = &CaloCluster::setRawPhi;
  m_setM   = &CaloCluster::setRawM;
  //
  return true;
}

bool CaloCluster::setStateAlt()
{
  // std::cout << "CaloCluster [" << this
  //	    << "] setStateAlt()" << std::endl;
  m_getE   = &CaloCluster::getAltE;
  m_getEta = &CaloCluster::getAltEta;
  m_getPhi = &CaloCluster::getAltPhi;
  m_getM   = &CaloCluster::getAltM;
  //
  m_setE   = &CaloCluster::setAltE;
  m_setEta = &CaloCluster::setAltEta;
  m_setPhi = &CaloCluster::setAltPhi;
  m_setM   = &CaloCluster::setAltM;
  return true;
}

bool CaloCluster::setStateCal()
{
  // std::cout << "CaloCluster [" << this
  //	    << "] setStateCal()" << std::endl;
  m_getE   = &CaloCluster::getCalE;
  m_getEta = &CaloCluster::getCalEta;
  m_getPhi = &CaloCluster::getCalPhi;
  m_getM   = &CaloCluster::getCalM;
  //
  m_setE   = &CaloCluster::setCalE;
  m_setEta = &CaloCluster::setCalEta;
  m_setPhi = &CaloCluster::setCalPhi;
  m_setM   = &CaloCluster::setCalM;
  return true;
}

void CaloCluster::addBadChannel(const CaloClusterBadChannelData& badChannel)
{
 m_badChannelData.push_back(badChannel);
}

void CaloCluster::resetBadChannel()
{
 m_badChannelData.clear();
}

const CaloCluster::badChannelList* CaloCluster::getBadChannel() const
{
 return &m_badChannelData;
}

unsigned int CaloCluster::getClusterEtaSize() const{

  unsigned int size = 0;
  if(m_clusterSize==SW_55ele ||
     m_clusterSize==SW_55gam ||
     m_clusterSize==SW_55Econv){
    size = 5;
  }else if(m_clusterSize==SW_35ele ||
	   m_clusterSize==SW_37ele ||
	   m_clusterSize==SW_35gam ||
	   m_clusterSize==SW_37gam ||
	   m_clusterSize==SW_35Econv ||
	   m_clusterSize==SW_37Econv){
    size = 3;
  }else if(m_clusterSize==SW_7_11){
    size = 7;
  }

  return size;

}

unsigned int CaloCluster::getClusterPhiSize() const{

  unsigned int size = 0;
  if(m_clusterSize==SW_55ele ||
     m_clusterSize==SW_55gam ||
     m_clusterSize==SW_55Econv ||
     m_clusterSize==SW_35ele ||
     m_clusterSize==SW_35gam ||
     m_clusterSize==SW_35Econv){
    size = 5;
  }else if(
	   m_clusterSize==SW_37ele ||
	   m_clusterSize==SW_37gam ||
	   m_clusterSize==SW_37Econv){
    size = 7;
  }else if(m_clusterSize==SW_7_11){
    size = 11;
  }

  return size;

}

CLHEP::HepLorentzVector
CaloCluster::hlv(CaloCluster::signalstate_t s) const {
  if(hasSignalState(s)){
    switch (s) {
    case statename_t::CALIBRATED: {
      P4EEtaPhiM tmp = *this;
      return tmp.hlv();
    }
    case statename_t::UNCALIBRATED: {
      P4EEtaPhiM tmp (m_rawE, m_rawEta, m_rawPhi, m_rawM);
      return tmp.hlv();
    }
    case statename_t::ALTCALIBRATED: {
      P4EEtaPhiM tmp (m_altE, m_altEta, m_altPhi, m_altM);
      return tmp.hlv();
    }
    default:
      break;
    }
  }
  return this->hlv();
}

unsigned int CaloCluster::samplingPattern() const
{
  return m_samplingPattern;
}


// MomentStoreIter methods moved out-of-line to avoid a cling problem.
// Symptom was that iteration over the moments would enter an infinte
// loop in the ESD_18.0.0 test of CaloAthenaPool.

/*! \brief Default constructor builds unusable iterator */
CaloCluster::MomentStoreIter::MomentStoreIter() : m_iter(), m_firstStore(nullptr), m_secndStore(nullptr) { }
/*! \brief Standard constructor for a useable iterator */
CaloCluster::MomentStoreIter::MomentStoreIter(const moment_iterator_i& iter,
                const moment_store* firstStore,
                                              const moment_store* secndStore/*=0*/)
  : m_iter(iter), m_firstStore(firstStore), m_secndStore(secndStore)
{ }
/*! Destructor */
CaloCluster::MomentStoreIter::~MomentStoreIter() { }

/*! \brief Advance iterator */
CaloCluster::MomentStoreIter CaloCluster::MomentStoreIter::next()
{
  m_iter++;
  if ( m_iter == m_firstStore->end() &&
       ( m_secndStore != nullptr && m_secndStore->size() > 0 ) )
  {
    m_iter = m_secndStore->begin();
  }
  return *this;
}

/*! Step back iterator */
CaloCluster::MomentStoreIter CaloCluster::MomentStoreIter::prev()
{
  if ( m_secndStore != nullptr && m_iter == m_secndStore->begin() )
  {
    m_iter = m_firstStore->end();
  }
  m_iter--;
  return *this;
}

/*! \brief Post-advance operator */
CaloCluster::MomentStoreIter CaloCluster::MomentStoreIter::operator++()    { return this->next(); }
/*! \brief Pre-advance operator */
CaloCluster::MomentStoreIter CaloCluster::MomentStoreIter::operator++(int) { return this->next(); }
/*! \brief Post-step back operator */
CaloCluster::MomentStoreIter CaloCluster::MomentStoreIter::operator--()    { return this->prev(); }
/*! \brief Pre-step back operator */
CaloCluster::MomentStoreIter CaloCluster::MomentStoreIter::operator--(int) { return this->prev(); }

/*! \brief Equality comparator */
bool CaloCluster::MomentStoreIter::operator==(const MomentStoreIter& anOther)
{ return m_iter == anOther.m_iter; }
/*! \brief Inequality comparator */
bool CaloCluster::MomentStoreIter::operator!=(const MomentStoreIter& anOther)
{ return m_iter != anOther.m_iter; }

/*! \brief Operator access to \a CaloClusterMoment */
const CaloClusterMoment& CaloCluster::MomentStoreIter::operator*() const { return *m_iter; }
/*! \brief Function access to \a CaloClusterMoment */
const CaloClusterMoment& CaloCluster::MomentStoreIter::getMoment() const { return *m_iter; }
/*! \brief Function access to moment type */
CaloCluster::moment_type CaloCluster::MomentStoreIter::getMomentType() const { return m_iter.getMomentType(); }
