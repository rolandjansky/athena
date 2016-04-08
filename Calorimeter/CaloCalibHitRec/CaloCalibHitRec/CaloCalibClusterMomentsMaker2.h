/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCALIBCLUSTERMOMENTSMAKER2_H
#define CALOCALIBCLUSTERMOMENTSMAKER2_H
/**
 * @class CaloCalibClusterMomentsMaker2
 * @version \$Id: CaloCalibClusterMomentsMaker2.h,v 1.8 2009-05-18 16:16:48 pospelov Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>, Gennady Pospelov <guennadi.pospelov@cern.ch>
 * @date 17-June-2008
 * @brief Calculate calibration hit based moments for CaloCluster objects using Primary Particle ID
 *
 * This is a CaloClusterCollectionProcessor which can be plugged into
 * a CaloClusterMaker for calculating calibration hit based moments
 * for each CaloCluster. */
class StoreGateSvc; 
class CaloDetDescrManager; 
class CaloDetDescrElement;
class CaloCell_ID;
class CaloDM_ID;
class CaloDmDescrManager;
class McEventCollection;
class TruthParticleContainer;

#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ToolHandle.h" 

#include "CaloRec/CaloClusterCollectionProcessor.h"
//#include "CaloEvent/CaloClusterMoment.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDmDetDescr/CaloDmDescrManager.h"

#include <string>
#include <vector>
#include <set>
#include <map>


class CaloCalibClusterMomentsMaker2: public AthAlgTool, virtual public CaloClusterCollectionProcessor
{
 public:

  /**
   * @brief Class to define range of valid bins in eta x phi plane
   */
  class CalibHitIPhiIEtaRange {
    public:
      char iPhi,iEtaMin,iEtaMax;
  };

  /**
   * @brief Class to store cluster number and weight for calorimeter cells
   */
  class MyCellInfo {
    public:
      MyCellInfo(Identifier id): m_cellid(id) {}
      MyCellInfo(Identifier id, int iClus, double w): m_cellid(id)
      {
        m_ClusWeights.push_back(std::pair<int, double> (iClus, w) );
      }
      void Add(const MyCellInfo *other)
      {
        for(std::vector<std::pair<int, double> >::const_iterator it=other->m_ClusWeights.begin(); it!=other->m_ClusWeights.end(); it++){
          m_ClusWeights.push_back( (*it) );
        }
      }
      bool Equals(MyCellInfo* const& h) const { 
        return (m_cellid == h->m_cellid);
      };
      Identifier m_cellid;
      std::vector<std::pair<int, double> > m_ClusWeights;
  };

  /**
   * @brief Class to sort CellInfo objects
   */
  class MyCellInfoSort {
    public:
      bool operator() ( MyCellInfo* const& p, MyCellInfo* const& q ) const
      {
        return p->m_cellid < q->m_cellid;
      }
  };

  typedef std::set<MyCellInfo *, MyCellInfoSort> m_CellInfoSet_t;

  /**
   * @brief Class to store cluster's calibration energies
   */
  class MyClusInfo {
    public:
      class ClusCalibEnergy {
        public:
          double engTot;
          std::vector<double > engSmp;
          ClusCalibEnergy() : engTot(0) { engSmp.resize(CaloSampling::Unknown+1, 0.0); }
          void Add(double eng, int nsmp)
          {
            engTot += eng;
            engSmp[nsmp] +=eng;
          }
      };

      MyClusInfo() : engCalibOut(0), engCalibDead(0) {
        engCalibDeadInArea.resize(CaloDmDescrArea::DMA_MAX,0.0);
        engCalibParticle.clear();
      }

      void Add(double eng, int nsmp, int pid = 0)
      {
        engCalibIn.Add(eng, nsmp);
        std::map<int, ClusCalibEnergy>::iterator pos = engCalibParticle.find(pid);
        if(pos != engCalibParticle.end()) {
          pos->second.Add(eng, nsmp);
        }else{
          ClusCalibEnergy tmp;
          tmp.Add(eng, nsmp);
          engCalibParticle[pid] = tmp;
        }
      }

      ClusCalibEnergy engCalibIn;
      double engCalibOut;
      double engCalibDead;
      std::vector<double > engCalibDeadInArea;
      std::map<int, ClusCalibEnergy > engCalibParticle;
  };
  typedef std::vector<MyClusInfo *> m_ClusInfo_t;


  /** @brief typedef for a pair to index the enums defined in
    *  CaloClusterMoment with a string. */
  typedef std::pair<std::string,xAOD::CaloCluster::MomentType> 
  moment_name_pair;

  /** @brief vector of pairs defined above.*/
  typedef std::vector<moment_name_pair> moment_name_vector;

  /** @brief set of pairs defined above.*/
  typedef std::set<moment_name_pair> moment_name_set;

  CaloCalibClusterMomentsMaker2(const std::string& type, const std::string& name,
                 const IInterface* parent);

  virtual ~CaloCalibClusterMomentsMaker2();

  using CaloClusterCollectionProcessor::execute;
  StatusCode execute(xAOD::CaloClusterContainer* theClusColl);
  StatusCode initialize();

 private:

  /** 
   * @brief vector holding the input list of names of moments to
   * calculate.
   *
   * This is the list of desired names of moments given in the
   * jobOptions.*/
  std::vector<std::string>  m_momentsNames; 

  /** 
   * @brief vector holding the names of valid moments which can be
   * calculated.
   *
   * Each name has to correspond to one of the enums in
   * CaloClusterMoment. This list is defined in the constructor of
   * CaloCalibClusterMomentsMaker2 and holds the names and enums of
   * moments defined in CaloClusterMoment. The name is used as the key
   * to the enum. */
  moment_name_vector m_validNames;

  /** 
   * @brief set of moments which will be calculated.
   *
   * This set will hold each valid enum indexed with the name if it
   * was found on the input list (m_momentsNames) and in the list of
   * valid moment names (m_validNames). */
  moment_name_set m_validMoments;

  /** 
   * @brief vector holding the list of moment names which go in the first store
   * i.e. they are available on the AOD.
   *
   * Only moments listed in this property can later be directly retrieved from
   * AOD - the others are available in the ESD ... */
  std::vector<std::string>  m_momentsNamesAOD; 

  /** 
   * @brief set holding the list of moment enums which go in the first store
   * i.e. they are available on the AOD.
   *
   * Only moments listed in this property can later be directly retrieved from
   * AOD - the others are available in the ESD ... */
  std::set<xAOD::CaloCluster::MomentType>  m_momentsAOD; 

  /** 
   * @brief vector of calibration hit container names to use. 
   *
   * The containers specified in this property should hold calibration
   * hits inside the calorimeter systems. */
  std::vector<std::string> m_CalibrationHitContainerNames;

  /** 
   * @brief vector of dead material calibration hit container names to use. 
   *
   * The containers specified in this property should hold calibration
   * hits outside the calorimeter systems - i.e. dead material hits ... */
  std::vector<std::string> m_DMCalibrationHitContainerNames;

  /** 
   * @brief name of truth particle container
   */
  std::string m_truthParticleCollectionName;

  const CaloDetDescrManager* m_calo_dd_man; 

  const CaloCell_ID* m_calo_id;

  const CaloDM_ID*    m_caloDM_ID;

  const CaloDmDescrManager* m_caloDmDescrManager; 

  //const McEventCollection* m_truthEvent;
  const TruthParticleContainer* m_truthParticles;

  int m_n_phi_out;
  int m_n_eta_out;
  double m_out_phi_max;
  double m_out_eta_max;

  double m_rmaxOut[3];

  std::vector<CalibHitIPhiIEtaRange> *m_i_phi_eta[3];

  bool m_isInitialized;

  bool m_foundAllContainers;

  enum keys_dm_energy_sharing {kMatchDmOff, kMatchDmLoose, kMatchDmMedium, kMatchDmTight};
  enum keys_calib_frac_origin {kCalibFracEM, kCalibFracHAD, kCalibFracREST, kCalibFracMax};

  bool m_doDeadEnergySharing;
  bool m_doOutOfClusterL;
  bool m_doOutOfClusterM;
  bool m_doOutOfClusterT;
  bool m_doDeadL;
  bool m_doDeadM;
  bool m_doDeadT;
  bool m_useParticleID;
  bool m_doCalibFrac;
  float m_energyMin;
  float m_energyMinCalib;
  float m_apars_alpha;
  float m_apars_r0;
  //double m_showerScale;
  int m_MatchDmType;

  double angle_mollier_factor(double x);
  void get_calib_frac(const MyClusInfo *clusInfo, std::vector<double> &engFrac);

  m_CellInfoSet_t m_CellInfo;
  m_ClusInfo_t m_ClusInfo;
};

#endif // CALOCALIBCLUSTERMOMENTSMAKER2_H






