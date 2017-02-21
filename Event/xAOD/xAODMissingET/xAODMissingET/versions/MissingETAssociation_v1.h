// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGETASSOCIATION_v1_H
#define XAODMISSINGET_VERSIONS_MISSINGETASSOCIATION_v1_H

#include "AthContainers/AuxElement.h"

#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODMissingET/versions/MissingETCompositionBase.h"
#include "xAODMissingET/versions/MissingET_v1.h"

#include <vector>
#include <set>

namespace xAOD
{
  /*!  @brief MET association descriptor contains object links and corresponding parameters */ 
  class MissingETAssociation_v1 : public SG::AuxElement
  {
  public:

    static bool testPolicy(unsigned int type, MissingETBase::UsageHandler::Policy p);

    /*! @brief Vector sum of constituents for subtractive overlap removal */                                                                                                            
    class ConstVec
    {
    public:
      ConstVec();                                 /*!< @brief Default constructor */
      ConstVec(double cpx,double cpy,double cpz,double ce,double sumpt); /*!< @brief Constructor with parameters */
      ConstVec(const IParticle& ipar);            /*!< @brief Constructor from IParticle */
      // ConstVec(const ConstVec& cvec);             /*!< @brief Copy constructor */
      ~ConstVec();                                /*!< @brief Data class destructor */
      /*! @name Accessors */
      /*!@{*/
      double cpx() const; /*!< @brief Returns @f$ c_{x} @f$ */
      double cpy() const; /*!< @brief Returns @f$ c_{y} @f$ */
      double cpz() const; /*!< @brief Returns @f$ c_{z} @f$ */
      double cpt() const; /*!< @brief Returns @f$ c_{T} @f$ */
      double ce() const;  /*!< @brief Returns @f$ c_{e} @f$ */
      double sumpt() const;  /*!< @brief Returns sum of component pt */
      /*!@}*/
      /*! @name Setters */
      /*!@{*/
      void setCpx(double px); /*!< @brief Set @f$ c_{x} @f$ */
      void setCpy(double py); /*!< @brief Set @f$ c_{y} @f$ */
      void setCpz(double pz); /*!< @brief Set @f$ c_{z} @f$ */
      void setCe (double e);  /*!< @brief Set @f$ c_{e} @f$ */
      void setSumpt (double sumpt);  /*!< @brief Set sumpt */
      /*!@}*/
      /*! @name Modifiers for constituent vector */
      /*!@{*/
      double& cpx(); /*!< @brief Returns reference to modifiable data word storing @f$ c_{x} @f$ */
      double& cpy(); /*!< @brief Returns reference to modifiable data word storing @f$ c_{y} @f$ */
      double& cpz(); /*!< @brief Returns reference to modifiable data word storing @f$ c_{z} @f$ */
      double& ce();  /*!< @brief Returns reference to modifiable data word storing @f$ c_{e} @f$ */
      double& sumpt(); /*!< @brief Returns reference to modifiable data word storing sum of component pt */
      ConstVec& operator+=(const ConstVec& cvec); /*!< @brief Add another vector */
      ConstVec& operator-=(const ConstVec& cvec); /*!< @brief Subtract another vector */
      ConstVec& operator*=(double scale);         /*!< @brief Scale vector by multiplication */ 
      ConstVec& operator/=(double scale);         /*!< @brief Scale vector by division */
      /*!@}*/
      /*! @name Comparators */
      /*!@{*/
      bool operator==(const ConstVec& cvec) const; /*!< @brief Equality comparator */
      bool operator!=(const ConstVec& cvec) const; /*!< @brief Inequality operator */
      /*!@}*/
     private:
      /*! @name Cache for vectors */
      /*!@{*/
      double m_cpx; /*!< @brief @f$ P_{x} @f$ */
      double m_cpy; /*!< @brief @f$ P_{y} @f$ */
      double m_cpz; /*!< @brief @f$ P_{y} @f$ */
      double m_ce;  /*!< @brief @f$ E @f$ */
      double m_sumpt;  /*!< @brief @f$ E @f$ */
      /*!@}*/
    }; // MissingETComponent_v1::ConstVec

    explicit MissingETAssociation_v1(bool createStore=false);     /*!< @brief Default constructor */
    MissingETAssociation_v1(const Jet* pjet, bool isMisc=false); /*!< @brief Construct with a link to a Jet object */
    MissingETAssociation_v1(const MissingETAssociation_v1& assocDescr);                                    /*!< @brief Copy constructor */
    MissingETAssociation_v1& operator=(const MissingETAssociation_v1& assocDescr);                         /*! @brief Assignment operator */
    virtual ~MissingETAssociation_v1();                                                                 /*! @brief Base class destructor */

    /*! @name Set reference jet
     *  @anchor contrib_set
     */
    /*!@{*/
    /*! @brief Set reference jet by object pointer reference */
    bool setRefJet(const Jet* pJet);
    /*! @brief Set reference jet by container pointer and index reference */
    bool setRefJet(const JetContainer* pJetCont,size_t pJetIdx);
    /*!@}*/
 
    /*! @name Adding data to the MET association description
     *  @anchor contrib_add
     */
    bool addObject(const IParticle* pPart,const std::vector<const IParticle*>& constlist);     /*!< @brief Add object (particle) from pointer reference with individual constituent vectors */
    /*!@}*/

    /*! @name Conventional and dedicated getter and setters 
     *  @anchor contrib_convention
     */
    /*!@{*/
    const std::vector<double>& calpx() const;                /*!< @brief Get the vector of @f$ P_{x} @f$  */
    double calpx(const IParticle* pPart) const;              /*!< @brief Get the vector @f$ P_{x} @f$ for a given object referenced by pointer */
    double calpx(size_t keyIdx) const;                       /*!< @brief Get the vector @f$ P_{x} @f$ for a given object referenced by index */
    //
    bool setCalPx(const std::vector<double>& calpxvec);      /*!< @brief Set the vector of @f$ P_{x} @f$  */
    bool setCalPx(size_t keyIdx,double calpx);               /*!< @brief Set calo constituent vector @f$ P_{x} @f$ for a given contributing object referenced by index */
    //
    const std::vector<double>& calpy() const;                /*!< @brief Get the vector of @f$ P_{y} @f$  */
    double calpy(size_t keyIdx) const;                       /*!< @brief Get the vector @f$ P_{y} @f$ for a given object referenced by index */
    //
    bool setCalPy(const std::vector<double>& calpyvec);      /*!< @brief Set the vector of @f$ P_{y} @f$  */
    bool setCalPy(size_t keyIdx,double calpy);               /*!< @brief Set calo constituent vector @f$ P_{y} @f$ for a given contributing object referenced by index */
    //
    const std::vector<double>& calpz() const;                /*!< @brief Get the vector of @f$ P_{z} @f$  */
    double calpz(size_t keyIdx) const;                       /*!< @brief Get the vector @f$ P_{z} @f$ for a given object referenced by index */
    //
    bool setCalPz(const std::vector<double>& calpzvec);      /*!< @brief Set the vector of @f$ P_{z} @f$  */
    bool setCalPz(size_t keyIdx,double calpz);               /*!< @brief Set calo constituent vector @f$ P_{z} @f$ for a given contributing object referenced by index */
    //
    const std::vector<double>& cale() const;                 /*!< @brief Get the vector of @f$ E @f$  */
    double cale(size_t keyIdx) const;                        /*!< @brief Get the vector @f$ E @f$ for a given object referenced by index */
    //
    bool setCalE(const std::vector<double>& calevec);        /*!< @brief Set the vector of @f$ E @f$  */
    bool setCalE(size_t keyIdx,double cale);                 /*!< @brief Set calo constituent vector @f$ E @f$ for a given contributing object referenced by index */
    //
    const std::vector<double>& calsumpt() const;             /*!< @brief Get the vector of sumpt  */
    double calsumpt(size_t keyIdx) const;                    /*!< @brief Get the sumpt for a given object referenced by index */
    //
    bool setCalSumpt(const std::vector<double>& calsumptvec); /*!< @brief Set the vector of sumpt  */
    bool setCalSumpt(size_t keyIdx,double calsumpt);          /*!< @brief Set calo constituent sumpt for a given contributing object referenced by index */
    //
    const std::vector<MissingETBase::Types::bitmask_t>& calkey() const;   /*!< @brief Get the vector of cal keys */
    MissingETBase::Types::bitmask_t calkey(size_t keyIdx) const;          /*!< @brief Get the key for a given entry referenced by index */
    //
    bool setCalKey(const std::vector<MissingETBase::Types::bitmask_t>& calkeyvec); /*!< @brief Set the vector of keys  */
    bool setCalKey(size_t keyIdx,MissingETBase::Types::bitmask_t calkey);          /*!< @brief Set calo key for a given contributing entry referenced by index */
    //
    bool setCalVec(size_t keyIdx,const ConstVec& cvec=ConstVec());           /*!< @brief Set the calo constituent vector of an object contribution referenced by index */
    bool setCalVec(size_t keyIdx,double calpx,double calpy,double calpz,double ce,double sumpt);            /*!< @brief Set the calo constituent vectors of an object contribution referenced by index */
    //
    bool addCalVec(MissingETBase::Types::bitmask_t key,double calpx,double calpy,double calpz,double cale,double sumpt);            /*!< @brief Add a key/vector pair for calo contributions */
    //
    bool clearCalVecs();           /*!< @brief Reset the calo constituent vectors and keys */
    //
    /*!@{*/
    const std::vector<double>& trkpx() const;                /*!< @brief Get the vector of @f$ P_{x} @f$  */
    double trkpx(size_t keyIdx) const;                       /*!< @brief Get the vector @f$ P_{x} @f$ for a given object referenced by index */
    //
    bool setTrkPx(const std::vector<double>& trkpxvec);      /*!< @brief Set the vector of @f$ P_{x} @f$  */
    bool setTrkPx(size_t keyIdx,double trkpx);               /*!< @brief Set track constituent vector @f$ P_{x} @f$ for a given contributing object referenced by index */
    //
    const std::vector<double>& trkpy() const;                /*!< @brief Get the vector of @f$ P_{y} @f$  */
    double trkpy(size_t keyIdx) const;                       /*!< @brief Get the vector @f$ P_{y} @f$ for a given object referenced by index */
    //
    bool setTrkPy(const std::vector<double>& trkpyvec);      /*!< @brief Set the vector of @f$ P_{y} @f$  */
    bool setTrkPy(size_t keyIdx,double trkpy);               /*!< @brief Set track constituent vector @f$ P_{y} @f$ for a given contributing object referenced by index */
    //
    const std::vector<double>& trkpz() const;                /*!< @brief Get the vector of @f$ P_{z} @f$  */
    double trkpz(size_t keyIdx) const;                       /*!< @brief Get the vector @f$ P_{z} @f$ for a given object referenced by index */
    //
    bool setTrkPz(const std::vector<double>& trkpzvec);      /*!< @brief Set the vector of @f$ P_{z} @f$  */
    bool setTrkPz(size_t keyIdx,double trkpz);               /*!< @brief Set track constituent vector @f$ P_{z} @f$ for a given contributing object referenced by index */
    //
    const std::vector<double>& trke() const;                 /*!< @brief Get the vector of @f$ E @f$  */
    double trke(size_t keyIdx) const;                        /*!< @brief Get the vector @f$ E @f$ for a given object referenced by index */
    //
    bool setTrkE(const std::vector<double>& trkevec);        /*!< @brief Set the vector of @f$ E @f$  */
    bool setTrkE(size_t keyIdx,double trke);                 /*!< @brief Set track constituent vector @f$ E @f$ for a given contributing object referenced by index */
    //
    const std::vector<double>& trksumpt() const;             /*!< @brief Get the vector of @f$ E @f$  */
    double trksumpt(size_t keyIdx) const;                    /*!< @brief Get the vector @f$ E @f$ for a given object referenced by index */
    //
    bool setTrkSumpt(const std::vector<double>& trksumptvec); /*!< @brief Set the vector of @f$ E @f$  */
    bool setTrkSumpt(size_t keyIdx,double trksumpt);          /*!< @brief Set track constituent vector @f$ E @f$ for a given contributing object referenced by index */
    //
    const std::vector<MissingETBase::Types::bitmask_t>& trkkey() const;   /*!< @brief Get the vector of trk keys */
    MissingETBase::Types::bitmask_t trkkey(size_t keyIdx) const;          /*!< @brief Get the key for a given entry referenced by index */
    //
    bool setTrkKey(const std::vector<MissingETBase::Types::bitmask_t>& trkkeyvec); /*!< @brief Set the vector of keys  */
    bool setTrkKey(size_t keyIdx,MissingETBase::Types::bitmask_t trkkey);          /*!< @brief Set trk key for a given contributing entry referenced by index */
    //
    bool setTrkVec(size_t keyIdx,const ConstVec& cvec=ConstVec());           /*!< @brief Set the track constituent vector of an object contribution referenced by index */
    bool setTrkVec(size_t keyIdx,double trkpx,double trkpy,double trkpz,double trke,double sumpt);            /*!< @brief Set the track constituent vectors of an object contribution referenced by index */
    //
    bool addTrkVec(MissingETBase::Types::bitmask_t key,double trkpx,double trkpy,double trkpz,double ce,double sumpt);            /*!< @brief Add a key/vector pair for calo contributions */
    //
    bool clearTrkVecs();           /*!< @brief Reset the track constituent vectors and keys */
    //
    /*!@{*/
    double jettrkpx() const;              /*!< @brief Get the vector @f$ P_{x} @f$ for a given object referenced by index */
    double jettrkpy() const;              /*!< @brief Get the vector @f$ P_{y} @f$ for a given object referenced by pointer */
    double jettrkpz() const;              /*!< @brief Get the vector @f$ P_{z} @f$ for a given object referenced by pointer */
    double jettrke() const;               /*!< @brief Get the vector @f$ E @f$ for a given object referenced by index */
    double jettrksumpt() const;           /*!< @brief Get the sumpt for a given object referenced by index */
    //
    void setJetTrkPx(double px);    /*!< @brief Set track constituent vector @f$ P_{x} @f$ for the reference jet */
    void setJetTrkPy(double py);    /*!< @brief Set track constituent vector @f$ P_{y} @f$ for the reference jet */
    void setJetTrkPz(double pz);    /*!< @brief Set track constituent vector @f$ P_{z} @f$ for the reference jet */
    void setJetTrkE(double e);      /*!< @brief Set track constituent vector @f$ E @f$ for the reference jet */
    void setJetTrkSumpt(double sumpt);      /*!< @brief Set track constituent vector @f$ E @f$ for the reference jet */
    void setJetTrkVec(const ConstVec& cvec=ConstVec());  /*!< @brief Set the track constituent vector for the reference jet */
    void setJetTrkVec(double px,double py,double pz,double ce,double sumpt);    /*!< @brief Set the track constituent vector for the reference jet */
    //
    const MissingETBase::Types::jetlink_t& jetLink() const;                     /*!< @brief Get the link to the reference jet */
    bool setJetLink(const MissingETBase::Types::jetlink_t& jetLnk);             /*!< @brief Set the link to the reference jet */
    const MissingETBase::Types::objlink_vector_t& objectLinks() const;          /*!< @brief Get the vector of links to the contributing physics or signal objects */
    bool setObjectLinks(const MissingETBase::Types::objlink_vector_t& objLnks); /*!< @brief Set the vector of links to the contributing physics or signal objects */

    //
    const std::vector<std::vector<size_t> >& overlapIndices() const;            /*!< @brief Get the list of object overlapIndices */
    std::vector<size_t> overlapIndices(const IParticle* pPart) const;           /*!< @brief Get the overlap indices for a given object referenced by pointer */
    std::vector<size_t> overlapIndices(size_t objIdx) const;                    /*!< @brief Get the overlap indices for a given object referenced by index */
    //
    const std::vector<std::vector<unsigned char> >& overlapTypes() const;            /*!< @brief Get the list of object overlapTypes */
    std::vector<unsigned char> overlapTypes(const IParticle* pPart) const;           /*!< @brief Get the overlap types for a given object referenced by pointer */
    std::vector<unsigned char> overlapTypes(size_t objIdx) const;                    /*!< @brief Get the overlap types for a given object referenced by index */
    //
    void clearOverlaps();  /*!< @brief Clear the vectors of overlap indices  */
    bool setOverlaps(const std::vector<std::vector<size_t> >& overlapIndices,
		     const std::vector<std::vector<unsigned char> >& overlapTypes);            /*!< @brief Set the vector of overlap indices and types */
    bool setOverlaps(const IParticle* pPart,const std::vector<size_t>& overlapIndices,
		     const std::vector<unsigned char>& overlapTypes);  /*!< @brief Set overlap indices and types for a given contributing object referenced by pointer */
    bool setOverlaps(size_t objIdx,const std::vector<size_t>& overlapIndices,
		     const std::vector<unsigned char>& overlapTypes);           /*!< @brief Set overlap indices and types for a given contributing object referenced by index */
    void setOverrideMom(std::map<const IParticle*,ConstVec> pOverride);         /*!< @brief Set override momenta */
    std::map<const IParticle*,ConstVec> overrideMom();                          /*!< @brief Get override momenta */
    void addOverrideMom(std::map<const IParticle*,ConstVec> &pOverride);        /*!< @brief Extend override momentum map */

    /*!@}*/

    /*! @name Specific methods for manipulating the association description data 
     *  @anchor contrib_manip
     */
    /*!@{*/
    bool removeContrib(const IParticle* pPart);               /*!< @brief Remove a contribution referenced by an object pointer */
    bool removeContrib(size_t objIdx);                        /*!< @brief Remove a contribution referenced by an index */
    bool removeContrib();                                     /*!< @brief Remove all contributions */
    bool resetContrib(const IParticle* pPart);                /*!< @brief Reset the contribution parameters of an object referenced by a pointer */
    bool resetContrib(size_t objIdx);                         /*!< @brief Reset the contribution parameters of an object referenced by an index */
    bool resetContrib();                                      /*!< @brief Reset all contribution parameters */
    void resetCache();                                        /*!< @brief reset cache for overlap removal decisions */
    /*!@}*/

    /*! @name Dedicated accessors for linked objects */
    /*!@{*/
    const Jet* refJet() const;                                /*!< @brief Access reference jet */
    const JetContainer* refJetContainer() const;              /*!< @brief Access reference jet container holding reference jet */
    size_t refJetIndex() const;                               /*!< @brief Access index of reference jet in its container */
    std::vector<const IParticle*> objects() const;            /*!< @brief Access contributing objects */
    std::vector<const IParticle*> objects(std::vector<ConstVec>& calVecs,std::vector<ConstVec>& trkVecs) const; /*!< @brief Access contributing objects and retrieve constituent vectors */
    std::vector<const IParticle*> objects(const std::vector<double>*& calpxPtr,    
					  const std::vector<double>*& calpyPtr,
					  const std::vector<double>*& calpzPtr,
					  const std::vector<double>*& calePtr,
					  const std::vector<double>*& calsumptPtr,
					  const std::vector<double>*& trkpxPtr,
					  const std::vector<double>*& trkpyPtr,
					  const std::vector<double>*& trkpzPtr,
					  const std::vector<double>*& trkePtr,
					  const std::vector<double>*& trksumptPtr) const; /*!< @brief Access contributing objects and retrieve constituent vectors */
    ConstVec calVec(const IParticle* pPart) const;            /*!< @brief Get calo constituent vector for a given object */
    ConstVec calVec(size_t keyIdx) const;                     /*!< @brief Get calo constituent vector for a given entry (index) in the contributing object list */
    ConstVec trkVec(const IParticle* pPart) const;            /*!< @brief Get track constituent vector for a given object */
    ConstVec trkVec(size_t keyIdx) const;                     /*!< @brief Get track constituent vector for a given entry (index) in the contributing object list */
    ConstVec jetTrkVec() const;                               /*!< @brief Get track constituent vector for the reference jet */
    /*!@}*/ 

    /*! @name Adding and retrieving selection and overlap information */
    void setObjSelectionFlag(size_t objIdx, bool status) const;           /*!< @brief Flag object as selected for MET calculation */
    void setObjSelectionFlag(const IParticle* pPart, bool status) const;  /*!< @brief Flag object as selected for MET calculation */
    void resetObjSelectionFlags() const;                                  /*!< @brief Reset flags */

    bool objSelected(size_t objIdx) const;                                /*!< @brief Test if object is selected for MET calculation */
    bool objSelected(const IParticle* pPart) const;                       /*!< @brief Test if object is selected for MET calculation */

    bool addOverlap(const IParticle* pPart,size_t index, unsigned char type);  /*!< @brief Add overlap index for a given contributing object referenced by pointer */
    bool addOverlap(size_t objIdx,size_t index, unsigned char type);           /*!< @brief Add overlap index for a given contributing object referenced by index */

    bool identifyOverlaps();                                              /*!< @brief Loop over contributing objects to identify those that share constituents */
    bool identifyOverlaps(size_t objIdx);                                 /*!< @brief Search association for particles that share constituents with this one */
    bool identifyOverlaps(const IParticle* pPart);                        /*!< @brief Search association for particles that share constituents with this one */

    bool hasOverlaps(size_t objIdx,MissingETBase::UsageHandler::Policy p=MissingETBase::UsageHandler::OnlyCluster) const;
                                                                          /*!< @brief Check if any selected particles overlap this one */
    bool hasOverlaps(const IParticle* pPart,
		     MissingETBase::UsageHandler::Policy p=MissingETBase::UsageHandler::OnlyCluster) const;
                                                                          /*!< @brief Check if any selected particles overlap this one */

    bool hasAlternateConstVec() const;
    xAOD::JetFourMom_t getAlternateConstVec() const;

    ConstVec overlapCalVec() const;                                       /*!< @brief Retrieve total cluster-based vector to be subtracted from the jet */
    ConstVec overlapTrkVec() const;                                       /*!< @brief Retrieve total track-based vector to be subtracted from the jet */

    bool containsSignal(const IParticle* pSig) const;                     /*!< @brief Check if this signal object matches the constituents of any contributing objects */
    bool containsPhysics(const IParticle* pPhys) const;                   /*!< @brief Check if this physics object matches any contributing objects */
    bool checkUsage(const IParticle* pSig,MissingETBase::UsageHandler::Policy p) const; /*!< @brief Check if this signal object matches the constituents of any flagged contributing objects */
    bool isMisc() const;                                                  /*!< @brief Check if this association is a miscellaneous association */

    /*! @name Allocating and accessing objects in the contribution
     *  @anchor contrib_alloc
     */
    /*! @brief Find index of given object in contributing object store 
     * 
     *  The returned index is valid for the contributing object store and the parameter stores (index-parallel stores). 
     *
     *  @return A valid index @f$ i \in \{ 0 \ldots N_{\rm objects} - 1 \} @f$, where @f$ N_{\rm objects} @f$ is the total number of contributing objects to the
     *          MET contribution. If the given object is not in the store with the requested status, MissingETBase::Numerical::invalidIndex() is returned.
     *
     *
     *  This method makes use of a cache. If the pointer to the checked object is the same as the last one successfully searched for, the index is returned from 
     *  a cached data word. If the pointer is not the same, xAOD::MissingETAssociation::findIndex(const IParticle*) is invoked. If this method finds the referenced
     *  object, the  cache is updated and the valid index is returned. In case the object is not found, MissingETBase::Nummerical::invalidIndex() is returned.   
     *
     *  @return Valid object index @f$ i = 0 \ldots N_{\rm objects} - 1 @f$ if object found in list, else MissingETBase::Numerical::invalidIndex().
     *
     *  @param[in] pPart  pointer to non-modifiable object
     *  @param[in] pPart pointer to non-modifiable object in the list of contributing objects
     */
    size_t findIndex(const IParticle* pPart) const;
    /*!@}*/
    size_t findCalIndex(MissingETBase::Types::bitmask_t mask) const;
    size_t findTrkIndex(MissingETBase::Types::bitmask_t mask) const;

    /*! @name List management and information */
    /*!@{*/
    /*! @brief Update all internally used ElementLink instances 
     * 
     *  @note [Deprecated] This method scans the full list of contributing objects. It invokes
     *        MissingETAssociation_v1::updateJetlink internally.
     */
    // void updateLinks();
    // /*! @brief [Deprecated] Update link to reference jet only */
    // void updateJetLink();
    size_t size() const;
    size_t sizeCal() const;
    size_t sizeTrk() const;
    /*! @brief Empty list of contributing objects indicator */
    bool empty() const;
    /*!@}*/

    /*! @name Comparators
     *
     *  @param[in] contrib reference to non-modifiable contribution object to compare to. 
     */
    /*!@{*/
    bool operator==(const MissingETAssociation_v1& contrib) const; /*! @brief Equality */
    bool operator!=(const MissingETAssociation_v1& contrib) const;
    /*!@}*/

  protected:
    /// Function initialising the object to work in standalone mode
    void createPrivateStore();

    /*! @name Internal non-const accessors 
     *
     *  The underlying data model uses the auxiliary store MissingETAuxComposition_v1, which only employs low level data structures and ElementLinks. The accessors
     *  forward their specific request to static accessors, which retrieve the data by internally used names. 
     */
    /*!@{*/
    MissingETBase::Types::objlink_vector_t&  f_objectLinks(); /*!< @brief Non-const link reference */
    MissingETBase::Types::jetlink_t&         f_jetLink();     /*!< @brief Non-const link reference  */
    //
    std::vector<double>&                     f_calpx();
    std::vector<double>&                     f_calpy();
    std::vector<double>&                     f_calpz();
    std::vector<double>&                     f_cale();
    std::vector<double>&                     f_calsumpt();
    std::vector<MissingETBase::Types::bitmask_t>& f_calkey();
    //
    std::vector<double>&                     f_trkpx();
    std::vector<double>&                     f_trkpy();
    std::vector<double>&                     f_trkpz();
    std::vector<double>&                     f_trke();
    std::vector<double>&                     f_trksumpt();
    std::vector<MissingETBase::Types::bitmask_t>& f_trkkey();
    //
    double&                                  f_jettrkpx();
    double&                                  f_jettrkpy();
    double&                                  f_jettrkpz();
    double&                                  f_jettrke();
    double&                                  f_jettrksumpt();
    //
    char&                                    f_isMisc();
    //
    std::vector<std::vector<size_t> >&       f_overlapIndices(); /*!< @brief record overlapIndices between constributing objects */
    std::vector<std::vector<unsigned char> >& f_overlapTypes();  /*!< @brief record overlapTypes between constributing objects */
    /*!@}*/

    /*! @name Helpers */
    /*!@{*/
    /*! @brief Copy the contribution data from a source
     *     *  @param[in] contrib reference to non-modifiable contribution object (the source). 
     */
    bool f_removeContrib(size_t objIdx);
    void copyData(const MissingETAssociation_v1& assocDescr);
    /*! @brief Generalized link manager
     *
     *  Inserts link data into ElementLink, even if incomplete. At least, the pointer of linked object will be set (except if NULL). 
     *
     *  @tparam POBJ linked object type
     *  @tparam LINK specific ElementLink type
     *
     *  @param[in] pObj     pointer to non-modifiable object to be linked
     *  @param[in] elemLink reference to modifiable ElementLink object. Initial data content of this object are overwritten.
     *
     *  @note This is the generalized implementation for e.g. MissingETAssociation_v1::setMET and MissingETAssociation_v1::addObject
     */
    template<class POBJ,class LINK> void f_setObject(const POBJ* pObj,LINK& elemLink);
    template<class LINK> bool f_setLink(LINK& elemLink);
    /*!@}*/

    /*! @name Management of local cache */

    void initCache(); /*!< @brief initialise cache for overlap removal decisions */

    /*! @name Internal data for local cache */
    /*!@{*/
    mutable std::vector<std::vector<ElementLink<IParticleContainer> > > m_objConstLinks;   /*!< @brief Cache object constituents for overlap-finding
											    */
    mutable MissingETBase::Types::bitmask_t m_useObjectFlags;  /*< @brief Cache overlap removal decisions
								*/
    mutable std::vector<const IParticle*> m_contribObjects;  /*< @brief Cache pointers to accepted objects
							      */
    /*!@}*/
  private:
    mutable std::map<const IParticle*,ConstVec> m_override;
    /*! @name Cache remembering the previous search result */
    /*!@{*/
    mutable const IParticle* m_lastObjectPointer; /*!< @brief Pointer to last searched object */
    mutable size_t           m_lastObjectIndex;   /*!< @brief Index of last searched object in list */
    /*!@}*/

    static size_t m_objConstLinkReserve;
    static size_t m_contribObjReserve;

  }; // MissingETAssociation_v1
}

namespace MissingETBase
{
  namespace Types
  {
    typedef xAOD::MissingETAssociation_v1::ConstVec constvec_t; /*!< @brief Type for constituent vector */
    typedef std::vector<constvec_t> constvec_vector_t; /*!< @brief Vector type for constituent vector containers */
  }
}

/*! @file MissingETComponent_v1.h */
/*! @name Global namespace operators for MissingETComponent_v1::MissingETBase::Types::constvec_t */
/*!@{*/
/*! @brief Add constituent vectors */
MissingETBase::Types::constvec_t operator+(const MissingETBase::Types::constvec_t& v0,const MissingETBase::Types::constvec_t& v2);
/*! @brief Subtract constituent vectors */
MissingETBase::Types::constvec_t operator-(const MissingETBase::Types::constvec_t& v0,const MissingETBase::Types::constvec_t& v2);
/*! @brief Scale constituent vector by multiplication */
MissingETBase::Types::constvec_t operator*(const MissingETBase::Types::constvec_t& cvec,double scale);
/*! @brief Scale constituent vector by multiplication */
MissingETBase::Types::constvec_t operator*(double scale,const MissingETBase::Types::constvec_t& cvec);
/*! @brief Scale constituent vector by division */
MissingETBase::Types::constvec_t operator/(const MissingETBase::Types::constvec_t& cvec,double scale);
/*!@}*/
#include "xAODMissingET/versions/MissingETAssociation_v1.icc"
#endif
