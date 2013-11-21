// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEEVENT_EVENTFEATURE_H
#define EVENTSHAPEEVENT_EVENTFEATURE_H

//#include "EventShapeEvent/EventShapes.h"

#include <string>
#include <vector>
#include <algorithm>
#include <map>

class EventFeature
{
public:

  /*! @*/
  /*! @brief Type enumerators */
  enum FeatureType 
    { ETDENSITY             = 0x1000, 
      ETDENSITY_JETAREA     = 0x1100,  /*!< @c FastJet density */
      ETDENSITY_JETAREA_ETA = 0x1101,  /*!< @c FastJet density, @f$ \eta @f$ 
				       dependent */
      ETDENSITY_ETA_WINDOWS = 0x1201, /*!< density in sliding @f$ \eta @f$ 
				       windows */ 
      ETDENSITY_USER    = 0x0010,     /*!< User defined density */
      UNKNOWN           = 0x0000,     /*!< Unknown type */
      DEFAULT           = 0xffff      /*!< Default type */
  };

  /*! @brief Feature structure descriptors
   * 
   *  Mostly useful for T/P converter yet public for converter access
   */
  enum FeatureStructureMask { NDATAWORDS  = 0x0000007f, /*<! Mask for number of
							  data words/block (max 
							  accomodation 128)*/
			      NDATABLOCKS = 0x00003f80, /*<! Mask for number of
							  data blocks (max 
							  accommodation 128)*/
			      TAGBITS     = 0x0000c000, /*<! Mask for additional
							  tag bits (2 bits)*/
			      MASKLENGTH  = 0x0000ffff, /*<! Mask for total
							  used bit range (16)*/
			      BITSHIFT    = 0x00000007 }; /*<! Shift step */    

  /*! @brief Default constructor 
   * 
   *  Instantiates empty data object, with defaults:
   *
   *  <code> type     = EventFeature::UNKNOWN; </code>
   *  <code> tag      = "Default"; </code>
   *  <code> dataList = std::vector<double>(); </code>
   *
   *  This object also has no internal data structure (0 data words in 0 
   *  blocks).
   */
  EventFeature();
  /*! @brief Loaded constructor 
   *
   *  @param[in] type     @c EventFeature type
   *  @param[in] tag      @c string identifier/tag for data
   *  @param[in] dataList reference to non-modifiable data vector to be copied
   *                      to internal store
   *   
   *  @note Flat data structure with @c dataList.size() words in one block.
   *  
   */
  EventFeature(FeatureType                type,
	       const std::string&         tag,
	       const std::vector<double>& dataList);
  /*! @brief Loaded constructor 
   *
   *  @param[in] type     @c EventFeature type
   *  @param[in] tag      @c string identifier/tag for data
   *  @param[in] data     scalar (number) data word to be stored internally
   *  @param[in] dataList reference to non-modifiable data vector to be copied
   *                      to internal store
   */
  EventFeature(FeatureType                type,
 	       const std::string&         tag,
	       unsigned int               dataStructure,
	       const std::vector<double>& dataList);
  /*! @brief Loaded constructor 
   *
   *  @param[in] type     @c EventFeature type
   *  @param[in] tag      @c string identifier/tag for data
   *  @param[in] data     scalar (number) data word to be stored internally
   *   
   *  @note Internal multiple (vector) store is set to its default value 
   *  (@c std::vector<double>() ). 
   *  
   */
  EventFeature(FeatureType                type,
	       const std::string&         tag,
	       unsigned int               dataStructure);
  /*! @brief Copy constructor */
  EventFeature(const EventFeature& feature);

  /*! @brief Destructor */
  virtual ~EventFeature();

  /*! @brief Assignment operator
   *
   *  Copies data content from @c EventFeature source.
   * 
   *  @param[in] feature reference to non-modifiable @c EventFeature object
   * 
   */
  EventFeature& operator=(const EventFeature& feature);

  /*! @brief Retrieve feature tag
   * 
   *  @return @c const reference to non-modifiable feature tag (string). 
   */
  const std::string& tag() const;
  /*! @brief Retrieve feature type
   *
   *  @return Copy of feature type.
   */
  FeatureType type() const;
  /*! @brief Retrieve data structure descriptor
   * 
   *  @return Copy of data structure descriptor
   */ 
  unsigned int featureStructure() const;
  /*! @brief Retrieve number of data blocks
   *
   *  @return Number of data blocks
   */
  size_t numberDataBlocks() const;
  /*! @brief Retrieve number of words in data block
   *
   *  @return Number of words in data block
   */
  size_t numberDataWords()  const;
  /*! @brief Retrieve reference to vector store
   *
   *  @return @c const reference to non-modifiable vector data store. 
   */
  const std::vector<double>& dataList() const;
  /*! @brief Retrieve reference to vector store
   *
   *  @return @c const reference to modifiable vector data store. 
   */
  std::vector<double>& dataList();

  /*! @brief Return one datablock */
  bool dataBlock(size_t blockIdx,std::vector<double>& data) const;

  /*! @brief Set feature tag
   *
   *  @param[in] tag reference to non-modifiable tag string
   */
  void setTag(const std::string& tag);
  /*! @brief Set feature type
   * 
   *  @param[in] type feature type
   */
  void setType(FeatureType type);
  /*! @brief Set number of blocks
   *
   *  @param[in] nBlocks number of data blocks
   */
  void setNumberDataBlocks(size_t nBlocks);
  /*! @brief Set number of data words per block
   * 
   *  @param[in] nWords number of data words per block
   */
  void setNumberDataWords(size_t nWords);

  /*! @brief Set vector data store
   *
   *  @param[in] data reference to non-modifiable data vector 
   */
  void setDataList(const std::vector<double>& data);
  /*! @brief Set one data block */
  bool setDataBlock(size_t blockIdx,const std::vector<double>& data,
		    bool replace=true);
  bool addDataBlock(const std::vector<double>& data);

  // /*! @brief Binned scalar data model 
  //  *
  //  *  @param[in] lmin minimum 
  //  */
  // void setBinnedData(double lmin,double lmax,double data);

  /*! @brief Convert feature tag to @c FeatureType */
  static FeatureType featureType(const std::string& typeTag);
  /*! @brief Default feature type */
  static FeatureType featureType();
  /*! @brief Convert a @c FeatureType to a tag */
  static const std::string& featureTag(FeatureType type);
  /*! @brief Default feature tag */
  static const std::string& featureTag();

  /*! @brief Build compressed feature structure */
  unsigned int buildFeatureStructure() const;

private:

  typedef std::pair<size_t,size_t> index_t;

  // enum FeatureStructure 
  //   { SCALAR               = 0x1000,
  //     SCALAR_BINNED_SPARSE = 0x1001,
  //     SCALAR_BINNED_CONT   = 0x1002,
  //     VECTOR               = 0x0100,
  //     VECTOR_BINNED_SPARSE = 0x0101,
  //     VECTOR_BINNED_CONT   = 0x0102
  //   };
  //
  // b00000000000000000000000001111111  = 0x007f (0...127) N_datawords
  // b00000000000000000011111110000000  = 0x03f8 (0...127) N_datablocks
  // b0000000000000000________________  = 16 bits
  //

  /*! @brief Internal cache for feature type */
  FeatureType          m_type;
  /*! @brief Internal cache for feature tag */
  std::string          m_tag;
  /*! @brief Internal cache data structure description (words per block)*/
  unsigned int         m_dataWords;
  /*! @brief Internal cache for data structure descriptor (number of blocks) */
  mutable unsigned int m_dataBlocks;
  /*! @brief Internal cache for compressed data structure descriptor */
  mutable unsigned int m_featureStructure;
  /*! @brief Internal cache for vector data */
  mutable std::vector<double>  m_dataList;

  /*! @brief Control flag for setting number of data words
   * 
   *  Can only be done once!
   */
  bool m_structureFixed;

  /*! @brief Lookup for feature type and tag matches */
  static std::map<FeatureType,std::string> m_featureTags;
  /*! @brief Fill lookup once */  
  static const std::map<FeatureType,std::string>& fillFeatureTags();

  /*! @brief Set/update feature structure */
  void setFeatureStructure(bool updateCache=false) const;

  /*! */
  bool insertDataBlock(size_t startIdx,size_t endIdx,
		       const std::vector<double>& data); 
  bool addDataBlock(size_t startIdx,size_t endIdx,const std::vector<double>& 
		    data);

  /*! */
  bool indexRange(size_t startIdx,index_t& idx) const;
};

/*! @class EventFeature
 *
 *  @brief Basic container for event feature data.
 *
 *  Basic container for event feature data or parameters. Each entry is 
 *  characterized by the type of information, a tag, and possibly scalar
 *  (number) and/or vector (list of numbers or list of number patterns).
 *  The possible interpretation of the number patterns depends on the 
 *  feature type and is usually implemented in the tool producing the
 *  @c EventFeature object. 
 */
// Three simple structural patterns are provided by 
//  *  the data object, for convenience: 
//  *  
//  *  - type <tt>ETDENSITY_JETAREA</tt> implied structure type <tt>SCALAR</tt>
//  *    - stores one number/event (global event transverse energy density
//  *    - uses @c m_data cache, @c m_dataList is empty
//  *  - type <tt>ETDENSITY_JETAREA_ETA</tt> implied structure 
//  *    <tt>SCALAR_BINNED</tt> 
//  *  - type <tt>ETDENSITY_ETA_WINDOWS</tt> implied structure 
//  *    <tt>FUNCTION_PARAMETERS</tt>
//  */

inline const std::string& EventFeature::tag() const
{ return m_tag; }

inline EventFeature::FeatureType EventFeature::type() const
{ return m_type; }

inline unsigned int EventFeature::featureStructure() const 
{ return m_featureStructure; }

inline const std::vector<double>& EventFeature::dataList() const
{ return m_dataList; }

inline std::vector<double>& EventFeature::dataList()
{ return m_dataList; }

inline void EventFeature::setType(FeatureType type)
{ m_type = type; }

inline void EventFeature::setTag(const std::string& tag) 
{ m_tag = tag; }

inline void EventFeature::setDataList(const std::vector<double>& data)
{ 
  m_dataList.resize(data.size()); 
  std::copy(data.begin(),data.end(),m_dataList.begin());
  this->setFeatureStructure();
}

inline size_t EventFeature::numberDataWords() const
{ return m_dataWords; }

inline size_t EventFeature::numberDataBlocks() const
{ this->setFeatureStructure(); return m_dataBlocks; }
#endif
