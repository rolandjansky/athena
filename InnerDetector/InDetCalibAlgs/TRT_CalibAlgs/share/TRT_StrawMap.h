#ifndef __STRAW_MAP_H__
#define __STRAW_MAP_H__

#include <iostream>
#include <cassert>
#include <cmath>

// *************************************************************************************************************
// Sasa Fratina / July 22, 2011
// 
// athena to detector read-out numbering mapping
// documented in ATL-COM-INDET-2010-002
// debugged using special noise scans done by Jon Stahlman and by Mike Hance
// 
// call from your code by using one of the two constructors, and then access any value you want
// recommended: create one dummy instance of strawMap class that lives for the full length of the code, like:
//              main() ... strawMap dummy_straw(1,0,0); - otherwise inicializations might be called many times 
// *************************************************************************************************************

class strawMap {
	
public:
	
	strawMap(int side, int phi, int straw);	// straw: unique straw index within a phi sector, 0-1641 for barrel and 1642-5481 for end-cap straws, used also bt TRT_StrawStatus
	strawMap(int side, int phi, int layer, int strawLayer, int strawWithinStrawLayer); // the five athena identifiers
		
	~strawMap();
	
	inline int side() const { return m_side; }
	inline int phi() const { return m_phi; }
	inline int straw() const { return m_straw; }
	
	inline int layer() const { return m_strawToLayer[m_straw]; };
	inline int strawLayer() const { return m_strawToStrawLayer[m_straw]; };
	inline int strawWithinLayer() const { return m_strawToStrawWithinLayer[m_straw]; };
	
	int DTMROC();	
	int ASDBLR();	
	int TTCgroup();
	
	int HVpad();
	
private:
	
	int m_side; // +1 side A -1 side C
	int m_phi;
	int m_straw;
	
	static int *m_strawToDTMROC_barrel;
	static int *m_strawToASDBLR_barrelSideA;
	static int *m_strawToASDBLR_barrelSideC;

	static int *m_strawToHVpad_barrel;
	
	static int *m_strawToLayer;
	static int *m_strawToStrawLayer;
	static int *m_strawToStrawWithinLayer;
	
	static int instance_count;
	
	void initialize();			
	void clean_up();	
	
	int barrelStrawNumber(int strawNumber, int strawlayerNumber, int LayerNumber);	
	int endcapStrawNumber(int strawNumber, int strawLayerNumber, int layerNumber);
	int ec_chip(int side, int phi, int layerNumber, int strawLayerNumber, int strawNumber);	
};

int *strawMap::m_strawToDTMROC_barrel = 0;
int *strawMap::m_strawToASDBLR_barrelSideA = 0;
int *strawMap::m_strawToASDBLR_barrelSideC = 0;

int *strawMap::m_strawToHVpad_barrel = 0;

int *strawMap::m_strawToLayer = 0;
int *strawMap::m_strawToStrawLayer = 0;
int *strawMap::m_strawToStrawWithinLayer = 0;

int strawMap::instance_count = 0;



strawMap::strawMap(int side, int phi, int straw) : m_side(side), m_phi(phi), m_straw(straw) {
	
	if (m_strawToDTMROC_barrel==0) initialize();
	instance_count++;
	
	assert( straw>=0 && straw<5482 );
	assert( fabs(side)==1 || fabs(side)==2 );
	assert( phi>=0 && phi<32 );
};

strawMap::strawMap(int side, int phi, int layer, int strawLayer, int strawWithinStrawLayer) : m_side(side), m_phi(phi) {
	
	if (m_strawToDTMROC_barrel==0) initialize();
	instance_count++;
	
	m_straw = (fabs(side)==1) ? barrelStrawNumber(strawWithinStrawLayer, strawLayer, layer) : endcapStrawNumber(strawWithinStrawLayer, strawLayer, layer);
	
	assert( m_straw>=0 && m_straw<5482 );
	assert( fabs(side)==1 || fabs(side)==2 );
	assert( phi>=0 && phi<32 );
};

strawMap::~strawMap() {	
	instance_count--;
	if (instance_count==0) clean_up();
};

int strawMap::DTMROC() {
	
	if (fabs(m_side)==1) return m_strawToDTMROC_barrel[m_straw];
	
	assert( fabs(m_side)==2 ); // end-caps
	int layerNumber = this->layer();
	int strawLayerNumber = this->strawLayer();
	int strawNumber = this->strawWithinLayer();
	
	int chip = ec_chip( m_side, m_phi, layerNumber, strawLayerNumber, strawNumber); // returns 0-11
	chip += 12 * ( this->TTCgroup() - 9 ); // increment according to the board number
	chip += 104;
	return chip;
};

int strawMap::ASDBLR() {
	
	if (m_side==1) return m_strawToASDBLR_barrelSideA[m_straw];
	else if (m_side==-1) return m_strawToASDBLR_barrelSideC[m_straw];
	
	assert(fabs(m_side)==2);
	
	int strawNumber = this->strawWithinLayer();
	int strawLayerNumber = this->strawLayer();
	
	int asdblr = (strawNumber%4 - strawNumber%2) / 2;
	if (!(asdblr==0||asdblr==1)) printf("ASDBLR error: %d\n", asdblr);
	if (strawLayerNumber%8 >= 4) asdblr = 1 - asdblr;
	if (m_side<0) asdblr = 1 - asdblr;
	
	return asdblr;
};

int strawMap::TTCgroup() {
	
	if (fabs(m_side)==1) { // barrel
		
		// return logical board index:
		// 0 for Board 1S (has 10 chips)  0 -  9
		// 1 for 1L (11)                 10 - 20
		// 2 for 2S (15)                 21 - 35
		// 3 for 2L, first 9 chips       36 - 44
		// 4 for 2L, second 9 chips      45 - 53
		// 5 for 3S, first 11            54 - 64
		// 6 for 3S, second 12           65 - 76
		// 7 for 3L, first 13            77 - 89 
		// 8 for 3L, second 14           90 - 103
		
		const int list[] = {10, 11, 15, 9, 9, 11, 12, 13, 14};
		int count = 0;
		
		int chip = this->DTMROC();
		
		for (int i=0; i<9; i++) {
			count += list[i];
			if (chip < count) return i;
		}
		
		assert(count==104);
		assert(0); // should never come this far
		return -1;
		
	} else if (fabs(m_side)==2) { // end-caps
		
		int straw = m_straw - 1642;
		int board = 9;
		while (straw>=192) { // the number of straws per triplet board
			straw -= 192;
			board++;
		}
		return board;
	}
	
	assert(0);
	return -1;
};

int strawMap::HVpad() {
	
	if (fabs(m_side)==1) return m_strawToHVpad_barrel[m_straw];
	
	assert( fabs(m_side)==2 ); // end-caps	
	return ( (this->DTMROC()) * 2 + (this->ASDBLR()) );	
};

void strawMap::initialize() {
	
	printf("strawMap::initialize - initialize table containters\n");
	static int count_initialize(0); count_initialize++;
	if (count_initialize>2) {
		printf("strawMap::initialize - if you are seeing a lot of this messages, \n");
		printf("                      create one dummy instance of strawMap class that lives \n");
		printf("                      from start till the end of your programm. \n");
	}
	
	int table_length = 1642; // N straws in barrel
	m_strawToDTMROC_barrel = new int[table_length];
	m_strawToASDBLR_barrelSideA = new int[table_length];
	m_strawToASDBLR_barrelSideC = new int[table_length];
	for (int i=0; i<table_length; i++) {
		m_strawToDTMROC_barrel[i] = 0;
		m_strawToASDBLR_barrelSideA[i] = 0;
		m_strawToASDBLR_barrelSideC[i] = 0;
	}
	m_strawToHVpad_barrel = new int[table_length];		
	
	table_length = 5482; // N straws in barrel + end-caps
	m_strawToStrawLayer = new int[table_length]; 
	m_strawToLayer = new int[table_length]; 		
	m_strawToStrawWithinLayer = new int[table_length]; 
	
	for (int i=0; i<table_length; i++) {
		m_strawToLayer[i] = 0;
		m_strawToStrawLayer[i] = 0;
		m_strawToStrawWithinLayer[i] = 0;
	}
	
	int index[15];
	int count(0);			
	char filename[1000]; sprintf(filename, "TRT_StrawMap.txt");
	FILE *f = fopen(filename, "r"); 
	if (!f) {
		printf("TRT_StrawMap::initialize() ERROR: failed to open the mapping file %s \n", filename);
		printf("TRT_StrawMap::initialize() ERROR: you need to fix the file name / link, WILL CRASH THE CODE NOW\n");
		assert(0);			
	}
	while(fscanf(f, "%d %d %d %d %d %d %d %d\n", index, index+1, index+2, index+3, index+4, index+5, index+6, index+7)==8) {
		assert(index[0]>=0 && index[0]<5482);
		m_strawToLayer[index[0]] = index[1];
		m_strawToStrawLayer[index[0]] = index[2];
		m_strawToStrawWithinLayer[index[0]] = index[3];
		if (index[0]<1642) {
			m_strawToDTMROC_barrel[index[0]] = index[4];
			m_strawToASDBLR_barrelSideA[index[0]] = index[5];
			m_strawToASDBLR_barrelSideC[index[0]] = index[6];
			m_strawToHVpad_barrel[index[0]] = index[7];	
		}
		count++;
	}		
	printf("read %d lines from file %s\n", count, filename);
	fclose(f);
	
	return;
};	

void strawMap::clean_up() {
	
	printf("strawMap::clean_up - delete all tables\n\n");
	
	delete[] m_strawToLayer; m_strawToLayer = 0; 
	delete[] m_strawToStrawLayer; m_strawToStrawLayer = 0; 
	delete[] m_strawToStrawWithinLayer; m_strawToStrawWithinLayer = 0; 
	
	delete[] m_strawToDTMROC_barrel; m_strawToDTMROC_barrel = 0;
	delete[] m_strawToASDBLR_barrelSideA; m_strawToASDBLR_barrelSideA = 0; 
	delete[] m_strawToASDBLR_barrelSideC; m_strawToASDBLR_barrelSideC = 0; 
	
	delete[] m_strawToHVpad_barrel; m_strawToHVpad_barrel = 0;
	
	return;
};	

int strawMap::barrelStrawNumber(int strawNumber, int strawlayerNumber, int LayerNumber) {
	
	const int numberOfStraws[74] = { 0, 15, 31, 47, 63, 79, 96, 113, 130, 147, 164, 182, 200, 218, 236, 254, 273, 292, 311, 329, // layer 0, 329 straws, strawlayers 0-18
		348, 368, 388, 408, 428, 448, 469, 490, 511, 532, 553, 575, 597, 619, 641, 663, 686, 709, 732, 755, 778, 802, 826, 849, // layer 1, 520 straws, strawLayers 0-23
		872, 896, 920, 944, 968, 993, 1018, 1043, 1068, 1093, 1119, 1145, 1171, 1197, 1223, 1250, 1277, 1304, 1331, 1358, 1386, 1414, 1442, 1470, 1498, 1527, 1556, 1585, 1614, 1642 }; // layer 2
	
	if (LayerNumber==1) strawlayerNumber+= 19;
	else if (LayerNumber==2) strawlayerNumber+= 43;
	
	return ( numberOfStraws[strawlayerNumber+1] - strawNumber -1 );
};

int strawMap::endcapStrawNumber(int strawNumber, int strawLayerNumber, int layerNumber) {
	
	int board = layerNumber;
	if (board<6) { board *= 2; if (strawLayerNumber>7) board++; }
	else { board += 6; }
	
	int straw = board * 192 + strawNumber * 8 + strawLayerNumber % 8 ;
	straw += 1642;
	return straw;
};	

int strawMap::ec_chip(int side, int phi, int layerNumber, int strawLayerNumber, int strawNumber) {
	// endcap only, return 0-12
	
	assert( fabs(side)==2 );
	
	static int count = 0;
	static int *map_A0 = new int[12];
	static int *map_A8 = new int[12];
	static int *map_C0 = new int[12];
	static int *map_C8 = new int[12]; 
	if (!count) { 
		count++;
		int chipIndexToChipHardware_A0[] = {2, 1, 3, 0, 6, 5, 7, 4, 10, 9, 11, 8};
		int chipIndexToChipHardware_A8[] = {8, 11, 9, 10, 4, 7, 5, 6, 0, 3, 1, 2};
		int chipIndexToChipHardware_C0[] = {1, 2, 0, 3, 5, 6, 4, 7, 9, 10, 8, 11};
		int chipIndexToChipHardware_C8[] = {11, 8, 10, 9, 7, 4, 6, 5, 3, 0, 2, 1};
		int *map_list[] = {map_A0, map_A8, map_C0, map_C8};
		int *chip_list[] = {chipIndexToChipHardware_A0, chipIndexToChipHardware_A8, chipIndexToChipHardware_C0, chipIndexToChipHardware_C8};
		for (int i=0; i<4; i++) for (int j=0; j<12; j++) map_list[i][j] = chip_list[i][j];
	}
	
	int chip = (strawNumber - strawNumber%4) / 2;
	if (strawLayerNumber%8 >= 4) chip++; 
	
	int nominal_reversed = 1 - ( (phi - phi%8)/8 ) %2 ;
	int *map = 0;
	if (side==2) map = nominal_reversed ? map_A8 : map_A0;
	if (side==-2) map = nominal_reversed ? map_C8 : map_C0;
	chip = map[chip];		
	
	return chip;
}		

#endif // __STRAW_MAP_H__
