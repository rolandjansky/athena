/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <map>

class Properties {

public:
    Properties() {};
    bool Read (const std::string& strFile) {
        std::ifstream is(strFile.c_str());
        if (!is.is_open()) return false;

        while (!is.eof()) {
            std::string strLine;
            getline(is,strLine);

            int nPos = strLine.find('=');
            if ((signed int)std::string::npos == nPos) continue; // no '=', invalid line;
            std::string strKey = strLine.substr(0,nPos);
            std::string strVal = strLine.substr(nPos + 1, strLine.length() - nPos + 1);
            m_map.insert(std::map<std::string,std::string>::value_type(strKey,strVal));
        }
        return true;
    }

    bool GetValue(const std::string& strKey, std::string& strValue) const {
        std::map<std::string,std::string>::const_iterator i;
        i = m_map.find(strKey);

        if (i != m_map.end()) {
            strValue = i->second;
            return true;
        }
        return false;
    }

protected:
    std::map<std::string,std::string> m_map;
};

class ScaleFactorMap {
    
public:
    std::string mapFile;
    std::string isolationloc;
    std::string offlineloc;
    std::string triggerloc;
    std::string mapfileloc;
    
    /**
     if isolation, trigger and offline are in the same location
    */
    ScaleFactorMap(const std::string& rootloc)
    {
        isolationloc = rootloc;
        offlineloc = rootloc;
        triggerloc = rootloc;
        mapfileloc = "../data/";
    }
    
    /**
     if isolation, trigger and offline are in the different locations
     and map gets created in current working directory
    */
    ScaleFactorMap(const std::string& isoloc, const std::string& offloc, const std::string& trigloc)
    {
        isolationloc = isoloc;
        triggerloc = trigloc;
        offlineloc = offloc;
        mapfileloc = "./";
    }
    
    /**
     if isolation, trigger, offline and mapfile are in the different locations
    */
    ScaleFactorMap(const std::string& isoloc, const std::string& offloc, const std::string& trigloc, const std::string& maploc)
    {
        isolationloc = isoloc;
        triggerloc = trigloc;
        offlineloc = offloc;
        mapfileloc = maploc;
    }
    
    bool contain(const std::string& str, std::string strFind)
    {
        if (str.find(strFind) != std::string::npos) {
            return true;
        }
        return false;
    }
    
    std::string convertToKeyFromOfflineFilename(const std::string& fileName)
    {
        std::string key;
        if (contain(fileName, "RecoTrk")) {
            key = "Reconstruction";
        }
        if (contain(fileName, "LooseAndBLayerLLH_d0z0")) {
            key = "LooseBLayer";
        }
        if (contain(fileName, "MediumLLH_d0z0")) {
            key = "Medium";
        }
        if (contain(fileName, "TightLLH_d0z0")) {
            key = "Tight";
        }
        if (contain(fileName, "LooseAndBLayerLLH") && !contain(fileName, "d0z0")) {
            key = "LooseBLayer_Nod0z0";
        }
        if (contain(fileName, "MediumLLH") && !contain(fileName, "d0z0")) {
            key = "Medium_Nod0z0";
        }
        if (contain(fileName, "TightLLH") && !contain(fileName, "d0z0")) {
            key = "Tight_Nod0z0";
        }
        // These files having ID correlations only exist in ICHEP_v2 
        // and ICHEP_v3 recommendations
        if (contain(fileName, "_correlationID")){
            key +="_corrID";
        }
        return key;
    }
    
    std::string convertToKeyFromIsolationFilename(const std::string& fileName)
    {
        std::string key = convertToKeyFromOfflineFilename(fileName);
        
        //Isolation specific WPs
        if (contain(fileName, "isolFixedCutLoose.")) {
            key += "_FixedCutLoose";
        }
        if (contain(fileName, "isolFixedCutTight.")) {
            key += "_FixedCutTight";
        }
        if (contain(fileName, "isolFixedCutTightTrackOnly.")) {
            key += "_FixedCutTightTrackOnly";
        }
        if (contain(fileName, "isolGradient.") ) {
            key += "_Gradient";
        }
        if (contain(fileName, "isolGradientLoose.")) {
            key += "_GradientLoose";
        }
        if (contain(fileName, "isolLoose.") ) {
            key += "_Loose";
        }
        // As one of the trigger SF file has name isolLooTrackOnly in ICHEP_v1 files
        if (contain(fileName, "isolLooseTrackOnly.") || contain(fileName, "isolLooTrackOnly.")) {
            key += "_LooseTrackOnly";
        }
        if (contain(fileName, "isolTight.")) {
            key += "_Tight";
        }
        else {
            key +="";
        }
        return key;
    }
    
    std::string convertToKeyFromTriggerFilename(const std::string& fileName)
    {
        std::string key;
        // Trigger files for th full trigger names DI_E case
        if (contain(fileName, "DI_E_2015_e12_lhloose_L1EM10VH_2016_e15_lhvloose_nod0_L1EM13VH")) {
            key = "DI_E_2015_e12_lhloose_L1EM10VH_2016_e15_lhvloose_nod0_L1EM13VH";
        }
        if (contain(fileName, "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0")) {
            key = "DI_E_2015_e12_lhloose_L1EM10VH_2016_e17_lhvloose_nod0";
        }
        if (contain(fileName, "DI_E_2015_e17_lhloose_2016_e17_lhloose")) {
            key = "DI_E_2015_e17_lhloose_2016_e17_lhloose";
        }

        // Trigger files with full trigger names for MULTI_L case
        if (contain(fileName, "MULTI_L_2015_e17_lhloose_2016_e17_lhloose_nod0")) {
            key = "MULTI_L_2015_e17_lhloose_2016_e17_lhloose_nod0";
        }
        if (contain(fileName, "MULTI_L_2015_e7_lhmedium_2016_e7_lhmedium_nod0")) {
            key = "MULTI_L_2015_e7_lhmedium_2016_e7_lhmedium_nod0";
        }
        if (contain(fileName, "MULTI_L_2015_e12_lhloose_2016_e12_lhloose_nod0")) {
            key = "MULTI_L_2015_e12_lhloose_2016_e12_lhloose_nod0";
        }
        if (contain(fileName, "MULTI_L_2015_e24_lhmedium_L1EM20VHI_2016_e24_lhmedium_nod0_L1EM20VHI")) {
            key = "MULTI_L_2015_e24_lhmedium_L1EM20VHI_2016_e24_lhmedium_nod0_L1EM20VHI";
        }
        if (contain(fileName, "MULTI_L_2015_e24_lhmedium_L1EM20VHI_2016_e26_lhmedium_nod0_L1EM22VHI")) {
            key = "MULTI_L_2015_e24_lhmedium_L1EM20VHI_2016_e26_lhmedium_nod0_L1EM22VHI";
        }
        
        // Trigger files for TRI_E case
        if (contain(fileName, "TRI_E_2015_e17_lhloose_2016_e17_lhloose_nod0")){
            key = "TRI_E_2015_e17_lhloose_2016_e17_lhloose_nod0";
        }
        if (contain(fileName, "TRI_E_2015_e17_lhloose_2016_e17_lhmedium_nod0")){
            key = "TRI_E_2015_e17_lhloose_2016_e17_lhmedium_nod0";
        }
        if (contain(fileName, "TRI_E_2015_e9_lhloose_2016_e9_lhloose_nod0")){
            key = "TRI_E_2015_e9_lhloose_2016_e9_lhloose_nod0";
        }
        if (contain(fileName, "TRI_E_2015_e9_lhloose_2016_e9_lhmedium_nod0")){
            key = "TRI_E_2015_e9_lhloose_2016_e9_lhmedium_nod0";
        }
        
        // Trigger files for SUNGLE_E case
        if (contain(fileName, "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e24_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0")) {
            key = "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e24_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0";
        }
        if (contain(fileName, "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0")) {
            key = "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0";
        }
        
        // Extra trigger files that contains only 2015 or 2016 trigger
        // 2015 
        if (!contain(fileName, "2016") && contain(fileName, "2015") && contain(fileName, "e17_lhloose_L1EM15")) {
            key = "2015_e17_lhloose_L1EM15";
        }
        if (!contain(fileName, "2016") && contain(fileName, "2015") && contain(fileName, "e17_lhmedium")) {
            key = "2015_e17_lhmedium";
        }
         
        // 2016 
        if (!contain(fileName, "2015") && contain(fileName, "2016") && contain(fileName, "e17_lhmedium_nod0")) {
            key = "2016_e17_lhmedium_nod0";
        }
        if (!contain(fileName, "2015") && contain(fileName, "2016") && contain(fileName, "e17_lhmedium_nod0_L1EM15HI")) {
            key = "2016_e17_lhmedium_nod0_L1EM15HI";
        }
        if (!contain(fileName, "2015") && contain(fileName, "2016") && contain(fileName, "e17_lhmedium_nod0_ivarloose_L1EM15HI")) {
            key = "2016_e17_lhmedium_nod0_ivarloose_L1EM15HI";
        }

       if (contain(fileName, "efficiencySF.")){
            key += ("_" + convertToKeyFromIsolationFilename(fileName));
        } 
        else {
             key = ("Eff_"+ key +"_" + convertToKeyFromIsolationFilename(fileName));
        }
        return key;
    }
    
    int getdir(std::string dir, std::vector<std::string>& files)
    {
        DIR* dp;
        struct dirent* dirp;
        if ((dp = opendir(dir.c_str())) == NULL) {
            std::cout << "Error(" << errno << ") opening " << dir << std::endl;
            return errno;
        }
        
        while ((dirp = readdir(dp)) != NULL) {
            std::string fileName = std::string(dirp->d_name);
            if (contain(fileName, "root")) {
                std::string key;
                if (contain(dir, "offline")) {
                    key = convertToKeyFromOfflineFilename(fileName);
                }
                if (contain(dir, "trigger")) {
                    key = convertToKeyFromTriggerFilename(fileName);
                }
                if (contain(dir, "isolation")) {
                    key = convertToKeyFromIsolationFilename(fileName);
                }
                std::string reallocation = dir + fileName;
                std::string properties = key + "=" + reallocation;
                files.push_back(properties);
            }
        }
        closedir(dp);
        return 0;
    }
    
    void deleteExistingMapFile(std::string file)
    {
        remove(file.c_str());
    }
    
    void writeToFile(std::vector<std::string>& map, std::string file)
    {
        //ofstream output_file(file);
        //Different compiler demands
        std::ofstream output_file(file.c_str());
        std::ostream_iterator<std::string> output_iterator(output_file, "\n\n");
        //std::ostream_iterator<std::string> output_iterator(output_file, "\n");
        copy(map.begin(), map.end(), output_iterator);
    }
    
    std::string getValueByKey(const std::string& key)
    {
        Properties props;
        props.Read(mapFile);
        
        std::string strVal;
        if (!props.GetValue(key, strVal)) {
            std::cout << "Error(" << key << ") not found " << std::endl;
        }
        else {
            return strVal;
        }
        return "";
    }
    
    void generateMap()
    {
        const std::string isolationDir = std::string("isolation/");
        const std::string offlineDir = std::string("offline/");
        const std::string triggerDir = std::string("trigger/");
        mapFile = std::string(mapfileloc + "map1.txt");
        
        std::vector<std::string> files = std::vector<std::string>();
        
        getdir(isolationloc + isolationDir, files);
        getdir(offlineloc + offlineDir, files);
        getdir(triggerloc + triggerDir, files);
        
        deleteExistingMapFile(mapFile);
        writeToFile(files, mapFile);
    }
};

//int generateMapFile(int argc, char* argv[])
int main(int argc, char* argv[])
{
    ///1st way : WORKING
    std::string str1 = std::string(argv[1]);
    ScaleFactorMap scaleMap(str1);
    scaleMap.generateMap();
    std::cout << scaleMap.getValueByKey("Medium") <<std::endl;
    ///2nd Way : WORKING
    //string str2 = argv[2];
    //string str3 = argv[3];
    //ScaleFactorMap scaleMap (str1, str2, str3, str2);
    //scaleMap.generateMap();
}
