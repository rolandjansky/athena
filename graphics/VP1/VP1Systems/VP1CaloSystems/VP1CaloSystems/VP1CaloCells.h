/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1CALOSYSTEMS_VP1CALOCELL_H
#define VP1CALOSYSTEMS_VP1CALOCELL_H

#include "VP1Base/VP1Interval.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <map>
#include <set>
#include <vector>
#include <string>
#include <QMap>
#include <QPair>
#include <QList>

class VP1ExtraSepLayerHelper;
class SoSeparator;
class SoNode;
class SoGenericBox;
class CaloCell;
class CaloCell_ID;
class TileID;
class TileHWID;
class TileTBID;
class TileCell;

class TileDigits;
class TileInfo;
class LArDigit;
class TileCablingService;
class ICaloBadChanTool;

class QCheckBox;
class QDoubleSpinBox;

// Structure for global cuts
class VP1CC_GlobalCuts
{
 public:
  
  bool operator == (const VP1CC_GlobalCuts& other) const {return sideA==other.sideA 
      && sideC==other.sideC
      && allowedEta==other.allowedEta
      && allowedPhi==other.allowedPhi; }

  bool sideA;
  bool sideC;
  VP1Interval allowedEta;
  QList<VP1Interval> allowedPhi;
  double clipRadius;
};

// ************* Enums *****************
// Positions of bits in the regular selection bitmaps

// Selection types
enum VP1CC_SelectionTypes
{
  VP1CC_SelTypeEMB0      = 0,
  VP1CC_SelTypeEMB1      = 1,
  VP1CC_SelTypeEMB2      = 2,
  VP1CC_SelTypeEMB3      = 3,
  VP1CC_SelTypeEMEC0     = 4,
  VP1CC_SelTypeEMEC1     = 5,
  VP1CC_SelTypeEMEC2     = 6,
  VP1CC_SelTypeEMEC3     = 7,
  VP1CC_SelTypeHEC0      = 8,
  VP1CC_SelTypeHEC1      = 9,
  VP1CC_SelTypeHEC2      = 10,
  VP1CC_SelTypeHEC3      = 11,
  VP1CC_SelTypeFCAL1     = 12,
  VP1CC_SelTypeFCAL2     = 13,
  VP1CC_SelTypeFCAL3     = 14,
  VP1CC_SelTypeTileB     = 15,
  VP1CC_SelTypeTileEC    = 16,
  VP1CC_SelTypeTileCrack = 17
};

// Separator types
enum VP1CC_SeparatorTypes
{
  VP1CC_SepLArEMBPos        = 0,
  VP1CC_SepLArEMECPos       = 1,
  VP1CC_SepLArHECPos        = 2,
  VP1CC_SepLArFCALPos       = 3,
  VP1CC_SepLArEMBNeg        = 4,
  VP1CC_SepLArEMECNeg       = 5,
  VP1CC_SepLArHECNeg        = 6,
  VP1CC_SepLArFCALNeg       = 7,
  VP1CC_SepTilePositiveUp   = 8,
  VP1CC_SepTilePositiveDown = 9,
  VP1CC_SepTilePositiveNeg  = 10,
  VP1CC_SepTileNegativeUp   = 11,
  VP1CC_SepTileNegativeDown = 12,
  VP1CC_SepTileNegativePos  = 13,
  VP1CC_SepMBTS             = 14
};

// Separator map
typedef std::map<VP1CC_SeparatorTypes, VP1ExtraSepLayerHelper*, std::less<VP1CC_SeparatorTypes> > VP1CC_SeparatorMap;

// Set of keys for the instances of above map
typedef std::set<VP1CC_SeparatorTypes> VP1CC_SeparatorTypesSet;

// Pair of intervals for selecting cells with pos/neg energies kept within Cell Managers
typedef QPair<VP1Interval,VP1Interval> VP1CCIntervalPair;

// Map of interval pairs by Selection types. Is transferred from the Controller to CellManagers when
// selection chenges in GUI. The managers should be able to find their corresponding pair by themselves
typedef QMap<VP1CC_SelectionTypes,VP1CCIntervalPair> VP1CCIntervalMap;

// Helper structure, collects all interface objects needed by VP1CaloCellManager
typedef struct
{
  const QCheckBox* globalEnableCB;        // Display/hide subsystem
  const QCheckBox* showNegativeCB;        // Display/hide negative energy cells
  const QCheckBox* upperThreshOffCB;      // Disable/enable upper energy threshold

  const QDoubleSpinBox* lowerThresholdSB; // Lower threshold
  QDoubleSpinBox* upperThresholdSB;       // Upper threshold

} VP1CCUi2Manager;

// This map is used by the Calo Cell Controller
// It keeps two objects of this type, one for simple and one for expert mode
typedef std::map<VP1CC_SelectionTypes,VP1CCUi2Manager*> VP1CCSelectionType2GuiMap;

// *********** Classes ************************

//
// Base class which has Calo subsystem specific sub-classes.
// The following diagram shows class hierarchy
//
//     VP1CaloCell
//          |------VP1CC_LAr
//          |        |----VP1CC_LArEMB
//          |        |----VP1CC_LArEMECHEC
//          |        |----VP1CC_LArFCAL
//          |
//          |------VP1CC_Tile
//          |------VP1CC_Tile
//
//
// The sub-classes differ
// in the way they create 3D objects corresponding to CaloCell
//
class VP1CaloCell;
typedef std::map<SoNode*, VP1CaloCell*, std::less<SoNode*> > VP1CC_SoNode2CCMap;

class VP1CaloCell
{
 public:
  VP1CaloCell(const CaloCell* caloCell);
  virtual ~VP1CaloCell();

  double energyToTransverse(const double&) const;

  bool cutPassed(const VP1CC_GlobalCuts& globalCuts);

  // Get Cell
  const CaloCell* getCaloCell();

  // Get Cell identifier
  Identifier getID();

  void updateScene(VP1CC_SoNode2CCMap* node2cc,
		   bool useEt,
		   const QPair<bool,double>& scale,
		   bool outline,
		   const VP1CC_GlobalCuts& globalCuts);


  // Build 3D object(s)
  virtual void build3DObjects(VP1CC_SoNode2CCMap* node2cc,
			      bool useEt,
			      const QPair<bool,double>& scale,
			      bool outline, 
            const VP1CC_GlobalCuts& ) = 0;

  double cellDepth(const QPair<bool,double>& scale, const double& energy)
  { return std::max(1.0*Gaudi::Units::mm,scale.second*(scale.first?log(1+fabs(energy)):fabs(energy))); }

  virtual bool isInsideClipVolume(const VP1CC_GlobalCuts&  globalCuts) ; // by default uses radius to determine this
  
  // Remove 3D object(s) from scene
  virtual void remove3DObjects(VP1CC_SoNode2CCMap* node2cc) = 0;
  
  // Create a string with cell specific information
  // For Tile Barrel & EC cells print both PMT specific info no matter which hit has been selected
  // Input parameter:
  //  calo_id:           ID helper, needed to decode Cell ID
  virtual std::vector<std::string> ToString(const CaloCell_ID*  calo_id, const std::string& extrainfos="") = 0;

 protected:
  const CaloCell* m_caloCell;          // Pointer to Calo Cell

};

//
//  *** *** *** LAr *** *** ***
//
class VP1CC_LAr : public VP1CaloCell
{
 public:
  VP1CC_LAr(const CaloCell* caloCell);
  virtual ~VP1CC_LAr();

  virtual void remove3DObjects(VP1CC_SoNode2CCMap* node2cc);

 protected:
  SoGenericBox* m_hit;
  VP1ExtraSepLayerHelper* m_helper;
};


//
// *************** LArEMB ************************
//

class VP1CC_LArEMB : public VP1CC_LAr
{
 public:
  VP1CC_LArEMB(const CaloCell* caloCell,
		   const VP1CC_SeparatorMap* separators);
  virtual ~VP1CC_LArEMB();

  virtual void build3DObjects(VP1CC_SoNode2CCMap* node2cc,
			      bool useEt,
			      const QPair<bool,double>& scale,
			      bool outline, 
            const VP1CC_GlobalCuts& );

  std::vector<std::string> ToString(const CaloCell_ID*  calo_id, const std::string& extrainfos="");
};

//
// *************** LArEMECHEC ************************
//

class VP1CC_LArEMECHEC : public VP1CC_LAr
{
 public:
  VP1CC_LArEMECHEC(const CaloCell* caloCell,
		       const CaloCell_ID*  calo_id,
		       const VP1CC_SeparatorMap* separators);

  virtual ~VP1CC_LArEMECHEC();

  virtual void build3DObjects(VP1CC_SoNode2CCMap* node2cc,
			      bool useEt,
			      const QPair<bool,double>& scale,
			      bool outline, 
            const VP1CC_GlobalCuts& );

  std::vector<std::string> ToString(const CaloCell_ID*  calo_id, const std::string& extrainfos="");

};

//
// *************** LArFCAL ************************
//

class VP1CC_LArFCAL : public VP1CC_LAr
{
 public:
  VP1CC_LArFCAL(const CaloCell* caloCell,
		    const VP1CC_SeparatorMap* separators);

  virtual ~VP1CC_LArFCAL();

  virtual void build3DObjects(VP1CC_SoNode2CCMap* node2cc,
			      bool useEt,
			      const QPair<bool,double>& scale,
			      bool outline, 
            const VP1CC_GlobalCuts& );

  std::vector<std::string> ToString(const CaloCell_ID*  calo_id, const std::string& extrainfos="");

};

//
// *********** Tile Base Class ****************
//
class VP1CC_Tile : public VP1CaloCell
{
 public:
  VP1CC_Tile(const CaloCell* caloCell,
		 const TileID* tile_id);
  virtual ~VP1CC_Tile();

  // return == 0 for OK
  virtual int GetFragChannel(const TileHWID* tile_hw_id,
			     bool up,  // false - onl1, true - onl2
			     int& frag,
			     int& channel) = 0;

 protected:
  const TileID* m_tileID;
};

//
//  *** *** *** Tile Barrel & EC *** *** ***
//
class VP1CC_TileBarEc : public VP1CC_Tile
{
 public:
  VP1CC_TileBarEc(const CaloCell* caloCell,
		      const TileID* tile_id,
		      const VP1CC_SeparatorMap* separators);
  virtual ~VP1CC_TileBarEc();

  virtual void build3DObjects(VP1CC_SoNode2CCMap* node2cc,
			      bool useEt,
			      const QPair<bool,double>& scale,
			      bool outline, 
            const VP1CC_GlobalCuts& );
  
  bool isInsideClipVolume(const VP1CC_GlobalCuts& globalCuts);

  virtual void remove3DObjects(VP1CC_SoNode2CCMap* node2cc);

  std::vector<std::string> ToString(const CaloCell_ID*  calo_id, const std::string& extrainfos="");

  int GetFragChannel(const TileHWID* tile_hw_id,
		     bool up,
		     int& frag,
		     int& channel);

 protected:
  SoGenericBox* m_hitUp;
  SoGenericBox* m_hitDown;
  VP1ExtraSepLayerHelper* m_helperUp;
  VP1ExtraSepLayerHelper* m_helperDown;

  std::string id2name(Identifier& id);
};

//
//  *** *** *** Tile Crack *** *** ***
//
class VP1CC_TileCrack : public VP1CC_Tile
{
 public:
  VP1CC_TileCrack(const CaloCell* caloCell,
		      const TileID* tile_id,
		      const VP1CC_SeparatorMap* separators);

  virtual ~VP1CC_TileCrack();

  virtual void build3DObjects(VP1CC_SoNode2CCMap* node2cc,
			      bool useEt,
			      const QPair<bool,double>& scale,
			      bool outline, 
            const VP1CC_GlobalCuts& );

  bool isInsideClipVolume(const VP1CC_GlobalCuts& globalCuts);
  
  virtual void remove3DObjects(VP1CC_SoNode2CCMap* node2cc);

  std::vector<std::string> ToString(const CaloCell_ID*  calo_id, const std::string& extrainfos="");

  int GetFragChannel(const TileHWID* tile_hw_id,
		     bool up,
		     int& frag,
		     int& channel);
 protected:
  SoGenericBox* m_hit;
  VP1ExtraSepLayerHelper* m_helper;
};

// ---------------------------------------------------
//
//             *** *** MBTS *** ***
//
// ---------------------------------------------------

// Data structures needed to keep following information:
//
//   1. MBTS scintillator shape parameters (2 different shapes)
//   2. MBTS scintillator global positions

typedef std::map<int, GeoTrf::Transform3D, std::less<int> > VP1CC_MbtsXfMap;

typedef struct
{
  double dx1;  //-|
  double dx2;  // |
  double dy1;  // |-Scintillator shape parameters
  double dy2;  // |
  double dz;   //-|
  VP1CC_MbtsXfMap aTransforms; // Absolute positions on the A side
  VP1CC_MbtsXfMap cTransforms; // Absolute positions on the C side
} VP1CC_MbtsScinInfo;

// The instance of this map contains 2 elements
// 0 - scintillators at lower R
// 1 - scintillators at higher R
typedef std::map<int, VP1CC_MbtsScinInfo*, std::less<int> > VP1CC_MbtsScinInfoMap;

class VP1Mbts;
typedef std::map<SoNode*, VP1Mbts*, std::less<SoNode*> > VP1CC_SoNode2MbtsMap;

class VP1Mbts
{
 public:
  VP1Mbts(const TileCell*            cell,
	  const TileTBID*            idhelper,
         SoSeparator*               separator,
         bool                       run2Geo);
  ~VP1Mbts();

  // Update graphics scene according to the trivial energy threshold criterion
  // Return true if the new object has been created
  bool UpdateScene(VP1CC_MbtsScinInfoMap* drawinfo,
		   VP1CC_SoNode2MbtsMap* node2mbts,
		   double energy,
		   bool outline, 
       double clipRadius);

  // Print out cell information to the vector of strings
  std::vector<std::string> ToString();


 private:
  const TileCell* m_cell;        // Pointer to the Cell object (TileCell)
  const TileTBID* m_idhelper;    // Tile TB id helper
  SoSeparator*    m_separator;   // Separator helper
  bool            m_run2Geo;     // RUN2 geometry: the readout granularity of MBTS2 changes from 8 to 4
};

#endif
