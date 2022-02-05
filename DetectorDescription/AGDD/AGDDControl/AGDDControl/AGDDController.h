/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDController_H
#define AGDDController_H

class AGDDBuilder;
class IAGDDParser;
class GeoFullPhysVol;

#include "AGDDKernel/AGDDSectionStore.h"
#include "AGDDKernel/AGDDVolumeStore.h"
#include "AGDDKernel/AGDDSectionStore.h"
#include "AGDDKernel/AGDDDetectorStore.h"
#include "AGDDKernel/AGDDPositionerStore.h"
#include "AGDDKernel/AliasStore.h"
#include "AGDDModel/AGDDColorStore.h"
#include "AGDDModel/AGDDMaterialStore.h"
#include "AGDDModel/AGDDParameterStore.h"
#include "AGDDControl/XMLHandlerStore.h"
#include "AGDDControl/ExpressionEvaluator.h"
#include "CxxUtils/checker_macros.h"

#include <string>
#include <vector>


class AGDDController {
public:
        AGDDController();
	~AGDDController();
	void SetBuilder(AGDDBuilder *b);
	void SetParser(IAGDDParser *b);
	AGDDBuilder* GetBuilder();
	IAGDDParser* GetParser();
	void AddFile(const std::string& fName);
	void AddSection(const std::string& section);
	void AddVolume(const std::string& volume);
	void ParseFiles();
	void ParseString(const std::string&);
	bool WriteAGDDtoDBFile(const std::string&);
	void PrintSections() const;
	void BuildVolumes();
	void BuildSections();
	void BuildAll();
	void Clean();
	
        XMLHandlerStore& GetHandlerStore();
        AGDDVolumeStore& GetVolumeStore();
        AGDDSectionStore& GetSectionStore();
        AGDDColorStore& GetColorStore();
        AGDDDetectorStore& GetDetectorStore();
        AGDDPositionerStore& GetPositionerStore();
        AGDDMaterialStore& GetMaterialStore();
        AGDDParameterStore& GetParameterStore();
        AliasStore& GetAliasStore();
        AGDD::ExpressionEvaluator& Evaluator();

        //static void PrintVolumeHierarchy(const std::string&, int);
	
	void UseGeoModelDetector ATLAS_NOT_THREAD_SAFE (const std::string&);
	
	void Locked(bool b) {m_locked=b;}
	bool Locked() const {return m_locked;}
	
	void DisableSections(bool b) {m_disableSections=b;}
	bool DisableSections() const {return m_disableSections;}
private:
	
        AGDDController(const AGDDController& c) = delete;
        AGDDController& operator=(const AGDDController& c) = delete;
	IAGDDParser *m_theParser;
	AGDDBuilder* m_theBuilder;
	
	std::vector<std::string> m_filesToParse;
	std::vector<std::string> m_sectionsToBuild;
	std::vector<std::string> m_volumesToBuild;
	std::vector<std::string> m_structuresToBuild;
	
	bool m_locked;
	bool m_disableSections;
	
	int m_printLevel;

        AGDDVolumeStore m_vs;
        AGDDSectionStore m_ss;
        AGDDColorStore m_cs;
        AGDDDetectorStore m_ds;
        AGDDPositionerStore m_ps;
        AGDDMaterialStore m_ms;
        AGDDParameterStore m_prs;
        XMLHandlerStore m_xs;
        AliasStore m_as;
        AGDD::ExpressionEvaluator m_eval;
};

#endif
