// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "HTTMapMakerAlg.h"

#include "TrigHTTInput/IHTTEventInputHeaderTool.h"
#include "TH2.h"

#include "GaudiKernel/IEventProcessor.h"


///////////////////////////////////////////////////////////////////////////////
// Initialize

HTTMapMakerAlg::HTTMapMakerAlg (const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_monitorFile((m_outFileName.value() + ".root").c_str(), "RECREATE")
{
}


StatusCode HTTMapMakerAlg::initialize()
{
    std::stringstream ss(m_description);
    std::string line;
    ATH_MSG_INFO("Tag config:");
    if (m_description.value() != nullptr) 
        while (std::getline(ss, line, '\n')) 
            ATH_MSG_INFO('\t' << line);
    
    // reset Hit and Module vectors    
    m_pbHits.clear(); m_peHits.clear(); m_sbHits.clear(); m_seHits.clear(); m_allHits.clear();
    m_track2modules.clear(); m_track2slice.clear(); m_slice2modules.clear();
    m_key_etamods.clear(); m_key_etamods2.clear(); m_usedTracks.clear();
    m_radii.clear(); m_keylayer.clear(); m_keylayer2.clear(); 

    parseKeyString();
    ATH_CHECK(m_hitInputTool.retrieve());

    ATH_MSG_DEBUG("initialize() Instantiating root objects");
    ATH_MSG_DEBUG("initialize() Finished");

    return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//                          MAIN EXECUTE ROUTINE                             //
///////////////////////////////////////////////////////////////////////////////

StatusCode HTTMapMakerAlg::execute()
{
    // Read inputs
    bool done = false;
    ATH_CHECK(readInputs(done));

    if (done) {
      IEventProcessor* appMgr=nullptr;
      ATH_CHECK(service("ApplicationMgr",appMgr));
      if (!appMgr) {
          ATH_MSG_ERROR("Failed to retrieve ApplicationMgr as IEventProcessor");
          return StatusCode::FAILURE;
      }
      return appMgr->stopRun();
    }

    // Reset data pointers
    m_eventHeader.reset();

    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
//                       INPUT READING AND PROCESSING                        //
///////////////////////////////////////////////////////////////////////////////


StatusCode HTTMapMakerAlg::readInputs(bool & done)
{
    // Read primary input
    ATH_CHECK(m_hitInputTool->readData(&m_eventHeader, done));
    if (done)
    {
        ATH_MSG_INFO("Cannot read more events from file, returning");
        return StatusCode::SUCCESS; // end of loop over events
    }
 
    HTTEventInfo eventinfo = m_eventHeader.event();
    ATH_MSG_DEBUG ("Getting Event " << eventinfo);

    for (auto hit: m_eventHeader.hits()) // fill track to modules map and hit vectors
    { 
        // barcode cut: these hits are not associated with our single muon tracks
        if (hit.getBarcodePt() == 0) continue;

        SiliconTech det = hit.getDetType();
        DetectorZone bec = hit.getDetectorZone();
        int lyr = hit.getPhysLayer(); 
        int eta = hit.getHTTEtaModule();
        int phi = hit.getPhiModule();

        if (hit.isPixel() && hit.isBarrel()) {
            m_pbHits.push_back(hit);
            if (lyr > m_pbmax) m_pbmax = lyr;
        }
        else if (hit.isPixel() && !hit.isBarrel()) { 
            int HTTlayer = eta + m_diskIndex[lyr];
            hit.setPhysLayer(HTTlayer);
            lyr = hit.getPhysLayer(); 
            m_peHits.push_back(hit);
            if (bec == DetectorZone::posEndcap && lyr > m_pemax[0]) m_pemax[0] = lyr;
            if (bec == DetectorZone::negEndcap && lyr > m_pemax[1]) m_pemax[1] = lyr;
        }
        else if (!hit.isPixel() && hit.isBarrel()) {
            m_sbHits.push_back(hit);
            if (lyr > m_sbmax) m_sbmax = lyr;
        }
        else if (!hit.isPixel() && !hit.isBarrel()) {
            m_seHits.push_back(hit);
            if (bec == DetectorZone::posEndcap && lyr > m_semax[0]) m_semax[0] = lyr;
            if (bec == DetectorZone::negEndcap && lyr > m_semax[1]) m_semax[1] = lyr;
        }
        else ATH_MSG_ERROR("Invalid Region");

        if (isOnKeyLayer(1,det,bec,lyr)) m_key_etamods.insert(eta); // keep track of all etamods on the key layer for slicing
        
        if (m_key2) // if doing 2D slicing
            if (isOnKeyLayer(2,det,bec,lyr)) m_key_etamods2.insert(eta); 

        Module mod = Module(det, bec, lyr, eta, phi);
        if (std::find(m_track2modules[hit.getEventIndex()].begin(), m_track2modules[hit.getEventIndex()].end(), mod) == m_track2modules[hit.getEventIndex()].end())
            m_track2modules[hit.getEventIndex()].push_back(mod);
        m_allHits.push_back(hit);
    }

    return StatusCode::SUCCESS;
}




///////////////////////////////////////////////////////////////////////////////
//                     OUTPUT WRITING AND MONITORING                         //
///////////////////////////////////////////////////////////////////////////////
StatusCode HTTMapMakerAlg::writePmapAndRmap(std::vector<HTTHit> const & pbHits, std::vector<HTTHit> const & peHits, std::vector<HTTHit> const & sbHits, std::vector<HTTHit> const & seHits, int reg)
{
    // Plane Map 

    // to avoid vector _M_range_check errors, print at least one line for each DetectorZone
    if (m_pbmax == -1) m_pbmax = 0;
    if (m_sbmax == -1) m_sbmax = 0;
    if (m_pemax[0] == -1) m_pemax[0] = 0;
    if (m_pemax[1] == -1) m_pemax[1] = 0;
    if (m_semax[0] == -1) m_semax[0] = 0;
    if (m_semax[1] == -1) m_semax[1] = 0;

    std::string pmap_path = m_outFileName.value() + "region" + std::to_string(m_region) + ".pmap";
    ATH_MSG_INFO("Creating pmap: " << pmap_path);
    m_pmap.open(pmap_path, std::ofstream::out);
    m_pmap << "ATLAS-P2-ITK-22-02-00\n" << m_planes[reg].size() << " logical_s1\n" << m_planes2[reg].size() << " logical_s2\n";
    m_pmap << m_pbmax+1 << " pixel barrel \n" << m_pemax[0]+1 << " pixel endcap+ \n" << m_pemax[1]+1 << " pixel endcap- \n"; 
    m_pmap << m_sbmax+1 << " SCT barrel \n" << m_semax[0]+1 << " SCT endcap+\n" << m_semax[1]+1 << " SCT endcap-\n";
    m_pmap << "! silicon endCap physDisk physLayer ['stereo' stripSide <strip only>] 'plane1' logiLayer1 'plane2' logiLayer2\n";
    m_pmap << "\nregion " << reg << "\n"; 

    int p1,p2;
    for (int lyr = 0; lyr <= m_pbmax; lyr++) { // Pixel Barrel
        p1 = findPlane(m_planes[reg], "pb" + std::to_string(lyr));
        p2 = findPlane(m_planes2[reg], "pb" + std::to_string(lyr));
        m_pmap << "pixel 0    -1    " << lyr << " plane1 " << p1 << "    plane2 " << p2 << "\n";
    }
    for (int lyr = 0; lyr <= m_pemax[0]; lyr++) { // Pixel Postive Endap
        p1 = findPlane(m_planes[reg], "pe" + std::to_string(lyr) + "+");
        p2 = findPlane(m_planes2[reg], "pe" + std::to_string(lyr) + "+");
        m_pmap << "pixel 1    " << lyr << "    " << lyr << " plane1 " << p1 << "    plane2 " << p2 << "\n";
    }
    for (int lyr = 0; lyr <= m_pemax[1]; lyr++) { // Pixel Negative Endcap
        p1 = findPlane(m_planes[reg], "pe" + std::to_string(lyr) + "-");
        p2 = findPlane(m_planes2[reg], "pe" + std::to_string(lyr) + "-");
        m_pmap << "pixel 2    " << lyr << "    " << lyr << " plane1 " << p1 << "    plane2 " << p2 << "\n";
    }
    for (int lyr = 0; lyr <= m_sbmax; lyr++) { // Strip Barrel
        p1 = findPlane(m_planes[reg], "sb" + std::to_string(lyr));
        p2 = findPlane(m_planes2[reg], "sb" + std::to_string(lyr));
        m_pmap << "SCT 0    -1    " << lyr << " stereo " <<  lyr % 2 << " plane1 "  << p1 << "    plane2 " << p2 << "\n";
    }
    for (int lyr = 0; lyr <= m_semax[0]; lyr++) { // Strip Positive Endcap
        p1 = findPlane(m_planes[reg], "se" + std::to_string(lyr) + "+");
        p2 = findPlane(m_planes2[reg], "se" + std::to_string(lyr) + "+");
        m_pmap << "SCT 1    "  << lyr/2 << "    " << lyr << " stereo " <<  lyr % 2 << " plane1 " << p1 << "    plane2 " << p2 << "\n";
    }
    for (int lyr = 0; lyr <= m_semax[1]; lyr++) { // Strip Negative Endcap
        p1 = findPlane(m_planes[reg], "se" + std::to_string(lyr) + "-");
        p2 = findPlane(m_planes2[reg], "se" + std::to_string(lyr) + "-");
        m_pmap << "SCT 2    "  << lyr/2 << "    " << lyr << " stereo " <<  lyr % 2 << " plane1 " << p1 << "    plane2 " << p2 << "\n";
    }
    
    // Region Map
    std::string rmap_path = m_outFileName.value() + "region" + std::to_string(m_region) + ".rmap";
    ATH_MSG_INFO("Creating rmap: " << rmap_path);
    m_rmap.open(rmap_path, std::ofstream::out);
    m_rmap << "towers 1 phi 16\n\n0\n";

    m_rmap << makeRmapLines(pbHits, SiliconTech::pixel, DetectorZone::barrel, m_pbmax);
    m_rmap << makeRmapLines(peHits, SiliconTech::pixel, DetectorZone::posEndcap, m_pemax[0]); 
    m_rmap << makeRmapLines(peHits, SiliconTech::pixel, DetectorZone::negEndcap, m_pemax[1]); 
    
    m_rmap << makeRmapLines(sbHits, SiliconTech::strip, DetectorZone::barrel, m_sbmax); 
    m_rmap << makeRmapLines(seHits, SiliconTech::strip, DetectorZone::posEndcap, m_semax[0]);
    m_rmap << makeRmapLines(seHits, SiliconTech::strip, DetectorZone::negEndcap, m_semax[1]);
    
    m_pmap.close();
    m_rmap.close();
    

    return StatusCode::SUCCESS;
}

StatusCode HTTMapMakerAlg::writeSubrmap(std::vector<HTTHit> const & allHits)
{
    /*   ----------  Create z-slices ---------- */

    if (m_nSlices == -1) m_nSlices = m_key_etamods.size(); //default is full granularity slicing
    float etasPerSlice = float(m_key_etamods.size())/m_nSlices;
    std::vector<std::vector<int>> key_modules_for_slices; // indexed by slice, holds module eta values

   // easier to use vector than set, convert m_key_etamods into key_etas
    std::vector<int> key_etas;  
    std::vector<int> key_etas2; // used if 2D slicing
    key_etas.insert(key_etas.end(), m_key_etamods.begin(), m_key_etamods.end());

    for (unsigned i = 0; i < key_etas.size(); i++)
    {
        if (i >= (key_modules_for_slices.size() * etasPerSlice)) key_modules_for_slices.push_back(std::vector<int>());
        key_modules_for_slices.back().push_back(key_etas[i]);
    }

    std::map<int, int> keymod2slice;
    for (unsigned s = 0; s < key_modules_for_slices.size(); s++)
        for (unsigned e = 0; e < key_modules_for_slices[s].size(); e++)
            keymod2slice[key_modules_for_slices[s][e]] = s;

    std::string key = m_keystring.value();
    key.erase(std::remove(key.begin(), key.end(), ','), key.end());
    key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
    std::string key2 = m_keystring2.value();
    key2.erase(std::remove(key2.begin(), key2.end(), ','), key2.end());
    key2.erase(std::remove(key2.begin(), key2.end(), ' '), key2.end());

    ATH_MSG_DEBUG("Doing z-slicing");
    if (m_key_etamods.size() == 0) ATH_MSG_ERROR("Found 0 slices using the keystring: '" << m_keystring << "'");
    ATH_MSG_DEBUG("Nslices = " << std::to_string(m_nSlices) << ":");
    
    std::stringstream eta_slices;
    for (unsigned s = 0; s < key_modules_for_slices.size(); s++){   
        for (unsigned e = 0; e < key_modules_for_slices[s].size(); e++){
            eta_slices << key_modules_for_slices[s][e] << " ";
        }
        eta_slices << ", ";
    }
    ATH_MSG_DEBUG(eta_slices.str());

    // 2D key layer slicing
    if (m_key2) 
    { // make new slices from combinations of keylayer1 slices and keylayer2 slices

        /*------------- setup keylayer2 keymodule to slice map  -------------- */
        std::vector<std::vector<int>> key_modules_for_slices2; 
        float etasPerSlice2 = float(m_key_etamods2.size())/m_nSlices;
        key_etas2.insert(key_etas2.end(), m_key_etamods2.begin(), m_key_etamods2.end());
        for (unsigned i = 0; i < key_etas2.size(); i++)
        {
        if (i >= (key_modules_for_slices2.size() * etasPerSlice2)) key_modules_for_slices2.push_back(std::vector<int>());
        key_modules_for_slices2.back().push_back(key_etas2[i]);
        }

        std::map<int, int> keymod2slice2;
        for (unsigned s = 0; s < key_modules_for_slices2.size(); s++)
            for (unsigned e = 0; e < key_modules_for_slices2[s].size(); e++)
                keymod2slice2[key_modules_for_slices2[s][e]] = s;
        /*----------------------------------------------------------------- */

        int new_nSlice = m_nSlices;
        for (int s1 = 0; s1 < m_nSlices; s1++){ // loop over keylayer1's slices
            for (int s2 = 0; s2 < m_nSlices; s2++){ // loop over keylayer2's slices
                
                for (auto& pair: m_track2modules) // track loop
                {
                    if (m_usedTracks.find(pair.first) != m_usedTracks.end()) continue; // skip if already assigned a slice to this track
                    bool key1 = false;
                    bool key2 = false;
                    for (auto& m: pair.second) // module loop
                    {
                        if (isOnKeyLayer(1,m.det,m.bec,m.lyr)) 
                            if (keymod2slice[m.eta] == s1) key1 = true;
                        
                        if (isOnKeyLayer(2,m.det,m.bec,m.lyr)) 
                            if (keymod2slice2[m.eta] == s2) key2 = true;
                    }
                
                    if (key1 && key2)
                    {
                        int newSlice = m_nSlices*s1 + s2;
                        m_track2slice[pair.first] = newSlice;
                        m_usedTracks.insert(pair.first);
                        if (newSlice + 1 > new_nSlice) new_nSlice = newSlice + 1; // find max slice, to set new total number of slices
                    }
                }
            }
        }
        m_nSlices = new_nSlice;
        ATH_MSG_DEBUG("These slices were further divided based on the key layer 2: '" << key2 << "'. Now nSlices = " << m_nSlices);
        ATH_MSG_DEBUG("Using " << m_usedTracks.size() << " tracks out of " << m_maxEvents << ". The rest were missing a hit in one or both of the key layers");
    }
    
    
    // 1D key layer slicing
    else 
    {
        for (auto& hit: allHits) // Fill the track to slice map
        {
            if (m_usedTracks.find(hit.getEventIndex()) != m_usedTracks.end()) continue; // skip if already done a hit from this track
            if (isOnKeyLayer(1,hit.getDetType(),hit.getDetectorZone(), hit.getPhysLayer()))
            { // if hit is in key layer, add it's barcode to the map
                int s = keymod2slice[hit.getHTTEtaModule()];
                m_track2slice[hit.getEventIndex()] = s;
                m_usedTracks.insert(hit.getEventIndex());
            }
        }
        ATH_MSG_DEBUG("Using " << m_usedTracks.size() << " tracks out of " << m_maxEvents << ". The rest missed the key layer");
    }

    std::stringstream trim;
    trim << std::fixed << std::setprecision(3) << m_trim;
    std::string str_trim = trim.str();
    int dot = str_trim.find_last_of(".");
    str_trim.replace(dot,1,"p");
    
    std::string subrmap_path = m_outFileName.value() + "region" + std::to_string(m_region) + "_" + key + "_" + key2 + "_trim" + str_trim  + "_NSlices-" + std::to_string(m_nSlices) + ".rmap";

    ATH_MSG_DEBUG("Creating subrmap: " << subrmap_path);
    m_subrmap.open(subrmap_path, std::ofstream::out);
    m_subrmap << "towers " << m_nSlices << " phi 16\n\n";

    // Resize numTracks vector to be equal to the number of slices 
    for (auto& pair: m_track2modules) 
        for (auto& m: pair.second)
            m.numTracks.resize(m_nSlices,0);
        
    

    // Count tracks per slice
    std::vector<std::vector<int>> slicedTracks (m_nSlices); // vector of tracks, indexed by slice
    for (auto trk: m_usedTracks) {
        int s = m_track2slice[trk];
        slicedTracks[s].push_back(trk);
        std::vector<Module> mods = m_track2modules[trk];
        for (auto mod: mods)
            for (auto& pair: m_track2modules) 
                for (auto& m: pair.second)
                    if (m == mod) 
                        m.numTracks[s]++;
    }

    drawSlices(allHits);

    // Now do trimming and Fill slice2module map
    int trimmed = 0;
    std::vector<std::vector<Module>::iterator> toTrim;
    for (int s = 0; s < m_nSlices; s++) {
        for (auto trk : slicedTracks[s]) {
            toTrim.clear();
            for (std::vector<Module>::iterator m = m_track2modules[trk].begin(); m != m_track2modules[trk].end(); m++)
                if (100 * ( float((*m).numTracks[s]) / float(slicedTracks[s].size()) ) < m_trim)
                    toTrim.push_back(m);
            trimmed += toTrim.size();

            for(auto m : toTrim)
                m_track2modules[trk].erase(m);

            int s = m_track2slice[trk];
            m_slice2modules[s].insert( m_slice2modules[s].end(), m_track2modules[trk].begin(), m_track2modules[trk].end() ); // add all modules from track to slice s
        }
    }
    ATH_MSG_DEBUG("Trimmed off " << trimmed << " modules that were hit by less than " << m_trim << "% of tracks");

    /*   ----------  Print z-slice map ---------- */

    for (int s = 0; s < m_nSlices; s++) 
    {
        m_subrmap << s << "\n";
        m_subrmap << makeSubrmapLines(m_slice2modules[s], SiliconTech::pixel, DetectorZone::barrel, m_pbmax);
        m_subrmap << makeSubrmapLines(m_slice2modules[s], SiliconTech::pixel, DetectorZone::posEndcap, m_pemax[0]); 
        m_subrmap << makeSubrmapLines(m_slice2modules[s], SiliconTech::pixel, DetectorZone::negEndcap, m_pemax[1]);
    
        m_subrmap << makeSubrmapLines(m_slice2modules[s], SiliconTech::strip, DetectorZone::barrel, m_sbmax);
        m_subrmap << makeSubrmapLines(m_slice2modules[s], SiliconTech::strip, DetectorZone::posEndcap, m_semax[0]); 
        m_subrmap << makeSubrmapLines(m_slice2modules[s], SiliconTech::strip, DetectorZone::negEndcap, m_semax[1]); 
        m_subrmap << "\n\n";
    }

    m_subrmap.close();
    return StatusCode::SUCCESS;
}

StatusCode HTTMapMakerAlg::writeEtaPatterns()
{
    std::stringstream trim;
    trim << std::fixed << std::setprecision(3) << m_trim;
    std::string str_trim = trim.str();
    int dot = str_trim.find_last_of(".");
    str_trim.replace(dot,1,"p");

    std::string slicingType = "";
    if (m_key2) slicingType = "2D";
    std::string etapat_path = m_outFileName.value() + "region" + std::to_string(m_region) + "_trim" + str_trim + "_NSlices-" + std::to_string(m_nSlices) + slicingType + "_etapatterns.patt";

    ATH_MSG_DEBUG("Creating eta patterns file: " << etapat_path);
    m_etapat.open(etapat_path, std::ofstream::out);

    // assign logical layer to each module
    for (auto& pair: m_track2modules) { 
        for (auto& m: pair.second)
        {
            if (m.det == SiliconTech::pixel && m.bec == DetectorZone::barrel) m.plane = findPlane(m_planes[m_region], "pb" + std::to_string(m.lyr));
            if (m.det == SiliconTech::pixel && m.bec == DetectorZone::posEndcap) m.plane = findPlane(m_planes[m_region], "pe" + std::to_string(m.lyr) + "+");
            if (m.det == SiliconTech::pixel && m.bec == DetectorZone::negEndcap) m.plane = findPlane(m_planes[m_region], "pe" + std::to_string(m.lyr) + "-");

            if (m.det == SiliconTech::strip && m.bec == DetectorZone::barrel) m.plane = findPlane(m_planes[m_region], "sb" + std::to_string(m.lyr));
            if (m.det == SiliconTech::strip && m.bec == DetectorZone::posEndcap) m.plane = findPlane(m_planes[m_region], "se" + std::to_string(m.lyr) + "+");
            if (m.det == SiliconTech::strip && m.bec == DetectorZone::negEndcap) m.plane = findPlane(m_planes[m_region], "se" + std::to_string(m.lyr) + "-");
        }
    }

    for(auto trk: m_usedTracks)
    {
        std::stringstream track_etapatts;
        unsigned planesDone = 0;
        for (unsigned p = 0; p < m_planes[m_region].size(); p++)
        {
            for (std::vector<Module>::iterator m = m_track2modules[trk].begin(); m != m_track2modules[trk].end(); m++)
                if ((*m).plane == static_cast<int>(p))
                {
                    track_etapatts << std::to_string(static_cast<int>((*m).det)) << "\t" << std::to_string(static_cast<int>((*m).bec)) << "\t" << std::to_string((*m).eta) << "\t\t";
                    planesDone++;
                    break;
                }
        }
        if (planesDone == m_planes[m_region].size())
            m_etapat << track_etapatts.str() << "\n";
        
    }
    m_etapat.close();
    return StatusCode::SUCCESS;
}

StatusCode HTTMapMakerAlg::writeRadiiFile(std::vector<HTTHit> const & allHits)
{
    // calculate mean radii
    m_radii.resize(m_nSlices, std::vector<std::vector<float>>(m_planes[m_region].size(),std::vector<float>(0)));
    for (auto hit: allHits)
    {
        SiliconTech det = hit.getDetType();
        DetectorZone bec = hit.getDetectorZone();
        int lyr = hit.getPhysLayer();
        int slice = m_track2slice[hit.getEventIndex()];
        int plane = -1;
        if (det == SiliconTech::pixel && bec == DetectorZone::barrel) plane = findPlane(m_planes[m_region], "pb" + std::to_string(lyr));
        if (det == SiliconTech::pixel && bec == DetectorZone::posEndcap) plane = findPlane(m_planes[m_region], "pe" + std::to_string(lyr) + "+");
        if (det == SiliconTech::pixel && bec == DetectorZone::negEndcap) plane = findPlane(m_planes[m_region], "pe" + std::to_string(lyr) + "-");
        if (det == SiliconTech::strip && bec == DetectorZone::barrel) plane = findPlane(m_planes[m_region], "sb" + std::to_string(lyr));
        if (det == SiliconTech::strip && bec == DetectorZone::posEndcap) plane = findPlane(m_planes[m_region], "se" + std::to_string(lyr) + "+");
        if (det == SiliconTech::strip && bec == DetectorZone::negEndcap) plane = findPlane(m_planes[m_region], "se" + std::to_string(lyr) + "-");

        if (plane != -1) m_radii[slice][plane].push_back(hit.getR());
    }

    // print file
    std::string radii_path = m_outFileName.value() + "region" + std::to_string(m_region) + "_NSlices-" + std::to_string(m_nSlices) + "_MeanRadii.txt";
    ATH_MSG_DEBUG("Creating radii file: " << radii_path);
    m_radfile.open(radii_path, std::ofstream::out);
    for (int s = 0; s < m_nSlices; s++){
        m_radfile << std::to_string(s) << " ";
        for (unsigned p = 0; p < m_planes[m_region].size(); p++){
            if (m_radii[s][p].size() != 0){
                float avg = std::accumulate(m_radii[s][p].begin(), m_radii[s][p].end(), 0)/float(m_radii[s][p].size());
                m_radfile << std::setprecision(3) << std::fixed << avg << " ";
            } else {
                int avg = -1;
                m_radfile << avg << " ";
            }
        }
        m_radfile << "\n";
    }
    m_radfile.close();

    return StatusCode::SUCCESS;
}

StatusCode HTTMapMakerAlg::writeMedianZFile(std::vector<HTTHit> const & allHits)
{
    // calculate median z
    m_z.resize(m_nSlices, std::vector<std::vector<float>>(m_planes[m_region].size(),std::vector<float>(0)));
    for (auto hit: allHits)
    {
        SiliconTech det = hit.getDetType();
        DetectorZone bec = hit.getDetectorZone();
        int lyr = hit.getPhysLayer();
        int slice = m_track2slice[hit.getEventIndex()];
        int plane = -1;
        if (det == SiliconTech::pixel && bec == DetectorZone::barrel) plane = findPlane(m_planes[m_region], "pb" + std::to_string(lyr));
        if (det == SiliconTech::pixel && bec == DetectorZone::posEndcap) plane = findPlane(m_planes[m_region], "pe" + std::to_string(lyr) + "+");
        if (det == SiliconTech::pixel && bec == DetectorZone::negEndcap) plane = findPlane(m_planes[m_region], "pe" + std::to_string(lyr) + "-");
        if (det == SiliconTech::strip && bec == DetectorZone::barrel) plane = findPlane(m_planes[m_region], "sb" + std::to_string(lyr));
        if (det == SiliconTech::strip && bec == DetectorZone::posEndcap) plane = findPlane(m_planes[m_region], "se" + std::to_string(lyr) + "+");
        if (det == SiliconTech::strip && bec == DetectorZone::negEndcap) plane = findPlane(m_planes[m_region], "se" + std::to_string(lyr) + "-");

        if (plane != -1) m_z[slice][plane].push_back(hit.getZ());
    }

    // print file
    std::string zed_path = m_outFileName.value() + "region" + std::to_string(m_region) + "_NSlices-" + std::to_string(m_nSlices) + "_MedianZ.txt";
    ATH_MSG_DEBUG("Creating median z file: " << zed_path);
    m_zedfile.open(zed_path, std::ofstream::out);
    for (int s = 0; s < m_nSlices; s++){
        m_zedfile << std::to_string(s) << " ";
        for (unsigned p = 0; p < m_planes[m_region].size(); p++){
            if (m_z[s][p].size() != 0){
                float minZ = *std::min_element(m_z[s][p].begin(), m_z[s][p].end());
                float maxZ = *std::max_element(m_z[s][p].begin(), m_z[s][p].end()); 
                float median = (minZ + maxZ)/2;
                m_zedfile << std::setprecision(3) << std::fixed << median << " ";
            } else {
                int median = -1;
                m_zedfile << median << " ";
            }
        }
        m_zedfile << "\n";
    }
    m_zedfile.close();

    return StatusCode::SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////
// Finalize
StatusCode HTTMapMakerAlg::finalize()
{
    // Write the output
    ATH_CHECK(writePmapAndRmap(m_pbHits, m_peHits, m_sbHits, m_seHits, m_region));
    ATH_CHECK(writeSubrmap(m_allHits));
    ATH_CHECK(writeEtaPatterns());
    ATH_CHECK(writeRadiiFile(m_allHits));
    ATH_CHECK(writeMedianZFile(m_allHits));
    return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// Helpers

void HTTMapMakerAlg::drawSlices(std::vector<HTTHit> const & allHits)
{
    m_monitorFile.cd();

    std::vector<TH2F*> h_slicemap;
    char *hname = new char[50];

    for (int i = 0; i < m_nSlices; i++)
    {
        sprintf(hname,"rz_slice%d",i);     
        TH2F *h = new TH2F(hname,hname,1500,-400,1100,1100,0,1100);
        h_slicemap.push_back(h);
    } 

    for (auto hit: allHits)
    {
        if (m_usedTracks.find(hit.getEventIndex()) == m_usedTracks.end()) continue; // skip if we don't use this track
        int s = m_track2slice[hit.getEventIndex()];
        h_slicemap[s]->Fill(hit.getZ(),hit.getR());
    }

    for (int i = 0; i < m_nSlices; i++)
        h_slicemap[i]->Write();
    
    delete [] hname;
}

bool HTTMapMakerAlg::isOnKeyLayer(int keynum, SiliconTech t_det, DetectorZone t_bec, int lyr)
{
    int det = static_cast<int>(t_det);
    int bec = static_cast<int>(t_bec);
    if (keynum == 1) {
        if (m_keylayer["det"].find(det) != m_keylayer["det"].end() && m_keylayer["bec"].find(bec) != m_keylayer["bec"].end() && m_keylayer["lyr"].find(lyr) != m_keylayer["lyr"].end()) 
            return true;
    }
    else if (keynum == 2) {
        if (m_keylayer2["det"].find(det) != m_keylayer2["det"].end() && m_keylayer2["bec"].find(bec) != m_keylayer2["bec"].end() && m_keylayer2["lyr"].find(lyr) != m_keylayer2["lyr"].end()) 
            return true;
    }
    return false;
}

int HTTMapMakerAlg::findPlane(const std::vector<std::vector<std::string>>& planes, const std::string& test) // find what plane a layer is assigned to. 
{
    int pcounter = 0;
    for (auto& plane : planes) {
        for (auto& layer : plane) {
            if (test == layer) return pcounter;
        }
        pcounter ++;
    }   
    return -1;
}

std::string HTTMapMakerAlg::makeRmapLines(std::vector<HTTHit> const & hits, SiliconTech det, DetectorZone bec, int max)
{
    std::stringstream rmap_line;
    std::set<int> etas, phis;    

    for(int lyr = 0; lyr <= max; lyr++)  
    {
        etas.clear();
        phis.clear();
        for (auto hit: hits)
        {
            if(static_cast<int>(hit.getPhysLayer()) == lyr && hit.getDetectorZone() == bec)  // cast from uint to int just to remove Wsign-compare warnings
            {
                etas.insert(hit.getHTTEtaModule());
                phis.insert(hit.getPhiModule());
            }
        }
        if (etas.size() != 0) rmap_line << static_cast<int>(det) << " " << static_cast<int>(bec) << " " << lyr << " " << *phis.begin() << " " << *phis.rbegin() << " " << phis.size() << " " << *etas.begin() << " " << *etas.rbegin() << " " << etas.size() << "\n";
        else rmap_line << static_cast<int>(det) << " " << static_cast<int>(bec) << " " << lyr << " 0 0 0 0 0 0\n";

    }

    return rmap_line.str();
}

void HTTMapMakerAlg::parseKeyString()
{
    // Parse keystring and define the Key Layer
    std::string delimiter = ",";
    std::string s = m_keystring.value();
    std::string det, bec, lyr; 
    std::map <std::string, std::vector<std::string>> abrevs = { {"pb",{"pixel","barrel"}}, {"pe",{"pixel","endcap"}}, {"sb",{"strip","barrel"}}, {"se",{"strip","endcap"}} };
    if( s.find(delimiter) != std::string::npos) // keylayer format is 'strip,barrel,4'
    {
        try {
            std::string det = s.substr(0, s.find(delimiter));
            s.erase(0, s.find(delimiter) + delimiter.length());
            std::string bec = s.substr(0, s.find(delimiter));
            s.erase(0, s.find(delimiter) + delimiter.length());
            std::string lyr = s.substr(0, s.find(delimiter));
            s.erase(0, s.find(delimiter) + delimiter.length());
            m_keylayer["det"].insert(static_cast<int>(m_det2tech[det]));
            m_keylayer["bec"].insert(static_cast<int>(m_bec2zone[bec]));
            m_keylayer["lyr"].insert(std::stoi(lyr));
        } catch (...){
            ATH_MSG_ERROR("Invalid KeyString: '" << m_keystring << "'." << "Accepted formats are 'strip,posEndcap,2', 'pixel,barrel,3', or 'plane 0'");
        }
    }
    else // keylayer format is 'plane 0'
    {
        std::string delimiter = " ";
        try {
            s.erase(0, s.find(delimiter) + delimiter.length());
            std::string plane = s.substr(0, s.find(delimiter));
            std::vector<std::string> s = m_planes[m_region][std::stoi(plane)];
            for (unsigned i = 0; i < s.size(); i++){
                std::string reg = s[i].substr(0, 2);
                std::vector<std::string> zone = abrevs[reg];
                if (s[i].back() == '+') zone[1] = "posEndcap";
                if (s[i].back() == '-') zone[1] = "negEndcap";
                s[i].erase(std::remove(s[i].begin(), s[i].end(), '+'), s[i].end());
                s[i].erase(std::remove(s[i].begin(), s[i].end(), '-'), s[i].end());
                std::string lyr = s[i].substr(2);
                m_keylayer["det"].insert(static_cast<int>(m_det2tech[zone[0]]));
                m_keylayer["bec"].insert(static_cast<int>(m_bec2zone[zone[1]]));
                m_keylayer["lyr"].insert(std::stoi(lyr));
            }
        } catch (...){
            ATH_MSG_ERROR("Invalid KeyString: '" << m_keystring << "'." << "Accepted formats are 'strip,posEndcap,2', 'pixel,barrel,3', or 'plane 0'");
        }
    }

    // if 2D slicing
    if (m_keystring2.value() != "")
    {
        m_key2 = true;
        std::string s = m_keystring2.value();
        if( s.find(delimiter) != std::string::npos) // keylayer format is 'strip,barrel,8'
        {
            try {
                std::string det = s.substr(0, s.find(delimiter));
                s.erase(0, s.find(delimiter) + delimiter.length());
                std::string bec = s.substr(0, s.find(delimiter));
                s.erase(0, s.find(delimiter) + delimiter.length());
                std::string lyr = s.substr(0, s.find(delimiter));
                s.erase(0, s.find(delimiter) + delimiter.length());
                m_keylayer2["det"].insert(static_cast<int>(m_det2tech[det]));
                m_keylayer2["bec"].insert(static_cast<int>(m_bec2zone[bec]));
                m_keylayer2["lyr"].insert(std::stoi(lyr));
            } catch (...){
                ATH_MSG_ERROR("Invalid KeyString2: '" << m_keystring2 << "'." << "Accepted formats are 'strip,posEndcap,2', 'pixel,barrel,3', or 'plane 0'");
            }
        } 
        else // keylayer format is 'plane 0'
        {
            std::string delimiter = " ";
            try {
                s.erase(0, s.find(delimiter) + delimiter.length());
                std::string plane = s.substr(0, s.find(delimiter));
                std::vector<std::string> s = m_planes[m_region][std::stoi(plane)];
                for (unsigned i = 0; i < s.size(); i++){
                    std::string reg = s[i].substr(0, 2);
                    std::vector<std::string> zone = abrevs[reg];
                    if (s[i].back() == '+') zone[1] = "posEndcap";
                    if (s[i].back() == '-') zone[1] = "negEndcap";
                    s[i].erase(std::remove(s[i].begin(), s[i].end(), '+'), s[i].end());
                    s[i].erase(std::remove(s[i].begin(), s[i].end(), '-'), s[i].end());
                    std::string lyr = s[i].substr(2);
                    m_keylayer2["det"].insert(static_cast<int>(m_det2tech[zone[0]]));
                    m_keylayer2["bec"].insert(static_cast<int>(m_bec2zone[zone[1]]));
                    m_keylayer2["lyr"].insert(std::stoi(lyr));
                }
            } catch (...){
                ATH_MSG_ERROR("Invalid KeyString2: '" << m_keystring2 << "'." << "Accepted formats are 'strip,posEndcap,2', 'pixel,barrel,3', or 'plane 0'");
            }
        }
    }
}

std::string HTTMapMakerAlg::makeSubrmapLines(std::vector<Module> const & allmods, SiliconTech det, DetectorZone bec, int max)
{
    std::stringstream subrmap_line;
    std::set<int> etas, phis;  

    std::vector<Module> mods;
    for (auto mod: allmods) // just want modules in pb/pe/sb/se etc, not all at once
        if (mod.det == det && mod.bec == bec) mods.push_back(mod);
  
    for(int lyr = 0; lyr <= max; lyr++)  
    {
        etas.clear();
        phis.clear();
        for (auto mod: mods)
        {
            if(mod.lyr == lyr)
            {
                etas.insert(mod.eta);
                phis.insert(mod.phi);
            }
        }
        if (etas.size() != 0) subrmap_line << static_cast<int>(det) << " " << static_cast<int>(bec) << " " << lyr << " " << *phis.begin() << " " << *phis.rbegin() << " " << phis.size() << " " << *etas.begin() << " " << *etas.rbegin() << " " << etas.size() << "\n";
        else subrmap_line << static_cast<int>(det) << " " << static_cast<int>(bec) << " " << lyr << " 0 0 0 0 0 0\n";

    }

    return subrmap_line.str();
}
