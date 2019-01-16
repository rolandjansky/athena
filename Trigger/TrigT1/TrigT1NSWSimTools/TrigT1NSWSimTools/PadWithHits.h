// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//S.I 26-07-18 : This class and the structure is too problematic in terms of memory management while using raw pointers
#ifndef NSW_PADWITHITS_H
#define NSW_PADWITHITS_H

#include <string>
#include <vector>

#include "TrigT1NSWSimTools/PadData.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"

namespace NSWL1 {

  struct Pad {
    int ieta;
    int iphi;
    int multiplet;
    int layer;
    int sector;
    int sectortype;
    int side; //!< A(==0) side is z>0, facing LHCb; C(==1) side is z<0, facing ALICE
    int module;

    float m_cornerXyz[4][3]; //!< (x,y,z) coordinates of the 4 pad corners (in the order low to high phi, low to high R)
    
    
    std::shared_ptr<PadData> m_padData;
    
    Pad(const int &eta, const int &phi,
        const int &multi, const int &layerType, const int &sn, const int &si, const int &mod) :
      ieta(eta), iphi(phi), multiplet(multi), layer(layerType), sector(sn), sectortype(sn), side(si), module(mod),
      m_cornerXyz{},m_padData(nullptr)
    { 
        
        
    }

     Pad(std::shared_ptr<PadData> pData ) :
      ieta(pData->padEtaId()), iphi(pData->padPhiId()), multiplet(pData->multipletId()), layer(pData->gasGapId()), sector(pData->sectorId()), sectortype(pData->sectorType()), side(pData->sideId()), module(pData->moduleId()),m_cornerXyz{},m_padData(pData)
    { 
    }
    
    std::string pickle() const; //!< simple dict-like representation (might differ from '<<')

    Pad& fillCornerCoords(float corners[4][3]);
    static int sideFromZ(float z) {return (z>0.0 ? 0:1);}
    
  };
  float midSectorPhi(int sector);         //!< angle phi at the center of the sector
  float phi_mpi_pi(const double &val);    //!< bring phi to [-pi,+pi)
  float phi_zero_2pi(const double &val);  //!< bring phi to [0, +2pi)

/**
  @brief Class that holds two pieces of information: pad geometry and hit indices.
  
  The detector information (ieta, iphi, layer, etc.) must be provided
  trhough the constructor.  The pad geometrical information
  (i.e. surface limits) might require further calculation, and should be
  set with the set(Low/High)* methods.
  The hits on the pad must be added with PadWithHits::addHit.
  
  In the future we might implement some hash function to speed up the
  pad lookup when adding the hits. 

  Note: the 'pickle' representation is meant to be read in python to
  debug single trigger/events by dumping them to a txt file. The idea is
  to write to a string all the datamembers reducing them to basic
  _python_ types (i.e. numbers, strings, lists etc.) contained within a
  dictionary. The key names should be representative of the attribute,
  but drop all the naming convention tattings (e.g. 'm_loPhi' will be
  stored as 'loPhi'). The curly braces delimiting the dictionary
  corresponding to an object are taken care of by the code that does the
  actual dump, not by the object providing its representation; in this
  way, the object does not need to worry about introspection. That is,
  if I am an object of type A providing my 'pickle' representation, I
  don't need to worry whether I am of type A or of type B : A; the code
  that is actually dumping A should know the type, and take care of
  the curly braces appropriately for example doing
  "{A.pickle(), B.pickle()}" or "{'a' : {A.pickle()}, 'b': {B.pickle()}}".
  

  @author davide.gerbaudo@gmail.com
  @date April 2013
*/
  class PadWithHits : public Pad
  {
  public:
    PadWithHits(const int &ieta, const int &iphi,
                const int &multiplet,
                const int &layerType, const int &sector, const int &sectortype, const int &side, const int &module);
    PadWithHits(const Pad &p);
    
    PadWithHits(std::shared_ptr<PadData> pData);
    
    const std::vector<size_t>& hitIndices() const { return m_hitIndices; }
    float totEnergy() const { return m_totEnergy; }
    float avgEta() const { return m_avgEta; }
    float avgPhi() const { return m_avgPhi; }
    std::string pickle() const; //!< simple dict-like representation
  protected:
    void updateWeightedCoord(const double &eta, const double &phi, const float &charge);
  protected:
    std::vector<size_t> m_hitIndices;
    float m_totEnergy; //!< total energy from all hits
    float m_avgEta;    //!< energy-weighted eta position
    float m_avgPhi;    //!< energy-weighted phi position
    float m_minEta;    //!< minimum charge eta position
    float m_minPhi;    //!< minimum charge phi position
    float m_maxEta;    //!< maximum charge eta position
    float m_maxPhi;    //!< maximum charge phi position

  }; // class PadWithHits

  
  
  // helper functions
  std::vector<size_t> filterBySector(const std::vector<Pad>         &pads, int sector);
  std::vector<size_t> filterBySector(const std::vector<PadWithHits> &pads, int sector);
  std::vector<size_t> filterBySide(const std::vector<Pad>    &pads,const std::vector<size_t> &padSelectedIndices,int side);
  std::vector<size_t> filterBySide(const std::vector<PadWithHits> &pads,const std::vector<size_t> &padSelectedIndices,int side);
  std::vector<size_t> filterByLayer(const std::vector<Pad>    &pads, const std::vector<size_t> &padSelectedIndices,int layer);
  std::vector<size_t> filterByLayer(const std::vector<PadWithHits> &pads,const std::vector<size_t> &padSelectedIndices,int layer);
  std::vector<size_t> filterByMultiplet(const std::vector<Pad>    &pads, const std::vector<size_t> &padSelectedIndices,int multiplet);
  std::vector<size_t> filterByMultiplet(const std::vector<PadWithHits> &pads,const std::vector<size_t> &padSelectedIndices,int multiplet);

} // end namespace nsw

#endif
