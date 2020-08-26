/* 
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*
*
*	AFPSiLayerAlgorithm
*
*
*/

#include "Run3AFPMonitoring/AFPSiLayerAlgorithm.h"
#include <Run3AFPMonitoring/AFPFastReco.h>
#include "StoreGate/ReadHandleKey.h"
#include "xAODForward/AFPStationID.h"
#include "EventInfo/EventID.h"

#include <vector>


	unsigned int clusterCounter[1000][4][4];
	unsigned int clusterCounterFront[1000][4][4];
	unsigned int clusterCounterEnd[1000][4][4];
	unsigned int clusterCounterMiddle[1000][4][4];
	
	unsigned int clusterCounterStation[1000][4];
	unsigned int clusterCounterStationFront[1000][4];
	unsigned int clusterCounterStationEnd[1000][4];
	unsigned int clusterCounterStationMiddle[1000][4];

	int previouslb = 0;
	int previouslbFront = 0;
	int previouslbEnd = 0;
	int previouslbMiddle = 0;
	
	int previouslbStation = 0;
	int previouslbStationFront = 0;
	int previouslbStationEnd = 0;
	int previouslbStationMiddle = 0;

	unsigned int counterForEvents = 0;
	unsigned int counterForEventsFront = 0;
	unsigned int counterForEventsEnd = 0;
	unsigned int counterForEventsMiddle = 0;
	
	unsigned int counterForEventsStation = 0;
	unsigned int counterForEventsStationFront = 0;
	unsigned int counterForEventsStationEnd = 0;
	unsigned int counterForEventsStationMiddle = 0;
	
	/*
	int frontBCIDs[] = {63,75,87,99,114,126,138,150,165,177,189,201,240,252,264,276,291,303,315,327,342,354,366,378,417,429,441,453,468,480,492,
504,519,531,543,555,594,606,618,630,645,657,669,681,696,708,720,732,780,792,804,816,831,843,855,867,882,894,906,918,957,969,981,993,1008,1020,
1032,1044,1059,1071,1083,1095,1134,1146,1158,1170,1185,1197,1209,1221,1236,1248,1260,1272,1311,1323,1335,1347,1362,1374,1386,1398,1413,1425,1437,
1449,1488,1500,1512,1524,1539,1551,1563,1575,1590,1602,1614,1626,1674,1686,1698,1710,1725,1737,1749,1761,1776,1788,1800,1812,1851,1863,1875,1887,
1902,1914,1926,1938,1953,1965,1977,1989,2028,2040,2052,2064,2079,2091,2103,2115,2130,2142,2154,2166,2205,2217,2229,2241,2256,2268,2280,2292,2307,
2319,2331,2343,2382,2394,2406,2418,2433,2445,2457,2469,2484,2496,2508,2520,2568,2580,2592,2604,2619,2631,2643,2655,2670,2682,2694,2706,2745,2757,
2769,2781,2796,2808,2820,2832,2847,2859,2871,2883,2922,2934,2946,2958,2973,2985,2997,3009,3024,3036,3048,3060,3099,3111,3123,3135,3150,3162,3174,
3186,3201,3213,3225,3237,3276,3288,3300,3312,3327,3339,3351,3363,3378,3390,3402,3414,-1};
	*/
	std::vector<int> frontBCIDsVector;
	/*
	int middleBCIDs[] = {64,65,66,67,68,69,76,77,78,79,80,81,88,89,90,91,92,93,100,101,102,103,104,105,115,116,117,118,119,120,127,128,129,130,131,132,139,140,141,142,143,144,151,
152,153,154,155,156,166,167,168,169,170,171,178,179,180,181,182,183,190,191,192,193,194,195,202,203,204,205,206,207,241,242,243,244,245,246,253,254,255,256,257,258,
265,266,267,268,269,270,277,278,279,280,281,282,292,293,294,295,296,297,304,305,306,307,308,309,316,317,318,319,320,321,328,329,330,331,332,333,343,344,345,346,347,
348,355,356,357,358,359,360,367,368,369,370,371,372,379,380,381,382,383,384,418,419,420,421,422,423,430,431,432,433,434,435,442,443,444,445,446,447,454,455,456,457,
458,459,469,470,471,472,473,474,481,482,483,484,485,486,493,494,495,496,497,498,505,506,507,508,509,510,520,521,522,523,524,525,532,533,534,535,536,537,544,545,546,
547,548,549,556,557,558,559,560,561,595,596,597,598,599,600,607,608,609,610,611,612,619,620,621,622,623,624,631,632,633,634,635,636,646,647,648,649,650,651,658,659,
660,661,662,663,670,671,672,673,674,675,682,683,684,685,686,687,697,698,699,700,701,702,709,710,711,712,713,714,721,722,723,724,725,726,733,734,735,736,737,738,781,
782,783,784,785,786,793,794,795,796,797,798,805,806,807,808,809,810,817,818,819,820,821,822,832,833,834,835,836,837,844,845,846,847,848,849,856,857,858,859,860,861,
868,869,870,871,872,873,883,884,885,886,887,888,895,896,897,898,899,900,907,908,909,910,911,912,919,920,921,922,923,924,958,959,960,961,962,963,970,971,972,973,974,
975,982,983,984,985,986,987,994,995,996,997,998,999,1009,1010,1011,1012,1013,1014,1021,1022,1023,1024,1025,1026,1033,1034,1035,1036,1037,1038,1045,1046,1047,1048,
1049,1050,1060,1061,1062,1063,1064,1065,1072,1073,1074,1075,1076,1077,1084,1085,1086,1087,1088,1089,1096,1097,1098,1099,1100,1101,1135,1136,1137,1138,1139,1140,1147,
1148,1149,1150,1151,1152,1159,1160,1161,1162,1163,1164,1171,1172,1173,1174,1175,1176,1186,1187,1188,1189,1190,1191,1198,1199,1200,1201,1202,1203,1210,1211,1212,1213,
1214,1215,1222,1223,1224,1225,1226,1227,1237,1238,1239,1240,1241,1242,1249,1250,1251,1252,1253,1254,1261,1262,1263,1264,1265,1266,1273,1274,1275,1276,1277,1278,1312,
1313,1314,1315,1316,1317,1324,1325,1326,1327,1328,1329,1336,1337,1338,1339,1340,1341,1348,1349,1350,1351,1352,1353,1363,1364,1365,1366,1367,1368,1375,1376,1377,1378,
1379,1380,1387,1388,1389,1390,1391,1392,1399,1400,1401,1402,1403,1404,1414,1415,1416,1417,1418,1419,1426,1427,1428,1429,1430,1431,1438,1439,1440,1441,1442,1443,1450,
1451,1452,1453,1454,1455,1489,1490,1491,1492,1493,1494,1501,1502,1503,1504,1505,1506,1513,1514,1515,1516,1517,1518,1525,1526,1527,1528,1529,1530,1540,1541,1542,1543,
1544,1545,1552,1553,1554,1555,1556,1557,1564,1565,1566,1567,1568,1569,1576,1577,1578,1579,1580,1581,1591,1592,1593,1594,1595,1596,1603,1604,1605,1606,1607,1608,1615,
1616,1617,1618,1619,1620,1627,1628,1629,1630,1631,1632,1675,1676,1677,1678,1679,1680,1687,1688,1689,1690,1691,1692,1699,1700,1701,1702,1703,1704,1711,1712,1713,1714,
1715,1716,1726,1727,1728,1729,1730,1731,1738,1739,1740,1741,1742,1743,1750,1751,1752,1753,1754,1755,1762,1763,1764,1765,1766,1767,1777,1778,1779,1780,1781,1782,1789,
1790,1791,1792,1793,1794,1801,1802,1803,1804,1805,1806,1813,1814,1815,1816,1817,1818,1852,1853,1854,1855,1856,1857,1864,1865,1866,1867,1868,1869,1876,1877,1878,1879,
1880,1881,1888,1889,1890,1891,1892,1893,1903,1904,1905,1906,1907,1908,1915,1916,1917,1918,1919,1920,1927,1928,1929,1930,1931,1932,1939,1940,1941,1942,1943,1944,1954,
1955,1956,1957,1958,1959,1966,1967,1968,1969,1970,1971,1978,1979,1980,1981,1982,1983,1990,1991,1992,1993,1994,1995,2029,2030,2031,2032,2033,2034,2041,2042,2043,2044,
2045,2046,2053,2054,2055,2056,2057,2058,2065,2066,2067,2068,2069,2070,2080,2081,2082,2083,2084,2085,2092,2093,2094,2095,2096,2097,2104,2105,2106,2107,2108,2109,2116,
2117,2118,2119,2120,2121,2131,2132,2133,2134,2135,2136,2143,2144,2145,2146,2147,2148,2155,2156,2157,2158,2159,2160,2167,2168,2169,2170,2171,2172,2206,2207,2208,2209,
2210,2211,2218,2219,2220,2221,2222,2223,2230,2231,2232,2233,2234,2235,2242,2243,2244,2245,2246,2247,2257,2258,2259,2260,2261,2262,2269,2270,2271,2272,2273,2274,2281,
2282,2283,2284,2285,2286,2293,2294,2295,2296,2297,2298,2308,2309,2310,2311,2312,2313,2320,2321,2322,2323,2324,2325,2332,2333,2334,2335,2336,2337,2344,2345,2346,2347,
2348,2349,2383,2384,2385,2386,2387,2388,2395,2396,2397,2398,2399,2400,2407,2408,2409,2410,2411,2412,2419,2420,2421,2422,2423,2424,2434,2435,2436,2437,2438,2439,2446,
2447,2448,2449,2450,2451,2458,2459,2460,2461,2462,2463,2470,2471,2472,2473,2474,2475,2485,2486,2487,2488,2489,2490,2497,2498,2499,2500,2501,2502,2509,2510,2511,2512,
2513,2514,2521,2522,2523,2524,2525,2526,2569,2570,2571,2572,2573,2574,2581,2582,2583,2584,2585,2586,2593,2594,2595,2596,2597,2598,2605,2606,2607,2608,2609,2610,2620,
2621,2622,2623,2624,2625,2632,2633,2634,2635,2636,2637,2644,2645,2646,2647,2648,2649,2656,2657,2658,2659,2660,2661,2671,2672,2673,2674,2675,2676,2683,2684,2685,2686,
2687,2688,2695,2696,2697,2698,2699,2700,2707,2708,2709,2710,2711,2712,2746,2747,2748,2749,2750,2751,2758,2759,2760,2761,2762,2763,2770,2771,2772,2773,2774,2775,2782,
2783,2784,2785,2786,2787,2797,2798,2799,2800,2801,2802,2809,2810,2811,2812,2813,2814,2821,2822,2823,2824,2825,2826,2833,2834,2835,2836,2837,2838,2848,2849,2850,2851,
2852,2853,2860,2861,2862,2863,2864,2865,2872,2873,2874,2875,2876,2877,2884,2885,2886,2887,2888,2889,2923,2924,2925,2926,2927,2928,2935,2936,2937,2938,2939,2940,2947,
2948,2949,2950,2951,2952,2959,2960,2961,2962,2963,2964,2974,2975,2976,2977,2978,2979,2986,2987,2988,2989,2990,2991,2998,2999,3000,3001,3002,3003,3010,3011,3012,3013,
3014,3015,3025,3026,3027,3028,3029,3030,3037,3038,3039,3040,3041,3042,3049,3050,3051,3052,3053,3054,3061,3062,3063,3064,3065,3066,3100,3101,3102,3103,3104,3105,3112,
3113,3114,3115,3116,3117,3124,3125,3126,3127,3128,3129,3136,3137,3138,3139,3140,3141,3151,3152,3153,3154,3155,3156,3163,3164,3165,3166,3167,3168,3175,3176,3177,3178,
3179,3180,3187,3188,3189,3190,3191,3192,3202,3203,3204,3205,3206,3207,3214,3215,3216,3217,3218,3219,3226,3227,3228,3229,3230,3231,3238,3239,3240,3241,3242,3243,3277,
3278,3279,3280,3281,3282,3289,3290,3291,3292,3293,3294,3301,3302,3303,3304,3305,3306,3313,3314,3315,3316,3317,3318,3328,3329,3330,3331,3332,3333,3340,3341,3342,3343,
3344,3345,3352,3353,3354,3355,3356,3357,3364,3365,3366,3367,3368,3369,3379,3380,3381,3382,3383,3384,3391,3392,3393,3394,3395,3396,3403,3404,3405,3406,3407,3408,3415,
3416,3417,3418,3419,3420,-1};
	*/
	std::vector<int> middleBCIDsVector;
	/*
	int endBCIDs[] = {70,82,94,106,121,133,145,157,172,184,196,208,247,259,271,283,298,310,322,334,349,361,373,385,424,436,448,460,475,487,499,511,526,538,550,562,
601,613,625,637,652,664,676,688,703,715,727,739,787,799,811,823,838,850,862,874,889,901,913,925,964,976,988,1000,1015,1027,1039,1051,1066,1078,1090,1102,1141,1153,
1165,1177,1192,1204,1216,1228,1243,1255,1267,1279,1318,1330,1342,1354,1369,1381,1393,1405,1420,1432,1444,1456,1495,1507,1519,1531,1546,1558,1570,1582,1597,1609,1621,
1633,1681,1693,1705,1717,1732,1744,1756,1768,1783,1795,1807,1819,1858,1870,1882,1894,1909,1921,1933,1945,1960,1972,1984,1996,2035,2047,2059,2071,2086,2098,2110,2122,
2137,2149,2161,2173,2212,2224,2236,2248,2263,2275,2287,2299,2314,2326,2338,2350,2389,2401,2413,2425,2440,2452,2464,2476,2491,2503,2515,2527,2575,2587,2599,2611,2626,
2638,2650,2662,2677,2689,2701,2713,2752,2764,2776,2788,2803,2815,2827,2839,2854,2866,2878,2890,2929,2941,2953,2965,2980,2992,3004,3016,3031,3043,3055,3067,3106,3118,
3130,3142,3157,3169,3181,3193,3208,3220,3232,3244,3283,3295,3307,3319,3334,3346,3358,3370,3385,3397,3409,3421,-1};
	*/
	std::vector<int> endBCIDsVector;
		
	bool isInList(int bcid, int* arr)
	{
		int i=0;
		while(arr[i] != -1)
		{
			if(bcid == arr[i])
			{
				return true;
			}
			i++;
		}
		return false;
	}
	
	bool isInListVector(int bcid, std::vector<int> arr)
	{
		for(int i=0; i<arr.size(); i++)
		{
			if(arr[i] == bcid)
				return true;
		}
		return false;
	}

AFPSiLayerAlgorithm::AFPSiLayerAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
, m_afpHitContainerKey("AFPSiHitContainer")
// 2
, m_bunchCrossingTool("BunchCrossingTool")
{
	declareProperty("AFPSiHitContainer", m_afpHitContainerKey);
	// 2
	declareProperty("BunchCrossingTool", m_bunchCrossingTool);
}


AFPSiLayerAlgorithm::~AFPSiLayerAlgorithm() {}


StatusCode AFPSiLayerAlgorithm::initialize() {

	using namespace Monitored;

	m_HitmapGroups = buildToolMap<std::map<std::string,int>>(m_tools,"AFPSiLayerTool", m_stationnames, m_pixlayers);
	m_TrackGroup = buildToolMap<int>(m_tools, "AFPSiLayerTool", m_stationnames);


	// We must declare to the framework in initialize what SG objects we are going to use:
	SG::ReadHandleKey<xAOD::AFPSiHitContainer> afpHitContainerKey("AFPSiHits");
	ATH_CHECK(m_afpHitContainerKey.initialize());
	
	
	for(int a=0; a<1000; a++)
	{
		for(int b=0;b<4;b++)
		{
			for(int c=0; c<4; c++)
			{
				clusterCounter[a][b][c] = 0;
			}
		}
	}
	
	// 2
	std::cout << "\n\nKrecemo!...\n\n";
	m_bunchCrossingTool.setTypeAndName("Trig::MCBunchCrossingTool/BunchCrossingTool"); // this works, but LHCBunch doesn't
	ATH_CHECK( m_bunchCrossingTool.retrieve() );
	std::cout << "\n\n\n";
	ATH_MSG_INFO( "initialization completed" );
	std::cout << "\n\n\n";
	std::cout << "Number of filled bunches: " << m_bunchCrossingTool.numberOfFilledBunches() << std::endl;
	std::cout << "Number of bunch trains: " << m_bunchCrossingTool.numberOfBunchTrains() << std::endl;
	return AthMonitorAlgorithm::initialize();
}

//StatusCode AFPSiLayerAlgorithm::execute(const EventContext& ctx) const {
//	using namespace Monitored;

	
//	return StatusCode::SUCCESS;
//}

StatusCode AFPSiLayerAlgorithm::fillHistograms( const EventContext& ctx ) const {
	using namespace Monitored;

	for(int j=0; j<3500; j++)
	{
		if(!m_bunchCrossingTool->isFilled(j))
			std::cout << "Not filled " << j << "\n";
	}
	

	// 2
	unsigned int temp = GetEventInfo(ctx)->bcid();
	if(m_bunchCrossingTool->isFilled(temp))
	{
		std::cout << "Filled " << temp << "\n";
	}
	else
	{
		std::cout << "\t\tNOT filled " << temp << "\n";	
	}
	
	// !!! Check this logic for exteme values (min and max; 0? and 3564)
	if(m_bunchCrossingTool->isFilled(temp))
	{
		if(!m_bunchCrossingTool->isFilled(temp-1))
		{
			frontBCIDsVector.push_back(temp);
			++counterForEventsFront;
			++counterForEventsStationFront;
		}
		else
		{
			if(m_bunchCrossingTool->isFilled(temp+1))
			{
				middleBCIDsVector.push_back(temp);
				++counterForEventsMiddle;
				++counterForEventsStationMiddle;
			}
			else
			{
				endBCIDsVector.push_back(temp);
				++counterForEventsEnd;
				++counterForEventsStationEnd;
			}
		}
	}

	static unsigned int numberOfClusterStationPlane[4][4] = { {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} };
	//static unsigned int lumiBlocks[100];
	
	// Declare the quantities which should be monitored:
	auto lb = Monitored::Scalar<int>("lb", 0);
	auto muPerBCID = Monitored::Scalar<float>("muPerBCID", 0.0);
	auto run = Monitored::Scalar<int>("run",0);
	auto weight = Monitored::Scalar<float>("weight", 1.0);
	//auto previouslb = Monitored::Scalar<int>("previouslb", 0);

	auto nSiHits = Monitored::Scalar<int>("nSiHits", 1);
	auto clustersPerPlane = Monitored::Scalar<float>("clustersPerPlane", 1.0);
	auto clustersPerPlane2 = Monitored::Scalar<float>("clustersPerPlane2", 0.0);
	auto clustersPerPlaneFront = Monitored::Scalar<float>("clustersPerPlaneFront", 0.0);
	auto clustersPerPlaneEnd = Monitored::Scalar<float>("clustersPerPlaneEnd", 0.0);
	auto clustersPerPlaneMiddle = Monitored::Scalar<float>("clustersPerPlaneMiddle", 0.0);
	
	auto clustersPerStation = Monitored::Scalar<float>("clustersPerStation", 0.0);
	auto clustersPerStationFront = Monitored::Scalar<float>("clustersPerStationFront", 0.0);
	auto clustersPerStationEnd = Monitored::Scalar<float>("clustersPerStationEnd", 0.0);
	auto clustersPerStationMiddle = Monitored::Scalar<float>("clustersPerStationMiddle", 0.0);

	auto pixelRowIDChip = Monitored::Scalar<int>("pixelRowIDChip", 0); 
	auto pixelColIDChip = Monitored::Scalar<int>("pixelColIDChip", 0); 

	auto timeOverThreshold = Monitored::Scalar<float>("timeOverThreshold", 0.0);

	auto clusterX = Monitored::Scalar<float>("clusterX", 0.0);
	auto clusterY = Monitored::Scalar<float>("clusterY", 0.0); 

	auto trackX = Monitored::Scalar<float>("trackX", 0.0);
	auto trackY = Monitored::Scalar<float>("trackY", 0.0);

	auto layerEfficiency = Monitored::Scalar<float>("layerEfficiency", 0.0);
	auto layerNumber = Monitored::Scalar<int>("layerNumber", 0);
	
	
	lb = GetEventInfo(ctx)->lumiBlock();
	muPerBCID = lbAverageInteractionsPerCrossing(ctx);
	//run = GetEventInfo(ctx)->runNumber();
	fill("AFPSiLayerTool", lb, muPerBCID);
	

	++counterForEvents;		// Counter for the all BCIDs
	++counterForEventsStation;

	int tempbcid = GetEventInfo(ctx)->bcid();
	

	/* Old way for BCIDs
	if(isInList(tempbcid, frontBCIDs) == true)
	{
		++counterForEventsFront;
		++counterForEventsStationFront;
	} 
	else if (isInList(tempbcid, endBCIDs) == true)
	{
		++counterForEventsEnd;
		++counterForEventsStationEnd;
	}
	else if(isInList(tempbcid, middleBCIDs) == true)
	{
		++counterForEventsMiddle;
		++counterForEventsStationMiddle;
	}
	*/
	
	//std::cout << "\t\t" << counterForEvents << " lb:" << lb << std::endl;
	

	SG::ReadHandle<xAOD::AFPSiHitContainer> afpHitContainer(m_afpHitContainerKey, ctx);
	if(! afpHitContainer.isValid())
	{
		ATH_MSG_WARNING("evtStore() does not contain hits collection with name " << m_afpHitContainerKey);
		return StatusCode::SUCCESS;
	}

	ATH_CHECK( afpHitContainer.initialize() );

	nSiHits = afpHitContainer->size();
	//hitsPerPlane = afpHitContainer->size();
	fill("AFPSiLayerTool", lb, nSiHits);
	

	

	for(const xAOD::AFPSiHit *hitsItr: *afpHitContainer)
	{
		lb = GetEventInfo(ctx)->lumiBlock();
		pixelRowIDChip = hitsItr->pixelRowIDChip();
		pixelColIDChip = hitsItr->pixelColIDChip();
		timeOverThreshold = hitsItr->timeOverThreshold();
		
		if (hitsItr->stationID()<4 && hitsItr->stationID()>=0 && hitsItr->pixelLayerID()<4 && hitsItr->pixelLayerID()>=0) 
		{
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelRowIDChip, pixelColIDChip);
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelRowIDChip);
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], pixelColIDChip);
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(hitsItr->stationID())).at(m_pixlayers.at(hitsItr->pixelLayerID()))], timeOverThreshold);
			
		}
		else ATH_MSG_WARNING("Unrecognised station index: " << hitsItr->stationID());
	}
	
	// Filling of cluster and track 2D histograms
	AFPMon::AFPFastReco fast(afpHitContainer.get());
	fast.reco();

	for (const auto& track : fast.tracks()) 
	{
		trackX = track.x;
		trackY = track.y;
		fill(m_tools[m_TrackGroup.at(m_stationnames.at(track.station))], trackY, trackX);
	}

// Clusters:
// Davide's approach
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			clustersPerPlane2 = numberOfClusterStationPlane[i][j]*1.0;
			if(muPerBCID != 0)
			{
				clustersPerPlane2 /= (muPerBCID*counterForEvents);
			}
			else
			{
				clustersPerPlane2 = -0.1;
			}
			//std::cout <<"\t"<< clustersPerPlane2 << std::endl;
			fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlane2);
		}
	}
// Davide's approach - End

// Kris's approach:
	for(const auto& cluster : fast.clusters()) 
	{
		clusterX = cluster.x;
		clusterY = cluster.y;
		fill(m_tools[m_HitmapGroups.at(m_stationnames.at(cluster.station)).at(m_pixlayers.at(cluster.layer))], clusterY, clusterX); // Swap after suggestion
		
		lb = GetEventInfo(ctx)->lumiBlock();
		++numberOfClusterStationPlane[cluster.station][cluster.layer];
		
		// Time for fill - current and previous lb are different, and the previouslb is not -1 (it means - this is not the first lb)
		if(lb > previouslb && previouslb != 0)
		{
			for(int i=0; i<4; i++)
			{
				for(int j=0; j<4; j++)
				{
					clustersPerPlane = clusterCounter[previouslb][i][j]*1.0;
					if(muPerBCID != 0)
					{
						clustersPerPlane = clustersPerPlane/(muPerBCID*counterForEvents);
					}
					else
					{
						clustersPerPlane = -0.1;
					}
					fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlane);
				}
			}
				
			previouslb = lb;
			++clusterCounter[lb][cluster.station][cluster.layer];
			counterForEvents=1;
			
			for(int i=0; i<4; i++)
			{
				for(int j=0; j<4; j++)
				{
					numberOfClusterStationPlane[i][j] = 0;
				}
			}
		}
			
			// First time in lumiblock (in plane)
		else if(clusterCounter[lb][cluster.station][cluster.layer] == 0) 
		{
			++clusterCounter[lb][cluster.station][cluster.layer];
			previouslb = lb;
		}
			
		// Lumiblock is same, so proceed
		else if(lb==previouslb)	// Same lumiblock
		{++clusterCounter[lb][cluster.station][cluster.layer];}
			
	}	// ..... end cluster loop .....
		
		
		
	// =============== Stations all BCIDs ===============
	for(const auto& cluster : fast.clusters()) 
	{
		lb = GetEventInfo(ctx)->lumiBlock();
		if(lb > previouslbStation && previouslbStation != 0)
		{
			for(int i=0; i<4; i++)
			{
				clustersPerStation = clusterCounterStation[previouslbStation][i]*1.0;
				if(muPerBCID != 0)
				{clustersPerStation = clustersPerStation/(muPerBCID*counterForEventsStation*4);}
				else{clustersPerStation = -0.1;}

				fill(m_tools[m_TrackGroup.at(m_stationnames.at(i))], lb, clustersPerStation);
			}
			previouslbStation=lb;
			++clusterCounterStation[lb][cluster.station];
			counterForEventsStation=1;
		}
		else if (clusterCounterStation[lb][cluster.station] == 0)
		{
			++clusterCounterStation[lb][cluster.station];
			previouslbStation = lb;
		}
		else if (lb==previouslbStation)
		{++clusterCounterStation[lb][cluster.station];}
	}
	// ========== Front Station ==========
	if(isInListVector(GetEventInfo(ctx)->bcid(), frontBCIDsVector))
	{
		for(const auto& cluster : fast.clusters()) 
		{
			lb = GetEventInfo(ctx)->lumiBlock();
			if(lb > previouslbStationFront && previouslbStationFront != 0)
			{
				for(int i=0; i<4; i++)
				{
					clustersPerStationFront = clusterCounterStationFront[previouslbStationFront][i]*1.0;
					if(muPerBCID != 0)
					{
						clustersPerStationFront = clustersPerStationFront/(muPerBCID*counterForEventsStationFront*4);
					}
					else{clustersPerStationFront = -0.1;}
					
					fill(m_tools[m_TrackGroup.at(m_stationnames.at(i))], lb, clustersPerStationFront);
				}
				previouslbStationFront=lb;
				++clusterCounterStationFront[lb][cluster.station];
				counterForEventsStationFront=1;
			}
			else if (clusterCounterStationFront[lb][cluster.station] == 0)
			{
				++clusterCounterStationFront[lb][cluster.station];
				previouslbStationFront = lb;
			}
			else if (lb == previouslbStationFront)
			{++clusterCounterStationFront[lb][cluster.station];}
		}
	}
	// ..... end front station .....
			
	// ========== End Station ==========
	if(isInListVector(GetEventInfo(ctx)->bcid(), endBCIDsVector))
	{	
		for(const auto& cluster : fast.clusters())
		{
			lb = GetEventInfo(ctx)->lumiBlock();
			if(lb > previouslbStationEnd && previouslbStationEnd != 0)
			{
				for(int i=0; i<4; i++)
				{
					clustersPerStationEnd = clusterCounterStationEnd[previouslbStationEnd][i]*1.0;
					if(muPerBCID != 0)
					{
						clustersPerStationEnd = clustersPerStationEnd/(muPerBCID*counterForEventsStationEnd*4);
					}
					else{clustersPerStationEnd = -0.1;}
				
					fill(m_tools[m_TrackGroup.at(m_stationnames.at(i))], lb, clustersPerStationEnd);
				}
				previouslbStationEnd=lb;
				++clusterCounterStationEnd[lb][cluster.station];
				counterForEventsStationEnd=1;
			}
			else if (clusterCounterStationEnd[lb][cluster.station] == 0)
			{
				++clusterCounterStationEnd[lb][cluster.station];
				previouslbStationEnd = lb;
			}
			else if (lb == previouslbStationEnd)
			{++clusterCounterStationEnd[lb][cluster.station];}
		}
	}
	// ..... end end station .....
	
	// ========== Middle Station ==========
	if(isInListVector(GetEventInfo(ctx)->bcid(), middleBCIDsVector))
	{
		for(const auto& cluster : fast.clusters())
		{
			if(lb > previouslbStationMiddle && previouslbStationMiddle != 0)
			{
				for(int i=0; i<4; i++)
				{
					clustersPerStationMiddle = clusterCounterStationMiddle[previouslbStationMiddle][i]*1.0;
					if(muPerBCID != 0)
					{
						clustersPerStationMiddle = clustersPerStationMiddle/(muPerBCID*counterForEventsStationMiddle*4);
					}
					else{clustersPerStationMiddle = -0.1;}
					fill(m_tools[m_TrackGroup.at(m_stationnames.at(i))], lb, clustersPerStationMiddle);
				}
				previouslbStationMiddle=lb;
				++clusterCounterStationMiddle[lb][cluster.station];
				counterForEventsStationMiddle=1;
			}
			else if (clusterCounterStationMiddle[lb][cluster.station] == 0)
			{
				++clusterCounterStationMiddle[lb][cluster.station];
				previouslbStationMiddle = lb;
			}
			else if (lb == previouslbStationMiddle)
			{++clusterCounterStationMiddle[lb][cluster.station];}
			// end middle station
		}
	}
			// end stations
		
		// ========== Front BCID ========== (planes)
	if(isInListVector(GetEventInfo(ctx)->bcid(), frontBCIDsVector))
	{
		for(const auto& cluster : fast.clusters()) 
			{
			lb = GetEventInfo(ctx)->lumiBlock();
			// Time for fill - current and previous lb are different, and the previouslbFront is not -1 (it means - this is not the first lb)
			if(lb > previouslbFront && previouslbFront != 0)
			{
				//std::cout << "\tlb!=previouslbFront" << std::endl;
				for(int i=0; i<4; i++)
				{
					for(int j=0; j<4; j++)
					{
						clustersPerPlaneFront = clusterCounterFront[previouslbFront][i][j]*1.0;
						if(muPerBCID != 0)
						{
							clustersPerPlaneFront = clustersPerPlaneFront/(muPerBCID*counterForEventsFront);
						}
						else
						{
							clustersPerPlaneFront = -0.1;
						}
						//std::cout << "\tFILL FRONT" << std::endl;
						//std::cout << "clustersPerPlaneFront (it was zero on cernbox): " << clustersPerPlaneFront << std::endl;
						fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlaneFront);
					}
				}
				previouslbFront = lb;
				++clusterCounterFront[lb][cluster.station][cluster.layer];
				counterForEventsFront=1;
			}
		
		// First time in lumiblock (in plane)
			else if(clusterCounterFront[lb][cluster.station][cluster.layer] == 0) 
			{
				++clusterCounterFront[lb][cluster.station][cluster.layer];
				previouslbFront = lb;
			//std::cout << "\tFirst time" << std::endl;
			//std::cout << "\tLuminosity block: " << lb << std::endl;
			}
		
			// Lumiblock is same, so proceed
			else if(lb==previouslbFront)	// Same lumiblock
			{
				++clusterCounterFront[lb][cluster.station][cluster.layer];
			}
		}
	}
		
	// ========== End BCID ==========
	if(isInListVector(GetEventInfo(ctx)->bcid(), endBCIDsVector))
	{
		for(const auto& cluster : fast.clusters()) 
		{
			lb = GetEventInfo(ctx)->lumiBlock();
			// Time for fill - current and previous lb are different, and the previouslbEnd is not -1 (it means - this is not the first lb)
			if(lb > previouslbEnd && previouslbEnd != 0)
			{
				//std::cout << "\tlb!=previouslbEnd" << std::endl;
				for(int i=0; i<4; i++)
				{
					for(int j=0; j<4; j++)
					{
						clustersPerPlaneEnd = clusterCounterEnd[previouslbEnd][i][j]*1.0;
						if(muPerBCID != 0)
						{
							clustersPerPlaneEnd = clustersPerPlaneEnd/(muPerBCID*counterForEventsEnd);
						}
						else
						{
							clustersPerPlaneEnd = -0.1;
						}
						fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlaneEnd);
					}
				}
					previouslbEnd = lb;
					++clusterCounterEnd[lb][cluster.station][cluster.layer];
					counterForEventsEnd=1;
			}
			
			// First time in lumiblock (in plane)
			else if(clusterCounterEnd[lb][cluster.station][cluster.layer] == 0) 
			{
				++clusterCounterEnd[lb][cluster.station][cluster.layer];				
				previouslbEnd = lb;
			}
			
			// Lumiblock is same, so proceed
			else if(lb==previouslbEnd)	// Same lumiblock
			{
				++clusterCounterEnd[lb][cluster.station][cluster.layer];
			}
		}
	}
		
	// ========== Middle BCID ==========
	if(isInListVector(GetEventInfo(ctx)->bcid(), middleBCIDsVector))
	{
		for(const auto& cluster : fast.clusters()) 
		{
			lb = GetEventInfo(ctx)->lumiBlock();
			// Time for fill - current and previous lb are different, and the previouslbMiddle is not -1 (it means - this is not the first lb)
			if(lb > previouslbMiddle && previouslbMiddle != 0)
			{
				//std::cout << "\tlb!=previouslbMiddle" << std::endl;
				for(int i=0; i<4; i++)
				{
					for(int j=0; j<4; j++)
					{
						clustersPerPlaneMiddle = clusterCounterMiddle[previouslbMiddle][i][j]*1.0;
						if(muPerBCID != 0)
						{
							clustersPerPlaneMiddle = clustersPerPlaneMiddle/(muPerBCID*counterForEventsMiddle);
						}
						else
						{
							clustersPerPlaneMiddle = -0.1;
						}
						fill(m_tools[m_HitmapGroups.at(m_stationnames.at(i)).at(m_pixlayers.at(j))], lb, clustersPerPlaneMiddle);
					}
				}
				previouslbMiddle = lb;
				++clusterCounterMiddle[lb][cluster.station][cluster.layer];
				counterForEventsMiddle=1;
			}
			
			// First time in lumiblock (in plane)
			else if(clusterCounterMiddle[lb][cluster.station][cluster.layer] == 0) 
			{
				++clusterCounterMiddle[lb][cluster.station][cluster.layer];				
				previouslbMiddle = lb;
			}
			
			// Lumiblock is same, so proceed
			else if(lb==previouslbMiddle)	// Same lumiblock
			{
				++clusterCounterMiddle[lb][cluster.station][cluster.layer];
			}
		}
	}
	


	// Filling of layerEff 2D histogram
/*	
	for(int i = 0; i < 4; i++)
	{
		unsigned long tempSum = numberOfHitsStationPlane[i][0] + numberOfHitsStationPlane[i][1] + numberOfHitsStationPlane[i][2] + numberOfHitsStationPlane[i][3];
		if(tempSum>0)
		{
			for(int j = 0; j < 4; j++)
			{
				layerNumber = i*4 + j;
				layerEfficiency = numberOfHitsStationPlane[i][j] / tempSum;
				layerEfficiency += i*4+j;
				//fill("AFPSiLayerTool", layerNumber, layerEfficiency);
				fill("AFPSiLayerTool", layerEfficiency);
			}
		}
	}
*/	

	return StatusCode::SUCCESS;
} // end of fillHistograms


