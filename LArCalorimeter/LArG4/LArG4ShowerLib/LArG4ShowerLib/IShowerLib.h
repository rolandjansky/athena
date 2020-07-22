/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SHOWER_LIB_ISHOWER_LIB_H
#define SHOWER_LIB_ISHOWER_LIB_H

// STL includes
#include <string>
#include <vector>
#include "LArG4Code/EnergySpot.h"

#include "LArG4ShowerLib/Shower.h"
#include "LArG4ShowerLib/ShowerLibStatistics.h"

// forward declarations
class G4Track;
class TFile;
class TTree;

#include "AtlasHepMC/GenParticle_fwd.h"

// Namespace for the ShowerLib related classes
namespace ShowerLib {


  /**
   *
   *   @short Class for shower library shower lib interface
   *
   *          Define interface for top level shower library access
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *  @author Sasha Glazov, DESY Hamburg, Germany
   *
   * @version \$Id: IShowerLib.h 576158 2013-12-18 16:58:58Z gsedov $
   *
   */

  class IShowerLib {

  public:

     //! empty destructor
    virtual ~IShowerLib() {}

    //! get shower for given G4 track
    virtual std::vector<EnergySpot>* getShower(const G4Track* track, ShowerLibStatistics* stats, int randomShift) const = 0;
    //! get average length of showers for the given energy
    virtual double getContainmentZ(const G4Track* track) const = 0;
    //! get average lateral spread of the showers for the given energy
    virtual double getContainmentR(const G4Track* track) const = 0;
    //! store shower in the library
    virtual bool storeShower(HepMC::ConstGenParticlePtr genParticle,const Shower* shower) = 0;
    //! write library to ROOT file
    virtual bool writeToROOT(TFile* dest) = 0;

    virtual ShowerLibStatistics* createStatistics() const = 0;
    //! print library statistics
    const std::string statistics() const;

    //! get detector tag
    inline virtual const std::string detector() const;
//    //! set detector tag
//    inline virtual void detector(const std::string& det);
    //! get particle tag
    inline virtual int particle_id() const;
//    //! set particle tag
//    inline virtual void particle_id(const int particle_id);
    //! get Release tag
    inline virtual const std::string release() const;
    //! set Release tag
    inline virtual void release(const std::string& release);
    //! get Geometry tag
    inline virtual const std::string geometry() const;
    //! set Geometry tag
    inline virtual void geometry(const std::string& geometry);
    //! get geant version tag
    inline virtual const std::string geantVersion() const;
    //! set geant version tag
    inline virtual void geantVersion(const std::string& version);
    //! get geant 4 physics list name
    inline virtual const std::string physicsList() const;
    //! set geant 4 physics list name
    inline virtual void physicsList(const std::string& list);
    //! get comment
    inline virtual const std::string comment() const;
    //! set given string as comment
    inline virtual void comment(const std::string& comment);
    //! add given string to comment
    inline virtual void addComment(const std::string& comment);

    inline virtual const std::string getName() const { return "IShowerLib"; }

  protected:

    //! default constructor
    IShowerLib() : m_detector("NotSet"), m_particle(0), m_release("NotSet"), m_geometry("NotSet"), m_geantVersion("NotSet"), m_physicsList("NotSet"), m_filled(false) {}
    //! read metadata from the given TTree
    bool readMeta(TTree* source);
    //! write metadata to the given TTree
    bool writeMeta(TTree* dest) const;
    
    virtual const std::string printParameters() const = 0;

    std::string m_detector;                         //!< name of the detector
    int m_particle;                                 //!< ID of the generated particles
    std::string m_release;                          //!< atlas software release
    std::string m_geometry;                         //!< atlas geometry version
    std::string m_geantVersion;                     //!< geant 4 version
    std::string m_physicsList;                      //!< used geant 4 physics list
    std::string m_comment;                          //!< comment

    bool m_filled;                        //!< is the library read from ROOT or from structure file

  };


  // inline functions

  const std::string IShowerLib::detector() const { return m_detector; }
  //void IShowerLib::detector(const std::string& det) { m_detector = det; }

  int IShowerLib::particle_id() const { return m_particle; }
  //void IShowerLib::particle_id(const int particle) { m_particle = particle; }

  const std::string IShowerLib::release() const { return m_release; }
  void IShowerLib::release(const std::string& release) { m_release = release; }

  const std::string IShowerLib::geometry() const { return m_geometry; }
  void IShowerLib::geometry(const std::string& geometry) { m_geometry = geometry; }

  const std::string IShowerLib::geantVersion() const { return m_geantVersion; }
  void IShowerLib::geantVersion(const std::string& version) { m_geantVersion = version; }

  const std::string IShowerLib::physicsList() const { return m_physicsList; }
  void IShowerLib::physicsList(const std::string& list) { m_physicsList = list; }

  const std::string IShowerLib::comment() const { return m_comment; }
  void IShowerLib::comment(const std::string& comment) { m_comment = comment; }
  void IShowerLib::addComment(const std::string& comment) { m_comment += comment; }

} // namespace ShowerLib

#endif // SHOWER_LIB_ISHOWER_LIB_H
