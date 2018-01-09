/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1CaloSystems/VP1CaloCellSystem.h"
#include "VP1CaloSystems/VP1CaloCellManager.h"
#include "VP1CaloSystems/VP1CaloCellController.h"
#include "VP1CaloSystems/VP1CaloCells.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1HEPVis/nodes/SoGenericBox.h"

#include <QBuffer>
#include <QByteArray>
#include <QString>
#include <QMessageBox>

#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloEvent/CaloCellContainer.h"

#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileConditions/Exception.h"


#include "StoreGate/StoreGateSvc.h"

#include <map>
#include <vector>
#include <sstream>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IToolSvc.h"
#include "LArCabling/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCablingService.h"
#include "CaloConditions/ICaloBadChanTool.h"

// MBTS
#include "VP1CaloSystems/VP1MbtsHelper.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Base/VP1CustomTourEditor.h"
#include <stdexcept>


// -------------------- Clockwork -------------------------------
class VP1CaloCellSystem::Clockwork
{
public:
	Clockwork();
	~Clockwork();

	// -------------- Methods ------------------
	// Called at the beginning of every event
	// Creates new manager objects and stores them into
	void BuildCellManagers();

	// Called at the beginning of every event after BuildCellManagers().
	// Iterates over Calo Cells in the SG container, builds VP1CC objects
	// and stores the pointers into appropriate Manager(s)
	void FillManagers();

	// Special situation with Tile Raw Channel keys in SG. They have to be searched in a special order
	// The order is hardwired unfortunately and may vary in time...
	std::string SearchTileRawchanKeys(const std::vector<std::string>& inputKeys);

	// -------------- Data members ----------------
	VP1CaloCellController* controller;

	//--------------------------------------------------------------
	//
	//  Section: Flags
	//

	bool noCalo;       // If Calo has been switched off don't show anything
	bool noLArDigitsGlobal;  // Are we able to show LAr digits at all?
	bool noLArDigitsEvent;  // Are we able to show LAr digits for this event?
	bool noTileDigitsGlobal; // Are we able to show Tile digits at all?
	bool noTileDigitsEvent; // Are we able to show Tile digits for this event?

	bool modeSimple;   // Which mode (simple/expert) is used for LAr

	//--------------------------------------------------------------
	//
	//  Section: DetStore objects: managers, ID helpers
	//

	// Managers
	const CaloDetDescrManager* calo_dd_man;
	const TileDetDescrManager* tile_dd_man;

	// ID helpers
	const CaloCell_ID*         calo_id;
	const TileID*              tile_id;
	const TileHWID*            tile_hw_id;

	//--------------------------------------------------------------
	//
	//  Section: Tools, Services
	//

	const TileInfo*            tile_info;
	const TileCablingService*  tile_cabling;
	LArCablingService*         lar_cabling;
	const LArOnlineID*         lar_onlineID;
	const ICaloBadChanTool*    calo_badchannel;

	//--------------------------------------------------------------
	//
	//  Section: StoreGate containers
	//

	const CaloCellContainer*       calocells;
	const LArDigitContainer*       lar_digits;
	const TileDigitsContainer*     tile_digits;
	const TileRawChannelContainer* tile_rawchannel;

	//--------------------------------------------------------------
	//
	//  Section: Maps of VP1CaloCellManager-s
	//

	VP1CCManagerContainer  cell_managers;

	//--------------------------------------------------------------
	//
	//  Section: MBTS
	//

	VP1MbtsHelper*      mbtsHelper;

	//--------------------------------------------------------------
	//
	//  Section: Maps
	//

	// Map of VP1CaloCell objects by their 3D objects
	VP1CC_SoNode2CCMap node2ccMap;

	// Separator helper map
	VP1CC_SeparatorMap sepHelperMap;

	// Set of keys for the above map
	VP1CC_SeparatorTypesSet sepTypes;
};

VP1CaloCellSystem::Clockwork::Clockwork()
:controller(0),
 noCalo(false),
 noLArDigitsGlobal(false),
 noLArDigitsEvent(false),
 noTileDigitsGlobal(false),
 noTileDigitsEvent(false),
 modeSimple(true),    // simple mode by default, expert on demand
 calo_dd_man(0),
 tile_dd_man(0),
 calo_id(0),
 tile_id(0),
 tile_hw_id(0),
 tile_info(0),
 tile_cabling(0),
 lar_cabling(0),
 lar_onlineID(0),
 calo_badchannel(0),
 calocells(0),
 lar_digits(0),
 tile_digits(0),
 tile_rawchannel(0),
 mbtsHelper(new VP1MbtsHelper(false))
{
	// Initialize set of available separator types besides MBTS
	sepTypes.insert(VP1CC_SepLArEMBPos);
	sepTypes.insert(VP1CC_SepLArEMECPos);
	sepTypes.insert(VP1CC_SepLArHECPos);
	sepTypes.insert(VP1CC_SepLArFCALPos);
	sepTypes.insert(VP1CC_SepLArEMBNeg);
	sepTypes.insert(VP1CC_SepLArEMECNeg);
	sepTypes.insert(VP1CC_SepLArHECNeg);
	sepTypes.insert(VP1CC_SepLArFCALNeg);
	sepTypes.insert(VP1CC_SepTilePositiveUp);
	sepTypes.insert(VP1CC_SepTilePositiveDown);
	sepTypes.insert(VP1CC_SepTilePositiveNeg);
	sepTypes.insert(VP1CC_SepTileNegativeUp);
	sepTypes.insert(VP1CC_SepTileNegativeDown);
	sepTypes.insert(VP1CC_SepTileNegativePos);
}

VP1CaloCellSystem::Clockwork::~Clockwork()
{
	if(mbtsHelper) delete mbtsHelper;
}

void VP1CaloCellSystem::Clockwork::BuildCellManagers()
{
	// Populate cell_managers map
	cell_managers[VP1CC_SelTypeEMB0] = new VP1CaloCellManager(VP1CC_SelTypeEMB0,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeEMB1] = new VP1CaloCellManager(VP1CC_SelTypeEMB1,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeEMB2] = new VP1CaloCellManager(VP1CC_SelTypeEMB2,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeEMB3] = new VP1CaloCellManager(VP1CC_SelTypeEMB3,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeEMEC0] = new VP1CaloCellManager(VP1CC_SelTypeEMEC0,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeEMEC1] = new VP1CaloCellManager(VP1CC_SelTypeEMEC1,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeEMEC2] = new VP1CaloCellManager(VP1CC_SelTypeEMEC2,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeEMEC3] = new VP1CaloCellManager(VP1CC_SelTypeEMEC3,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeHEC0] = new VP1CaloCellManager(VP1CC_SelTypeHEC0,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeHEC1] = new VP1CaloCellManager(VP1CC_SelTypeHEC1,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeHEC2] = new VP1CaloCellManager(VP1CC_SelTypeHEC2,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeHEC3] = new VP1CaloCellManager(VP1CC_SelTypeHEC3,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeFCAL1] = new VP1CaloCellManager(VP1CC_SelTypeFCAL1,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeFCAL2] = new VP1CaloCellManager(VP1CC_SelTypeFCAL2,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeFCAL3] = new VP1CaloCellManager(VP1CC_SelTypeFCAL3,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeTileB] = new VP1CaloCellManager(VP1CC_SelTypeTileB,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeTileEC] = new VP1CaloCellManager(VP1CC_SelTypeTileEC,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());
	cell_managers[VP1CC_SelTypeTileCrack] = new VP1CaloCellManager(VP1CC_SelTypeTileCrack,&node2ccMap,
			controller->energyModeEt(),controller->scale(),
			controller->showVolumeOutLines(),controller->globalCuts());

	// Connect controller signals to manager slots
	for(VP1CCManagerContainer::iterator it=cell_managers.begin(); it!=cell_managers.end(); it++) {
		connect(controller,SIGNAL(selectionIntervalsChanged(VP1CCIntervalMap)),
				it->second,SLOT(selectionUpdated(VP1CCIntervalMap)));
		connect(controller,SIGNAL(scaleChanged(QPair<bool,double>)),
				it->second,SLOT(scaleUpdated(QPair<bool,double>)));
		connect(controller,SIGNAL(showVolumeOutLinesChanged(bool)),
				it->second,SLOT(outlineUpdated(bool)));
		connect(controller,SIGNAL(globalCutsChanged(VP1CC_GlobalCuts)),
				it->second,SLOT(globalCutsUpdated(VP1CC_GlobalCuts)));
		connect(controller->customTourEditor(),SIGNAL(clipVolumeRadiusChanged(double)),
				it->second,SLOT(clipVolumeRadiusChanged(double)));
	}
}

void VP1CaloCellSystem::Clockwork::FillManagers()
{
	// For events with no Calo Cells do nothing
	if(!calocells) return;

	// loop over all calo cells in the calo cell container
	CaloCellContainer::const_iterator i_cell=calocells->begin();

	for(; i_cell!=calocells->end(); ++i_cell) {
		const CaloCell* _cell = *i_cell;

		// for each cell:
		//   1. determine its type
		//   2. build VP1CaloCell of appropriate type
		//   3. add it to the appropriate manager
		try {
			if(calo_id->is_em_barrel(_cell->ID())) {
				// ------- EMB ----------
				VP1CC_LArEMB* _embCC = new VP1CC_LArEMB(_cell,&sepHelperMap);

				switch(calo_id->sampling(_cell->ID())) {
				case 0: {
					cell_managers[VP1CC_SelTypeEMB0]->add(_embCC);
					break;
				}
				case 1: {
					cell_managers[VP1CC_SelTypeEMB1]->add(_embCC);
					break;
				}
				case 2: {
					cell_managers[VP1CC_SelTypeEMB2]->add(_embCC);
					break;
				}
				case 3: {
					cell_managers[VP1CC_SelTypeEMB3]->add(_embCC);
					break;
				}
				default: { //unknown case, tidy up
				  delete _embCC; _embCC=nullptr;
				  break;
				}
				}
				// ------- EMB ----------
			}
			else if(calo_id->is_em_endcap(_cell->ID()) || calo_id->is_hec(_cell->ID())) {
				VP1CC_LArEMECHEC* _emechecCC = new VP1CC_LArEMECHEC(_cell,calo_id,&sepHelperMap);

				if(calo_id->is_em_endcap(_cell->ID())) {
					// ------- EMEC ----------
					switch(calo_id->sampling(_cell->ID())) {
					case 0: {
						cell_managers[VP1CC_SelTypeEMEC0]->add(_emechecCC);
						break;
					}
					case 1: {
						cell_managers[VP1CC_SelTypeEMEC1]->add(_emechecCC);
						break;
					}
					case 2: {
						cell_managers[VP1CC_SelTypeEMEC2]->add(_emechecCC);
						break;
					}
					case 3: {
						cell_managers[VP1CC_SelTypeEMEC3]->add(_emechecCC);
						break;
					}
					default: { //unknown case, tidy up
					  delete _emechecCC; _emechecCC=nullptr;
					  break;
					}
					}
					// ------- EMEC ----------
				} else {
					// ------- HEC ----------
					switch(calo_id->sampling(_cell->ID())) {
					case 0: {
						cell_managers[VP1CC_SelTypeHEC0]->add(_emechecCC);
						break;
					}
					case 1: {
						cell_managers[VP1CC_SelTypeHEC1]->add(_emechecCC);
						break;
					}
					case 2: {
						cell_managers[VP1CC_SelTypeHEC2]->add(_emechecCC);
						break;
					}
					case 3: {
						cell_managers[VP1CC_SelTypeHEC3]->add(_emechecCC);
						break;
					}
					default: { //unknown case, tidy up
					  delete _emechecCC; _emechecCC=nullptr;
					  break;
					}
					}
					// ------- EMEC ----------
				}
			}
			else if(calo_id->is_fcal(_cell->ID())) {
				// ------- FCAL ----------
				VP1CC_LArFCAL* _fcalCC = new VP1CC_LArFCAL(_cell,&sepHelperMap);

				switch(calo_id->sampling(_cell->ID())) {
				case 1: {
					cell_managers[VP1CC_SelTypeFCAL1]->add(_fcalCC);
					break;
				}
				case 2: {
					cell_managers[VP1CC_SelTypeFCAL2]->add(_fcalCC);
					break;
				}
				case 3: {
					cell_managers[VP1CC_SelTypeFCAL3]->add(_fcalCC);
					break;
				}
				default: { //unknown case, tidy up
					  delete _fcalCC; _fcalCC=nullptr;
					  break;
					}
				}
				// ------- FCAL ----------
			}
			else if(!calo_id->is_tile_gapscin(_cell->ID())) {
				// ------- Tile Barrel EC ----------
				VP1CC_TileBarEc* _tileCC = new VP1CC_TileBarEc(_cell,tile_id,&sepHelperMap);

				if(calo_id->is_tile_barrel(_cell->ID()))
					cell_managers[VP1CC_SelTypeTileB]->add(_tileCC);
				else
					cell_managers[VP1CC_SelTypeTileEC]->add(_tileCC);
			}
			else if(calo_id->is_tile_gapscin(_cell->ID())) {
				// ------- Tile Crack ----------
				VP1CC_TileCrack* _tilecrackCC = new VP1CC_TileCrack(_cell,tile_id,&sepHelperMap);

				cell_managers[VP1CC_SelTypeTileCrack]->add(_tilecrackCC);
			}
		}
		catch(std::runtime_error& err) {
			// --- do nothing for now
			std::cout << "RunTime exception caught! --> " << err.what() << std::endl;
		}
	} // iterate over cells
}

std::string VP1CaloCellSystem::Clockwork::SearchTileRawchanKeys(const std::vector<std::string>& inputKeys)
{
	// Search order
	std::vector<std::string> searchOrder;
	searchOrder.push_back("TileRawChannelOpt2");
	searchOrder.push_back("TileRawChannelOpt");
	searchOrder.push_back("TileRawChannelFixed");
	searchOrder.push_back("TileRawChannelFitCool");
	searchOrder.push_back("TileRawChannelFit");
	searchOrder.push_back("TileRawChannelCnt");
	searchOrder.push_back("TileRawChannelFlt");

	for(size_t i=0; i<searchOrder.size(); i++) {
		std::string searchString = searchOrder[i];
		for(size_t ii=0; ii<inputKeys.size(); ii++)
			if(inputKeys[ii]==searchString)
				return searchString;
	}
	// No keys found from Search Order, return empty string
	return std::string("");
}

// -------------------- Clockwork -------------------------------



// ---------------------- System ----------------------------

VP1CaloCellSystem::VP1CaloCellSystem()
:IVP13DSystemSimple("Cells","Display energy of CaloCell objects","tsulaia@mail.cern.ch or rbianchi@cern.ch"),
 _clockwork(new Clockwork())
{
}

VP1CaloCellSystem::~VP1CaloCellSystem()
{
	delete _clockwork;
}

QWidget* VP1CaloCellSystem::buildController()
{
	// Standard stuff
	_clockwork->controller = new VP1CaloCellController(this);

	// Connect controller signals to various slots
	// Energy mode
	connect(_clockwork->controller,SIGNAL(energyModeEtChanged(bool)),
			this,SLOT(energyMode()));

	// Mbts
  _clockwork->mbtsHelper->setController(_clockwork->controller);
	connect(_clockwork->controller,SIGNAL(selectionMbtsChanged(VP1Interval)),
			_clockwork->mbtsHelper,SLOT(selectionUpdated(VP1Interval)));
	connect(_clockwork->controller,SIGNAL(showVolumeOutLinesChanged(bool)),
			_clockwork->mbtsHelper,SLOT(outlineUpdate(bool)));
  if (_clockwork->controller->customTourEditor()){
    std::cout <<" Connecting to CVRC" <<std::endl;
    connect(_clockwork->controller->customTourEditor(),SIGNAL(clipVolumeRadiusChanged(double)),
			_clockwork->mbtsHelper,SLOT(clipVolumeRadiusChanged(double)));
  }
	return _clockwork->controller;
}

void VP1CaloCellSystem::systemcreate(StoreGateSvc* detstore)
{
	// Make sure we can use required custom 3D objects:
	SoGenericBox::initClass();

	// ------------- DD Managers and ID Helpers -------------
	StatusCode status = detstore->retrieve(_clockwork->calo_dd_man);
	if(status.isFailure() || _clockwork->calo_dd_man==0) {
		_clockwork->noCalo = true;
		messageDebug("Unable to retrieve Calo DD Manager");
		return;
	}

	status = detstore->retrieve(_clockwork->tile_dd_man);
	if(status.isFailure() || _clockwork->tile_dd_man==0) {
		_clockwork->noCalo = true;
		messageDebug("Unable to retrieve Tile DD Manager");
		return;
	}

	_clockwork->calo_id = _clockwork->calo_dd_man->getCaloCell_ID();
	if(_clockwork->calo_id==0) {
		_clockwork->noCalo = true;
		messageDebug("0 pointer to Calo ID Helper");
		return;
	}

	_clockwork->tile_id = _clockwork->tile_dd_man->get_id();
	if(_clockwork->tile_id ==0) {
		_clockwork->noCalo = true;
		messageDebug("0 pointer to Tile ID Helper");
		return;
	}

	status = detstore->retrieve(_clockwork->tile_hw_id);
	if(status.isFailure() || _clockwork->tile_hw_id==0) {
		messageDebug("Unable to retireve Tile HWID");
		_clockwork->tile_hw_id = 0;
		_clockwork->noTileDigitsGlobal = true;
	}
	// ------------- DD Managers and ID Helpers -------------


	// ------------- Tools/Services -------------
	status = detstore->retrieve(_clockwork->tile_info,"TileInfo");
	if(status.isFailure() || _clockwork->tile_info==0) {
		messageDebug("Unable to retrieve Tile Info");
		_clockwork->tile_info = 0;
		_clockwork->noTileDigitsGlobal = true;
	}

	_clockwork->tile_cabling = TileCablingService::getInstance();
	if(_clockwork->tile_cabling==0) {
		messageDebug("0 pointer to TileCabling");
		_clockwork->noTileDigitsGlobal = true;
	}

	status = toolSvc()->retrieveTool("LArCablingService",_clockwork->lar_cabling);
	if (status.isFailure() || _clockwork->lar_cabling == 0) {
		messageDebug("Failed to locate LAr Cabling Service");
		_clockwork->lar_cabling = 0;
		_clockwork->noLArDigitsGlobal = true;
	}

	status = detstore->retrieve(_clockwork->lar_onlineID,"LArOnlineID");
	if (status.isFailure() || _clockwork->lar_onlineID == 0) {
		messageDebug("Failed to retrieve LAr online ID");
		_clockwork->lar_onlineID = 0;
	}


	status = toolSvc()->retrieveTool("CaloBadChanTool",_clockwork->calo_badchannel);
	if (status.isFailure() || _clockwork->calo_badchannel == 0) {
		messageDebug("Failed to locate Calo Bad Channel Tool");
		_clockwork->calo_badchannel = 0;
	}

	// Pass retrieved pointers to the controller (for Tile Pulse display)
	_clockwork->controller->initTilePulse(_clockwork->tile_hw_id,
			_clockwork->tile_info,
			_clockwork->tile_cabling);
	// ------------- Tools/Services -------------


	// -------------- MBTS ----------------
	try {
		_clockwork->mbtsHelper->systemcreate(detstore);
	} catch(std::runtime_error& err) {
		message (err.what());
	}
	// -------------- MBTS ----------------
}

void VP1CaloCellSystem::systemerase()
{
	// Clear node 2 CC map
	_clockwork->node2ccMap.clear();

	// Delete Cell Managers and clear the map
	VP1CCManagerContainer::iterator it = _clockwork->cell_managers.begin();
	for(; it!=_clockwork->cell_managers.end(); it++)
		if(it->second) delete it->second;

	_clockwork->cell_managers.clear();
	_clockwork->calocells = 0;

	// delete separator helpers and clear the separator helper map
	VP1CC_SeparatorMap::iterator itSepHelper = _clockwork->sepHelperMap.begin();
	for(; itSepHelper!= _clockwork->sepHelperMap.end(); itSepHelper++)
		if(itSepHelper->second) delete itSepHelper->second;
	_clockwork->sepHelperMap.clear();

	// MBTS
	_clockwork->mbtsHelper->systemerase();
}

void VP1CaloCellSystem::systemuncreate()
{
	messageDebug("VP1CaloCellSystem::systemuncreate()...");
	_clockwork->controller->DeleteDigitForms();
}

void VP1CaloCellSystem::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)
{
	if(_clockwork->noCalo) return;

	root->addChild(_clockwork->controller->drawOptions());

	// Iterators for SG keys
	std::vector<std::string>::const_iterator firstKey, lastKey;

	// ---------------- Retrieve calo cells from SG ---------------------
	// Check for SG keys
	std::vector<std::string> strCaloCellKeys = sg->keys<CaloCellContainer>();
	firstKey = strCaloCellKeys.begin();
	lastKey = strCaloCellKeys.end();


	if(firstKey!=lastKey) {
		// Some keys found
		std::string keyCaloCell = *firstKey;
		StatusCode status = sg->retrieve(_clockwork->calocells,keyCaloCell);
		if(status.isSuccess() && _clockwork->calocells!=0) {
			// Build a basis for the scene graph
			//   1. Create one Separator and one Material for each member of VP1CC_SeparatorTypes
			//   2. Also create Separator Helper objects
			VP1CC_SeparatorTypesSet::iterator itSepType=_clockwork->sepTypes.begin();
			for(; itSepType!=_clockwork->sepTypes.end(); itSepType++) {
				SoSeparator* childSep = new SoSeparator();
				VP1ExtraSepLayerHelper* sepHelper = new VP1ExtraSepLayerHelper(childSep);

				root->addChild(_clockwork->controller->GetMaterial(*itSepType));
				root->addChild(childSep);

				_clockwork->sepHelperMap[*itSepType] = sepHelper;
			}

			setupEnergyMode();

		} else
			message("Unable to retrieve Calo Cells from Store Gate for this event");

		while(++firstKey!=lastKey)
			messageDebug("Another Calo Cell collection found in the SG with key = " + QString((*firstKey).c_str()));
	} else
		message("No Calo Cells in Store Gate for this event");     // No keys found

	// ---------------- Retrieve calo cells from SG ---------------------

	// -------------------------- Digits ---------------------------------
	_clockwork->controller->ClearHideDigitForms();
	_clockwork->controller->EnableDigitsCheckbox(false);
	_clockwork->noLArDigitsEvent = _clockwork->noLArDigitsGlobal;
	_clockwork->noTileDigitsEvent = _clockwork->noTileDigitsGlobal;


	/*
  if(_clockwork->calo_badchannel == 0)
    _clockwork->mainController->chbxEnableBadChannels->setEnabled(false);
  else
    _clockwork->mainController->chbxEnableBadChannels->setEnabled(true);
	 */

	if(!_clockwork->noLArDigitsEvent) {
		// Get keys for LArDigitContainers in the Store Gate
		// If we have more than one container then it means we have All and/or Thinned and/or IIC -> use All if available, otherwise use Thinned
		std::vector<std::string> strLArDigitKeys = sg->keys<LArDigitContainer>();
		firstKey = strLArDigitKeys.begin();
		lastKey = strLArDigitKeys.end();

		if(firstKey==lastKey){
			message("No LAr Digits in Store Gate for this event");
			_clockwork->noLArDigitsEvent = true;
		} else {
			std::string strLArDigitKeys(""), tagThinned(""), tagIIC(""), tagFirstOther("");

			for(;firstKey!=lastKey;++firstKey) {
				if((*firstKey).find("Thinned")!=std::string::npos)
					tagThinned = *firstKey;
				else if((*firstKey).find("IIC")!=std::string::npos)
					tagIIC = *firstKey;
				else if(tagFirstOther=="")
					tagFirstOther = *firstKey;
			}

			if(tagFirstOther!="")
				strLArDigitKeys = tagFirstOther;
			else if(tagThinned!="")
				strLArDigitKeys = tagThinned;
			else
				strLArDigitKeys = tagIIC;

			QString mess = QString("Retrieving LArDigitContainer with the key ") + QString(strLArDigitKeys.c_str());
			messageDebug(mess);

			StatusCode status = sg->retrieve(_clockwork->lar_digits,strLArDigitKeys);

			if(status.isSuccess() && _clockwork->lar_digits != 0) {
				_clockwork->controller->EnableDigitsCheckbox(true);
				std::ostringstream numLarDigits;
				numLarDigits << _clockwork->lar_digits->size();
				message("Number of LAr digits for this event " + QString(numLarDigits.str().c_str()));
			} else {
				message("Unable to retrieve LAr digits from Store Gate for this event");
				_clockwork->lar_digits = 0;
				_clockwork->noLArDigitsEvent = true;
			}
		} // LArDigits found in the SG
	}

	if(!_clockwork->noTileDigitsEvent) {
		// Get keys for TileDigitContainers in the Store Gate
		// If we have more than one container then use the first one and issue a warning message
		std::vector<std::string> strTileDigitKeys = sg->keys<TileDigitsContainer>();
		firstKey = strTileDigitKeys.begin();
		lastKey = strTileDigitKeys.end();

		if(firstKey==lastKey){
			message("No Tile Digits in Store Gate for this event");
			_clockwork->noTileDigitsEvent = true;
		} else {
			std::string keyTileDigits = *firstKey;
			StatusCode status = sg->retrieve(_clockwork->tile_digits,keyTileDigits);

			if(status.isFailure() || _clockwork->tile_digits==0){
				message("Unable to retrieve Tile digits from Store Gate for this event");
				_clockwork->tile_digits = 0;
				_clockwork->noTileDigitsEvent = true;
			} else {
				_clockwork->controller->EnableDigitsCheckbox(true);

				// ------------ Tile Raw Channels (pulses) ------------------
				std::vector<std::string> strTileRawchanKeys = sg->keys<TileRawChannelContainer>();
				firstKey = strTileRawchanKeys.begin();
				lastKey = strTileRawchanKeys.end();

				if(firstKey==lastKey) {
					message("No Tile Raw Channels in Store Gate for this event");
					_clockwork->noTileDigitsEvent = true;
				} else {
					std::string keyTileRawChan = _clockwork->SearchTileRawchanKeys(strTileRawchanKeys);

					// If no keys found from the specified order then just try the fisr one
					if(keyTileRawChan=="")
						keyTileRawChan = *firstKey;

					QString mess = QString("Retrieving Tile Raw Channels with the key ") + QString(keyTileRawChan.c_str());
					messageDebug(mess);

					status = sg->retrieve(_clockwork->tile_rawchannel,keyTileRawChan);
					if(status.isFailure() || _clockwork->tile_rawchannel==0) {
						message("Unable to retrieve Tile raw channels from Store Gate for this event");
						_clockwork->tile_rawchannel = 0;
					}
				}
			} // Digits retrieved successfully
		} // Digit container keys found in SG
	} // Tile digits

	// -------------------------- Digits ---------------------------------

	// -------------------------- MBTS ---------------------------------
	// -- material
	root->addChild(_clockwork->controller->GetMaterial(VP1CC_SepMBTS));

	// -- separator
	SoSeparator* mbtsSeparator = new SoSeparator;
	root->addChild(mbtsSeparator);
	try {
		_clockwork->mbtsHelper->buildEventSceneGraph(sg,mbtsSeparator);
		_clockwork->mbtsHelper->refreshGraph(_clockwork->controller->selectionMbts());
	} catch(std::runtime_error& err) {
		message (err.what());
	}
	// -------------------------- MBTS ---------------------------------
	messageDebug("end of buildEventSceneGraph()");
}

void VP1CaloCellSystem::userPickedNode(SoNode* pickedNode, SoPath *pickedPath)
{
	messageDebug("VP1CaloCellSystem::userPickedNode()");

	SoNode* _tail = pickedPath->getTail();

	// Find the VP1CC object which correspond to the selected 3D object
	VP1CC_SoNode2CCMap::iterator itNode2CC = _clockwork->node2ccMap.find(_tail);

	if(itNode2CC!=_clockwork->node2ccMap.end()){
		// VP1CC object found.

		std::string channel_string = "";
		if ( _clockwork->lar_cabling && _clockwork->lar_onlineID ) {
			VP1CC_LAr* larCC = dynamic_cast<VP1CC_LAr*>((*itNode2CC).second);
			if(larCC) {
				Identifier cellOffline = larCC->getID();

				std::ostringstream idstring;
				if ( _clockwork->calo_id->is_em(cellOffline) || _clockwork->calo_id->is_hec(cellOffline) ) {
					idstring <<"(b_ec=" << _clockwork->calo_id->pos_neg(cellOffline) << ", samp="
							<< _clockwork->calo_id->sampling(cellOffline) << ", reg="
							<< _clockwork->calo_id->region(cellOffline) << ", eta="
							<< _clockwork->calo_id->eta(cellOffline) << ", phi="
							<< _clockwork->calo_id->phi(cellOffline) << ") ";
				}
				else if ( _clockwork->calo_id->is_fcal(cellOffline) ) {
					idstring <<"(pos_neg=" << _clockwork->calo_id->pos_neg(cellOffline) << ",mod="
							<< _clockwork->calo_id->sampling(cellOffline) << ",eta="
							<< _clockwork->calo_id->eta(cellOffline) << ",phi="
							<< _clockwork->calo_id->phi(cellOffline) << ") ";
				}
				channel_string = idstring.str();

				HWIdentifier hwId;
				try {
					hwId = _clockwork->lar_cabling->createSignalChannelID(cellOffline);
				} catch(LArID_Exception&) {
					message("LArID Exception caught while creating signal channel id");
					_clockwork->controller->ClearHideDigitForms();
					return;
				}
				channel_string += _clockwork->lar_onlineID->channel_name(hwId);
			}
		}

		// Print information to the log window
		std::vector<std::string> msg;
		try {
			msg = (*itNode2CC).second->ToString(_clockwork->calo_id, channel_string);
		} catch(std::runtime_error& err) {
			message(err.what());
			return;
		}

		for(unsigned int ii=0; ii<msg.size(); ii++)
			message((msg[ii]).c_str());

		// Digits
		if(_clockwork->controller->showDigits()) {

			messageDebug("showDigits() is checked...");


			// ---------------------- LAr -------------------------
			VP1CC_LAr* larCC = dynamic_cast<VP1CC_LAr*>((*itNode2CC).second);
			if(larCC) {
				messageDebug("LAr node selected");
				if(!_clockwork->noLArDigitsEvent) {
					Identifier cellOffline = larCC->getID();
					HWIdentifier hwId;
					try {
						hwId = _clockwork->lar_cabling->createSignalChannelID(cellOffline);
					} catch(LArID_Exception& e) {
						message("EXCEPTION!! LArIDException caught while creating signal channel id!!");
						_clockwork->controller->ClearHideDigitForms();
						return;
					}

					LArDigitContainer::const_iterator itLArDig=_clockwork->lar_digits->begin();
					for(;itLArDig!=_clockwork->lar_digits->end(); itLArDig++) {
						if((*itLArDig)->hardwareID()==hwId) {

							message("displaying LAr digits...");

							try {
								_clockwork->controller->displayLArDigits( (*itLArDig)->nsamples(),
										(*itLArDig)->samples(),
										msg);
							} catch (std::exception& e) {
								message("Exception caught!!");
								std::cout << "ERROR!!! Exception caught! --> " << e.what() << std::endl;
								QString title = "ERROR!!! Exception caught!";
								QString mex = "While displaying LAr digits, an exception has been caught!\n\nException details:\n\n" + QString::fromStdString(e.what());
								messageDebug("Clearing the DigitForm and returning...");
								_clockwork->controller->ClearHideDigitForms();
								messageDebug("Displaying the QMessageBox...");
								QWidget* widget = 0; // the message box will be parentless
								QMessageBox::warning( widget, title,
										mex,
										QMessageBox::Ok,
										QMessageBox::Ok
								);
								messageDebug("Box shown. Returning...");
								return;
							}
							messageDebug("Ok. Returning...");
							return;
						} // we found the digit container
					} // loop over digit containers

					// if not found the right cell and thus if not returning, we arrive here...
					message("Unable to find digits for the selected cell");
				} else {
					message("LAr digit display not available");
				}

				// Since we are here, hide digits widgets
				messageDebug("LAr. Clearing digits form...");
				_clockwork->controller->ClearHideDigitForms();
				messageDebug("Cleared. Returning...");
				return;
			}// we are in LAr CC


			// ------------- Tile Barrel & End Cap ------------------
			VP1CC_TileBarEc* tileCC = dynamic_cast<VP1CC_TileBarEc*>((*itNode2CC).second);
			if(tileCC) {
				messageDebug("Tile Barrel & End Cap");
				if(!_clockwork->noTileDigitsEvent) {
					int _frag1, _channel1, _frag2, _channel2;
					if((tileCC->GetFragChannel(_clockwork->tile_hw_id,false,_frag1,_channel1)==0) &&
							(tileCC->GetFragChannel(_clockwork->tile_hw_id,true,_frag2,_channel2)==0)) {

						// ----------------- Plot digits -------------------
						const TileDigits* digits1 = 0;
						TileDigitsContainer::const_iterator itTileDig = _clockwork->tile_digits->begin();

						// loop over Tile digits
						for(;itTileDig!=_clockwork->tile_digits->end();itTileDig++) {
							if((*itTileDig)->size() >0 && (*itTileDig)->identify()==_frag1) {
								messageDebug("Baa");
								digits1 = (**itTileDig)[_channel1];

								// search for another container when we are in D0
								const TileDigits* digits2 = 0;
								if(_channel2==_channel1) {
									itTileDig = _clockwork->tile_digits->begin();

									for(;itTileDig!=_clockwork->tile_digits->end();itTileDig++) {
										if((*itTileDig)->size() >0 && (*itTileDig)->identify()==_frag2) {
											digits2 = (**itTileDig)[_channel2];
											break;
										}
									}
								} else {  // D0
									messageDebug("CCC");
									digits2 = (**itTileDig)[_channel2];
								}

								if(digits2==0) {
									messageDebug("DDD");
									message("Unable to find digits for the PMT 1");
									_clockwork->controller->ClearHideDigitForms();
									return;
								}
								messageDebug("Displaying Tile digits...");
								_clockwork->controller->displayTileDigits(digits1->NtimeSamples(),
										digits1->samples(),
										digits2->samples(),
										msg);
								break;
							} // found digits 1
						} // loop over tile digits


						if(digits1==0) {
							message("Unable to find digits for the PMT 0");
							_clockwork->controller->ClearHideDigitForms();
							return;
						}
						// ----------------- Plot digits -------------------

						// ---------------- Plot pulse --------------------
						messageDebug("Tile BB & EC plot pulse");
						TileRawChannelContainer::const_iterator itTileRawCh = _clockwork->tile_rawchannel->begin();
						const TileRawChannel* rawchannel1 = 0;
						// loop over Tile channels
						for(;itTileRawCh!=_clockwork->tile_rawchannel->end();itTileRawCh++) {

							if((*itTileRawCh)->size()>0 && (*itTileRawCh)->identify()==_frag1) {
								rawchannel1 = (**itTileRawCh)[_channel1];

								// find Raw Channel 2
								const TileRawChannel* rawchannel2 = 0;
								if(_channel2==_channel1) {
									messageDebug("channel2==_channel1");
									// D0
									itTileRawCh = _clockwork->tile_rawchannel->begin();
									for(;itTileRawCh!=_clockwork->tile_rawchannel->end();itTileRawCh++)
										if((*itTileRawCh)->size()>0 && (*itTileRawCh)->identify()==_frag2) {
											messageDebug("Get channel2...");
											//the following value is never used, gets immediately overwritten after the 
											// break from loop
											//rawchannel2 = (**itTileRawCh)[_channel2];
											break;
										}
								} else{
									messageDebug("channel2!=_channel1. Get channel2...");
								}
								rawchannel2 = (**itTileRawCh)[_channel2];

								if(rawchannel2==0) {
									message("Unable to get raw channel for PMT1");
									break;
								}

								messageDebug("Displaying Tile pulse...");
								std::cout << "debug vars :" << rawchannel1 << " - " <<  rawchannel2 << std::endl;
								std::cout << _clockwork->tile_rawchannel << std::endl;
								std::cout << digits1->samples().size() << std::endl;

								// note: the controller is the VP1CaloCellController class
								try {
									_clockwork->controller->displayTilePulse(rawchannel1,rawchannel2,_clockwork->tile_rawchannel,digits1->samples().size());
								} catch (TileCalib::InvalidRawChanUnit& e) {
									message("Exception caught!!");
									std::cout << "ERROR!!! Exception caught from Tile! --> " << e.what() << std::endl;
									QString title = "ERROR!!! Exception caught!";
									QString mex = "While displaying Tile Pulse, an exception has been caught!\n\nException details:\n\n" + QString::fromStdString(e.what());
									messageDebug("Clearing the DigitForm and returning...");
									_clockwork->controller->ClearHideDigitForms();
									messageDebug("Displaying the QMessageBox...");
									QWidget* widget = 0; // the message box will be parentless
									QMessageBox::warning( widget, title,
											mex,
											QMessageBox::Ok,
											//													 | QMessageBox::Discard
											//													 | QMessageBox::Cancel,
											QMessageBox::Ok
									);
									messageDebug("Returning...");
									return;
								}
								messageDebug("Displayed Tile pulse.");
								break;
							}
						}

						if(rawchannel1==0) {
							message("Unable to get raw channel for PMT0");
						}
						// ---------------- Plot pulse --------------------
						messageDebug("Returning...");
						return;
					}
					message("Unable to get drawer and channel IDs");
				} else
					message("Tile digit display not available");


				messageDebug("clearing...");
				_clockwork->controller->ClearHideDigitForms();
				messageDebug("cleared. Return...");
				return;
			}

			// -------------------------- Tile Crack ------------------------------

			VP1CC_TileCrack* tileCrack = dynamic_cast<VP1CC_TileCrack*>((*itNode2CC).second);
			if(tileCrack) {
				messageDebug("Tile Crack");
				if(!_clockwork->noTileDigitsEvent) {
					int _frag, _channel;
					if(tileCrack->GetFragChannel(_clockwork->tile_hw_id,false,_frag,_channel)==0) {
						// ----------------- Plot digits -------------------
						const TileDigits* digits = 0;
						TileDigitsContainer::const_iterator itTileDig = _clockwork->tile_digits->begin();
						for(;itTileDig!=_clockwork->tile_digits->end();itTileDig++) {
							if((*itTileDig)->size() >0 && (*itTileDig)->identify()==_frag) {
								digits = (**itTileDig)[_channel];

								_clockwork->controller->displayTileDigits(digits->NtimeSamples(),
										digits->samples(),
										msg);
								break;
							}
						}
						if(digits==0) {
							message("Unable to find digits");
							_clockwork->controller->ClearHideDigitForms();
							return;
						}
						// ----------------- Plot digits -------------------

						// ---------------- Plot pulse --------------------
						TileRawChannelContainer::const_iterator itTileRawCh = _clockwork->tile_rawchannel->begin();
						const TileRawChannel* rawchannel = 0;
						for(;itTileRawCh!=_clockwork->tile_rawchannel->end();itTileRawCh++) {
							messageDebug("Tile Crack plot pulse");
							if((*itTileRawCh)->size()>0 && (*itTileRawCh)->identify()==_frag) {
								rawchannel = (**itTileRawCh)[_channel];
								_clockwork->controller->displayTilePulse(rawchannel,_clockwork->tile_rawchannel,digits->samples().size());
							}
						}
						if(rawchannel==0)
							message("Unable to get raw channel");
						// ---------------- Plot pulse --------------------
						return;
					}
					message("Unable to get drawer and channel ID");
				} else
					message("Tile digit display not available");

				_clockwork->controller->ClearHideDigitForms();

				return;
			}

		}// show digits
		return;
	}// Find node in the VP1CC map

	messageDebug("out of the big loop on nodes...");

	// Maybe MBTS has been selected?
	std::vector<std::string> msg = _clockwork->mbtsHelper->userPickedNode(pickedNode);
	for(unsigned int ii=0; ii<msg.size(); ii++)
		message((msg[ii]).c_str());
}

QByteArray VP1CaloCellSystem::saveState()
{
	VP1Serialise serialise(0/*version*/,this);
	serialise.save(IVP13DSystemSimple::saveState());

	ensureBuildController();

	serialise.save(_clockwork->controller->saveSettings());

	serialise.disableUnsavedChecks();
	return serialise.result();
}

void VP1CaloCellSystem::restoreFromState(QByteArray ba)
{
	VP1Deserialise state(ba,this);

	if (state.version()!=0) {
		message("Warning: State data in .vp1 file is in wrong format - ignoring!");
		return;
	}
	ensureBuildController();

	IVP13DSystemSimple::restoreFromState(state.restoreByteArray());

	_clockwork->controller->restoreSettings(state.restoreByteArray());

	state.disableUnrestoredChecks();
}

void VP1CaloCellSystem::energyMode()
{
	// Remove objects from 3D scene
	while(_clockwork->node2ccMap.begin()!=_clockwork->node2ccMap.end()) {
		VP1CC_SoNode2CCMap::iterator it_node2cc = _clockwork->node2ccMap.begin();
		it_node2cc->second->remove3DObjects(&_clockwork->node2ccMap);
	}

	// Clear node 2 CC map
	_clockwork->node2ccMap.clear();

	// Delete Cell Managers and clear the map
	VP1CCManagerContainer::iterator it_ccman = _clockwork->cell_managers.begin();
	for(; it_ccman!=_clockwork->cell_managers.end(); it_ccman++)
		if(it_ccman->second) delete it_ccman->second;

	_clockwork->cell_managers.clear();

	// Setup new managers from scratch in the new energy mode
	setupEnergyMode();
}

void VP1CaloCellSystem::setupEnergyMode()
{
	// Prepare Separator Helpers for large changes
	VP1CC_SeparatorMap::iterator itSepHelp = _clockwork->sepHelperMap.begin();
	for(;itSepHelp != _clockwork->sepHelperMap.end(); itSepHelp++)
		itSepHelp->second->largeChangesBegin();

	// Create Managers
	_clockwork->BuildCellManagers();

	// Assign Cells to Managers
	_clockwork->FillManagers();

	// Update scene with initial values of controller objects
	const VP1CCIntervalMap& intervals = _clockwork->controller->selectionIntervals();
	VP1CCManagerContainer::iterator it = _clockwork->cell_managers.begin();
	for(; it!=_clockwork->cell_managers.end(); it++)
		it->second->selectionUpdated(intervals);

	// Large changes over
	itSepHelp = _clockwork->sepHelperMap.begin();
	for(;itSepHelp != _clockwork->sepHelperMap.end(); itSepHelp++)
		itSepHelp->second->largeChangesEnd();

	updateGUI();
}
