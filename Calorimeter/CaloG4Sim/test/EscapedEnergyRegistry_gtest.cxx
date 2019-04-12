#include "CaloG4Sim/EscapedEnergyRegistry.h"
#include "CaloG4Sim/derivedVEscapedEnergyProcessing.h"
#include "gtest/gtest.h"

TEST( testcase, test1 ) {

const G4String name = "processing";
CaloG4::EscapedEnergyRegistry* classToTest = CaloG4::EscapedEnergyRegistry::GetInstance();
CaloG4::derivedVEscapedEnergyProcessing* classToOperate = new CaloG4::derivedVEscapedEnergyProcessing();
classToTest->AddAndAdoptProcessing( name, classToOperate );
ASSERT_EQ( classToOperate, classToTest->GetProcessing( name ) );

}

int main(int argc, char **argv) {

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

}
