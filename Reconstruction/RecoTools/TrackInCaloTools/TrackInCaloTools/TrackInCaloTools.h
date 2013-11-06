/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKINCALOTOOLS_H
#define TRACKINCALOTOOLS_H
//
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
//
#include "TrkParameters/TrackParameters.h" // cannot forward declare because it is a typedef

#include "IEnergyDepositionTool/IEnergyDepositionTool.h"
#include "CaloDetDescr/CaloPhiRange.h"

#include "ITrackInCaloTools/ITrackExtrapolatorToCalo.h"
#include "ITrackInCaloTools/ITrackInCaloTools.h"
#include "TrackInCaloTools/TICTListener.h"
#include "TrackInCaloTools/ConeDefinition.h"

#define NLAYERS 11
/////////////////////////////////////////////////////////
class ICaloNoiseTool;
class CaloDepthTool;
class CaloDetDescrManager;
class TileDetDescrManager;
class CaloCell_ID;

/////////////////////////////////////////////////////////
/** @class TrackInCaloTools

    Tool used to calculate calorimetric isolation for muons and muon energy loss
    https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackInCaloTools

    @author Konstantinos.Bachas@cern.ch, Samira.Hassani@cern.ch, Bruno.Lenzi@cern.ch
*/
class TrackInCaloTools: public AthAlgTool, virtual public ITrackInCaloTools, virtual public IEnergyDepositionTool, virtual TICTListener
{
  public:
    TrackInCaloTools (const std::string& type,const std::string& name,const IInterface* parent);
    ~TrackInCaloTools();

    StatusCode initialize();
    StatusCode finalize();
    
    typedef std::vector<const CaloCell*> ListOfCells;
    
    // Energy (Et) around the track, subtracting etCore
    double getIsolationEnergy(const Trk::TrackParameters* track,const double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut);
    // Get energy for range of layers
    double getMeasuredEnergy(const Trk::TrackParameters* track,const double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt = true);
    // Energy for several cones
    bool getMeasuredEnergy(const Trk::TrackParameters* track,const std::vector<double>& dR, std::vector<double>& energies, CaloLayer first_layer=ps,  CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt = true);
    // Get the core energy, corresponding to the energy deposited by a muon track
    double getEcore(const Trk::TrackParameters* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt = true); 
    // Cells around the track per layer, subtracting the ones used for eCore
    std::vector< std::vector<const CaloCell*> > getCellsForIsolation(const Trk::TrackParameters* track,const double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut);
    // Cells used for the calculation of eCore
    std::vector<const CaloCell*> getCellsForEcore(const Trk::TrackParameters* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false, bool usePosition = false)
    { return getCellsForConeDef(track, first_layer, last_layer, useLooseCut, m_eCore, usePosition); }
    // Get cells around the track for all layers
    std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const Trk::TrackParameters* track,const double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false);
    // Get cells around the track for all layers, "rectangular cone"
    std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const Trk::TrackParameters* track,const double deta, const double dphi, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false);
    // Get neta x nphi cells around the track for given layer
    std::vector<const CaloCell*> getCellsAroundTrackOnLayer(const Trk::TrackParameters *track, CaloLayer layer, int neta, int nphi, int dir_eta=0, int dir_phi=0, bool useLooseCut=false);
    // Get cells on track (one cell per layer)
    std::vector<const CaloCell*> getCellsOnTrack(const Trk::TrackParameters* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false);
    // Get all cells crossed by track
    std::vector<const CaloCell*> getCellsCrossedByTrack(const Trk::TrackParameters* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false);
    std::vector<double> getWeightsForCellsCrossed(const Trk::TrackParameters* track, bool useLooseCut);
    double getPathInsideCell(const Trk::TrackParameters *track, const CaloCell *cell);
    // Get the neta X nphi neighbours of the cell
    std::vector<const CaloCell*> getNeighbours(const CaloCell*, int neta, int nphi, int dir_eta=0, int dir_phi=0, bool useLooseCut=false);
    // Get sum( Et**EtExp * dR**dRExp )/sum(Et) within a cone of dR
    double getIsolationWidth(const Trk::TrackParameters* track,const double dR, const double EtExp, const double dRExp, bool negRemoval=true, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false);
    
    //-----------------------------------------------------------------------------------------
    // same for Trk::Track
    double getIsolationEnergy(const Trk::Track* track,const double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
    { return getIsolationEnergy(m_trackExtrapolatorToCalo->getTrackParameters(track), dR, first_layer, last_layer, useLooseCut); };
    double getMeasuredEnergy(const Trk::Track* track,const double dR, CaloLayer first_layer=ps,  CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt = true)
    { return getMeasuredEnergy(m_trackExtrapolatorToCalo->getTrackParameters(track), dR, first_layer, last_layer, useLooseCut, isEt);  };
    bool getMeasuredEnergy(const Trk::Track* track,const std::vector<double>& dR, std::vector<double>& energies, CaloLayer first_layer=ps,  CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt = true)
    { return getMeasuredEnergy(m_trackExtrapolatorToCalo->getTrackParameters(track), dR, energies, first_layer, last_layer, useLooseCut, isEt); };
    double getEcore(const Trk::Track* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt = true)
    { return getEcore(m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut, isEt); }
    std::vector< std::vector<const CaloCell*> > getCellsForIsolation(const Trk::Track* track,const double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
    { return getCellsForIsolation(m_trackExtrapolatorToCalo->getTrackParameters(track),dR, first_layer, last_layer, useLooseCut); };
    std::vector<const CaloCell*> getCellsForEcore(const Trk::Track* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)
    { return getCellsForEcore(m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut); }
    std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const Trk::Track* track,const double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)
    { return getCellsAroundTrack(m_trackExtrapolatorToCalo->getTrackParameters(track), dR, first_layer, last_layer, useLooseCut); };
    std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const Trk::Track* track,const double deta,const double dphi, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)
    { return getCellsAroundTrack(m_trackExtrapolatorToCalo->getTrackParameters(track), deta, dphi, first_layer, last_layer, useLooseCut); };
    std::vector<const CaloCell*> getCellsAroundTrackOnLayer(const Trk::Track *track, CaloLayer layer, int neta, int nphi, int dir_eta=0, int dir_phi=0, bool useLooseCut=false)
    { return getCellsAroundTrackOnLayer(m_trackExtrapolatorToCalo->getTrackParameters(track), layer, neta, nphi, dir_eta, dir_phi, useLooseCut); };
    std::vector<const CaloCell*> getCellsOnTrack(const Trk::Track* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)
    { return getCellsOnTrack( m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut ); };
     std::vector<const CaloCell*> getCellsCrossedByTrack(const Trk::Track* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)
     { return getCellsCrossedByTrack( m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut ); };
    std::vector<double> getWeightsForCellsCrossed(const Trk::Track* track, bool useLooseCut)
    { return getWeightsForCellsCrossed( m_trackExtrapolatorToCalo->getTrackParameters(track), useLooseCut); };
    double getPathInsideCell(const Trk::Track *track, const CaloCell *cell)
    { return getPathInsideCell(m_trackExtrapolatorToCalo->getTrackParameters(track), cell); };
    double getIsolationWidth(const Trk::Track* track,const double dR, const double EtExp, const double dRExp, bool negRemoval=true, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)
    { return getIsolationWidth(m_trackExtrapolatorToCalo->getTrackParameters(track),dR, EtExp, dRExp, negRemoval, first_layer, last_layer, useLooseCut); }
    
    //-----------------------------------------------------------------------------------------
    // for INavigable4Momentum
    double getIsolationEnergy(const INavigable4Momentum* track,const double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
    { const INavigable4Momentum *t = track; 
return getIsolationEnergy(m_trackExtrapolatorToCalo->getTrackParameters(t), dR, first_layer, last_layer, useLooseCut); };
    double getMeasuredEnergy(const INavigable4Momentum* track,const double dR, CaloLayer first_layer=ps,  CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt = true)
    { return getMeasuredEnergy(m_trackExtrapolatorToCalo->getTrackParameters(track), dR, first_layer, last_layer, useLooseCut, isEt);  };
    bool getMeasuredEnergy(const INavigable4Momentum* track,const std::vector<double>& dR, std::vector<double>& energies, CaloLayer first_layer=ps,  CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt = true)
    { return getMeasuredEnergy(m_trackExtrapolatorToCalo->getTrackParameters(track), dR, energies, first_layer, last_layer, useLooseCut, isEt); };
    double getEcore(const INavigable4Momentum* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false, bool isEt = true)
    { return getEcore(m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut, isEt); }
    std::vector< std::vector<const CaloCell*> > getCellsForIsolation(const INavigable4Momentum* track,const double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut)
    { return getCellsForIsolation(m_trackExtrapolatorToCalo->getTrackParameters(track),dR, first_layer, last_layer, useLooseCut); };
    std::vector<const CaloCell*> getCellsForEcore(const INavigable4Momentum* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)
    { return getCellsForEcore(m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut); }
    std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const INavigable4Momentum* track,const double dR, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)
    { return getCellsAroundTrack(m_trackExtrapolatorToCalo->getTrackParameters(track), dR, first_layer, last_layer, useLooseCut); };
    std::vector<std::vector<const CaloCell*> > getCellsAroundTrack(const INavigable4Momentum* track,const double deta,const double dphi, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)
    { return getCellsAroundTrack(m_trackExtrapolatorToCalo->getTrackParameters(track), deta, dphi, first_layer, last_layer, useLooseCut); };
    std::vector<const CaloCell*> getCellsAroundTrackOnLayer(const INavigable4Momentum *track, CaloLayer layer, int neta, int nphi, int dir_eta=0, int dir_phi=0, bool useLooseCut=false)
    { return getCellsAroundTrackOnLayer(m_trackExtrapolatorToCalo->getTrackParameters(track), layer, neta, nphi, dir_eta, dir_phi, useLooseCut); };
    std::vector<const CaloCell*> getCellsOnTrack(const INavigable4Momentum* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)
    { return getCellsOnTrack( m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut ); };
     std::vector<const CaloCell*> getCellsCrossedByTrack(const INavigable4Momentum* track, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)
     { return getCellsCrossedByTrack( m_trackExtrapolatorToCalo->getTrackParameters(track), first_layer, last_layer, useLooseCut ); };
    std::vector<double> getWeightsForCellsCrossed(const INavigable4Momentum* track, bool useLooseCut)
    { return getWeightsForCellsCrossed( m_trackExtrapolatorToCalo->getTrackParameters(track), useLooseCut); };
    double getPathInsideCell(const INavigable4Momentum *track, const CaloCell *cell)
    { return getPathInsideCell(m_trackExtrapolatorToCalo->getTrackParameters(track), cell); };
    double getIsolationWidth(const INavigable4Momentum* track,const double dR, const double EtExp, const double dRExp, bool negRemoval=true, CaloLayer first_layer=ps, CaloLayer last_layer=hec3, bool useLooseCut=false)
    { return getIsolationWidth(m_trackExtrapolatorToCalo->getTrackParameters(track),dR, EtExp, dRExp, negRemoval, first_layer, last_layer, useLooseCut); }
    
    // Isolation for a list of particles
    std::vector<double> getIsolationEnergy(std::vector<const INavigable4Momentum *> tracks, double dR, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut);

    //-----------------------------------------------------------------------------------------
    // For IEnergyDepositionTool
    std::pair<double,double> measurement(CaloCell_ID::CaloSample sample, const Trk::TrackParameters& parm);
//     
    //the core method - calculation of e/et, associated cells and cells on track
    StatusCode getEnergyAndCells(const Trk::TrackParameters* track,std::vector<double> conesize,std::vector< std::vector<double> > &etot,std::vector< std::vector<ListOfCells> > &usedCells, bool useLooseCut=false, bool isEt=true);
    // Get the cell layer according to the internal convention (CaloLayer)
    int getLayer(const CaloCell *cell);
    void setNoiseThreshold(double threshold);
    double getNoiseThreshold() { return m_cellNoiseRMSCut; };

  private:
    // Properties (that can be set via jobOptions)
    double              m_cellNoiseRMSCut;
    double              m_cellNoiseRMSCutLoose;
    bool                m_useExtrapolation;
    ToolHandle<ICaloNoiseTool> m_noiseTool;
    std::string         m_cellContainerName;
    bool m_negativeCells;
    std::vector<std::string> m_eCoreDef;
    bool m_cosmics;
    bool m_calibCrossed;
    
    // For the listener
    void m_clear();
    // Pointers and values that are reset at the end of event by m_clear
    const Trk::TrackParameters *m_currentTrackParameters;
    std::vector<const CaloCell*> m_cellsOnTrack;
    std::vector<std::vector<const CaloCell*> > m_cellsPerLayer;
    std::vector <std::pair<double, double> > m_extrapolations;
    double m_cone;
    std::vector<double> m_weightsForCellsCrossed;
    
    // Calorimeter pointers
    const CaloDetDescrManager* m_calo_dd;
    const CaloCell_ID* m_caloCell_ID;
    const TileDetDescrManager* m_tileDDM;
    
    StatusCode m_noiseToolSc; 
    std::vector<CaloCell_ID::SUBCALO> m_subCalos;
    // Extrapolator
    ITrackExtrapolatorToCalo *m_trackExtrapolatorToCalo;
    
    bool newTrack(const Trk::TrackParameters* track);
    
    StatusCode getCellsForCone(double cone);
    int getLayer(CaloCell_ID::CaloSample calo);
    void getCaloSamples();
    StatusCode getExtrapolations(const Trk::TrackParameters* track, bool usePosition);
    bool isCellOnTrack(const CaloCell *cell, double eta, double phi);
    bool accept(const CaloCell *cell, bool useLooseCut=false);
    void selectCells(std::vector<const CaloCell*> &cells, bool useLooseCut=false);
    void getDirections(std::vector<const CaloCell*> cells, int neta, int nphi, int *dir_eta, int *dir_phi, double eta=-9, double phi=0);
    void selectNeighbours(std::vector<const CaloCell*> &cells, int neta, int nphi, int dir_eta, int dir_phi, double eta=-9, double phi=0);
    
    // For eCore definition
    ConeDefinition *m_eCore;
    void setEcoreDefinition(std::vector<std::string> eCoreDef) { m_eCore->setConeDefinition(eCoreDef); };
    void print_EcoreDefinition() { m_eCore->printConeDefinition(); };

    
    // Utils
    std::vector<double> getParametersForCellSelection(CaloCell_ID::SUBCALO subCalo);
    std::vector<double> getMidPointAndDistance(std::pair<double, double> p1, std::pair<double, double> p2);
    
    std::vector<const CaloCell*> getCellsForConeDef(const Trk::TrackParameters* track, CaloLayer first_layer, CaloLayer last_layer, bool useLooseCut, ConeDefinition *coneDef, bool usePosition);
    double getLargestConeNeeded(ConeDefinition *coneDef);
    double getConeSizeForDef(int ilayer, ConeDefinition *coneDef);
    void removeCoreFromCone(std::vector< std::vector< const CaloCell*> > &cells, std::vector< const CaloCell*> cells_core, CaloLayer first_layer, CaloLayer last_layer);
    void selectCellsByLayer(std::vector<const CaloCell*> &cells, int layer);
    void selectCellsBySample(std::vector<const CaloCell*> &cells, CaloCell_ID::CaloSample sample);
    double pathInsideCell(const CaloCell *cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit);
    bool cellCrossedByTrack(const CaloCell *cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit);
    bool crossedPhi(const CaloCell *cell, double phi_entrance, double phi_exit);
    double getPathLengthInEta(const CaloCell *cell, double eta_entrance, double eta_exit);
    double getPathLengthInZ(const CaloCell *cell, double z_entrance, double z_exit);
    double getPathLengthInZ(double zMin, double zMax, double z_entrance, double z_exit);
    double getPathLengthInZTileBar1(const CaloCell *cell, double z_entrance, double z_exit);  

    double deltaR(const Trk::TrackParameters *a, const Trk::TrackParameters *b);
    double phiMean(double a, double b) { return 0.5*(a+b) + (a*b < 0)*M_PI; };
    double square(double x) { return x*x; };
    int sign(double x, double min = 1e-5);
//     int round(double x);

    
    TrackInCaloTools(const TrackInCaloTools&);
    TrackInCaloTools &operator= (const TrackInCaloTools&);
};
#endif //TRACKINCALOTOOLS_H
