/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TrackTools.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Track tools.
 *
 * Created in February 2013, based on TrackInCaloTools.
 */

#ifndef TrackTools_H
#define TrackTools_H

/// STOREGATE INCLUDES
#include "StoreGate/StoreGateSvc.h"
#include "AthContainers/ConstDataVector.h"

/// GAUDI INCLUDES
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"

/// CALORIMETER INCLUDES
#include "CaloEvent/CaloCellContainer.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloEvent/CaloCell.h"


#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"

/// UTILITIES
#include "FourMomUtils/P4Helpers.h"
#include "IEnergyDepositionTool/IEnergyDepositionTool.h"
#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
// #include "CaloDetDescr/CaloPhiRange.h"
#include "ConeDefinition.h"
#include "Scintillator.h"

/// TRACK INCLUDES
#include "TrkParameters/TrackParameters.h" 
#include "IExtrapolator.h"
#include "ITrackTools.h"
#include "TrackInCaloTools/TICTListener.h"
#include "Particle/TrackParticle.h"

/// C++ INCLUDES
#include <string>

/// USEFUL GLOBAL VARIABLES
#define CONE_NxM 0.35 // Cone to contain 3x3 cells of any layer. Largest cell is 0.2 x 0.1 --> sqrt(0.3**2 + 0.15**2)
#define NLAYERS 11

/// FORWARD DECLARATIONS
class ICaloNoiseTool;
class CaloDepthTool;
class CaloDetDescrManager;
class TileDetDescrManager;
class CaloCell_ID;
class ConeDefinition;
namespace Trk{
    class GeoShapeConverter;
} // TRK

using HepGeom::Transform3D;
using HepGeom::Translate3D;
using CLHEP::Hep3Vector;
using CLHEP::mm;

//==========================================================================================================================
class TrackTools: public AthAlgTool, virtual public ITrackTools, virtual public IEnergyDepositionTool, virtual TICTListener{
//==========================================================================================================================
    public:
        TrackTools (const std::string& type,const std::string& name,const IInterface* parent);
        ~TrackTools();

        StatusCode initialize();
        StatusCode finalize();

        typedef std::vector<const CaloCell*> ListOfCells;

        // METHOD FOR CALOCELL FILTERING
        void getCellsWithinConeAroundTrack(const Trk::Track* track, 
                                           const CaloCellContainer* input,
                                           ConstDataVector<CaloCellContainer>* output,  
                                           double cone,
                                           bool includelar);
        void getCellsWithinConeAroundTrack(const Rec::TrackParticle* track, 
                                           const CaloCellContainer* input,
                                           ConstDataVector<CaloCellContainer>* output,  
                                           double cone,
                                           bool includelar){
             getCellsWithinConeAroundTrack(track->originalTrack(),input,output,cone,includelar);
        };

        std::vector< double > getXYZEtaPhiInCellSampling(const Trk::Track* track, const CaloCell *cell);
        std::vector< double > getXYZEtaPhiInCellSampling(double exit[],const Trk::Track* track, const CaloCell *cell);
        std::vector< double > getXYZEtaPhiInCellSampling(const Trk::Track* track, int sampling);

        //Get Energy from LAr cone; Added by Archil
        double getEnergyLArCone(const Trk::Track* track, double cone);

        //=================================
        // METHODS FOR Trk::TrackParameters
        //=================================
        std::vector<const Trk::TrackParameters*> getExit(const Trk::TrackParameters *track);
        std::vector<const Trk::TrackParameters*> getEntrance(const Trk::TrackParameters *track);

        std::vector<const Trk::TrackParameters*> getExit(const Trk::Track *track){
            return getExit(m_trackExtrapolatorToCalo->getTrackParameters(track));
        };
        std::vector<const Trk::TrackParameters*> getEntrance(const Trk::Track *track){
            return getEntrance(m_trackExtrapolatorToCalo->getTrackParameters(track));
        };

        std::vector<const Trk::TrackParameters*> getExit(const INavigable4Momentum *track){
            const INavigable4Momentum *t = track; 
            return getExit(m_trackExtrapolatorToCalo->getTrackParameters(t));
        };
        std::vector<const Trk::TrackParameters*> getEntrance(const INavigable4Momentum *track){
            const INavigable4Momentum *t = track; 
            return getEntrance(m_trackExtrapolatorToCalo->getTrackParameters(t));
        };

        const Trk::TrackParameters* getExit(const Trk::TrackParameters *track, int layer);
        const Trk::TrackParameters* getEntrance(const Trk::TrackParameters *track, int layer);

        const Trk::TrackParameters* getExit(const Trk::Track *track, int layer){
            return getExit(m_trackExtrapolatorToCalo->getTrackParameters(track), layer);
        };
        const Trk::TrackParameters* getEntrance(const Trk::Track *track, int layer){
            return getEntrance(m_trackExtrapolatorToCalo->getTrackParameters(track), layer);
        };

        const Trk::TrackParameters* getExit(const INavigable4Momentum *track, int layer){
            const INavigable4Momentum *t = track; 
            return getExit(m_trackExtrapolatorToCalo->getTrackParameters(t), layer);
        };
        const Trk::TrackParameters* getEntrance(const INavigable4Momentum *track, int layer){
            const INavigable4Momentum *t = track; 
            return getEntrance(m_trackExtrapolatorToCalo->getTrackParameters(t), layer);
        };

        double getExit(const Trk::TrackParameters *track, int layer, int pos);
        double getEntrance(const Trk::TrackParameters *track, int layer, int pos);

        double getExit(const Trk::Track *track, int layer, int pos){
            return getExit(m_trackExtrapolatorToCalo->getTrackParameters(track), layer, pos);
        };
        double getEntrance(const Trk::Track *track, int layer, int pos){
            return getEntrance(m_trackExtrapolatorToCalo->getTrackParameters(track), layer, pos);
        };

        double getExit(const INavigable4Momentum *track, int layer, int pos){
            const INavigable4Momentum *t = track; 
            return getExit(m_trackExtrapolatorToCalo->getTrackParameters(t), layer, pos);
        };
        double getEntrance(const INavigable4Momentum *track, int layer, int pos){
            const INavigable4Momentum *t = track; 
            return getEntrance(m_trackExtrapolatorToCalo->getTrackParameters(t), layer, pos);
        };
        std::vector< std::pair<double, double> > getEtaPhiPerLayer(const Trk::TrackParameters* track);
        std::vector< std::pair<double, double> > getEtaPhiPerLayer(const Trk::Track *track){
            return getEtaPhiPerLayer(m_trackExtrapolatorToCalo->getTrackParameters(track));
        };
        std::vector< std::pair<double, double> > getEtaPhiPerLayer(const INavigable4Momentum *track){
            const INavigable4Momentum *t = track; 
            return getEtaPhiPerLayer(m_trackExtrapolatorToCalo->getTrackParameters(t));
        };
        std::vector< std::vector<double> > getXYZEtaPhiPerLayer(const Trk::TrackParameters* track);
        std::vector< std::vector<double> > getXYZEtaPhiPerLayer(const Trk::Track *track){
            return getXYZEtaPhiPerLayer(m_trackExtrapolatorToCalo->getTrackParameters(track));
        };
        std::vector< std::vector<double> > getXYZEtaPhiPerLayer(const INavigable4Momentum *track){
            const INavigable4Momentum *t = track; 
            return getXYZEtaPhiPerLayer(m_trackExtrapolatorToCalo->getTrackParameters(t));
        };
        double getIsolationEnergy(const Trk::TrackParameters* track, const double dR,
                                  CaloLayer first_layer, CaloLayer last_layer,
                                  bool useLooseCut);
        double getMeasuredEnergy(const Trk::TrackParameters* track, const double dR,
                                 CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                 bool useLooseCut = false, bool isEt = true);
        bool getMeasuredEnergy(const Trk::TrackParameters* track, const std::vector<double>& dR, std::vector<double>& energies,
                               CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                               bool useLooseCut = false, bool isEt = true);
        double getEcore(const Trk::TrackParameters* track,
                        CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                        bool useLooseCut = false, bool isEt = true); 
        std::vector< std::vector<const CaloCell*> > getCellsForIsolation(const Trk::TrackParameters* track, const double dR, 
                                                                         CaloLayer first_layer, CaloLayer last_layer,
                                                                         bool useLooseCut);
        std::vector<const CaloCell*> getCellsForEcore(const Trk::TrackParameters* track,
                                                      CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                      bool useLooseCut = false, bool usePosition = false){
                                                          return getCellsForConeDef(track,
                                                                                    first_layer, last_layer,
                                                                                    useLooseCut, m_eCore, usePosition);
                                                      }; // getCellsForEcore
        std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const Trk::TrackParameters* track,const double dR,
                                                                       CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                                       bool useLooseCut = false);
        std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const Trk::TrackParameters* track,const double deta, const double dphi,
                                                                       CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                                       bool useLooseCut = false);
        std::vector<const CaloCell*> getCellsAroundTrackOnLayer(const Trk::TrackParameters *track,
                                                                CaloLayer layer,
                                                                int neta, int nphi, int dir_eta=0, int dir_phi=0,
                                                                bool useLooseCut = false);
  	bool isClusterAroundTrack(double cone, const CaloCluster* cluster, const Rec::TrackParticle* track);
  	std::vector<const CaloCluster*> getClustersAroundTrack(double cone,  const Rec::TrackParticle* track);
        std::vector<const CaloCell*> getCellsOnTrack(const Trk::TrackParameters* track,
                                                     CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                     bool useLooseCut = false);
        std::vector<const CaloCell*> getCellsCrossedByTrack(const Trk::TrackParameters* track,
                                                            CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                            bool useLooseCut = false);
        std::vector<double> getWeightsForCellsCrossed(const Trk::TrackParameters* track, bool useLooseCut);
        double getPathInsideCell(const Trk::TrackParameters *track, const CaloCell *cell);
        double getCBNTPathInsideCell(const Trk::Track *track, const CaloCell *cell);
//         double getCBNTActivePathInsideCell(const Trk::TrackParameters *track, const CaloCell *cell);
        std::vector<const CaloCell*> getNeighbours(const CaloCell*, int neta, int nphi, int dir_eta=0, int dir_phi=0, bool useLooseCut = false);
        double getIsolationWidth(const Trk::TrackParameters* track,
                                 const double dR, const double EtExp, const double dRExp,
                                 bool negRemoval = true,
                                 CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                 bool useLooseCut = false);

        //=======================
        // METHODS FOR Trk::Track
        //=======================
        std::vector<const CaloCell*> getAssociatedCells(const Trk::Track* track, CaloLayer first_layer, CaloLayer last_layer);
        double getIsolationEnergy(const Trk::Track* track, const double dR,
                                  CaloLayer first_layer, CaloLayer last_layer,
                                  bool useLooseCut){
            return getIsolationEnergy(m_trackExtrapolatorToCalo->getTrackParameters(track), dR, first_layer, last_layer, useLooseCut);
        }; // GETISOLATIONENERGY
        double getMeasuredEnergy(const Trk::Track* track, const double dR,
                                 CaloLayer first_layer = ps,  CaloLayer last_layer = hec3,
                                 bool useLooseCut = false, bool isEt = true){
            return getMeasuredEnergy(m_trackExtrapolatorToCalo->getTrackParameters(track), dR, first_layer, last_layer, useLooseCut, isEt);
        }; // GETMEASUREDENERGY
        bool getMeasuredEnergy(const Trk::Track* track,const std::vector<double>& dR, std::vector<double>& energies,
                               CaloLayer first_layer = ps,  CaloLayer last_layer = hec3,
                               bool useLooseCut = false, bool isEt = true){
            return getMeasuredEnergy(m_trackExtrapolatorToCalo->getTrackParameters(track), dR, energies,
                                     first_layer, last_layer, useLooseCut, isEt);
        }; // GETMEASUREDENERGY
        double getEcore(const Trk::Track* track,
                        CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                        bool useLooseCut = false, bool isEt = true){
            return getEcore(m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut, isEt);
        }; // GETECORE
        std::vector< std::vector<const CaloCell*> > getCellsForIsolation(const Trk::Track* track, const double dR,
                                                                         CaloLayer first_layer, CaloLayer last_layer,
                                                                         bool useLooseCut){
            return getCellsForIsolation(m_trackExtrapolatorToCalo->getTrackParameters(track),dR, first_layer, last_layer, useLooseCut);
        }; // GETCELLSFORISOLATION
        std::vector<const CaloCell*> getCellsForEcore(const Trk::Track* track,
                                                      CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                      bool useLooseCut = false){
            return getCellsForEcore(m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut);
        }; // GETCELLSFORECORE
        std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const Trk::Track* track, const double dR, 
                                                                       CaloLayer first_layer = ps, CaloLayer last_layer = hec3, 
                                                                       bool useLooseCut = false){
            return getCellsAroundTrack(m_trackExtrapolatorToCalo->getTrackParameters(track), dR, first_layer, last_layer, useLooseCut);
        }; // GETCELLSAROUNDTRACK
        std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const Trk::Track* track, const double deta,const double dphi,
                                                                       CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                                       bool useLooseCut = false){
            return getCellsAroundTrack(m_trackExtrapolatorToCalo->getTrackParameters(track), deta, dphi, 
                                       first_layer, last_layer, useLooseCut);
        }; // GETCELLSAROUNDTRACK
        std::vector<const CaloCell*> getCellsAroundTrackOnLayer(const Trk::Track *track,
                                                                CaloLayer layer, int neta, int nphi, int dir_eta=0, int dir_phi=0,
                                                                bool useLooseCut = false){
            return getCellsAroundTrackOnLayer(m_trackExtrapolatorToCalo->getTrackParameters(track),
                                              layer, neta, nphi, dir_eta, dir_phi, useLooseCut);
        }; // GETCELLSAROUNDTRACKONLAYER
        std::vector<const CaloCell*> getCellsOnTrack(const Trk::Track* track,
                                                     CaloLayer first_layer = ps, CaloLayer last_layer = hec3, bool useLooseCut = false){
            return getCellsOnTrack( m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut );
        }; // GETCELLSONTRACK
         std::vector<const CaloCell*> getCellsCrossedByTrack(const Trk::Track* track,
                                                             CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                             bool useLooseCut = false){
            return getCellsCrossedByTrack(m_trackExtrapolatorToCalo->getTrackParameters(track),
                                          first_layer, last_layer, useLooseCut );
        }; // GETCELLSCROSSEDBYTRACK
        std::vector<double> getWeightsForCellsCrossed(const Trk::Track* track, bool useLooseCut){
            return getWeightsForCellsCrossed( m_trackExtrapolatorToCalo->getTrackParameters(track), useLooseCut);
        }; // GETWEIGHTSFORCELLSCROSSED
        double getPathInsideCell(const Trk::Track *track, const CaloCell *cell){
            return getPathInsideCell(m_trackExtrapolatorToCalo->getTrackParameters(track), cell);
        }; // GETPATHINSIDECELL

//         double getCBNTActivePathInsideCell(const Trk::Track *track, const CaloCell *cell){
//             return getCBNTActivePathInsideCell(m_trackExtrapolatorToCalo->getTrackParameters(track), cell);
//         }; // GETPATHINSIDECELL
        double getIsolationWidth(const Trk::Track* track,const double dR, const double EtExp, const double dRExp, bool negRemoval = true,
                                 CaloLayer first_layer = ps, CaloLayer last_layer = hec3, bool useLooseCut = false){
            return getIsolationWidth(m_trackExtrapolatorToCalo->getTrackParameters(track),dR, EtExp, dRExp, negRemoval,
                                     first_layer, last_layer, useLooseCut);
        }; // GETISOLATIONWIDTH

        //====================
        // INavigable4Momentum
        //====================
        double getIsolationEnergy(const INavigable4Momentum* track, const double dR,
                                  CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut){
            const INavigable4Momentum *t = track; 
            return getIsolationEnergy(m_trackExtrapolatorToCalo->getTrackParameters(t), dR, 
                                      first_layer, last_layer, useLooseCut);
        }; // GETISOLATIONENERGY
        double getMeasuredEnergy(const INavigable4Momentum* track, const double dR,
                                 CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                 bool useLooseCut = false, bool isEt = true){
            return getMeasuredEnergy(m_trackExtrapolatorToCalo->getTrackParameters(track), dR,
                                     first_layer, last_layer, useLooseCut, isEt);
        }; // GETMEASUREDENERGY
        bool getMeasuredEnergy(const INavigable4Momentum* track, const std::vector<double>& dR, std::vector<double>& energies,
                               CaloLayer first_layer = ps, CaloLayer last_layer = hec3, bool useLooseCut = false, bool isEt = true){
            return getMeasuredEnergy(m_trackExtrapolatorToCalo->getTrackParameters(track), dR, energies,
                                     first_layer, last_layer, useLooseCut, isEt);
        }; // GETMEASUREDENERGY
        double getEcore(const INavigable4Momentum* track, CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                        bool useLooseCut = false, bool isEt = true){
            return getEcore(m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut, isEt);
        }; // GETECORE
        std::vector< std::vector<const CaloCell*> > getCellsForIsolation(const INavigable4Momentum* track,const double dR, 
                                                                         CaloLayer first_layer, CaloLayer last_layer,
                                                                         bool useLooseCut){
            return getCellsForIsolation(m_trackExtrapolatorToCalo->getTrackParameters(track),dR, first_layer, last_layer, useLooseCut);
        }; // GETCELLSFORISOLATION
        std::vector<const CaloCell*> getCellsForEcore(const INavigable4Momentum* track,
                                                      CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                      bool useLooseCut = false){
            return getCellsForEcore(m_trackExtrapolatorToCalo->getTrackParameters(track),
                                    first_layer, last_layer, useLooseCut);
        }; // GETCELLSFORECORE
        std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const INavigable4Momentum* track, const double dR,
                                                                       CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                                       bool useLooseCut = false){
            return getCellsAroundTrack(m_trackExtrapolatorToCalo->getTrackParameters(track), dR,
                                       first_layer, last_layer, useLooseCut);
        }; // GETCELLSAROUNDTRACK
        std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const INavigable4Momentum* track, const double deta, const double dphi, 
                                                                       CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                                       bool useLooseCut = false){
            return getCellsAroundTrack(m_trackExtrapolatorToCalo->getTrackParameters(track),
                                       deta, dphi, first_layer, last_layer, useLooseCut);
        }; // GETCELLSAROUNDTRACK
        std::vector<const CaloCell*> getCellsAroundTrackOnLayer(const INavigable4Momentum *track,
                                                                CaloLayer layer, int neta, int nphi, int dir_eta = 0, int dir_phi = 0,
                                                                bool useLooseCut = false){
            return getCellsAroundTrackOnLayer(m_trackExtrapolatorToCalo->getTrackParameters(track),
                                              layer, neta, nphi, dir_eta, dir_phi, useLooseCut);
        }; // GETCELLSAROUNDTRACKONLAYER
        std::vector<const CaloCell*> getCellsOnTrack(const INavigable4Momentum* track,
                                                     CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                     bool useLooseCut = false){
            return getCellsOnTrack(m_trackExtrapolatorToCalo->getTrackParameters(track),
                                   first_layer, last_layer, useLooseCut );
        }; // GETCELLSONTRACK
        std::vector<const CaloCell*> getCellsCrossedByTrack(const INavigable4Momentum* track,
                                                            CaloLayer first_layer = ps, CaloLayer last_layer = hec3,
                                                            bool useLooseCut = false){
            return getCellsCrossedByTrack( m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut );
        }; // GETCELLSCROSSEDBYTRACK
        std::vector<double> getWeightsForCellsCrossed(const INavigable4Momentum* track, bool useLooseCut){
            return getWeightsForCellsCrossed( m_trackExtrapolatorToCalo->getTrackParameters(track), useLooseCut);
        }; // GETWEIGHTSFORCELLSCROSSED
        double getPathInsideCell(const INavigable4Momentum *track, const CaloCell *cell){
            return getPathInsideCell(m_trackExtrapolatorToCalo->getTrackParameters(track), cell);
        }; // GETPATHINSIDECELL

//         double getCBNTActivePathInsideCell(const INavigable4Momentum *track, const CaloCell *cell){
//             return getCBNTActivePathInsideCell(m_trackExtrapolatorToCalo->getTrackParameters(track), cell);
//         }; // GETPATHINSIDECELL
        double getIsolationWidth(const INavigable4Momentum* track,
                                 const double dR, const double EtExp, const double dRExp, bool negRemoval = true,
                                 CaloLayer first_layer = ps, CaloLayer last_layer = hec3, bool useLooseCut = false){
            return getIsolationWidth(m_trackExtrapolatorToCalo->getTrackParameters(track),
                                     dR, EtExp, dRExp, negRemoval, first_layer, last_layer, useLooseCut);
        }; // GETISOLATIONWIDTH

        //=====================
        // SCINTILLATOR METHODS
        //=====================
//         float getActivePath(const Trk::Track* i_trk, const CaloCell* i_cell);
//         float getActivePath(const Trk::Track* i_trk, const CaloCell* i_cell,
//                             std::vector<const Scintillator*> scintillators, short& N_scint);
        int retrieveIndex(int sampling, float eta);
        int retrieveIndex(const CaloCell* cell){ return retrieveIndex(cell->caloDDE()->getSampling(), cell->caloDDE()->eta()); };
//         void getScintCoordinates(const CaloCell* cell, std::vector<const Scintillator*>& scintillators);

        //==================
        // REMAINING METHODS
        //==================
        // GET ISOLATION ENERGY
        std::vector<double> getIsolationEnergy(std::vector<const INavigable4Momentum *> tracks, double dR,
                                               CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut);

        // MEASURE ENERGY MEASUREMENT
        std::pair<double,double> measurement(CaloCell_ID::CaloSample sample, const Trk::TrackParameters& parm);

        // COMPUTE E, ET, ASSOCIATED CELLS AND CELLS ON TRACK
        StatusCode getEnergyAndCells(const Trk::TrackParameters* track,
                                     std::vector<double> conesize,
                                     std::vector< std::vector<double> > &etot,
                                     std::vector< std::vector<ListOfCells> > &usedCells, 
                                     bool useLooseCut = false, bool isEt = true);

        // UTILITIES
        int getLayer(const CaloCell *cell);
        void setNoiseThreshold(double threshold);
        double getNoiseThreshold() { return m_cellNoiseRMSCut; };

        bool accept(const CaloCell *cell, bool useLooseCut = false);
    private:
        ToolHandle<IExtrapolateToCaloTool> m_toCalo;

        // JOBOPTIONS PROPERTIES
        double              m_cellNoiseRMSCut;
        double              m_cellNoiseRMSCutLoose;
        bool                m_useExtrapolation;
        ToolHandle<ICaloNoiseTool> m_noiseTool;
	ToolHandle<ITrackTools>      m_inCalo;
        std::string         m_cellContainerName;
  	std::string 	    m_caloclusterContainerName;
        bool                m_negativeCells;
        std::vector<std::string> m_eCoreDef;
        bool m_calibCrossed;

        // USEFUL POINTERS
        void m_clear();
        const Trk::TrackParameters *m_currentTrackParameters;
        std::vector<const CaloCell*> m_cellsOnTrack;
        std::vector<std::vector<const CaloCell*> > m_cellsPerLayer;
        std::vector <std::pair<double, double> > m_extrapolations;
        double m_cone;
        std::vector<double> m_weightsForCellsCrossed;

        // CALORIMETER POINTERS
        const CaloDetDescrManager* m_calo_dd;
        const CaloCell_ID* m_caloCell_ID;
        const TileDetDescrManager* m_tileDDM;
	const CaloClusterContainer* m_caloclusterContainer;

        // TOOLS
        StatusCode m_noiseToolSc; 
        std::vector<CaloCell_ID::SUBCALO> m_subCalos;
        IExtrapolator *m_trackExtrapolatorToCalo;
        StoreGateSvc *m_detStoreSvc;



        
        // CELLS AND TRACKS METHODS
        int getLayer(CaloCell_ID::CaloSample calo);
        void getCaloSamples();
        void selectCells(std::vector<const CaloCell*> &cells, bool useLooseCut = false);
        void getDirections(std::vector<const CaloCell*> cells, int neta, int nphi, int *dir_eta, int *dir_phi, double eta = -9, double phi = 0);
        void selectNeighbours(std::vector<const CaloCell*> &cells, int neta, int nphi, int dir_eta, int dir_phi, double eta = -9, double phi = 0);
        StatusCode getCellsForCone(double cone);
        StatusCode getExtrapolations(const Trk::TrackParameters* track, bool usePosition);

        // ECORE DEFINITION
        ConeDefinition *m_eCore;
        void setEcoreDefinition(std::vector<std::string> eCoreDef){
            m_eCore->setConeDefinition(eCoreDef);
        }; // SETECOREDEFINITION
        void print_EcoreDefinition(){
            m_eCore->printConeDefinition();
        }; // PRINT_ECOREDEFINITION

        // UTILILITIES
        std::vector<double> getParametersForCellSelection(CaloCell_ID::SUBCALO subCalo);
        std::vector<double> getMidPointAndDistance(std::pair<double, double> p1, std::pair<double, double> p2);
        std::vector<const CaloCell*> getCellsForConeDef(const Trk::TrackParameters* track,
                                                        CaloLayer first_layer, CaloLayer last_layer,
                                                        bool useLooseCut, ConeDefinition *coneDef, bool usePosition);
        double getLargestConeNeeded(ConeDefinition *coneDef);
        double getConeSizeForDef(int ilayer, ConeDefinition *coneDef);
        void removeCoreFromCone(std::vector< std::vector< const CaloCell*> > &cells,
                                std::vector< const CaloCell*> cells_core,
                                CaloLayer first_layer, CaloLayer last_layer);
        void selectCellsByLayer(std::vector<const CaloCell*> &cells, int layer);
        void selectCellsBySample(std::vector<const CaloCell*> &cells, CaloCell_ID::CaloSample sample);
        double pathInsideCell(const CaloCell *cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit);

        double getPathLengthInPhi(const CaloCell*, double phi_entrance, double phi_exit);
        double getPathLengthInPhi(const Scintillator* scint, double phi_entrance, double phi_exit);
        double getPathLengthInEta(const CaloCell *cell, double eta_entrance, double eta_exit);
        double getPathLengthInEta(const Scintillator* scint, double eta_entrance, double eta_exit);
        double getPathLengthInZ(const CaloCell *cell, double z_entrance, double z_exit);
        double getPathLengthInZ(const Scintillator* scint, double z_entrance, double z_exit);
        double getPathLengthInZ(double zMin, double zMax, double z_entrance, double z_exit);
        double getPathLengthInZTileBar1(const CaloCell *cell, double z_entrance, double z_exit);
        double deltaR(const Trk::TrackParameters *a, const Trk::TrackParameters *b);
        double phiMean(double a, double b) { return 0.5*(a+b) + (a*b < 0)*M_PI; };
        double square(double x) { return x*x; };
        int sign(double x, double min = 1e-5);

        // SCINTILLATOR METHODS
        void dumpGrandKids(const PVConstLink pv);
        void dumpKids(const PVConstLink pv);
        void dump(const PVConstLink pv);
        std::vector<Scintillator> m_scint;

        bool newTrack(const Trk::TrackParameters* track);
        bool isCellOnTrack(const CaloCell *cell, double eta, double phi);
        bool cellCrossedByTrack(const CaloCell *cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit);
        bool crossedPhi(const CaloCell *cell, double phi_entrance, double phi_exit);
        bool crossedPhi(const Scintillator *scint, double phi_entrance, double phi_exit);
        bool check(double a, double A, double dA);
        bool isScintOnTrack(const Scintillator *scint, double eta, double phi);

        std::pair<float,float> getLadderZ(int sampling, float eta, bool ladd);
        std::pair<float,float> getLadderZ(const CaloCell* cell, bool ladd);
        float getPathInsideScint(const Trk::TrackParameters *track, const CaloCell *cell, const Scintillator* scint);
        float getPathInsideScint(const Trk::Track *track, const CaloCell *cell, const Scintillator* scint){
            return getPathInsideScint(m_trackExtrapolatorToCalo->getTrackParameters(track), cell, scint);
        };
        float getPathInsideScint(const INavigable4Momentum *track, const CaloCell *cell, const Scintillator* scint){
            return getPathInsideScint(m_trackExtrapolatorToCalo->getTrackParameters(track), cell, scint);
        };
        float pathInsideScint(const CaloCell *cell, const Scintillator* scint, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit);
        int getFirstTile(int sampling, float eta, bool ladd);
        int getFirstTile(const CaloCell* cell, bool ladd){
            return getFirstTile(cell->caloDDE()->getSampling(), cell->caloDDE()->eta(), ladd);
        };
        double phidiff(double phi1, double phi2);

        double getPathFromCBNTMethod(const CaloCell* cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit);
        double getPathFromExtra(const CaloCell* cell, std::vector<Trk::TrackParameters*> track_extrapolations);
        double getPathFromExtra(const Trk::Track* i_trk, const CaloCell* i_cell);
        double getActivePathFromCBNTMethod(const CaloCell* cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit);

        // DEFAULT CONSTRUCTOR
        TrackTools(const TrackTools&);
        TrackTools &operator= (const TrackTools&);
};

#endif //TrackTools_H
