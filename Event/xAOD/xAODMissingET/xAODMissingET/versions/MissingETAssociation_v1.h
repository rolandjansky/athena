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
#include "xAODMissingET/MissingETAssociationHelper.h"

#include <vector>
#include <set>

namespace xAOD
{
  // Forward declaration
  class MissingETAssociationHelper;

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
      ConstVec(float cpx,float cpy,float cpz,float ce,float sumpt); /*!< @brief Constructor with parameters */
      ConstVec(const IParticle& ipar);            /*!< @brief Constructor from IParticle */
      // ConstVec(const ConstVec& cvec);             /*!< @brief Copy constructor */
      ~ConstVec();                                /*!< @brief Data class destructor */
      /*! @name Accessors */
      /*!@{*/
      float cpx() const; /*!< @brief Returns @f$ c_{x} @f$ */
      float cpy() const; /*!< @brief Returns @f$ c_{y} @f$ */
      float cpz() const; /*!< @brief Returns @f$ c_{z} @f$ */
      float cpt() const; /*!< @brief Returns @f$ c_{T} @f$ */
      float ce() const;  /*!< @brief Returns @f$ c_{e} @f$ */
      float sumpt() const;  /*!< @brief Returns sum of component pt */
      /*!@}*/
      /*! @name Setters */
      /*!@{*/
      void setCpx(float px); /*!< @brief Set @f$ c_{x} @f$ */
      void setCpy(float py); /*!< @brief Set @f$ c_{y} @f$ */
      void setCpz(float pz); /*!< @brief Set @f$ c_{z} @f$ */
      void setCe (float e);  /*!< @brief Set @f$ c_{e} @f$ */
      void setSumpt (float sumpt);  /*!< @brief Set sumpt */
      /*!@}*/
      /*! @name Modifiers for constituent vector */
      /*!@{*/
      float& cpx(); /*!< @brief Returns reference to modifiable data word storing @f$ c_{x} @f$ */
      float& cpy(); /*!< @brief Returns reference to modifiable data word storing @f$ c_{y} @f$ */
      float& cpz(); /*!< @brief Returns reference to modifiable data word storing @f$ c_{z} @f$ */
      float& ce();  /*!< @brief Returns reference to modifiable data word storing @f$ c_{e} @f$ */
      float& sumpt(); /*!< @brief Returns reference to modifiable data word storing sum of component pt */
      ConstVec& operator+=(const ConstVec& cvec); /*!< @brief Add another vector */
      ConstVec& operator-=(const ConstVec& cvec); /*!< @brief Subtract another vector */
      ConstVec& operator*=(float scale);         /*!< @brief Scale vector by multiplication */ 
      ConstVec& operator/=(float scale);         /*!< @brief Scale vector by division */
      /*!@}*/
      /*! @name Comparators */
      /*!@{*/
      bool operator==(const ConstVec& cvec) const; /*!< @brief Equality comparator */
      bool operator!=(const ConstVec& cvec) const; /*!< @brief Inequality operator */
      /*!@}*/
     private:
      /*! @name Cache for vectors */
      /*!@{*/
      float m_cpx; /*!< @brief @f$ P_{x} @f$ */
      float m_cpy; /*!< @brief @f$ P_{y} @f$ */
      float m_cpz; /*!< @brief @f$ P_{y} @f$ */
      float m_ce;  /*!< @brief @f$ E @f$ */
      float m_sumpt;  /*!< @brief @f$ E @f$ */
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
    const std::vector<float>& calpx() const;                /*!< @brief Get the vector of @f$ P_{x} @f$  */
    float calpx(const IParticle* pPart) const;              /*!< @brief Get the vector @f$ P_{x} @f$ for a given object referenced by pointer */
    float calpx(size_t keyIdx) const;                       /*!< @brief Get the vector @f$ P_{x} @f$ for a given object referenced by index */
    //
    bool setCalPx(const std::vector<float>& calpxvec);      /*!< @brief Set the vector of @f$ P_{x} @f$  */
    bool setCalPx(size_t keyIdx,float calpx);               /*!< @brief Set calo constituent vector @f$ P_{x} @f$ for a given contributing object referenced by index */
    //
    const std::vector<float>& calpy() const;                /*!< @brief Get the vector of @f$ P_{y} @f$  */
    float calpy(size_t keyIdx) const;                       /*!< @brief Get the vector @f$ P_{y} @f$ for a given object referenced by index */
    //
    bool setCalPy(const std::vector<float>& calpyvec);      /*!< @brief Set the vector of @f$ P_{y} @f$  */
    bool setCalPy(size_t keyIdx,float calpy);               /*!< @brief Set calo constituent vector @f$ P_{y} @f$ for a given contributing object referenced by index */
    //
    const std::vector<float>& calpz() const;                /*!< @brief Get the vector of @f$ P_{z} @f$  */
    float calpz(size_t keyIdx) const;                       /*!< @brief Get the vector @f$ P_{z} @f$ for a given object referenced by index */
    //
    bool setCalPz(const std::vector<float>& calpzvec);      /*!< @brief Set the vector of @f$ P_{z} @f$  */
    bool setCalPz(size_t keyIdx,float calpz);               /*!< @brief Set calo constituent vector @f$ P_{z} @f$ for a given contributing object referenced by index */
    //
    const std::vector<float>& cale() const;                 /*!< @brief Get the vector of @f$ E @f$  */
    float cale(size_t keyIdx) const;                        /*!< @brief Get the vector @f$ E @f$ for a given object referenced by index */
    //
    bool setCalE(const std::vector<float>& calevec);        /*!< @brief Set the vector of @f$ E @f$  */
    bool setCalE(size_t keyIdx,float cale);                 /*!< @brief Set calo constituent vector @f$ E @f$ for a given contributing object referenced by index */
    //
    const std::vector<float>& calsumpt() const;             /*!< @brief Get the vector of sumpt  */
    float calsumpt(size_t keyIdx) const;                    /*!< @brief Get the sumpt for a given object referenced by index */
    //
    bool setCalSumpt(const std::vector<float>& calsumptvec); /*!< @brief Set the vector of sumpt  */
    bool setCalSumpt(size_t keyIdx,float calsumpt);          /*!< @brief Set calo constituent sumpt for a given contributing object referenced by index */
    //
    const std::vector<MissingETBase::Types::bitmask_t>& calkey() const;   /*!< @brief Get the vector of cal keys */
    MissingETBase::Types::bitmask_t calkey(size_t keyIdx) const;          /*!< @brief Get the key for a given entry referenced by index */
    //
    bool setCalKey(const std::vector<MissingETBase::Types::bitmask_t>& calkeyvec); /*!< @brief Set the vector of keys  */
    bool setCalKey(size_t keyIdx,MissingETBase::Types::bitmask_t calkey);          /*!< @brief Set calo key for a given contributing entry referenced by index */
    //
    bool setCalVec(size_t keyIdx,const ConstVec& cvec=ConstVec());           /*!< @brief Set the calo constituent vector of an object contribution referenced by index */
    bool setCalVec(size_t keyIdx,float calpx,float calpy,float calpz,float ce,float sumpt);            /*!< @brief Set the calo constituent vectors of an object contribution referenced by index */
    //
    bool addCalVec(MissingETBase::Types::bitmask_t key,float calpx,float calpy,float calpz,float cale,float sumpt);            /*!< @brief Add a key/vector pair for calo contributions */
    //
    bool clearCalVecs();           /*!< @brief Reset the calo constituent vectors and keys */
    //
    /*!@{*/
    const std::vector<float>& trkpx() const;                /*!< @brief Get the vector of @f$ P_{x} @f$  */
    float trkpx(size_t keyIdx) const;                       /*!< @brief Get the vector @f$ P_{x} @f$ for a given object referenced by index */
    //
    bool setTrkPx(const std::vector<float>& trkpxvec);      /*!< @brief Set the vector of @f$ P_{x} @f$  */
    bool setTrkPx(size_t keyIdx,float trkpx);               /*!< @brief Set track constituent vector @f$ P_{x} @f$ for a given contributing object referenced by index */
    //
    const std::vector<float>& trkpy() const;                /*!< @brief Get the vector of @f$ P_{y} @f$  */
    float trkpy(size_t keyIdx) const;                       /*!< @brief Get the vector @f$ P_{y} @f$ for a given object referenced by index */
    //
    bool setTrkPy(const std::vector<float>& trkpyvec);      /*!< @brief Set the vector of @f$ P_{y} @f$  */
    bool setTrkPy(size_t keyIdx,float trkpy);               /*!< @brief Set track constituent vector @f$ P_{y} @f$ for a given contributing object referenced by index */
    //
    const std::vector<float>& trkpz() const;                /*!< @brief Get the vector of @f$ P_{z} @f$  */
    float trkpz(size_t keyIdx) const;                       /*!< @brief Get the vector @f$ P_{z} @f$ for a given object referenced by index */
    //
    bool setTrkPz(const std::vector<float>& trkpzvec);      /*!< @brief Set the vector of @f$ P_{z} @f$  */
    bool setTrkPz(size_t keyIdx,float trkpz);               /*!< @brief Set track constituent vector @f$ P_{z} @f$ for a given contributing object referenced by index */
    //
    const std::vector<float>& trke() const;                 /*!< @brief Get the vector of @f$ E @f$  */
    float trke(size_t keyIdx) const;                        /*!< @brief Get the vector @f$ E @f$ for a given object referenced by index */
    //
    bool setTrkE(const std::vector<float>& trkevec);        /*!< @brief Set the vector of @f$ E @f$  */
    bool setTrkE(size_t keyIdx,float trke);                 /*!< @brief Set track constituent vector @f$ E @f$ for a given contributing object referenced by index */
    //
    const std::vector<float>& trksumpt() const;             /*!< @brief Get the vector of @f$ E @f$  */
    float trksumpt(size_t keyIdx) const;                    /*!< @brief Get the vector @f$ E @f$ for a given object referenced by index */
    //
    bool setTrkSumpt(const std::vector<float>& trksumptvec); /*!< @brief Set the vector of @f$ E @f$  */
    bool setTrkSumpt(size_t keyIdx,float trksumpt);          /*!< @brief Set track constituent vector @f$ E @f$ for a given contributing object referenced by index */
    //
    const std::vector<MissingETBase::Types::bitmask_t>& trkkey() const;   /*!< @brief Get the vector of trk keys */
    MissingETBase::Types::bitmask_t trkkey(size_t keyIdx) const;          /*!< @brief Get the key for a given entry referenced by index */
    //
    bool setTrkKey(const std::vector<MissingETBase::Types::bitmask_t>& trkkeyvec); /*!< @brief Set the vector of keys  */
    bool setTrkKey(size_t keyIdx,MissingETBase::Types::bitmask_t trkkey);          /*!< @brief Set trk key for a given contributing entry referenced by index */
    //
    bool setTrkVec(size_t keyIdx,const ConstVec& cvec=ConstVec());           /*!< @brief Set the track constituent vector of an object contribution referenced by index */
    bool setTrkVec(size_t keyIdx,float trkpx,float trkpy,float trkpz,float trke,float sumpt);            /*!< @brief Set the track constituent vectors of an object contribution referenced by index */
    //
    bool addTrkVec(MissingETBase::Types::bitmask_t key,float trkpx,float trkpy,float trkpz,float ce,float sumpt);            /*!< @brief Add a key/vector pair for calo contributions */
    //
    bool clearTrkVecs();           /*!< @brief Reset the track constituent vectors and keys */
    //
    /*!@{*/
    float jettrkpx() const;              /*!< @brief Get the vector @f$ P_{x} @f$ for a given object referenced by index */
    float jettrkpy() const;              /*!< @brief Get the vector @f$ P_{y} @f$ for a given object referenced by pointer */
    float jettrkpz() const;              /*!< @brief Get the vector @f$ P_{z} @f$ for a given object referenced by pointer */
    float jettrke() const;               /*!< @brief Get the vector @f$ E @f$ for a given object referenced by index */
    float jettrksumpt() const;           /*!< @brief Get the sumpt for a given object referenced by index */
    //
    void setJetTrkPx(float px);    /*!< @brief Set track constituent vector @f$ P_{x} @f$ for the reference jet */
    void setJetTrkPy(float py);    /*!< @brief Set track constituent vector @f$ P_{y} @f$ for the reference jet */
    void setJetTrkPz(float pz);    /*!< @brief Set track constituent vector @f$ P_{z} @f$ for the reference jet */
    void setJetTrkE(float e);      /*!< @brief Set track constituent vector @f$ E @f$ for the reference jet */
    void setJetTrkSumpt(float sumpt);      /*!< @brief Set track constituent vector @f$ E @f$ for the reference jet */
    void setJetTrkVec(const ConstVec& cvec=ConstVec());  /*!< @brief Set the track constituent vector for the reference jet */
    void setJetTrkVec(float px,float py,float pz,float ce,float sumpt);    /*!< @brief Set the track constituent vector for the reference jet */
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
    /*!@}*/

    /*! @name Dedicated accessors for linked objects */
    /*!@{*/
    const Jet* refJet() const;                                /*!< @brief Access reference jet */
    const JetContainer* refJetContainer() const;              /*!< @brief Access reference jet container holding reference jet */
    size_t refJetIndex() const;                               /*!< @brief Access index of reference jet in its container */
    std::vector<const IParticle*> objects() const;            /*!< @brief Access contributing objects */
    std::vector<const IParticle*> objects(std::vector<ConstVec>& calVecs,std::vector<ConstVec>& trkVecs) const; /*!< @brief Access contributing objects and retrieve constituent vectors */
    std::vector<const IParticle*> objects(const std::vector<float>*& calpxPtr,    
					  const std::vector<float>*& calpyPtr,
					  const std::vector<float>*& calpzPtr,
					  const std::vector<float>*& calePtr,
					  const std::vector<float>*& calsumptPtr,
					  const std::vector<float>*& trkpxPtr,
					  const std::vector<float>*& trkpyPtr,
					  const std::vector<float>*& trkpzPtr,
					  const std::vector<float>*& trkePtr,
					  const std::vector<float>*& trksumptPtr) const; /*!< @brief Access contributing objects and retrieve constituent vectors */
    ConstVec calVec(const IParticle* pPart) const;            /*!< @brief Get calo constituent vector for a given object */
    ConstVec calVec(size_t keyIdx) const;                     /*!< @brief Get calo constituent vector for a given entry (index) in the contributing object list */
    ConstVec trkVec(const IParticle* pPart) const;            /*!< @brief Get track constituent vector for a given object */
    ConstVec trkVec(size_t keyIdx) const;                     /*!< @brief Get track constituent vector for a given entry (index) in the contributing object list */
    ConstVec jetTrkVec() const;                               /*!< @brief Get track constituent vector for the reference jet */
    /*!@}*/ 

    /*! @name Adding and retrieving selection and overlap information */
    bool addOverlap(const IParticle* pPart,size_t index, unsigned char type);  /*!< @brief Add overlap index for a given contributing object referenced by pointer */
    bool addOverlap(size_t objIdx,size_t index, unsigned char type);           /*!< @brief Add overlap index for a given contributing object referenced by index */

    bool identifyOverlaps();                                              /*!< @brief Loop over contributing objects to identify those that share constituents */
    bool identifyOverlaps(size_t objIdx);                                 /*!< @brief Search association for particles that share constituents with this one */
    bool identifyOverlaps(const IParticle* pPart);                        /*!< @brief Search association for particles that share constituents with this one */

    bool hasOverlaps(const MissingETAssociationHelper* helper, size_t objIdx,MissingETBase::UsageHandler::Policy p=MissingETBase::UsageHandler::OnlyCluster) const;
                                                                          /*!< @brief Check if any selected particles overlap this one */
    bool hasOverlaps(const MissingETAssociationHelper* helper, const IParticle* pPart,
		     MissingETBase::UsageHandler::Policy p=MissingETBase::UsageHandler::OnlyCluster) const;
                                                                          /*!< @brief Check if any selected particles overlap this one */

    bool hasAlternateConstVec() const;
    xAOD::JetFourMom_t getAlternateConstVec() const;

    ConstVec overlapCalVec(const MissingETAssociationHelper* helper) const;    /*!< @brief Retrieve total cluster-based vector to be subtracted from the jet */
    ConstVec overlapTrkVec(const MissingETAssociationHelper* helper) const;    /*!< @brief Retrieve total track-based vector to be subtracted from the jet */

    bool containsSignal(const IParticle* pSig) const;                     /*!< @brief Check if this signal object matches the constituents of any contributing objects */
    bool containsPhysics(const IParticle* pPhys) const;                   /*!< @brief Check if this physics object matches any contributing objects */
    bool isMisc() const;                                                  /*!< @brief Check if this association is a miscellaneous association */
    bool checkUsage(const MissingETAssociationHelper* helper, const IParticle* pSig, MissingETBase::UsageHandler::Policy p) const; /*!< @brief Check if this signal object matches the constituents of any flagged contributing objects */

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
    std::vector<float>&                     f_calpx();
    std::vector<float>&                     f_calpy();
    std::vector<float>&                     f_calpz();
    std::vector<float>&                     f_cale();
    std::vector<float>&                     f_calsumpt();
    std::vector<MissingETBase::Types::bitmask_t>& f_calkey();
    //
    std::vector<float>&                     f_trkpx();
    std::vector<float>&                     f_trkpy();
    std::vector<float>&                     f_trkpz();
    std::vector<float>&                     f_trke();
    std::vector<float>&                     f_trksumpt();
    std::vector<MissingETBase::Types::bitmask_t>& f_trkkey();
    //
    float&                                  f_jettrkpx();
    float&                                  f_jettrkpy();
    float&                                  f_jettrkpz();
    float&                                  f_jettrke();
    float&                                  f_jettrksumpt();
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
    std::vector<std::vector<ElementLink<IParticleContainer> > > m_objConstLinks;   /*!< @brief Cache object constituents for overlap-finding */
    /*!@}*/
  private:
    std::map<const IParticle*,ConstVec> m_override;

    static const SG::AuxElement::Accessor<std::vector<float> > m_acc_calpx;
    static const SG::AuxElement::Accessor<std::vector<float> > m_acc_calpy;
    static const SG::AuxElement::Accessor<std::vector<float> > m_acc_calpz;
    static const SG::AuxElement::Accessor<std::vector<float> > m_acc_cale;
    static const SG::AuxElement::Accessor<std::vector<float> > m_acc_calsumpt;

    static const SG::AuxElement::Accessor<std::vector<float> > m_acc_trkpx;
    static const SG::AuxElement::Accessor<std::vector<float> > m_acc_trkpy;
    static const SG::AuxElement::Accessor<std::vector<float> > m_acc_trkpz;
    static const SG::AuxElement::Accessor<std::vector<float> > m_acc_trke;
    static const SG::AuxElement::Accessor<std::vector<float> > m_acc_trksumpt;

    static const SG::AuxElement::Accessor<float> m_acc_jettrkpx;
    static const SG::AuxElement::Accessor<float> m_acc_jettrkpy;
    static const SG::AuxElement::Accessor<float> m_acc_jettrkpz;
    static const SG::AuxElement::Accessor<float> m_acc_jettrke;
    static const SG::AuxElement::Accessor<float> m_acc_jettrksumpt;

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
MissingETBase::Types::constvec_t operator*(const MissingETBase::Types::constvec_t& cvec,float scale);
/*! @brief Scale constituent vector by multiplication */
MissingETBase::Types::constvec_t operator*(float scale,const MissingETBase::Types::constvec_t& cvec);
/*! @brief Scale constituent vector by division */
MissingETBase::Types::constvec_t operator/(const MissingETBase::Types::constvec_t& cvec,float scale);
/*!@}*/
#include "xAODMissingET/versions/MissingETAssociation_v1.icc"
#endif
