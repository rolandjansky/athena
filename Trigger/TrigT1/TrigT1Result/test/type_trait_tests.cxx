#include "TrigT1Result/MuCTPI_RIO.h"
#include "TrigT1Result/MuCTPIRoI.h"
#include "TrigT1Result/MuCTPIResult.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/RecRoIBResult.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/MuCTPI_MultiplicityWord_Decoder.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "TrigT1Result/Header.h"
#include "TrigT1Result/L1TopoResult.h"
#include "TrigT1Result/JetEnergyRoI.h"
#include "TrigT1Result/JetEnergyResult.h"
#include "TrigT1Result/EMTauRoI.h"
#include "TrigT1Result/EMTauResult.h"
#include "TrigT1Result/CTPRoI.h"
#include "TrigT1Result/CTPResult.h"
#include "TrigT1Result/CTP_RIO.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/Trailer.h"
#

using namespace ROIB;

int main(){
    static_assert(std::is_nothrow_move_constructible<Header>::value);
    static_assert(std::is_nothrow_move_constructible<Trailer>::value);
    static_assert(std::is_trivially_copy_constructible<MuCTPIRoI>::value);
    static_assert(std::is_trivially_destructible<MuCTPIRoI>::value);
    static_assert(std::is_nothrow_move_constructible<MuCTPIResult>::value);
    static_assert(std::is_nothrow_move_assignable<MuCTPIResult>::value);
    static_assert(std::is_trivially_copy_constructible<MuCTPI_RIO::MyRoI>::value);
    static_assert(std::is_trivially_destructible<MuCTPI_RIO::MyRoI>::value);

    

    static_assert(std::is_nothrow_move_constructible<RecRoIBResult>::value);
    static_assert(std::is_nothrow_move_constructible<RoIBResult>::value);
    static_assert(std::is_trivially_copy_constructible<MuCTPI_MultiplicityWord_Decoder>::value);
    static_assert(std::is_trivially_destructible<MuCTPI_MultiplicityWord_Decoder>::value);
    static_assert(std::is_trivially_copy_constructible<MuCTPI_DataWord_Decoder>::value);
    static_assert(std::is_trivially_destructible<MuCTPI_DataWord_Decoder>::value);

    static_assert(std::is_nothrow_move_constructible<L1TopoResult>::value);
    static_assert(std::is_trivially_copy_constructible<JetEnergyRoI>::value);
    static_assert(std::is_trivially_destructible<JetEnergyRoI>::value);
    static_assert(std::is_nothrow_move_constructible<JetEnergyResult>::value);
    static_assert(std::is_trivially_copy_constructible<EMTauRoI>::value);
    static_assert(std::is_trivially_destructible<EMTauRoI>::value);
    static_assert(std::is_nothrow_move_constructible<EMTauResult>::value);
    static_assert(std::is_trivially_copy_constructible<CTPRoI>::value);
    static_assert(std::is_trivially_destructible<CTPRoI>::value);

    //Dunno why this isn't movable but it isn't used much
    //static_assert(std::is_nothrow_move_constructible<MuCTPI_RIO>::value);
    //static_assert(std::is_nothrow_move_constructible<MuCTPI_RDO>::value);
    
    //Dunno what CTPdataformatVersion is so this will have to wait
//    static_assert(std::is_trivially_copy_constructible<CTPResult>::value);
//    static_assert(std::is_trivially_copy_constructible<CTP_RIO>::value);
//    static_assert(std::is_trivially_copy_constructible<CTP_RDO>::value);  
	return 0;
}