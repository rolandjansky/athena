#include "channelMappings/EfexCellMapping.h"
/*!
 *  \class EfexCellMapping
 *  New class for Efex Cell Mappings, constructs either
 *  from shared CSVTables instance  */

EfexCellMapping::EfexCellMapping(int crate, int efexnumber, 
        int fpga, int mgtchannelout , int dataword)
{
    auto genintint = [] (int col,int intstr)
    {return std::make_pair(col , std::to_string(intstr) );};
    using searchpairs = std::vector<std::pair<int,std::string>>;

    //Retrieve input quad inMGT pair
    auto tab6search  = searchpairs();
    tab6search.push_back(genintint(0,fpga));
    tab6search.push_back(genintint(3,mgtchannelout));
    auto tab6line = m_tables.GetTable(6)->FindLine(tab6search);
    int quad = std::stoi( tab6line->at(1) );
    int mgtin = std::stoi( tab6line->at(2) );
    this->init(crate,efexnumber, fpga, quad, mgtin, dataword);
}

EfexCellMapping::EfexCellMapping( roiType, int crate, int efexnumber,
        int fpga, int eta, int phi )
{
   if ( (crate<0) || (crate>1) || (efexnumber<0) || (efexnumber>11) )
      return;
   if ( (eta<0) || (eta>5) || (phi<0) || (phi>7) )
      return;
      
   int etaIndex = (efexnumber%3)*16 + fpga*4 + eta - 25;
   int phiIndex = crate*32 +(efexnumber/3)*8 + phi + 2;
   phiIndex %= 64;
   double etaWidth = 0.1;
   double phiWidth = M_PI/32.0;
   double etaCoord = (etaIndex+0.5)*etaWidth;
   double phiCoord = (phiIndex+0.5)*phiWidth;
   
   m_region = L1CaloDetectorRegion( L1CaloDetectorRegion::CPM,
                                    L1CaloDetectorRegion::Both,
                                    true, etaIndex, phiIndex,
                                    etaWidth, phiWidth,
                                    etaCoord, phiCoord );
}

EfexCellMapping::EfexCellMapping( roiType, int crate, int efexnumber,
        int fpga, int eta, int phi, int seed )
{
   if ( (crate<0) || (crate>1) || (efexnumber<0) || (efexnumber>11) )
      return;
   if ( (eta<0) || (eta>5) || (phi<0) || (phi>7) )
      return;
   if ( (seed<0) || (seed>3) )
      return;

   int etaIndex = (efexnumber%3)*64 + fpga*16 + eta*4 + seed - 100;
   int phiIndex = crate*32 +(efexnumber/3)*8 + phi + 2;
   phiIndex %= 64;
   double etaWidth = 0.025;
   double phiWidth = M_PI/32.0;
   double etaCoord = (etaIndex+0.5)*etaWidth;
   double phiCoord = (phiIndex+0.5)*phiWidth;

   m_region = L1CaloDetectorRegion( L1CaloDetectorRegion::CPM,
                                    L1CaloDetectorRegion::Both,
                                    true, etaIndex, phiIndex,
                                    etaWidth, phiWidth,
                                    etaCoord, phiCoord );
}


L1CaloDetectorRegion EfexCellMapping::getDetectorRegion() const
{
    return m_region;   
}

EfexHardwareInfo EfexCellMapping::getHardwareInfo() const
{
    return m_hwinfo;
}
std::string EfexCellMapping::getLatomeID() const
{
    return m_latomeid;
}
void EfexCellMapping::init(int crate, int efexnumber, 
        int fpga, int quad, int mgtchannel , int dataword){
    //Ensure m_region always exists even if invalid
    m_region = L1CaloDetectorRegion();
    m_hwinfo = EfexHardwareInfo();
    //Run common constructor methods
    std::string efexcords = this->findModuleCords(crate, efexnumber);
    //       Test data read in
    /*
       for(int tableidx=1 ; tableidx<6; tableidx++ ){
       std::cout << "Printing Table:" << std::to_string(tableidx)<< std::endl;
       ((m_tables)->GetTable(tableidx))->PrintTable(true);       
       }
       */
    //        (m_tables)->GetTable(4)->PrintTable();
    //Basic Lamdas to gen search pairs
    auto genintstr = [] (int col, std::string str )
    {return std::make_pair(col , str );};
    auto genintint = [] (int col,int intstr)
    {return std::make_pair(col , std::to_string(intstr) );};
    using searchpairs = std::vector<std::pair<int,std::string>>;

    //Complete CSV search

    //METHOD #1 to retrieve eta-phi-layer   
    auto tab4search  = searchpairs();
    tab4search.push_back(genintstr(0,efexcords));
    tab4search.push_back(genintint(1,fpga));
    tab4search.push_back(genintint(2,quad));
    tab4search.push_back(genintint(3,mgtchannel));
    auto tab4line = m_tables.GetTable(4)->FindLine(tab4search);
    if ( tab4line->size() == 0 )
      return;
//    std::cout << "Table4 Search Found:" << std::endl;
//    for(auto entry : *tab4line ){std::cout << entry << " ,";}
//    std::cout << std::endl;

    //Create a valid minipod str
    std::string mpod = tab4line->at(4);
    auto tab3search  = searchpairs();
    tab3search.push_back(genintstr(0,efexcords));
    tab3search.push_back(genintstr(1,mpod));
    auto tab3line = m_tables.GetTable(3)->FindLine(tab3search);
    if ( tab3line->size() == 0 )
      return;
//    std::cout << "Table3 Search Found:" << std::endl;
//    for(auto entry : *tab3line ){std::cout << entry << " ,";}
//    std::cout << std::endl;
    m_inputconnector = std::stoi(tab3line->at(2));
    m_fibrenumber = std::stoi(tab3line->at(3));

    //Generate a valid HW info object
    m_hwinfo = EfexHardwareInfo(efexcords,m_fibrenumber,
        m_inputconnector,mpod);

    auto tab2search  = searchpairs();
    tab2search.push_back(genintstr(0,efexcords));
    tab2search.push_back(genintint(1,m_inputconnector));
    tab2search.push_back(genintint(2,m_fibrenumber));
    tab2search.push_back(genintint(3,dataword ));
    auto tab2line = m_tables.GetTable(2)->FindLine(tab2search);
    if ( tab2line->size() == 0 )
      return;

    int globaleta = std::stoi( tab2line->at(4) );
    int globalphi = std::stoi( tab2line->at(5) );
    //std::cout << "My layer str is " << tab2line->at(6) << std::endl;
    int layer = std::stoi( tab2line->at(6),nullptr ,16 );
    m_latomeid = tab2line->at(8);
    //std::cout << "My layer int is " << tab2line->at(6) << std::endl;
    //Once Search complete, generate detector region info

    //Predeclare enum
    auto efexlabel = L1CaloDetectorRegion::EFEX;
    //    enum layertype: int;
    double supercelletawidth(0);
    double supercellphiwidth(0.09817477);
    int supercelletaoffset(0);
    if(layer == 0){
        supercelletawidth = 0.1;
    }
    else if(1 <= layer  && layer <= 4){
        supercelletawidth = 0.025;
        supercelletaoffset = (layer - 1 )%4;
    }
    else if(5 <= layer  && layer <= 8){
        supercelletawidth = 0.025;
        supercelletaoffset = (layer - 1 )%4;
    }
    else if(layer == 9 || layer == 10){
        supercelletawidth = 0.1;
    }
    double toweretalowside = (double)globaleta/10;
    double towerphilowside = (double)globalphi*0.09817477;
    double supercelletacoord = toweretalowside + 0.025*(double)supercelletaoffset +
        0.5*supercelletawidth ; 
    double supercellphicoord = towerphilowside + 0.5*supercellphiwidth;
    if(layer == 0){
        m_region = L1CaloDetectorRegion( efexlabel,
                L1CaloDetectorRegion::Presampler, true, globaleta, globalphi,
                supercelletawidth, supercellphiwidth , supercelletacoord, supercellphicoord);
    }
    else if(1 <= layer  && layer <= 4){
        m_region = L1CaloDetectorRegion( efexlabel,
                L1CaloDetectorRegion::Front, true, globaleta, globalphi, 
                supercelletawidth, supercellphiwidth , supercelletacoord, supercellphicoord);
    }
    else if(5 <= layer  && layer <= 8){
        m_region = L1CaloDetectorRegion( efexlabel,
                L1CaloDetectorRegion::Middle, true, globaleta, globalphi,  
                supercelletawidth, supercellphiwidth , supercelletacoord, supercellphicoord);
    }
    else if(layer == 9){
        m_region = L1CaloDetectorRegion( efexlabel,
                L1CaloDetectorRegion::Back, true, globaleta, globalphi,  
                supercelletawidth, supercellphiwidth , supercelletacoord, supercellphicoord);
    }
    else if(layer == 10){
        m_region = L1CaloDetectorRegion( efexlabel,
                L1CaloDetectorRegion::Hadronic, true, globaleta, globalphi,  
                supercelletawidth, supercellphiwidth , supercelletacoord, supercellphicoord);
    }
    int globalEta = m_region.getEtaIndex();
    int globalPhi = m_region.getPhiIndex();
    std::string miniPodLabel = m_hwinfo.getMpodLabel();
    int moduleCBA = 2 - ( efexcords.at(0) - 'A' ); //ASCII operation
    int octantNum = ( efexcords.at(1) - '0' ) - 1; //ASCII operation
    int moduleBaseEta = -24 + (moduleCBA * 16);
    int fpgaBaseEta = moduleBaseEta + (fpga * 4);
    int moduleLowerEta = moduleBaseEta;
    int moduleUpperEta = moduleBaseEta + 16;
    int fpgaLowerEta = fpgaBaseEta;
    int fpgaUpperEta = fpgaBaseEta + 4;
    if      (moduleCBA == 0 && fpga == 0) { moduleLowerEta--; fpgaLowerEta--; }
    else if (moduleCBA == 2 && fpga == 3) { moduleUpperEta++; fpgaUpperEta++; }
    int shiftedPhi = (globalPhi + 62) % 64;
    int lowerShiftedPhi = octantNum * 8;
    int upperShiftedPhi = lowerShiftedPhi + 8;
    int fpgaOverlap = 0;
    if (shiftedPhi >= upperShiftedPhi || shiftedPhi < lowerShiftedPhi) {
        // Set overlap bit 1 if phi is just below the core, 2 otherwise.
        // We need special treatment of the wraparound near phi=0 (or 64).
        if (octantNum == 0 || octantNum == 7) {
            fpgaOverlap |= (shiftedPhi > 32) ? 1 : 2;
        }
        else {
            fpgaOverlap |= (shiftedPhi < lowerShiftedPhi) ? 1 : 2;
        }
    }
    if (globalEta >= fpgaUpperEta || globalEta < fpgaLowerEta) {
        fpgaOverlap |= (globalEta < fpgaLowerEta) ? 4 : 8;
    }
    m_hwinfo.setOverlap(fpgaOverlap);

}
std::string EfexCellMapping::findModuleCords(int crate, int efexnumber) const {
    // Set of sensible range checks, if out of range return so blank
    // declaration of L1CaloDetector Region
    if ( (crate<0 || crate>1) ){ 
        throw "Invalid Crate # created within eFEX mapping";
        return "Err";
    }
    if ( efexnumber<0 || efexnumber>11 ){
        throw "Invalid Module # created within eFEX mapping";
        return "Err";
    }
    // For input crate and efexnumber (assuming 'eFEX number' for 
    // efexnumber label)

    // Find old eFEX labels in mapping csvs
    // with octant [1:8] and eta slice [A,B,C]:
    int octant = 4*crate + efexnumber/3 + 1; //Note int div  

    int etaslicenum = 3 - efexnumber%3; //obvious A=1 etc mapping
    std::string etaslice;
    switch(etaslicenum){
        case 1:
            etaslice = 'A';
            break;
        case 2:
            etaslice = 'B';
            break;
        case 3:
            etaslice = 'C';
            break;
    }
    return etaslice + std::to_string(octant);  
}
